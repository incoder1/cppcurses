#ifdef CURSES_HAS_PRAGMA_ONCE
#pragma once
#endif

#ifndef __INSTRUSIVE_PTR_HPP_INCLUDED__
#define __INSTRUSIVE_PTR_HPP_INCLUDED__

#include <assert.h>
#include <functional>

namespace curses {

namespace detail
{

template< class Y, class T > struct sp_convertible
{
    typedef char (&yes) [1];
    typedef char (&no)  [2];

    static yes f( T* );
    static no  f( ... );

    enum _vt { value = sizeof( (f)( static_cast<Y*>(0) ) ) == sizeof(yes) };
};

template< class Y, class T > struct sp_convertible< Y, T[] >
{
    enum _vt { value = false };
};

template< class Y, class T > struct sp_convertible< Y[], T[] >
{
    enum _vt { value = sp_convertible< Y[1], T[1] >::value };
};

template< class Y, size_t N, class T > struct sp_convertible< Y[N], T[] >
{
    enum _vt { value = sp_convertible< Y[1], T[1] >::value };
};

struct sp_empty
{
};

template< bool > struct sp_enable_if_convertible_impl;

template<> struct sp_enable_if_convertible_impl<true>
{
    typedef sp_empty type;
};

template<> struct sp_enable_if_convertible_impl<false>
{
};

template< class Y, class T > struct sp_enable_if_convertible: public sp_enable_if_convertible_impl< sp_convertible< Y, T >::value >
{
};

} // namespace detail

template<class T>
class intrusive_ptr
{
private:

    typedef intrusive_ptr this_type;

public:

    typedef T element_type;

    intrusive_ptr() CURSES_NOEXCEPT :
    	px_(NULL)
    {}

    intrusive_ptr(T * p, bool add_ref = true ):
    	px_(p)
    {
        if(px_ != NULL && add_ref ) {
			intrusive_ptr_add_ref(px_);
        }
    }

	intrusive_ptr(intrusive_ptr const & rhs):
        	px_( rhs.px_ )
    {
        if( px_ != NULL ) {
			intrusive_ptr_add_ref(px_);
        }
    }

    template<class U>
    intrusive_ptr( intrusive_ptr<U> const & rhs,
		typename detail::sp_enable_if_convertible<U,T>::type = detail::sp_empty()
				) CURSES_NOEXCEPT:
    	px_(rhs.get())
    {
    	 if( px_ != NULL ) {
			intrusive_ptr_add_ref(px_);
    	 }
    }

    ~intrusive_ptr() CURSES_NOEXCEPT
    {
        if( px_ != NULL ) {
			intrusive_ptr_release(px_);
        }
    }

	intrusive_ptr(intrusive_ptr && rhs) CURSES_NOEXCEPT :
		px_(rhs.px_)
    {
        rhs.px_ = nullptr;
    }

	intrusive_ptr & operator=(intrusive_ptr && rhs) CURSES_NOEXCEPT
    {
        this_type( static_cast< intrusive_ptr && >( rhs ) ).swap(*this);
        return *this;
    }

	explicit operator bool () const CURSES_NOEXCEPT
    {
        return px_ != nullptr;
    }

	intrusive_ptr& operator=(intrusive_ptr const & rhs)
    {
        this_type(rhs).swap(*this);
        return *this;
    }

    intrusive_ptr & operator=(T * rhs)
    {
        this_type(rhs).swap(*this);
        return *this;
    }

    void reset() CURSES_NOEXCEPT
    {
        this_type().swap(*this);
    }

    void reset(T * rhs )
    {
        this_type( rhs ).swap(*this);
    }

    void reset(T * rhs, bool add_ref )
    {
        this_type(rhs,add_ref).swap(*this);
    }

    T * get() const CURSES_NOEXCEPT
    {
        return px_;
    }

    T* detach() CURSES_NOEXCEPT
    {
        T * ret = px_;
        px_ = NULL;
        return ret;
    }

    T& operator*() const
    {
        assert( px_ != 0 );
        return *px_;
    }

    T * operator->() const
    {
        assert( px_ != NULL);
        return px_;
    }

    void swap(intrusive_ptr & rhs) CURSES_NOEXCEPT
    {
        T * tmp = px_;
        px_ = rhs.px_;
        rhs.px_ = tmp;
    }

private:
    T * px_;
};

template<class T, class U>
inline bool operator==(intrusive_ptr<T> const & a, intrusive_ptr<U> const & b)
{
    return a.get() == b.get();
}

template<class T, class U>
inline bool operator!=(intrusive_ptr<T> const & a, intrusive_ptr<U> const & b)
{
    return a.get() != b.get();
}

template<class T, class U>
inline bool operator==(intrusive_ptr<T> const & a, U * b)
{
    return a.get() == b;
}

template<class T, class U>
inline bool operator!=(intrusive_ptr<T> const & a, U * b)
{
    return a.get() != b;
}

template<class T, class U>
inline bool operator==(T * a, intrusive_ptr<U> const & b)
{
    return a == b.get();
}

template<class T, class U>
inline bool operator!=(T * a, intrusive_ptr<U> const & b)
{
    return a != b.get();
}

namespace detail {
	typedef decltype(nullptr) nullptr_t;
}

template<class T>
inline bool operator==( intrusive_ptr<T> const & p, detail::nullptr_t ) CURSES_NOEXCEPT
{
    return p.get() == 0;
}

template<class T>
inline bool operator==( detail::nullptr_t, intrusive_ptr<T> const & p ) CURSES_NOEXCEPT
{
    return p.get() == 0;
}

template<class T>
inline bool operator!=( intrusive_ptr<T> const & p, detail::nullptr_t ) CURSES_NOEXCEPT
{
    return p.get() != 0;
}

template<class T>
inline bool operator!=(detail::nullptr_t, intrusive_ptr<T> const & p ) CURSES_NOEXCEPT
{
    return p.get() != 0;
}

template<class T>
inline bool operator<(intrusive_ptr<T> const & a, intrusive_ptr<T> const & b)
{
    return a.get() < b.get();
}

template<class T>
void swap(intrusive_ptr<T> & lhs, intrusive_ptr<T> & rhs)
{
    lhs.swap(rhs);
}

template<class T>
T * get_pointer(intrusive_ptr<T> const & p)
{
    return p.get();
}

template<class T, class U>
intrusive_ptr<T> static_pointer_cast(intrusive_ptr<U> const & p)
{
    return static_cast<T *>(p.get());
}

template<class T, class U>
intrusive_ptr<T> const_pointer_cast(intrusive_ptr<U> const & p)
{
    return const_cast<T *>(p.get());
}

template<class T, class U>
intrusive_ptr<T> dynamic_pointer_cast(intrusive_ptr<U> const & p)
{
    return dynamic_cast<T *>(p.get());
}

} // namesapce

namespace std {

	template< class T > std::size_t hash_value(curses::intrusive_ptr<T> const & p )
	{
		return std::hash< T* >() ( p.get() );
	}

	template<typename T>
	struct hash< curses::intrusive_ptr<T> >
	{
	public:
		CURSES_CONSTEXPR std::size_t operator()(curses::intrusive_ptr<T> const & p) const {
			return hash_value<T>(p);
		}
	};

} // namesapce std

#endif // __INSTRUSIVE_PTR_HPP_INCLUDED__
