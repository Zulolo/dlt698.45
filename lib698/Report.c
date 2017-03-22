/*
 * Report.c
 *
 *  Created on: 2017-2-23
 *      Author: wzm
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "AccessFun.h"
#include "StdDataType.h"
#include "Objectdef.h"
#include "dlt698def.h"
#include "dlt698.h"
#include "PublicFunction.h"
#include "secure.h"
#include "event.h"
extern int FrameHead(CSINFO *csinfo,INT8U *buf);
extern void FrameTail(INT8U *buf,int index,int hcsi);
extern INT8U Get_Event(OAD oad,INT8U eventno,INT8U** Getbuf,int *Getlen,ProgramInfo* prginfo_event);
INT8S (*pSendfun_report)(int fd,INT8U* sndbuf,INT16U sndlen);

typedef struct{
	OI_698 oi;
	INT8U num;
	OAD oad[20];
}event_guanlian;
const static event_guanlian guanlian_oad[]={
		{0x3106,6,{{0x2022,0x02,0x00},{0x201e,0x02,0x00},{0x2020,0x02,0x00},{0x2024,0x02,0x00},{0x3300,0x02,0x00},{0x3309,0x02,0x00}}},
		{0x3114,7,{{0x2022,0x02,0x00},{0x201e,0x02,0x00},{0x2020,0x02,0x00},{0x2024,0x02,0x00},{0x3300,0x02,0x00},{0x4000,0x22,0x00},{0x4000,0x82,0x00}}},
        {0x3104,10,{{0x2022,0x02,0x00},{0x201e,0x02,0x00},{0x2020,0x02,0x00},{0x2024,0x02,0x00},{0x3300,0x02,0x00},{0x201E,0x42,0x00},{0xF203,0x42,0x01},{0xF203,0x42,0x02},{0xF203,0x42,0x03},{0xF203,0x42,0x04}}},
        {0x3101,7,{{0x2022,0x02,0x00},{0x201e,0x02,0x00},{0x2020,0x02,0x00},{0x2024,0x02,0x00},{0x3300,0x02,0x00},{0x4300,0x22,0x02},{0x4300,0x42,0x02}}},
		{0x3105,5,{{0x2022,0x02,0x00},{0x201e,0x02,0x00},{0x2020,0x02,0x00},{0x2024,0x02,0x00},{0x3300,0x02,0x00}}},
		{0x3109,6,{{0x2022,0x02,0x00},{0x201e,0x02,0x00},{0x2020,0x02,0x00},{0x2024,0x02,0x00},{0x3300,0x02,0x00},{0x4401,0x22,0x01}}},
		{0x310B,7,{{0x2022,0x02,0x00},{0x201e,0x02,0x00},{0x2020,0x02,0x00},{0x2024,0x02,0x00},{0x3300,0x02,0x00},{0x0010,0x22,0x01},{0x0010,0x82,0x01}}},
		{0x310C,7,{{0x2022,0x02,0x00},{0x201e,0x02,0x00},{0x2020,0x02,0x00},{0x2024,0x02,0x00},{0x3300,0x02,0x00},{0x0010,0x22,0x01},{0x0010,0x82,0x01}}},
		{0x310D,7,{{0x2022,0x02,0x00},{0x201e,0x02,0x00},{0x2020,0x02,0x00},{0x2024,0x02,0x00},{0x3300,0x02,0x00},{0x0010,0x22,0x01},{0x0010,0x82,0x01}}},
		{0x310E,6,{{0x2022,0x02,0x00},{0x201e,0x02,0x00},{0x2020,0x02,0x00},{0x2024,0x02,0x00},{0x3300,0x02,0x00},{0x0010,0x22,0x01}}},
		{0x310F,8,{{0x2022,0x02,0x00},{0x201e,0x02,0x00},{0x2020,0x02,0x00},{0x2024,0x02,0x00},{0x3300,0x02,0x00},{0x6041,0x22,0x00},{0x0010,0x22,0x01},{0x0030,0x22,0x01}}},
		{0x3110,7,{{0x2022,0x02,0x00},{0x201e,0x02,0x00},{0x2020,0x02,0x00},{0x2024,0x02,0x00},{0x3300,0x02,0x00},{0x2200,0x42,0x02},{0x3110,0x06,0x01}}},
		{0x3115,5,{{0x2022,0x02,0x00},{0x201e,0x02,0x00},{0x2020,0x02,0x00},{0x2024,0x02,0x00},{0x3300,0x02,0x00}}},
		{0x3118,5,{{0x2022,0x02,0x00},{0x201e,0x02,0x00},{0x2020,0x02,0x00},{0x2024,0x02,0x00},{0x3300,0x02,0x00}}},
		{0x3119,7,{{0x2022,0x02,0x00},{0x201e,0x02,0x00},{0x2020,0x02,0x00},{0x2024,0x02,0x00},{0x3300,0x02,0x00},{0x2003,0x42,0x00},{0x0010,0x42,0x01}}},
		{0x3200,6,{{0x2022,0x02,0x00},{0x201e,0x02,0x00},{0x2020,0x02,0x00},{0x2024,0x02,0x00},{0x3300,0x02,0x00},{0x2301,0x23,0x00}}},
		{0x3201,6,{{0x2022,0x02,0x00},{0x201e,0x02,0x00},{0x2020,0x02,0x00},{0x2024,0x02,0x00},{0x3300,0x02,0x00},{0x2301,0x49,0x00}}},
		{0x3202,6,{{0x2022,0x02,0x00},{0x201e,0x02,0x00},{0x2020,0x02,0x00},{0x2024,0x02,0x00},{0x3300,0x02,0x00},{0x810C,0x22,0x01}}},
		{0x3030,5,{{0x2022,0x02,0x00},{0x201e,0x02,0x00},{0x2020,0x02,0x00},{0x2024,0x02,0x00},{0x3300,0x02,0x00}}},
		{0x3100,5,{{0x2022,0x02,0x00},{0x201e,0x02,0x00},{0x2020,0x02,0x00},{0x2024,0x02,0x00},{0x3300,0x02,0x00}}},
		{0x3107,5,{{0x2022,0x02,0x00},{0x201e,0x02,0x00},{0x2020,0x02,0x00},{0x2024,0x02,0x00},{0x3300,0x02,0x00}}},
		{0x3108,5,{{0x2022,0x02,0x00},{0x201e,0x02,0x00},{0x2020,0x02,0x00},{0x2024,0x02,0x00},{0x3300,0x02,0x00}}},
		{0x310A,5,{{0x2022,0x02,0x00},{0x201e,0x02,0x00},{0x2020,0x02,0x00},{0x2024,0x02,0x00},{0x3300,0x02,0x00}}},
		{0x3111,5,{{0x2022,0x02,0x00},{0x201e,0x02,0x00},{0x2020,0x02,0x00},{0x2024,0x02,0x00},{0x3300,0x02,0x00}}},
		{0x3112,5,{{0x2022,0x02,0x00},{0x201e,0x02,0x00},{0x2020,0x02,0x00},{0x2024,0x02,0x00},{0x3300,0x02,0x00}}},
		{0x3116,5,{{0x2022,0x02,0x00},{0x201e,0x02,0x00},{0x2020,0x02,0x00},{0x2024,0x02,0x00},{0x3300,0x02,0x00}}},
		{0x3117,5,{{0x2022,0x02,0x00},{0x201e,0x02,0x00},{0x2020,0x02,0x00},{0x2024,0x02,0x00},{0x3300,0x02,0x00}}},
		{0x311A,5,{{0x2022,0x02,0x00},{0x201e,0x02,0x00},{0x2020,0x02,0x00},{0x2024,0x02,0x00},{0x3300,0x02,0x00}}},
		{0x311B,7,{{0x2022,0x02,0x00},{0x201e,0x02,0x00},{0x2020,0x02,0x00},{0x2024,0x02,0x00},{0x3300,0x02,0x00},{0x4000,0x02,0x00},{0x330D,0x02,0x00}}},
		{0x311C,5,{{0x2022,0x02,0x00},{0x201e,0x02,0x00},{0x2020,0x02,0x00},{0x2024,0x02,0x00},{0x3300,0x02,0x00}}},
		{0x3203,5,{{0x2022,0x02,0x00},{0x201e,0x02,0x00},{0x2020,0x02,0x00},{0x2024,0x02,0x00},{0x3300,0x02,0x00}}}
};

INT8U Report_Event(CommBlock *com,Reportevent report_event,INT8U report_type){

	int apduplace =0;
	int index=0, hcsi=0,temindex=0,i=0;
	pSendfun_report = com->p_send;
	CSINFO csinfo;
	csinfo.dir = 1;
	csinfo.prm = 0;
	csinfo.funcode = 0x03;
	csinfo.gframeflg = 0;
	csinfo.sa_type = 0;
	INT8U sa_len=com->serveraddr[0];
	for(i=0;i<sa_len;i++) {
		csinfo.sa[i] = com->serveraddr[sa_len-i];
	}
	csinfo.ca = com->taskaddr;
    csinfo.sa_length = sa_len;
	INT8U sendbuf_report[300],tem_buf[300];
	index = FrameHead(&csinfo,sendbuf_report);
	hcsi = index;  //两字节校验
	index = index + 2;
	apduplace = index;		//记录APDU 起始位置
	OAD oad;
	oad.OI=report_event.oi;
	oad.attflg=2;
	oad.attrindex=0;
	if(report_type == 1){
		tem_buf[temindex++] = REPORT_NOTIFICATION;
		tem_buf[temindex++] = REPORTNOTIFICATIONLIST;
		tem_buf[temindex++] = 0B10000000;	//	piid
		tem_buf[temindex++] = 1; //个数
		tem_buf[temindex++] = dtoad;//OAD数据

		tem_buf[temindex++] = ((oad.OI>>8)&0x00ff);
		tem_buf[temindex++] = oad.OI&0x00ff;
		tem_buf[temindex++] = oad.attflg;
		tem_buf[temindex++] = oad.attrindex;
	}else{
		tem_buf[temindex++] = REPORT_NOTIFICATION;
	    tem_buf[temindex++] = REPROTNOTIFICATIONRECORDLIST;
	    tem_buf[temindex++] = 0x02;
	    tem_buf[temindex++] = 0x01;
	    tem_buf[temindex++] = ((oad.OI>>8)&0x00ff);
	    tem_buf[temindex++] = oad.OI&0x00ff;
		tem_buf[temindex++] = oad.attflg;
		tem_buf[temindex++] = oad.attrindex;

		INT8U oadnumindex=temindex;
	    tem_buf[temindex++] = 0x00; //OAD数量

		INT8U in=0,_in=0;
		for(in=0;in<sizeof(guanlian_oad)/sizeof(event_guanlian);in++){
            if(guanlian_oad[in].oi == oad.OI){
            	fprintf(stderr,"\\\\num=%d \n",guanlian_oad[in].num);
            	tem_buf[oadnumindex]+=guanlian_oad[in].num;
            	for(_in=0;_in<guanlian_oad[in].num;_in++){
            		tem_buf[temindex++] = 0x00;
            		tem_buf[temindex++] = (guanlian_oad[in].oad[_in].OI>>8)&0x00ff;
					tem_buf[temindex++] = guanlian_oad[in].oad[_in].OI&0x00ff;
					tem_buf[temindex++] = guanlian_oad[in].oad[_in].attflg;
					tem_buf[temindex++] = guanlian_oad[in].oad[_in].attrindex;
            	}
            }
		}
	}
	INT8U *data=NULL;
	int datalen=0;
	if(Get_Event(oad,1,&data,&datalen,(ProgramInfo*)com->shmem) == 1){
		if(data!=NULL && datalen>0 && datalen<256){
			fprintf(stderr,"datalen=%d \n",datalen);
			tem_buf[temindex++] = 1; //data
			if(report_type == 1){
				memcpy(&tem_buf[temindex],data,datalen);
				temindex +=datalen;
			}else{
				tem_buf[temindex++] = 1; //1个
				memcpy(&tem_buf[temindex],&data[STANDARD_NO_INDEX],datalen-2);//事件序号以后得数据
				temindex +=datalen-2;
			}
		}else
			tem_buf[temindex++] = 0; //data
	}
	if (data!=NULL)
		free(data);

	if(com->f101.active == 1){
		sendbuf_report[index++]=16; //SECURIGY-REQUEST
		sendbuf_report[index++]=0;  //明文应用数据单元
		INT16U seclen=composeAutoReport(tem_buf,temindex);
		if(seclen>0){
			memcpy(&sendbuf_report[index],tem_buf,seclen);
			index +=seclen;
		}
	}else{
       memcpy(&sendbuf_report[index],tem_buf,temindex);
       index +=temindex;
	}
	sendbuf_report[index++] = 0;	//跟随上报信息域 	FollowReport
	sendbuf_report[index++] = 0;	//时间标签		TimeTag
	FrameTail(sendbuf_report,index,hcsi);
	if(pSendfun_report!=NULL)
		pSendfun_report(com->phy_connect_fd,sendbuf_report,index+3);
	return (index+3);
}
