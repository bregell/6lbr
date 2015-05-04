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
 *      Radio transmit power resource for Openmote
 * \author
 *      Johan Bregell <johan@bregell.se>
 */

#include <stdlib.h>
#include <string.h>
#include "net/netstack.h"
#include "dev/leds.h"
#include "rest-engine.h"
#include "er-coap.h"


static void res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_put_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

/*
 * A handler function named [resource name]_handler must be implemented for each RESOURCE.
 * A buffer for the response payload is provided through the buffer pointer. Simple resources can ignore
 * preferred_size and offset, but must respect the REST_MAX_CHUNK_SIZE limit for the buffer.
 * If a smaller block size is requested for CoAP, the REST framework automatically splits the data.
 */
RESOURCE(res_transmit_power,
         "title=\"Transmit Power\";rt=\"power level -24 - 7\"",
         res_get_handler,
         NULL,
         res_put_handler,
         NULL);

static void
res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  unsigned int accept = -1;
  radio_value_t txpower;
  NETSTACK_RADIO.get_value(RADIO_PARAM_TXPOWER, &txpower);


  REST.get_header_accept(request, &accept);
  if(accept == -1 || accept == REST.type.TEXT_PLAIN){

    REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
    snprintf(
    (char *)buffer,
    REST_MAX_CHUNK_SIZE,
    "%d dBm",
    txpower
  );
    REST.set_response_payload(
    response,
    (uint8_t *)buffer,
    strlen((char *)buffer)
  );
  } else if(accept == REST.type.APPLICATION_JSON){
    REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
    snprintf(
      (char *)buffer,
      REST_MAX_CHUNK_SIZE,
      "{'dBm':%d}",
      txpower
    );
    REST.set_response_payload(
      response,
      (uint8_t *)buffer,
      strlen((char *)buffer)
    );
  } else {
    REST.set_response_status(response, REST.status.NOT_ACCEPTABLE);
    const char *msg = "Supporting content-types text/plain and application/json";
    REST.set_response_payload(response, msg, strlen(msg));
  }
}

static void
res_put_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  coap_packet_t *const coap_req = (coap_packet_t *)request;
  char payload_buffer[20] = { 0 };
  int txpower = 0;
  
  REST.set_header_content_type(response, REST.type.TEXT_PLAIN);

  if(coap_req->payload_len){
    if(coap_req->content_format == REST.type.TEXT_PLAIN){
      if(coap_req->payload_len <= 3){
        memcpy(payload_buffer, coap_req->payload, coap_req->payload_len);
        txpower = atoi(payload_buffer);
        if(txpower >= -21 && txpower <= 7){
          NETSTACK_RADIO.set_value(RADIO_PARAM_TXPOWER, txpower);
          REST.set_response_status(response, REST.status.CHANGED);
          return;
        }
        snprintf(
          (char *)buffer,
          REST_MAX_CHUNK_SIZE,
          "%d is out of range (-21 - 7)dBm",
          txpower
        );
        REST.set_response_payload(
          response,
          (uint8_t *)buffer,
          strlen((char *)buffer)
        );
        return;
      }
    } else {
      REST.set_response_status(response, REST.status.NOT_ACCEPTABLE);
      const char *msg = "Supporting content-types text/plain";
      REST.set_response_payload(response, msg, strlen(msg));
      return;
    }
  }
  REST.set_response_status(response, REST.status.BAD_REQUEST);
  const char *error_msg = "NoPayload";
  REST.set_response_payload(response, error_msg, strlen(error_msg));
  return;
}
/**
 * @}
 * @}
 */
