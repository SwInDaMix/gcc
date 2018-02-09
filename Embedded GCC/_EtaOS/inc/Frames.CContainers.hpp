/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Frames.CContainers.hpp
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
		CLASS_TYPEID_INHERIT(CStack, CContainerMulti)
		public:
			/// **********************************
			enum class EAlignmentStack : uint32_t {
				Start		= 0,
				Middle		= 1,
				End			= 2,
				Spread		= 3,
				FillFirst	= 4,
				FillLast	= 5,
			};
			/// **********************************

		private:
			EOrientation d_orientation;
			EAlignmentEx d_alignment_array;
			EAlignmentStack d_alignment_stack;
			bool d_is_same_length;

		protected:
			CStack(CStack const &other) = delete;
			CStack &operator=(CStack const &other) = delete;
		public:
			CStack(CContainer &container_parent, SNullable<uint8_t> separation_width, EOrientation orientation, EAlignmentEx alignment_array, EAlignmentStack alignment_stack, bool is_same_length);
			virtual ~CStack() override;

		protected:
			virtual void OnEvent(CFrameEvent &event) override;
			virtual System::Drawing::SSize CalcFitSize() override;
			virtual void CalcChildRects(void(CFrame::*f_set_rect)(System::Drawing::SRect const &rc)) override;

		private:
			uint32_t _CalcFrameAlignment(uint32_t *width_max, uint32_t *height_max, uint32_t *count);
		};
		/// *********************************************************************
		CLASS_TYPEID_INHERIT(CGrid, CContainerMulti)
		private:
			int32_t const *const d_column_definitions;
			int32_t const *const d_row_definitions;

		protected:
			CGrid(CGrid const &other) = delete;
			CGrid &operator=(CGrid const &other) = delete;
		public:
			CGrid(CContainer &container_parent, int32_t const *column_definitions, int32_t const *row_definitions);
			virtual ~CGrid() override;

		protected:
			virtual void OnEvent(CFrameEvent &event) override;
			virtual System::Drawing::SSize CalcFitSize() override;
			virtual void CalcChildRects(void(CFrame::*f_set_rect)(System::Drawing::SRect const &rc)) override;

		private:
		};
		/// *********************************************************************
		CLASS_TYPEID_INHERIT(CTabMulti, CContainerMulti)
		private:

		protected:
			CTabMulti(CTabMulti const &other) = delete;
			CTabMulti &operator=(CTabMulti const &other) = delete;
		public:
			CTabMulti(CContainer &container_parent);
			virtual ~CTabMulti() override;

		protected:
			virtual void OnEvent(CFrameEvent &event) override;
			virtual System::Drawing::SSize CalcFitSize() override;
			virtual void CalcChildRects(void(CFrame::*f_set_rect)(System::Drawing::SRect const &rc)) override;

		private:
		};
		/// *********************************************************************
		ENUM_CLASS_OPERATORS(CStack::EAlignmentStack, uint32_t)
		/// *********************************************************************

		// *********************************************************************
		// *********************************************************************
	}
}
