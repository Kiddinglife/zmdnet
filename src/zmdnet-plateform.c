
#define uHZ 1000
/* See zmdnet-env.h for comments about these variables */
int maxsockets = 25600;
int hz = uHZ;
int ip_defttl = 64;
int ipport_firstauto = 49152, ipport_lastauto = 65535;
int nmbclusters = 65536;
int  ip_id = 0; /*__Userspace__ TODO Should it be initialized to zero? */