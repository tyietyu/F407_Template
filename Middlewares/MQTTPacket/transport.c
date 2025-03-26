/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *    Sergio R. Caprile - port to the bare metal environment and serial media specifics
 *******************************************************************************/

/** By the way, this is a nice bare bones example, easier to expand to whatever non-OS
media you might have */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "transport.h"

/**
This simple low-level implementation assumes a single connection for a single thread. Thus, single static
variables are used for that connection.
On other scenarios, you might want to put all these variables into a structure and index via the 'sock'
parameter, as some functions show in the comments
The blocking rx function is not supported.
If you plan on writing one, take into account that the current implementation of
MQTTPacket_read() has a function pointer for a function call to get the data to a buffer, but no provisions
to know the caller or other indicator (the socket id): int (*getfn)(unsigned char*, int)
*/

extern UART_HandleTypeDef huart2;
static transport_handle_t transport[MAX_CONNECTIONS];

void transport_sendPacketBuffernb_start(int sock, unsigned char *buf, int buflen)
{
    transport[sock].from    = buf;    // from[sock] or mystruct[sock].from
    transport[sock].howmany = buflen; // myhowmany[sock] or mystruct[sock].howmany
}

int transport_sendPacketBuffernb(int sock)
{
    transport_iofunctions_t *myio = transport[sock].io; // io[sock] or mystruct[sock].io
    int len;
    /* you should have called open() with a valid pointer to a valid struct and
    called sendPacketBuffernb_start with a valid buffer, before calling this */
    // assert((myio != NULL) && (myio->send != NULL) && (from != NULL));
    if ((len = myio->send(transport[sock].from, transport[sock].howmany)) > 0) {
        transport[sock].from += len;
        if ((transport[sock].howmany -= len) <= 0) {
            return TRANSPORT_DONE;
        }
    } else if (len < 0) {
        return TRANSPORT_ERROR;
    }
    return TRANSPORT_AGAIN;
}

int transport_sendPacketBuffer(int sock, unsigned char *buf, int buflen)
{
    int rc;

    transport_sendPacketBuffernb_start(sock, buf, buflen);
    while ((rc = transport_sendPacketBuffernb(sock)) == TRANSPORT_AGAIN) {
        /* this is unlikely to loop forever unless there is a hardware problem */
    }
    if (rc == TRANSPORT_DONE) {
        return buflen;
    }
    return TRANSPORT_ERROR;
}

int transport_getdata(unsigned char *buf, int count)
{
//    int rc;
//   // assert(0);		/* This function is NOT supported, it is just here to tease you */
//    transport_sendPacketBuffernb_start(sock, buf, count);
//    while((rc=transport_sendPacketBuffernb(sock)) == TRANSPORT_AGAIN){
//    	/* this is unlikely to loop forever unless there is a hardware problem */
//    }
//    if(rc == TRANSPORT_DONE){
//    	return count;
//    }
    return TRANSPORT_ERROR; /* nah, it is here for similarity with other transport examples */
}

int transport_getdatanb(void *sck, unsigned char *buf, int count)
{
    int sock                      = *((int *)sck);      /* sck: pointer to whatever the system may use to identify the transport */
    transport_iofunctions_t *myio = transport[sock].io; // io[sock] or mystruct[sock].io
    int len;
    if (myio != NULL && myio->recv != NULL) {
        while ((len = myio->recv(buf, count)) == 0);
        if (len > 0) {
            return len;
        }
        if (len == -2) {
            return 0;
        }
        if (len == -1) {
            return TRANSPORT_ERROR;
        }
    }
    return TRANSPORT_ERROR;
}

/**
return >=0 for a connection descriptor, <0 for an error code
*/
int transport_open(transport_iofunctions_t *thisio)
{
    int idx = 0; // 使用某种方式分配索引

    for (idx = 0; idx < MAX_CONNECTIONS; idx++) {
        if (transport[idx].io == NULL) {
            transport[idx].io = thisio;
            return idx;
        }
    }
    return TRANSPORT_ERROR;
}

int transport_close(int sock)
{
    transport[sock].io = NULL;
    return TRANSPORT_DONE;
}

int uart_send(unsigned char *buf, unsigned int len)
{
    if (HAL_UART_Transmit_DMA(&huart2, buf, len) == HAL_OK) {
        return len;
    }
    return -1;
}

int uart_recv(unsigned char *buf, unsigned int len)
{
    if (HAL_UART_Receive_DMA(&huart2, buf, len) == HAL_OK) {
        return len;
    }
    return -1;
}
