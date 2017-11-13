/** \file CUdpServer.h
 *
 * \brief the head file include class CUdpServer, for UDP communication
 *
 * \author by Alex, jthu4alex@163.com, on 2017/11/14
 *
 */

#ifndef CUdpServer_H
#define CUdpServer_H

#include <WINSOCK2.H>

enum SWITCH_TYPE { ZD6, S700K, ZYJ7 };

/**
 * \brief 本类主要是负责UDP通信，包含功能为：发送、接收、保存源数据、获取(/设置)接收缓冲区大小、获取下位机开始采集的时间
 *
 */
class CUdpServer
{
    public:
        CUdpServer( const char *serverAddress, const char *clientAddress, int port, SWITCH_TYPE TypeofSwitch );
        //CUdpServer( const char *serverAddress, const char *clientAddress, int port );
        virtual ~CUdpServer();
        unsigned int GetrecvBufSize() { return m_unRecvBufSize; }
        void SetrecvBufSize( unsigned int val ) { m_unRecvBufSize = val; }
        int SendData( char *pszData );
        int RecvData( int nTimeUpSec );
        void SavingRawData( char* szDirPath );
        void GetTimeofStartAcq( SYSTEMTIME *tTimeofStartAcq );

    private:
        unsigned int m_unRecvBufSize;
        SWITCH_TYPE m_emTypeofSwitch;
        char (*m_ppszDataBuf)[1470];
        int (*m_ppnarrRawData)[370];
        int m_nFrameCounter;
        int m_nSizeofEveryChannelRawData;
        #ifdef _WIN32
        SYSTEMTIME m_tTimeofFirstRecv, m_tTimeofStartAcq;     //correct to ms
        #endif // _WIN32

        WSADATA wsaData;
        SOCKET RecvSocket;
        sockaddr_in RecvAddr;//服务器地址
        char RecvBuf[1500];//发送数据的缓冲区
        int m_nBufLen;//缓冲区大小
        sockaddr_in SenderAddr;//client addr

        bool __FrameAnalysis( void );
        void __ExtractRawData( void );
        bool __IsOKCmdBack( SOCKET sockFd, char* pszSentCmd, int nTimeOutMs );

};

#endif // CUdpServer_H
