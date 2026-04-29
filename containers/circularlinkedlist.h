#ifndef __CIRCULARLINKEDLIST_H__
#define __CIRCULARLINKEDLIST_H__

#include <vector>
#include "linkedlist.h"

template <typename T>
struct AscendingCLLTrait : BaseTrait<T, less<T>, LLNode<T>> {};

template <typename T>
struct DescendingCLLTrait : BaseTrait<T, greater<T>, LLNode<T>> {};

// Tarea: Adaptar los recorridos
// para cuando el siguiente nodo coincide con el nodo inicial
template <typename Container>
class CLLForwardIterator
    : public general_iterator<Container, CLLForwardIterator<Container>> {
public:
    using MySelf = CLLForwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Node   = typename Container::Node;
private:
    Node *m_start; // nodo inicial, nullptr si es centinela end()
public:
    CLLForwardIterator(Container *pContainer, Node *pNode)
        : Parent(pContainer, pNode), m_start(pNode) {}

    MySelf operator++() {
        if (this->m_pNode) {
            this->m_pNode = this->m_pNode->getNext();
            if (this->m_pNode == m_start)   // volvio al inicio
                this->m_pNode = nullptr;
        }
        return *this;
    }
};

template <typename Trait>
class CircularLinkedList : public LinkedList<Trait> {
public:
    using Base       = LinkedList<Trait>;
    using value_type = typename Trait::value_type;
    using Node       = typename Trait::Node;
    using MySelf     = CircularLinkedList<Trait>;

    using forward_iterator = CLLForwardIterator<MySelf>;
    friend forward_iterator;

private:
    // inserta al final cerrando el circulo
    void link_push_back(const value_type &value, Ref ref);
    // rompe el circulo y libera todos los nodos
    void unlink_all();

public:
    CircularLinkedList() : Base() {}
    CircularLinkedList(const CircularLinkedList &other);
    CircularLinkedList(CircularLinkedList &&other) noexcept;
    CircularLinkedList& operator=(const CircularLinkedList &other);
    CircularLinkedList& operator=(CircularLinkedList &&other) noexcept;
    // Destructor Seguro
    virtual ~CircularLinkedList() {
        unique_lock<shared_mutex> lock(this->m_mtx);
        unlink_all();
    }

    // Tarea: Adaptar la insercion — mantiene orden y cierra el circulo
    void insert(const value_type &value, Ref ref) override;
    void push_front(value_type value, Ref ref) override;
    std::tuple<value_type, Ref> pop_front() override;
    void push_back(value_type value, Ref ref) override;
    std::tuple<value_type, Ref> pop_back() override;

    forward_iterator begin() { return forward_iterator(this, this->m_pRoot); }
    forward_iterator end()   { return forward_iterator(this, nullptr); }

    // devuelve el indice de la primera ocurrencia o -1 si no existe
    int contains(const value_type &value) const {
        unique_lock<shared_mutex> lock(this->m_mtx);
        if (!this->m_pRoot) return -1;
        int index = 0;
        Node* curr = this->m_pRoot;
        do {
            if (curr->getData() == value) return index;
            curr = curr->getNext();
            ++index;
        } while (curr != this->m_pRoot);
        return -1;
    }

    // recorre en reversa recolectando nodos (CLL no tiene prev)
    template <typename Func, typename... Args>
    void ReverseForEach(Func func, Args&&... args) {
        unique_lock<shared_mutex> lock(this->m_mtx);
        if (!this->m_pRoot) return;
        vector<Node*> nodes;
        Node* curr = this->m_pRoot;
        do {
            nodes.push_back(curr);
            curr = curr->getNext();
        } while (curr != this->m_pRoot);
        for (auto it = nodes.rbegin(); it != nodes.rend(); ++it)
            func((*it)->getDataRef(), std::forward<Args>(args)...);
    }

    friend ostream& operator<<(ostream& os, const CircularLinkedList& list) {
        unique_lock<shared_mutex> lock(list.m_mtx);
        os << "[";
        if (list.m_pRoot) {
            Node* curr = list.m_pRoot;
            bool first = true;
            do {
                if (!first) os << ",";
                os << "(" << curr->getData() << "," << curr->getRef() << ")";
                first = false;
                curr = curr->getNext();
            } while (curr != list.m_pRoot);
        }
        os << "]";
        return os;
    }

    friend istream& operator>>(istream& is, CircularLinkedList& list) {
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
                    list.link_push_back(val, ref); // lock ya tomado, no pasar por insert
            }
        }
        return is;
    }
};

template <typename Trait>
void CircularLinkedList<Trait>::link_push_back(const value_type &value, Ref ref) {
    Node* n = new Node(value, ref);
    if (this->m_size == 0) {
        this->m_pRoot = this->m_tail = n;
    } else {
        this->m_tail->setNext(n);
        this->m_tail = n;
    }
    this->m_tail->setNext(this->m_pRoot); // cierra el circulo
    this->m_size++;
}

