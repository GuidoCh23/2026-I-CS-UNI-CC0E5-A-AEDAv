#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#include <iostream>
#include <cstddef> // size_t
#include <string>
#include <sstream>
#include <mutex>
#include <shared_mutex> // shared_mutex
#include "general_iterator.h"
#include "util.h"
#include "../types.h"
using namespace std;

// t4-impl forward it(op++): operator++ avanza al siguiente nodo
template <typename Container>
class LinkedListForwardIterator : public general_iterator<Container, LinkedListForwardIterator<Container>>{
    using MySelf = LinkedListForwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
public:
    MySelf operator++(){ this->m_pNode = this->m_pNode->getNext(); return *this; }
};

// Linked List Node
template <typename T>
class LLNode{
    using Node = LLNode<T>;
private:
    T   m_data;
    Ref  m_ref;
    Node *m_next;
public:
    LLNode() : m_data(T()), m_ref(Ref()), m_next(nullptr) {}
    LLNode(T data, Ref ref) : m_data(data), m_ref(ref), m_next(nullptr) {}
    LLNode(T data, Ref ref, Node *next) : m_data(data), m_ref(ref), m_next(next) {}
    virtual ~LLNode() {}

    T      getData() const { return m_data; }
    T&     getDataRef()    { return m_data; }
    void   setData(T data) { m_data = data; }
    Ref    getRef()  const { return m_ref; }
    void   setRef(Ref ref) { m_ref = ref; }
    Node*  getNext() const { return m_next; }
    Node*& getNextRef()    { return m_next; }
    void   setNext(Node *next) { m_next = next; }
};

template <typename T>
struct AscendingLinkedListTrait{
    using value_type = T;
    using Node = LLNode<T>;
    using Comp = less<T>;
};

template <typename T>
struct DescendingLinkedListTrait{
    using value_type = T;
    using Node = LLNode<T>;
    using Comp = greater<T>;
};

template <typename Trait>
class LinkedList{
public:
    using value_type = typename Trait::value_type;
    using Node       = typename Trait::Node;
    using Comp       = typename Trait::Comp;
    using MySelf     = LinkedList<Trait>;

    using forward_iterator = LinkedListForwardIterator<MySelf>;
    // friend forward_iterator;

private:
    Node *m_pRoot = nullptr;
    Node *m_tail = nullptr;
    size_t m_size = 0;
    Comp   m_comp;
    mutable shared_mutex m_mtx;
public:
    LinkedList() {}
    // t1: copy constructor
    LinkedList(const LinkedList &other);
    // t2: move constructor
    LinkedList(LinkedList &&other);

    LinkedList& operator=(const LinkedList &other);
    LinkedList& operator=(LinkedList &&other);
    // t3-destructor seguro: libera todos los nodos
    virtual        ~LinkedList();
    virtual void    push_front(value_type value, Ref ref);
    virtual void    pop_front();
    virtual void    push_back(value_type value, Ref ref);
    virtual void    pop_back();
private:
            void    internal_insert(Node* &pPrev, const value_type &value, Ref ref);

public:
    virtual void    insert(const value_type &value, Ref ref);

    virtual value_type& operator[](size_t index);
    virtual size_t  size() const;
    virtual string  toString() const;

    forward_iterator begin() { return forward_iterator(this, m_pRoot); }
    forward_iterator end()   { return forward_iterator(this, nullptr); }

    // t12-foreach: foreach con control concurrente
    template <typename Func, typename... Args>
    void ForEach(Func func, Args &&...  args){
        unique_lock<shared_mutex> lock(m_mtx);
        ::ForEach(begin(), end(), func, std::forward<Args>(args)... );
    }
};

// t1-copy constructor: copia nodo a nodo preservando el orden
template <typename Trait>
LinkedList<Trait>::LinkedList(const LinkedList &other){
    Node *curr = other.m_pRoot;
    while(curr){
        Node *newNode = new Node(curr->getData(), curr->getRef());
        if(!m_pRoot){ 
            m_pRoot = newNode;
            m_tail = newNode; 
        }else{ 
            m_tail->setNext(newNode);
            m_tail = newNode; 
        }
        curr = curr->getNext();
        m_size++;
    }
}

// t2-move constructor: transfiere propiedad de los nodos
template <typename Trait>
LinkedList<Trait>::LinkedList(LinkedList &&other){
    m_pRoot = other.m_pRoot;
    m_tail  = other.m_tail;
    m_size  = other.m_size;
    other.m_pRoot = nullptr;
    other.m_tail  = nullptr;
    other.m_size  = 0;
}

template <typename Trait>
LinkedList<Trait>& LinkedList<Trait>::operator=(const LinkedList &other){
    if(this == &other) return *this;
    Node *curr = m_pRoot;
    while(curr){ 
        Node *next = curr->getNext();
        delete curr;
        curr = next; 
    }
    m_pRoot = nullptr; m_tail = nullptr; m_size = 0;
    curr = other.m_pRoot;
    while(curr){
        Node *newNode = new Node(curr->getData(), curr->getRef());
        if(!m_pRoot){ 
            m_pRoot = newNode;
            m_tail = newNode;
        }else{
            m_tail->setNext(newNode);
            m_tail = newNode;
        }
        curr = curr->getNext();
        m_size++;
    }
    return *this;
}

