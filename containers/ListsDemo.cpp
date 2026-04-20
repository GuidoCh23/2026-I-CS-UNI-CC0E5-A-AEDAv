#include "linkedlist.h"
#include <iostream>
#include <sstream>
using namespace std;

void LinkedListDemo(){
    LinkedList<DescendingLinkedListTrait<T1>> list;
    list.insert(1, 15);
    list.insert(2, 25);
    list.insert(3, 35);
    list.insert(4, 45);
    list.insert(5, 55);
    cout << "insert:" << list << endl;

    // t5: push_front
    list.push_front(9, 99);
    cout << "push_front:" << list << endl;

    // t6: pop_front
    list.pop_front();
    cout << "pop_front:" << list << endl;

    // t7: push_back
    list.push_back(0, 0);
    cout << "push_back:" << list << endl;

    // t8: pop_back
    list.pop_back();
    cout << "pop_back:" << list << endl;

    // t9: operator[]
    cout << "lista[0]:" << list[0] << endl;
    cout << "lista[2]:" << list[2] << endl;

    // t12: foreach
    cout << "foreach:";
    list.ForEach([](T1& x){ cout << x << " "; });
    cout << endl;

    // t1: copy constructor
    LinkedList<DescendingLinkedListTrait<T1>> copia(list);
    cout << "copia:" << copia << endl;

    // t2: move constructor
    LinkedList<DescendingLinkedListTrait<T1>> movida(move(copia));
    cout << "movida:" << movida << endl;

    // t10/t11: operator>>
    string s = list.toString();
    LinkedList<DescendingLinkedListTrait<T1>> leida;
    istringstream(s) >> leida;
    cout << "operator>>:" << leida << endl;
}

void ListsDemo(){
    LinkedListDemo();

}
