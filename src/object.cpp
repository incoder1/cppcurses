#include "stdafx.hpp"
#include "object.hpp"

namespace curses {

object::object() CURSES_NOEXCEPT:
	refCount_(0)
{}

object::~object() CURSES_NOEXCEPT
{}

#ifdef CURSES_HAS_ATOMIC
void intrusive_ptr_add_ref(object* const obj)
{
	obj->refCount_.fetch_add(1, memory_order::memory_order_relaxed);
}

void intrusive_ptr_release(object* const obj)
{
   if (obj->refCount_.fetch_sub(1, memory_order::memory_order_release) == 1) {
      atomic_thread_fence(memory_order::memory_order_acquire);
      delete obj;
   }
}
#else
void intrusive_ptr_add_ref(object* const obj)
{
	++obj->refCount_;
}

void intrusive_ptr_release(object* const obj)
{
   if (--obj == 1) {
      delete obj;
   }
}
#endif // CURSES_HAS_ATOMIC

template<class M>
class lock_guard: private noncopyable
{
public:
	explicit lock_guard(M& m):
		m_(m)
	{
		m_.lock();
	}

	~lock_guard() {
		m_.unlock();
	}

private:
	M & m_;
};

#if defined(CURSES_WIN) && !defined(CURSES_JEMALLOC)

namespace win {

// A win32 critical section based implementation of fast user space mutex
class futex CURSES_FINAL:private curses::noncopyable {
public:
	typedef lock_guard<futex> scoped_lock;
	explicit futex() CURSES_NOEXCEPT
	{
		::InitializeCriticalSection(&cs_);
	}
	~futex() CURSES_NOEXCEPT
	{
		::DeleteCriticalSection(&cs_);
	}
	CURSES_FORCEINLINE void lock() {
		::EnterCriticalSection(&cs_);
	}
	CURSES_FORCEINLINE void unlock() {
		::LeaveCriticalSection(&cs_);
	}
private:
	::CRITICAL_SECTION cs_;
};

class heap_allocator:private curses::noncopyable
{
private:
	heap_allocator():
		hHeap_(NULL)
	{
		hHeap_ = ::HeapCreate(0,0,0);
	}
	static void release() CURSES_NOEXCEPT {
		delete _instance.load(memory_order::memory_order_consume);
		_instance.store(NULL, memory_order::memory_order_release);
	}
public:
	static heap_allocator* const instance() {
		heap_allocator* tmp = _instance.load(memory_order::memory_order_consume);
		if (!tmp) {
			futex::scoped_lock lock(_futex);
			if (!(tmp = _instance.load(memory_order::memory_order_consume))) {
				tmp = new heap_allocator();
				_instance.store(tmp, memory_order::memory_order_release);
				std::atexit(&heap_allocator::release);
			}
		}
		return tmp;
	}
	~heap_allocator() CURSES_NOEXCEPT
	{
		::HeapDestroy(hHeap_);
	}
	inline void* allocate(std::size_t bytes) {
		return ::HeapAlloc(hHeap_, 0, bytes);
	}
	inline void release(void * const block) {
		::HeapFree(hHeap_, 0, block);
	}
private:
	::HANDLE hHeap_;
	static futex _futex;
	static atomic<heap_allocator*> _instance;
};

futex heap_allocator::_futex;
atomic<heap_allocator*> heap_allocator::_instance(NULL);

} // namesapce win

void* object::operator new(std::size_t s) CURSES_THROW(std::bad_alloc)
{
	return win::heap_allocator::instance()->allocate(s);
}

void object::operator delete(void* const p) CURSES_NOEXCEPT
{
	return win::heap_allocator::instance()->release(p);
}

#endif // CURSES_WIN

#ifdef CURSES_JEMALLOC

void* object::operator new(std::size_t s) CURSES_THROW(std::bad_alloc)
{
	return ::je_malloc(s);
}

void object::operator delete(void* const p) CURSES_NOEXCEPT
{
	return ::je_free(p);
}

#endif // CURSES_JEMALLOC

} // namespace curses
