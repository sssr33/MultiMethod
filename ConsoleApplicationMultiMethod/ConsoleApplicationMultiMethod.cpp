// ConsoleApplicationMultiMethod.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Typelist.h"

#include <iostream>
#include <cstdint>

struct ICollidable;
typedef void(*CollideFn)(ICollidable* a, ICollidable* b, ICollidable* c);

const int ICollidableClassCount = 2;

struct ICollidableMMTable1Row {
    CollideFn r1[ICollidableClassCount] = {};
};

struct ICollidableMMTable0Row {
    ICollidableMMTable1Row* r0[ICollidableClassCount] = {};
};

struct ICollidableMMTable {
    ICollidableMMTable0Row* arg0 = nullptr;
    uint32_t arg1 = 0;
};

using TypList1 = Typelist<double, float, int>;
TypelistFn::GetType<TypList1, 2>::type xx;

int fff(int x) {
    return 123;
}
int fff(float x) {
    return 333;
}
int fff(double x) {
    return 999;
}

template<class... Ts>
struct TestForEachTypeH {
};

template<class TypelistT, size_t... IdxS>
struct TestForEachTypeH<TypelistT, std::index_sequence<IdxS...>> {
    int arr[TypelistFn::Size<TypelistT>::size] = { fff(typename TypelistFn::GetType<TypList1, IdxS>::type())... };
};

template<class TypelistT>
struct TestForEachType : public TestForEachTypeH<TypelistT, TypelistFn::IndexSeq<TypelistT>>{
};

template<class BaseT, class TypelistT, class RowT, class... RowTs>
struct RowGen0 : public RowGen0<BaseT, TypelistT, RowTs...>{

};

template<class BaseT, class TypelistT>
struct RowGen0<BaseT, TypelistT, void> {};

template<class BaseT, class... Deriveds>
struct MMTableGen {
    using Row0T = RowGen0<BaseT, Typelist<Deriveds...>, Deriveds...>;
};

//template<class T>
//struct CollideThunkGen {};
//
//template <class FNT, class BaseT, class AT, class BT, class CT>
//struct CollideThunkGen<Typelist<FNT, BaseT, AT, BT, CT>>
//{
//    static void Thunk(BaseT* a, BaseT* b, BaseT* c) {
//        FNT()(static_cast<AT*>(a), static_cast<BT*>(b), static_cast<CT*>(c));
//    }
//};

void CollideAAAThunk(ICollidable* a, ICollidable* b, ICollidable* c);
void CollideAABThunk(ICollidable* a, ICollidable* b, ICollidable* c);
void CollideABAThunk(ICollidable* a, ICollidable* b, ICollidable* c);
void CollideABBThunk(ICollidable* a, ICollidable* b, ICollidable* c);
void CollideBAAThunk(ICollidable* a, ICollidable* b, ICollidable* c);
void CollideBABThunk(ICollidable* a, ICollidable* b, ICollidable* c);
void CollideBBAThunk(ICollidable* a, ICollidable* b, ICollidable* c);
void CollideBBBThunk(ICollidable* a, ICollidable* b, ICollidable* c);

ICollidableMMTable1Row rowAA = { { CollideAAAThunk, CollideAABThunk } };
ICollidableMMTable1Row rowAB = { { CollideABAThunk, CollideABBThunk } };
ICollidableMMTable1Row rowBA = { { CollideBAAThunk, CollideBABThunk } };
ICollidableMMTable1Row rowBB = { { CollideBBAThunk, CollideBBBThunk } };

ICollidableMMTable0Row rowA = { { &rowAA, &rowAB } };
ICollidableMMTable0Row rowB = { { &rowBA, &rowBB } };

ICollidableMMTable atable = { &rowA, 0 };
ICollidableMMTable btable = { &rowB, 1 };

struct ICollidable {
    ICollidableMMTable *mmtable = nullptr;
};

struct ICollidableA : public ICollidable {
    ICollidableA() {
        this->mmtable = &atable;
    }
};

struct ICollidableB : public ICollidable {
    ICollidableB() {
        this->mmtable = &btable;
    }
};

void CollideMM(ICollidable* a, ICollidable* b, ICollidable* c);

