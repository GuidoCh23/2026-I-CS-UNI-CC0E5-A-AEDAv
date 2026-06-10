#ifndef __HEAP_H__
#define __HEAP_H__

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <tuple>
#include <shared_mutex>
#include <mutex>
#include <utility>
#include "../types.h"
#include "traits.h"
#include "vector.h"
using namespace std;

// HeapNode - nodo del heap, compatible con BaseTrait
template<typename T>
struct HeapNode {
    using value_type = T;
    T   m_data;
    Ref m_ref;
    HeapNode() : m_data(T{}), m_ref(Ref{}) {}
    HeapNode(T data, Ref ref) : m_data(data), m_ref(ref) {}
    friend ostream& operator<<(ostream& os, const HeapNode& n) {
        return os << "(" << n.m_data << "," << n.m_ref << ")";
    }
};

// Tarea Heaps: Adaptar Vector a Trait
// MinHeapTrait y MaxHeapTrait usan BaseTrait<HeapNode<T>, Comp>
// El almacenamiento interno del Heap es Vector<Trait>, reutilizando Vector adaptado
template<typename T>
struct MinHeapTrait : public BaseTrait<HeapNode<T>, less<T>> {};

template<typename T>
struct MaxHeapTrait : public BaseTrait<HeapNode<T>, greater<T>> {};

template<typename Trait>
class Heap {
public:
    using value_type = typename Trait::value_type;
    using Comp       = typename Trait::Comp;
    using Node       = typename Trait::Node;
    using MySelf     = Heap<Trait>;

private:
    // Tarea Heaps: Adaptar Vector a Trait - Vector<Trait> como almacenamiento interno
    Vector<Trait>        m_vec;
    Comp                 m_comp;
    mutable shared_mutex m_mtx;

    // indices del heap implicito en arreglo
    size_t parent(size_t i) const { return (i - 1) / 2; }
    size_t left(size_t i)   const { return 2 * i + 1;   }
    size_t right(size_t i)  const { return 2 * i + 2;   }

    void swap_nodes(size_t i, size_t j) {
        Node tmp  = m_vec[i];
        m_vec[i]  = m_vec[j];
        m_vec[j]  = tmp;
    }

    // Tarea Heaps: heapifyUp
    void heapifyUp(size_t i) {
        while (i > 0 && m_comp(m_vec[i].m_data, m_vec[parent(i)].m_data)) {
            swap_nodes(i, parent(i));
            i = parent(i);
        }
    }

    // Tarea Heaps: heapifyDown
    void heapifyDown(size_t i) {
        size_t sz   = m_vec.size();
        size_t best = i;
        size_t l    = left(i);
        size_t r    = right(i);
        if (l < sz && m_comp(m_vec[l].m_data, m_vec[best].m_data)) best = l;
        if (r < sz && m_comp(m_vec[r].m_data, m_vec[best].m_data)) best = r;
        if (best != i) { swap_nodes(i, best); heapifyDown(best); }
    }

public:
    Heap(size_t capacity = 16) : m_vec(capacity), m_comp() {}

    Heap(const Heap& other) : m_vec(), m_comp() {
        shared_lock<shared_mutex> lock(other.m_mtx);
        m_vec = other.m_vec;
    }

    Heap(Heap&& other) noexcept : m_vec(), m_comp() {
        unique_lock<shared_mutex> lock(other.m_mtx);
        m_vec = move(other.m_vec);
    }

    virtual ~Heap() {}

    // Tarea Heaps: insert
    void insert(value_type value, Ref ref) {
        unique_lock<shared_mutex> lock(m_mtx);
        m_vec.push_back(value, ref);
        heapifyUp(m_vec.size() - 1);
    }

    // Tarea Heaps: extract
    // Extrae el elemento de mayor o menor prioridad (depende del heap)
    tuple<value_type, Ref> extract() {
        unique_lock<shared_mutex> lock(m_mtx);
        if (m_vec.size() == 0) throw runtime_error("heap vacio");
        auto result  = make_tuple(m_vec[0].m_data, m_vec[0].m_ref);
        size_t last  = m_vec.size() - 1;
        m_vec[0]     = m_vec[last];
        m_vec.pop_back();
        if (m_vec.size() > 0) heapifyDown(0);
        return result;
    }

    // Tarea Heaps: peek
    // Obtiene el elemento de mayor o menor prioridad (depende del heap)
    // sin removerlo
    tuple<value_type, Ref> peek() const {
        shared_lock<shared_mutex> lock(m_mtx);
        if (m_vec.size() == 0) throw runtime_error("heap vacio");
        return make_tuple(m_vec[0].m_data, m_vec[0].m_ref);
    }

    bool   isEmpty() const { shared_lock<shared_mutex> lock(m_mtx); return m_vec.size() == 0; }
    size_t size()    const { shared_lock<shared_mutex> lock(m_mtx); return m_vec.size(); }

    template<typename Func, typename... Args>
    void forEach(Func func, Args&&... args) {
        shared_lock<shared_mutex> lock(m_mtx);
        size_t sz = m_vec.size();
        for (size_t i = 0; i < sz; ++i)
            func(m_vec[i].m_data, forward<Args>(args)...);
    }

    Node* begin() { return m_vec.data(); }
    Node* end()   { return m_vec.data() + m_vec.size(); }

    // representacion visual del arbol por niveles
    string treeToString() const {
        shared_lock<shared_mutex> lock(m_mtx);
        size_t sz = m_vec.size();
        if (sz == 0) return "  (vacio)\n";
        ostringstream oss;
        size_t level_start = 0;
        size_t level_size  = 1;
        while (level_start < sz) {
            oss << "  ";
            size_t end = (level_start + level_size < sz) ? level_start + level_size : sz;
            for (size_t i = level_start; i < end; ++i)
                oss << m_vec[i].m_data << " ";
            oss << "\n";
            level_start += level_size;
            level_size  *= 2;
        }
        return oss.str();
    }

    // Tarea Heaps: toString - formato [(val,ref),...] igual que el resto de contenedores
    string toString() const {
        shared_lock<shared_mutex> lock(m_mtx);
        size_t sz = m_vec.size();
        ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < sz; ++i) {
            if (i > 0) oss << ",";
            oss << m_vec[i];
        }
        oss << "]";
        return oss.str();
    }

    // Tarea Heaps: operator<<
    friend ostream& operator<<(ostream& os, const Heap& h) {
        return os << h.toString();
    }

    // Tarea Heaps: operator>>
    friend istream& operator>>(istream& is, Heap& h) {
        char ch;
        if (!(is >> ch) || ch != '[') { is.clear(ios_base::failbit); return is; }
        value_type val; Ref ref; char comma, paren;
        while (is >> ch && ch != ']')
            if (ch == '(')
                if (is >> val >> comma >> ref >> paren)
                    if (comma == ',' && paren == ')')
                        h.insert(val, ref);
        return is;
    }
};

#endif // __HEAP_H__
