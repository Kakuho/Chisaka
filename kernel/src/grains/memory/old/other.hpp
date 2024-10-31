#ifndef MEM_OTHER_HPP
#define MEM_OTHER_HPP
#include "memorylist.hpp"

// this requires some cleanup
namespace memory{

  inline void test1(){
    kout << "----\n";
    pmm::printEntries();
    kout << "----\n";
    int* one = reinterpret_cast<int*>(pmm::allocHeap(5));
    pmm::printEntries();
    kout << "----\n";
    int* two = reinterpret_cast<int*>(pmm::allocHeap(128));
    pmm::printEntries();
    kout << "----\n";
    pmm::free(two);
    pmm::printEntries();
    kout << "----\n";
    int* three = reinterpret_cast<int*>(pmm::allocStack(32));
    pmm::printEntries();
    kout << "----\n";
  }

  inline void testfree(){
    pmm::printEntries();
    kout << "----\n";
    // we have one memory chunk allocated []
    char* pch1 = reinterpret_cast<char*>(pmm::allocHeap(10));
    pmm::printEntries();
    kout << "----\n";
    // we have 2 memory chunk allocated [] []
    char* pch2 = reinterpret_cast<char*>(pmm::allocHeap(10));
    pmm::printEntries();
    kout << "----\n";
    // we have 3 memory chunk allocated [] [] []
    char* pch3 = reinterpret_cast<char*>(pmm::allocHeap(10));
    pmm::printEntries();
    kout << "----\n";
    // we have 4 memory chunk allocated [] [] [] []
    char* pch4 = reinterpret_cast<char*>(pmm::allocHeap(10));
    pmm::printEntries();
    kout << "----\n";
    // we free up the third chunk: [] [] [FREE] []
    pmm::free(pch3);
    pmm::printEntries();

    kout << "----\n";
    // we free up the first chunk: [FREE] [] [FREE] []
    pmm::free(pch1);
    pmm::printEntries();
  }

  class A{
    int a;
    int b;
    long c;
  };

  inline void testfree2(){
    // mem1 
    // mem2 
    // mem3
    pmm::printEntries();
    kout << "----\n";
    A* pch1 = new A;
    pmm::printEntries();
    kout << "----\n";
    A* pch2 = new A;
    pmm::printEntries();
    kout << "----\n";
    A* pch3 = new A;
    pmm::printEntries();
    kout << "----\n";
    A* pch4 = new A;
    pmm::printEntries();
    kout << "----\n";
    // pch1
    // pch2
    // pch3
    // pch4
    // mem1
    // mem2
    // mem3
    delete pch3;
    // pch1
    // pch2
    // mem1
    // pch4
    // mem2
    // mem3
    // mem4
    pmm::printEntries();
    kout << "----\n";
    delete pch1;
    // mem1
    // pch2
    // mem2
    // pch4
    // mem3
    // mem4
    // mem5
    pmm::printEntries();
    kout << "----\n";
  }

}

#endif
