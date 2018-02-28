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
 * \brief ������Ҫ�Ǹ���UDPͨ�ţ���������Ϊ�����͡����ա�����Դ���ݡ���ȡ(/����)���ջ�������С����ȡ��λ����ʼ�ɼ���ʱ��
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
        sockaddr_in RecvAddr;//��������ַ
        char RecvBuf[1500];//�������ݵĻ�����
        int m_nBufLen;//��������С
        sockaddr_in SenderAddr;//client addr

        bool __FrameAnalysis( void );
        void __ExtractRawData( void );
        bool __IsOKCmdBack( SOCKET sockFd, char* pszSentCmd, int nTimeOutMs );

};

#endif // CUdpServer_H
