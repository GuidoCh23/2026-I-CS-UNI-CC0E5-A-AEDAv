#ifndef __ITERATOR_H__
#define __ITERATOR_H__
#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

template <typename Container, class IteratorBase> // 
class general_iterator
{public:
    using Node = typename Container::Node;
    using myself = general_iterator<Container, IteratorBase>;
    
protected:
    Container *m_pContainer;
    Node      *m_pNode;
public:
    general_iterator(Container *pContainer, Node *pNode)
        : m_pContainer(pContainer), m_pNode(pNode) {}
    general_iterator(myself &other) 
          : m_pContainer(other.m_pContainer), m_pNode(other.m_pNode){}
    general_iterator(myself &&other) // Move constructor
          {   m_pContainer = move(other.m_pContainer);
              m_pNode      = move(other.m_pNode);
          }
    IteratorBase operator=(IteratorBase &iter)
          {   m_pContainer = move(iter.m_pContainer);
              m_pNode      = move(iter.m_pNode);
              return *(IteratorBase *)this; // Pending static_cast?
          }
    Node *getNode() const { return m_pNode; }
    friend bool operator==(const IteratorBase &a, const IteratorBase &b) { return a.getNode() == b.getNode(); }
    typename Container::value_type &operator*(){
        return m_pNode->getDataRef();
    }
};

// Examen Final Forward Iterator
template <typename Node>
class snapshot_iterator {
    std::shared_ptr<std::vector<Node>> m_data;
    size_t m_pos;
protected:
    void Advance() { ++m_pos; }
    bool IsEnd() const { return !m_data || m_pos >= m_data->size(); }
public:
    snapshot_iterator() : m_pos(0) {}
    snapshot_iterator(std::shared_ptr<std::vector<Node>> data, size_t pos)
        : m_data(std::move(data)), m_pos(pos) {}
    Node& operator*()  { return (*m_data)[m_pos]; }
    bool operator==(const snapshot_iterator& o) const {
        return (IsEnd() && o.IsEnd()) || (m_data == o.m_data && m_pos == o.m_pos);
    }
    bool operator!=(const snapshot_iterator& o) const { return !(*this == o); }
};

#endif
 