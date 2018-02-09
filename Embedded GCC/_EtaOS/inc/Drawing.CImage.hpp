/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Drawing.CImage.hpp
***********************************************************************/

#pragma once

#include "Types.h"
#include "Drawing.EColor.hpp"
#include "Drawing.SSize.hpp"

namespace System {
	namespace Drawing {
		/// *********************************************************************
		CLASS_TYPEID(CImage)
			friend class CGraphics;
			friend class CImage16;
			friend class CImage8;
			friend class CImage1;
			friend class CPixelEnumerator16;
			friend class CPixelEnumerator8;
			friend class CPixelEnumerator1;

		public:
			/// **********************************
			CLASS_TYPEID(CPixelEnumerator)
				friend class CGraphics;
				friend class CImage;
				friend class CImage16;
				friend class CImage8;
				friend class CImage1;
				friend class CPixelEnumerator16;
				friend class CPixelEnumerator8;
				friend class CPixelEnumerator1;

			private:
				CImage const &d_img;
				EColor d_current_pixel;
				uint32_t d_count;

			protected:
				CPixelEnumerator(CImage const &img);
			public:
				CPixelEnumerator(CPixelEnumerator const &other) = delete;
				CPixelEnumerator &operator=(CPixelEnumerator const &other) = delete;
				virtual ~CPixelEnumerator() override;

				EColor CurrentPixel_get();
				virtual bool MoveToNextPixel() = 0;
			};
			/// **********************************

		private:
			void const *const d_imgbmp;
			SSize const d_size;

		protected:
			CImage(void const *imgbmp, uint32_t width, uint32_t height);
		public:
			CImage(CImage const &other) = delete;
			CImage &operator=(CImage const &other) = delete;
			virtual ~CImage() override;

			void const *ImgBmp_get() const;
			SSize const &Size_get() const;
			uint32_t Width_get() const;
			uint32_t Height_get() const;

			virtual CPixelEnumerator *CreateEnumerator() const = 0;
		};
		/// **********************************
		CLASS_TYPEID_INHERIT(CImage1, CImage)
		public:
			/// **********************************
			CLASS_TYPEID_INHERIT(CPixelEnumerator1, CPixelEnumerator)
				friend class CImage1;

			private:
				uint8_t const *d_imgbmp;
				EColor const d_clr_fore;
				EColor const d_clr_back;
				uint8_t d_byte;
				uint8_t d_mask;

			protected:
				CPixelEnumerator1(CImage1 const &img);
				CPixelEnumerator1(CImage1 const &img, EColor clr_fore, EColor clr_back);
			public:
				virtual ~CPixelEnumerator1() override;

				virtual bool MoveToNextPixel() override final;
			};
			/// **********************************

		private:
			EColor const d_clr_fore;
			EColor const d_clr_back;

		public:
			CImage1(void const *imgbmp, uint32_t width, uint32_t height);
			CImage1(void const *imgbmp, uint32_t width, uint32_t height, EColor clr_fore, EColor clr_back);
			virtual ~CImage1() override;

			virtual CPixelEnumerator *CreateEnumerator() const override final;
			CPixelEnumerator *CreateEnumerator(EColor clr_fore, EColor clr_back) const;
		};
		/// **********************************
		CLASS_TYPEID_INHERIT(CImage8, CImage)
		public:
			/// **********************************
			CLASS_TYPEID_INHERIT(CPixelEnumerator8, CPixelEnumerator)
				friend class CImage8;

			private:
				uint8_t const *d_imgbmp;

			protected:
				CPixelEnumerator8(CImage8 const &img);
			public:
				virtual ~CPixelEnumerator8() override;

				virtual bool MoveToNextPixel() override final;
			};
			/// **********************************

		public:
			CImage8(void const *imgbmp, uint32_t width, uint32_t height);
			virtual ~CImage8() override;

			virtual CPixelEnumerator *CreateEnumerator() const override final;
		};
		/// **********************************
		CLASS_TYPEID_INHERIT(CImage16, CImage)
		public:
			/// **********************************
			CLASS_TYPEID_INHERIT(CPixelEnumerator16, CPixelEnumerator)
				friend class CImage16;

			private:
				uint16_t const *d_imgbmp;

			protected:
				CPixelEnumerator16(CImage16 const &img);
			public:
				virtual ~CPixelEnumerator16() override;

				virtual bool MoveToNextPixel() override final;
			};
			/// **********************************

		public:
			CImage16(void const *imgbmp, uint32_t width, uint32_t height);
			virtual ~CImage16() override;

			virtual CPixelEnumerator *CreateEnumerator() const override final;
		};
		/// *********************************************************************

		// *********************************************************************
		inline EColor CImage::CPixelEnumerator::CurrentPixel_get() { return d_current_pixel; }
		// **********************************
		inline void const *CImage::ImgBmp_get() const { return d_imgbmp; }
		inline SSize const &CImage::Size_get() const { return d_size; }
		inline uint32_t CImage::Width_get() const { return d_size.Width_get(); }
		inline uint32_t CImage::Height_get() const { return d_size.Height_get(); }
		// *********************************************************************
	}
}
