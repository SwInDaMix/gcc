/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Drawing.CSize.cpp
 ***********************************************************************/

#include "Drawing.SSize.hpp"
#include "Drawing.SSizeSigned.hpp"

using namespace System::Drawing;

/// *********************************************************************
SSize::SSize() : d_width(0), d_height(0) {  }
SSize::SSize(uint32_t width, uint32_t height) : d_width(width), d_height(height) {  }
SSize::SSize(SSizeSigned const &size) : d_width(size.d_width), d_height(size.d_height) {  }

SSize SSize::operator +(SSize const &size) const { return SSize(d_width + size.d_width, d_height + size.d_height); }
SSize const &SSize::operator +=(SSize const &size) { d_width += size.d_width; d_height += size.d_height; return *this; }
SSize SSize::operator -(SSize const &size) const { return SSize(d_width - size.d_width, d_height - size.d_height); }
SSize const &SSize::operator -=(SSize const &size) { d_width -= size.d_width; d_height -= size.d_height; return *this; }
SSize SSize::operator |(SSize const &size) const { return SSize(max(d_width, size.d_width), max(d_height, size.d_height)); }
SSize const &SSize::operator |=(SSize const &size) { maxwith(d_width, size.d_width); maxwith(d_height, size.d_height); return *this; }
SSize SSize::operator &(SSize const &size) const { return SSize(min(d_width, size.d_width), min(d_height, size.d_height)); }
SSize const &SSize::operator &=(SSize const &size) { minwith(d_width, size.d_width); minwith(d_height, size.d_height); return *this; }
SSize SSize::operator +(SSizeSigned const &size) const { return SSize(d_width + size.d_width, d_height + size.d_height); }
SSize const &SSize::operator +=(SSizeSigned const &size) { d_width += size.d_width; d_height += size.d_height; return *this; }
SSize SSize::operator -(SSizeSigned const &size) const { return SSize(d_width - size.d_width, d_height - size.d_height); }
SSize const &SSize::operator -=(SSizeSigned const &size) { d_width -= size.d_width; d_height -= size.d_height; return *this; }

bool SSize::operator ==(SSize const &other) const { return d_width == other.d_width && d_height == other.d_height; }
bool SSize::operator !=(SSize const &other) const { return d_width != other.d_width || d_height != other.d_height; }
/// *********************************************************************
