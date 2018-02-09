/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Frames.CFrame.hpp
 ***********************************************************************/

#pragma once

#include "Types.h"
#include "Frames.Config.hpp"
#include "Frames.CFrameEvent.hpp"
#include "Drawing.CFont.hpp"
#include "Drawing.CImage.hpp"
#include "Drawing.EColor.hpp"
#include "Drawing.SEdge.hpp"
#include "Drawing.SPoint.hpp"
#include "Drawing.SRect.hpp"
#include "Drawing.SSize.hpp"

namespace System {
	namespace Frames {
		class CWindow;
		class CContainer;
		class CContainerSingle;
		class CContainerMulti;
		/// *********************************************************************
		CLASS_TYPEID(CFrame)
			friend class CFrameEvent;
			friend class CContainer;
			friend class CContainerSingle;
			friend class CContainerMulti;
			friend class CWindow;

		public:
			/// **********************************
			enum class EStyleFlags : uint32_t {
				Focusable			= 1 << 0,
				Enabled				= 1 << 1,
				Hidden				= 1 << 2,
				_UserFlags			= 1 << 8,
			};
			/// **********************************
			enum class EKeyDialog : uint16_t {
				Dialog_OK_Yes_Next,
				Dialog_Cancel_No_Back,
				Dialog_Left,
				Dialog_Right,
				Dialog_Up,
				Dialog_Down,
				Dialog_Menu_Options,
				Dialog_Frame_Prev,
				Dialog_Frame_Next,
			};
			/// **********************************

		public:
			static System::Drawing::CImage1 s_img1_frame_close;
			static System::Drawing::CImage1 s_img1_frame_maximize;
			static System::Drawing::CImage1 s_img1_frame_minimize;
			static System::Drawing::CImage1 s_img1_frame_restore;

		private:
			CWindow &d_window_root;
			CContainer *const d_container_parent;
			CFrame *d_frame_prev;
			CFrame *d_frame_next;
			System::Drawing::SSize d_sz_min;

			#define frame_cached_ptr_decl(Type, GName, LName) \
			private: \
				Type *d_##LName; \
				Type *d_cached_##LName; \
				bool _ResetValidFlags_##GName(bool stop_if_set); \
			public: \
				Type &GName##_get(); \
				void GName##_set(Type *value);
			#define frame_cached_nullable_decl(Type, GName, LName) \
			private: \
				SNullable<Type> d_##LName; \
				SNullable<Type> d_cached_##LName; \
				bool _ResetValidFlags_##GName(bool stop_if_set); \
			public: \
				Type GName##_get(); \
				void GName##_set(SNullable<Type> const &value);

			SNullable<System::Drawing::SPoint> d_cached_pt_location;
			SNullable<System::Drawing::SSize> d_cached_sz_size;
			SNullable<System::Drawing::SSize> d_cached_sz_size_fit;

			frame_cached_ptr_decl(const System::Drawing::CFont, Font, font)
			frame_cached_nullable_decl(bool, FontIsBold, font_is_bold)
			frame_cached_nullable_decl(bool, FontIsShadow, font_is_shadow)
			frame_cached_nullable_decl(System::Drawing::EColor, ColorForeground, color_foreground)
			frame_cached_nullable_decl(System::Drawing::EColor, ColorBackground, color_background)

		protected:
			EStyleFlags const d_style_flags;

		protected:
			CFrame(CFrame const &other) = delete;
			CFrame &operator=(CFrame const &other) = delete;
			CFrame(CWindow &window_root, CContainer *container_parent, EStyleFlags style_flags);
			virtual ~CFrame() override;

		public:
			CWindow &WindowRoot_get() const;
			CContainer *ContainerParent_get() const;
			CFrame *FramePrev_get() const;
			CFrame *FrameNext_get() const;
			EStyleFlags StyleFlags_get() const;
			bool IsFocused_get() const;
			bool IsParticipating_get() const;

			System::Drawing::SPoint const &Location_get();
			System::Drawing::SSize const &Size_get();
			System::Drawing::SSize const &SizeFit_get();
			System::Drawing::SSize const &SizeMin_get();
			void SizeMin_set(System::Drawing::SSize const &sz_min);

			uint8_t SeparationWidth_get();
			void SeparationWidth_set(SNullable<uint8_t> const &value);

			void Repaint();
			void RepaintWithBackground();
			virtual CFrame *HitTest(System::Drawing::SPoint const &pt);

