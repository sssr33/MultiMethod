#include "VariantMM.h"

#include <iostream>
#include <variant>
#include <tuple>

// https://stackoverflow.com/questions/31255890/obtain-all-but-last-parameter-of-variadic-template
// https://stackoverflow.com/questions/16928669/how-to-get-n-th-type-from-a-tuple
// https://prog-cpp.ru/permutation/

namespace details {
    template<template<class...>class Z, class, class...>
    struct can_apply :std::false_type {};
    template<template<class...>class Z, class...Ts>
    struct can_apply<Z, std::void_t<Z<Ts...>>, Ts...> :
        std::true_type {};
}

template<template<class...>class Z, class...Ts>
using can_apply = details::can_apply<Z, void, Ts...>;

class A {
    int val = 11;
};

class B {
    int val = 22;
};

class C {
    int val = 44;
    int val2 = 44;
};

using VarObj = std::variant<A, B, C>;

struct ColliderConcrete {
    void operator()(A& a, B& b) {
        std::cout << "Collide A B!\n";
    }

    void operator()(A& a, C& c) {
        std::cout << "Collide A C!\n";
    }

    void operator()(B& a, C& c) {
        std::cout << "Collide B C!\n";
    }

    /*void operator()(C& a, A& c) {
        std::cout << "Collide C A!\n";
    }

    void operator()(A& a1, A& a2) {
        std::cout << "Collide A A!\n";
    }*/

    void operator()(A&, B&, C&) {
        std::cout << "Collide A B C!\n";
    }

    void operator()(A&, B&, B&) {
        std::cout << "Collide A B B!\n";
    }

    void operator()(A&, C&, C&) {
        std::cout << "Collide A C C!\n";
    }



    void operator()(A&, B&, C&, C&) {
        std::cout << "Collide A B C C!\n";
    }

    void operator()(A&, B&, B&, B&) {
        std::cout << "Collide A B B B!\n";
    }

    void operator()(A&, C&, C&, C&) {
        std::cout << "Collide A C C C!\n";
    }
};

template<class F, class...Ts>
using collide_r = decltype(std::declval<F>().operator()(std::declval<Ts>()...));

template<class F, class...Ts>
using can_collide = can_apply<collide_r, F, Ts...>;

struct Collider : public ColliderConcrete {
    template<class TA, class TB>
    void Collide2Generic(TA& a, TB& b) {
        std::cout << "Collide 2 generic!\n";
    }

    template<class TA, class TB, class TC>
    void Collide3Generic(TA& a, TB& b, TC& c) {
        std::cout << "Collide 3 generic!\n";
    }

    template<class TA, class TB, class TC, class TD>
    void Collide4Generic(TA& a, TB& b, TC& c, TD& d) {
        std::cout << "Collide 4 generic!\n";
    }

    template<size_t...BI, size_t...AI, size_t...OthersI, class... Args>
    void operator2Impl(
        std::index_sequence<BI...>,
        std::index_sequence<AI...>,
        std::index_sequence<OthersI...>,
        std::tuple<Args&...> args)
    {
        using t_args = std::tuple<Args&...>;

        if constexpr (can_collide<ColliderConcrete&,
            typename std::tuple_element<OthersI, t_args>::type...,
            typename std::tuple_element<AI, t_args>::type...,
            typename std::tuple_element<BI, t_args>::type...
        >::value)
        {
            ColliderConcrete::operator()(
                std::get<OthersI>(std::move(args))...,
                std::get<AI>(std::move(args))...,
                std::get<BI>(std::move(args))...
                );
        }
        else if constexpr (can_collide<ColliderConcrete&,
            typename std::tuple_element<OthersI, t_args>::type...,
            typename std::tuple_element<BI, t_args>::type...,
            typename std::tuple_element<AI, t_args>::type...
        >::value)
        {
            ColliderConcrete::operator()(
                std::get<OthersI>(std::move(args))...,
                std::get<BI>(std::move(args))...,
                std::get<AI>(std::move(args))...
                );
        }
        else {
            this->Collide2Generic(
                std::get<AI>(std::move(args))...,
                std::get<BI>(std::move(args))...
            );
        }
    }

    template<class... Args>
    void operator2(std::tuple<Args&...> args) {
        if constexpr (sizeof...(Args) >= 2) {
            this->operator2Impl(
                std::index_sequence<sizeof...(Args) - 1>{},
                std::index_sequence<sizeof...(Args) - 2>{},
                std::make_index_sequence<sizeof...(Args) - 2>{},
                args);
        }
        else {
            int stop = 234;
        }
    }

    template<class TupleT, size_t...BI, size_t...AI, size_t...OthersI>
    static constexpr bool canOperator2Impl(
        std::index_sequence<BI...>,
        std::index_sequence<AI...>,
        std::index_sequence<OthersI...>)
    {
        if constexpr (can_collide<ColliderConcrete&,
            typename std::tuple_element<OthersI, TupleT>::type...,
            typename std::tuple_element<AI, TupleT>::type...,
            typename std::tuple_element<BI, TupleT>::type...
        >::value)
        {
            return true;
        }
        else if constexpr (can_collide<ColliderConcrete&,
            typename std::tuple_element<OthersI, TupleT>::type...,
            typename std::tuple_element<BI, TupleT>::type...,
            typename std::tuple_element<AI, TupleT>::type...
        >::value)
        {
            return true;
        }

        return false;
    }

