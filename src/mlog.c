#include "mlog.h"
#include "zmdnet-plateform.h"

#if defined(ZMDNET_LOCAL_TRACE_BUF) || defined(__APPLE__)
void zmdnet_log_trace(uint32_t fr, const char *str, uint32_t a, uint32_t b,
    uint32_t c, uint32_t d, uint32_t e, uint32_t f)
{
  //todo
}

void zmdnet_print_addr(struct sockaddr *sa)
{
#ifdef ZMDNET_SUPPORT_IPV6
#if defined(__FreeBSD__) && __FreeBSD_version >= 700000
  char ip6buf[INET6_ADDRSTRLEN];
#endif
#endif

  switch (sa->sa_family)
  {
#ifdef ZMDNET_SUPPORT_IPV6
  case AF_INET6:
  {
    struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *) sa;
    zmdnet_printf("IPv6 address: %x:%x:%x:%x:%x:%x:%x:%x:port:%d scope:%u\n",
        ntohs(sin6->sin6_addr.s6_addr16[0]),
        ntohs(sin6->sin6_addr.s6_addr16[1]),
        ntohs(sin6->sin6_addr.s6_addr16[2]),
        ntohs(sin6->sin6_addr.s6_addr16[3]),
        ntohs(sin6->sin6_addr.s6_addr16[4]),
        ntohs(sin6->sin6_addr.s6_addr16[5]),
        ntohs(sin6->sin6_addr.s6_addr16[6]),
        ntohs(sin6->sin6_addr.s6_addr16[7]), ntohs(sin6->sin6_port),
        sin6->sin6_scope_id);
    break;
  }
#endif
#ifdef ZMDNET_SUPPORT_IPV4
  case AF_INET:
  {
    struct sockaddr_in *sin;
    unsigned char *p;
    sin = (struct sockaddr_in *) sa;
    p = (unsigned char *) &sin->sin_addr;
    zmdnet_printf("IPv4 address: %u.%u.%u.%u:%d\n", p[0], p[1], p[2], p[3],
        ntohs(sin->sin_port));
    break;
  }
#endif
  case AF_CONN:
  {
    struct sockaddr_conn *sconn = (struct sockaddr_conn *) sa;
    zmdnet_printf("AF_CONN address: %p\n", sconn->sconn_addr);
    break;
  }
  default:
    zmdnet_printf("?\n")
    ;
    break;
  }
}

#endif
