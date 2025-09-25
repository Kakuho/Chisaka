#include "table_manager.hpp"
#include "memory/address.hpp"

namespace Firmware::Acpi{

void TableManager::Initialise(RsdpTable* addr){
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

}
