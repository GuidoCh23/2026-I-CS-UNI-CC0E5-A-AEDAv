#ifndef __AVLTREE_H__
#define __AVLTREE_H__

#include <algorithm>
#include <shared_mutex>
#include "BinaryTree.h"
using namespace std;

// AVL: Extender BinaryTreeNode para tener la altura - AVLNode hereda BinaryTreeNode<T, AVLNode<T>> + m_height
template<typename T>
struct AVLNode : BinaryTreeNode<T, AVLNode<T>> {
    int m_height;
    AVLNode(T data, Ref ref = 0)
        : BinaryTreeNode<T, AVLNode<T>>(data, ref), m_height(1) {}
};

template<typename T> using AscendingAVLTrait  = AscendingTrait<AVLNode<T>>;
template<typename T> using DescendingAVLTrait = DescendingTrait<AVLNode<T>>;

template<typename Trait>
class AVLTree : public BinaryTree<Trait> {
public:
    using Base       = BinaryTree<Trait>;
    using Node       = typename Base::Node;
    using value_type = typename Base::value_type;
    using Base::Base;

    // AVL: Constructor copia - llama internal_copy con vtable de AVLTree para preservar m_height
    AVLTree(const AVLTree& other) : Base() {
        shared_lock<shared_mutex> lock(other.m_mtx);
        this->m_pRoot = this->internal_copy(other.m_pRoot);
        this->m_size  = other.m_size;
    }

    AVLTree(AVLTree&& other) noexcept : Base(move(other)) {}

    // AVL: Adaptar insert de BinaryTree - override con rebalanceo AVL
    // AVL: Concurrency in all (unique_lock)
    void insert(const value_type& data, Ref ref) override {
        unique_lock<shared_mutex> lock(this->m_mtx);
        this->m_pRoot = avl_insert(this->m_pRoot, data, ref);
    }

protected:
    // AVL: internal_copy preserva m_height de cada nodo
    Node* internal_copy(Node* pNode) override {
        if (!pNode) return nullptr;
        Node* n = new Node(pNode->m_data, pNode->m_ref);
        n->m_height    = pNode->m_height;
        n->m_pChild[0] = internal_copy(pNode->m_pChild[0]);
        n->m_pChild[1] = internal_copy(pNode->m_pChild[1]);
        return n;
    }

private:
    int node_height(Node* n) const { return n ? n->m_height : 0; }

    int balance(Node* n) const {
        return n ? node_height(n->m_pChild[0]) - node_height(n->m_pChild[1]) : 0;
    }

    void update_height(Node* n) {
        if (n) n->m_height = 1 + max(node_height(n->m_pChild[0]),
                                     node_height(n->m_pChild[1]));
    }

    Node* rotate_right(Node* y) {
        Node* x  = y->m_pChild[0];
        Node* T2 = x->m_pChild[1];
        x->m_pChild[1] = y;
        y->m_pChild[0] = T2;
        update_height(y);
        update_height(x);
        return x;
    }

    Node* rotate_left(Node* x) {
        Node* y  = x->m_pChild[1];
        Node* T2 = y->m_pChild[0];
        y->m_pChild[0] = x;
        x->m_pChild[1] = T2;
        update_height(x);
        update_height(y);
        return y;
    }

    // AVL: Adaptar insert - insercion recursiva con actualizacion de altura y rotaciones
    Node* avl_insert(Node* node, const value_type& data, Ref ref) {
        if (!node) { ++this->m_size; return new Node(data, ref); }

        // mismo criterio de branch que BinaryTree::internal_insert
        auto branch = (int)this->m_comp(node->m_data, data);
        node->m_pChild[branch] = avl_insert(node->m_pChild[branch], data, ref);
        update_height(node);

        int bf = balance(node);

        // Left-Left → rotacion derecha simple
        if (bf > 1  && !this->m_comp(node->m_pChild[0]->m_data, data))
            return rotate_right(node);
        // Right-Right → rotacion izquierda simple
        if (bf < -1 &&  this->m_comp(node->m_pChild[1]->m_data, data))
            return rotate_left(node);
        // Left-Right → rotacion izquierda sobre hijo izq + derecha sobre nodo
        if (bf > 1  &&  this->m_comp(node->m_pChild[0]->m_data, data)) {
            node->m_pChild[0] = rotate_left(node->m_pChild[0]);
            return rotate_right(node);
        }
        // Right-Left → rotacion derecha sobre hijo der + izquierda sobre nodo
        if (bf < -1 && !this->m_comp(node->m_pChild[1]->m_data, data)) {
            node->m_pChild[1] = rotate_right(node->m_pChild[1]);
            return rotate_left(node);
        }
        return node;
    }
};

#endif // __AVLTREE_H__
