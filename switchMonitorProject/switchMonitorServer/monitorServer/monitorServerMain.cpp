/***************************************************************
 * Name:      monitorServerMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Alex (jthu4alex@163.com)
 * Created:   2017-03-09
 * Copyright: Alex (www.gzrobot.net)
 * License:
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "monitorServerMain.h"
#include <wx/dirdlg.h>
#include <string>
#include <windows.h>

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__WXMAC__)
        wxbuild << _T("-Mac");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

BEGIN_EVENT_TABLE(monitorServerFrame, wxFrame)
    EVT_CLOSE(monitorServerFrame::OnClose)
    EVT_MENU(idMenuQuit, monitorServerFrame::OnQuit)
    EVT_MENU(idMenuZD6, monitorServerFrame::OnZD6)
    EVT_MENU(idMenuS700K, monitorServerFrame::OnS700K)
    EVT_MENU(idMenuZYJ7, monitorServerFrame::OnZYJ7)
    EVT_SOCKET(SERVER_ID,  monitorServerFrame::OnServerEvent)
    EVT_SOCKET(SOCKET_ID,  monitorServerFrame::OnSocketEvent)
END_EVENT_TABLE()

monitorServerFrame::monitorServerFrame(wxFrame *frame, const wxString& title)
    : wxFrame(frame, -1, title)
{
#if wxUSE_MENUS
    // create a menu bar
    wxMenuBar* mbar = new wxMenuBar();
    wxMenu* fileMenu = new wxMenu(_T(""));
    fileMenu->Append(idMenuQuit, _("&Quit\tAlt-F4"), _("Quit the application"));
    mbar->Append(fileMenu, _("&File"));

    wxMenu* analyzeMenu = new wxMenu(_T(""));
    analyzeMenu->Append(idMenuZD6, _("&ZD6\tF1"), _("Analyze datas of ZD6"));
    analyzeMenu->Append(idMenuS700K, _("&S700K\tF2"), _("Analyze datas of S700K"));
    analyzeMenu->Append(idMenuZYJ7, _("&ZYJ7\tF3"), _("Analyze datas of ZYJ7"));
    mbar->Append(analyzeMenu, _("&Analyze"));

    SetMenuBar(mbar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar with some information about the used wxWidgets version
    CreateStatusBar(2);
    SetStatusText(_("Hello Code::Blocks user!"),0);
    SetStatusText(wxbuildinfo(short_f), 1);
#endif // wxUSE_STATUSBAR

    //!< read regedit to get the IP,port,user and pw
    HKEY hKey;
    HKEY hTempKey;
    DWORD dwSize = sizeof(DWORD);
    DWORD dwType = REG_DWORD;
    DWORD nPort4TCP = 0;
    DWORD nPort4SQL = 0;
    char szValue[256];
    char sztemp[64];
    DWORD dwSzType = REG_SZ;
    DWORD dwSzSize = sizeof(szValue);
    string strIP4SQL = "";
    string strUser4SQL = "";
    string strPW4SQL = "";

    LPCTSTR data_Set= _T("Software\\GZMetro");
    if (ERROR_SUCCESS == ::RegOpenKeyEx(HKEY_CURRENT_USER, data_Set,0,KEY_ALL_ACCESS, &hKey))
    {
        //!< port4TCP
        if (::RegQueryValueEx(hKey, _T("port4TCP"), 0, &dwType, (LPBYTE)&nPort4TCP, &dwSize) != ERROR_SUCCESS)
        {
             cout << "no key named port4TCP" << endl;
        }
        else
        {
            cout << "the value is " << nPort4TCP << endl;
        }

        //!< port4SQL
        if (::RegQueryValueEx(hKey, _T("port4SQL"), 0, &dwType, (LPBYTE)&nPort4SQL, &dwSize) != ERROR_SUCCESS)
        {
             cout << "no key named port4SQL" << endl;
        }
        else
        {
            cout << "the value is " << nPort4SQL << endl;
        }

        //!< IP4SQL
        if (::RegQueryValueEx(hKey, _T("IP4SQL"), 0, &dwSzType, (LPBYTE)&szValue, &dwSzSize) != ERROR_SUCCESS)
        {
             cout << "no key named IP4SQL" << endl;
        }
        else
        {
            //!< transform unicode to utf-8, or the raw data is 'szValue[]= '1 0 . 3 . 3 . 1 4 4'.
            for( int i=0;i<dwSzSize;i=i+2 )
            {
                sztemp[i/2] = szValue[i];
            }

            strIP4SQL = sztemp;
            cout << strIP4SQL << endl;
            dwSzSize = 256;
        }

        //!< user4SQL
        if (::RegQueryValueEx(hKey, _T("user4SQL"), 0, &dwSzType, (LPBYTE)&szValue, &dwSzSize) != ERROR_SUCCESS)
        {
             cout << "no key named user4SQL" << endl;
        }
        else
        {
            //!< transform unicode to utf-8
            for( int i=0;i<dwSzSize;i=i+2 )
            {
                sztemp[i/2] = szValue[i];
            }

            strUser4SQL = sztemp;
            cout << strUser4SQL << endl;
            dwSzSize = 256;
        }

        //!< pw4SQL
        if (::RegQueryValueEx(hKey,_T("pw4SQL"), 0, &dwSzType, (LPBYTE)&szValue, &dwSzSize) != ERROR_SUCCESS)
        {
             cout << "no key named pw4SQL" << endl;
        }
        else
        {
            //!< transform unicode to utf-8
            for( int i=0;i<dwSzSize;i=i+2 )
            {
                sztemp[i/2] = szValue[i];
            }

            strPW4SQL = sztemp;
            cout << strPW4SQL << endl;
        }
    }
    else
    {
        printf("the key do not exist, and will be created!\n");

        if (ERROR_SUCCESS == ::RegCreateKey(HKEY_CURRENT_USER, data_Set, &hTempKey))
        {
            printf("success to create key!\n");
        }
    }
    ::RegCloseKey(hKey);

    //m_pTcpServer = new CNetController( "localhost", (unsigned int)nPort4TCP, server );
    //m_pTcpServer->Initial();

    m_pDBCtrler = new CSqlController( strIP4SQL, (unsigned int)nPort4SQL, strUser4SQL, strPW4SQL );
    m_pDBCtrler->Initial("switchmonitordb", "tab4alldata" );

    wxIPV4address addr;
    addr.Service(nPort4TCP);

    // Create the socket
    m_server = new wxSocketServer(addr);

    // We use Ok() here to see if the server is really listening
    if (! m_server->Ok())
    {
        cout << "Could not listen at the specified port !" << endl;
        return;
    }
    else
    {
        cout << "Server listening." << endl;
    }

    // Setup the event handler and subscribe to connection events
    m_server->SetEventHandler(*this, SERVER_ID);
    m_server->SetNotify(wxSOCKET_CONNECTION_FLAG);
    m_server->Notify(true);

    m_busy = false;
    m_numClients = 0;
    m_bIsDefault = false;
    m_sockUI = NULL;
    m_sockAcquirer = NULL;

    //!< start analyzing
    //ZD6Analyzing();
}


monitorServerFrame::~monitorServerFrame()
{
//    delete m_pTcpServer;
//    m_pTcpServer = NULL;
    delete m_pDBCtrler;
    m_pDBCtrler = NULL;
    delete m_server;
    m_server = NULL;
}

void monitorServerFrame::OnClose(wxCloseEvent &event)
{
    Destroy();
}

void monitorServerFrame::OnQuit(wxCommandEvent &event)
{
    Destroy();
}

void monitorServerFrame::OnZD6(wxCommandEvent &event)
{
//    SWITCH_TYPE typeofSwitch = ZD6;
//    int nResult = 0;
//    while( true )
//    {
//        nResult = Diagnosing( typeofSwitch );   //!<  diagnose and save data and result
//        if( nResult != 0 )
//        {
//            break;
//        }
//    }
}

void monitorServerFrame::OnS700K(wxCommandEvent &event)
{
//    SWITCH_TYPE typeofSwitch = S700K;
//    int nResult = 0;
//    while( true )
//    {
//        nResult = Diagnosing( typeofSwitch );   //!<  diagnose and save data and result
//        if( nResult != 0 )
//        {
//            break;
//        }
//    }

//    wxString strDataDirPath = _("/");
//    wxDirDialog dialog( this );
//    if (dialog.ShowModal() == wxID_OK)
//    {
//        strDataDirPath = dialog.GetPath();
//    }
//    else
//    {
//        return;
//    }
//    SWITCH_TYPE typeofSwitch = S700K;
//    CFaultAnalyzer faultAnalyzer( std::string(strDataDirPath.mb_str()), typeofSwitch );
//    double arrdTransformRatio[] = { 600, 600, 600, 40, 40, 40 };  //{v,v,v,i,i,i}
//    double *parrdScore = faultAnalyzer.GetScore( arrdTransformRatio );
//
//    printf("S700K fault confidences for provided data:\n\n");
//	printf("Actuating fault: %.2f %%\n", parrdScore[0]);
//	printf("Engage difficult: %.2f %%\n", parrdScore[1]);
//	printf("Indicating fault: %.2f %%\n", parrdScore[2]);
//	printf("Jam: %.2f %%\n", parrdScore[3]);
//	printf("Motor fault: %.2f %%\n", parrdScore[4]);
//	printf("Movement resistance: %.2f %%\n", parrdScore[5]);
//	printf("Power fault: %.2f %%\n", parrdScore[6]);
//	printf("Unlock difficult: %.2f %%\n", parrdScore[7]);
//
//    cout << "Analyzing is successful!" << endl;
}

void monitorServerFrame::OnZYJ7(wxCommandEvent &event)
{
    wxString strDataDirPath = _("/");
    wxDirDialog dialog( this );
    if (dialog.ShowModal() == wxID_OK)
    {
        strDataDirPath = dialog.GetPath();
    }
    else
    {
        return;
    }
    SWITCH_TYPE typeofSwitch = ZYJ7;
    CFaultAnalyzer faultAnalyzer( std::string(strDataDirPath.mb_str()), typeofSwitch );
    double arrdTransformRatio[] = { 600, 600, 600, 600, 40, 40, 40, 3000, 3000 };   // {v,v,v,v,i,i,i,p,p}
    double *parrdScore = faultAnalyzer.GetScore( arrdTransformRatio );

    printf("ZYJ7 fault confidences for provided data:\n\n");
	printf("Actuating fault: %.2f %%\n", parrdScore[0]);
	printf("Engage difficult: %.2f %%\n", parrdScore[1]);
	printf("Indicating fault: %.2f %%\n", parrdScore[2]);
	printf("Jam: %.2f %%\n", parrdScore[3]);
	printf("Motor fault: %.2f %%\n", parrdScore[4]);
	printf("Movement resistance: %.2f %%\n", parrdScore[5]);
	printf("Power fault: %.2f %%\n", parrdScore[6]);
	printf("Unlock difficult: %.2f %%\n", parrdScore[7]);

    cout << "Analyzing is successful!" << endl;
}

int monitorServerFrame::Diagnosing( SWITCH_TYPE typeofSwitch, wxString strPath )
{
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_INTENSITY | FOREGROUND_GREEN );
    cout << "START A NEW TASK" << endl;
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN |FOREGROUND_BLUE );

//    //!< receive block and wait for TCP
//    char szRecvBuf[1024]={0};
//    m_pTcpServer->Recv( szRecvBuf, 1024 );
//    cout << szRecvBuf << endl;
//    string strDataDirPath = szRecvBuf;
//
//    if( szRecvBuf[0] == 'Q' )
//    {
//        //return -1;
//        Close(true);
//    }
//    bool bIsDefault = false;
//    if( strDataDirPath.substr(strDataDirPath.length()-8) == "_default" )
//    {
//        bIsDefault = true;
//        strDataDirPath.erase( strDataDirPath.length()-8, 8 );
//    }
    string strDataDirPath = strPath.ToStdString();

    CFaultAnalyzer faultAnalyzer( strDataDirPath, typeofSwitch );
//    double arrdTransformRatio[] = { 300.0, 300.0, 17.857 };    // {v1,v2,i} 3.0=300V, i=17.857A
    double arrdTransformRatioZD6[] = { 600, 600, 40 };
    double arrdTransformRatioS700K[] = { 600, 600, 600, 40, 40, 40 };  //{v,v,v,i,i,i}
    double arrdTransformRatioZYJ7[] = { 600, 600, 600, 600, 40, 40, 40, 3000, 3000 };   // {v,v,v,v,i,i,i,p,p}
    double *parrdTransformRatio = arrdTransformRatioZD6;
    if( typeofSwitch == S700K )
    {
        parrdTransformRatio = arrdTransformRatioS700K;
    }
    if( typeofSwitch == ZYJ7 )
    {
        parrdTransformRatio = arrdTransformRatioZYJ7;
    }

    double *parrdScore = NULL;
    parrdScore = faultAnalyzer.GetScore( parrdTransformRatio );

    printf("SWITCH fault confidences for provided data:\n\n");
    printf("Actuating fault: %.2f %%\n", parrdScore[0]);
    printf("Engage difficult: %.2f %%\n", parrdScore[1]);
    printf("Indicating fault: %.2f %%\n", parrdScore[2]);
    printf("Jam: %.2f %%\n", parrdScore[3]);
    printf("Motor fault: %.2f %%\n", parrdScore[4]);
    printf("Movement resistance: %.2f %%\n", parrdScore[5]);
    printf("Power fault: %.2f %%\n", parrdScore[6]);
    printf("Unlock difficult: %.2f %%\n", parrdScore[7]);

    cout << "Analyzing is successful!" << endl;

    //!< save real data, here just save the default data
    if( m_bIsDefault )
    {
        faultAnalyzer.SetBaseData( m_bIsDefault );
        int result = faultAnalyzer.SaveRealData( parrdTransformRatio );
        if (result != 0)
        {
            cout << "save real data fail!" << endl;
            return 1;
        }
        m_bIsDefault = false;
    }

    //!< save preprocessing data
    int nResult = faultAnalyzer.SaveAfterPreProcessing( parrdTransformRatio );
    if( nResult != 0 )
    {
        cout << "Error, fail to save preprocessing data!" << endl;
        return 1;
    }
    else
    {
        cout << "Preprocessing data was successfully saved!" << endl;
    }

    //!< update DB specified column
    string strTypeofAcq = "trigger";
    int nIsL2R = 1;
    faultAnalyzer.GetInfo( &strTypeofAcq, &nIsL2R );
    wxString wstrIsL2R = wxString::Format(wxT("%i"), nIsL2R );
    string strIsL2R = std::string(wstrIsL2R.mbc_str());
    string arrstrScore[10] = {"0"};
    for( int i=0;i<8;++i )
    {
        arrstrScore[i] = wxString::Format(wxT("%f"), parrdScore[i]);
    }
    wxString strTmp(strDataDirPath);
    strTmp.Replace("\\","\\\\");
    strDataDirPath = strTmp.mb_str();
    string strSetValue = "ACTUATING=" + arrstrScore[0] + ",ENGAGE=" + arrstrScore[1] + ",INDICATING=" + arrstrScore[2] + ",JAM=" + arrstrScore[3]
                        + ",MOTOR=" + arrstrScore[4] + ",MOVEMENT=" + arrstrScore[5] + ",POWERERR=" + arrstrScore[6] + ",UNLOCKERR=" + arrstrScore[7]
                        + ",STATUS='" + strTypeofAcq + "',ISL2R=" + strIsL2R;
    string strDBCmd = "UPDATE tab4alldata set " + strSetValue + " WHERE PATH='" + strDataDirPath + "';";
    cout << strDBCmd << endl;
    nResult = m_pDBCtrler->Insert( strDBCmd );
    if( nResult != 1 )
    {
        cout << "sql update error" << endl;
        return 1;
    }
    else
    {
        cout << "Database updata success!" << endl;
    }

    return 0;
}

void monitorServerFrame::ZD6Analyzing( wxString strPath )
{
    SWITCH_TYPE typeofSwitch = ZD6;
    int nResult = 0;
    nResult = Diagnosing( typeofSwitch, strPath );   //!<  diagnose and save data and result
    if( nResult != 0 )
    {
        cout << "Fail to analyze!" << endl;
    }
    else
    {
        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_INTENSITY | FOREGROUND_GREEN );
        cout << "WAITTING FOR NEXT TASK" << endl;
        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN |FOREGROUND_BLUE );
    }
}

void monitorServerFrame::OnServerEvent( wxSocketEvent &event )
{
    wxString s = _("OnServerEvent: ");
    wxSocketBase *sock;

    switch(event.GetSocketEvent())
    {
        case wxSOCKET_CONNECTION : s.Append(_("wxSOCKET_CONNECTION\n")); break;
        default                  : s.Append(_("Unexpected event !\n")); break;
    }

    cout << s << endl;

    // Accept new connection if there is one in the pending
    // connections queue, else exit. We use Accept(false) for
    // non-blocking accept (although if we got here, there
    // should ALWAYS be a pending connection).

    sock = m_server->Accept(false);

    if (sock)
    {
        cout << "New client connection accepted" << endl;
    }
    else
    {
        cout << "Error: couldn't accept a new connection" << endl;
        return;
    }

    sock->SetEventHandler(*this, SOCKET_ID);
    sock->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
    sock->Notify(true);

    m_numClients++;
}

void monitorServerFrame::OnSocketEvent( wxSocketEvent &event )
{
    wxString s = _("OnSocketEvent: ");
    wxSocketBase *sock = event.GetSocket();

    // First, print a message
    switch(event.GetSocketEvent())
    {
        case wxSOCKET_INPUT : s.Append(_("wxSOCKET_INPUT\n")); break;
        case wxSOCKET_LOST  : s.Append(_("wxSOCKET_LOST\n")); break;
        default             : s.Append(_("Unexpected event !\n")); break;
    }

    cout << s << endl;

    // Now we process the event
    switch(event.GetSocketEvent())
    {
        case wxSOCKET_INPUT:
        {
            // We disable input events, so that the test doesn't trigger
            // wxSocketEvent again.
            sock->SetNotify(wxSOCKET_LOST_FLAG);

            // Which test are we going to run?
            unsigned char c;
            unsigned char len=0;
//            char *buf;
            wxString strBuf, strSubBefore, strSubAfter;

            // Receive data from socket and send it back. We will first
            // get a byte with the buffer size, so we can specify the
            // exact size and use the wxSOCKET_WAITALL flag. Also, we
            // disabled input events so we won't have unwanted reentrance.
            // This way we can avoid the infamous wxSOCKET_BLOCK flag.

            sock->SetFlags(wxSOCKET_WAITALL);

            char buf[128]={0};

            char *tem = buf;

            // Read the data
            do{
                sock->Read(&c, 1);
                *(tem++) = c;
                len++;
            }while(c!=0x0A);
            strBuf.Printf(_("%s"), buf);
            cout << "Client said: " << strBuf << endl;
            cout << "first char: " << *buf << endl;

            // Write it back
            sock->Write(buf, len);

            strSubBefore = strBuf.Before('=');
            strSubAfter  = strBuf.After('=').BeforeLast('\r');
            cout << strSubBefore << endl;
            cout << strSubAfter << endl;
            if( strSubBefore == "CMD" )
            {
                m_bIsDefault = true;
                m_sockUI = sock;
            }
            else if( strSubBefore == "PATH" )
            {
                ZD6Analyzing(strSubAfter);
                m_sockAcquirer = sock;
            }
            else if( strSubBefore == "MSG" )
            {
                SendMSG2UI( buf, len );
            }
            else
            {
                cout << "unknow CMD or PATH" << endl;
            }

            // Enable input events again.
            sock->SetNotify(wxSOCKET_LOST_FLAG | wxSOCKET_INPUT_FLAG);
            break;
        }
        case wxSOCKET_LOST:
        {
            m_numClients--;

            // Destroy() should be used instead of delete wherever possible,
            // due to the fact that wxSocket uses 'delayed events' (see the
            // documentation for wxPostEvent) and we don't want an event to
            // arrive to the event handler (the frame, here) after the socket
            // has been deleted. Also, we might be doing some other thing with
            // the socket at the same time; for example, we might be in the
            // middle of a test or something. Destroy() takes care of all
            // this for us.

            cout << "Deleting socket." << endl;
            sock->Destroy();
            break;
        }
        default: ;
    }
}

void monitorServerFrame::SendMSG2UI( const void *buffer, wxUint32 nbytes )
{
    if( m_sockUI != NULL )
    {
        m_sockUI->Write( buffer, nbytes );
    }
}
