/*
 * pulse.c
 *
 *  Created on: Aug 16, 2017
 *      Author: z
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <syslog.h>

#include "pluse.h"
#include "PublicFunction.h"
#include "Shmem.h"
#include "Objectdef.h"
#include "AccessFun.h"
#include "PublicFunction.h"

extern ProgramInfo* JProgramInfo;

void pluseGetCount(int *pulse) {
	unsigned int buf[2];
	int fd = open("/dev/pulse", O_RDWR);
	if (fd > 0) {
		read(fd, buf, sizeof(unsigned int) * 2);
		pulse[0] = buf[0];
		pulse[1] = buf[1];
	}
	close(fd);
}

int pluseGetTimeZone() {
	TS ts;
	TSGet(&ts);
	fprintf(stderr, "VgetNowZone %d\n", ts.Hour);
	if (ts.Hour < 6) {
		return 0;
	} else if (ts.Hour < 12) {
		return 1;
	} else if (ts.Hour < 18) {
		return 2;
	}
	return 3;
}

/*
 * 返回脉冲输入的脉冲输入端口号的属性索引，来进行脉冲数的电量统计
 * 脉冲配置单元规约为数组，但是只认为一个有效
 * */
int get_pulseoad(INT8U pno,CLASS12 *class12,INT8U *pindex,INT32U *pk,INT8U *pconf)
{
	int i=0,j=0;
	for(i = 0; i< MAX_PULSE_NUM; i++) {
		for(j=0;j<MAX_PULSE_UNIT;j++) {
			if(class12[i].unit[j].no.OI == 0xF20A && class12[i].unit[j].no.attrindex == (pno+1))	{	//脉冲输入
				*pindex = i;
				*pk = class12[i].unit[j].k;
				*pconf = class12[i].unit[j].conf;
				return  1;
			}
		}
	}
	*pindex = 0;
	*pk = 0;
	*pconf = 0;
	return 0;
}

//根据脉冲计算电量
void pluseCalcDD(PluseUnit * pu)
{
	int index = 0, pulse = 0, time_zone =0;
	INT8U 	pl_index = 0, pl_conf=0;
	INT32U 	pl_k=0;
	INT32U	con=0;

	for (index = 0; index < 2; index++) {
		pulse = pu->pNow[index] - pu->pDD[index];
		if(pulse == 0) continue;//检查变化
		if(get_pulseoad(index,pu->class12,&pl_index,&pl_k,&pl_conf)==1) {
//			asyslog(LOG_INFO,"\nread day_pos_p=%d_%d_%d_%d ",pu->class12[0].day_pos_p[0],pu->class12[0].day_pos_p[1]
//				               ,pu->class12[0].day_pos_p[2],pu->class12[0].day_pos_p[3]);
//			asyslog(LOG_INFO,"[%d]pl_index=%d pl_k=%d pl_conf=%d",index,pl_index,pl_k,pl_conf);
			pu->class12[pl_index].pluse_count += pulse;
//			asyslog(LOG_INFO,"[%d]new pu->pNow=%d_%d pu->pDD=%d_%d pulse=%d pluse_count=%d",index,
//					pu->pNow[0],pu->pNow[1],pu->pDD[0],pu->pDD[1],pulse,pu->class12[pl_index].pluse_count);
			pu->pDD[index] = pu->pNow[index];
			if(pl_k == 0)  continue;
			con = (pulse * 10000.0) / pl_k;			//10000.0 （kwh, 换算-4）, TODO:替换实际单位
			time_zone = pluseGetTimeZone();/// TODO:get_rates_no
			asyslog(LOG_INFO,"[%d] time_zone=%d  con =%d  pulse=%d pl_k=%d" ,index,time_zone,con,pulse,pl_k);
			switch (pl_conf) {
			case 0://正向有功
				pu->class12[pl_index].val_pos_p[time_zone] += con;
				pu->class12[pl_index].day_pos_p[time_zone] += con;
				pu->class12[pl_index].mon_pos_p[time_zone] += con;
				fprintf(stderr, "[CTRL]实时正向有功 %d\n",pu->class12[pl_index].val_pos_p[time_zone]);
				break;
			case 2://反向有功
				pu->class12[pl_index].val_nag_p[time_zone] += con;
				pu->class12[pl_index].day_nag_p[time_zone] += con;
				pu->class12[pl_index].mon_nag_p[time_zone] += con;
				fprintf(stderr, "[CTRL]实时反向有功 %d\n",pu->class12[pl_index].val_nag_p[time_zone]);
				break;
			case 1://正向无功
				pu->class12[pl_index].val_pos_q[time_zone] += con;
				pu->class12[pl_index].day_pos_q[time_zone] += con;
				pu->class12[pl_index].mon_pos_q[time_zone] += con;
				fprintf(stderr, "[CTRL]实时正向无功 %d\n",pu->class12[pl_index].val_pos_q[time_zone]);
				break;
			case 3://反向无功
				pu->class12[pl_index].val_nag_q[time_zone] += con;
				pu->class12[pl_index].day_nag_q[time_zone] += con;
				pu->class12[pl_index].mon_nag_q[time_zone] += con;
				fprintf(stderr, "[CTRL]实时反向无功 %d\n",pu->class12[pl_index].val_nag_q[time_zone]);
				break;
			}
		}
	}
//	asyslog(LOG_INFO,"save day_pos_p=%d_%d_%d_%d ",pu->class12[0].day_pos_p[0],pu->class12[0].day_pos_p[1]
//		               ,pu->class12[0].day_pos_p[2],pu->class12[0].day_pos_p[3]);
}


