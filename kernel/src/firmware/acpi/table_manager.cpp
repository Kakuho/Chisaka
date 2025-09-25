#include "table_manager.hpp"
#include "memory/address.hpp"

namespace Firmware::Acpi{

void TableManager::Initialise(RsdpTable* addr){
  InitialiseRoot(addr);
  InitialiseTablePtrs();
}

void TableManager::InitialiseRoot(RsdpTable* addr){
  m_rsdp = addr;
  kassert(m_rsdp->IsSignatureCorrect());
  if(m_rsdp->XsdtPresent()){
    m_xsdt = reinterpret_cast<XsdtTable*>(m_rsdp->xsdtAddr);
    kassert(m_xsdt->IsSignatureCorrect());
    kout << "Xsdt Table found: " << intmode::hex << m_rsdp->xsdtAddr;
  }
  else{
    m_rsdt = reinterpret_cast<RsdtTable*>(Mem::PhysToKVirtAddr(m_rsdp->rsdtAddr));
    kassert(m_rsdt->IsSignatureCorrect());
    kassert(m_rsdt->header.length == 0x38);
    kout << "rsdt Table found: " << intmode::hex << m_rsdp->rsdtAddr << '\n';
  }
}

void TableManager::InitialiseTablePtrs(){
  for(std::size_t i = 0; i < m_rsdt->header.length; i++){
    TableHeader* nextheader = m_rsdt->VirtEntryHeader(i);
    if(nextheader->signature == FadtTable::SIG){
      m_fadt = reinterpret_cast<FadtTable*>(nextheader);
      kassert(m_fadt->IsSignatureCorrect());
    }
    if(nextheader->signature == HpetTable::SIG){
      m_hpet = reinterpret_cast<HpetTable*>(nextheader);
      kassert(m_hpet->IsSignatureCorrect());
    }
  }
}

}
