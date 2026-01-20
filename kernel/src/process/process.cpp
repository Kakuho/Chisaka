#include "process.hpp"
#include "kassert.hpp"

namespace Chisaka{

  void Process::SetParent(Process* proc){
    if(proc->child){
      proc->child->InsertSibling(this);
    }
    else{
      proc->child = this;
    }
    this->parentId = proc->id;

  }

  void Process::InsertSibling(Process* proc){
    kassert("Proc::InsertSibiling(Process*) -- IMPLEMENT aii::List::Node FIRST!");
  }

  void Process::InsertChild(Process* proc){
    if(this->child){
      this->child->InsertSibling(proc);
    }
    else{
      this->child = proc;
    }
    proc->parentId = this->id;
  }

}
