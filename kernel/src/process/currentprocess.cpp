#include "currentprocess.hpp"

namespace Proc{

void ProcessHandle::operator=(ProcessHandle&& src){
  this->m_process = src.m_process;
  src.m_process = nullptr;
}

}
