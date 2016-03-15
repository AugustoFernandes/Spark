#ifndef NETDEVICE
#define NETDEVICE

#include <stdbool.h>

struct llOptions{
    char iface_name[IFNAMSIZ];
    int sfd;
    unsigned int buffl;
    bool bsd_immediate;
};


bool get_burnedin_mac(int sd, char *iface_name, struct sockaddr *hwa);

bool get_flags(int sd, char *iface_name, short *flag);

bool get_hwaddr(int sd, char *iface_name, struct sockaddr *hwaddr);

bool set_flags(int sd, char *iface_name, short flags);

bool set_hwaddr(int sd, char *iface_name, struct sockaddr *hwaddr);

void init_lloptions(struct llOptions *llo, char *iface_name, unsigned int buffl);

int llsocket(struct llOptions *llo);

#endif