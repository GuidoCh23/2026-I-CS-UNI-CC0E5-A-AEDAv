#ifndef __DOUBLELINKEDLIST_H__
#define __DOUBLELINKEDLIST_H__

#include "linkedlist.h"

// DLLNode: extiende LLNode con puntero al nodo anterior 
template <typename T>
class DLLNode : public LLNode<T, DLLNode<T>> {
    using Base = LLNode<T, DLLNode<T>>;
public:
    using Node = DLLNode<T>;
private:
    Node *m_pPrev;
public:
    DLLNode() : Base(), m_pPrev(nullptr) {}
    DLLNode(T data, Ref ref, Node *next = nullptr, Node *prev = nullptr)
        : Base(data, ref, next), m_pPrev(prev) {}

    Node*  getPrev() const     { return m_pPrev; }
    void   setPrev(Node *prev) { m_pPrev = prev; }
    Node*& getPrevRef()        { return m_pPrev; }
};

// Tarea: Adaptar Traits a BaseTrait (DLL)
template <typename T>
struct AscendingDLLTrait : BaseTrait<T, less<T>, DLLNode<T>>{};

template <typename T>
struct DescendingDLLTrait : BaseTrait<T, greater<T>, DLLNode<T>>{};

// Tarea: Crear DoubleLinkedListForwardIterator
template <typename Container>
class DLLForwardIterator
    : public general_iterator<Container, DLLForwardIterator<Container>>{
public:
    using MySelf = DLLForwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;

    MySelf operator++() {
        if (this->m_pNode)
            this->m_pNode = this->m_pNode->getNext();
        return *this;
    }
};

// Tarea: Crear DoubleLinkedListBackwardIterator
template <typename Container>
class DLLBackwardIterator
    : public general_iterator<Container, DLLBackwardIterator<Container>> {
public:
    using MySelf = DLLBackwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;

    MySelf operator++() {
        if (this->m_pNode)
            this->m_pNode = this->m_pNode->getPrev();
        return *this;
    }
};

template <typename Trait>
class DoubleLinkedList : public LinkedList<Trait> {
public:
    using Base       = LinkedList<Trait>;
    using value_type = typename Trait::value_type;
    using Node       = typename Trait::Node;
    using MySelf     = DoubleLinkedList<Trait>;

    using forward_iterator  = DLLForwardIterator<MySelf>;
    using backward_iterator = DLLBackwardIterator<MySelf>;
    friend forward_iterator;
    friend backward_iterator;

private:
    // inserta al final manteniendo m_pPrev
    void link_push_back(const value_type &value, Ref ref);
    // libera todos los nodos
    void unlink_all();

public:
    DoubleLinkedList() : Base() {}
    DoubleLinkedList(const DoubleLinkedList &other);
    DoubleLinkedList(DoubleLinkedList &&other) noexcept;
    DoubleLinkedList& operator=(const DoubleLinkedList &other);
    DoubleLinkedList& operator=(DoubleLinkedList &&other) noexcept;
    // Destructor Seguro
    virtual ~DoubleLinkedList() {
        unique_lock<shared_mutex> lock(this->m_mtx);
        unlink_all();
    }

    void insert(const value_type &value, Ref ref) override;
    void push_front(value_type value, Ref ref) override;
    std::tuple<value_type, Ref> pop_front() override;
    void push_back(value_type value, Ref ref) override;
    std::tuple<value_type, Ref> pop_back() override;

    // Tarea: usar en bucle nativo foreach — begin/end con tipo DLL correcto
    forward_iterator  begin()  { return forward_iterator(this, this->m_pRoot); }
    forward_iterator  end()    { return forward_iterator(this, nullptr); }
    backward_iterator rbegin() { return backward_iterator(this, this->m_tail); }
    backward_iterator rend()   { return backward_iterator(this, nullptr); }

    template <typename Func, typename... Args>
    void ReverseForEach(Func func, Args&&... args) {
        unique_lock<shared_mutex> lock(this->m_mtx);
        for (auto it = rbegin(); it != rend(); ++it)
            func(*it, std::forward<Args>(args)...);
    }

    // Mejora libre #2: devuelve el indice de la primera ocurrencia o -1 si no existe
    int contains(const value_type &value) const {
        unique_lock<shared_mutex> lock(this->m_mtx);
        int index = 0;
        for (Node* curr = this->m_pRoot; curr; curr = curr->getNext(), ++index)
            if (curr->getData() == value) return index;
        return -1;
    }

    // Mejora libre #1: invierte la lista intercambiando next y prev de cada nodo
    void reverse() {
        unique_lock<shared_mutex> lock(this->m_mtx);
        if (this->m_size <= 1) return;
        Node* curr = this->m_pRoot;
        while (curr) {
            Node* tmp = curr->getNext();
            curr->setNext(curr->getPrev());
            curr->setPrev(tmp);
            curr = tmp;
        }
        std::swap(this->m_pRoot, this->m_tail);
    }

    // Tarea: operator<<
    friend ostream& operator<<(ostream& os, const DoubleLinkedList& list) {
        unique_lock<shared_mutex> lock(list.m_mtx);
        os << "[";
        bool first = true;
        for (Node* curr = list.m_pRoot; curr; curr = curr->getNext()) {
            if (!first) os << ",";
            os << "(" << curr->getData() << "," << curr->getRef() << ")";
            first = false;
        }
        os << "]";
        return os;
    }

