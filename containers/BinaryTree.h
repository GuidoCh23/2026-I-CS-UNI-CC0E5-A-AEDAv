#ifndef __BINARYTREE_H__
#define __BINARYTREE_H__

#include <iostream>
#include <sstream>
#include <string>
#include <shared_mutex>
#include <algorithm>
#include "stack.h"
#include "traits.h"
#include "../types.h"
using namespace std;

// BinaryTree: BinaryTreeNode con CRTP (mismo patron que LLNode), value_type y Ref
template<typename T, typename DerivedNode = void>
struct BinaryTreeNode {
    using MyNode     = conditional_t<is_void_v<DerivedNode>, BinaryTreeNode<T>, DerivedNode>;
    using value_type = T;
    T       m_data;
    Ref     m_ref;
    MyNode *m_pChild[2];
    BinaryTreeNode(T data, Ref ref = 0)
        : m_data(data), m_ref(ref), m_pChild{nullptr, nullptr} {}
    T&  getDataRef() { return m_data; }
    Ref getRef()     { return m_ref; }
};

// BinaryTree: BTIteratorBase con Stack<Node*> pre-llenado e indice
template<typename Node, typename value_type>
class BTIteratorBase {
protected:
    Stack<Node*> m_nodes;
    size_t       m_index;
public:
    BTIteratorBase(Stack<Node*> nodes, size_t index)
        : m_nodes(move(nodes)), m_index(index) {}
    bool operator==(const BTIteratorBase& o) const { return m_index == o.m_index; }
    bool operator!=(const BTIteratorBase& o) const { return m_index != o.m_index; }
    value_type& operator*() { return m_nodes[m_index]->m_data; }
};

// BinaryTree: forward iterator (inorder/preorder/postorder) - incrementa m_index
template<typename Node, typename value_type>
class BTForwardIterator : public BTIteratorBase<Node, value_type> {
public:
    using BTIteratorBase<Node, value_type>::BTIteratorBase;
    BTForwardIterator& operator++() { ++this->m_index; return *this; }
};

// BinaryTree: backward iterator (inorder/preorder/postorder) - decrementa m_index
template<typename Node, typename value_type>
class BTBackwardIterator : public BTIteratorBase<Node, value_type> {
public:
    using BTIteratorBase<Node, value_type>::BTIteratorBase;
    BTBackwardIterator& operator++() {
        // size_t: al decrementar 0 se obtiene SIZE_MAX que actua como centinela de fin
        if (this->m_index == 0) this->m_index = SIZE_MAX;
        else                    --this->m_index;
        return *this;
    }
};

// BinaryTree: TraversalView con begin/end, rbegin/rend y forEach/rForEach
template<typename FwdIt, typename BwdIt>
class TraversalView {
    FwdIt m_begin, m_end;
    BwdIt m_rbegin, m_rend;
public:
    TraversalView(FwdIt b, FwdIt e, BwdIt rb, BwdIt re)
        : m_begin(b), m_end(e), m_rbegin(rb), m_rend(re) {}
    FwdIt begin()  { return m_begin; }
    FwdIt end()    { return m_end; }
    BwdIt rbegin() { return m_rbegin; }
    BwdIt rend()   { return m_rend; }
    template<typename Func, typename... Args>
    void forEach(Func func, Args&&... args) {
        for (auto it = m_begin; it != m_end; ++it)
            func(*it, forward<Args>(args)...);
    }
    template<typename Func, typename... Args>
    void rForEach(Func func, Args&&... args) {
        for (auto it = m_rbegin; it != m_rend; ++it)
            func(*it, forward<Args>(args)...);
    }
};

template<typename Trait>
class BinaryTree {
public:
    using value_type = typename Trait::value_type;
    using Node       = typename Trait::Node;
    using Compare    = typename Trait::Comp;
    using FwdIt      = BTForwardIterator<Node, value_type>;
    using BwdIt      = BTBackwardIterator<Node, value_type>;
    using View       = TraversalView<FwdIt, BwdIt>;

protected:
    Node*   m_pRoot;
    size_t  m_size;
    Compare m_comp;
    mutable shared_mutex m_mtx;