    template<size_t...CI, size_t...BI, size_t...AI, size_t...OthersI, class... Args>
    void operator3Impl(
        std::index_sequence<CI...>,
        std::index_sequence<BI...>,
        std::index_sequence<AI...>,
        std::index_sequence<OthersI...>,
        std::tuple<Args&...> args)
    {
        // ABC
        if constexpr (canOperator2Impl<std::tuple<Args&...>>(std::index_sequence<CI...>{}, std::index_sequence<BI...>{}, std::index_sequence<OthersI..., AI...>{})) {
            this->operator2Impl(
                std::index_sequence<CI...>{},
                std::index_sequence<BI...>{},
                std::index_sequence<OthersI..., AI...>{},
                args);
        }
        // BCA
        else if constexpr (canOperator2Impl<std::tuple<Args&...>>(std::index_sequence<AI...>{}, std::index_sequence<CI...>{}, std::index_sequence<OthersI..., BI...>{})) {
            this->operator2Impl(
                std::index_sequence<AI...>{},
                std::index_sequence<CI...>{},
                std::index_sequence<OthersI..., BI...>{},
                args);
        }
        // CAB
        else if constexpr (canOperator2Impl<std::tuple<Args&...>>(std::index_sequence<BI...>{}, std::index_sequence<AI...>{}, std::index_sequence<OthersI..., CI...>{})) {
            this->operator2Impl(
                std::index_sequence<BI...>{},
                std::index_sequence<AI...>{},
                std::index_sequence<OthersI..., CI...>{},
                args);
        }
        else {
            this->Collide3Generic(
                std::get<AI>(std::move(args))...,
                std::get<BI>(std::move(args))...,
                std::get<CI>(std::move(args))...
            );
        }
    }

    template<class... Args>
    void operator3(std::tuple<Args&...> args) {
        if constexpr (sizeof...(Args) >= 3) {
            this->operator3Impl(
                std::index_sequence<sizeof...(Args) - 1>{},
                std::index_sequence<sizeof...(Args) - 2>{},
                std::index_sequence<sizeof...(Args) - 3>{},
                std::make_index_sequence<sizeof...(Args) - 3>{},
                args);
        }
        else {
            int stop = 234;
        }
    }

    template<class TupleT, size_t...CI, size_t...BI, size_t...AI, size_t...OthersI>
    static constexpr bool canOperator3Impl(
        std::index_sequence<CI...>,
        std::index_sequence<BI...>,
        std::index_sequence<AI...>,
        std::index_sequence<OthersI...>)
    {
        // ABC
        if constexpr (canOperator2Impl<TupleT>(std::index_sequence<CI...>{}, std::index_sequence<BI...>{}, std::index_sequence<OthersI..., AI...>{})) {
            return true;
        }
        // BCA
        else if constexpr (canOperator2Impl<TupleT>(std::index_sequence<AI...>{}, std::index_sequence<CI...>{}, std::index_sequence<OthersI..., BI...>{})) {
            return true;
        }
        // CAB
        else if constexpr (canOperator2Impl<TupleT>(std::index_sequence<BI...>{}, std::index_sequence<AI...>{}, std::index_sequence<OthersI..., CI...>{})) {
            return true;
        }

        return false;
    }

    template<size_t...DI, size_t...CI, size_t...BI, size_t...AI, size_t...OthersI, class... Args>
    void operator4Impl(
        std::index_sequence<DI...>,
        std::index_sequence<CI...>,
        std::index_sequence<BI...>,
        std::index_sequence<AI...>,
        std::index_sequence<OthersI...>,
        std::tuple<Args&...> args)
    {
        // ABCD
        if constexpr (canOperator3Impl<std::tuple<Args&...>>(std::index_sequence<DI...>{}, std::index_sequence<CI...>{}, std::index_sequence<BI...>{}, std::index_sequence<OthersI..., AI...>{})) {
            this->operator3Impl(
                std::index_sequence<DI...>{},
                std::index_sequence<CI...>{},
                std::index_sequence<BI...>{},
                std::index_sequence<OthersI..., AI...>{},
                args);
        }
        // BCDA
        else if constexpr (canOperator3Impl<std::tuple<Args&...>>(std::index_sequence<AI...>{}, std::index_sequence<DI...>{}, std::index_sequence<CI...>{}, std::index_sequence<OthersI..., BI...>{})) {
            this->operator3Impl(
                std::index_sequence<AI...>{},
                std::index_sequence<DI...>{},
                std::index_sequence<CI...>{},
                std::index_sequence<OthersI..., BI...>{},
                args);
        }
        // CDAB
        else if constexpr (canOperator3Impl<std::tuple<Args&...>>(std::index_sequence<BI...>{}, std::index_sequence<AI...>{}, std::index_sequence<DI...>{}, std::index_sequence<OthersI..., CI...>{})) {
            this->operator3Impl(
                std::index_sequence<BI...>{},
                std::index_sequence<AI...>{},
                std::index_sequence<DI...>{},
                std::index_sequence<OthersI..., CI...>{},
                args);
        }
        // DABC
        else if constexpr (canOperator3Impl<std::tuple<Args&...>>(std::index_sequence<CI...>{}, std::index_sequence<BI...>{}, std::index_sequence<AI...>{}, std::index_sequence<OthersI..., DI...>{})) {
            this->operator3Impl(
                std::index_sequence<CI...>{},
                std::index_sequence<BI...>{},
                std::index_sequence<AI...>{},
                std::index_sequence<OthersI..., DI...>{},
                args);
        }
        else {
            this->Collide4Generic(
                std::get<AI>(std::move(args))...,
                std::get<BI>(std::move(args))...,
                std::get<CI>(std::move(args))...,
                std::get<DI>(std::move(args))...
            );
        }
    }

