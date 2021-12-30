#pragma once

#include <cstdint>
#include <utility>

// ======== Typelist
template<class...>
struct Typelist {};

// ======== TypelistSize
template<class TypelistT>
struct TypelistSize {};

template<class... Types>
struct TypelistSize<Typelist<Types...>>
{
    static const size_t size = sizeof...(Types);
};

// ======== TypelistGetType
template<size_t TypeIdx, class... Types>
struct TypelistGetTypeH : public TypelistGetTypeH<TypeIdx - 1, Types...>
{};

template<size_t TypeIdx, class T, class... Types>
struct TypelistGetTypeH<TypeIdx, T, Types...> : public TypelistGetTypeH<TypeIdx - 1, Types...>
{};

template<class T, class... Types>
struct TypelistGetTypeH<0, T, Types...>
{
    typedef T type;
};

template<>
struct TypelistGetTypeH<0>
{
    typedef void type;
};

template<class TypelistT, size_t TypeIdx>
struct TypelistGetType {};

template<size_t TypeIdx, class... Types>
struct TypelistGetType<Typelist<Types...>, TypeIdx>
{
    typedef typename TypelistGetTypeH<TypeIdx, Types...>::type type;
};

// ======== TypelistFn
struct TypelistFn {
    template<class TypelistT>
    using Size = TypelistSize<TypelistT>;

    template<class TypelistT, size_t TypeIdx>
    using GetType = TypelistGetType<TypelistT, TypeIdx>;

    template<class TypelistT>
    using IndexSeq = std::make_index_sequence<TypelistFn::Size<TypelistT>::size>;
};
