#include <iostream>
#include <string>

#include "../types.h"
#include "linkedlist.h"
#include "doublelinkedlist.h"
#include "circularlinkedlist.h"
#include "circulardoublelinkedlist.h"

using namespace std;

template <typename Container>
void DemoList(Container& list){
    list.insert(28, 15);
    list.insert(17, 25);
    list.insert(8, 35);
    list.insert(4, 45);
    list.insert(35, 55);
    cout << list << endl;
}

void LinkedListDemo(){
    LinkedList<AscendingLinkedListTrait<T1>> list;
    DemoList(list);

    cout << "ForEach: ";
    list.ForEach([](T1& x){ cout << x << " "; });
    cout << endl;
}

void DoubleLinkedListDemo(){
    DoubleLinkedList<AscendingDLLTrait<T1>> list;
    DemoList(list);

    cout << "ForEach: ";
    list.ForEach([](T1& x){ cout << x << " "; });
    cout << endl;

    cout << "ReverseForEach: ";
    list.ReverseForEach([](T1& x){ cout << x << " "; });
    cout << endl;

    cout << "Backward iterator: ";
    for (auto it = list.rbegin(); it != list.rend(); ++it)
        cout << *it << " ";
    cout << endl;
}

void CircularLinkedListDemo(){
    CircularLinkedList<AscendingCLLTrait<T1>> list;
    DemoList(list);

    cout << "circularForEach (2 vueltas): ";
    list.circularForEach(2, [](T1& x){ cout << x << " "; });
    cout << endl;

    cout << "Circular iterator: ";
    for (auto it = list.cbegin(); it != list.cend(); ++it)
        cout << *it << " ";
    cout << endl;
}

void CircularDoubleLinkedListDemo(){
    CircularDoubleLinkedList<AscendingCDLLTrait<T1>> list;
    DemoList(list);

    cout << "circularForEach forward (2 vueltas): ";
    list.circularForEach(2, 1, [](T1& x){ cout << x << " "; });
    cout << endl;

    cout << "circularForEach backward (2 vueltas): ";
    list.circularForEach(2, -1, [](T1& x){ cout << x << " "; });
    cout << endl;

    cout << "Circular forward iterator: ";
    for (auto it = list.cbegin(); it != list.cend(); ++it)
        cout << *it << " ";
    cout << endl;

    cout << "Circular backward iterator: ";
    for (auto it = list.crbegin(); it != list.crend(); ++it)
        cout << *it << " ";
    cout << endl;
}

void ListsDemo(){
    LinkedListDemo();
    DoubleLinkedListDemo();
    CircularLinkedListDemo();
    CircularDoubleLinkedListDemo();
}
