#include <iostream>
#include <sstream>
#include <string>
#include <thread>

#include "../types.h"
#include "linkedlist.h"

using namespace std;

template <typename Container>
void DemoList(Container& list) {
    list.insert(28, 15);
    list.insert(17, 25);
    list.insert(8, 35);
    list.insert(4, 45);
    list.insert(35, 55);
    cout << list << endl;
}

void LinkedListDemo() {
    cout << "LINKED LIST:" << endl;
    LinkedList<AscendingLinkedListTrait<T1>> list;
    DemoList(list);

    cout << "ForEach: ";
    list.ForEach([](T1& x){ cout << x << " "; });
    cout << endl;

    // operator<< y operator>>
    ostringstream oss;
    oss << list;
    LinkedList<AscendingLinkedListTrait<T1>> list2;
    istringstream iss(oss.str());
    iss >> list2;
    cout << "operator>>: " << list2 << endl;
}

void ListsDemo() {
    LinkedListDemo();
}
