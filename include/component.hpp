#ifndef __CURSES_COMPONENT_HPP_INCLUDED__
#define __CURSES_COMPONENT_HPP_INCLUDED__

#include "textjet.hpp"

namespace curses {

class component;
CURSES_DECLARE_SPTR(component);

class CURSES_PUBLIC component:public textjet
{
	protected:
		component(sp_view&& view) CURSES_NOEXCEPT;
	public:
		virtual ~component() CURSES_NOEXCEPT;
		void add_child(const sp_component& child);
		void reparent(const sp_component& new_parent);
		const sp_component parent() const {
			return parent_;
		}
	private:
		typedef CURSES_UNORDERED_SET(sp_component) components_set;
		void reparent(component* const new_parent);
	private:
		sp_component parent_;
		components_set childs_;
};

} // namesapce curses

#endif // __CURSES_COMPONENT_HPP_INCLUDED__
