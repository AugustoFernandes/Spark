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

#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

#include <ipv4.h>
#include <tcp.h>

struct TcpHeader *build_tcp_packet(unsigned short src, unsigned short dst, unsigned int seqn, unsigned int ackn,
                                   unsigned char flags, unsigned short window, unsigned short urgp,
                                   unsigned long paysize, unsigned char *payload) {
    unsigned long size = TCPHDRSIZE + paysize;
    struct TcpHeader *ret = NULL;
    if ((ret = (struct TcpHeader *) malloc(size)) == NULL)
        return NULL;
    injects_tcp_header((unsigned char *) ret, src, dst, seqn, ackn, flags, window, urgp);
    if (payload != NULL)
        memcpy(ret->data, payload, paysize);
    return ret;
}


struct TcpHeader *injects_tcp_header(unsigned char *buf, unsigned short src, unsigned short dst, unsigned int seqn,
                                     unsigned int ackn, unsigned char flags,
                                     unsigned short window, unsigned short urgp) {
    struct TcpHeader *ret = (struct TcpHeader *) buf;
    memset(ret, 0x00, TCPHDRSIZE);
    ret->offset = TCPHDRLEN;
    ret->src = htons(src);
    ret->dst = htons(dst);
    ret->seqn = htonl(seqn);
    ret->ackn = htonl(ackn);
    ret->flags = flags;
    ret->window = htons(window);
    ret->urp = htons(urgp);
    return ret;
}

unsigned short tcp_checksum4(struct TcpHeader *TcpHeader, struct Ipv4Header *ipv4Header) {
    unsigned short *buf = (unsigned short *) TcpHeader;
    unsigned short tcpl = ntohs(ipv4Header->len) - (unsigned short) IPV4HDRSIZE;
    register unsigned int sum = 0;
    TcpHeader->checksum = 0;

    // Add the pseudo-header
    sum += *(((unsigned short *) &ipv4Header->saddr));
    sum += *(((unsigned short *) &ipv4Header->saddr) + 1);
    sum += *(((unsigned short *) &ipv4Header->daddr));
    sum += *(((unsigned short *) &ipv4Header->daddr) + 1);
    sum += htons(ipv4Header->protocol);
    sum += htons(tcpl);

    for (int i = 0; i < tcpl; i += 2)
        sum += *buf++;
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    sum = ~sum;
    return (unsigned short) sum;
}