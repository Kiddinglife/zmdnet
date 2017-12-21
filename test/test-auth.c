//#include <winsock2.h>
//#include <ws2tcpip.h>
//
//int doit()
//{
//    SOCKET sd = WSASocket(AF_INET, SOCK_DGRAM, 0, 0, 0, 0);
//    if (sd == SOCKET_ERROR) {
//        cerr << "Failed to get a socket. Error " << WSAGetLastError() <<
//            endl; return 1;
//    }
//
//    INTERFACE_INFO InterfaceList[20];
//    unsigned long nBytesReturned;
//    if (WSAIoctl(sd, SIO_GET_INTERFACE_LIST, 0, 0, &InterfaceList,
//        sizeof(InterfaceList), &nBytesReturned, 0, 0) == SOCKET_ERROR) {
//        cerr << "Failed calling WSAIoctl: error " << WSAGetLastError() <<
//            endl;
//        return 1;
//    }
//
//    int nNumInterfaces = nBytesReturned / sizeof(INTERFACE_INFO);
//    cout << "There are " << nNumInterfaces << " interfaces:" << endl;
//    for (int i = 0; i < nNumInterfaces; ++i) {
//        cout << endl;
//
//        sockaddr_in *pAddress;
//        pAddress = (sockaddr_in *) & (InterfaceList[i].iiAddress);
//        cout << " " << inet_ntoa(pAddress->sin_addr);
//
//        pAddress = (sockaddr_in *) & (InterfaceList[i].iiBroadcastAddress);
//        cout << " has bcast " << inet_ntoa(pAddress->sin_addr);
//
//        pAddress = (sockaddr_in *) & (InterfaceList[i].iiNetmask);
//        cout << " and netmask " << inet_ntoa(pAddress->sin_addr) << endl;
//
//        cout << " Iface is ";
//        u_long nFlags = InterfaceList[i].iiFlags;
//        if (nFlags & IFF_UP) cout << "up";
//        else                 cout << "down";
//        if (nFlags & IFF_POINTTOPOINT) cout << ", is point-to-point";
//        if (nFlags & IFF_LOOPBACK)     cout << ", is a loopback iface";
//        cout << ", and can do: ";
//        if (nFlags & IFF_BROADCAST) cout << "bcast ";
//        if (nFlags & IFF_MULTICAST) cout << "multicast ";
//        cout << endl;
//    }
//
//    return 0;
//}

//#include "../src/common/log.h"
//
//#ifdef _WIN32
//#define _CRT_SECURE_NO_WARNINGS
//#endif
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <stdarg.h>
//#ifndef _WIN32
//#include <unistd.h>
//#endif
//#include <sys/types.h>
//#ifndef _WIN32
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#else
//#include <io.h>
//#endif
//
//void debug_printf(const char *format, ...)
//{
//    va_list ap;
//    va_start(ap, format);
//    vprintf(format, ap);
//    va_end(ap);
//}
//
//
//int main()
//{
//    //WSADATA WinsockData;
//    //if (WSAStartup(MAKEWORD(2, 2), &WinsockData) != 0) {
//    //    cerr << "Failed to find Winsock 2.2!" << endl;
//    //    return 2;
//    //}
//
//    //int nRetVal = doit();
//
//    //WSACleanup();
//
//    //return nRetVal;
//  g_base_info.debug_printf_func = debug_printf;
//  zmdnet_printf("test zmdnetprintf %d\n", 12);
//}
//#define _GNU_SOURCE     /* To get defns of NI_MAXSERV and NI_MAXHOST */

//#ifdef _WIN32
//#define _CRT_SECURE_NO_WARNINGS
//#endif
//
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <stdarg.h>
//#include "../src/common/log.h"
//
//#include <netdb.h>
//#include <ifaddrs.h>
//#include <stdlib.h>
//#include <unistd.h>
//#include <linux/if_link.h>
//
//  struct ifaddrs *ifaddr, *ifa;
//  int family, s, n;
//  char host[NI_MAXHOST];
//
//  if (getifaddrs(&ifaddr) == -1)
//  {
//    perror("getifaddrs");
//    exit(EXIT_FAILURE);
//  }
//
//  /* Walk through linked list, maintaining head pointer so we
//   can free list later */
//
//  for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++)
//  {
//    if (ifa->ifa_addr == NULL)
//      continue;
//
//    family = ifa->ifa_addr->sa_family;
//
//    /* Display interface name and family (including symbolic
//     form of the latter for the common families) */
//
//    printf("%-8s %s (%d)\n", ifa->ifa_name,
//        (family == AF_PACKET) ? "AF_PACKET" : (family == AF_INET) ? "AF_INET" :
//        (family == AF_INET6) ? "AF_INET6" : "???", family);
//
//    /* For an AF_INET* interface address, display the address */
//    if (family == AF_INET || family == AF_INET6)
//    {
//      if(family == AF_INET6)
//        printf("hello\n");
//
//      if(ifa->ifa_flags & IFF_BROADCAST && family == AF_INET)
//      {
//        zmdnet_print_addr(ifa->ifa_ifu.ifu_broadaddr);
//      }
//
//      s = getnameinfo(ifa->ifa_addr,
//          (family == AF_INET) ?
//              sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6), host,
//          NI_MAXHOST,
//          NULL, 0, NI_NUMERICHOST);
//      if (s != 0)
//      {
//        printf("getnameinfo() failed: %s\n", gai_strerror(s));
//        exit(EXIT_FAILURE);
//      }
//
//      printf("\t\taddress: <%s>\n", host);
//
//    }
//    else if (family == AF_PACKET && ifa->ifa_data != NULL)
//    {
//      struct rtnl_link_stats *stats = ifa->ifa_data;
//
//      printf("\t\ttx_packets = %10u; rx_packets = %10u\n"
//          "\t\ttx_bytes   = %10u; rx_bytes   = %10u\n", stats->tx_packets,
//          stats->rx_packets, stats->tx_bytes, stats->rx_bytes);
//    }
//  }
//
//  freeifaddrs(ifaddr);
//  exit(EXIT_SUCCESS);

#include "../src/protocolstack/auth.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

static void debug_printf(const char *format, ...)
{
  va_list ap;
  va_start(ap, format);
  vprintf(format, ap);
  va_end(ap);
}

static void test_read_random()
{
  printf("test_read_random()\n");
  unsigned int v;
  char buf[1024];
  read_random(buf, 1024);
  memcpy(&v, buf, 4);
  printf("%u\n", v);
}

#include "../src/common/log.h"
int main(int argc, char *argv[])
{
  g_base_info.debug_printf_func = debug_printf;
  test_read_random();
  return 0;
}