template <typename Trait>
void CircularLinkedList<Trait>::unlink_all() {
    if (this->m_tail) this->m_tail->setNext(nullptr); // rompe el circulo
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
template <typename Trait>
CircularLinkedList<Trait>::CircularLinkedList(const CircularLinkedList &other) : Base() {
    unique_lock<shared_mutex> lock(other.m_mtx);
    if (!other.m_pRoot) return;
    Node* curr = other.m_pRoot;
    do {
        link_push_back(curr->getData(), curr->getRef());
        curr = curr->getNext();
    } while (curr != other.m_pRoot);
}

// TODO: Move constructor
template <typename Trait>
CircularLinkedList<Trait>::CircularLinkedList(CircularLinkedList &&other) noexcept : Base() {
    unique_lock<shared_mutex> lock(other.m_mtx);
    this->m_pRoot = std::exchange(other.m_pRoot, nullptr);
    this->m_tail  = std::exchange(other.m_tail,  nullptr);
    this->m_size  = std::exchange(other.m_size,  0);
}

// TODO: Copy assignment operator
template <typename Trait>
CircularLinkedList<Trait>& CircularLinkedList<Trait>::operator=(const CircularLinkedList &other) {
    if (this == &other) return *this;
    unique_lock<shared_mutex> lk1(this->m_mtx);
    unique_lock<shared_mutex> lk2(other.m_mtx);
    unlink_all();
    if (!other.m_pRoot) return *this;
    Node* curr = other.m_pRoot;
    do {
        link_push_back(curr->getData(), curr->getRef());
        curr = curr->getNext();
    } while (curr != other.m_pRoot);
    return *this;
}

// TODO: Move assignment operator
template <typename Trait>
CircularLinkedList<Trait>& CircularLinkedList<Trait>::operator=(CircularLinkedList &&other) noexcept {
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
void CircularLinkedList<Trait>::push_front(value_type value, Ref ref) {
    unique_lock<shared_mutex> lock(this->m_mtx);
    Node* n = new Node(value, ref, this->m_pRoot);
    if (this->m_size == 0) {
        this->m_pRoot = this->m_tail = n;
    } else {
        this->m_pRoot = n;
    }
    this->m_tail->setNext(this->m_pRoot);
    this->m_size++;
}

template <typename Trait>
std::tuple<typename CircularLinkedList<Trait>::value_type, Ref>
CircularLinkedList<Trait>::pop_front() {
    unique_lock<shared_mutex> lock(this->m_mtx);
    if (!this->m_pRoot) throw out_of_range("lista vacia");
    Node* old = this->m_pRoot;
    auto result = std::make_tuple(old->getData(), old->getRef());
    if (this->m_size == 1) {
        this->m_pRoot = this->m_tail = nullptr;
    } else {
        this->m_pRoot = old->getNext();
        this->m_tail->setNext(this->m_pRoot);
    }
    delete old;
    this->m_size--;
    return result;
}

template <typename Trait>
void CircularLinkedList<Trait>::push_back(value_type value, Ref ref) {
    unique_lock<shared_mutex> lock(this->m_mtx);
    link_push_back(value, ref);
}

template <typename Trait>
std::tuple<typename CircularLinkedList<Trait>::value_type, Ref>
CircularLinkedList<Trait>::pop_back() {
    unique_lock<shared_mutex> lock(this->m_mtx);
    if (!this->m_tail) throw out_of_range("lista vacia");
    Node* old = this->m_tail;
    auto result = std::make_tuple(old->getData(), old->getRef());
    if (this->m_size == 1) {
        this->m_pRoot = this->m_tail = nullptr;
    } else {
        Node* prev = this->m_pRoot;
        while (prev->getNext() != this->m_tail)
            prev = prev->getNext();
        this->m_tail = prev;
        this->m_tail->setNext(this->m_pRoot);
    }
    delete old;
    this->m_size--;
    return result;
}

template <typename Trait>
void CircularLinkedList<Trait>::insert(const value_type &value, Ref ref) {
    unique_lock<shared_mutex> lock(this->m_mtx);
    if (this->m_size == 0) {
        Node* n = new Node(value, ref);
        n->setNext(n);
        this->m_pRoot = this->m_tail = n;
        this->m_size++;
        return;
    }
    if (this->m_comp(value, this->m_pRoot->getDataRef())) {
        Node* n = new Node(value, ref, this->m_pRoot);
        this->m_pRoot = n;
        this->m_tail->setNext(this->m_pRoot);
        this->m_size++;
        return;
    }
    Node* prev = this->m_pRoot;
    while (prev->getNext() != this->m_pRoot &&
           !this->m_comp(value, prev->getNext()->getDataRef()))
        prev = prev->getNext();
    Node* n = new Node(value, ref, prev->getNext());
    prev->setNext(n);
    if (prev == this->m_tail) {
        this->m_tail = n;
        this->m_tail->setNext(this->m_pRoot);
    }
    this->m_size++;
}

#endif // __CIRCULARLINKEDLIST_H__
