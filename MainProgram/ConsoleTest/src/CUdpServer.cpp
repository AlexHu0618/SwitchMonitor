#include <stdio.h>
#include <iostream>
#include "CUdpServer.h"

using namespace std;

CUdpServer::CUdpServer(const char* serverAddress, const char* clientAddress, int port)
{
    //ctor
    BufLen=1500;//��������С
//    dataTemp[100][1500]={0};
    //��ʼ��Socket
    WSAStartup(MAKEWORD(2,2),&wsaData);
    //�����������ݱ���socket
    RecvSocket=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    //��socket���ƶ��˿ں�0.0.0.0��
    RecvAddr.sin_family=AF_INET;
    RecvAddr.sin_port=htons(port);
    RecvAddr.sin_addr.s_addr=inet_addr(serverAddress);   //192.168.1.106
    bind(RecvSocket,(SOCKADDR *)&RecvAddr,sizeof(RecvAddr));

    SenderAddr.sin_family=AF_INET;
    SenderAddr.sin_port=htons(port);
    SenderAddr.sin_addr.s_addr=inet_addr(clientAddress);  //192.168.1.154
}

CUdpServer::~CUdpServer()
{
    //dtor
    closesocket(RecvSocket);
    WSACleanup();
}

int CUdpServer::SendData(char *pdata)
{
    int sendBytes = sendto(RecvSocket,pdata,sizeof(pdata),0,(SOCKADDR *)&SenderAddr,sizeof(SenderAddr));
    if(sendBytes != SOCKET_ERROR)
    {
        cout << "send " << sendBytes << "bytes." << endl;
        return 0;
    }
    else
    {
        cout << "send command error." << endl;
        return -1;
    }
}

int CUdpServer::RecvData(char (*pdata)[1470])
{
    //����Recvfrom�����ڰ󶨵�socket�Ͻ�������
    cout << "receiving datagrams..." << endl;
    fd_set readfds,testfds;
    struct timeval tv;
    FD_ZERO(&readfds);
    FD_SET(RecvSocket,&readfds);
    tv.tv_sec=3;
    tv.tv_usec=0;
    int SenderAddrSize=sizeof(SenderAddr);
    int result;
    int total=0;
    int frameCout=0;
    bool haveReceived=false;
    while(true)
    {
        testfds = readfds;
        result = select(FD_SETSIZE,&testfds,NULL,NULL,&tv);
        if(!(result>0) && haveReceived)
        {
            break;
        }
        else{
        switch(result)
        {
        case -1:
            perror("select");
            break;
        case 0:
            break;
        default:
            if(FD_ISSET(RecvSocket,&testfds))
            {
//                int recvBytes = recvfrom(RecvSocket,RecvBuf,BufLen,0,(SOCKADDR *)&SenderAddr,&SenderAddrSize);
//                int recvBytes = recvfrom(RecvSocket,dataTemp[frameCout++],BufLen,0,(SOCKADDR *)&SenderAddr,&SenderAddrSize);
                int recvBytes = recvfrom(RecvSocket,*pdata,BufLen,0,(SOCKADDR *)&SenderAddr,&SenderAddrSize);
                if(recvBytes>0)
                {
//                    printf("the recvBytes is: %d\n",recvBytes);
//                    for(int i=0;i<recvBytes;++i)
//                    {
//                        printf("%02X ",(unsigned char)RecvBuf[i]);
//                    }
//                    printf("\n");
                    total += recvBytes;
                    frameCout++;
                    pdata++;
//                    strcpy(dataTemp[i++],RecvBuf);
//                    memcpy(dataTemp[i++],RecvBuf,recvBytes);
                }
                else
                {
                    printf("no data\n");
                }
                haveReceived=true;
            }
            break;
        }}
    }
//    pdata = dataTemp;
    cout << endl;
    cout << "recived " << total <<" Bytes data" << endl;
    return frameCout;
}

bool CUdpServer::FrameAnalysis(int frameCout)
{

}
