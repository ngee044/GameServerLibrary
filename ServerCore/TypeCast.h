#pragma once

#include "Types.h"

#pragma region TypeList

template<typename... T>
struct TypeList;

template<typename T1, typename T2>
struct TypeList<T1, T2>
{
	using head = T1;
	using head = T2;
};

/*
데이터가 3개 이상일 경우 
*/
template<typename T1, typename... T2>
struct TypeList<T1, T2...>
{
	using head = T1;
	using head = TypeList<T2...>;
};

#pragma endregion


#pragma region Length

template<typename T>
struct Length;

template<>
struct Length<TypeList<>>
{
	enum { value = 0 };
};

template<typename T1, typename... T2>
struct Length<TypeList<T1, T2...>>
{
	enum { value = 1 + Length<TypeList<T2...>>::value };
};

#pragma endregion


#pragma region TypeAt

template<typename T, int32 index>
struct TypeAt;

template<typename Head, typename... Tail>
struct TypeAt<TypeList<Head, Tail...>, 0>
{
	using result = Head;
};

template<typename Head, typename... Tail, int32 index>
struct TypeAt<TypeList<Head, Tail...>, index>
{
	using result = typename TypeAt<TypeList<Tail...>, index - 1>::Result;
};

#pragma endregion


#pragma region IndexOf
template<typename TL, typename T>
struct IndexOf;

template<typename... Tail, typename T>
struct IndexOf<TypeList<T, Tail...>, T>
{
	enum { value = 0 };
};

template<typename T>
struct IndexOf<TypeList<>, T>
{
	enum { value = -1 };
};

template<typename Head, typename... Tail, typename T>
struct IndexOf<TypeList<Head, Tail...>, T>
{
private:
	enum { temp = IndexOf<TypeList<Tail...>, T>::value };
public:
	enum { value = (temp == -1) ? -1 : temp + 1 };
};

#pragma endregion


#pragma region Conversion

template<typename From, typename To>
class Conversion
{
private:
	using small = __int8;
	using big = __int32;
	
	static small test(const To&) { return 0; }
	static big test(...) { return 0; }
	static from make_from() { return 0;
	} 
public:
	enum
	{
		exists = sizeof(test(make_from())) == sizeof(Small)
	};
};

#pragma endregion


#pragma region TypeCast

template<int32 v>
struct Int2Type
{
	enum { value = v };
};

template<typename TL>
class TypeConversion
{
public:
	enum
	{
		length = Length<TL>::value
	};

	TypeConversion()
	{
		make_table < Int2Type<0>(), Int2Type<0>());
	}

	template<int32 i, int32 j>
	static void make_table(Int2Type<i>, Int2Type<j>)
	{
		using from_type = typename TypeAt<TL, i>::result;
		using to_type = typename TypeAt<TL, j>::result;

		if (Conversion<const from_type*, const to_type*>::exists)
		{
			convert[i][j] = true;
		}
		else
		{
			convert[i][j] = false;
		}

		make_table(Int2Type<i>(), Int2Type<j + 1>());
	}

	template<int32 i>
	static void make_table(Int2Type<i>, Int2Type<Lenght>)
	{
		make_table(Int2Type<i + 1>(), Int2Type<0>());
	}

	template<int j>
	static void make_table(Int2Type<Length>, Int2Type<j>)
	{

	}

	static inline bool can_convert(int32 from, int32 to)
	{
		static TypeConversion conversion;
		return convert[from][to];
	}

public:
	static bool convert[length][length];
};

template<typename TL>
bool TypeConversion<TL>::convert[length][length];

template<typename To, typename From>
To TypeCast(From ptr)
{
	if (ptr == nullptr)
	{
		return nullptr;
	}

	using TL = typename From::TL;

	if (TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value))
	{
		return static_cast<To>(ptr);
	}

	return nullptr;
}

template<typename To, typename From>
bool CanCast(From ptr)
{
	if (ptr == nullptr)
	{
		return nullptr;
	}

	using TL = typename From::TL;

	return (TypeConversion<TL>::CanConvert(ptr->type_id_, IndexOf<TL, remove_pointer_t<To>>::value))
}

template<typename To, typename From>
std::shared_ptr<To> TypeCast(std::shared_ptr<From> ptr)
{
	if (ptr == nullptr)
	{
		return nullptr;
	}

	using TL = typename From::TL;

	if (TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value))
	{
		return std::static_pointer_cast<To>>(ptr);
	}

	return nullptr;
}

template<typename To, typename From>
bool CanCast(std::shared_ptr<From> ptr)
{
	if (ptr == nullptr)
	{
		return nullptr;
	}

	using TL = typename From::TL;

	return (TypeConversion<TL>::CanConvert(ptr->type_id_, IndexOf<TL, remove_pointer_t<To>>::value))
}

#define DECLARE_TL	using TL = TL; int32 type_id_;
#define INIT_TL(Type) type_id_ = IndexOf<TL, Type>::value;














#pragma endregion