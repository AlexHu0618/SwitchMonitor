/** \file CUdpServer.cpp
 *
 * \brief the c++ file for UDP communication
 *
 * \author by Alex, jthu4alex@163.com, on 2017/11/14
 *
 */

#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <time.h>
#include <math.h>
#include "CUdpServer.h"

/** \brief ��λ������Ƶ��Ϊ20kHz
 */
#define FREQ 20000     // frequency=20kHz

using namespace std;

/** \brief ���캯��
 *
 * \param serverAddress, ��������ַ; clientAddress, �ͻ��˵�ַ(��λ��); port, �˿ں�; TypeofSwitch, �豸����(ZD6/S700K/ZYJ7)
 * \enum CUdpServer::SWITCH_TYPE
 * \return ��
 *
 */
CUdpServer::CUdpServer( const char* serverAddress, const char* clientAddress, int port, SWITCH_TYPE TypeofSwitch )
{
    m_emTypeofSwitch = TypeofSwitch;
    //ctor
    m_nBufLen = 1500;//��������С
    //��ʼ��Socket
    WSAStartup( MAKEWORD(2,2),&wsaData );
    //�����������ݱ���socket
    RecvSocket = socket( AF_INET,SOCK_DGRAM,IPPROTO_UDP );
    //��socket���ƶ��˿ں�0.0.0.0��
    RecvAddr.sin_family = AF_INET;
    RecvAddr.sin_port = htons(port);
    RecvAddr.sin_addr.s_addr = inet_addr( serverAddress );   //192.168.1.106
    bind( RecvSocket, (SOCKADDR *)&RecvAddr, sizeof(RecvAddr) );

    SenderAddr.sin_family = AF_INET;
    SenderAddr.sin_port = htons(port);
    SenderAddr.sin_addr.s_addr = inet_addr(clientAddress);  //192.168.1.154
}

CUdpServer::~CUdpServer()
{
    //dtor
    closesocket(RecvSocket);
    WSACleanup();
    delete []m_ppszDataBuf;
    m_ppszDataBuf = NULL;
    delete []m_ppnarrRawData;
    m_ppnarrRawData = NULL;
}

/** \brief ��������Ҫ�Ƿ������ݣ�����ָ��0x03��0x04���ͺ�ֱ�ӷ����ж���λ���Ƿ����ָ��ɹ���1�볬ʱ��
 *
 * \param pszData, ���ݻ�����ָ��
 * \return �ɹ�����0�����ɹ�����-1
 *
 */
int CUdpServer::SendData( char *pszData )
{
    int sendBytes = sendto(RecvSocket,pszData,sizeof(pszData),0,(SOCKADDR *)&SenderAddr,sizeof(SenderAddr));
    if(sendBytes != SOCKET_ERROR)
    {
        cout << "send " << sendBytes << "bytes." << endl;
    }
    else
    {
        cout << "send command error." << endl;
        return -1;
    }
    //!< it must be reback for confirmation while sent command 0x03 and 0x04
    if( pszData[0] != 0x01 )
    {
        if( !__IsOKCmdBack( RecvSocket, pszData, 1000 ) )
        {
            cout << "the equitment has not reback the cmd!" << endl;
            return -1;
        }
    }

    return 0;
}

/** \brief ��������Ҫ���ж���λ���Ƿ����ָ��ɹ�
 *
 * \param sockFd SOCKET��socket�ļ���
 * \param pszSentCmd char*��Ҫ���͵�ָ�����ָ��
 * \param nTimeOutMs int����ʱʱ��
 * \return bool���ɹ�����0�����ɹ�����-1
 *
 */
bool CUdpServer::__IsOKCmdBack( SOCKET sockFd, char* pszSentCmd, int nTimeOutMs )
{
    int nResult = setsockopt( sockFd, SOL_SOCKET, SO_RCVTIMEO, (char*)&nTimeOutMs, sizeof(int) );
    if( nResult != 0 )
    {
        cout << "IsOKCmdBack() error" << endl;
        return false;
    }
    char* pBuf = new char[1470];
    nResult = recv( sockFd, pBuf, 1470, 0 );
    if( nResult > 0 )
    {
        if( pBuf[0] == pszSentCmd[0] && pBuf[2] == pszSentCmd[0] )
        {
            delete [] pBuf;
            pBuf = NULL;
            return true;
        }
        else
        {
            cout << "reback cmd is error" << endl;
            delete [] pBuf;
            pBuf = NULL;
            return false;
        }

    }
    else
    {
        cout << "recv() error " << nResult << endl;
        delete [] pBuf;
        pBuf = NULL;
        return false;
    }
}

/** \brief ��������Ҫ�ǽ�����λ�����ݣ��ڵ�һ֡����ʱ��ȡ����ʱ�䣬������ɺ�����Ƿ���©֡
 *
 * \param nTimeUpSec int����ʱʱ��
 * \return int���ɹ����ؽ��յ���֡�������ɹ�����0
 *
 */
