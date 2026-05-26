#ifndef __AVLTREE_H__
#define __AVLTREE_H__

#include <algorithm>
#include <shared_mutex>
#include "BinaryTree.h"
using namespace std;

// AVL: Extender BinaryTreeNode para tener la altura - AVLNode hereda BinaryTreeNode<T, AVLNode<T>> + m_height
template<typename T>
struct AVLNode : BinaryTreeNode<T, AVLNode<T>> {
    size_t m_height;
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

protected:
    // AVL: Adaptar insert de BinaryTree - reutiliza BST insert del padre y adapta con altura y rotaciones
    Node* internal_insert(Node* pNode, const value_type& data, Ref ref) override {
        pNode = Base::internal_insert(pNode, data, ref);
        update_height(pNode);

        Ref bf = balance(pNode);

        // Left-Left -> rotacion derecha simple
        if (bf > 1  && !this->m_comp(pNode->m_pChild[0]->m_data, data))
            return rotate(pNode, 1);
        // Right-Right -> rotacion izquierda simple
        if (bf < -1 &&  this->m_comp(pNode->m_pChild[1]->m_data, data))
            return rotate(pNode, 0);
        // Left-Right -> rotacion izquierda sobre hijo izq + derecha sobre nodo
        if (bf > 1  &&  this->m_comp(pNode->m_pChild[0]->m_data, data)) {
            pNode->m_pChild[0] = rotate(pNode->m_pChild[0], 0);
            return rotate(pNode, 1);
        }
        // Right-Left -> rotacion derecha sobre hijo der + izquierda sobre nodo
        if (bf < -1 && !this->m_comp(pNode->m_pChild[1]->m_data, data)) {
            pNode->m_pChild[1] = rotate(pNode->m_pChild[1], 1);
            return rotate(pNode, 0);
        }
        return pNode;
    }

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
    size_t node_height(Node* n) const { return n ? n->m_height : 0; }

    Ref balance(Node* n) const {
        return n ? (Ref)node_height(n->m_pChild[0]) - (Ref)node_height(n->m_pChild[1]) : 0;
    }

    void update_height(Node* n) {
        if (n) n->m_height = 1 + max(node_height(n->m_pChild[0]),
                                     node_height(n->m_pChild[1]));
    }

    // AVL: dir=1 -> right, dir=0 -> left - unifica rotate_right y rotate_left
    Node* rotate(Node* node, size_t dir) {
        size_t opp = 1 - dir;
        Node* pivot  = node->m_pChild[opp];
        Node* middle = pivot->m_pChild[dir];
        pivot->m_pChild[dir] = node;
        node->m_pChild[opp]  = middle;
        update_height(node);
        update_height(pivot);
        return pivot;
    }

};

#endif // __AVLTREE_H__
