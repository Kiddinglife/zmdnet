#include "../common/log.h"
#include "../userland/zmdnetint.h"

#ifndef _WIN32
#include <arpa/inet.h>
#endif

#if defined(ZMDNET_LOCAL_TRACE_BUF) || defined(__APPLE__)
void zmdnet_log_trace(uint32_t fr, const char *str, uint32_t a, uint32_t b,
    uint32_t c, uint32_t d, uint32_t e, uint32_t f)
{
  //todo
}
#endif


void zmdnet_print_addr(struct sockaddr *sa)
{
  char ipbuf[INET6_ADDRSTRLEN];
  struct sockaddr_in6 *sin6;
  struct sockaddr_in *sin;
  struct sockaddr_conn *sconn;
  if(sa == NULL)
    printf("sa null\n");
  switch (sa->sa_family)
  {
#ifdef ZMDNET_SUPPORT_IPV6
  case AF_INET6:
    sin6 = (struct sockaddr_in6 *) sa;
    inet_ntop(AF_INET6, &(sin6->sin6_addr), ipbuf, INET6_ADDRSTRLEN);
    zmdnet_printf("IPv6=%s port=%d scope=%u\n", ipbuf, ntohs(sin6->sin6_port), sin6->sin6_scope_id);
    break;
#endif
#ifdef ZMDNET_SUPPORT_IPV4
  case AF_INET:
    sin = (struct sockaddr_in *) sa;
    inet_ntop(AF_INET, &(sin->sin_addr), ipbuf, INET6_ADDRSTRLEN);
    zmdnet_printf("IPv4=%s port=%d\n", ipbuf,ntohs(sin->sin_port));
    break;
#endif
  case AF_CONN:
    sconn = (struct sockaddr_conn *) sa;
    zmdnet_printf("AF_CONN address: %p\n", sconn->sconn_addr);
    break;
  default:
    zmdnet_printf("?\n");
    break;
  }
}
