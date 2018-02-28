#ifndef CNETCONTROLLER_H
#define CNETCONTROLLER_H

#include <string>
#include <winsock2.h>

using namespace std;
typedef enum eType{server, client} eTypeCS;

class CNetController
{
private:
    string m_strIPAddr;
    unsigned int m_nPort;
    eTypeCS m_eType;
    SOCKADDR_IN m_addrClient;
    SOCKET m_sock;
    SOCKET m_sockConn;
    bool m_bConnected;

    bool __GetLocalIP( WSADATA wsaData, char* pszIP );

public:
    CNetController( string strIPAddr, unsigned int nPort, eTypeCS eType );
    ~CNetController();

    int Initial();
    int Send( const char* pszBuf, int nLength );
    int Recv( char* pszBuf, int nLength );
};

#endif // CNETCONTROLLER_H
