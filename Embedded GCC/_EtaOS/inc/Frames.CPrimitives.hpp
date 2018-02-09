/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Frames.CPrimitives.hpp
 ***********************************************************************/

#pragma once

#include "Types.h"
#include "Common.hpp"
#include "Frames.Config.hpp"
#include "Frames.CFrame.hpp"
#include "Frames.CFrameEvent.hpp"
#include "Drawing.EColor.hpp"
#include "Drawing.CFont.hpp"
#include "Drawing.CGraphics.hpp"
#include "Drawing.SEdge.hpp"
#include "Drawing.SPoint.hpp"
#include "Drawing.SRect.hpp"
#include "Drawing.SSize.hpp"

namespace System {
	namespace Frames {
		/// *********************************************************************
		CLASS_TYPEID_INHERIT(CPrimitiveString, CFrame)
		private:
			EAlignment d_alignment_horizontal;
			EAlignment d_alignment_vertical;
			char const *d_str_string;

		protected:
			CPrimitiveString(CPrimitiveString const &other) = delete;
			CPrimitiveString &operator=(CPrimitiveString const &other) = delete;
		public:
			CPrimitiveString(CContainer &container_parent, EAlignment alignment_horizontal, EAlignment alignment_vertical, char const *str_string);
			virtual ~CPrimitiveString() override;

		protected:
			virtual void OnEvent(CFrameEvent &event) override;
			virtual System::Drawing::SSize CalcFitSize() override;
		};
		/// *********************************************************************
		CLASS_TYPEID_INHERIT(CPrimitiveImage, CFrame)
		public:
			/// **********************************
			enum class EType : uint32_t {
				Color					= 0,
				ColorWithTransparent	= 1,
				Monochrome				= 2,
			};
			/// **********************************

		private:
			EAlignment d_alignment_horizontal;
			EAlignment d_alignment_vertical;
			EType const d_type;
			System::Drawing::CImage const &d_image;
			System::Drawing::EColor const d_clr_transparent;

		protected:
			CPrimitiveImage(CPrimitiveImage const &other) = delete;
			CPrimitiveImage &operator=(CPrimitiveImage const &other) = delete;
		public:
			CPrimitiveImage(CContainer &container_parent, EAlignment alignment_horizontal, EAlignment alignment_vertical, System::Drawing::CImage const &image, SNullable<System::Drawing::EColor> const &clr_transparent);
			CPrimitiveImage(CContainer &container_parent, EAlignment alignment_horizontal, EAlignment alignment_vertical, System::Drawing::CImage1 const &image1, SNullable<System::Drawing::EColor> const &clr_fore, SNullable<System::Drawing::EColor> const &clr_back);
			virtual ~CPrimitiveImage() override;

		protected:
			virtual void OnEvent(CFrameEvent &event) override;
			virtual System::Drawing::SSize CalcFitSize() override;
		};
		/// *********************************************************************
		CLASS_TYPEID_INHERIT(CPrimitiveSeparator, CFrame)
		private:
			EOrientation const d_orientation;
			bool const d_is_inside;

		protected:
			CPrimitiveSeparator(CPrimitiveSeparator const &other) = delete;
			CPrimitiveSeparator &operator=(CPrimitiveSeparator const &other) = delete;
		public:
			CPrimitiveSeparator(CContainer &container_parent, EOrientation orientation, bool is_inside);
			virtual ~CPrimitiveSeparator() override;

		protected:
			virtual void OnEvent(CFrameEvent &event) override;
			virtual System::Drawing::SSize CalcFitSize() override;
		};
		/// *********************************************************************
		CLASS_TYPEID_INHERIT(CPrimitiveSpace, CFrame)
		private:
			System::Drawing::SSize const d_sz_test;

		protected:
			CPrimitiveSpace(CPrimitiveSpace const &other) = delete;
			CPrimitiveSpace &operator=(CPrimitiveSpace const &other) = delete;
		public:
			CPrimitiveSpace(CContainer &container_parent, System::Drawing::SSize const &sz_test);
			virtual ~CPrimitiveSpace() override;

		protected:
			virtual void OnEvent(CFrameEvent &event) override;
			virtual System::Drawing::SSize CalcFitSize() override;
		};
		/// *********************************************************************

		// *********************************************************************
		// *********************************************************************
	}
}
