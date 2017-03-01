﻿#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include "cjcomm.h"

//共享内存地址
static ProgramInfo* JProgramInfo = NULL;
static CLASS25 Class25;

ProgramInfo* getShareMem(void) {
    return JProgramInfo;
}

void clearcount(int index) {
    JProgramInfo->Projects[index].WaitTimes = 0;
}

void QuitProcess(int sig) {
    asyslog(LOG_INFO, "通信模块退出,收到信号类型(%d)", sig);
    IfrDestory();
    SerialDestory();
    ServerDestory();
    MmqDestory();
    ClientDestory();

    //关闭AT模块
    asyslog(LOG_INFO, "关闭AT模块电源");
    AT_POWOFF();

    //等待处理完成
    asyslog(LOG_INFO, "等待(2秒)清理工作完成……");
    sleep(2);

    exit(0);
}

void WriteLinkRequest(INT8U link_type, INT16U heartbeat, LINK_Request* link_req) {
    TS ts = {};
    TSGet(&ts);
    link_req->type              = link_type;
    link_req->piid_acd.data     = 0;
    link_req->time.year         = ((ts.Year << 8) & 0xff00) | ((ts.Year >> 8) & 0xff); // apdu 先高后低
    link_req->time.month        = ts.Month;
    link_req->time.day_of_month = ts.Day;
    link_req->time.day_of_week  = ts.Week;
    link_req->time.hour         = ts.Hour;
    link_req->time.minute       = ts.Minute;
    link_req->time.second       = ts.Sec;
    link_req->time.milliseconds = 0;
    link_req->heartbeat         = ((heartbeat << 8) & 0xff00) | ((heartbeat >> 8) & 0xff);
}

void Comm_task(CommBlock* compara) {
    int sendlen           = 0;
    static time_t oldtime = 0;
    TS ts                 = {};
    INT16U heartbeat      = 60;

    time_t newtime = time(NULL);
    if (abs(newtime - oldtime) > heartbeat) {
        TSGet(&ts);
        oldtime = newtime;
        if (compara->testcounter >= 2) {
            close(compara->phy_connect_fd);
            compara->phy_connect_fd = -1;
            AT_POWOFF();
            compara->testcounter = 0;
            return;
        } else if (compara->linkstate == close_connection) //物理通道建立完成后，如果请求状态为close，则需要建立连接
        {
            WriteLinkRequest(build_connection, heartbeat, &compara->link_request);
            asyslog(LOG_INFO, "建立链接 %02d-%02d-%02d %d:%d:%d\n", compara->link_request.time.year,
                    compara->link_request.time.month, compara->link_request.time.day_of_month, compara->link_request.time.hour,
                    compara->link_request.time.minute, compara->link_request.time.second);
            sendlen = Link_Request(compara->link_request, compara->serveraddr, compara->SendBuf);
        } else {
            WriteLinkRequest(heart_beat, heartbeat, &compara->link_request);
            sendlen = Link_Request(compara->link_request, compara->serveraddr, compara->SendBuf);
        }
        compara->p_send(compara->phy_connect_fd, compara->SendBuf, sendlen);
        compara->testcounter++;
    }
}

