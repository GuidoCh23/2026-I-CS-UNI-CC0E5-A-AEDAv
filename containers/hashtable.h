#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

#include <iostream>
#include <shared_mutex>
#include "avl.h"
#include "../types.h"
using namespace std;

// Tarea HashTable(AVL): KVPair - par clave/valor, comparacion solo por key
template<typename K, typename V>
struct KVPair {
    K first;
    V second;
    KVPair() : first(K{}), second(V{}) {}
    KVPair(const K& k, const V& v) : first(k), second(v) {}
    bool operator< (const KVPair& o) const { return first <  o.first; }
    bool operator> (const KVPair& o) const { return first >  o.first; }
    bool operator==(const KVPair& o) const { return first == o.first; }
    friend ostream& operator<<(ostream& os, const KVPair& kv) {
        return os << kv.first << ":" << kv.second;
    }
    friend istream& operator>>(istream& is, KVPair& kv) {
        char sep;
        is >> kv.first >> sep >> kv.second;
        return is;
    }
};

// Tarea HashTable(AVL): HashTableTrait - trait que contiene Key y Value
template<typename K, typename V>
struct HashTableTrait : AscendingTrait<AVLNode<KVPair<K,V>>> {
    using key_type    = K;
    using mapped_type = V;
};

// Tarea HashTable(AVL): HashTable hereda de AVL - reutiliza <<, >>, copy, move, begin/end
template<typename K, typename V>
class HashTable : public AVL<HashTableTrait<K,V>> {
public:
    using Base        = AVL<HashTableTrait<K,V>>;
    using key_type    = K;
    using mapped_type = V;
    using kv_type     = KVPair<K,V>;
    using Node        = typename Base::Node;

    HashTable() = default;

    // Tarea HashTable(AVL): Constructor copia
    HashTable(const HashTable& other) : Base(other) {}

    // Tarea HashTable(AVL): Move constructor
    HashTable(HashTable&& other) noexcept : Base(move(other)) {}

    HashTable& operator=(const HashTable& other) {
        if (this != &other) {
            unique_lock<shared_mutex> lock(this->m_mtx);
            shared_lock<shared_mutex> olock(other.m_mtx);
            this->internal_clear(this->m_pRoot);
            this->m_pRoot = this->internal_copy(other.m_pRoot);
            this->m_size  = other.m_size;
        }
        return *this;
    }

    HashTable& operator=(HashTable&& other) noexcept {
        if (this != &other) {
            unique_lock<shared_mutex> lock(this->m_mtx);
            unique_lock<shared_mutex> olock(other.m_mtx);
            this->internal_clear(this->m_pRoot);
            this->m_pRoot = exchange(other.m_pRoot, nullptr);
            this->m_size  = exchange(other.m_size,  size_t{});
        }
        return *this;
    }

    // Tarea HashTable(AVL): m[key] = value
    mapped_type& operator[](const key_type& key) {
        unique_lock<shared_mutex> lock(this->m_mtx);
        kv_type probe{key, mapped_type{}};
        Node* found = this->internal_search(this->m_pRoot, probe);
        if (found) return found->m_data.second;
        this->m_pRoot = this->internal_insert(this->m_pRoot, probe, Ref{});
        ++this->m_size;
        found = this->internal_search(this->m_pRoot, probe);
        return found->m_data.second;
    }

    void insert(const key_type& key, const mapped_type& value) {
        (*this)[key] = value;
    }

    mapped_type& at(const key_type& key) {
        shared_lock<shared_mutex> lock(this->m_mtx);
        kv_type probe{key, mapped_type{}};
        Node* found = this->internal_search(this->m_pRoot, probe);
        if (!found) throw runtime_error("key no encontrada");
        return found->m_data.second;
    }

    Node* find(const key_type& key) const {
        shared_lock<shared_mutex> lock(this->m_mtx);
        kv_type probe{key, mapped_type{}};
        return this->internal_search(this->m_pRoot, probe);
    }

    bool isEmpty() const { return this->size() == 0; }

    template<typename Func, typename... Args>
    void forEach(Func func, Args&&... args) {
        shared_lock<shared_mutex> lock(this->m_mtx);
        vector<Node*> nodes;
        this->fill_inorder(this->m_pRoot, nodes);
        for (auto* n : nodes)
            func(n->m_data, forward<Args>(args)...);
    }

    // Tarea HashTable(AVL): operator<< - reutiliza toString de BinaryTree
    friend ostream& operator<<(ostream& os, const HashTable& ht) {
        return os << ht.toString();
    }

    // Tarea HashTable(AVL): operator>> - reutiliza operator>> de BinaryTree via upcast
    friend istream& operator>>(istream& is, HashTable& ht) {
        BinaryTree<HashTableTrait<K,V>>& base = ht;
        return operator>>(is, base);
    }
};

#endif // __HASHTABLE_H__
