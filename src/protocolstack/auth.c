
#include "../common/log.h"
#include "../../thirdparty/libs/sodiumlib/include/sodium.h"

static int inited_sodium = 0;
void read_random(void *buf, int count)
{
  if(!inited_sodium)
  {
    if (sodium_init() == -1)
    {
      panic("init sodium failed!");
    }
    inited_sodium = 1;
  }
  randombytes_buf(buf, count);
}
