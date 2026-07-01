//#include <iostream.h>
#include <time.h>
#include <stdlib.h>
#include <string>
#include "BTree.h"
#include "traits.h"

//const Key * keys="CDAMPIWNBKEHOLJYQZFXVRTSGU";
const Key * keys1 = "D1XJ2xTg8zKL9AhijOPQcEowRSp0NbW567BUfCqrs4FdtYZakHIuvGV3eMylmn";
const Key * keys2 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
const Key * keys3 = "DYZakHIUwxVJ203ejOP9Qc8AdtuEop1XvTRghSNbW567BfiCqrs4FGMyzKLlmn";

const size_t BTreeSize = 3;
void BTreeDemo()
{
       size_t result, i;
       BTree <BTreeTrait<Key, Ref, less<Key>>> bt (BTreeSize);
       for (i = 0; keys1[i]; i++)
       {
               //cout<<"Inserting "<<keys1[i]<<endl;
               result = bt.Insert(keys1[i], i*i);
               //bt.Print(cout);
       }
       // Examen Final Operator<< y Print
       cout << bt;
       /*for (i = 0; keys2[i]; i++)
       {
               cout << "Searching " << keys2[i] << " ";
               long ObjID = bt.Search(keys2[i]);
               if( ObjID != -1 )
                       cout << "Achei " << keys2[i] << " ID = " << ObjID << endl;
               else
                       cout <<"Nao achei!" << keys2[i] << endl;
       }*/
       /*cout.flush();

       for (i = 0; keys3[i]; i++)
       {
               cout << "Removing " << keys3[i] << " ";
               if( bt.Remove(keys3[i], -1) )
                       cout << keys3[i] << " removido !" << endl;
               else
                       cout <<"Nao achei!" << keys3[i] << endl;
               bt.Print(cout);
       }
       bt.Print(cout);
       cout.flush();*/

       // P1 Tarea ForEach Variadico
       size_t count = 0;
       bt.ForEach([](auto &info, size_t level, size_t *pCount) {
               (*pCount)++;
       }, &count);
       cout << "Total de claves: " << count << endl;

       // P1 Tarea FirstThat Variadico
       // Buscamos G
       auto *found = bt.FirstThat([](auto &info, size_t level, Key target) -> decltype(&info) {
               if(info.key == target) return &info;
               return nullptr;
       }, 'G');
       if(found)
               cout << found->key << " encontrado con ObjID=" << found->ObjID << endl;
               
       // Buscamos n
       auto *found2 = bt.FirstThat([](auto &info, size_t level, Key target) -> decltype(&info) {
               if(info.key == target) return &info;
               return nullptr;
       }, 'n');
       if(found2)
               cout << found2->key << " encontrado con ObjID=" << found2->ObjID << endl;

       // Buscamos E
       auto *found3 = bt.FirstThat([](auto &info, size_t level, Key target) -> decltype(&info) {
               if(info.key == target) return &info;
               return nullptr;
       }, 'E');
       if(found3)
               cout << found3->key << " encontrado con ObjID=" << found3->ObjID << endl;
}









/*const Key * keys="CDAMPIWNBKEHOLJYQZFXVRTSGU";
const Key * keys2="CDAMPIWNBKEHOLJYQZFXVRTSGU";
const int BTreeSize = 3;
main (int argc, char * argv)
{
       //__int64 li;
       BTree <__int64> bt (BTreeSize);
       for (register int i = 0; i < 1000000; i++)
       {
               //cout<<"Inserting "<<keys[i]<<endl;
               bt.Insert(i, i-1);
               //bt.Print(cout);
       }

       for (i = 0; i < 1000; i++)
       {
               __int64 key = 975000+(::rand()%50000);
               //cout << "Searching " << (long)key << " ";
               long ObjID = bt.Search(key);
               if( ObjID != -1 )
                       cout << "Achei " << (long)key << " ID = " << ObjID << endl;
               else
                       cout <<"  Nao achei!" << (long)key << endl;
       }
       cout.flush();

       return 1;
}*/



/*const int BTreeSize = 3;
main (int argc, char * argv)
{
       int result, i;
       BTree <LONGLONG> bt(BTreeSize);
       result = bt.Create ("ernesto3-string-btree-start.dat",ios::in|ios::out);
       if (!result) { cout<<"Please delete testbt.dat"<<endl;return 0; }
       srand( (unsigned)time( NULL ) );
       LARGE_INTEGER key;
       for (i = 0; i < 1000000; i++)
       {
               //cout<<"Inserting "<<keys[i]<<endl;
               char strTmp[50];
               key.LowPart = rand();
               key.HighPart = rand();
               std::string str(strTmp);
               result = bt.Insert(key.QuadPart, i);
               //bt.Print(cout);
               if( i % 100000 == 0 )
               {       cout << i << endl; cout.flush();        }
       }
       //cout << "Searching D " << bt.Search();
       //bt.Search(1,1);
       cout.flush();
       return 1;
}*/
