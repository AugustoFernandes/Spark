/*
* <icmp, part of Spark.>
* Copyright (C) <2015-2016> <Jacopo De Luca>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "ipv4.h"
#include "icmp4.h"

struct IcmpHeader *build_icmp4_echo_request(unsigned char *buff, unsigned short id, unsigned short seqn,
                                            struct Ipv4Header *ipv4Header, unsigned long paysize,
                                            unsigned char *payload) {
    return buinj_icmp4_echo_request(buff, true, id, seqn, ipv4Header, paysize, payload);
}

struct IcmpHeader *build_icmp4_packet(unsigned char type, unsigned char code, struct Ipv4Header *ipv4Header,
                                      unsigned long paysize,
                                      unsigned char *payload) {
    unsigned long size = ICMP4HDRSIZE + paysize;
    struct IcmpHeader *ret = NULL;
    if ((ret = (struct IcmpHeader *) malloc(size)) == NULL)
        return NULL;
    injects_icmp4_header((unsigned char *) ret, type, code);
    if (payload != NULL) {
        memcpy(ret->data, payload, paysize);
        ret->chksum = icmp4_checksum(ret, ipv4Header);
    }
    return ret;
}

struct IcmpHeader *injects_icmp4_echo_request(unsigned char *buff, unsigned short id, unsigned short seqn,
                                              struct Ipv4Header *ipv4Header, unsigned long paysize,
                                              unsigned char *payload) {
    return buinj_icmp4_echo_request(buff, false, id, seqn, ipv4Header, paysize, payload);
}

struct IcmpHeader *injects_icmp4_header(unsigned char *buff, unsigned char type, unsigned char code) {
    struct IcmpHeader *ret = (struct IcmpHeader *) buff;
    memset(ret, 0x00, ICMP4HDRSIZE);
    ret->type = type;
    ret->code = code;
    return ret;

}

static struct IcmpHeader *buinj_icmp4_echo_request(unsigned char *buff, bool memalloc, unsigned short id,
                                                            unsigned short seqn,
                                                            struct Ipv4Header *ipv4Header, unsigned long paysize,
                                                            unsigned char *payload) {
    struct IcmpHeader *icmp;
    if (memalloc)
        icmp = build_icmp4_packet(ICMPTY_ECHO_REQUEST, 0, NULL, paysize, NULL);
    else {
        icmp = (struct IcmpHeader *) buff;
        icmp->type = ICMPTY_ECHO_REQUEST;
        icmp->code = 0;
    }
    if (icmp == NULL)
        return NULL;
    icmp->echo.id = htons(id);
    icmp->echo.sqn = htons(seqn);
    if (payload != NULL)
        memcpy(icmp->data, payload, paysize);
    else {
        FILE *urandom;
        urandom = fopen("/dev/urandom", "r");
        fread(icmp->data, 1, paysize, urandom);
        fclose(urandom);
    }
    icmp->chksum = icmp4_checksum(icmp, ipv4Header);
    return icmp;

}

unsigned short icmp4_checksum(struct IcmpHeader *icmpHeader, struct Ipv4Header *ipv4Header) {
    unsigned short *buf = (unsigned short *) icmpHeader;
    unsigned short icmpl = ntohs(ipv4Header->len) - (unsigned short) IPV4HDRSIZE;
    register unsigned int sum = 0;
    icmpHeader->chksum = 0;

    for (int i = 0; i < icmpl; i += 2)
        sum += *buf++;
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    sum = ~sum;
    return (unsigned short) sum;
}
