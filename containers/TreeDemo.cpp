#include <iostream>
#include <sstream>
#include <string>

#include "../types.h"
#include "BinaryTree.h"
#include "traits.h"

using namespace std;

using AscBT  = BinaryTree<AscendingTrait<BinaryTreeNode<T1>>>;
using DescBT = BinaryTree<DescendingTrait<BinaryTreeNode<T1>>>;

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

void TreesDemo() {
    BinaryTreeDemo();
}
