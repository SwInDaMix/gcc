/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Drawing.CPoint.cpp
***********************************************************************/

#include "Drawing.SPoint.hpp"

using namespace System::Drawing;

/// *********************************************************************
SPoint::SPoint() : d_x(0), d_y(0) {  }
SPoint::SPoint(uint32_t x, uint32_t y) : d_x(x), d_y(y) {  }

SPoint SPoint::operator +(SSize const &size) const { return SPoint(d_x + size.d_width, d_y + size.d_height); }
SPoint const &SPoint::operator +=(SSize const &size) { d_x += size.d_width; d_y += size.d_height; return *this; }
SPoint SPoint::operator -(SSize const &size) const { return SPoint(d_x - size.d_width, d_y - size.d_height); }
SPoint const &SPoint::operator -=(SSize const &size) { d_x -= size.d_width; d_y -= size.d_height; return *this; }
SPoint SPoint::operator +(SSizeSigned const &size) const { return SPoint(d_x + size.d_width, d_y + size.d_height); }
SPoint const &SPoint::operator +=(SSizeSigned const &size) { d_x += size.d_width; d_y += size.d_height; return *this; }
SPoint SPoint::operator -(SSizeSigned const &size) const { return SPoint(d_x - size.d_width, d_y - size.d_height); }
SPoint const &SPoint::operator -=(SSizeSigned const &size) { d_x -= size.d_width; d_y -= size.d_height; return *this; }
SSizeSigned SPoint::operator -(SPoint const &point) const { return SSize(d_x - point.d_x, d_y - point.d_y); }

bool SPoint::operator ==(SPoint const &other) const { return d_x == other.d_x && d_y == other.d_y; }
bool SPoint::operator !=(SPoint const &other) const { return d_x != other.d_x || d_y != other.d_y; }
/// *********************************************************************
