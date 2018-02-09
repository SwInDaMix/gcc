/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Collections.CSorter.hpp
***********************************************************************/

#pragma once

#include "Types.h"

namespace System {
	namespace Collections {
		/// *********************************************************************
		template<typename TElement>
		class CSorter {
		private:
			CSorter() = delete;
			CSorter(CSorter const &other) = delete;
			CSorter &operator=(CSorter const &other) = delete;
			~CSorter() = delete;

		public:
			static void QuickSort(TElement *elements, uint16_t length, int32_t (*f_comapre)(TElement const &x, TElement const &y));
		private:
			static void _QuickSort_recursive(TElement *elements, uint16_t l, uint16_t r, int32_t (*f_comapre)(TElement const &x, TElement const &y));
		};
		/// *********************************************************************

		// *********************************************************************
		template<typename TElement> /*static*/ void CSorter<TElement>::QuickSort(TElement *a_elements, uint16_t length, int32_t (*f_comapre)(TElement const &x, TElement const &y)) {
			return _QuickSort_recursive(a_elements, 0, length - 1, f_comapre);
		}
		template<typename TElement> /*static*/ void CSorter<TElement>::_QuickSort_recursive(TElement *a_elements, uint16_t l, uint16_t r, int32_t (*f_comapre)(TElement const &x, TElement const &y)) {
			uint16_t _i = l, _j = r;
			TElement &_element_pivot = a_elements[l + ((r - l) >> 1)];
			while(_i <= _j) {
				while(f_comapre(a_elements[_i], _element_pivot) < 0) _i++;
				while(f_comapre(a_elements[_j], _element_pivot) > 0) _j--;
				if (_i <= _j) { TElement &_element_tmp = a_elements[_i]; a_elements[_i] = a_elements[_j]; a_elements[_j] = _element_tmp; _i++; _j--; }
			}
			if (_i < r) _QuickSort_recursive(a_elements, _i, r, f_comapre);
			if (l < _j) _QuickSort_recursive(a_elements, l, _j, f_comapre);
		}
		// *********************************************************************
	}
}
