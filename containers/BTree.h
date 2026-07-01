// btree.h

#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include <memory>
#include "BTreePage.h"
#include "general_iterator.h"

#define DEFAULT_BTREE_ORDER 3

// P1 Tarea Traits
template <typename Trait>
class BTree
// this is the full version of the BTree
{
       typedef CBTreePage <Trait> BTNode;// useful shorthand

       using keyType   = typename Trait::KeyType;
       using ObjIDType = typename Trait::ObjIDType;

       /*struct ObjectInfo
       {
               keyType first;
               long    second;
               ObjectInfo *&operator->() { return this; }
       };*/

public:
       //typedef ObjectInfo iterator;
       typedef typename BTNode::ObjectInfo      ObjectInfo;

       // Examen Final Forward Iterator
       class ForwardIterator : public snapshot_iterator<ObjectInfo> {
       public:
              using snapshot_iterator<ObjectInfo>::snapshot_iterator;
              ForwardIterator& operator++() { this->Advance(); return *this; }
       };

       ForwardIterator begin() {
              auto snap = std::make_shared<std::vector<ObjectInfo>>();
              m_Root.ForEach([](auto& info, size_t, auto* v) {
                     v->push_back(info);
              }, (size_t)0, snap.get());
              return ForwardIterator(snap, 0);
       }
       ForwardIterator end() { return ForwardIterator(); }

       // Examen Final Backward Iterator
       class BackwardIterator : public snapshot_iterator<ObjectInfo> {
       public:
              using snapshot_iterator<ObjectInfo>::snapshot_iterator;
              BackwardIterator& operator++() { this->Advance(); return *this; }
       };

       BackwardIterator rbegin() {
              auto snap = std::make_shared<std::vector<ObjectInfo>>();
              m_Root.ForEach([](auto& info, size_t, auto* v) {
                     v->push_back(info);
              }, (size_t)0, snap.get());
              std::reverse(snap->begin(), snap->end());
              return BackwardIterator(snap, 0);
       }
       BackwardIterator rend() { return BackwardIterator(); }

public:
       BTree(size_t order = DEFAULT_BTREE_ORDER, bool unique = true);
       ~BTree();
       //int           Open (char * name, int mode);
       //int           Create (char * name, int mode);
       //int           Close ();
       bool            Insert (const keyType key, const ObjIDType ObjID);
       bool            Remove (const keyType key, const ObjIDType ObjID);
       ObjIDType       Search (const keyType key);
       size_t          size()  { return m_NumKeys; }
       size_t          height() { return m_Height;      }
       size_t          GetOrder() { return m_Order;     }

       // P1 Tarea ForEach Variadico
       template <typename Func, typename... Args>
       void ForEach(Func func, Args&&... args);

       template <typename Func, typename... Args>
       ObjectInfo* FirstThat(Func func, Args&&... args);

       //typedef               ObjectInfo iterator;

protected:
       BTNode          m_Root;
       size_t          m_Height;  // height of tree
       size_t          m_Order;   // order of tree
       size_t          m_NumKeys; // number of keys
       bool            m_Unique;  // Accept the elements only once ?
};

const size_t MaxHeight = 5;
template <typename Trait>
BTree<Trait>::BTree(size_t order, bool unique)
                               : m_Unique(unique),
                                 m_Order(order),
                                 m_Root(2 * order  + 1, unique),
                                 m_NumKeys(0)
{
       m_Root.SetMaxKeysForChilds(order);
       m_Height = 1;
}

template <typename Trait>
BTree<Trait>::~BTree()
{
}

template <typename Trait>
bool BTree<Trait>::Insert(const keyType key, const ObjIDType ObjID)
{
       bt_ErrorCode error = m_Root.Insert(key, ObjID);
       if( error == bt_duplicate )
               return false;
       m_NumKeys++;
       if( error == bt_overflow )
       {
               m_Root.SplitRoot();
               m_Height++;
       }
       return true;
}

template <typename Trait>
bool BTree<Trait>::Remove (const keyType key, const ObjIDType ObjID)
{
       bt_ErrorCode error = m_Root.Remove(key, ObjID);
       if( error == bt_duplicate || error == bt_nofound )
               return false;
       m_NumKeys--;

       if( error == bt_rootmerged )
               m_Height--;
       return true;
}

template <typename Trait>
typename BTree<Trait>::ObjIDType BTree<Trait>::Search (const keyType key)
{
       ObjIDType ObjID = -1;
       m_Root.Search(key, ObjID);
       return ObjID;
}


template <typename Trait>
template <typename Func, typename... Args>
void BTree<Trait>::ForEach(Func func, Args&&... args)
{
       for (auto it = begin(); it != end(); ++it)
              func(*it, (size_t)0, std::forward<Args>(args)...);
}

template <typename Trait>
template <typename Func, typename... Args>
typename BTree<Trait>::ObjectInfo *
BTree<Trait>::FirstThat(Func func, Args&&... args)
{
       for (auto it = begin(); it != end(); ++it) {
              auto* r = func(*it, (size_t)0, std::forward<Args>(args)...);
              if (r) return r;
       }
       return nullptr;
}

// Examen Final Operator<<
template <typename Trait>
ostream& operator<<(ostream& os, BTree<Trait>& bt){
       bt.ForEach([](auto &info, size_t level, ostream *os){
               for(size_t i = 0; i < level; i++) *os << "\t";
               *os << info.key << " " << info.ObjID << "\n";
       }, &os);
       return os;
}

// Examen Final Operator>>
template <typename Trait>
istream& operator>>(istream& is, BTree<Trait>& bt){
       typename Trait::KeyType   key;
       typename Trait::ObjIDType ObjID;
       while(is >> key >> ObjID)
               bt.Insert(key, ObjID);
       return is;
}





#endif