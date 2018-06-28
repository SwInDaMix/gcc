/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Types.h
 ***********************************************************************/

#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>

typedef bool bit;

#define NL "\r\n"

#define ntz(x)	\
	( (1 & (((x) & 0x00000001) == 0)) \
	+ (1 & (((x) & 0x00000003) == 0)) \
    + (1 & (((x) & 0x00000007) == 0)) \
    + (1 & (((x) & 0x0000000F) == 0)) \
    + (1 & (((x) & 0x0000001F) == 0)) \
	+ (1 & (((x) & 0x0000003F) == 0)) \
	+ (1 & (((x) & 0x0000007F) == 0)) \
	+ (1 & (((x) & 0x000000FF) == 0)) \
	+ (1 & (((x) & 0x000001FF) == 0)) \
	+ (1 & (((x) & 0x000003FF) == 0)) \
	+ (1 & (((x) & 0x000007FF) == 0)) \
	+ (1 & (((x) & 0x00000FFF) == 0)) \
	+ (1 & (((x) & 0x00001FFF) == 0)) \
	+ (1 & (((x) & 0x00003FFF) == 0)) \
	+ (1 & (((x) & 0x00007FFF) == 0)) \
	+ (1 & (((x) & 0x0000FFFF) == 0)) \
	+ (1 & (((x) & 0x0001FFFF) == 0)) \
	+ (1 & (((x) & 0x0003FFFF) == 0)) \
	+ (1 & (((x) & 0x0007FFFF) == 0)) \
	+ (1 & (((x) & 0x000FFFFF) == 0)) \
	+ (1 & (((x) & 0x001FFFFF) == 0)) \
	+ (1 & (((x) & 0x003FFFFF) == 0)) \
	+ (1 & (((x) & 0x007FFFFF) == 0)) \
	+ (1 & (((x) & 0x00FFFFFF) == 0)) \
	+ (1 & (((x) & 0x01FFFFFF) == 0)) \
	+ (1 & (((x) & 0x03FFFFFF) == 0)) \
	+ (1 & (((x) & 0x07FFFFFF) == 0)) \
	+ (1 & (((x) & 0x0FFFFFFF) == 0)) \
	+ (1 & (((x) & 0x1FFFFFFF) == 0)) \
	+ (1 & (((x) & 0x3FFFFFFF) == 0)) \
	+ (1 & (((x) & 0x7FFFFFFF) == 0)) \
	+ (1 & (((x) & 0xFFFFFFFF) == 0)) )

#ifdef __cplusplus
using namespace std;

#define ENUM_CLASS_OPERATORS(TEnum, TInt) \
	inline TEnum operator &(TEnum x, TEnum y) { return static_cast<TEnum>(static_cast<TInt>(x) & static_cast<TInt>(y)); } \
	inline TEnum operator |(TEnum x, TEnum y) { return static_cast<TEnum>(static_cast<TInt>(x) | static_cast<TInt>(y)); } \
	inline TEnum operator ^(TEnum x, TEnum y) { return static_cast<TEnum>(static_cast<TInt>(x) ^ static_cast<TInt>(y)); } \
	inline TEnum operator ~(TEnum x) { return static_cast<TEnum>(~static_cast<TInt>(x)); } \
	inline TEnum &operator &=(TEnum &x, TEnum y) { x = x & y; return x; } \
	inline TEnum &operator |=(TEnum &x, TEnum y) { x = x | y; return x; } \
	inline TEnum &operator ^=(TEnum &x, TEnum y) { x = x ^ y; return x; } \
	inline bool	flags(TEnum x) { return (bool)x; }