template <typename Trait>
LinkedList<Trait>& LinkedList<Trait>::operator=(LinkedList &&other){
    if(this == &other) return *this;
    Node *curr = m_pRoot;
    while(curr){
        Node *next = curr->getNext();
        delete curr;
        curr = next; 
    }
    m_pRoot = other.m_pRoot;
    m_tail = other.m_tail;
    m_size = other.m_size;
    other.m_pRoot = nullptr;
    other.m_tail = nullptr;
    other.m_size = 0;
    return *this;
}

// t3-destructor seguro: recorre y elimina cada nodo
template <typename Trait>
LinkedList<Trait>::~LinkedList(){
    Node *curr = m_pRoot;
    while(curr){
        Node *next = curr->getNext();
        delete curr;
        curr = next;
    }
}

// t5-push_front: inserta al inicio sin mantener orden
template <typename Trait>
void LinkedList<Trait>::push_front(value_type value, Ref ref){
    unique_lock<shared_mutex> lock(m_mtx);
    Node *newNode = new Node(value, ref, m_pRoot);
    m_pRoot = newNode;
    if(!m_tail) m_tail = m_pRoot;
    m_size++;
}

// t6-pop_front: elimina el nodo del inicio
template <typename Trait>
void LinkedList<Trait>::pop_front(){
    unique_lock<shared_mutex> lock(m_mtx);
    if(!m_pRoot) return;
    Node *toDelete = m_pRoot;
    m_pRoot = m_pRoot->getNext();
    if(!m_pRoot) m_tail = nullptr;
    delete toDelete;
    m_size--;
}

// t7-push_back: inserta al final sin mantener orden
template <typename Trait>
void LinkedList<Trait>::push_back(value_type value, Ref ref){
    unique_lock<shared_mutex> lock(m_mtx);
    Node *newNode = new Node(value, ref);
    if(!m_tail){ 
        m_pRoot = newNode;
        m_tail = newNode;
    }else{
        m_tail->setNext(newNode);
        m_tail = newNode;
    }
    m_size++;
}

// t8-pop_back: elimina el nodo del final
template <typename Trait>
void LinkedList<Trait>::pop_back(){
    unique_lock<shared_mutex> lock(m_mtx);
    if(!m_pRoot) return;
    if(m_pRoot == m_tail){
        delete m_pRoot;
        m_pRoot = nullptr; m_tail = nullptr;
        m_size--;
        return;
    }
    Node *curr = m_pRoot;
    while(curr->getNext() != m_tail)
        curr = curr->getNext();
    delete m_tail;
    m_tail = curr;
    m_tail->setNext(nullptr);
    m_size--;
}

template <typename Trait>
void LinkedList<Trait>::internal_insert(Node* &pPrev, const value_type &value, Ref ref){
    if(!pPrev || m_comp(value, pPrev->getDataRef())){
        pPrev = new Node(value, ref, pPrev);
        m_size++;
        // si el nuevo nodo quedo al final, actualizar tail
        if(pPrev->getNext() == nullptr)
            m_tail = pPrev;
        return;
    }
    internal_insert(pPrev->getNextRef(), value, ref);
}

template <typename Trait>
void LinkedList<Trait>::insert(const value_type &value, Ref ref){
    unique_lock<shared_mutex> lock(m_mtx);
    internal_insert(m_pRoot, value, ref);
}

// t9-operator[]: acceso por indice
template <typename Trait>
typename LinkedList<Trait>::value_type& LinkedList<Trait>::operator[](size_t index){
    shared_lock<shared_mutex> lock(m_mtx);
    Node *curr = m_pRoot;
    for(size_t i = 0; i < index; i++)
        curr = curr->getNext();
    return curr->getDataRef();
}

template <typename Trait>
size_t LinkedList<Trait>::size() const{
    shared_lock<shared_mutex> lock(m_mtx);
    return m_size;
}

template <typename Trait>
string LinkedList<Trait>::toString() const{
    shared_lock<shared_mutex> lock(m_mtx);
    ostringstream oss;
    oss << "[";
    Node *curr = m_pRoot;
    bool first = true;
    while(curr){
        if(!first) oss << ",";
        oss << "(" << curr->getData() << "," << curr->getRef() << ")";
        curr = curr->getNext();
        first = false;
    }
    oss << "]";
    return oss.str();
}

// t11-operator<<:imprime la lista usando toString
template <typename Trait>
ostream& operator<<(ostream& os, const LinkedList<Trait>& list){
    return os << list.toString();
}

// t10-operator>>: lee el formato [(data, ref), ...] producido por toString
template <typename Trait>
istream& operator>>(istream& is, LinkedList<Trait>& list){
    using value_type = typename Trait::value_type;
    char ch;
    if(!(is >> ch) || ch != '[') return is;
    while(is >> ch && ch != ']'){
        if(ch != '('){ 
            is.setstate(ios::failbit);
            return is;
        }
        value_type data; Ref ref;
        is >> data >> ch >> ref;  // ch consume la coma entre data y ref
        if(!(is >> ch) || ch != ')'){
            is.setstate(ios::failbit);
            return is; 
        }
        list.insert(data, ref);
        is >> ch;  // consume ',' o ']'
        if(ch == ']') break;
    }
    return is;
}

void LinkedListDemo();
void ListsDemo();

#endif // __LINKEDLIST_H__