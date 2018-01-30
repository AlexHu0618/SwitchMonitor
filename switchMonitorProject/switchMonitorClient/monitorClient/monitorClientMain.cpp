/***************************************************************
 * Name:      monitorClientMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Alex (jthu4alex@163.com)
 * Created:   2017-04-13
 * Copyright: Alex (www.gzrobot.net)
 * License:
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "monitorClientMain.h"
#include <wx/timer.h>
#include <wx/filename.h>
#include <wx/string.h>
#include <windows.h>
#include <shellapi.h>

using namespace std;

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

BEGIN_EVENT_TABLE(monitorClientFrame, wxFrame)
    EVT_CLOSE(monitorClientFrame::OnClose)
    EVT_MENU(idMenuQuit, monitorClientFrame::OnQuit)
    EVT_MENU(idMenuZD6, monitorClientFrame::OnZD6)
    EVT_MENU(idMenuS700K, monitorClientFrame::OnS700K)
    EVT_MENU(idMenuZYJ7, monitorClientFrame::OnZYJ7)
    EVT_SOCKET(SOCKET_ID, monitorClientFrame::OnSocketEvent)
END_EVENT_TABLE()

/** \brief 主窗体构造函数，负责窗体创建，通信连接初始化，并发送指令到下位机开始触发采集
 *
 * \param frame wxFrame*
 * \param title const wxString&
 *
 */
