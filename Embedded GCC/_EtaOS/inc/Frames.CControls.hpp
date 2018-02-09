/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Frames.CControls.hpp
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
		CLASS_TYPEID_INHERIT(CBorder, CContainerSingle)
		public:
			/// **********************************
			enum class EType : uint32_t {
				Simple				= 0,
				SimpleRounded		= 1,
				Thin3DOutside		= 2,
				Thin3DInside		= 3,
				Thick3DOutside		= 4,
				Thick3DInside		= 5,
				StaticOutside		= 6,
				StaticInside		= 7,
			};
			/// **********************************

		private:
			EType const d_type;
			System::Drawing::EColor const d_clr_simple;

		protected:
			CBorder(CBorder const &other) = delete;
			CBorder &operator=(CBorder const &other) = delete;
		public:
			CBorder(CContainer &container_parent, SNullable<uint8_t> separation_width, EType type, System::Drawing::EColor clr_simple);
			CBorder(CContainer &container_parent, System::Drawing::SSize const &sz_test);
			virtual ~CBorder() override;

		protected:
			virtual void OnEvent(CFrameEvent &event) override;
			virtual System::Drawing::SSize CalcFitSize() override;
			virtual void CalcChildRects(void(CFrame::*f_set_rect)(System::Drawing::SRect const &rc)) override;

		private:
		};
		/// *********************************************************************
		CLASS_TYPEID_INHERIT(CButton, CContainerSingle)
		private:
			CButton(CContainer &container_parent, System::Drawing::CImage const *image, SNullable<System::Drawing::EColor> const &clr_1, SNullable<System::Drawing::EColor> const &clr_2, char const *str_text);
		protected:
			CButton(CButton const &other) = delete;
			CButton &operator=(CButton const &other) = delete;
		public:
			CButton(CContainer &container_parent);
			CButton(CContainer &container_parent, System::Drawing::CImage const *image, SNullable<System::Drawing::EColor> const &clr_transparent, char const *str_text);
			CButton(CContainer &container_parent, System::Drawing::CImage1 const *image, SNullable<System::Drawing::EColor> const &clr_fore, SNullable<System::Drawing::EColor> const &clr_back, char const *str_text);
			virtual ~CButton() override;

		protected:
			virtual void OnEvent(CFrameEvent &event) override;
			virtual System::Drawing::SSize CalcFitSize() override;
			virtual void CalcChildRects(void(CFrame::*f_set_rect)(System::Drawing::SRect const &rc)) override;

		private:
		};
		/// *********************************************************************
		CLASS_TYPEID_INHERIT(CCheckBox, CContainerSingle)
		private:
			static System::Drawing::CImage1 const s_img1_frame_checkbox_checked;

			bool const d_is_tristate;
			SNullable<bool> d_is_checked;

		protected:
			CCheckBox(CCheckBox const &other) = delete;
			CCheckBox &operator=(CCheckBox const &other) = delete;
		public:
			CCheckBox(CContainer &container_parent, bool is_tristate, SNullable<bool> is_checked);
			CCheckBox(CContainer &container_parent, bool is_tristate, SNullable<bool> is_checked, char const *str_text);
			virtual ~CCheckBox() override;

		protected:
			virtual void OnEvent(CFrameEvent &event) override;
			virtual System::Drawing::SSize CalcFitSize() override;
			virtual void CalcChildRects(void(CFrame::*f_set_rect)(System::Drawing::SRect const &rc)) override;

		private:
		};
		/// *********************************************************************

		// *********************************************************************
		// *********************************************************************
	}
}
