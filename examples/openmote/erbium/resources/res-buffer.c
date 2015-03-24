/*
 * Copyright (c) 2015, Johan Bregell, i3tex AB
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 */

/**
 * \addtogroup openmote-examples 
 * @{
 * 
 * \defgroup openmote-erbium-server Erbium server for the OpenMote OpenBattery platform
 * @{
 * 
 * \file
 *      Resource to test the bandwidth and troughput of CoAP
 * \author
 *      Johan Bregell <johan@bregell.se>
 */

#include <stdlib.h>
#include <string.h>
#include "rest-engine.h"
#include "er-coap.h"

#define BUFFER_SIZE 256

static volatile uint8_t buffer[BUFFER_SIZE] = { 0 };
static volatile uint32_t head = 0, tail = 0;

static void res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_put_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

/*
 * A handler function named [resource name]_handler must be implemented for each RESOURCE.
 * A buffer for the response payload is provided through the buffer pointer. Simple resources can ignore
 * preferred_size and offset, but must respect the REST_MAX_CHUNK_SIZE limit for the buffer.
 * If a smaller block size is requested for CoAP, the REST framework automatically splits the data.
 */
RESOURCE(res_buffer,
         "title=\"Circular Buffer\"",
         res_get_handler,
         NULL,
         res_put_handler,
         NULL);

static void
res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  coap_packet_t *const coap_req = (coap_packet_t *)request;
  coap_packet_t *const coap_res = (coap_packet_t *)response;
  
  if(coap_req->content_format == REST.type.TEXT_PLAIN){
    REST.set_header_content_type(request, REST.type.TEXT_PLAIN);
    
    volatile int i = 0;
    volatile int ptr = tail;
    while(ptr != head && i < REST_MAX_CHUNK_SIZE){
      coap_res->payload[i] = buffer[ptr];
      ptr = (ptr+1)%BUFFER_SIZE;
      i++;
    }
  }
}

static void
res_put_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  coap_packet_t *const coap_req = (coap_packet_t *)request;
  
  if(coap_req->content_format == REST.type.TEXT_PLAIN){
    volatile int i = 0;
    tail = head;
    while(i < coap_req->payload_len){
      buffer[head] = coap_req->payload[i];
      head = (head+1)%BUFFER_SIZE;
      i++;
    }
    buffer[head] = '\0';
    REST.set_response_status(response, REST.status.CHANGED);
  }
}
/**
 * @}
 * @}
 */
 
