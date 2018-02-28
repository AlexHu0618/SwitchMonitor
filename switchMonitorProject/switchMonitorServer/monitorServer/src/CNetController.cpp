#include "CNetController.h"
#include <iostream>

CNetController::CNetController( string strIPAddr, unsigned int nPort, eTypeCS eType )
{
    m_strIPAddr = strIPAddr;
    m_nPort = nPort;
    m_eType = eType;
    m_bConnected = false;
}

CNetController::~CNetController()
{
    if( m_eType == server && m_bConnected )
    {
        closesocket( m_sockConn );
    }
    closesocket( m_sock );
    WSACleanup();
}

int CNetController::Initial()
{
    WSADATA wsaData;
    if( WSAStartup(MAKEWORD(2, 2), &wsaData) != 0 )
    {
        cout << "Failed to load Winsock" << endl;
        return -1;
    }

    //创建用于监听的TCP套接字
    m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if( m_sock == SOCKET_ERROR )
    {
        cout << "Socket() error: " << WSAGetLastError();
        return -1;
    }

    SOCKADDR_IN addrSrv;
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(m_nPort); //1024以上的端口号

    if( m_eType == server )
    {
        addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

        /**< bind and listen */
        int retVal = bind(m_sock, (LPSOCKADDR)&addrSrv, sizeof(SOCKADDR_IN));
        if( retVal == SOCKET_ERROR )
        {
            cout << "Failed bind: " << WSAGetLastError() << endl;
            return -1;
        }
        if( listen(m_sock, 10) == SOCKET_ERROR )
        {
            cout << "Listen failed: " << WSAGetLastError() << endl;
            return -1;
        }
        cout << "waitting for Client connecting..." << endl;
        while( 1 )
        {
            //等待客户请求到来
            int nLen = sizeof(SOCKADDR);
            m_sockConn = accept(m_sock, (SOCKADDR*)&m_addrClient, &nLen);
            if( m_sockConn == SOCKET_ERROR )
            {
                cout << "Accept failed: " << WSAGetLastError() << endl;
                return -1;
            }
            cout << "Accept client IP: " << inet_ntoa(m_addrClient.sin_addr) << endl;
            m_bConnected = true;
            return 0;
        }
    }
    else if ( m_eType == client )
    {
         char IP[20]={0};
         if( !__GetLocalIP( wsaData, IP ))
         {
             cout << "GetLocalIP failed!" << endl;
             return -1;
         }
         addrSrv.sin_addr.S_un.S_addr = inet_addr(IP);
         /**< connect to server */
         int retval = connect( m_sock, (struct sockaddr*)&addrSrv, sizeof(addrSrv));
         if( retval  == INVALID_SOCKET )
         {
             cout << "Connect failed: " << WSAGetLastError();
             return -1;
         }
         else
         {
             m_bConnected = true;
             return 0;
         }
    }
    else
    {
        cout << "the argument 'eType' is wrong, please input 'sever' or 'client'!" << endl;
        return -1;
    }
}

bool CNetController::__GetLocalIP( WSADATA wsaData, char* pszIP )
{
    char hostname[256];
    int ret=gethostname(hostname,sizeof(hostname));
    if (ret==SOCKET_ERROR)
    {
        return false;
    }
    //3.获取主机ip
    HOSTENT* host=gethostbyname(hostname);
    if (host==NULL)
    {
        return false;
    }
    //4.转化为char*并拷贝返回
    strcpy(pszIP,inet_ntoa(*(in_addr*)*host->h_addr_list));
    return true;
}

int CNetController::Send( const char* pszBuf, int nLength )
{
    if( m_bConnected )
    {
        int nByteSent = 0;
        if( m_eType == server )
        {
            nByteSent = send(m_sockConn, pszBuf, nLength , 0);
        }
        else
        {
            nByteSent = send(m_sock, pszBuf, nLength , 0);
        }
        if( nByteSent == SOCKET_ERROR )
        {
            cout << "send failed" << endl;
            return -1;
        }
        return nByteSent;
    }
    else
    {
        cout << "no Connected" << endl;
        return -1;
    }
}

int CNetController::Recv( char* pszBuf, int nLength )
{
    if( m_bConnected )
    {
        int nByteRecv = recv( m_sockConn, pszBuf, nLength, 0 );
        return nByteRecv;
    }
    else
    {
        cout << "no Connected" << endl;
        return -1;
    }
}
