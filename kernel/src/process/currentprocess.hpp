#pragma once

#include "process.hpp"

namespace Proc{
  class ProcessHandle{
    public:
      void operator=(ProcessHandle&& src);

    private:
      Process* m_process;
  };

  struct CurrentProcess{
    Process process;
  };
}
