#include "portable.h"

#include <netdb.h>
#include <ifaddrs.h>
#include <stdlib.h>
#include <unistd.h>

#include "../common/log.h"

int is_broadcast_ipv4addr(struct sockaddr* sa)
{
  if (sa->sa_family != AF_INET)
  {
    panic("not af-inet4");
  }

  struct ifaddrs *ifaddr, *ifa;
  int family;

  if (getifaddrs(&ifaddr) == -1)
  {
    panic("getifaddrs(0 failed");
  }

  /* Walk through linked list, maintaining head pointer so we
   can free list later */
  for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
  {
    if (ifa->ifa_addr == NULL)
      continue;
    family = ifa->ifa_addr->sa_family;
    if (ifa->ifa_flags & IFF_BROADCAST && family == AF_INET
        && ifa->ifa_ifu.ifu_broadaddr != NULL)
    {

      if (sa->sa_family == ifa->ifa_ifu.ifu_broadaddr->sa_family
          && ((struct sockaddr_in*) sa)->sin_addr.s_addr
              == ((struct sockaddr_in*) (ifa->ifa_ifu.ifu_broadaddr))->sin_addr.s_addr)
        return 1;
    }
  }

  freeifaddrs(ifaddr);
  return 0;
}
