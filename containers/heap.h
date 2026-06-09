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
// El arreglo interno de Heap usa Trait::Node como tipo de elemento
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
    Node   *m_data;
    size_t  m_size;
    size_t  m_capacity;
    Comp                 m_comp;
    mutable shared_mutex m_mtx;

    void resize() {
        m_capacity = m_capacity * 2;
        Node *nd   = new Node[m_capacity];
        for (size_t i = 0; i < m_size; ++i) nd[i] = m_data[i];
        delete[] m_data;
        m_data = nd;
    }

    // indices del heap implicito en arreglo
    size_t parent(size_t i) const { return (i - 1) / 2; }
    size_t left(size_t i)   const { return 2 * i + 1;   }
    size_t right(size_t i)  const { return 2 * i + 2;   }

    void swap_nodes(size_t i, size_t j) {
        Node tmp  = m_data[i];
        m_data[i] = m_data[j];
        m_data[j] = tmp;
    }

    // Tarea Heaps: heapifyUp
    void heapifyUp(size_t i) {
        while (i > 0 && m_comp(m_data[i].m_data, m_data[parent(i)].m_data)) {
            swap_nodes(i, parent(i));
            i = parent(i);
        }
    }

    // Tarea Heaps: heapifyDown
    void heapifyDown(size_t i) {
        size_t best = i;
        size_t l    = left(i);
        size_t r    = right(i);
        if (l < m_size && m_comp(m_data[l].m_data, m_data[best].m_data)) best = l;
        if (r < m_size && m_comp(m_data[r].m_data, m_data[best].m_data)) best = r;
        if (best != i) { swap_nodes(i, best); heapifyDown(best); }
    }

public:
    // constructores
    Heap(size_t capacity = 16)
        : m_data(new Node[capacity]), m_size(0), m_capacity(capacity), m_comp() {}

    Heap(const Heap& other) : m_data(nullptr), m_size(0), m_capacity(0), m_comp() {
        shared_lock<shared_mutex> lock(other.m_mtx);
        m_capacity = other.m_capacity;
        m_size     = other.m_size;
        m_data     = new Node[m_capacity];
        for (size_t i = 0; i < m_size; ++i) m_data[i] = other.m_data[i];
    }

    Heap(Heap&& other) noexcept : m_data(nullptr), m_size(0), m_capacity(0), m_comp() {
        unique_lock<shared_mutex> lock(other.m_mtx);
        m_capacity       = other.m_capacity;
        m_size           = other.m_size;
        m_data           = other.m_data;
        other.m_capacity = 0;
        other.m_size     = 0;
        other.m_data     = nullptr;
    }

    virtual ~Heap() { delete[] m_data; }

    // Tarea Heaps: insert
    void insert(value_type value, Ref ref) {
        unique_lock<shared_mutex> lock(m_mtx);
        if (m_size == m_capacity) resize();
        m_data[m_size] = Node(value, ref);
        heapifyUp(m_size);
        ++m_size;
    }

    // Tarea Heaps: extract
    // Extrae el elemento de mayor o menor prioridad (depende del heap)
    tuple<value_type, Ref> extract() {
        unique_lock<shared_mutex> lock(m_mtx);
        if (m_size == 0) throw runtime_error("heap vacio");
        auto result = make_tuple(m_data[0].m_data, m_data[0].m_ref);
        m_data[0]   = m_data[m_size - 1];
        --m_size;
        if (m_size > 0) heapifyDown(0);
        return result;
    }

    // Tarea Heaps: peek
    // Obtiene el elemento de mayor o menor prioridad (depende del heap)
    // sin removerlo
    tuple<value_type, Ref> peek() const {
        shared_lock<shared_mutex> lock(m_mtx);
        if (m_size == 0) throw runtime_error("heap vacio");
        return make_tuple(m_data[0].m_data, m_data[0].m_ref);
    }

    bool   isEmpty() const { shared_lock<shared_mutex> lock(m_mtx); return m_size == 0; }
    size_t size()    const { shared_lock<shared_mutex> lock(m_mtx); return m_size; }

    template<typename Func, typename... Args>
    void forEach(Func func, Args&&... args) {
        shared_lock<shared_mutex> lock(m_mtx);
        for (size_t i = 0; i < m_size; ++i)
            func(m_data[i].m_data, forward<Args>(args)...);
    }

    Node* begin() { return m_data; }
    Node* end()   { return m_data + m_size; }

    // representacion visual del arbol por niveles
    string treeToString() const {
        shared_lock<shared_mutex> lock(m_mtx);
        if (m_size == 0) return "  (vacio)\n";
        ostringstream oss;
        size_t level_start = 0;
        size_t level_size  = 1;
        while (level_start < m_size) {
            oss << "  ";
            size_t end = (level_start + level_size < m_size) ? level_start + level_size : m_size;
            for (size_t i = level_start; i < end; ++i)
                oss << m_data[i].m_data << " ";
            oss << "\n";
            level_start += level_size;
            level_size  *= 2;
        }
        return oss.str();
    }

    // Tarea Heaps: toString - formato [(val,ref),...] igual que el resto de contenedores
    string toString() const {
        shared_lock<shared_mutex> lock(m_mtx);
        ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < m_size; ++i) {
            if (i > 0) oss << ",";
            oss << m_data[i];
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
