#include "process_manager.hpp"
#include "x86_64/paging/upper_page_table.hpp"
#include "x86_64/paging/utilities.hpp"

namespace{
  Chisaka::ProcessManager manager;
}

namespace Chisaka{

void ProcessManager::Init(){
  m_kernelTable = reinterpret_cast<X8664::UpperPageTable*>(X8664::GetBaseTable());
  InitRootProcess();
  m_currentProcess = m_root;
}

void ProcessManager::InitRootProcess(){
  m_root = reinterpret_cast<Process*>(Kernel::kmalloc(sizeof(Process)));
  m_root->id = procCount++;
  m_root->child = nullptr;
  m_root->childId = -1;
  CreatePagetable(m_root);
}

Process* ProcessManager::CreateProcess(){
  auto spawned = reinterpret_cast<Process*>(Kernel::kmalloc(sizeof(Process)));
  spawned->id = procCount++;
  spawned->SetParent(m_currentProcess);
  if(spawned->parent->child){
    spawned->parent->child->InsertSibling(spawned);
  }
  else{
    spawned->parent->child = spawned;
  }
  CreatePagetable(spawned);
  return spawned;
}

void ProcessManager::CreatePagetable(Process* process){
  auto baseTable = reinterpret_cast<X8664::UpperPageTable*>(
      Kernel::kmalloc(sizeof(X8664::UpperPageTable))
  );
  MapKernel(baseTable);
  process->pageTable = baseTable;
}

void ProcessManager::MapKernel(X8664::UpperPageTable* ptbl){
  // Kernel is mapped from 0xffff8000'00000000 -> 0xffff8002'40000000 
  // and is an identity mapping of the first 0x2'40000000 bytes of ram
  // 1 Pml4e maps 0x80'00000000
  using namespace X8664;
  ptbl->AddEntry(
      0xffff8000'00000000/0x1000, 
      X8664::UpperPageEntry{m_root->pageTable, 
      UPeOpt::Present}
  );
}

pid_t ProcessManager::Kill(Process* process){
  // unmap process' parent, children and siblings. 
  // then deallocate its memory
}

pid_t ProcessManager::Kill(pid_t pid){
  Process* proc = Find(pid);
  return Kill(proc);
}


Process* ProcessManager::Find(pid_t pid){

}

}