int CUdpServer::RecvData( int nTimeUpSec )
{
    switch (m_emTypeofSwitch)
    {
    case S700K:
        m_ppszDataBuf = new char[20000][1470];
        break;
    case ZYJ7:
        m_ppszDataBuf = new char[30000][1470];
        break;
    case ZD6:
    default:
        m_ppszDataBuf = new char[10000][1470];   //[frames][bytes]
        break;
    }
    char (*ppszMovePtr)[1470] = m_ppszDataBuf;

    //����Recvfrom�����ڰ󶨵�socket�Ͻ�������
    cout << "receiving datagrams..." << endl;
    fd_set readfds,testfds;
    struct timeval tv;
    FD_ZERO(&readfds);
    FD_SET(RecvSocket,&readfds);
    tv.tv_sec= nTimeUpSec;
    tv.tv_usec=0;
    int SenderAddrSize=sizeof(SenderAddr);
    int result = -1;
    int total=0;
    m_nFrameCounter=0;
    bool haveReceived=false;
    int recvBytes = 0;
    while (true)
    {
        if( haveReceived )  //finished receving
        {
            break;
        }
        testfds = readfds;
        result = select( FD_SETSIZE, &testfds, NULL, NULL, &tv );  // polling every 3sec for checking the RecvSocket whether can be read

        switch(result)
        {
        case -1:
            perror("function select() error.\n");
            return 0;
        case 0:
            cout << "error!time up: " << tv.tv_sec << "s" << endl;
            return 0;
        default:
            if(FD_ISSET(RecvSocket,&testfds))   // check the filedescriptor set whether can be read, it means whether there are some datas in the receive buffer
            {
                #ifdef _WIN32
                GetLocalTime(&m_tTimeofFirstRecv);
                printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d week%1d\n",m_tTimeofFirstRecv.wYear,m_tTimeofFirstRecv.wMonth,m_tTimeofFirstRecv.wDay,m_tTimeofFirstRecv.wHour,m_tTimeofFirstRecv.wMinute, m_tTimeofFirstRecv.wSecond,m_tTimeofFirstRecv.wMilliseconds,m_tTimeofFirstRecv.wDayOfWeek);
                #endif // _WIN32
                do
                {
                    recvBytes = recvfrom( RecvSocket, *ppszMovePtr, m_nBufLen, 0, (SOCKADDR *)&SenderAddr, &SenderAddrSize );
                    if(recvBytes>0)
                    {
                        total += recvBytes;
                        m_nFrameCounter++;
                        ppszMovePtr++;
                    }
                }while( recvBytes > 0 );
                haveReceived=true;
            }
            break;
        }
    }

    cout << endl;
    cout << "recived " << total <<" Bytes data" << endl;

    //!<  analyzing frame
    cout << "start to analyze frame..." << endl;
    bool bAnalyzeIsOK = __FrameAnalysis();

    return m_nFrameCounter;
}

/** \brief ��������Ҫ�Ƿ����Ƿ�©֡����������������������λ���ɼ�������ʱ��
 *
 * \param void
 * \return bool���ɹ�����true�����ɹ�����false
 *
 */
bool CUdpServer::__FrameAnalysis( void )
{
    int nAreaNum = m_ppszDataBuf[0][0]&0xFF;
    int nAreaFrameNum = 0;
    int nAreaFrameCounter = 0;
    for (int nFrameNum = 0; nFrameNum < m_nFrameCounter; nFrameNum++)
    {
        if ((m_ppszDataBuf[nFrameNum][0]&0xFF) == nAreaNum)      // all the frames in the same area
        {
            nAreaFrameNum = (m_ppszDataBuf[0][2]&0xFF) | (m_ppszDataBuf[0][3]<<8&0xFF00);
            if (nAreaFrameNum != nAreaFrameCounter)
            {
                cout << "miss frame in the air(frmaNum): " << nAreaNum  << "(" << nAreaFrameNum << ")" << endl;
                return false;
            }
            nAreaFrameCounter++;
        }
        else
        {
            nAreaNum = m_ppszDataBuf[nFrameNum][0]&0xFF;      //next area
            nAreaFrameCounter = 0;
        }
    }
    //caculate the start acquiring time
    int nSecondofAcq = round((int)((m_ppszDataBuf[0][4]&0xFF) | (m_ppszDataBuf[0][5]<<8&0xFF00) | (m_ppszDataBuf[0][6]&0xFF0000) | ((m_ppszDataBuf[0][7]<<8&0xFF000000)/FREQ)));
    m_tTimeofStartAcq = m_tTimeofFirstRecv;
    if( m_tTimeofFirstRecv.wSecond < nSecondofAcq )
    {
        m_tTimeofStartAcq.wMinute = m_tTimeofFirstRecv.wMinute;
        m_tTimeofStartAcq.wSecond = m_tTimeofFirstRecv.wSecond + 60 - nSecondofAcq;
    }
    else
    {
        m_tTimeofStartAcq.wSecond = m_tTimeofFirstRecv.wSecond - nSecondofAcq;
    }
    return true;
}

/** \brief ��������Ҫ�Ǵ�ÿһ֡�г�ȡÿһ������ֵ��32bit��ʾ�����󻺳���
 *
 * \param void
 * \return void
 *
 */
