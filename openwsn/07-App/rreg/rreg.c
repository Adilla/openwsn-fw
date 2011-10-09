#include "openwsn.h"
#include "rreg.h"
#include "opencoap.h"
#include "openqueue.h"
#include "packetfunctions.h"
#include "openserial.h"

//=========================== variables =======================================

#define RREGPERIOD        10

typedef struct {
   uint8_t              delay;
   coap_resource_desc_t desc;
} rreg_vars_t;

rreg_vars_t rreg_vars;

const uint8_t rreg_path0[]  = "reg";
const uint8_t rreg_rdpath[] = "rd?h=poi";

//=========================== prototypes ======================================

error_t rreg_receive(OpenQueueEntry_t* msg,
                     coap_header_iht*  coap_header,
                     coap_option_iht*  coap_options);
void    rreg_timer();

//=========================== public ==========================================

void rreg_init() {
   // prepare the resource descriptor for the /.well-known/core path
   rreg_vars.desc.path0len      = sizeof(rreg_path0)-1;
   rreg_vars.desc.path0val      = (uint8_t*)(&rreg_path0);
   rreg_vars.desc.path1len      = 0;
   rreg_vars.desc.path1val      = NULL;
   rreg_vars.desc.callbackRx    = &rreg_receive;
   rreg_vars.desc.callbackTimer = rreg_timer;
   
   rreg_vars.delay              = 0;
   
   opencoap_register(&rreg_vars.desc);
}

//=========================== private =========================================

error_t rreg_receive(OpenQueueEntry_t* msg,
                   coap_header_iht* coap_header,
                   coap_option_iht* coap_options) {
                      
   error_t outcome;
   
   if (coap_header->Code==COAP_CODE_REQ_POST) {
      // set delay to RREGPERIOD to gets triggered next time rreg_timer is called
      rreg_vars.delay                  = RREGPERIOD;
      
      // reset packet payload
      msg->payload                     = &(msg->packet[127]);
      msg->length                      = 0;
      
      // set the CoAP header
      coap_header->OC                  = 0;
      coap_header->Code                = COAP_CODE_RESP_VALID;
      
      outcome = E_SUCCESS;
   } else {
      outcome = E_FAIL;
   }
   
   return outcome;
}

void rreg_timer() {
   OpenQueueEntry_t* pkt;
   
   rreg_vars.delay += 2;
   
   if (rreg_vars.delay>=RREGPERIOD) {
      // reset the timer
      rreg_vars.delay = 0;
      // create a CoAP RD packet
      pkt = openqueue_getFreePacketBuffer();
      if (pkt==NULL) {
         openserial_printError(COMPONENT_RREG,ERR_NO_FREE_PACKET_BUFFER,
                               (errorparameter_t)0,
                               (errorparameter_t)0);
         openqueue_freePacketBuffer(pkt);
         return;
      }
      // add CoAP payload
      packetfunctions_reserveHeaderSize(pkt,6);
      pkt->payload[0] = '<';
      pkt->payload[1] = '/';
      pkt->payload[2] = 'p';
      pkt->payload[3] = 'o';
      pkt->payload[4] = 'i';
      pkt->payload[5] = '>';
      // add URI path option
      packetfunctions_reserveHeaderSize(pkt,sizeof(rreg_rdpath)-1);
      memcpy(pkt->payload,&rreg_rdpath,sizeof(rreg_rdpath)-1);
      packetfunctions_reserveHeaderSize(pkt,1);
      pkt->payload[0] = (COAP_OPTION_URIPATH-COAP_OPTION_CONTENTTYPE) << 4 |
                        sizeof(rreg_rdpath)-1;
      // add content-type option
      packetfunctions_reserveHeaderSize(pkt,2);
      pkt->payload[0]                  = COAP_OPTION_CONTENTTYPE << 4 |
                                         1;
      pkt->payload[1]                  = COAP_MEDTYPE_APPLINKFORMAT;
      // metadata
      pkt->l4_destination_port         = WKP_UDP_COAP;
      pkt->l3_destinationORsource.type = ADDR_128B;
      pkt->l3_destinationORsource.addr_128b[ 0] = 0x26;
      pkt->l3_destinationORsource.addr_128b[ 1] = 0x07;
      pkt->l3_destinationORsource.addr_128b[ 2] = 0xf7;
      pkt->l3_destinationORsource.addr_128b[ 3] = 0x40;
      pkt->l3_destinationORsource.addr_128b[ 4] = 0x00;
      pkt->l3_destinationORsource.addr_128b[ 5] = 0x00;
      pkt->l3_destinationORsource.addr_128b[ 6] = 0x00;
      pkt->l3_destinationORsource.addr_128b[ 7] = 0x3f;
      pkt->l3_destinationORsource.addr_128b[ 8] = 0x00;
      pkt->l3_destinationORsource.addr_128b[ 9] = 0x00;
      pkt->l3_destinationORsource.addr_128b[10] = 0x00;
      pkt->l3_destinationORsource.addr_128b[11] = 0x00;
      pkt->l3_destinationORsource.addr_128b[12] = 0x00;
      pkt->l3_destinationORsource.addr_128b[13] = 0x00;
      pkt->l3_destinationORsource.addr_128b[14] = 0x0e;
      pkt->l3_destinationORsource.addr_128b[15] = 0x29;
      // send
      if (opencoap_send(pkt)==E_FAIL) {
         openqueue_freePacketBuffer(pkt);
      }
   }
   return;
}