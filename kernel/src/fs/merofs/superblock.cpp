#include "superblock.hpp"

namespace Chisaka::Merofs{

SuperBlock::SuperBlock()
  : 
    totalPhysBlocks{0xd4d4'd4d4'd4d4'd4d4},
    totalInodeBlocks{0xdead'beef'dead'beef},
    freeInodes{0xface'face'face'face},
    freeDataBlocks{0xeeee'eeee'eeee'eeee},
    freeInodeHead{0xfafa'fafa'fafa'fafa},
    freeDataBlockHead{0xb2b2'b2b2'b2b2'b2b2}
{
  for(auto& i: padding){
    i = 0;
  }
}

}