void initComPara(CommBlock* compara) {
    int ret = 0, i = 0;
    CLASS19 oi4300    = {};
    CLASS_F101 oif101 = {};
    CLASS_4001_4002_4003 c4001;
    memset(&c4001, 0x00, sizeof(c4001));
    readCoverClass(0x4001, 0, &c4001, sizeof(c4001), para_vari_save);
    memcpy(compara->serveraddr, c4001.curstom_num, 16);
    compara->phy_connect_fd = -1;
    compara->testcounter    = 0;
    compara->linkstate      = close_connection;
    memset(compara->RecBuf, 0, sizeof(compara->RecBuf));
    memset(compara->SendBuf, 0, sizeof(compara->SendBuf));
    memset(compara->DealBuf, 0, sizeof(compara->DealBuf));
    compara->RHead     = 0;
    compara->RTail     = 0;
    compara->deal_step = 0;
    compara->rev_delay = 20;
    compara->shmem     = JProgramInfo;
    compara->p_send    = GenericWrite;

    memset(&oi4300, 0, sizeof(CLASS19));
    ret = readCoverClass(0x4300, 0, &oi4300, sizeof(CLASS19), para_vari_save);
    if (ret)
        memcpy(&compara->myAppVar.server_factory_version, &oi4300.info, sizeof(FactoryVersion));
    for (i = 0; i < 2; i++) {
        compara->myAppVar.FunctionConformance[i] = 0xff;
    }
    for (i = 0; i < 5; i++) {
        compara->myAppVar.ProtocolConformance[i] = 0xff;
    }
    compara->myAppVar.server_deal_maxApdu    = 1024;
    compara->myAppVar.server_recv_size       = 1024;
    compara->myAppVar.server_send_size       = 1024;
    compara->myAppVar.server_recv_maxWindow  = 1;
    compara->myAppVar.expect_connect_timeout = 56400;
    //--------------------
    memset(&oif101, 0, sizeof(CLASS_F101));
    ret = readCoverClass(0xf101, 0, &oif101, sizeof(CLASS_F101), para_vari_save);
    memcpy(&compara->f101, &oif101, sizeof(CLASS_F101));
}

/*********************************************************
 * 进程初始化
 *********************************************************/
void enviromentCheck(int argc, char* argv[]) {
    pid_t pids[128];
    if (prog_find_pid_by_name((INT8S*)argv[0], pids) > 1) {
        asyslog(LOG_ERR, "CJCOMM进程仍在运行,进程号[%d]，程序退出...", pids[0]);
        exit(0);
    }

    //绑定信号处理了函数
    struct sigaction sa = {};
    Setsig(&sa, QuitProcess);

    //读取设备参数
    readCoverClass(0x4500, 0, (void*)&Class25, sizeof(CLASS25), para_vari_save);
    asyslog(LOG_INFO, "工作模式 enum{混合模式(0),客户机模式(1),服务器模式(2)}：%d", Class25.commconfig.workModel);
    asyslog(LOG_INFO, "在线方式 enum{永久在线(0),被动激活(1)}：%d", Class25.commconfig.onlineType);
    asyslog(LOG_INFO, "连接方式 enum{TCP(0),UDP(1)}：%d", Class25.commconfig.connectType);
    asyslog(LOG_INFO, "连接应用方式 enum{主备模式(0),多连接模式(1)}：%d", Class25.commconfig.appConnectType);
    asyslog(LOG_INFO, "侦听端口列表：%d", Class25.commconfig.listenPort[0]);
    asyslog(LOG_INFO, "超时时间，重发次数：%02x", Class25.commconfig.timeoutRtry);
    asyslog(LOG_INFO, "心跳周期秒：%d", Class25.commconfig.heartBeat);

    //向cjmain报告启动
    JProgramInfo = OpenShMem("ProgramInfo", sizeof(ProgramInfo), NULL);
    memcpy(JProgramInfo->Projects[1].ProjectName, "cjcomm", sizeof("cjcomm"));
    JProgramInfo->Projects[1].ProjectID = getpid();
}

int main(int argc, char* argv[]) {
    printf("version 1015\n");

    enviromentCheck(argc, argv);

    CreateATWorker(&Class25);

    //开启网络IO事件处理框架
    aeEventLoop* ep;
    ep = aeCreateEventLoop(128);
    if (ep == NULL) {
        asyslog(LOG_ERR, "事件处理框架创建失败，程序终止。\n");
        exit(0);
    }

    StartIfr(ep, 0, NULL);
    StartSerial(ep, 0, NULL);
    StartServer(ep, 0, NULL);
    StartVerifiTime(ep, 0, JProgramInfo);
    StartClient(ep, 0, &Class25);

    aeMain(ep);

    //退出信号
    QuitProcess(99);
    return EXIT_SUCCESS;
}
