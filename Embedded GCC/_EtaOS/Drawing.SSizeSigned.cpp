/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Drawing.CSizeSigned.cpp
 ***********************************************************************/

#include "Drawing.SSize.hpp"
#include "Drawing.SSizeSigned.hpp"

using namespace System::Drawing;

/// *********************************************************************
SSizeSigned::SSizeSigned() : d_width(0), d_height(0) {  }
SSizeSigned::SSizeSigned(int32_t width, int32_t height) : d_width(width), d_height(height) {  }
SSizeSigned::SSizeSigned(SSize const &size) : d_width(size.d_width), d_height(size.d_height) {  }

SSizeSigned SSizeSigned::operator +(SSizeSigned const &size) const { return SSize(d_width + size.d_width, d_height + size.d_height); }
SSizeSigned const &SSizeSigned::operator +=(SSizeSigned const &size) { d_width += size.d_width; d_height += size.d_height; return *this; }
SSizeSigned SSizeSigned::operator -(SSizeSigned const &size) const { return SSize(d_width - size.d_width, d_height - size.d_height); }
SSizeSigned const &SSizeSigned::operator -=(SSizeSigned const &size) { d_width -= size.d_width; d_height -= size.d_height; return *this; }
SSizeSigned SSizeSigned::operator +(SSize const &size) const { return SSize(d_width + size.d_width, d_height + size.d_height); }
SSizeSigned const &SSizeSigned::operator +=(SSize const &size) { d_width += size.d_width; d_height += size.d_height; return *this; }
SSizeSigned SSizeSigned::operator -(SSize const &size) const { return SSize(d_width - size.d_width, d_height - size.d_height); }
SSizeSigned const &SSizeSigned::operator -=(SSize const &size) { d_width -= size.d_width; d_height -= size.d_height; return *this; }

bool SSizeSigned::operator ==(SSizeSigned const &other) const { return d_width == other.d_width && d_height == other.d_height; }
bool SSizeSigned::operator !=(SSizeSigned const &other) const { return d_width != other.d_width || d_height != other.d_height; }
/// *********************************************************************