			CFrame *GetFrameNext(bool is_cyclic) const;
			CFrame *GetFramePrev(bool is_cyclic) const;
			CFrame *GetFrameHierarchyNext(bool is_cyclic) const;
			CFrame *GetFrameHierarchyPrev(bool is_cyclic) const;
			CFrame *GetFrameHierarchyNextStyled(bool is_cyclic, EStyleFlags style_flags) const;
			CFrame *GetFrameHierarchyPrevStyled(bool is_cyclic, EStyleFlags style_flags) const;

		protected:
			virtual void OnEvent(CFrameEvent &event);

			virtual System::Drawing::SSize CalcFitSize();

			void SetRectFromParent(System::Drawing::SRect const &rc);

		private:
			void _DiscardSizesAndLocations();
			bool _UpdateFocusedFrame(CFrame *frame_focus_new);
			void _SetLocationFromParent(System::Drawing::SPoint const &pt_location);
			void _SetSizeFromParent(System::Drawing::SSize const &sz_size);
			CFrame *_GetFrameTabStyled(bool is_cyclic, CFrame *(CFrame::*f_next_frame)(bool is_cyclic) const, EStyleFlags style_flags) const;
			void _ResetValidFlags(bool stop_if_set, bool(CFrame::*f_reset)(bool stop_if_set));
		};
		/// *********************************************************************
		CLASS_TYPEID_INHERIT(CContainer, CFrame)
			friend class CFrame;
			friend class CContainerSingle;
			friend class CContainerMulti;
			friend class CWindow;

		private:
			bool d_is_custom;

			frame_cached_nullable_decl(uint8_t, SeparationWidth, separation_width)

		protected:
			CContainer(CContainer const &other) = delete;
			CContainer &operator=(CContainer const &other) = delete;
			CContainer(CWindow &window_root, CContainer *container_parent, bool is_custom, EStyleFlags style_flags);
			virtual ~CContainer() override;

		public:
			virtual CFrame *ChildFrameFirst_get() const = 0;
			virtual CFrame *ChildFrameLast_get() const = 0;

		protected:
			virtual void OnEvent(CFrameEvent &event) override;

			virtual bool IsFrameParticipating(CFrame const &frame) const;
			virtual void CalcChildRects(void(CFrame::*f_set_rect)(System::Drawing::SRect const &rc)) = 0;
			void DiscardSizeAndLocation();

