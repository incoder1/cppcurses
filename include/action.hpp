#ifndef __CURSES_ACTION_HPP_INCLUDED__
#define __CURSES_ACTION_HPP_INCLUDED__

#include "sobject.hpp"
#include "events.hpp"

namespace curses {

typedef CURSES_FUNCTOR< void(const event&) > event_handler;

template<typename T, class W>
event_handler bind_handler(T what, const intrusive_ptr<W>& w) {
	return CURSES_BIND( what , w.get(), curses::_1);
}

class CURSES_PUBLIC action:public object
{
	public:
		action(event_handler&& handler) CURSES_NOEXCEPT;
		~action() CURSES_NOEXCEPT;
		inline void emit(const event& e) const {
			handler_( e );
		}
		friend class signal;
	private:
		const event_handler handler_;
};

CURSES_DECLARE_SPTR(action);

typedef CURSES_UNORDERED_SET(event_type) event_type_set;

class CURSES_PUBLIC signal CURSES_FINAL {
public:
	explicit signal(const sp_action& action) CURSES_NOEXCEPT;

	void emit(const event& e) const;

	inline bool connect(const event_type& type) {
		return events_.insert(type).second;
	}

	inline bool can_handle_event(const event& e) {
		return events_.find( e.type ) != events_.end();
	}
private:
	event_type_set events_;
	sp_action action_;
};

} // namespace curses

#endif // __CURSES_ACTION_HPP_INCLUDED__
