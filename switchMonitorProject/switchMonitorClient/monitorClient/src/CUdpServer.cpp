#include <stdio.h>
#include <iostream>
#include <iomanip>
#include "CUdpServer.h"

using namespace std;

CUdpServer::CUdpServer( const char* serverAddress, const char* clientAddress, int port, SWITCH_TYPE TypeofSwitch )
{
    m_emTypeofSwitch = TypeofSwitch;
    //ctor
    m_nBufLen = 1500;//缓冲区大小
    //初始化Socket
    WSAStartup( MAKEWORD(2,2),&wsaData );
    //创建接收数据报的socket
    RecvSocket = socket( AF_INET,SOCK_DGRAM,IPPROTO_UDP );
    //将socket与制定端口和0.0.0.0绑定
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

int CUdpServer::SendData( char *pszData )
{
    int sendBytes = sendto(RecvSocket,pszData,sizeof(pszData),0,(SOCKADDR *)&SenderAddr,sizeof(SenderAddr));
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

int CUdpServer::RecvData( void )
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
        m_ppszDataBuf = new char[10000][1470];
        break;
    }
    char (*ppszMovePtr)[1470] = m_ppszDataBuf;

    //调用Recvfrom函数在绑定的socket上接收数据
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
    m_nFrameCounter=0;
    bool haveReceived=false;
    while (true)
    {
        testfds = readfds;
        result = select( FD_SETSIZE,&testfds,NULL,NULL,&tv );  // polling every 3sec for checking the RecvSocket whether can be read
        if(!(result>0) && haveReceived)
        {
            break;
        }
        else{
        switch(result)
        {
        case -1:
            perror("function select() error.\n");
            break;
        case 0:
            break;
        default:
            if(FD_ISSET(RecvSocket,&testfds))   // check the filedescriptor set whether can be read, it means whether there are some datas in the receive buffer
            {
                int recvBytes = recvfrom( RecvSocket, *ppszMovePtr, m_nBufLen, 0, (SOCKADDR *)&SenderAddr, &SenderAddrSize );
                if(recvBytes>0)
                {
                    total += recvBytes;
                    m_nFrameCounter++;
                    ppszMovePtr++;
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

    cout << endl;
    cout << "recived " << total <<" Bytes data" << endl;
    return m_nFrameCounter;
}

bool CUdpServer::__FrameAnalysis( void )
{

}

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

void CUdpServer::SavingRawData( void )
{
    __ExtractRawData();

    int nTotalChannel = 4;
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
        nTotalChannel = 4;
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
        for (int nFrameNum = nNumSavingFrameStart; nFrameNum < nNumSavingFrameEnd; ++nFrameNum)
        {
            for (int nDataNum = 0; nDataNum < 365; ++nDataNum)
            {
                fprintf( pFile, "%d\n", m_ppnarrRawData[nFrameNum][nDataNum] );
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