    // BinaryTree: Destructor Seguro - libera nodos recursivamente
    virtual void internal_clear(Node* pNode) {
        if (!pNode) return;
        internal_clear(pNode->m_pChild[0]);
        internal_clear(pNode->m_pChild[1]);
        delete pNode;
    }

    // BinaryTree: Constructor copia - copia el subarbol recursivamente
    virtual Node* internal_copy(Node* pNode) {
        if (!pNode) return nullptr;
        Node* n = new Node(pNode->m_data, pNode->m_ref);
        n->m_pChild[0] = internal_copy(pNode->m_pChild[0]);
        n->m_pChild[1] = internal_copy(pNode->m_pChild[1]);
        return n;
    }

    // Codigo hecho en clase - adaptado con Ref y branch corregido para orden ascendente
    virtual void internal_insert(Node*& pNode, const value_type& data, Ref ref) {
        if (!pNode) { pNode = new Node(data, ref); return; }
        // m_comp(current, new): true -> new es mayor -> va a la derecha (branch 1)
        auto branch = m_comp(pNode->m_data, data);
        internal_insert(pNode->m_pChild[branch], data, ref);
    }

    // BinaryTree: Otras mejoras libres #1 - busqueda BST con equivalencia generica
    virtual Node* internal_search(Node* pNode, const value_type& val) const {
        if (!pNode) return nullptr;
        if (!m_comp(pNode->m_data, val) && !m_comp(val, pNode->m_data)) return pNode;
        if (m_comp(val, pNode->m_data)) return internal_search(pNode->m_pChild[0], val);
        return internal_search(pNode->m_pChild[1], val);
    }

    int internal_height(Node* pNode) const {
        if (!pNode) return -1;
        return 1 + max(internal_height(pNode->m_pChild[0]),
                       internal_height(pNode->m_pChild[1]));
    }

    // BinaryTree: forward/backward iterator (inorder) - recorrido LNR
    void fill_inorder(Node* pNode, Stack<Node*>& s) const {
        if (!pNode) return;
        fill_inorder(pNode->m_pChild[0], s);
        s.push(pNode);
        fill_inorder(pNode->m_pChild[1], s);
    }

    // BinaryTree: forward/backward iterator (preorder) - recorrido NLR
    void fill_preorder(Node* pNode, Stack<Node*>& s) const {
        if (!pNode) return;
        s.push(pNode);
        fill_preorder(pNode->m_pChild[0], s);
        fill_preorder(pNode->m_pChild[1], s);
    }

    // BinaryTree: forward/backward iterator (postorder) - recorrido LRN
    void fill_postorder(Node* pNode, Stack<Node*>& s) const {
        if (!pNode) return;
        fill_postorder(pNode->m_pChild[0], s);
        fill_postorder(pNode->m_pChild[1], s);
        s.push(pNode);
    }

    View make_view(Stack<Node*> nodes) const {
        size_t n = nodes.size();
        FwdIt fwd_b(nodes, 0);
        FwdIt fwd_e(Stack<Node*>(), n);
        BwdIt bwd_b(nodes, n > 0 ? n - 1 : SIZE_MAX);
        BwdIt bwd_e(Stack<Node*>(), SIZE_MAX);
        return View(fwd_b, fwd_e, bwd_b, bwd_e);
    }

    // BinaryTree: Otras mejoras libres #2 - impresion visual indentada
    void internal_print(Node* pNode, ostream& os, int depth) const {
        if (!pNode) return;
        internal_print(pNode->m_pChild[1], os, depth + 1);
        for (int i = 0; i < depth; ++i) os << "  ";
        os << "(" << pNode->m_data << "," << pNode->m_ref << ")\n";
        internal_print(pNode->m_pChild[0], os, depth + 1);
    }

public:
    BinaryTree() : m_pRoot(nullptr), m_size(0) {}

    // BinaryTree: Constructor copia
    BinaryTree(const BinaryTree& other) : m_pRoot(nullptr), m_size(0) {
        shared_lock<shared_mutex> lock(other.m_mtx);
        m_pRoot = internal_copy(other.m_pRoot);
        m_size  = other.m_size;
    }

