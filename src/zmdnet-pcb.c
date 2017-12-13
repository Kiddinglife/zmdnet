#include "zmdnet-pcb.h"

#if defined(__FreeBSD__) && __FreeBSD_version >= 801000
VNET_DEFINE(struct zmdnet_base_info, g_base_info);
#else
struct zmdnet_base_info g_base_info;
#endif
