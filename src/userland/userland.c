#include "userland.h"

// need sctphdr to get port in ZMDNET_IP_OUTPUT. sctphdr defined in zmdnet.h
#include "../zmdnet.h"
#include "../common/log.h"
#include "../common/atomic.h"
// typedef struct callout timer_t which is used in the timer
// related functions such as ZMDNET_TIMER_INIT
// todo port to userspace at line 799
#include "../common/callout.h"

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
