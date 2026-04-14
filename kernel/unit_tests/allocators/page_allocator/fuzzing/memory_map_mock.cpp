#include "memory_map_mock.hpp"
#include <cstdint>
#include <initializer_list>

namespace Chisaka::Tests{
  void MemoryMapMock::Init() noexcept{
    // init it according to the layout of the real system
  }

  void MemoryMapMock::InitFlat(std::size_t nbytes) noexcept{
    m_buffer.resize(nbytes);
    m_entries.emplace_back(reinterpret_cast<std::uintptr_t>(&m_buffer[0]), nbytes);
    m_totalUseableBytes = nbytes;
  }

  void MemoryMapMock::InitDiscontigous(std::initializer_list<EntryInit_t> entryInits) noexcept{
    std::size_t count  = 0;
    std::size_t useable = 0;
    for(auto& e: entryInits){
      if(count == 0){
        m_entries.emplace_back(reinterpret_cast<std::uintptr_t>(&m_buffer[0]), e.length);
      }
      else{
        m_entries.emplace_back(m_entries[count-1].base + m_entries[count-1].length, e.length);
      }
      if(e.type == Entry_t::Type::Useable){
        useable += e.length;
      }
    }
    m_totalUseableBytes = useable;
  }
  
  void MemoryMapMock::DumpMemory(const std::string& filename) const{
    std::ofstream ofst{filename};
    ofst << std::setbase(16) << std::setw(2);
    ofst << "Memory Buffer Base: " << reinterpret_cast<std::uintptr_t>(&m_buffer[0]) << '\n';
    for(std::size_t i = 1; i < m_buffer.size() + 1; i++){
      ofst << std::setfill('0') << std::setw(2) << ((unsigned)m_buffer[i-1] & 0xFF) << ' ';
      if(i % 16 == 0){
        ofst << '\n';
      }
    }
  }
}
