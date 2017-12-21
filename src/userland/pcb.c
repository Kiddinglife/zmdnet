#include "pcb.h"

static struct base_info_t global_base_info = {0};
struct base_info_t* g_base_info()
{
    if(global_base_info.debug_printf_func != 0)
      printf("not zero");
    return &global_base_info;
}
