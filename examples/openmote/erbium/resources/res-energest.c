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
 *      Humidity Resource Openmote
 * \author
 *      Johan Bregell <johan@bregell.se>
 */

#include <stdlib.h>
#include <string.h>
#include "lpm.h"
#include "rest-engine.h"

static void res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

/*
 * A handler function named [resource name]_handler must be implemented for each RESOURCE.
 * A buffer for the response payload is provided through the buffer pointer. Simple resources can ignore
 * preferred_size and offset, but must respect the REST_MAX_CHUNK_SIZE limit for the buffer.
 * If a smaller block size is requested for CoAP, the REST framework automatically splits the data.
 */
RESOURCE(res_energest,
         "title=\"Energy Info\";rt=\"energy\"",
         res_get_handler,
         NULL,
         NULL,
         NULL);
         
static void
res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  unsigned int accept = -1; 
  
  float cpu, lpm;
  int cpu_f, lpm_f, cpu_d, lpm_d;
  /*
  static uint32_t last_cpu, last_lpm, last_transmit, last_listen;
  static uint32_t last_idle_transmit, last_idle_listen;
  */
  
  //uint32_t cpu, lpm, transmit, listen;
  //uint32_t all_cpu, all_lpm, all_transmit, all_listen;
  //uint32_t time, all_time, radio, all_radio;
  uint32_t all_cpu, all_lpm, all_time;
  
  energest_flush();

  all_cpu = energest_type_time(ENERGEST_TYPE_CPU);
  all_lpm = energest_type_time(ENERGEST_TYPE_LPM);
  //all_transmit = energest_type_time(ENERGEST_TYPE_TRANSMIT);
  //all_listen = energest_type_time(ENERGEST_TYPE_LISTEN);

  //cpu = all_cpu - last_cpu;
  //lpm = all_lpm - last_lpm;
  //transmit = all_transmit - last_transmit;
  //listen = all_listen - last_listen;

  /*
  last_cpu = energest_type_time(ENERGEST_TYPE_CPU);
  last_lpm = energest_type_time(ENERGEST_TYPE_LPM);
  last_transmit = energest_type_time(ENERGEST_TYPE_TRANSMIT);
  last_listen = energest_type_time(ENERGEST_TYPE_LISTEN);
  */
  
  //radio = transmit + listen;
  //time = cpu + lpm;
  all_time = all_cpu + all_lpm;
  //all_radio = energest_type_time(ENERGEST_TYPE_LISTEN) +
  //  energest_type_time(ENERGEST_TYPE_TRANSMIT);
  
  cpu = ((float)all_cpu/(float)all_time)*100.0;
  lpm = ((float)all_lpm/(float)all_time)*100.0;
  cpu_f = (int)(cpu);
  lpm_f = (int)(lpm);
  cpu_d = (int)((cpu-(float)cpu_f)*100.0);
  lpm_d = (int)((lpm-(float)lpm_f)*100.0);
    
  REST.get_header_accept(request, &accept);
  if(accept == -1 || accept == REST.type.TEXT_PLAIN){
    REST.set_header_content_type(request, REST.type.TEXT_PLAIN);
    snprintf(
      (char *)buffer, REST_MAX_CHUNK_SIZE, 
      "CPU:%d.%d%%, LPM:%d.%d%%, PM0:%u, PM1:%u, PM2:%u", 
      cpu_f, cpu_d,
      lpm_f, lpm_d, 
      (int)LPM_STATS_GET(0),
      (int)LPM_STATS_GET(1), 
      (int)LPM_STATS_GET(2)
    );
    REST.set_response_payload(response, (uint8_t *)buffer, strlen((char *)buffer));
  } else if(accept == REST.type.APPLICATION_JSON){
    REST.set_header_content_type(request, REST.type.APPLICATION_JSON);
    snprintf(
      (char *)buffer, REST_MAX_CHUNK_SIZE, 
      "{\"CPU(%%)\":%d.%d, \"LPM(%%)\":%d.%d}", 
      cpu_f, cpu_d,
      lpm_f, lpm_d
    );
    REST.set_response_payload(response, (uint8_t *)buffer, strlen((char *)buffer));
  } else {
    REST.set_response_status(response, REST.status.NOT_ACCEPTABLE);
    const char *msg = "Supporting content-types text/plain and application/json";
    REST.set_response_payload(response, msg, strlen(msg));
  }
}
/**
 * @}
 * @}
 */
