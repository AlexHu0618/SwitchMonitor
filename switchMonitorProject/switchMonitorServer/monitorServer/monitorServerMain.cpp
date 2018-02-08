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
    EVT_MENU(ANALYZE_FINISH_ID, monitorServerFrame::OnAnalyzeFinishEvent)
    EVT_TIMER(TIMER_ID, monitorServerFrame::OnTimer)
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
            for( unsigned int i=0;i<dwSzSize;i=i+2 )
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
            for( unsigned int i=0;i<dwSzSize;i=i+2 )
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
            for( unsigned int i=0;i<dwSzSize;i=i+2 )
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
    if (m_pDBCtrler->Initial("switchmonitordb", "tab4alldata" ) != 0)
    {
        cout << "ERROR! DB is not initialized." << endl;
    }

    wxIPV4address addr;
    addr.Service(nPort4TCP);

    //!< Create the socket
    m_server = new wxSocketServer(addr);

    //!< We use Ok() here to see if the server is really listening
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
    m_bACQIsRunning = false;
    m_numClients = 0;
    m_sockUI = NULL;
    m_sockAcquirer = NULL;
    m_sockDTU = NULL;
    m_emTypeofSwitch = ZD6;
    m_timer.SetOwner(this, TIMER_ID);

    m_pAnalyzer = new CFaultAnalyzer(m_pDBCtrler);

}


monitorServerFrame::~monitorServerFrame()
{
//    delete m_pTcpServer;
//    m_pTcpServer = NULL;
    delete m_pDBCtrler;
    m_pDBCtrler = NULL;
    delete m_server;
    m_server = NULL;
    delete m_pAnalyzer;
    m_pAnalyzer = NULL;
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
//    SWITCH_TYPE typeofSwitch = ZYJ7;
//    CFaultAnalyzer faultAnalyzer( std::string(strDataDirPath.mb_str()), typeofSwitch );
//    double arrdTransformRatio[] = { 600, 600, 600, 600, 40, 40, 40, 3000, 3000 };   // {v,v,v,v,i,i,i,p,p}
//    double *parrdScore = faultAnalyzer.GetScore( arrdTransformRatio );
//
//    printf("ZYJ7 fault confidences for provided data:\n\n");
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

            //!< Read the data
            do{
                sock->Read(&c, 1);
                *(tem++) = c;
                len++;
            }while(c!=0x0A);
            strBuf.Printf(_("%s"), buf);
            cout << "Client said: " << strBuf << endl;

            //!< reback the socket which received
            sock->Write(buf, len);

            strSubBefore = strBuf.Before('=');
            strSubAfter  = strBuf.After('=').BeforeLast('\r');
            if( strSubBefore == "CMD" )
            {
                m_pAnalyzer->SetBaseData(true);
//                switch(strSubAfter)
//                {
//                case "DEFAULT":
//                    m_pAnalyzer->SetBaseData(true);
//                    break;
//
//                case "STATUS":
//                    if (CheckACQ() !=0 )
//                    {
//                        cout << "ERROR! CheckACQ()." << endl;
//                    }
//                    break;
//
//                default:
//                    break;
//                }
            }
            else if( strSubBefore == "PATH" )
            {
                CAnalyzeThread *pAnalyzeThread = new CAnalyzeThread(this, m_pAnalyzer, strSubAfter);
                if (pAnalyzeThread->Create() != wxTHREAD_NO_ERROR)
                {
                    cout << "ERROR! Fail to create thread to analyze." << endl;
                }
                pAnalyzeThread->Run();
            }
            else if (strSubBefore == "MSG")
            {
//                SendMSG2UI( buf, len );
                if (strSubAfter == "S700K" || strSubAfter == "ZYJ7" || strSubAfter == "ZD6")
                {
                    m_emTypeofSwitch = (strSubAfter == "S700K") ? S700K : (strSubAfter == "ZYJ7") ? ZYJ7 : ZD6;
                }
                else if (strSubAfter == "RUNNING")
                {
                    m_bACQIsRunning = true;
                }
            }
            else if (strSubBefore == "ERR")
            {
                SendERR2DTU(buf, len);
            }
            else if (strSubBefore == "ID")
            {
                if (strSubAfter == "UI")
                {
                    m_sockUI = sock;
                }
                else if (strSubAfter == "ACQ")
                {
                    m_sockAcquirer = sock;
                    m_timer.Start(120000, true);    // 2min, oneshot
                }
                else if (strSubAfter == "DTU")
                {
                    m_sockDTU = sock;
                }
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
            SendERR2DTU("ERR=ACQLOST\r\n", 13);
            break;
        }
        default: ;
    }
}

void monitorServerFrame::OnAnalyzeFinishEvent(wxCommandEvent& event)
{
    cout << "send to UI" << endl;
    SendMSG2UI("MSG=UPDATE\r\n", 12);
    cout << "have sent to UI" << endl;
}

void monitorServerFrame::OnTimer(wxTimerEvent& event)
{
//    if (m_bACQIsRunning)
//    {
//        m_bACQIsRunning = false;
//        m_sockAcquirer->Write("CMD=STATUS\r\n", 12);
//    }
//    else
//    {
//        SendERR2DTU("ERR=ACQLOST\r\n", 13);
//    }
    if (!m_bACQIsRunning)
    {
        SendERR2DTU("ERR=ACQLOST\r\n", 13);
    }
}

void monitorServerFrame::SendMSG2UI( const void *buffer, wxUint32 nbytes )
{
    if( m_sockUI != NULL )
    {
        m_sockUI->Write( buffer, nbytes );
        cout << "sent info to UI" << endl;
    }
    else
    {
        cout << "ERROR! m_sockUI == NULL" << endl;
    }
}

void monitorServerFrame::SendERR2DTU( const void *buffer, wxUint32 nbytes )
{
    if( m_sockDTU != NULL )
    {
        m_sockDTU->Write( buffer, nbytes );
        cout << "sent info to DTU" << endl;
    }
    else
    {
        cout << "ERROR! m_sockDTU == NULL" << endl;
    }
}

bool monitorServerFrame::HaveSetAllDefault( void )
{
    string strTypeofAcq = "";
    return 0;
}

int monitorServerFrame::CheckACQ( void )
{
    if( m_sockAcquirer != NULL )
    {
        m_sockAcquirer->Write( "CMD=STATUS\r\n", 12 );
        cout << "sent info to client ACQ" << endl;
        return 0;
    }
    else
    {
        cout << "ERROR! m_sockAcquirer == NULL" << endl;
        return -1;
    }
}

void *CAnalyzeThread::Entry()
{
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_INTENSITY | FOREGROUND_GREEN );
    cout << "START A NEW TASK FOR" << endl;
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN |FOREGROUND_BLUE );

    int nResult = 0;
    nResult = m_pAnalyzer->Diagnosing( ZD6, m_strPath );   //!<  diagnose and save data and result
    if( nResult != 0 )
    {
        cout << "Fail to analyze!" << endl;
    }
    else
    {
        //!< send event to main thread
        wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, ANALYZE_FINISH_ID);
//        wxGetApp().AddPendingEvent(event);
        wxPostEvent(m_pFrame, event);

        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_INTENSITY | FOREGROUND_GREEN );
        cout << "WAITTING FOR NEXT TASK" << endl;
        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN |FOREGROUND_BLUE );
    }

    return NULL;
}
