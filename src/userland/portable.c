#include "portable.h"
#include "../common/log.h"

//#include <netdb.h>
//#include <ifaddrs.h>
//#include <stdlib.h>
//#include <unistd.h>

//int is_broadcast_ipv4addr(struct sockaddr* sa)
//{
//    if (sa->sa_family != AF_INET)
//    {
//        panic("not af-inet4");
//    }
//
//    struct ifaddrs *ifaddr, *ifa;
//    int family;
//
//    if (getifaddrs(&ifaddr) == -1)
//    {
//        panic("getifaddrs(0 failed");
//    }
//
//    /* Walk through linked list, maintaining head pointer so we
//    can free list later */
//    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
//    {
//        if (ifa->ifa_addr == NULL)
//            continue;
//        family = ifa->ifa_addr->sa_family;
//        if (ifa->ifa_flags & IFF_BROADCAST && family == AF_INET
//            && ifa->ifa_ifu.ifu_broadaddr != NULL)
//        {
//
//            if (sa->sa_family == ifa->ifa_ifu.ifu_broadaddr->sa_family
//                && ((struct sockaddr_in*) sa)->sin_addr.s_addr
//                == ((struct sockaddr_in*) (ifa->ifa_ifu.ifu_broadaddr))->sin_addr.s_addr)
//                return 1;
//        }
//    }
//
//    freeifaddrs(ifaddr);
//    return 0;
//}

#define uHZ 1000
/* See zmdnet-env.h for comments about these variables */
int maxsockets = 25600;
int hz = uHZ;
int ip_defttl = 64;
int ipport_firstauto = 49152, ipport_lastauto = 65535;
int nmbclusters = 65536;
int ip_id = 0; /*__Userspace__ TODO Should it be initialized to zero? */

void* hash_init(int elements, u_long *hashmask)
{
    zmdnet_assert(elements > 0, ("%s: bad elements %d", __func__, elements));
    long hashsize = 1;
    int i = 0;

    LIST_HEAD(lh, generic) *hashtbl;
    for (; hashsize <= elements; hashsize <<= 1)
        continue;
    hashsize >>= 1;

    zmdnet_malloc(hashtbl, struct lh, hashsize);
    for (; i < hashsize; i++)
        LIST_INIT(&hashtbl[i]);
    *hashmask = hashsize - 1;
    return (hashtbl);
}

void hash_destroy(void *vhashtbl, u_long hashmask)
{
    LIST_HEAD(lh, generic)
        *hashtbl = vhashtbl, *hp = vhashtbl;
    for (; hp <= &hashtbl[hashmask]; hp++)
        if (!LIST_EMPTY(hp))
        {
            zmdnet_debug(0, "%s at line %d: bucket list not empty \n", __func__,
                __LINE__);
            return;
        }
    zmdnet_free(hashtbl);
}

