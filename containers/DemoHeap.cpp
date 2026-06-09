#include <iostream>
#include <sstream>
#include <thread>
#include "../types.h"
#include "heap.h"
using namespace std;

// Tarea Heaps: DemoMinHeap y DemoMaxHeap
using MinHeap = Heap<MinHeapTrait<T1>>;
using MaxHeap = Heap<MaxHeapTrait<T1>>;

static const initializer_list<T1> kTestValues = {10, 4, 15, 1, 7, 12, 3};

void DemoMinHeap() {
    cout << "\nMinHeap" << endl;
    MinHeap h;

    // insert y heapifyUp
    cout << "insert:" << endl;
    for (T1 v : kTestValues) {
        h.insert(v, v * 10);
        cout << "insert(" << v << ") -> " << h << endl;
    }
    cout << "arbol:\n" << h.treeToString();

    // peek
    auto [pval, pref] = h.peek();
    cout << "peek (minimo): " << pval << " ref:" << pref << endl;

    // operator<< y operator>>
    ostringstream oss;
    oss << h;
    cout << "operator<<: " << oss.str() << endl;
    MinHeap h2;
    istringstream iss(oss.str());
    iss >> h2;
    cout << "operator>>: " << h2 << endl;

    // forEach
    cout << "forEach: ";
    h.forEach([](MinHeap::value_type& v){ cout << v << " "; });
    cout << endl;

    // range-based for
    cout << "range-for: ";
    for (auto& node : h) cout << node.m_data << " ";
    cout << endl;

    // extract y heapifyDown
    cout << "\nextract (orden ascendente):" << endl;
    while (!h.isEmpty()) {
        auto [val, ref] = h.extract();
        cout << "  extract -> " << val << " | ";
        h.forEach([](MinHeap::value_type& v){ cout << v << " "; });
        cout << endl;
    }
}

void DemoMaxHeap() {
    cout << "\nMaxHeap" << endl;
    MaxHeap h;

    cout << "insert:" << endl;
    for (T1 v : kTestValues) {
        h.insert(v, v * 10);
        cout << "insert(" << v << ") -> " << h << endl;
    }
    cout << "arbol:\n" << h.treeToString();

    auto [pval, pref] = h.peek();
    cout << "peek (maximo): " << pval << " ref:" << pref << endl;

    cout << "forEach: ";
    h.forEach([](MaxHeap::value_type& v){ cout << v << " "; });
    cout << endl;

    cout << "range-for: ";
    for (auto& node : h) cout << node.m_data << " ";
    cout << endl;

    cout << "\nextract (orden descendente):" << endl;
    while (!h.isEmpty()) {
        auto [val, ref] = h.extract();
        cout << "  extract -> " << val << " | ";
        h.forEach([](MaxHeap::value_type& v){ cout << v << " "; });
        cout << endl;
    }
}

void DemoHeapConcurrencia() {
    cout << "\nconcurrencia" << endl;
    MinHeap h;
    auto worker = [&h](T1 id) {
        T1 i{};
        while (i < 200) { h.insert(i * id, id); ++i; }
    };
    thread th1(worker, 1), th2(worker, 2), th3(worker, 3),
           th4(worker, 4), th5(worker, 5);
    th1.join(); th2.join(); th3.join(); th4.join(); th5.join();
    cout << "size 1000: " << h.size() << endl;
    auto [val, ref] = h.peek();
    cout << "peek minimo: " << val << endl;
}

void HeapDemo() {
    cout << endl;
    cout << "PRUEBAS HEAP" << endl;
    DemoMinHeap();
    DemoMaxHeap();
    DemoHeapConcurrencia();
    cout << "\nFIN DE LAS PRUEBAS" << endl;
}