//计算周期内实时功率
void pluseCalcPQ(PluseUnit * pu, int pulse, int index) {
	//检查参数
	if (pu->class12[index].unit[index].k == 0) {
		return;
	}

	double con = pu->class12[index].unit[index].k;
	con = (pulse * 60.0 * 10000.0) / con;

	switch (pu->class12[index].unit[index].conf) {
		case 0:
		case 2:
			pu->class12[index].p = (INT32S)con;
			fprintf(stderr, "[CTRL]实时有功功率 %d\n\n\n\n\n\n\n\n\n\n\n",
					pu->class12[index].p);
			break;
		case 1:
		case 3:
			pu->class12[index].q = (INT32S)con;
			fprintf(stderr, "[CTRL]实时无功功率 %d\n\n\n\n\n\n\n\n\n\n\n",
					pu->class12[index].q);
			break;
		default:
			break;
	}
}

int pluseInitUnit(PluseUnit * pu, ProgramInfo* JProgramInfo) {
	pluseGetCount(&pu->pNow[0]);

	for (int i = 0; i < 2; i++) {
		pu->step[i] = 0;
		pu->pDD[i] = pu->pNow[i];
		pu->pPQ[i] = pu->pNow[i];
	}
	asyslog(LOG_INFO,"初始化：pu->pNow=%d_%d pu->pDD=%d_%d pulsecount=%d_%d",pu->pNow[0],pu->pNow[1],pu->pDD[0],pu->pDD[1],
			JProgramInfo->class12[0].pluse_count,JProgramInfo->class12[1].pluse_count);
	pu->class12 = &JProgramInfo->class12[0];

	TS ts;
	TSGet(&ts);

	pu->old_day = ts.Day;
	pu->old_month = ts.Month;
	return 1;
}

void pluseCleanUpDayData(PluseUnit * pu) {
	for (int i = 0; i < 2; i++) {
		int size = sizeof(INT32U) * MAXVAL_RATENUM;
		memset(pu->class12[i].day_nag_p, 0x00, size);
		memset(pu->class12[i].day_nag_q, 0x00, size);
		memset(pu->class12[i].day_pos_p, 0x00, size);
		memset(pu->class12[i].day_pos_q, 0x00, size);
	}
}

void pluseCleanUpYearData(PluseUnit * pu) {
	for (int i = 0; i < 2; i++) {
		int size = sizeof(INT32U) * MAXVAL_RATENUM;
		memset(pu->class12[i].mon_nag_p, 0x00, size);
		memset(pu->class12[i].mon_nag_q, 0x00, size);
		memset(pu->class12[i].mon_pos_p, 0x00, size);
		memset(pu->class12[i].mon_pos_q, 0x00, size);
	}
}

void pluseRefreshUnit(PluseUnit * pu) {
	pluseGetCount(pu->pNow);
	fprintf(stderr, "[CTRL]脉冲计数%d-%d\n", pu->pNow[0], pu->pNow[1]);
	pluseCalcDD(pu);

	for (int i = 0; i < 2; i++) {
		switch (pu->step[i]) {
		case 0:
			if (pu->pNow[i] >= pu->pPQ[i]) {
				pu->last_time = time(NULL);
				pu->step[i] = 1;
			}
			break;
		case 1:
			if (abs((int)(time(NULL) - pu->last_time)) >= 59) {
				pluseCalcPQ(pu, pu->pNow[i] - pu->pPQ[i], i);
				pu->pPQ[i] = pu->pNow[i];
				pu->step[i] = 0;
			}
			break;
		default:
			pu->step[i] = 0;
			break;
		}
	}
	//跨日、月清零
	TS ts;
	TSGet(&ts);

	if (pu->old_day != ts.Day) {
		pu->old_day = ts.Day;
		pluseCleanUpDayData(pu);
	}

	if (pu->old_month != ts.Month) {
		pu->old_month = ts.Month;
		pluseCleanUpYearData(pu);
	}
}
