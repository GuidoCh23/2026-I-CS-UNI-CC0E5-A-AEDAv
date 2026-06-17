#include <iostream>
#include <sstream>
#include "../types.h"
#include "hashtable.h"
using namespace std;

// Tarea HashTable(AVL): DemoHashTable
void DemoHashTable() {
    cout << "\nPRUEBAS HASHTABLE(AVL)" << endl;

    HashTable<T1, T1> m;

    // Tarea HashTable(AVL): m[key] = value
    m[10] = 100;
    m[25] = 250;
    m[10] = 101;
    m[3]  = 30;
    m[7]  = 70;

    cout << "m[10] = " << m[10] << endl;
    cout << "m[3]  = " << m[3]  << endl;

    // Tarea HashTable(AVL): for(const auto& [key, value] : m)
    cout << "\nfor-range:" << endl;
    for (const auto& [key, value] : m)
        cout << "  " << key << " -> " << value << endl;

    // Tarea HashTable(AVL): operator<<
    cout << "\noperator<<: " << m << endl;

    // Tarea HashTable(AVL): operator>>
    ostringstream oss;
    oss << m;
    HashTable<T1, T1> m2;
    istringstream iss(oss.str());
    iss >> m2;
    cout << "operator>>: " << m2 << endl;

    // Tarea HashTable(AVL): Constructor copia
    HashTable<T1, T1> copia(m);
    copia[99] = 990;
    cout << "\ntamano original: " << m.size() << " | tamano copia: " << copia.size() << endl;
    cout << "original: " << m     << endl;
    cout << "copia:    " << copia << endl;

    // Tarea HashTable(AVL): Move constructor
    HashTable<T1, T1> moved(move(copia));
    cout << "\nmove: " << moved << endl;

    cout << "\nFIN PRUEBAS HASHTABLE(AVL)" << endl;
}
