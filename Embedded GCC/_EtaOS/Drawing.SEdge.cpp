/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Drawing.CEdge.cpp
***********************************************************************/

#include "Drawing.SEdge.hpp"
#include "Drawing.SRect.hpp"

using namespace System::Drawing;

/// *********************************************************************
SEdge::SEdge() : SEdge(0, 0, 0, 0) {  }
SEdge::SEdge(uint32_t edge) : d_left(edge), d_top(edge), d_right(edge), d_bottom(edge) {  }
SEdge::SEdge(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom) : d_left(left), d_top(top), d_right(right), d_bottom(bottom) {  }
SEdge::SEdge(SPoint const &location, SSize const &size) : SEdge(location.d_x, location.d_y, location.d_x + size.d_width, location.d_y + size.d_height) {  }
SEdge::SEdge(SRect const &rc) : SEdge(rc.d_location, rc.d_size) {  }

bool SEdge::operator==(SEdge const &other) const { return d_left == other.d_left && d_top == other.d_top && d_right == other.d_right && d_bottom == other.d_bottom; }
bool SEdge::operator!=(SEdge const &other) const { return d_left != other.d_left || d_top != other.d_top || d_right != other.d_right || d_bottom != other.d_bottom; }

SEdge::operator SRect() { return SRect(*this); }
/// *********************************************************************
