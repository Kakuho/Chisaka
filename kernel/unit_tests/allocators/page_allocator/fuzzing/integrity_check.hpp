#pragma once

// Integrity checks are checks done on the allocators internal data structures

#include <cstdint>
#include <string>
#include <unordered_set>
#include <vector>
#include <iostream>
#include <format>

namespace Chisaka::Tests{

  class IntegrityCheckers{
    public:
      using IntegrityCheck_T = bool(*)();

    private:
      struct TableEntry{
        IntegrityCheck_T checker;
        std::string succMessage;
        std::string failMessage;
      };

    public:
      bool Check(bool output){
        for(auto& e: m_buffer){
          if(!e.checker()){
            if(output){
              std::cout << std::format("\tIntegrity Check Fail {} \n", e.failMessage);
            }
            return false;
          }
          else{
            if(output){
              std::cout << std::format("\tIntegrity Check Succeded {} \n", e.succMessage);
            }
          }
        }
        return true;
      }

      void AddChecker(IntegrityCheck_T checker, std::string&& succMessage, std::string&& failMessage){
        m_buffer.emplace_back(checker, succMessage, failMessage);
      }

    private:
      std::vector<TableEntry> m_buffer;
  };

  template<typename E>
  bool CheckSorted(E& handle){
      // preliminary checks
    E* head = handle.next;
    if(!head){
      return true;
    }
    if(!head->next){
      return true;
    }
    // actual algo
    E* current = head;
    E* next = current->next;
    while(next){
      if(reinterpret_cast<std::uintptr_t>(current) > reinterpret_cast<std::uintptr_t>(next)){
        return false;
      }
      current = next;
      next = current->next;
    }
    return true;
  }

  template<typename E>
  bool CheckNoCycle(E& handle){
    // preliminary checks
    E* head = handle.next;
    if(!head){
      return true;
    }
    if(!head->next){
      return true;
    }
    // actual algo
    E* current = head;
    E* next = head->next;
    while(true){
      // current is updated once, next is updated twice. On null, then we're finished, no cycles
      current = current->next;
      next = next->next;
      if(!next){
        return true;
      }
      next = next->next;
      if(!next){
        return true;
      }
      if(reinterpret_cast<std::uintptr_t>(current) == reinterpret_cast<std::uintptr_t>(next)){
        // if at any point the two pointers compare equal, then we have a cycle
        return false; 
      }
    }
    return true;
  }

  template<typename E>
  bool CheckNoDupes(E& handle){
    // preliminary checks
    E* head = handle.next;
    if(!head){
      return true;
    }
    // actual algo
    std::unordered_set<void*> visited;
    E* current = head;
    visited.insert(current);
    while(current->next){
      if(visited.contains(current->next)){
        return false;
      }
      current = current->next;
    }
    return true;
  }

}
