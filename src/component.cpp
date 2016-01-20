#include "stdafx.hpp"
#include "component.hpp"

namespace curses {

component::component(sp_view&& view) CURSES_NOEXCEPT:
	textjet( forward<sp_view>(view) )
{}

component::~component() CURSES_NOEXCEPT
{}

void component::add_child(const sp_component& child)
{
	assert(!child->parent_);
	bool result = childs_.insert(child).second;
	assert(result);
	child->parent_ = this;
}

void component::reparent(component* const new_parent)
{
	if(NULL != new_parent) {
		parent_->childs_.erase(const_cast<component*>(this));
		parent_ = NULL;
		new_parent->add_child(const_cast<component*>(this));
	}
}

void component::reparent(const sp_component& new_parent)
{
	reparent(new_parent.get());
}

} // namespace curses
