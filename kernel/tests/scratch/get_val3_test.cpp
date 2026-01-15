#include "doctest.h"
#include "src/scratch/scratch.hpp"

TEST_CASE("testing scratch's getval") {
  Scratch scratch;
  CHECK(scratch.GetVal3() == 3);
}