    template<class... Args>
    void operator4(std::tuple<Args&...> args) {
        if constexpr (sizeof...(Args) >= 4) {
            this->operator4Impl(
                std::index_sequence<sizeof...(Args) - 1>{},
                std::index_sequence<sizeof...(Args) - 2>{},
                std::index_sequence<sizeof...(Args) - 3>{},
                std::index_sequence<sizeof...(Args) - 4>{},
                std::make_index_sequence<sizeof...(Args) - 4>{},
                args);
        }
        else {
            int stop = 234;
        }
    }

    template<class... Args>
    void operator()(std::tuple<Args&...> args) {
        if constexpr (sizeof...(Args) == 4) {
            this->operator4(args);
        }
        if constexpr (sizeof...(Args) == 3) {
            this->operator3(args);
        }
        else if constexpr (sizeof...(Args) == 2) {
            this->operator2(args);
        }
    }

    template<class... Args>
    void operator()(Args&... args) {
        this->operator()(std::forward_as_tuple(std::forward<Args&>(args)...));
    }
};

template<class ColliderT>
void Collide(VarObj& a, VarObj& b, ColliderT& collider) {
    std::visit([&](auto&& concreteA)
        {
            std::visit([&](auto&& concreteB)
                {
                    collider(concreteA, concreteB);
                },
                b);
        },
        a);
}

template<class ColliderT>
void Collide(VarObj& a, VarObj& b, VarObj& c, ColliderT& collider) {
    std::visit([&](auto&& concreteA)
        {
            std::visit([&](auto&& concreteB)
                {
                    std::visit([&](auto&& concreteC)
                        {
                            collider(concreteA, concreteB, concreteC);
                        },
                        c);
                },
                b);
        },
        a);
}

template<class ColliderT>
void Collide(VarObj& a, VarObj& b, VarObj& c, VarObj& d, ColliderT& collider) {
    std::visit([&](auto&& concreteA)
        {
            std::visit([&](auto&& concreteB)
                {
                    std::visit([&](auto&& concreteC)
                        {
                            std::visit([&](auto&& concreteD)
                                {
                                    collider(concreteA, concreteB, concreteC, concreteD);
                                },
                                d);
                        },
                        c);
                },
                b);
        },
        a);
}

void VariantMM_Test()
{
    std::cout << "VariantMM_Test start\n";

    sizeof(VarObj);
    sizeof(A);
    sizeof(B);
    sizeof(C);

    Collider collider;

    VarObj obj1 = A();
    VarObj obj2 = B();

    Collide(obj1, obj2, collider);

    obj1 = C();
    obj2 = B();

    Collide(obj1, obj2, collider);

    obj1 = C();
    obj2 = A();

    Collide(obj1, obj2, collider);

    obj1 = A();
    obj2 = C();

    Collide(obj1, obj2, collider);

    obj1 = A();
    obj2 = A();

    Collide(obj1, obj2, collider);

    obj1 = B();
    obj2 = B();

    Collide(obj1, obj2, collider);

    VarObj obj3 = B();

    obj1 = B();
    obj2 = A();
    obj3 = C();

    Collide(obj1, obj2, obj3, collider);

    obj1 = B();
    obj2 = B();
    obj3 = C();

    Collide(obj1, obj2, obj3, collider);

    obj1 = B();
    obj2 = A();
    obj3 = B();

    Collide(obj1, obj2, obj3, collider);


    VarObj obj4 = B();

    obj1 = B();
    obj2 = A();
    obj3 = C();
    obj4 = C();

    Collide(obj1, obj2, obj3, obj4, collider);

    obj1 = B();
    obj2 = B();
    obj3 = B();
    obj4 = A();

    Collide(obj1, obj2, obj3, obj4, collider);

    obj1 = C();
    obj2 = C();
    obj3 = C();
    obj4 = A();

    Collide(obj1, obj2, obj3, obj4, collider);

    std::cout << "VariantMM_Test done\n";
}