#define ENUM_CLASS_OPERATORS_INHERIT(TEnum, TBaseEnum, TInt) ENUM_CLASS_OPERATORS(TEnum, TInt) \
	inline TEnum operator &(TEnum x, TBaseEnum y) { return static_cast<TEnum>(static_cast<TInt>(x) & static_cast<TInt>(y)); } \
	inline TEnum operator |(TEnum x, TBaseEnum y) { return static_cast<TEnum>(static_cast<TInt>(x) | static_cast<TInt>(y)); } \
	inline TEnum operator ^(TEnum x, TBaseEnum y) { return static_cast<TEnum>(static_cast<TInt>(x) ^ static_cast<TInt>(y)); } \
	inline TEnum operator &(TBaseEnum x, TEnum y) { return static_cast<TEnum>(static_cast<TInt>(x) & static_cast<TInt>(y)); } \
	inline TEnum operator |(TBaseEnum x, TEnum y) { return static_cast<TEnum>(static_cast<TInt>(x) | static_cast<TInt>(y)); } \
	inline TEnum operator ^(TBaseEnum x, TEnum y) { return static_cast<TEnum>(static_cast<TInt>(x) ^ static_cast<TInt>(y)); } \
	inline TEnum &operator &=(TEnum &x, TBaseEnum y) { x = x & y; return x; } \
	inline TEnum &operator |=(TEnum &x, TBaseEnum y) { x = x | y; return x; } \
	inline TEnum &operator ^=(TEnum &x, TBaseEnum y) { x = x ^ y; return x; }

