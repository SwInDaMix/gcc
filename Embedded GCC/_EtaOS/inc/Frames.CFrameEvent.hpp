/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Frames.CFrameEvent.hpp
 ***********************************************************************/

#pragma once

#include "Types.h"
#include "Collections.CDoublyLinkedList.hpp"

namespace System {
	namespace Frames {
		/// *********************************************************************
		class CFrame;
		class CFrameEvent {
			friend class CFrame;

		public:
			/// **********************************
			enum class EEvent : uint8_t {
				Created,
				Closed,
				KeyDown,
				KeyUp,
				KeyCanceled,
				KeyPressed,
				KeyDialog,
				Paint,
				PaintWithBackground,
				Resize,
				Relocation,
				Timer,
			};
			/// **********************************
			enum class EEventType : uint8_t {
				Direct,
				Bubbling,
				Tunneling,
				Propagate,
			};
			/// **********************************
			enum class EEventResultFlags : uint8_t {
				None = 0x0,
				Handled = 0x1,
				Canceled = 0x2,
			};
			/// **********************************

		private:
			static System::Collections::CDoublyLinkedList<CFrameEvent*> s_queue_events;

			CFrame *const d_frame_from;
			CFrame &d_frame_to;
			EEvent const d_event;
			EEventType const d_event_type;
			uint32_t const d_value1;
			uint32_t const d_value2;
			void *const d_object;
			void (*const d_event_finalizer)(CFrameEvent &event);
			EEventResultFlags d_event_result_flags = EEventResultFlags::None;

		private:
			CFrameEvent(CFrame *const frame_from, CFrame &frame_to, EEvent event, EEventType event_type, uint32_t value1, uint32_t value2, void *object, void (*event_finalizer)(CFrameEvent &event));
			CFrameEvent(CFrameEvent const &other) = delete;
			CFrameEvent &operator=(CFrameEvent const &other) = delete;
			virtual ~CFrameEvent();

		public:
			CFrame *FrameFrom_get() const;
			CFrame &FrameTo_get() const;
			EEvent Event_get() const;
			EEventType EventType_get() const;
			uint32_t Value1_get() const;
			uint32_t Value2_get() const;
			void *Object_get() const;
			void (*EventFinalizer_get() const)(CFrameEvent &event);
			EEventResultFlags EventResultFlags_get() const;

			void MarkFlags(EEventResultFlags event_result_flags);

			static EEventResultFlags EventSend(CFrame *const frame_from, CFrame &frame_to, EEvent event, EEventType event_type, uint32_t value1, uint32_t value2, void *object, void (*event_finalizer)(CFrameEvent &event));
			static void EventSendAsync(CFrame *const frame_from, CFrame &frame_to, EEvent event, EEventType event_type, uint32_t value1, uint32_t value2, void *object, void (*event_finalizer)(CFrameEvent &event));
			static void EventDiscardAllForFrame(CFrame &frame_to);

			static bool _EventDispatch();
		private:
			static void _EventFire(CFrame &frame_to, CFrameEvent &event);
		};
		/// **********************************
		ENUM_CLASS_OPERATORS(CFrameEvent::EEvent, uint8_t)
		ENUM_CLASS_OPERATORS(CFrameEvent::EEventType, uint8_t)
		ENUM_CLASS_OPERATORS(CFrameEvent::EEventResultFlags, uint8_t)
		/// *********************************************************************

		// *********************************************************************
		inline CFrame *CFrameEvent::FrameFrom_get() const { return d_frame_from; }
		inline CFrame &CFrameEvent::FrameTo_get() const { return d_frame_to; }
		inline CFrameEvent::EEvent CFrameEvent::Event_get() const { return d_event; }
		inline CFrameEvent::EEventType CFrameEvent::EventType_get() const { return d_event_type; }
		inline uint32_t CFrameEvent::Value1_get() const { return d_value1; }
		inline uint32_t CFrameEvent::Value2_get() const { return d_value2; }
		inline void *CFrameEvent::Object_get() const { return d_object; }
		inline void (*CFrameEvent::EventFinalizer_get() const)(CFrameEvent &event) { return d_event_finalizer; }
		inline CFrameEvent::EEventResultFlags CFrameEvent::EventResultFlags_get() const { return d_event_result_flags; }

		inline void CFrameEvent::MarkFlags(EEventResultFlags event_result_flags) { d_event_result_flags |= event_result_flags; }
		// *********************************************************************
	}
}