    // BinaryTree: Move Constructor
    BinaryTree(BinaryTree&& other) noexcept : m_pRoot(nullptr), m_size(0) {
        unique_lock<shared_mutex> lock(other.m_mtx);
        m_pRoot       = other.m_pRoot;
        m_size        = other.m_size;
        other.m_pRoot = nullptr;
        other.m_size  = 0;
    }

    // BinaryTree: Destructor Seguro
    virtual ~BinaryTree() { clear(); }

    // BinaryTree: Concurrencia
    void clear() {
        unique_lock<shared_mutex> lock(m_mtx);
        internal_clear(m_pRoot);
        m_pRoot = nullptr;
        m_size  = 0;
    }

    // BinaryTree: Concurrencia
    virtual void insert(const value_type& data, Ref ref) {
        unique_lock<shared_mutex> lock(m_mtx);
        internal_insert(m_pRoot, data, ref);
        ++m_size;
    }

    // BinaryTree: Otras mejoras libres #1 — busqueda con shared_lock
    bool search(const value_type& val) const {
        shared_lock<shared_mutex> lock(m_mtx);
        return internal_search(m_pRoot, val) != nullptr;
    }

    int height() const {
        shared_lock<shared_mutex> lock(m_mtx);
        return internal_height(m_pRoot);
    }

    size_t size() const {
        shared_lock<shared_mutex> lock(m_mtx);
        return m_size;
    }

    // BinaryTree: usar en bucle nativo foreach (begin/end devuelven iterador inorder)
    FwdIt begin() const {
        shared_lock<shared_mutex> lock(m_mtx);
        Stack<Node*> s;
        fill_inorder(m_pRoot, s);
        return FwdIt(move(s), 0);
    }
    FwdIt end() const {
        shared_lock<shared_mutex> lock(m_mtx);
        return FwdIt(Stack<Node*>(), m_size);
    }

    // BinaryTree: forward/backward iterator (inorder)
    View inorder() const {
        shared_lock<shared_mutex> lock(m_mtx);
        Stack<Node*> s;
        fill_inorder(m_pRoot, s);
        return make_view(move(s));
    }

    // BinaryTree: forward/backward iterator (preorder)
    View preorder() const {
        shared_lock<shared_mutex> lock(m_mtx);
        Stack<Node*> s;
        fill_preorder(m_pRoot, s);
        return make_view(move(s));
    }

    // BinaryTree: forward/backward iterator (postorder)
    View postorder() const {
        shared_lock<shared_mutex> lock(m_mtx);
        Stack<Node*> s;
        fill_postorder(m_pRoot, s);
        return make_view(move(s));
    }

    // BinaryTree: ToString - formato [(val,ref),...] inorder
    string toString() const {
        shared_lock<shared_mutex> lock(m_mtx);
        ostringstream oss;
        oss << "[";
        Stack<Node*> s;
        fill_inorder(m_pRoot, s);
        for (size_t i = 0; i < s.size(); ++i) {
            if (i > 0) oss << ",";
            oss << "(" << s[i]->m_data << "," << s[i]->m_ref << ")";
        }
        oss << "]";
        return oss.str();
    }

    // BinaryTree: Otras mejoras libres #2 - impresion visual del arbol
    void printTree(ostream& os) const {
        shared_lock<shared_mutex> lock(m_mtx);
        internal_print(m_pRoot, os, 0);
    }
};

// BinaryTree: operator<< (incluye persistencia a archivos via ostream)
template<typename Trait>
ostream& operator<<(ostream& os, const BinaryTree<Trait>& t) {
    return os << t.toString();
}

// BinaryTree: operator>> — parsea formato [(val,ref),...] e inserta en el arbol
template<typename Trait>
istream& operator>>(istream& is, BinaryTree<Trait>& t) {
    char c;
    is >> c; // '['
    while (is.peek() != ']' && is.good()) {
        is >> c; // '('
        typename Trait::value_type val;
        Ref  ref;
        char comma;
        is >> val >> comma >> ref;
        is >> c; // ')'
        t.insert(val, ref);
        if (is.peek() == ',') is >> c;
    }
    is >> c; // ']'
    return is;
}

#endif // __BINARYTREE_H__