class CTypeID {
	protected:
		void *d_vtable;
		virtual ~CTypeID() {  }
	public:
		virtual char const *type_name() const = 0;
		virtual bool is_type_txt(char const *type_name_need) const = 0;
};
#define CLASS_TYPEID_INTERNAL(Compare, TClass) \
	class TClass : protected CTypeID { \
	public: \
		virtual char const *type_name() const override { return #TClass; } \
		virtual bool is_type_txt(char const *type_name_need) const override { return Compare(TClass::type_name(), type_name_need); } \
	private:
#define CLASS_TYPEID_INHERIT_INTERNAL(Compare, TClass, TBase) \
	class TClass : public TBase { \
	public: \
		virtual char const *type_name() const override { return #TClass; } \
		virtual bool is_type_txt(char const *type_name_need) const override { return Compare(TClass::type_name(), type_name_need) || TBase::is_type_txt(type_name_need); } \
	private:
#define is_type(Var, TClass) (((void *)(Var) != nullptr) && ((CTypeID *)(Var))->is_type_txt(#TClass))
#define as_type(Var, TClass) (is_type(Var, TClass) ? (TClass *)(Var) : nullptr)
#define typeid_ctor() { d_vtable = *((void*volatile*)this); }
#define typeid_dtor() { *((void*volatile*)this) = d_vtable; }
#ifdef __PLATFORM_WINDOWS__
	#define type_id_compare_type_name(x, y) (!strcmp(x, y))
	#define CLASS_TYPEID(TClass) CLASS_TYPEID_INTERNAL(type_id_compare_type_name, TClass)
	#define CLASS_TYPEID_INHERIT(TClass, TBase) CLASS_TYPEID_INHERIT_INTERNAL(type_id_compare_type_name, TClass, TBase)
#else
	#define type_id_compare_type_name(x, y) ((intptr_t)(x) == (intptr_t)(y))
	#define CLASS_TYPEID(TClass) CLASS_TYPEID_INTERNAL(type_id_compare_type_name, TClass)
	#define CLASS_TYPEID_INHERIT(TClass, TBase) CLASS_TYPEID_INHERIT_INTERNAL(type_id_compare_type_name, TClass, TBase)
#endif

template<typename T>
struct SNullable {
private:
	T d_value;
	bool d_is_set;

public:
	SNullable();
	SNullable(T value);
	SNullable(std::nullptr_t value);
	SNullable(SNullable<T> const &other);
	SNullable<T> &operator =(T const &other);
	SNullable<T> &operator =(nullptr_t const &other);
	SNullable<T> &operator =(SNullable<T> const &other);

public:
	bool IsValueSet_get() const;
	T GetValueOrDefault(T default_value) const;

	bool operator ==(SNullable<T>const &other) const;
	bool operator !=(SNullable<T> const &other) const;
	bool operator ==(T const &other) const;
	bool operator !=(T const &other) const;

	operator T const &() const;
};

template<typename T> SNullable<T>::SNullable() : d_value(), d_is_set(false) {  }
template<typename T> SNullable<T>::SNullable(T value) : d_value(value), d_is_set(true) {  }
template<typename T> SNullable<T>::SNullable(nullptr_t value) : d_value(), d_is_set(false) {  }
template<typename T> SNullable<T>::SNullable(SNullable<T> const &other) : d_value(other.d_value), d_is_set(other.d_is_set) {  }
template<typename T> SNullable<T> &SNullable<T>::operator =(T const &other) { d_value = other; d_is_set = true; return *this; }
template<typename T> SNullable<T> &SNullable<T>::operator =(nullptr_t const &other) { d_is_set = false; return *this; }
template<typename T> SNullable<T> &SNullable<T>::operator =(SNullable<T> const &other) { if((d_is_set = other.d_is_set)) { d_value = other.d_value; } return *this; }

template<typename T> inline bool SNullable<T>::IsValueSet_get() const { return d_is_set; }
template<typename T> inline T SNullable<T>::GetValueOrDefault(T default_value) const { return d_is_set ? d_value : default_value; }

template<typename T> bool SNullable<T>::operator ==(SNullable<T> const &other) const { return d_is_set == other.d_is_set && (!d_is_set || d_value == other.d_value); }
template<typename T> bool SNullable<T>::operator !=(SNullable<T> const &other) const { return d_is_set != other.d_is_set || (d_is_set && d_value != other.d_value); }
template<typename T> bool SNullable<T>::operator ==(T const &other) const { return d_is_set && (!d_is_set || d_value == other); }
template<typename T> bool SNullable<T>::operator !=(T const &other) const { return !d_is_set || (d_is_set && d_value != other); }

template<typename T> inline SNullable<T>::operator T const &() const { return d_value; }

#endif // __cplusplus

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __cplusplus
#define nullptr ((void *)0)
#endif

#if __AVR__
	#define force_ptr(ptr) __asm__ __volatile__("" : "=b" (ptr) : "0" (ptr))

	typedef __int24 int24_t;
	typedef __uint24 uint24_t;

	typedef union {
		struct {
			uint8_t b1;
			uint8_t b2;
		};
		uint16_t value;
	} ___int16;
	typedef union {
		struct {
			uint8_t b1;
			uint8_t b2;
			uint8_t b3;
		};
		uint24_t value;
	} ___int24;
	typedef union {
		struct {
			uint8_t b1;
			uint8_t b2;
			uint8_t b3;
			uint8_t b4;
		};
		uint32_t value;
	} ___int32;
	#define make_int16(a, b) __extension__ ({ ___int16 _res; _res.b1 = a; _res.b2 = b; _res.value; })
	#define make_int16_be(a, b) __extension__ ({ ___int16 _res; _res.b2 = a; _res.b1 = b; _res.value; })
	#define make_int24(a, b, c) __extension__ ({ ___int24 _res; _res.b1 = a; _res.b2 = b; _res.b3 = c; _res.value; })
	#define make_int24_be(a, b, c) __extension__ ({ ___int24 _res; _res.b3 = a; _res.b2 = b; _res.b1 = c; _res.value; })
	#define make_int32(a, b, c, d) __extension__ ({ ___int32 _res; _res.b1 = a; _res.b2 = b; _res.b3 = c; _res.b4 = d; _res.value; })
	#define make_int32_be(a, b, c, d) __extension__ ({ ___int32 _res; _res.b4 = a; _res.b3 = b; _res.b2 = c; _res.b1 = d; _res.value; })
#endif

#define sizeof_array(array) (sizeof(array) / sizeof(array[0]))

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef minwith
#define minwith(a, b) { if((a) > (b)) (a) = (b); }
#endif
#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef maxwith
#define maxwith(a, b) { if((a) < (b)) (a) = (b); }
#endif
#ifndef abs
#define abs(a) ((a) > 0 ? (a) : (-(a)))
#endif
#ifndef abswith
#define abswith(a) { if((a) < 0) (a) = (-(a)); }
#endif
#ifndef neg
#define neg(a) ((a) < 0 ? (a) : (-(a)))
#endif
#ifndef negwith
#define negwith(a) { if((a) > 0) (a) = (-(a)); }
#endif
#ifndef lowbyte
#define lowbyte(a) ((unsigned char)(a))
#endif
#ifndef hibyte
#define hibyte(a) ((unsigned char)((a) >> 8))
#endif
#ifndef wordfrombytes
#define wordfrombytes(a, b) (((b) << 8) | (a))
#endif

#ifdef __cplusplus
}
#endif
