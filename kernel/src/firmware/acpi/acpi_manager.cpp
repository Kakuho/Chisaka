#include "acpi_manager.hpp"

namespace Firmware::Acpi{

AcpiManager::AcpiManager()
{
  m_rsdp = static_cast<RsdpTable*>
    (
      Limine::rsdp_request.response->address
    );
  if(m_rsdp && m_rsdp->IsSignatureCorrect()){
    if(m_rsdp->xsdtAddr){
      m_xsdt = reinterpret_cast<XsdtTable*>
        (
          m_rsdp->xsdtAddr
        );
    }
    else if(m_rsdp->rsdtAddr){
      m_rsdt = reinterpret_cast<RsdtTable*>
        (
          m_rsdp->rsdtAddr
        );
    }
  }
}

// ------------------------------------------------------ //
// Parsing
// ------------------------------------------------------ //

void ParseRsdt(){
}

} // namespace Firmware::Acpi
