#include "stdafx.hpp"
#include "pen.hpp"

namespace curses {

// pen
pen::pen(const terminal* trm, const rectangle& window):
	term_(trm),
	region_(window),
	relative_()
{
	relative_ = region_.get_visible_box(window);
}

pen::~pen() CURSES_NOEXCEPT
{
}

void pen::outch(uint8_t x, uint8_t y, char_t ch) const
{
	if(region_.is_visible()) {
		position pos = region_.current_to_parent(x,y);
		if(region_.is_pos_visible(pos)) {
			term_->putch(pos,ch);
		}
	}
}

void pen::out_line(uint8_t x, uint8_t y, uint8_t l, char_t ch) const
{
	if(region_.is_visible()) {
		position pos = region_.current_to_parent(x,y);
		if(region_.is_pos_visible(pos)) {
			uint8_t vleng = region_.visible_line_len(pos,l);
			if(vleng > 0) {
				term_->put_line(pos,ch,vleng);
			}
		}
	}
}

void pen::out_box(uint8_t x, uint8_t y, uint8_t w, uint8_t h, char_t ch) const
{
	if(region_.is_visible()) {
		position pos = region_.current_to_parent(x,y);
		if(region_.is_pos_visible(pos)) {
			rectangle displaybox = region_.get_visible_box(x,y,w,h);
			term_->fill_rectangle(displaybox,ch);
		}
	}
}

uint8_t pen::out_text(uint8_t x, uint8_t y, const char_t* str) const
{
	uint8_t result = 0;
	if(region_.is_visible()) {
		position pos = region_.current_to_parent(x,y);
		if(region_.is_pos_visible(pos)) {
			uint8_t len = region_.visible_text_len(pos,str);
			char_t *s = const_cast<char_t*>(str);
			for(int i = 0; i < len; i++) {
				outch(x+i,y,*s);
				++s;
				++result;
			}
		}
	}
	return result;
}

texel* pen::clipt_rect(uint8_t x, uint8_t y, uint8_t &w, uint8_t &h) const
{
	texel *result = NULL;
	if( region_.is_visible() ) {
		rectangle vrc = region_.get_visible_box(x,y,w,h);
		bounds bds = vrc.get_bounds();
		if(bds.size()) {
			result = static_cast<texel*>( std::malloc(bds.size() * sizeof(texel) ) );
			term_->clipt_rect(vrc,result);
			w = bds.width;
			h = bds.height;
		}
	}
	return result;
}

void pen::paste_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, texel* const box) const
{
	if( region_.is_visible() && NULL != box ) {
		rectangle vrc = region_.get_visible_box( x, y, w, h);
		bounds bds = vrc.get_bounds();
		if( bds.size() ) {
			term_->paste_rect( vrc, box);
		}
		std::free(box);
	}
}

void pen::set_color(const text_color& cl) const
{
	term_->set_color(cl);
}

sp_pen pen::sub_pen(const rectangle& rect) const
{
	return sp_pen( new pen(term_, region_.get_visible_box(rect) ) );
}

} // namesapce curses

