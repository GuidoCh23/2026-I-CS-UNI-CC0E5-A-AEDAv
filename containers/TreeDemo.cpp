#include <iostream>
#include <sstream>
#include <string>

#include "../types.h"
#include "BinaryTree.h"
#include "AVLTree.h"
#include "traits.h"

using namespace std;

using AscBT   = BinaryTree<AscendingTrait<BinaryTreeNode<T1>>>;
using DescBT  = BinaryTree<DescendingTrait<BinaryTreeNode<T1>>>;
using AscAVL  = AVLTree<AscendingAVLTrait<T1>>;
using DescAVL = AVLTree<DescendingAVLTrait<T1>>;

template<typename Tree>
void DemoTree(Tree& tree) {
    tree.insert(28, 15);
    tree.insert(17, 25);
    tree.insert(8, 35);
    tree.insert(4, 45);
    tree.insert(35, 55);
    cout << tree << endl;
}

void BinaryTreeDemo() {
    cout << "BINARY TREE:" << endl;
    AscBT tree;
    DemoTree(tree);

    // BinaryTree: forward/backward iterator (inorder)
    cout << "Inorder forEach:  ";
    tree.inorder().forEach([](T1& x){ cout << x << " "; });
    cout << endl;

    cout << "Inorder rForEach: ";
    tree.inorder().rForEach([](T1& x){ cout << x << " "; });
    cout << endl;

    // BinaryTree: forward/backward iterator (preorder)
    cout << "Preorder forEach:  ";
    tree.preorder().forEach([](T1& x){ cout << x << " "; });
    cout << endl;

    cout << "Preorder rForEach: ";
    tree.preorder().rForEach([](T1& x){ cout << x << " "; });
    cout << endl;

    // BinaryTree: forward/backward iterator (postorder)
    cout << "Postorder forEach:  ";
    tree.postorder().forEach([](T1& x){ cout << x << " "; });
    cout << endl;

    cout << "Postorder rForEach: ";
    tree.postorder().rForEach([](T1& x){ cout << x << " "; });
    cout << endl;

    // BinaryTree: usar en bucle nativo foreach
    cout << "Native for loop (inorder): ";
    for (auto& x : tree) cout << x << " ";
    cout << endl;

    // BinaryTree: Otras mejoras libres #1 — search
    cout << "Search 17: " << (tree.search(17) ? "found" : "not found") << endl;
    cout << "Search 99: " << (tree.search(99) ? "found" : "not found") << endl;
    cout << "Height: " << tree.height() << ", Size: " << tree.size() << endl;

    // BinaryTree: Otras mejoras libres #2 — printTree visual
    cout << "PrintTree:\n";
    tree.printTree(cout);

    // BinaryTree: operator>> — deserializar desde string
    string s = tree.toString();
    AscBT tree2;
    istringstream iss(s);
    iss >> tree2;
    cout << "Deserialized: " << tree2 << endl;

    // BinaryTree: Constructor copia
    AscBT tree3(tree);
    cout << "Copy: " << tree3 << endl;

    // BinaryTree: Move Constructor
    AscBT tree4(move(tree3));
    cout << "Move: " << tree4 << endl;
}

void AVLTreeDemo() {
    cout << "AVL TREE:" << endl;
    // AVL: peor caso para BST sin AVL - insertar en orden ascendente
    AscAVL avl;
    avl.insert(1, 10); avl.insert(2, 20); avl.insert(3, 30);
    avl.insert(4, 40); avl.insert(5, 50); avl.insert(6, 60);
    avl.insert(7, 70);

    cout << "AVL inorder (1..7 en orden): " << avl << endl;
    cout << "Altura: " << avl.height() << " (esperado <= 3, sin AVL seria 6)" << endl;
    cout << "Size: " << avl.size() << endl;

    // AVL: inorder forEach y rForEach (heredados de BinaryTree)
    cout << "Inorder forEach:  ";
    avl.inorder().forEach([](T1& x){ cout << x << " "; });
    cout << endl;

    cout << "Inorder rForEach: ";
    avl.inorder().rForEach([](T1& x){ cout << x << " "; });
    cout << endl;

    // AVL: peor caso descendente — insertar en orden inverso (7..1)
    DescAVL avl2;
    avl2.insert(7, 70); avl2.insert(6, 60); avl2.insert(5, 50);
    avl2.insert(4, 40); avl2.insert(3, 30); avl2.insert(2, 20);
    avl2.insert(1, 10);
    cout << "AVL desc inorder (7..1 en orden): " << avl2 << endl;
    cout << "Altura: " << avl2.height() << " (esperado <= 3)" << endl;

    // AVL: Constructor copia (preserva m_height)
    AscAVL avl3(avl);
    cout << "Copy: " << avl3 << ", Height: " << avl3.height() << endl;

    // AVL: Move constructor
    AscAVL avl4(move(avl3));
    cout << "Move: " << avl4 << endl;

    // AVL: operator>> (heredado de BinaryTree)
    string s = avl.toString();
    AscAVL avl5;
    istringstream iss(s);
    iss >> avl5;
    cout << "Deserialized: " << avl5 << ", Height: " << avl5.height() << endl;

    // AVL: Otras mejoras libres #1 - search (heredada de BinaryTree)
    cout << "Search 4: " << (avl.search(4) ? "found" : "not found") << endl;
    cout << "Search 9: " << (avl.search(9) ? "found" : "not found") << endl;

    // AVL: Otras mejoras libres #2 - printTree visual
    cout << "PrintTree AVL:\n";
    avl.printTree(cout);
}

void TreesDemo() {
    BinaryTreeDemo();
    AVLTreeDemo();
}