		private:
			virtual void _AttachChildFrame(CFrame &frame) = 0;
			virtual void _DeattachChildFrame(CFrame &frame) = 0;
			void _ResetValidFlags(bool stop_if_set, bool(CContainer::*f_reset)(bool stop_if_set));
		};
		/// *********************************************************************
		CLASS_TYPEID_INHERIT(CContainerSingle, CContainer)
			friend class CFrame;
			friend class CContainer;
			friend class CContainerMulti;
			friend class CWindow;

		private:
			CFrame *d_frame_child;

		protected:
			CContainerSingle(CContainerSingle const &other) = delete;
			CContainerSingle &operator=(CContainerSingle const &other) = delete;
			CContainerSingle(CWindow &window_root, CContainer *container_parent, EStyleFlags style_flags);
			virtual ~CContainerSingle() override;

		public:
			CFrame *ChildFrame_get() const;
			virtual CFrame *ChildFrameFirst_get() const override;
			virtual CFrame *ChildFrameLast_get() const override;

			virtual CFrame *HitTest(System::Drawing::SPoint const &pt) override;

		protected:
			virtual void OnEvent(CFrameEvent &event) override;

		private:
			virtual void _AttachChildFrame(CFrame &frame) override;
			virtual void _DeattachChildFrame(CFrame &frame) override;
		};
		/// *********************************************************************
		CLASS_TYPEID_INHERIT(CContainerMulti, CContainer)
			friend class CFrame;
			friend class CContainer;
			friend class CContainerSingle;
			friend class CWindow;

		private:
			CFrame *d_frame_child_first;
			CFrame *d_frame_child_last;

		protected:
			CContainerMulti(CContainerMulti const &other) = delete;
			CContainerMulti &operator=(CContainerMulti const &other) = delete;
			CContainerMulti(CWindow &window_root, CContainer *container_parent, bool is_custom, EStyleFlags style_flags);
			virtual ~CContainerMulti() override;

		public:
			virtual CFrame *ChildFrameFirst_get() const override;
			virtual CFrame *ChildFrameLast_get() const override;

			virtual CFrame *HitTest(System::Drawing::SPoint const &pt) override;

		protected:
			virtual void OnEvent(CFrameEvent &event) override;

			CFrame *HitTestMulti(System::Drawing::SPoint const &pt);

		private:
			virtual void _AttachChildFrame(CFrame &frame) override;
			virtual void _DeattachChildFrame(CFrame &frame) override;
		};
		/// *********************************************************************
		CLASS_TYPEID_INHERIT(CWindow, CContainerSingle)
			friend class CFrame;
			friend class CContainer;
			friend class CContainerSingle;
			friend class CContainerMulti;

		public:
			/// **********************************
			enum class EStyleFlags : uint32_t {
				Border		= ((uint32_t)CFrame::EStyleFlags::_UserFlags) << 0,
			};
			/// **********************************

		private:
			static CWindow *s_window_first;
			static CWindow *s_window_last;

			CWindow *d_window_prev;
			CWindow *d_window_next;
			CFrame *d_frame_focus;
			bool d_is_discard_sizes_and_locations;
			System::Drawing::SPoint d_pt_location;
			char const *const d_str_caption;

		protected:
			CWindow(CWindow const &other) = delete;
			CWindow &operator=(CWindow const &other) = delete;
		public:
			CWindow(System::Drawing::SPoint const &pt_location, System::Drawing::SSize const &sz_min, SNullable<uint8_t> separation_width, char const *str_caption, EStyleFlags style_flags);
			virtual ~CWindow() override;

			static CWindow *WindowFirst_get();
			static CWindow *WindowLast_get();
			CWindow *WindowPrev_get() const;
			CWindow *WindowNext_get() const;
			CFrame *FrameFocus_get();
			char const *Caption_get() const;
			bool IsTop_get() const;

			CFrame *FrameFirst_get() const;
			CFrame *FrameLast_get() const;
			CFrame *GetFrameFirstStyled(CFrame::EStyleFlags style_flags) const;
			CFrame *GetFrameLastStyled(CFrame::EStyleFlags style_flags) const;

			void BringToTop();

		protected:
			virtual void OnEvent(CFrameEvent &event) override;
			virtual System::Drawing::SSize CalcFitSize() override;
			virtual void CalcChildRects(void(CFrame::*f_set_rect)(System::Drawing::SRect const &rc)) override;

		private:
			void _AttachWindowAsFirst();
			void _DeattachWindow();
			void _EnsureDiscardedSizesAndLocations();
		};
		/// *********************************************************************
		ENUM_CLASS_OPERATORS(CFrame::EStyleFlags, uint32_t)
		ENUM_CLASS_OPERATORS(CFrame::EKeyDialog, uint16_t)
		ENUM_CLASS_OPERATORS_INHERIT(CWindow::EStyleFlags, System::Frames::CFrame::EStyleFlags, uint32_t)
		/// *********************************************************************

		// *********************************************************************
		inline CWindow &CFrame::WindowRoot_get() const { return d_window_root; }
		inline CContainer *CFrame::ContainerParent_get() const { return d_container_parent; }
		inline CFrame *CFrame::FramePrev_get() const { return d_frame_prev; }
		inline CFrame *CFrame::FrameNext_get() const { return d_frame_next; }
		inline CFrame::EStyleFlags CFrame::StyleFlags_get() const { return d_style_flags; }
		inline bool CFrame::IsFocused_get() const { return d_window_root.d_frame_focus == this; }
		inline bool CFrame::IsParticipating_get() const { return d_container_parent->IsFrameParticipating(*this); }
		// **********************************
		// **********************************
		inline CFrame *CContainerSingle::ChildFrame_get() const { return d_frame_child; }
		// **********************************
		// **********************************
		inline /*static*/ CWindow *CWindow::WindowFirst_get() { return s_window_first; }
		inline /*static*/ CWindow *CWindow::WindowLast_get() { return s_window_last; }
		inline CWindow *CWindow::WindowPrev_get() const { return d_window_prev; }
		inline CWindow *CWindow::WindowNext_get() const { return d_window_next; }
		inline CFrame *CWindow::FrameFocus_get() { return d_frame_focus; }
		inline char const *CWindow::Caption_get() const { return d_str_caption; }
		inline bool CWindow::IsTop_get() const { return !d_window_prev; }
		// *********************************************************************
	}
}
