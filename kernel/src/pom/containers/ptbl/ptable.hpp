#pragma once

// Stack allocated process container, it uses a fixed size table in order to contain the processes
// the process model is simple for now, there are no siblings

#include <cstdint>

#include "aii/array.hpp"


namespace Chisaka::PTbl{
  class PTable{
    static constexpr std::size_t ENTRIES = 20;
    using IdType = std::uint8_t;

    struct ProcessType{
      // the index into the buffer is the process's id
      // later on when heap is tested we can put a x86 PML4 page table here
      IdType parent_id;
      bool present;
      struct{
      } pcb;
    };

    public:
    
    ProcessType& Process(IdType id){return m_procs[id];}
    const ProcessType& Process(IdType id) const {return m_procs[id];}

    ProcessType& operator[](IdType id){return Process(id);}
    const ProcessType& operator[](IdType id) const{return Process(id);}

    IdType NextFreeId() const;

    IdType AllocateProcess(IdType parent);
    void DeallocateProcess(IdType id);
    
    private:
      Aii::Array<ProcessType, ENTRIES> m_procs;
  };
}
