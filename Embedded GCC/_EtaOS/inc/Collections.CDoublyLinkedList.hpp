/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Collections.CDoublyLinkedList.hpp
***********************************************************************/

#pragma once

#include "Types.h"

namespace System {
	namespace Collections {
		/// *********************************************************************
		template<typename TElement>
		class CDoublyLinkedList {
		public:
			/// **********************************
			template<typename TElementNode>
			class CNode {
				friend class CDoublyLinkedList<TElementNode>;

			private:
				CDoublyLinkedList<TElementNode> &d_dll;
				CNode<TElementNode> *d_prev;
				CNode<TElementNode> *d_next;
				TElementNode d_element;

			private:
				CNode(CDoublyLinkedList<TElementNode> &dll, TElementNode const &element, bool is_before);
			public:
				CNode(CNode const &other) = delete;
				CNode &operator=(CNode const &other) = delete;
				virtual ~CNode();

				CDoublyLinkedList<TElementNode> const &Dll_get() const;
				CNode<TElementNode> *Next_get() const;
				CNode<TElementNode> *Prev_get() const;
				TElementNode const &Element_get() const;
				void Element_set(TElementNode const &element);
			};
			/// **********************************

		private:
			CNode<TElement> *d_first;
			CNode<TElement> *d_last;
			uint32_t d_count;

		public:
			CDoublyLinkedList();
			CDoublyLinkedList(CDoublyLinkedList const &other) = delete;
			CDoublyLinkedList &operator=(CDoublyLinkedList const &other) = delete;
			virtual ~CDoublyLinkedList();

			uint32_t Count_get() const;
			CNode<TElement> *First_get() const;
			CNode<TElement> *Last_get() const;

			CNode<TElement> *PushTop(TElement const &element);
			CNode<TElement> *PushBottom(TElement const &element);
			TElement PopTop();
			TElement PopBottom();
		};
		/// *********************************************************************

		// *********************************************************************
		template<typename TElement> template<typename TElementNode> CDoublyLinkedList<TElement>::CNode<TElementNode>::CNode(CDoublyLinkedList<TElementNode> &dll, TElementNode const &element, bool is_before)
			: d_dll(dll), d_element(element) {
			if(is_before) {
				d_next = nullptr;
				d_prev = d_dll.d_last; d_dll.d_last = this;
				if (d_prev) d_prev->d_next = this;
				if (!d_dll.d_first) d_dll.d_first = this;
			}
			else {
				d_prev = nullptr;
				d_next = d_dll.d_first; d_dll.d_first = this;
				if (d_next) d_next->d_prev = this;
				if (!d_dll.d_last) d_dll.d_last = this;
			}
			d_dll.d_count++;
		}
		template<typename TElement> template<typename TElementNode> /*virtual*/ CDoublyLinkedList<TElement>::CNode<TElementNode>::~CNode() {
			if (d_prev) d_prev->d_next = d_next; else d_dll.d_first = d_next;
			if (d_next) d_next->d_prev = d_prev; else d_dll.d_last = d_prev;
			d_dll.d_count--;
		}

		template<typename TElement> template<typename TElementNode> CDoublyLinkedList<TElementNode> const &CDoublyLinkedList<TElement>::CNode<TElementNode>::Dll_get() const { return d_dll; }
		template<typename TElement> template<typename TElementNode> CDoublyLinkedList<TElement>::CNode<TElementNode> *CDoublyLinkedList<TElement>::CNode<TElementNode>::Next_get() const { return d_next; }
		template<typename TElement> template<typename TElementNode> CDoublyLinkedList<TElement>::CNode<TElementNode> *CDoublyLinkedList<TElement>::CNode<TElementNode>::Prev_get() const { return d_prev; }
		template<typename TElement> template<typename TElementNode> TElementNode const &CDoublyLinkedList<TElement>::CNode<TElementNode>::Element_get() const { return d_element; }
		template<typename TElement> template<typename TElementNode> void CDoublyLinkedList<TElement>::CNode<TElementNode>::Element_set(TElementNode const &element) { d_element = element; }
		// **********************************
		template<typename TElement> CDoublyLinkedList<TElement>::CDoublyLinkedList() : d_first(nullptr), d_last(nullptr), d_count(0) {  }
		template<typename TElement> /*virtual*/ CDoublyLinkedList<TElement>::~CDoublyLinkedList() { while(d_last) delete d_last; }

		template<typename TElement> uint32_t CDoublyLinkedList<TElement>::Count_get() const { return d_count; }
		template<typename TElement> CDoublyLinkedList<TElement>::CNode<TElement> *CDoublyLinkedList<TElement>::First_get() const { return d_first; }
		template<typename TElement> CDoublyLinkedList<TElement>::CNode<TElement> *CDoublyLinkedList<TElement>::Last_get() const { return d_last; }

		template<typename TElement> CDoublyLinkedList<TElement>::CNode<TElement> *CDoublyLinkedList<TElement>::PushTop(TElement const &element) { return new CDoublyLinkedList<TElement>::CNode<TElement>(*this, element, true); }
		template<typename TElement> CDoublyLinkedList<TElement>::CNode<TElement> *CDoublyLinkedList<TElement>::PushBottom(TElement const &element) { return new CDoublyLinkedList<TElement>::CNode<TElement>(*this, element, false); }
		template<typename TElement> TElement CDoublyLinkedList<TElement>::PopTop() { TElement _element(d_first->d_element); delete d_first; return _element; }
		template<typename TElement> TElement CDoublyLinkedList<TElement>::PopBottom() { TElement _element(d_last->d_element); delete d_last; return _element; }
		// *********************************************************************
	}
}
