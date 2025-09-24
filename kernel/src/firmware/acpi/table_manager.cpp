#include "table_manager.hpp"

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
    m_rsdt = reinterpret_cast<RsdtTable*>(m_rsdp->rsdtAddr);
    kassert(m_rsdt->IsSignatureCorrect());
    kout << "rsdt Table found: " << intmode::hex << m_rsdp->rsdtAddr << '\n';
  }
}

}
