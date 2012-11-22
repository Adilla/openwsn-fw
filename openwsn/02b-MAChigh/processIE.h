#ifndef __PROCESSIE_H
#define __PROCESSIE_H

#define         MAXLINKNEIGHBORS    10

#include "schedule.h"
#include "openwsn.h"                                // needed for uin8_t, uint16_t

//=========================== typedef =========================================
typedef enum {
   ADV                             = 0,
   ASSOCIATE_REQ                   = 1,
   ASSOCIATE_RESP                  = 2,
   RES_LINK_REQ                    = 3,
   RES_LINK_RESP                   = 4,
   REMOVE_LINK_REQ                 = 5,
   SCHEDULE_REQ                    = 6,
   SCHEDULE_RESP                   = 7,
} resPacket_ID_t;

enum IE_groupID_enums {
   IE_ESDU                      = 0,
   IE_MLME                      = 1,
};

enum IE_type_enums {
   IE_TYPE_HEADER               = 0,
   IE_TYPE_PAYLOA               = 1,
};

typedef struct{
        uint8_t         slotframeID;
        uint16_t        slotframeSize;
        uint8_t         numOfLink;
        Link_t*         links;
}slotframeInfo_t;

typedef	struct{
	uint16_t	length;
	uint8_t	        SubID;
	uint8_t	        type;
}subIE_t;

typedef	struct{
	uint16_t	Length;
	uint8_t	        GroupID;
	uint8_t	        Type;
}IEHeader_t;

typedef struct{
  asn_t asn;
  uint8_t joinPriority;
}syncIEcontent_t;

typedef struct{
	uint8_t	        numOfSlotframes;
        slotframeInfo_t   slotframeInfo[MAXSOLTFRAMENUM];
}frameAndLinkIEcontent_t;

typedef	struct{
	uint8_t	        slotfameTemplt;
	void*	        otherField;
}timeslotIEcontent_t;

typedef struct{
  union {
	uint8_t	hoppingSequence_1Byte;
        uint8_t hoppingSequence_8Byte;
  };
}channelHoppingIEcontent_t;

typedef	struct{
	uint8_t		  numOfSlotframes;
        slotframeInfo_t   slotframeInfo[MAXSOLTFRAMENUM];
}uResLinkTypeIEcontent_t;

typedef	struct{
	uint8_t	uResCommandID;
}uResCommandIEcontent_t;

typedef	struct{
	uint8_t	slotframeID;
	uint8_t	numOfLinks;
}uResBandwidthIEcontent_t;

typedef	struct{
	uint8_t	compressType;
	uint8_t	otherFields;
}uResGeneralScheduleIEcontent_t;

//=========================== variables =======================================

//=========================== prototypes ======================================
//admin
void processIE_init();

void processIE_setMLME_IE ();
void processIE_setSubSyncIE();	
void processIE_setSubFrameAndLinkIE();
void processIE_setSubTimeslotIE();
void processIE_setSubChannelHoppingIE();
void processIE_setSubuResLinkTypeIE();
void processIE_setSubuResCommandIE();
void processIE_setSubuResBandwidthIE();
void processIE_setSubuResGeneralSheduleIE();
        
void processIE_getMLME_IE();
subIE_t* processIE_getSubSyncIE();	 
subIE_t* processIE_getSubFrameAndLinkIE(); 
subIE_t* processIE_getSubChannelHoppingIE(); 
subIE_t* processIE_getSubTimeslotIE(); 
subIE_t* processIE_getSubuResLinkTypeIE(); 
subIE_t* processIE_getSubuResCommandIE(); 
subIE_t* processIE_getSubuResBandwidthIE(); 
subIE_t* processIE_getSubuResGeneralSheduleIE();

syncIEcontent_t*                processIE_getSyncIEcontent();
frameAndLinkIEcontent_t*        processIE_getFrameAndLinkIEcontent();
timeslotIEcontent_t*            processIE_getTimeslotIEcontent();
channelHoppingIEcontent_t*      processIE_getChannelHoppingIEcontent();

uResLinkTypeIEcontent_t*        processIE_getuResLinkTypeIEcontent();
uResBandwidthIEcontent_t*       processIE_getuResBandwidthIEcontent();
uResCommandIEcontent_t*         processIE_getuResCommandIEcontent();
IEHeader_t*                     processIE_getIEHeader();

//reset
void resetSubIE();




#endif
