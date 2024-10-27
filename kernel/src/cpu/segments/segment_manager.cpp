#include "segment_manager.hpp"

namespace X8664{

SegmentManager::SegmentManager() noexcept:
  m_gdt{},
  m_gdtr{},
  m_taskSegment{} 
{
  using namespace SegmentAttributes;

  //  Old Style:
  //
  //  auto nullsegment = SegmentDescriptor{
  //    0, 0, 0, 0
  //  };
  //
  //  auto kernelCode = SegmentDescriptor{
  //    0, 0xFFFF, 0b1001'1010, 0b1010'1111
  //  };
  //
  //  auto kernelData = SegmentDescriptor{
  //    0, 0xFFFF, 0b1001'0010, 0b1100'1111
  //  };
  //
  //  auto userCode = SegmentDescriptor{
  //    0, 0xFFFF, 0b1111'1010, 0b1010'1111
  //  };
  //
  //  auto userData = SegmentDescriptor{
  //    0, 0xFFFF, 0b1111'0010, 0b1100'1111
  //  };
  //
  //  auto taskDesc = TaskDescriptor{
  //    &m_taskSegment, sizeof(m_taskSegment) - 1
  //  };
  // ------------------------------------------------------ //

  //  Null Segment

  //  auto nullsegment = SegmentDescriptor{
  //    0, 0, 0, 0
  //  };

  auto nullsegment = SegmentDescriptor
  {
    BaseLimit{
      .base = 0,
      .limit = 0
    },
    AccessByte::Null(),
    Flags::Null()
  };

  // Kernel Segments

  //  auto kernelCode = SegmentDescriptor{
  //    0, 0xFFFF, 0b1001'1010, 0b1010'1111
  //  };

  auto kernelCode  = SegmentDescriptor
  {
    BaseLimit{
      .base = 0,
      .limit = 0xFFFFF
    },
    AccessByte{
      .present = true,
      .privilaged = true,
      .isSystemSegment = false,
      .type = 0b1010
    },
    Flags{
      .isByteGranular = false,
      .db = false,
      .longModeSegment = true,
      .avl = false
    }
  };

  //  auto kernelData = SegmentDescriptor{
  //    0, 0xFFFF, 0b1001'0010, 0b1100'1111
  //  };

  auto kernelData  = SegmentDescriptor
  {
    BaseLimit{
      .base = 0,
      .limit = 0xFFFFF
    },
    AccessByte{
      .present = true,
      .privilaged = true,
      .isSystemSegment = false,
      .type = 0b0010
    },
    Flags{
      .isByteGranular = false,
      .db = true,
      .longModeSegment = false,
      .avl = false
    }
  };

  // User Segments

  //  auto userCode = SegmentDescriptor{
  //    0, 0xFFFF, 0b1111'1010, 0b1010'1111
  //  };

  auto userCode  = SegmentDescriptor
  {
    BaseLimit{
      .base = 0,
      .limit = 0xFFFFF
    },
    AccessByte{
      .present = true,
      .privilaged = false,
      .isSystemSegment = false,
      .type = 0b1010
    },
    Flags{
      .isByteGranular = false,
      .db = false,
      .longModeSegment = true,
      .avl = false
    }
  };

  //  auto userData    = SegmentDescriptor{
  //    0, 0xFFFF, 0b1111'0010, 0b1100'1111
  //  };

  auto userData  = SegmentDescriptor
  {
    BaseLimit{
      .base = 0,
      .limit = 0xFFFFF
    },
    AccessByte{
      .present = true,
      .privilaged = false,
      .isSystemSegment = false,
      .type = 0b0010
    },
    Flags{
      .isByteGranular = false,
      .db = true,
      .longModeSegment = false,
      .avl = false
    }
  };

  // Task Segment
  
  //  auto taskDesc = TaskDescriptor{
  //    &m_taskSegment, sizeof(m_taskSegment) - 1
  //  };

  auto taskDesc = TaskDescriptor
  {
    BaseLimit{
      .base = &m_taskSegment,
      .limit = sizeof(m_taskSegment) - 1
    },
    TaskAttributes{
      .isByteGranular = false,
      .avl = false,
      .present = true,
      .privilaged = true,
      .busy = false
    }
  };

  m_gdt = GdtTable{
    nullsegment, 
    kernelCode, 
    kernelData, 
    userCode, 
    userData, 
    taskDesc
  };
};

} // namespace x8664
