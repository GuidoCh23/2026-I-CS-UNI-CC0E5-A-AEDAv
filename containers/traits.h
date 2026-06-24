#ifndef __TRAITS_H__
#define __TRAITS_H__
#include <functional> // para less y greater
#include "../types.h"

template <typename _Node, typename _Comp>
struct BaseTrait{
    using Node       = _Node;
    using value_type = typename _Node::value_type;
    using Comp       = _Comp;
};

template <typename _Node>
struct AscendingTrait : public BaseTrait<_Node, less<typename _Node::value_type>>{
};
template <typename _Node>
struct DescendingTrait : public BaseTrait<_Node, greater<typename _Node::value_type>>{
};

// P1 Tarea Traits
template <typename _KeyType, typename _ObjIDType = Ref>
struct BTreeTrait {
    using KeyType   = _KeyType;
    using ObjIDType = _ObjIDType;
};

#endif // __TRAITS_H__