/*
 * Copyright (c) 2016 Jacopo De Luca
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

#include <ipv4.h>
#include <udp.h>

struct UdpHeader *build_udp_packet(unsigned short srcp, unsigned short dstp, unsigned short paysize,
                                   unsigned char *payload) {
    unsigned long size = UDPHDRSIZE + paysize;
    struct UdpHeader *ret = (struct UdpHeader *) malloc(size);
    if (ret == NULL)
        return NULL;
    injects_udp_header((unsigned char *) ret, srcp, dstp, paysize);
    if (payload != NULL)
        memcpy(ret->data, payload, paysize);
    return ret;
}

unsigned short udp_checksum4(struct UdpHeader *udpHeader, struct Ipv4Header *ipv4Header) {
    unsigned short *buf = (unsigned short *) udpHeader;
    register unsigned int sum = 0;
    udpHeader->checksum = 0;

    // Add the pseudo-header
    sum += *(((unsigned short *) &ipv4Header->saddr));
    sum += *(((unsigned short *) &ipv4Header->saddr) + 1);
    sum += *(((unsigned short *) &ipv4Header->daddr));
    sum += *(((unsigned short *) &ipv4Header->daddr) + 1);
    sum += htons(ipv4Header->protocol) + udpHeader->len;

    for (int i = 0; i < ntohs(udpHeader->len); i += 2)
        sum += *buf++;
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    sum = ~sum;
    return (unsigned short) (sum == 0 ? 0xFFFF : sum); // RFC 768
}

struct UdpHeader *injects_udp_header(unsigned char *buf, unsigned short srcp, unsigned short dstp,
                                     unsigned short len) {
    struct UdpHeader *ret = (struct UdpHeader *) buf;
    memset(ret, 0x00, UDPHDRSIZE);
    ret->srcport = htons(srcp);
    ret->dstport = htons(dstp);
    ret->len = htons(((unsigned short) UDPHDRSIZE) + len);
    return ret;
}
