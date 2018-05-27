/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Drawing.CRect.cpp
***********************************************************************/

#include "Drawing.SEdge.hpp"
#include "Drawing.SRect.hpp"

using namespace System::Drawing;

/// *********************************************************************
SRect::SRect() : d_location(), d_size() {  }
SRect::SRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height) : d_location(x, y), d_size(width, height) {  }
SRect::SRect(SPoint const &location, SSize const &size) : d_location(location), d_size(size) {  }

uint32_t SRect::CenterX_get() const { return d_location.d_x + (d_size.d_width >> 1); }
uint32_t SRect::CenterY_get() const { return d_location.d_y + (d_size.d_height >> 1); }
SPoint SRect::Center_get() const { return SPoint(CenterX_get(), CenterY_get()); }
bool SRect::IsInside(uint32_t x, uint32_t y) const { return x >= d_location.d_x && x < d_location.d_x + d_size.d_width && y >= d_location.d_y && y < d_location.d_y + d_size.d_height; }
bool SRect::IsInside(SPoint const &pt) const { return IsInside(pt.d_x, pt.d_y); }
bool SRect::IsInside(SRect const &rc) const { return rc.d_location.d_x >= d_location.d_x && rc.d_location.d_x + rc.d_size.d_width <= d_location.d_x + d_size.d_width && rc.d_location.d_y >= d_location.d_y && rc.d_location.d_y + rc.d_size.d_height <= d_location.d_y + d_size.d_height; }
bool SRect::IsIntersectsWith(SRect const &rc) const { return min(d_location.d_x + d_size.d_width, rc.d_location.d_x + rc.d_size.d_width) > max(d_location.d_x, rc.d_location.d_x) && min(d_location.d_y + d_size.d_height, rc.d_location.d_y + rc.d_size.d_height) > max(d_location.d_y, rc.d_location.d_y); }

SRect SRect::operator +(SSize const &size) const { return SRect(d_location, d_size + size); }
SRect const &SRect::operator +=(SSize const &size) { d_size += size; return *this; }
SRect SRect::operator -(SSize const &size) const { return SRect(d_location, d_size - size); }
SRect const &SRect::operator -=(SSize const &size) { d_size -= size; return *this; }
SRect SRect::operator +(SSizeSigned const &size) const { return SRect(d_location, d_size + size); }
SRect const &SRect::operator +=(SSizeSigned const &size) { d_size += size; return *this; }
SRect SRect::operator -(SSizeSigned const &size) const { return SRect(d_location, d_size - size); }
SRect const &SRect::operator -=(SSizeSigned const &size) { d_size -= size; return *this; }
SRect SRect::operator |(SRect const &rc) const { return SRect(min(d_location.d_x, rc.d_location.d_x), min(d_location.d_y, rc.d_location.d_y), max(Right_get(), rc.Right_get()) - d_location.d_x, max(Bottom_get(), rc.Bottom_get()) - d_location.d_y); }
SRect const &SRect::operator |=(SRect const &rc) {
	uint32_t _x = min(d_location.d_x, rc.d_location.d_x), _y = min(d_location.d_y, rc.d_location.d_y);
	d_size.d_width = max(Right_get(), rc.Right_get()) - d_location.d_x; d_size.d_height = max(Bottom_get(), rc.Bottom_get()) - d_location.d_y;
	d_location.d_x = _x; d_location.d_y = _y;
	return *this;
}
SRect SRect::operator &(SRect const &rc) const {
	uint32_t _x = max(d_location.d_x, rc.d_location.d_x), _y = max(d_location.d_y, rc.d_location.d_y), _right = min(Right_get(), rc.Right_get()), _bottom = min(Bottom_get(), rc.Bottom_get());
	if(_right >= _x && _bottom >= _y) return SRect(_x, _y, _right - _x, _bottom - _y);
	else return SRect();
}
SRect const &SRect::operator &=(SRect const &rc) {
	uint32_t _x = max(d_location.d_x, rc.d_location.d_x), _y = max(d_location.d_y, rc.d_location.d_y), _right = min(Right_get(), rc.Right_get()), _bottom = min(Bottom_get(), rc.Bottom_get());
	if(_right >= _x && _bottom >= _y) { d_location.d_x = _x; d_location.d_y = _y; d_size.d_width = _right - _x; d_size.d_height = _bottom - _y; }
	else d_location.d_x = d_location.d_y = d_size.d_width = d_size.d_height = 0;
	return *this;
}

SRect SRect::operator +(SEdge const &edg) const {
	uint32_t _left = edg.d_left, _top = edg.d_top;
	if(_left > d_location.d_x) _left = d_location.d_x;
	if(_top > d_location.d_y) _left = d_location.d_y;
	return SRect(d_location.d_x - _left, d_location.d_y - _top, d_size.d_width + (_left + edg.d_right), d_size.d_height + (_top + edg.d_bottom));
}
SRect const &SRect::operator +=(SEdge const &edg) {
	uint32_t _left = edg.d_left, _top = edg.d_top;
	if(_left > d_location.d_x) _left = d_location.d_x;
	if(_top > d_location.d_y) _left = d_location.d_y;
	d_location.d_x -= _left; d_location.d_y -= _top; d_size.d_width += (_left + edg.d_right); d_size.d_height += (_top + edg.d_bottom);
	return *this;
}
SRect SRect::operator -(SEdge const &edg) const {
	uint32_t _left = edg.d_left, _top = edg.d_top, _dw = _left + edg.d_right, _dh = _top + edg.d_bottom;
	if(_dw > d_size.d_width) _dw = d_size.d_width;
	if(_dh > d_size.d_height) _dh = d_size.d_height;
	return SRect(d_location.d_x + _left, d_location.d_y + _top, d_size.d_width - _dw, d_size.d_height - _dh);
}
SRect const &SRect::operator -=(SEdge const &edg) {
	uint32_t _left = edg.d_left, _top = edg.d_top, _dw = _left + edg.d_right, _dh = _top + edg.d_bottom;
	if(_dw > d_size.d_width) _dw = d_size.d_width;
	if(_dh > d_size.d_height) _dh = d_size.d_height;
	d_location.d_x += _left; d_location.d_y += _top; d_size.d_width -= _dw; d_size.d_height -= _dh;
	return *this;
}

bool SRect::operator ==(SRect const &other) const { return d_location == other.d_location && d_size == other.d_size; }
bool SRect::operator !=(SRect const &other) const { return d_location != other.d_location || d_size != other.d_size; }

SRect::operator SEdge() { return SEdge(*this); }
/// *********************************************************************
