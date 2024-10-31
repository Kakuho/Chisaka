#ifndef SEGMENT_MANAGER_HPP
#define SEGMENT_MANAGER_HPP

#include "structs/gdt_table.hpp"
#include "structs/gdt_descriptor.hpp"
#include "structs/segment_descriptor.hpp"
#include "structs/task_descriptor.hpp"
#include "structs/task_segment.hpp"
#include "structs/attributes.hpp"

extern "C" void segreg_reload();

namespace X8664{
  class SegmentManager{
    public:
      //---------------------------------------------------------------//
      //  Lifetime
      //---------------------------------------------------------------//

      SegmentManager() noexcept;

    private:
      // private initialisation routines
      //    might be useful?
      // n.b. we pressume a flat layout 
      void SetSegments() noexcept; 
      void SetNullSegment() noexcept;
      void SetKernelCodeSegment() noexcept;
      void SetKernelDataSegment() noexcept;
      void SetUserCodeSegment() noexcept;
      void SetUserDataSegment() noexcept;

    public:
      //---------------------------------------------------------------//
      //  Segments
      //---------------------------------------------------------------//

      [[nodiscard]] SegmentDescriptor& 
      NullSegment() noexcept{return m_gdt.NullSegment();}

      [[nodiscard]] SegmentDescriptor& 
      KernelCodeSegment() noexcept{return m_gdt.KernelCodeSegment();}

      [[nodiscard]] SegmentDescriptor& 
      KernelDataSegment() noexcept{return m_gdt.KernelDataSegment();}

      [[nodiscard]] SegmentDescriptor& 
      UserCodeSegment() noexcept{return m_gdt.UserCodeSegment();}

      [[nodiscard]] SegmentDescriptor& 
      UserDataSegment() noexcept{return m_gdt.UserDataSegment();}

      [[nodiscard]] TaskDescriptor& 
      TaskSegmentDescriptor() noexcept{return m_gdt.TaskSegment();}

      //---------------------------------------------------------------//
      //  Table Switching
      //---------------------------------------------------------------//

      void LoadCurrent()
      { m_gdtr.SwitchLoad(m_gdt); }

      void ReloadSegmentRegisters()
      { segreg_reload(); }

      void ReloadTaskRegister()
      { segreg_reload(); }

    private:
      GdtTable m_gdt;
      GdtDescriptor m_gdtr;
      TaskSegment m_taskSegment;
  };
}

#endif
