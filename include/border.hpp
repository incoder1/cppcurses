#ifndef __CURSES_BORDER_HPP_INCLUDED__
#define __CURSES_BORDER_HPP_INCLUDED__

#include "sobject.hpp"
#include "pen.hpp"

namespace curses {

class CURSES_PUBLIC border:public virtual object
{
public:
	virtual void out(const pen* p,const bounds& bds) = 0;
	virtual ~border() CURSES_NOEXCEPT;
	inline text_color color() {
		return color_;
	}
protected:
	border(const text_color& color);
private:
	text_color color_;
};

CURSES_DECLARE_SPTR(border);

class CURSES_PUBLIC single_line_border:public border
{
public:
	single_line_border(const text_color& c);
	virtual void out(const pen* p, const bounds& bds);
};

class CURSES_PUBLIC double_line_border:public border
{
public:
	double_line_border(const text_color& c);
	virtual void out(const pen* p, const bounds& bds);
};

namespace {

template<class B>
class basic_titled_boder: public B {
  public:
	basic_titled_boder(const text_color& c, const char_t* title):
		B( c ),
		title_(title)
	{}
	virtual void out(const pen* p, const bounds& bds) {
		B::out( p, bds);
		sp_pen tp = p->sub_pen(1,0, bds.width -1 , 1);
		tp->out_text(0, 0, title_);
	}
  private:
	const char_t* title_;
};

} // anonymus namespace

typedef basic_titled_boder<single_line_border> single_line_titled_border;
typedef basic_titled_boder<double_line_border> double_line_titled_border;

} // namespace curses

#endif // __CURSES_BORDER_HPP_INCLUDED__
