#pragma once

// class to encapsulate fuzz testing of page allocators

#include <cstddef>
#include <string>
#include <queue>
#include <unordered_set>
#include <random>
#include <optional>
#include <format>
#include <fstream>
#include <stdexcept>

namespace Chisaka::Tests{
  enum class FuzzAction{
    Allocate, 
    Deallocate
  };
    
  constexpr const char* ToString(FuzzAction action){
    switch(action){
      case FuzzAction::Allocate:
        return "Allocate";
      case FuzzAction::Deallocate:
        return "Deallocate";
      default:
        return "???";
    }
  }
  
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
    std::size_t seed;
  };

  template<typename Generator>
  struct FuzzIndexPicker{
    FuzzIndexPicker(Generator& gen, std::size_t elements):
      gen{gen},
      d{0, elements-1}{
    }

    FuzzIndexPicker(std::size_t elements):
      d{0, elements-1}{
    }

    void SetGenerator(Generator& generator){
      gen = generator;
    }

    std::size_t Gen(){ 
      return d(gen);
    }

    Generator& gen;
    std::uniform_int_distribution<std::size_t> d;
  };

  class PageFuzzer{
    using Allocator = void*(*)();
    using Deallocator = void(*)(void*);

    struct ValidationResult{
      bool result;
      std::optional<std::size_t> reason;
      static ValidationResult Failure(std::size_t reason){ return ValidationResult{false, reason};}
      static ValidationResult Success(){ return ValidationResult{true, std::nullopt};}
    };

    struct FuzzResult{
      FuzzAction action;
      void* address;
      ValidationResult result;
    };

    public:
      PageFuzzer(std::size_t seed, float balance);

      void Run(std::size_t iterations);
      void RunDeallocateOnly(std::size_t iterations);
      void RunAllocateOnly(std::size_t iterations);

      constexpr void SetAllocationFunction(Allocator alloc){ m_alloc = alloc;}
      constexpr void SetDeallocationFunction(Deallocator dealloc){ m_dealloc = dealloc;}

      ValidationResult ValidateAllocated(void* page); 
      ValidationResult ValidateDeallocated(void* page); 

      void DumpHistory(const std::string& file_name);

    private:
      FuzzAction GenAction() const;
      void* PickFromAllocated() const;
      void AddToHistory(FuzzAction action, void* page, ValidationResult result);

      bool IsAllocated(void* page);

    private:
      Allocator m_alloc;
      Deallocator m_dealloc;
      mutable FuzzActionGenerator m_actionGenerator;
      std::queue<FuzzResult> m_history;
      std::unordered_set<void*> m_allocated;
  };
}
