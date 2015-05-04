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

#define BUFFER_SIZE 1024


static int32_t large_update_size = 0;
static uint8_t large_update_store[BUFFER_SIZE] = { 0 };
static unsigned int large_update_ct = APPLICATION_OCTET_STREAM;

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
  /* Check the offset for boundaries of the resource data. */
  if(*offset >= large_update_size) {
    REST.set_response_status(response, REST.status.BAD_OPTION);
    /* A block error message should not exceed the minimum block size (16). */

    const char *error_msg = "BlockOutOfScope";
    REST.set_response_payload(response, error_msg, strlen(error_msg));
    return;
  }

  REST.set_response_payload(response, large_update_store + *offset,
                            MIN(large_update_size - *offset, preferred_size));
  REST.set_header_content_type(response, large_update_ct);

  /* IMPORTANT for chunk-wise resources: Signal chunk awareness to REST engine. */
  *offset += preferred_size;

  /* Signal end of resource representation. */
  if(*offset >= large_update_size) {
    *offset = -1;
  }
}

static void
res_put_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  coap_packet_t *const coap_req = (coap_packet_t *)request;
  uint8_t *incoming = NULL;
  size_t len = 0;

  unsigned int ct = -1;

  if(!REST.get_header_content_type(request, &ct)) {
    REST.set_response_status(response, REST.status.BAD_REQUEST);
    const char *error_msg = "NoContentType";
    REST.set_response_payload(response, error_msg, strlen(error_msg));
    return;
  }

  if((len = REST.get_request_payload(request, (const uint8_t **)&incoming))) {
    //if(coap_req->block1_num * coap_req->block1_size + len <= sizeof(large_update_store)) {
    int i = 0;
    while(i < len){
      large_update_store[((coap_req->block1_num * coap_req->block1_size)+i)%BUFFER_SIZE] = incoming[i];
      i++;
    }
    large_update_size = (coap_req->block1_num * coap_req->block1_size + len)%BUFFER_SIZE;
    large_update_ct = ct;

    REST.set_response_status(response, REST.status.CHANGED);
    coap_set_header_block1(response, coap_req->block1_num, 0,
                           coap_req->block1_size);
  } else {
    REST.set_response_status(response, REST.status.BAD_REQUEST);
    const char *error_msg = "NoPayload";
    REST.set_response_payload(response, error_msg, strlen(error_msg));
    return;
  }
}
/**
 * @}
 * @}
 */
