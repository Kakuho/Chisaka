#ifndef DRIVERS_PCI_IDENTIFICATION_HPP
#define DRIVERS_PCI_IDENTIFICATION_HPP

#include <cstdint>

#include "drivers/serial/kostream.hpp"

namespace Drivers::Pci{
  enum class a{ yea};

  inline void PrintIdentifier(std::uint32_t value){
    std::uint16_t vendorId = value & 0xFFFF;
    std::uint16_t deviceId = value >> 16;
    kout << "[" << vendorId << ":" << deviceId << "] ";
    switch(value){
      case 0x29c0'8086:
        kout << "Intel - 82G33/G31/P35/P31 Express DRAM Controller";
        break;
      case 0x10d3'8086:
        kout << "Intel - 82574L Gigabit Network Connection";
        break;
      case 0x2918'8086:
        kout << "Intel - 82801IB (ICH9) LPC Interface Controller";
        break;
      case 0x2922'8086:
        kout << "Intel - 82801IR/IO/IH (ICH9R/DO/DH) 6 port SATA Controller [AHCI mode]";
        break;
      case 0x2930'8086:
        kout << "Intel - 82801I (ICH9 Family) SMBus Controller";
        break;
      default:
        kout << "Unkown Vendor / Device Identitifer";
        break;
    }
  }

  inline void PrintInterruptInfo(std::uint32_t value){
    std::uint8_t interruptLine = value & 0xFF;
    std::uint16_t interruptPin = (value >> 8) & 0xFF;
    kout << "Interrupt Line:: " <<  interruptLine << "\nInterrupt Pin:: " << interruptPin << '\n';
  }


  inline void PrintClass(std::uint32_t value){
    std::uint8_t baseClass = value >> 24;
    std::uint8_t subClass = (value >> 16) & 0xFF;
    std::uint8_t interface = (value >> 8) & 0xFF;
    kout << "[" << baseClass << ":" << subClass << ":" << interface << "] ";

    switch(baseClass){
      case 0x01: 
        {
          // Mass Storage Controller
          kout << "Mass Storage Controller - ";
          switch(subClass){
            case 0x06:
              {
                switch(interface){
                  case 0x00:
                    kout << "Serial ATA Controller, Vendor Specific Interface";
                    break;
                  case 0x01:
                    kout << "Serial ATA Controller, AHCI Interface";
                    break;
                  case 0x02:
                    kout << "Serial ATA Controller, Serial Storage Bus Interface";
                    break;
                  default:
                    kout << "Unknown Controller";
                    break;
                }
                break;
              }
            default:
              kout << "Unkown Subclass - " << subClass;
              break;
          }
          break;
        }
      //-------------------------------------------------------------
      case 0x02: 
        {
          // Network Controller
          kout << "Network Controller - ";
          switch(subClass){
            case 0x00:
              {
                switch(interface){
                  case 0x00:
                    kout << "Ethernet Controller";
                    break;
                  default:
                    kout << "Unknown Controller";
                    break;
                }
                break;
              }
            default:
              kout << "Unkown Subclass - " << subClass;
              break;
          }
          break;
        }
      //-------------------------------------------------------------
      case 0x03: 
        {
          // Display Controllers
          kout << "Display Controller - ";
          switch(subClass){
            case 0x00:
              {
                switch(interface){
                  case 0x00:
                    kout << "VGA Compatible Controller";
                    break;
                  case 0x01:
                    kout << "8514 Compatible Controller";
                    break;
                  default:
                    kout << "Unknown Controller";
                    break;
                }
                break;
              }
            default:
              kout << "Unkown Subclass - " << subClass;
              break;
          }
          break;
        }
      //-------------------------------------------------------------
      case 0x06: 
        {
          // PCI Bridge
          kout << "PCI Bridge Device - ";
          switch(subClass){
            case 0x00:
              {
                switch(interface){
                  case 0x00:
                    kout << "Host Bridge";
                    break;
                  default:
                    kout << "Unknown Bridge";
                }
                break;
              }
            case 0x01:
              {
                switch(interface){
                  case 0x00:
                    kout << "ISA Bridge";
                    break;
                  default:
                    kout << "Unknown Bridge";
                    break;
                }
                break;
              }
            default:
              kout << "Unkown Subclass - " << subClass;
              break;
          }
          break;
        }
      //-------------------------------------------------------------
      case 0x0c: 
        {
          // Network Controller
          kout << "Serial Bus Controller - ";
          switch(subClass){
            case 0x05:
              {
                switch(interface){
                  case 0x00:
                    kout << "SMBus";
                    break;
                  default:
                    kout << "Unknown Controller";
                    break;
                }
                break;
              }
            default:
              kout << "Unkown Subclass - " << subClass;
              break;
          }
          break;
        }
    }
  }

}

#endif