void CUdpServer::__ExtractRawData( void )
{
    switch (m_emTypeofSwitch)
    {
    case S700K:
        m_ppnarrRawData = new int[20000][370];
        break;
    case ZYJ7:
        m_ppnarrRawData = new int[30000][370];
        break;
    case ZD6:
    default:
        m_ppnarrRawData = new int[10000][370];
        break;
    }

    m_nSizeofEveryChannelRawData = (m_ppszDataBuf[0][4]&0xFF) | (m_ppszDataBuf[0][5]<<8&0xFF00) | (m_ppszDataBuf[0][6]&0xFF0000) | (m_ppszDataBuf[0][7]<<8&0xFF000000);

    for (int i = 0; i < m_nFrameCounter; i++)
    {
        int byteCounter=0;
        for(int j=8;j<1468;j=j+4)
        {
            m_ppnarrRawData[i][byteCounter++] = (m_ppszDataBuf[i][j]&0xFF) | (m_ppszDataBuf[i][j+1]<<8&0xFF00) | (m_ppszDataBuf[i][j+2]<<16&0xFF0000) | (m_ppszDataBuf[i][j+3]<<24&0xFF000000);
        }
    }

    cout << "extracting raw data successful!" << endl;
}

/** \brief ��������Ҫ�Ǳ���󻺳����г�ȡ��Դ����
 *
 * \param szDirPath char*��Ҫ������ļ�������ָ��
 * \return void
 *
 */
void CUdpServer::SavingRawData( char* szDirPath )
{
    __ExtractRawData();

    int nTotalChannel = 3;
    switch (m_emTypeofSwitch)
    {
    case S700K:
        nTotalChannel = 6;
        break;
    case ZYJ7:
        nTotalChannel = 9;
        break;
    case ZD6:
    default:
        nTotalChannel = 3;
        break;
    }

    int nNumSavingFrameStart = 0;
    int nNumSavingFrameEnd = 0;
    for (int nChannelNum = 0; nChannelNum < nTotalChannel; ++nChannelNum)
    {
        char szFileName[] = {"DataCH"};
        char szChNum[2];
        snprintf( szChNum, sizeof(szChNum), "%d", nChannelNum );
        strcat( szFileName, szChNum );
        strcat( szFileName, ".dat");
        //wxString strFileName = _("DataCH") + wxString::Format( wxT("%i"), nChannelNum) + _(".dat");
        //FILE *pFile = fopen( strFileName.mb_str(), "wb" );
        FILE *pFile = fopen( szFileName, "wb" );
        if (pFile == NULL)
        {
            cout << "Can not open file " << szFileName << endl;
        }
        else
        {
            cout << "has opened the file " << szFileName << endl;
        }
        nNumSavingFrameEnd = (int)(m_nFrameCounter*(nChannelNum+1)/nTotalChannel);
        int nSizeofCHFrame = nNumSavingFrameEnd - nNumSavingFrameStart;

        for (int nFrameNum = nNumSavingFrameStart; nFrameNum < nNumSavingFrameEnd; ++nFrameNum)
        {
            //!< whether is the last frame of CH
            if( nFrameNum == (nNumSavingFrameEnd-1) )
            {
                for (int nDataNum = 0; nDataNum < (m_nSizeofEveryChannelRawData-365*(nSizeofCHFrame-1)); ++nDataNum)
                {
                    fprintf( pFile, "%d\n", m_ppnarrRawData[nFrameNum][nDataNum] );
                }
            }
            else
            {
                for (int nDataNum = 0; nDataNum < 365; ++nDataNum)
                {
                    fprintf( pFile, "%d\n", m_ppnarrRawData[nFrameNum][nDataNum] );
                }
            }
        }

        fclose( pFile );
        nNumSavingFrameStart = nNumSavingFrameEnd;
    }

    cout << "saving data to the file successful!" << endl;

    //!< show data
    for(int k=0;k<1500;k++)
    {
        cout << hex << setfill('0') << setw(2) << (unsigned int)(unsigned char)m_ppszDataBuf[0][k] << " ";
    }
    cout << endl;
    for(int j=0;j<365;j++)
    {
        cout << dec << m_ppnarrRawData[0][j] << " ";
    }
    cout << endl;
}

/** \brief ��������Ҫ�ǻ�ȡ��λ�������ɼ���ʱ��
 *
 * \param tTimeofStartAcq SYSTEMTIME*������ʱ��Ľṹ��
 * \return void
 *
 */
void CUdpServer::GetTimeofStartAcq( SYSTEMTIME *tTimeofStartAcq )
{
    tTimeofStartAcq->wYear = m_tTimeofStartAcq.wYear;
    tTimeofStartAcq->wMonth = m_tTimeofStartAcq.wMonth;
    tTimeofStartAcq->wDay = m_tTimeofStartAcq.wDay;
    tTimeofStartAcq->wHour = m_tTimeofStartAcq.wHour;
    tTimeofStartAcq->wMinute = m_tTimeofStartAcq.wMinute;
    tTimeofStartAcq->wSecond = m_tTimeofStartAcq.wSecond;
    tTimeofStartAcq->wMilliseconds = m_tTimeofStartAcq.wMilliseconds;
}