    // Tarea: operator>>
    friend istream& operator>>(istream& is, DoubleLinkedList& list) {
        unique_lock<shared_mutex> lock(list.m_mtx);
        char ch;
        if (!(is >> ch) || ch != '[') { 
            is.setstate(ios_base::failbit);
            return is; 
        }
        value_type val;
        Ref ref;
        char comma, close;
        while (is >> ch && ch != ']') {
            if (ch == '(') {
                if (is >> val >> comma >> ref >> close && comma == ',' && close == ')')
                    list.link_push_back(val, ref); // lock ya tomado, no pasar por push_back
            }
        }
        return is;
    }
};

template <typename Trait>
void DoubleLinkedList<Trait>::link_push_back(const value_type &value, Ref ref) {
    Node* n = new Node(value, ref, nullptr, this->m_tail);
    if (this->m_size == 0) this->m_pRoot = this->m_tail = n;
    else                  { this->m_tail->setNext(n); this->m_tail = n; }
    this->m_size++;
}

template <typename Trait>
void DoubleLinkedList<Trait>::unlink_all() {
    Node* curr = this->m_pRoot;
    while (curr) {
        Node* nx = curr->getNext();
        delete curr;
        curr = nx;
    }
    this->m_pRoot = nullptr;
    this->m_tail  = nullptr;
    this->m_size  = 0;
}

// TODO: Copy constructor
//       Simplificar y abstraer el bucle de copia de Nodes
//       Es posible que no necesites este constructor ya que lo heredaste
template <typename Trait>
DoubleLinkedList<Trait>::DoubleLinkedList(const DoubleLinkedList &other) : Base() {
    unique_lock<shared_mutex> lock(other.m_mtx);
    for (Node* curr = other.m_pRoot; curr; curr = curr->getNext())
        link_push_back(curr->getData(), curr->getRef());
}

// TODO: Move constructor
template <typename Trait>
DoubleLinkedList<Trait>::DoubleLinkedList(DoubleLinkedList &&other) noexcept : Base() {
    unique_lock<shared_mutex> lock(other.m_mtx);
    this->m_pRoot = std::exchange(other.m_pRoot, nullptr);
    this->m_tail  = std::exchange(other.m_tail,  nullptr);
    this->m_size  = std::exchange(other.m_size,  0);
}

// TODO: Copy assignment operator
template <typename Trait>
DoubleLinkedList<Trait>& DoubleLinkedList<Trait>::operator=(const DoubleLinkedList &other) {
    if (this == &other) return *this;
    unique_lock<shared_mutex> lk1(this->m_mtx);
    unique_lock<shared_mutex> lk2(other.m_mtx);
    unlink_all();
    for (Node* curr = other.m_pRoot; curr; curr = curr->getNext())
        link_push_back(curr->getData(), curr->getRef());
    return *this;
}

// TODO: Move assignment operator
template <typename Trait>
DoubleLinkedList<Trait>& DoubleLinkedList<Trait>::operator=(DoubleLinkedList &&other) noexcept {
    if (this == &other) return *this;
    unique_lock<shared_mutex> lk1(this->m_mtx);
    unique_lock<shared_mutex> lk2(other.m_mtx);
    unlink_all();
    this->m_pRoot = std::exchange(other.m_pRoot, nullptr);
    this->m_tail  = std::exchange(other.m_tail,  nullptr);
    this->m_size  = std::exchange(other.m_size,  0);
    return *this;
}

template <typename Trait>
void DoubleLinkedList<Trait>::push_front(value_type value, Ref ref) {
    unique_lock<shared_mutex> lock(this->m_mtx);
    Node* n = new Node(value, ref, this->m_pRoot, nullptr);
    if (this->m_size == 0) this->m_tail = n;
    else                   this->m_pRoot->setPrev(n);
    this->m_pRoot = n;
    this->m_size++;
}

template <typename Trait>
std::tuple<typename DoubleLinkedList<Trait>::value_type, Ref>
DoubleLinkedList<Trait>::pop_front() {
    unique_lock<shared_mutex> lock(this->m_mtx);
    if (!this->m_pRoot) throw out_of_range("lista vacia");
    Node* old = this->m_pRoot;
    auto result = std::make_tuple(old->getData(), old->getRef());
    this->m_pRoot = old->getNext();
    if (this->m_pRoot) this->m_pRoot->setPrev(nullptr);
    else this->m_tail = nullptr;
    delete old;
    this->m_size--;
    return result;
}

template <typename Trait>
void DoubleLinkedList<Trait>::push_back(value_type value, Ref ref) {
    unique_lock<shared_mutex> lock(this->m_mtx);
    link_push_back(value, ref);
}

template <typename Trait>
std::tuple<typename DoubleLinkedList<Trait>::value_type, Ref>
DoubleLinkedList<Trait>::pop_back() {
    unique_lock<shared_mutex> lock(this->m_mtx);
    if (!this->m_tail) throw out_of_range("lista vacia");
    Node* old = this->m_tail;
    auto result = std::make_tuple(old->getData(), old->getRef());
    this->m_tail = old->getPrev();
    if (this->m_tail) this->m_tail->setNext(nullptr);
    else this->m_pRoot = nullptr;
    delete old;
    this->m_size--;
    return result;
}

// insert: insercion ordenada manteniendo m_pPrev en cada nodo
template <typename Trait>
void DoubleLinkedList<Trait>::insert(const value_type &value, Ref ref) {
    unique_lock<shared_mutex> lock(this->m_mtx);
    Node* prev = nullptr;
    Node* curr = this->m_pRoot;
    while (curr && !this->m_comp(value, curr->getDataRef())) {
        prev = curr;
        curr = curr->getNext();
    }
    Node* n = new Node(value, ref, curr, prev);
    if (prev) prev->setNext(n);
    else this->m_pRoot = n;
    if (curr) curr->setPrev(n);
    else this->m_tail = n;
    this->m_size++;
}

#endif // __DOUBLELINKEDLIST_H__