monitorClientFrame::monitorClientFrame(wxFrame *frame, const wxString& title)
    : wxFrame(frame, -1, title)
{
#if wxUSE_MENUS
    // create a menu bar
    wxMenuBar* mbar = new wxMenuBar();
    wxMenu* fileMenu = new wxMenu(_T(""));
    fileMenu->Append(idMenuQuit, _("&Quit\tAlt-F4"), _("Quit the application"));
    mbar->Append(fileMenu, _("&File"));

    wxMenu* startMenu = new wxMenu(_T(""));
    startMenu->Append(idMenuZD6, _("&ZD6\tF1"), _("Start to acquire data for ZD6"));
    startMenu->Append(idMenuS700K, _("&S700K\tF2"), _("Start to acquire data for S700K"));
    startMenu->Append(idMenuZYJ7, _("&ZYJ7\tF3"), _("Start to acquire data for ZYJ7"));
    mbar->Append(startMenu, _("&Start"));

    wxMenu* settingMenu = new wxMenu(_T(""));
    m_MenuItemDefault = new wxMenuItem(settingMenu, idMenuDefault, _("Default\tF2"), _("default data"), wxITEM_CHECK);
    settingMenu->Append(m_MenuItemDefault);
    mbar->Append(settingMenu, _("&Setting"));

    SetMenuBar(mbar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar with some information about the used wxWidgets version
    CreateStatusBar(2);
    SetStatusText(_("Hello Code::Blocks user!"),0);
    SetStatusText(wxbuildinfo(short_f), 1);
#endif // wxUSE_STATUSBAR

    //!< initial config     TCP->DB->UDP
    if( InitializeAll() < 0 )
    {
        return;
    }

    m_nAcqCounter = 0;
    m_bIsConnUDP = false;
    m_emTypeofSwitch = ZD6;

    AcquireLoop();
}

monitorClientFrame::~monitorClientFrame()
{
    m_pTcpClient->Send("Quit",4);
    delete m_pTcpClient;
    m_pTcpClient = NULL;
    delete m_pDBCtrler;
    m_pDBCtrler = NULL;
    delete m_MenuItemDefault;
    m_MenuItemDefault = NULL;
    delete m_pUDPServer;
    m_pUDPServer = NULL;
}

void monitorClientFrame::OnClose(wxCloseEvent &event)
{
    Destroy();
}

void monitorClientFrame::OnQuit(wxCommandEvent &event)
{
    Destroy();
}

void monitorClientFrame::OnZD6(wxCommandEvent &event)
{

}

void monitorClientFrame::OnS700K(wxCommandEvent &event)
{
//    //!< connect to UDP client
//    SWITCH_TYPE typeofSwitch = S700K;
//    m_pUDPServer = new CUdpServer( "192.168.1.106", "192.168.1.154", 1024, typeofSwitch );
//
//    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_INTENSITY | FOREGROUND_GREEN );
//    cout << "START A NEW TEST FOR S700K" << endl;
//    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN |FOREGROUND_BLUE );
//
//    int nResult = 0;
//    nResult = SaveData( TRIGGER );
}

void monitorClientFrame::OnZYJ7(wxCommandEvent &event)
{
//    //!< connect to UDP client
//    SWITCH_TYPE typeofSwitch = ZYJ7;
//    m_pUDPServer = new CUdpServer( "192.168.1.106", "192.168.1.154", 1024, typeofSwitch );
//
//    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_INTENSITY | FOREGROUND_GREEN );
//    cout << "START A NEW TEST FOR ZYJ7" << endl;
//    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN |FOREGROUND_BLUE );
//
//    int nResult = 0;
//    nResult = SaveData( TRIGGER );
}

/** \brief 本函数主要是等待接收下位机数据，一旦接收到数据就创建文件夹并保存数据到本地，通过TCP发送保存路径到服务器，把时间和路径写入数据库
 *
 * \param emAcqMode ACQUIRE_MODE，采集模式（触发或直接采集）
 * \enum ACQUIRE_MODE
 * \return int，成功返回0；不成功返回-1
 *
 */
int monitorClientFrame::SaveData( ACQUIRE_MODE emAcqMode )
{
    //!< directly acquire or else wait for triggering
    int frameCout = 0;
    int nTimeUpSec = 30;
    if (emAcqMode == DIRECT)
    {
        cout << "start to directly acquire 3S" << endl;
        char acqCmd[4] = {0x04,0x00,0x03,0x07};     //time is 3s
        int result = m_pUDPServer->SendData(acqCmd);
        if(result<0)
        {
            cout << "Failed to send command 0X04!" << endl;
            return -1;
        }
        cout << "success to send command 0X04!" << endl;
    }

    //!< stop acquiring while received data or till to time up
    frameCout = m_pUDPServer->RecvData( nTimeUpSec );
    if( frameCout > 0 )
    {
        //!< stop acquiring
//        char stopCmd[4] = {0x02,0x00,0x00,0x02};
//        result = m_pUDPServer->SendData(stopCmd);
//        if(result<0)
//        {
//            cout << "send command 0X02 false!" << endl;
//        }
//        else
//        {
//            cout << "received " << frameCout << " frames data!" << endl;
//            cout << "\n" << endl;
//        }

        //!< mkdir with datetime
        wxString str4dataDir = "D:\\";
        if( MakeDir( &str4dataDir ) != 0 )
        {
            cout << "MakeDir() failed!" << endl;
            return -1;
        }

        //!< save data to local dish
        char szDirPath[100] = {0};
        strcpy( szDirPath, str4dataDir.mb_str());
        m_pUDPServer->SavingRawData( szDirPath );

        //!< for test to choose specified data file for analyzing
//        wxString str4dataDir = _("/");
//        wxDirDialog dialog( this );
//        if (dialog.ShowModal() == wxID_OK)
//        {
//            str4dataDir = dialog.GetPath();
//            cout << str4dataDir << endl;
//        }
//        else
//        {
//            return -1;
//        }

        //!< send the path to TCP server for analyzing
        wxString strSendCmd = "PATH=" + str4dataDir + "\r\n";
        m_pTcpClient->Send((const char*)strSendCmd.mb_str(), strSendCmd.size());
        cout << strSendCmd << endl;

//        //!< insert to DB
//        str4dataDir.Replace("\\", "\\\\");
//        wxString strTYPE = (m_emTypeofSwitch == ZD6) ? "ZD6" : (m_emTypeofSwitch == S700K) ? "S700K" : "ZYJ7";
//        SYSTEMTIME tTimeofStartAcq;
//        m_pUDPServer->GetTimeofStartAcq( &tTimeofStartAcq );
//        wxString strDate = wxString::Format( wxT("%i"), tTimeofStartAcq.wYear ) + "-" + wxString::Format( wxT("%02i"), tTimeofStartAcq.wMonth ) + "-" + wxString::Format( wxT("%02i"), tTimeofStartAcq.wDay );
//        wxString strTime = wxString::Format( wxT("%02i"), tTimeofStartAcq.wHour ) + ":" + wxString::Format( wxT("%02i"), tTimeofStartAcq.wMinute ) + ":" + wxString::Format( wxT("%02i"), tTimeofStartAcq.wSecond );
//        cout << tTimeofStartAcq.wYear << "-" << tTimeofStartAcq.wMonth << "-" << tTimeofStartAcq.wDay << endl;
//        wxString strSQLCmd = "INSERT INTO tab4alldata(TYPE, DATE, TIME, PATH) VALUES ('" + strTYPE + "', '" + strDate + "', '" + strTime + "', '" + str4dataDir + "');";
//        string strCmd = string( strSQLCmd.mb_str() );
//        cout << strSQLCmd << endl;
//        cout << strCmd << endl;
//        int result = m_pDBCtrler->Insert( strCmd );
//        if( result < 0 )
//        {
//            cout << "sql insert error" << endl;
//            return -1;
//        }
    }
    else
    {
        cout << "maybe time up, no data was acquired!" << endl;
        return -1;
    }

    return 0;
}

/** \brief 本函数主要是创建文件夹，用于保存数据
 *
 * \param pstr4dataDir wxString*，创建后的文件夹路径名
 * \return int，成功返回0；不成功返回-1
 *
 */
int monitorClientFrame::MakeDir( wxString* pstr4dataDir )
{
    //these for built a directory and a file
    SYSTEMTIME tTimeofStartAcq;
    m_pUDPServer->GetTimeofStartAcq( &tTimeofStartAcq );
    wxString strDate = wxString::Format( wxT("%i"), tTimeofStartAcq.wYear ) + "-" + wxString::Format( wxT("%02i"), tTimeofStartAcq.wMonth ) + "-" + wxString::Format( wxT("%02i"), tTimeofStartAcq.wDay );
    wxString strTime = wxString::Format( wxT("%02i"), tTimeofStartAcq.wHour ) + "-" + wxString::Format( wxT("%02i"), tTimeofStartAcq.wMinute ) + "-" + wxString::Format( wxT("%02i"), tTimeofStartAcq.wSecond );

    wxString str4dir=_T("D:\\SwitchData\\")+strDate;
    *pstr4dataDir = str4dir + "\\" + strTime;
    wxFileName::SetCwd(_T("D:\\"));    //we should set the current working directory before build a sub-directory
    if(!wxFileName::DirExists(_T("SwitchData")))
    {
        if(!wxFileName::Mkdir(_T("SwitchData")))
        {
            wxMessageBox(_T("Fail to build directory SwitchData!"),_T("Error"));
            return -1;
        }
    }
    wxFileName::SetCwd(_T("D:\\SwitchData"));
    if(!wxFileName::DirExists(strDate))
    {
        if(!wxFileName::Mkdir(strDate))
        {
            wxMessageBox(_T("Fail to build directory by date!"),_T("Error"));
            return -1;
        }
    }
    wxFileName::SetCwd(str4dir);
    if(!wxFileName::Mkdir(strTime))
    {
        wxMessageBox(_T("Fail to build directory by date!"),_T("Error"));
        return -1;
    }
    wxFileName::SetCwd( *pstr4dataDir );

    return 0;
}

/** \brief 本函数主要是读取注册表中的参数值，UDP连接下位机并发送开始采集指令，进入循环等待接收数据
 *
 * \param void
 * \return void
 *
 */
void monitorClientFrame::AcquireLoop( void )
{
    if( !m_bIsConnUDP )
    {
#ifdef _WIN32
        //!< read regedit to get the IP,port,user and pw
        HKEY hKey;
        HKEY hTempKey;
        DWORD dwSize = sizeof(DWORD);
        DWORD dwType = REG_DWORD;
        DWORD nPort4UDP = 0;
        char szValue[256];
        char sztemp[64];
        DWORD dwSzType = REG_SZ;
        DWORD dwSzSize = sizeof(szValue);
        string strIP4UDP_Clt = "";
        string strIP4UDP_Sev = "";

        LPCTSTR data_Set= _T("Software\\GZMetro");
        if (ERROR_SUCCESS == ::RegOpenKeyEx(HKEY_CURRENT_USER, data_Set,0,KEY_ALL_ACCESS, &hKey))
        {
            //!< port4UDP
            if (::RegQueryValueEx(hKey, _T("port4UDP"), 0, &dwType, (LPBYTE)&nPort4UDP, &dwSize) != ERROR_SUCCESS)
            {
                 cout << "no key named port4UDP" << endl;
            }
            else
            {
                cout << "the port4UDP is " << nPort4UDP << endl;
            }

            //!< IP4UDP_Clt
            if (::RegQueryValueEx(hKey, _T("IP4UDP_Clt"), 0, &dwSzType, (LPBYTE)&szValue, &dwSzSize) != ERROR_SUCCESS)
            {
                 cout << "no key named IP4UDP_Clt" << endl;
            }
            else
            {
                //!< transform unicode to utf-8, or the raw data is 'szValue[]= '1 0 . 3 . 3 . 1 4 4'.
                for( unsigned int i=0;i<dwSzSize;i=i+2 )
                {
                    sztemp[i/2] = szValue[i];
                }

                strIP4UDP_Clt = sztemp;
                cout << "the IP4UDP client is " << strIP4UDP_Clt << endl;
                dwSzSize = 256;
            }

            //!< IP4UDP_Sev
            if (::RegQueryValueEx(hKey, _T("IP4UDP_Sev"), 0, &dwSzType, (LPBYTE)&szValue, &dwSzSize) != ERROR_SUCCESS)
            {
                 cout << "no key named IP4UDP_Sev" << endl;
            }
            else
            {
                //!< transform unicode to utf-8
                for( unsigned int i=0;i<dwSzSize;i=i+2 )
                {
                    sztemp[i/2] = szValue[i];
                }

                strIP4UDP_Sev = sztemp;
                cout << "the IP4UDP server is " << strIP4UDP_Sev << endl;
                dwSzSize = 256;
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
#else
    cout << "Not WIN32, NO regedit!" << endl;
#endif  // _WIN32

        //!< connect to UDP client
        m_pUDPServer = new CUdpServer( strIP4UDP_Sev.c_str(), strIP4UDP_Clt.c_str(), (int)nPort4UDP, m_emTypeofSwitch );
    }

    int nConnUDPCounter = 10;    // try 10 times if no conn
    do
    {
        //!< send command to start acquiring
        char startCmd[4] = {0x03,0x00,0x00,0x03};
        int result = m_pUDPServer->SendData(startCmd);
        if( result < 0 )
        {
            cout << "Failed to send command 0X03! Please try again!" << endl;
            nConnUDPCounter--;
            continue;
        }
        else
        {
            cout << "have sent command 0X03" << endl;
            m_bIsConnUDP = true;
        }
    }while( !m_bIsConnUDP && nConnUDPCounter>0 );

    //!< acquire by cycle
    if( m_bIsConnUDP )
    {
        wxString strSendCmd = "MSG=Acquirer is working!\r\n";
        m_pTcpClient->Send((const char*)strSendCmd.mb_str(), strSendCmd.size());
        int nResult = 0;
        string strTYPE = (m_emTypeofSwitch == ZD6) ? "ZD6" : (m_emTypeofSwitch == S700K) ? "S700K" : "ZYJ7";
        while(1)
        {
            if (nResult == 0)
            {
                //!< set the console text color
                SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_INTENSITY | FOREGROUND_GREEN );
                cout << "START A NEW TEST FOR " << strTYPE << endl;
                SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN |FOREGROUND_BLUE );
            }
            if (m_nAcqCounter < 11)            // trigger 11 times
            {
                nResult = SaveData( TRIGGER );
                if (nResult != 0)
                {
                    cout << "ERROR: Trigger acquire failed!" << endl;
                    continue;
                }
                else
                {
                    ++m_nAcqCounter;
                }
            }
            else                              // directly once
            {
                nResult = SaveData( DIRECT );
                if (nResult != 0)
                {
                    cout << "ERROR: Directly acquire failed!" << endl;
                    continue;
                }
                else
                {
                    m_nAcqCounter = 0;
                }
            }
        }
    }
    else
    {
        cout << "Please send cmd to server" << endl;
        wxMessageBox(_T("Fail to connect FPGA, please restart software after close all opening program!"));
    }
}

/** \brief 本函数主要是读取注册表参数值，创建TCP连接，创建MySQL数据库连接
 *
 * \param void
 * \return int，成功返回0；不成功返回-1
 *
 */
int monitorClientFrame::InitializeAll( void )
{
#ifdef _WIN32
    //!< open server.exe by shell
//    HINSTANCE hNewExe = ShellExecuteA( NULL, "open", ".\\Server\\monitorServer.exe", NULL, NULL, SW_SHOW );
//    if( (DWORD)hNewExe <=32 )
//    {
//        cout << "Failed to open server.exe! Error = " << hNewExe << endl;
//        return -1;
//
//    }
//    else
//    {
//        cout << "success to open server.exe!" << endl;
//    }

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
             return -1;
        }
        else
        {
            cout << "the value is " << nPort4TCP << endl;
        }

        //!< port4SQL
        if (::RegQueryValueEx(hKey, _T("port4SQL"), 0, &dwType, (LPBYTE)&nPort4SQL, &dwSize) != ERROR_SUCCESS)
        {
             cout << "no key named port4SQL" << endl;
             return -1;
        }
        else
        {
            cout << "the value is " << nPort4SQL << endl;
        }

        //!< IP4SQL
        if (::RegQueryValueEx(hKey, _T("IP4SQL"), 0, &dwSzType, (LPBYTE)&szValue, &dwSzSize) != ERROR_SUCCESS)
        {
             cout << "no key named IP4SQL" << endl;
             return -1;
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
             return -1;
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
             return -1;
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

#else
    cout << "Not WIN32, NO read regedit!" << endl;
#endif // _WIN32

    //!< initial TCP
    m_pTcpClient = new CNetController( "localhost", (unsigned int)nPort4TCP, client );
    if( m_pTcpClient->Initial() < 0 )
    {
        return -1;
    }
    else
    {
        int nresult = 0;
        nresult = m_pTcpClient->Send( "ID=ACQ\r\n", 8);
        if (nresult != 8)
        {
            cout << "Fail to send cmd \"ID=ACQ\"!" << endl;
            return -1;
        }
    }

    //!< initial DB
    m_pDBCtrler = new CSqlController( strIP4SQL, (unsigned int)nPort4SQL, strUser4SQL, strPW4SQL );
    if( m_pDBCtrler->Initial( "switchmonitordb", "tab4alldata" ) < 0 )
    {
        return -1;
    }

    return 0;
}

void monitorClientFrame::OnSocketEvent( wxSocketEvent &event )
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
            cout << "Server said: " << strBuf << endl;

            strSubBefore = strBuf.Before('=');
            strSubAfter  = strBuf.After('=').BeforeLast('\r');
            if( strSubBefore == "CMD" )
            {
                if (strSubAfter == "STATUS")
                {
                    //!< reback the socket which received
                    if (m_bIsConnUDP)
                    {
                        char szMSGBuf[] = "MSG=RUNNING\r\n";
                        sock->Write(szMSGBuf, 13);
                    }
                    else
                    {
                        char szMSGBuf[] = "ERR=UDPLOST\r\n";
                        sock->Write(szMSGBuf, 13);
                    }
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
        default: ;
    }
}
