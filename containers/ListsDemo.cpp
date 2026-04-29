#include <iostream>
#include <fstream>
#include <string>
#include <thread>

#include "../types.h"
#include "linkedlist.h"
#include "doublelinkedlist.h"
#include "circularlinkedlist.h"
#include "circulardoublelinkedlist.h"

using namespace std;

template <typename Container>
void DemoList(Container& list) {
    list.insert(28, 15);
    list.insert(17, 25);
    list.insert(8,  35);
    list.insert(4,  45);
    list.insert(35, 55);
    cout << list << endl;
}

void TestBasicos() {
    cout << "\nTest Basicos" << endl;
    LinkedList<AscendingLinkedListTrait<T1>> asc;
    asc.insert(10, 1); asc.insert(5, 2); asc.insert(20, 3);
    cout << "Ascendente:  " << asc << endl;

    LinkedList<DescendingLinkedListTrait<T1>> desc;
    desc.insert(10, 1); desc.insert(5, 2); desc.insert(20, 3);
    cout << "Descendente: " << desc << endl;
}

void LinkedListDemo() {
    cout << "\nLinked List Demo" << endl;
    LinkedList<AscendingLinkedListTrait<T1>>  list;
    DemoList(list);

    LinkedList<DescendingLinkedListTrait<T1>> list2;
    DemoList(list2);
}

void DoubleLinkedListDemo() {
    cout << "\nDouble Linked List Demo" << endl;

    DoubleLinkedList<AscendingDLLTrait<T1>> list;
    DemoList(list);

    cout << "Foreach fwd:    ";
    for (auto& x : list)
        cout << x << " ";
    cout << endl;

    cout << "ReverseForEach: ";
    list.ReverseForEach([](T1& x){ cout << x << " "; });
    cout << endl;

    cout << "contains(8):  " << list.contains(8)  << endl;
    cout << "contains(99): " << list.contains(99) << endl;

    list.reverse();
    cout << "reverse:        ";
    for (auto& x : list)
        cout << x << " ";
    cout << endl;

    DoubleLinkedList<DescendingDLLTrait<T1>> desc;
    DemoList(desc);
}

void CircularLinkedListDemo() {
    cout << "\nCircular Linked List Demo" << endl;

    CircularLinkedList<AscendingCLLTrait<T1>> list;
    DemoList(list);

    cout << "Foreach:        ";
    for (auto& x : list)
        cout << x << " ";
    cout << endl;

    cout << "ReverseForEach: ";
    list.ReverseForEach([](T1& x){ cout << x << " "; });
    cout << endl;

    cout << "contains(8):  " << list.contains(8)  << endl;
    cout << "contains(99): " << list.contains(99) << endl;

    CircularLinkedList<DescendingCLLTrait<T1>> desc;
    DemoList(desc);
}

void CircularDoubleLinkedListDemo() {
    cout << "\nCircular Double Linked List Demo" << endl;

    CircularDoubleLinkedList<AscendingCDLLTrait<T1>> list;
    DemoList(list);

    cout << "Foreach fwd:    ";
    for (auto& x : list)
        cout << x << " ";
    cout << endl;

    cout << "ReverseForEach: ";
    list.ReverseForEach([](T1& x){ cout << x << " "; });
    cout << endl;

    cout << "contains(8):  " << list.contains(8)  << endl;
    cout << "contains(99): " << list.contains(99) << endl;

    CircularDoubleLinkedList<DescendingCDLLTrait<T1>> desc;
    DemoList(desc);
}

void TestConcurrencia() {
    cout << "\nTest De Concurrencia" << endl;
    LinkedList<AscendingLinkedListTrait<T1>> list;

    auto worker = [&list](int thread_id) {
        for (int i = 0; i < 1000; i++)
            list.push_front(i, thread_id);
    };

    thread t1(worker, 1), t2(worker, 2), t3(worker, 3),
           t4(worker, 4), t5(worker, 5);
    t1.join(); t2.join(); t3.join(); t4.join(); t5.join();

    cout << "5 hilos x 1000 inserciones. Tamano (esperado 5000): " << list.size() << endl;
    cout << (list.size() == 5000 ? "Exito" : "Fallo") << endl;
}

void TestOperators() {
    cout << "\nTest de Operadores" << endl;
    LinkedList<AscendingLinkedListTrait<T1>> list;
    list.insert(10, 100); list.insert(20, 200); list.insert(30, 300);
    cout << "Lista:   " << list << endl;
    cout << "list[0]: " << list[0] << "  list[2]: " << list[2] << endl;
}

void ListsDemo() {
    TestBasicos();
    LinkedListDemo();
    DoubleLinkedListDemo();
    CircularLinkedListDemo();
    CircularDoubleLinkedListDemo();
    TestConcurrencia();
    TestOperators();
}
