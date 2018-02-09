/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Frames.CFrameEvent.cpp
 ***********************************************************************/

#include "Frames.CFrameEvent.hpp"
#include "Frames.CFrame.hpp"

using namespace System;
using namespace System::Frames;
using namespace System::Collections;

/// *********************************************************************
/*static*/ CDoublyLinkedList<CFrameEvent*> CFrameEvent::s_queue_events;

CFrameEvent::CFrameEvent(CFrame *const frame_from, CFrame &frame_to, CFrameEvent::EEvent event, CFrameEvent::EEventType event_type, uint32_t value1, uint32_t value2, void *object, void (*event_finalizer)(CFrameEvent &event)) : d_frame_from(frame_from), d_frame_to(frame_to), d_event(event), d_event_type(event_type), d_value1(value1), d_value2(value2), d_object(object), d_event_finalizer(event_finalizer) {  }
/*virtual*/ CFrameEvent::~CFrameEvent() { if(d_event_finalizer) d_event_finalizer(*this); }

/*static*/ CFrameEvent::EEventResultFlags CFrameEvent::EventSend(CFrame *const frame_from, CFrame &frame_to, EEvent event, EEventType event_type, uint32_t value1, uint32_t value2, void *object, void (*event_finalizer)(CFrameEvent &event)) {
	CFrameEvent *_event_to_fire = new CFrameEvent(frame_from, frame_to, event, event_type, value1, value2, object, event_finalizer);
	s_queue_events.PushTop(_event_to_fire); bool _is_fired = false; CFrameEvent::EEventResultFlags _res = CFrameEvent::EEventResultFlags::None;
	while(!_is_fired) {
		CFrameEvent *_event = s_queue_events.PopBottom();
		_EventFire(_event->d_frame_to, *_event);
		if(_event == _event_to_fire) { _is_fired = true; _res = _event_to_fire->d_event_result_flags; }
		delete _event;
	}
	return _res;
}
/*static*/ void CFrameEvent::EventSendAsync(CFrame *const frame_from, CFrame &frame_to, EEvent event, EEventType event_type, uint32_t value1, uint32_t value2, void *object, void (*event_finalizer)(CFrameEvent &event)) {
	s_queue_events.PushTop(new CFrameEvent(frame_from, frame_to, event, event_type, value1, value2, object, event_finalizer));
}
/*static*/ void CFrameEvent::EventDiscardAllForFrame(CFrame &frame_to) {
	CDoublyLinkedList<CFrameEvent*>::CNode<CFrameEvent*> *_node_event = s_queue_events.First_get();
	while(_node_event) { CDoublyLinkedList<CFrameEvent*>::CNode<CFrameEvent*> *_node_event_next = _node_event->Next_get(); CFrameEvent *_frame_event = _node_event->Element_get(); if(&_frame_event->d_frame_to == &frame_to) { delete _frame_event; delete _node_event; } _node_event = _node_event_next; }
}

/*static*/ bool CFrameEvent::_EventDispatch() {
	bool _res;
	if((_res = s_queue_events.Count_get())) {
		CFrameEvent *_event = s_queue_events.PopBottom();
		_EventFire(_event->d_frame_to, *_event);
		delete _event;
	}
	return _res;
}
/*static*/ void CFrameEvent::_EventFire(CFrame &frame_to, CFrameEvent &event) {
	CFrameEvent::EEventType _event_type = event.EventType_get();
	if (_event_type == CFrameEvent::EEventType::Direct) {
		frame_to.OnEvent(event);
	}
	else if (_event_type == CFrameEvent::EEventType::Bubbling) {
		CFrame *_frame = &frame_to;
		while (_frame && !flags(event.EventResultFlags_get())) { _frame->OnEvent(event); _frame = _frame->ContainerParent_get(); }
	}
	else if (_event_type == CFrameEvent::EEventType::Tunneling) {
		uint32_t _count = 0, _count_copy = 0;
		CFrame *_frame = &frame_to;
		while (_frame) { _count++; _frame = _frame->ContainerParent_get(); }
		CFrame *_a_frames_tunneling[_count]; _frame = &frame_to; _count_copy = _count;
		while (_frame) { _a_frames_tunneling[--_count_copy] = _frame; _frame = _frame->ContainerParent_get(); }
		while (!flags(event.EventResultFlags_get())) { _a_frames_tunneling[_count_copy++]->OnEvent(event); }
	}
	else if (_event_type == CFrameEvent::EEventType::Propagate) {
		frame_to.OnEvent(event);
		CContainer *_container = as_type(&frame_to, CContainer);
		if(_container) {
			CFrame *_frame = _container->ChildFrameFirst_get();
			while (_frame && !flags(event.EventResultFlags_get())) { _EventFire(*_frame, event); _frame = _frame->FrameNext_get(); }
		}
	}
}
/// *********************************************************************
