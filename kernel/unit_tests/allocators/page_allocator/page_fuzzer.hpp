#pragma once

// class to encapsulate fuzz testing of page allocators

#include <cstddef>
#include <string>
#include <stack>
#include <unordered_set>
#include <random>

namespace Chisaka::Tests{
  enum class FuzzAction{Allocate, Deallocate};
  
  struct FuzzActionGenerator{
    FuzzActionGenerator(std::size_t seed, float balance):
      gen{std::mt19937{seed}},
      d{balance}{
    }

    FuzzAction Gen(){ 
      if(d(gen) == false){
        return FuzzAction::Allocate;
      }
      else{
        return FuzzAction::Deallocate;
      }
    }

    std::mt19937 gen;
    std::bernoulli_distribution d;
  };

  struct FuzzAllocatedPicker{
    FuzzAllocatedPicker(std::size_t seed, std::size_t elements):
      gen{std::mt19937{seed}},
      d{balance}{
    }

    FuzzAction Gen(){ 
      if(d(gen) == false){
        return FuzzAction::Allocate;
      }
      else{
        return FuzzAction::Deallocate;
      }
    }

    std::mt19937 gen;
    std::bernoulli_distribution d;
  };

  class PageFuzzer{
    using Allocator = void*(*)();
    using Deallocator = void(*)(void*);

    struct FuzzResult{
      FuzzAction action;
      bool result;
    };

    public:
      void Run(std::size_t iterations);
      void RunDeallocateOnly(std::size_t iterations);
      void RunAllocateOnly(std::size_t iterations);

      constexpr void SetAllocationFunction(Allocator alloc){ m_alloc = alloc;}
      constexpr void SetDeallocationFunction(Deallocator dealloc){ m_dealloc = dealloc;}

      void DumpHistory(const std::string& name);

      bool ValidateAllocated(void* page); 
      bool ValidateDeallocated(void* page); 

    private:
      FuzzAction GenAction() const;
      void* PickFromAllocated() const;
      void AddToHistory(FuzzAction action, bool result);

      bool IsAllocated(void* page);

    private:
      Allocator m_alloc;
      Deallocator m_dealloc;
      FuzzActionGenerator m_actionGenerator;
      std::stack<FuzzResult> m_history;
      std::unordered_set<void*> m_allocated;
  };
}