void Collide(ICollidable* a, ICollidable* b, ICollidable* c);
void Collide(ICollidableA* a, ICollidableA* b, ICollidableA* c);
void Collide(ICollidableA* a, ICollidableA* b, ICollidableB* c);
void Collide(ICollidableA* a, ICollidableB* b, ICollidableA* c);
void Collide(ICollidableA* a, ICollidableB* b, ICollidableB* c);
void Collide(ICollidableB* a, ICollidableA* b, ICollidableA* c);
void Collide(ICollidableB* a, ICollidableA* b, ICollidableB* c);
void Collide(ICollidableB* a, ICollidableB* b, ICollidableA* c);
void Collide(ICollidableB* a, ICollidableB* b, ICollidableB* c);

int main()
{
    TestForEachType<TypList1> xx;

    ICollidableA a;
    ICollidableB b;

    ICollidable* aa = &a;
    ICollidable* bb = &b;

    CollideMM(aa, aa, aa);
    CollideMM(bb, bb, bb);
    CollideMM(bb, aa, bb);
    CollideMM(aa, bb, aa);

    std::cout << "Hello World!\n";
}

void CollideMM(ICollidable* a, ICollidable* b, ICollidable* c) {
    if (!a->mmtable) {
        return;
    }
    if (!b->mmtable) {
        return;
    }
    if (!c->mmtable) {
        return;
    }

    a->mmtable->arg0->r0[b->mmtable->arg1]->r1[c->mmtable->arg1](a, b, c);
}

void CollideAAAThunk(ICollidable* a, ICollidable* b, ICollidable* c) {
    Collide(static_cast<ICollidableA*>(a), static_cast<ICollidableA*>(b), static_cast<ICollidableA*>(c));
}
void CollideAABThunk(ICollidable* a, ICollidable* b, ICollidable* c) {
    Collide(static_cast<ICollidableA*>(a), static_cast<ICollidableA*>(b), static_cast<ICollidableB*>(c));
}
void CollideABAThunk(ICollidable* a, ICollidable* b, ICollidable* c) {
    Collide(static_cast<ICollidableA*>(a), static_cast<ICollidableB*>(b), static_cast<ICollidableA*>(c));
}
void CollideABBThunk(ICollidable* a, ICollidable* b, ICollidable* c) {
    Collide(static_cast<ICollidableA*>(a), static_cast<ICollidableB*>(b), static_cast<ICollidableB*>(c));
}
void CollideBAAThunk(ICollidable* a, ICollidable* b, ICollidable* c) {
    Collide(static_cast<ICollidableB*>(a), static_cast<ICollidableA*>(b), static_cast<ICollidableA*>(c));
}
void CollideBABThunk(ICollidable* a, ICollidable* b, ICollidable* c) {
    Collide(static_cast<ICollidableB*>(a), static_cast<ICollidableA*>(b), static_cast<ICollidableB*>(c));
}
void CollideBBAThunk(ICollidable* a, ICollidable* b, ICollidable* c) {
    Collide(static_cast<ICollidableB*>(a), static_cast<ICollidableB*>(b), static_cast<ICollidableA*>(c));
}
void CollideBBBThunk(ICollidable* a, ICollidable* b, ICollidable* c) {
    Collide(static_cast<ICollidableB*>(a), static_cast<ICollidableB*>(b), static_cast<ICollidableB*>(c));
}


void Collide(ICollidable* a, ICollidable* b, ICollidable* c) {
    std::cout << "Base\n";
}
void Collide(ICollidableA* a, ICollidableA* b, ICollidableA* c) {
    std::cout << "AAA\n";
}
void Collide(ICollidableA* a, ICollidableA* b, ICollidableB* c) {
    std::cout << "AAB\n";
}
void Collide(ICollidableA* a, ICollidableB* b, ICollidableA* c) {
    std::cout << "ABA\n";
}
void Collide(ICollidableA* a, ICollidableB* b, ICollidableB* c) {
    std::cout << "ABB\n";
}
void Collide(ICollidableB* a, ICollidableA* b, ICollidableA* c) {
    std::cout << "BAA\n";
}
void Collide(ICollidableB* a, ICollidableA* b, ICollidableB* c) {
    std::cout << "BAB\n";
}
void Collide(ICollidableB* a, ICollidableB* b, ICollidableA* c) {
    std::cout << "BBA\n";
}
void Collide(ICollidableB* a, ICollidableB* b, ICollidableB* c) {
    std::cout << "BBB\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
