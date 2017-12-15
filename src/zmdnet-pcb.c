#include "zmdnet-pcb.h"

#if defined(__FreeBSD__) && __FreeBSD_version >= 801000
VNET_DEFINE(struct base_info_t, g_base_info);
#else
struct base_info_t g_base_info = {0};
#endif
