#ifndef __STACK_H__
#define __STACK_H__

#include <shared_mutex>
#include "vector.h"
using namespace std;

// Reutilizacion: Stack sobre Vector<T> para almacenar nodos en recorridos de BinaryTree
template<typename T>
class Stack {
    Vector<T>            m_vec;
    mutable shared_mutex m_mtx;
public:
    Stack(size_t capacity = 64) : m_vec(capacity) {}
    Stack(const Stack& other)   : m_vec(other.m_vec) {}
    Stack(Stack&& other) noexcept : m_vec(move(other.m_vec)) {}
    Stack& operator=(const Stack& other) {
        if (this != &other) m_vec = other.m_vec;
        return *this;
    }
    Stack& operator=(Stack&& other) noexcept {
        if (this != &other) m_vec = move(other.m_vec);
        return *this;
    }

    void   push(T val)          { unique_lock<shared_mutex> lock(m_mtx); m_vec.push_back(val, 0); }
    T      operator[](size_t i) const { shared_lock<shared_mutex> lock(m_mtx); return m_vec[i]; }
    bool   empty()         const { return m_vec.size() == 0; }
    size_t size()          const { return m_vec.size(); }
};

#endif // __STACK_H__
