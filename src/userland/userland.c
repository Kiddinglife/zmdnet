#include "userland.h"

#define uHZ 1000
/* See zmdnet-env.h for comments about these variables */
int maxsockets = 25600;
int hz = uHZ;
int ip_defttl = 64;
int ipport_firstauto = 49152, ipport_lastauto = 65535;
int nmbclusters = 65536;
int  ip_id = 0; /*__Userspace__ TODO Should it be initialized to zero? */

void* hash_init(int elements, u_long *hashmask)
{
    zmdnet_assert(elements > 0, ("%s: bad elements %d", __func__, elements));
    long hashsize = 1;
    int i = 0;

    LIST_HEAD(listhead, generic) *hashtbl;
    for (; hashsize <= elements; hashsize <<= 1)
        continue;
    hashsize >>= 1;

    zmdnet_malloc(hashtbl, LIST_HEAD(listhead, generic), hashsize);
    for (; i < hashsize; i++)
        LIST_INIT(&hashtbl[i]);
    *hashmask = hashsize - 1;
    return (hashtbl);
}

void hash_destroy(void *vhashtbl, u_long hashmask)
{
    LIST_HEAD(listhead, generic) *hashtbl = vhashtbl, *hp = vhashtbl;
    for (; hp <= &hashtbl[hashmask]; hp++)
        if (!LIST_EMPTY(hp))
        {
            zmdnet_debug(0, "%s at line %d: bucket list not empty \n", __func__, __LINE__);
            return;
        }
    zmdnet_free(hashtbl);
}

#ifdef _WIN32
int read_random(void *buf, int count)
{
    HMODULE hLib = LoadLibrary("ADVAPI32.DLL");
    if (hLib)
    {
        BOOLEAN(APIENTRY *pfn)(void*, ULONG) =
            (BOOLEAN(APIENTRY *)(void*, ULONG))GetProcAddress(hLib, "SystemFunction036");
        if (pfn)
            if (!pfn(buf, count))
                panic("%s:line%d:pfn() failed !\n", __func__, __LINE__);
        FreeLibrary(hLib);
    }
    return (count);
}
#else
#include <bsd/stdlib.h>
int read_random(void *buf, int count)
{
    arc4random_buf(buf, count);
    return (count);
}
#endif