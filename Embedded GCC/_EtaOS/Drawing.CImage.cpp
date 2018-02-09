/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Drawing.CImage.cpp
 ***********************************************************************/

#include "CPeriph.h"
#include "Drawing.CImage.hpp"

using namespace System::Drawing;

/// *********************************************************************
CImage::CPixelEnumerator::CPixelEnumerator(CImage const &img) : d_img(img), d_current_pixel(EColor::_Invalid), d_count(img.d_size.Width_get() * img.d_size.Height_get()) { typeid_ctor(); }
/*virtual*/ CImage::CPixelEnumerator::~CPixelEnumerator() { typeid_dtor(); }
/// **********************************
CImage::CImage(void const *imgbmp, uint32_t width, uint32_t height) : d_imgbmp(imgbmp), d_size(width, height) { typeid_ctor(); }
/*virtual*/ CImage::~CImage() { typeid_dtor(); }
/// **********************************
CImage1::CPixelEnumerator1::CPixelEnumerator1(CImage1 const &img) : CPixelEnumerator1(img, img.d_clr_fore, img.d_clr_back) {  }
CImage1::CPixelEnumerator1::CPixelEnumerator1(CImage1 const &img, EColor clr_fore, EColor clr_back) : CPixelEnumerator(img), d_imgbmp((uint8_t *)img.d_imgbmp), d_clr_fore(clr_fore), d_clr_back(clr_back), d_byte(*d_imgbmp++), d_mask(0x01) { typeid_ctor(); }
/*virtual*/ CImage1::CPixelEnumerator1::~CPixelEnumerator1() { typeid_dtor(); }

/*virtual override final*/ bool CImage1::CPixelEnumerator1::MoveToNextPixel() {
	if(d_count) {
		d_current_pixel = (d_byte & d_mask) ? d_clr_fore : d_clr_back; d_count--;
		if(d_mask == 0x80) { d_mask = 0x01; d_byte = *d_imgbmp++; } else d_mask <<= 1;
		return true;
	}
	else return false;
}
/// **********************************
CImage1::CImage1(void const *imgbmp, uint32_t width, uint32_t height) : CImage1(imgbmp, width, height, EColor::White, EColor::Black) {  }
CImage1::CImage1(void const *imgbmp, uint32_t width, uint32_t height, EColor clr_fore, EColor clr_back) : CImage(imgbmp, width, height), d_clr_fore(clr_fore), d_clr_back(clr_back) { typeid_ctor(); }
/*virtual*/ CImage1::~CImage1() { typeid_dtor(); }

/*virtual override final*/ CImage::CPixelEnumerator *CImage1::CreateEnumerator() const { return new CPixelEnumerator1(*this); }
CImage::CPixelEnumerator *CImage1::CreateEnumerator(EColor clr_fore, EColor clr_back) const { return new CPixelEnumerator1(*this, clr_fore, clr_back); }
/// **********************************
CImage8::CPixelEnumerator8::CPixelEnumerator8(CImage8 const &img) : CPixelEnumerator(img), d_imgbmp((uint8_t *)img.d_imgbmp) { typeid_ctor(); }
/*virtual*/ CImage8::CPixelEnumerator8::~CPixelEnumerator8() { typeid_dtor(); }

/*virtual override final*/ bool CImage8::CPixelEnumerator8::MoveToNextPixel() { if(d_count) { uint8_t _clr = *d_imgbmp++; d_current_pixel = (EColor)LCD_COLOR16_FROM_COLOR8(_clr); d_count--; return true; } else return false; }
/// **********************************
CImage8::CImage8(void const *imgbmp, uint32_t width, uint32_t height) : CImage(imgbmp, width, height) { typeid_ctor(); }
/*virtual*/ CImage8::~CImage8() { typeid_dtor(); }

/*virtual override final*/ CImage::CPixelEnumerator *CImage8::CreateEnumerator() const { return new CPixelEnumerator8(*this); }
/// **********************************
CImage16::CPixelEnumerator16::CPixelEnumerator16(CImage16 const &img) : CPixelEnumerator(img), d_imgbmp((uint16_t *)img.d_imgbmp) { typeid_ctor(); }
/*virtual*/ CImage16::CPixelEnumerator16::~CPixelEnumerator16() { typeid_dtor(); }

/*virtual override final*/ bool CImage16::CPixelEnumerator16::MoveToNextPixel() { if(d_count) { d_current_pixel = (EColor)*d_imgbmp++; d_count--; return true; } else return false; }
/// **********************************
CImage16::CImage16(void const *imgbmp, uint32_t width, uint32_t height) : CImage(imgbmp, width, height) { typeid_ctor(); }
/*virtual*/ CImage16::~CImage16() { typeid_dtor(); }

/*virtual override final*/ CImage::CPixelEnumerator *CImage16::CreateEnumerator() const { return new CPixelEnumerator16(*this); }
/// *********************************************************************
