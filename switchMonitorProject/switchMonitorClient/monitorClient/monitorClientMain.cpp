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
#include "CUdpServer.h"
#include <wx/timer.h>
#include <wx/filename.h>
#include <wx/string.h>

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
END_EVENT_TABLE()

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

    //!< initial TCP
    m_pTcpClient = new CNetController( "localhost", 1024, client );
    m_pTcpClient->Initial();

    //!< initial DB
    m_pDBCtrler = new CSqlController( "10.3.3.144", 3306, "yfzx", "yfzx3305" );
    m_pDBCtrler->Initial( "switchmonitordb", "tab4alldata" );
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
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_INTENSITY | FOREGROUND_GREEN );
    cout << "START A NEW TEST" << endl;
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN |FOREGROUND_BLUE );

    SWITCH_TYPE typeofSwitch = ZD6;
    CUdpServer *pZD6Server = new CUdpServer( "192.168.1.106", "192.168.1.154", 1024, typeofSwitch );

    wxString msg = _(" YES --- Acquiring directly\n NO --- Trigger");
    int answer = wxMessageBox(msg, _("choose the style of acquiring"), wxYES_NO | wxCENTER );

    //!< send command to continue asquiring
    char startCmd[4] = {0x03,0x00,0x00,0x03};
    int result = pZD6Server->SendData(startCmd);
    if (result < 0)
    {
        cout << "send data false! Please try again!" << endl;
        delete pZD6Server;
        pZD6Server = NULL;
        return;
    }
    cout << "have send command 0X03" << endl;

    int frameCout = 0;
    int nTimeUpSec = NULL;
    if (answer == wxYES)
    {//!< directly acquire
        cout << "start to directly acquire 25S" << endl;
        for(int i=0;i<1000;++i)
        {
            result = 0;
        }
        char acqCmd[4] = {0x04,0x00,0x03,0x07};     //time is 3s
        result = pZD6Server->SendData(acqCmd);
        if(result<0)
        {
            cout << "send command 0X04 false!" << endl;
            delete pZD6Server;
            pZD6Server = NULL;
            return;
        }
        cout << "success to send command 0X04" << endl;
        nTimeUpSec = 10;
    }

    frameCout = pZD6Server->RecvData( nTimeUpSec );
    if( frameCout > 0 )
    {
        //!< send command to stop acquiring
        char stopCmd[4] = {0x02,0x00,0x00,0x02};
        result=pZD6Server->SendData(stopCmd);
        if(result<0)
        {
            cout << "send command 0X02 false!" << endl;
        }
        else
        {
            cout << "received " << frameCout << " frames data!" << endl;
            cout << "\n" << endl;
        }

        //!< mkdir with datetime
        wxString str4dataDir = "D:\\";
        if( MakeDir( &str4dataDir ) != 0 )
        {
            cout << "MakeDir() failed!" << endl;
        }

        //!< save data
        char szDirPath[100] = {0};
        strcpy( szDirPath, str4dataDir.mb_str());
        pZD6Server->SavingRawData( szDirPath );

        //!< send the path to TCP server for analyzing
        //str4dataDir = "F:\\ZD6\\new_voltage_sensor\\default\\1_static";    //just for debug test
        wxString strSendCmd = str4dataDir;
        if( m_MenuItemDefault->IsChecked() )
        {
            strSendCmd += "_default";
        }
        m_pTcpClient->Send((const char*)strSendCmd.mb_str(), strSendCmd.size());
        cout << strSendCmd << endl;

        //!< insert to DB
        str4dataDir.Replace("\\", "\\\\");
        wxString strSQLCmd = "INSERT INTO tab4alldata(TYPE, DATE, TIME, ACTUATING, PATH) VALUES ('ZD6', CURDATE(), CURTIME(), 50, '" + str4dataDir + "');";
        string strCmd = string( strSQLCmd.mb_str() );
        cout << strSQLCmd << endl;
        cout << strCmd << endl;
        result = m_pDBCtrler->Insert( strCmd );
        if( result != 1 )
        {
            cout << "sql insert error" << endl;
        }
    }
    else
    {
        cout << "maybe time up, no data was acquired!" << endl;
    }

    delete pZD6Server;
    pZD6Server = NULL;
}

void monitorClientFrame::OnS700K(wxCommandEvent &event)
{
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_INTENSITY | FOREGROUND_GREEN );
    cout << "START A NEW TEST" << endl;
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN |FOREGROUND_BLUE );

    SWITCH_TYPE typeofSwitch = S700K;
    CUdpServer *pS700KServer = new CUdpServer( "192.168.1.106", "192.168.1.154", 1024, typeofSwitch );
    //CUdpServer *pS700KServer = new CUdpServer( "10.3.3.116", "10.3.3.106", 1024, typeofSwitch );

    wxString msg = _(" YES --- Acquiring directly\n NO --- Trigger");
    int answer = wxMessageBox(msg, _("choose the style of acquiring"), wxYES_NO | wxCENTER );

    //!< send command to continue asquiring
    char startCmd[4] = {0x03,0x00,0x00,0x03};
    int result = pS700KServer->SendData(startCmd);
    if (result < 0)
    {
        cout << "send data false!" << endl;
    }
    cout << "have send command 0X03" << endl;

    if (answer == wxYES)
    {//!< directly acquire
        cout << "start to directly acquire 30S" << endl;
        for(int i=0;i<1000;++i)
        {
            result = 0;
        }
        char acqCmd[4] = {0x04,0x00,0x00,0x04};
        result = pS700KServer->SendData(acqCmd);
        if(result<0)
        {
            cout << "send command 0X04 false!" << endl;
        }
        cout << "success to send command 0X04" << endl;
    }

    int frameCout = pS700KServer->RecvData(30);

    //!< send command to stop acquiring
    char stopCmd[4] = {0x02,0x00,0x00,0x02};
    result=pS700KServer->SendData(stopCmd);
    if(result<0)
    {
        cout << "send data false!" << endl;
    }
    else
    {
        cout << "received " << frameCout << " frames data!" << endl;
        cout << "\n" << endl;
    }

    //!< save data
    char szDirPath[] = "E:\\";
    pS700KServer->SavingRawData( szDirPath );

    delete pS700KServer;
    pS700KServer = NULL;
}

void monitorClientFrame::OnZYJ7(wxCommandEvent &event)
{
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_INTENSITY | FOREGROUND_GREEN );
    cout << "START A NEW TEST" << endl;
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN |FOREGROUND_BLUE );

    SWITCH_TYPE typeofSwitch = ZYJ7;
    CUdpServer *pZYJ7Server = new CUdpServer( "192.168.1.106", "192.168.1.154", 1024, typeofSwitch );
    //CUdpServer *pZYJ7Server = new CUdpServer( "10.3.3.116", "10.3.3.106", 1024, typeofSwitch );

    wxString msg = _(" YES --- Acquiring directly\n NO --- Trigger");
    int answer = wxMessageBox(msg, _("choose the style of acquiring"), wxYES_NO | wxCENTER );

    //!< send command to continue asquiring
    char startCmd[4] = {0x03,0x00,0x00,0x03};
    int result = pZYJ7Server->SendData(startCmd);
    if (result < 0)
    {
        cout << "send data false!" << endl;
    }
    cout << "have send command 0X03" << endl;

    if (answer == wxYES)
    {//!< directly acquire
        cout << "start to directly acquire 25S" << endl;
        for(int i=0;i<1000;++i)
        {
            result = 0;
        }
        char acqCmd[4] = {0x04,0x00,0x00,0x04};
        result = pZYJ7Server->SendData(acqCmd);
        if(result<0)
        {
            cout << "send command 0X04 false!" << endl;
        }
        cout << "success to send command 0X04" << endl;
    }

    int frameCout = pZYJ7Server->RecvData(30);

    //!< send command to stop acquiring
    char stopCmd[4] = {0x02,0x00,0x00,0x02};
    result=pZYJ7Server->SendData(stopCmd);
    if(result<0)
    {
        cout << "send data false!" << endl;
    }
    else
    {
        cout << "received " << frameCout << " frames data!" << endl;
        cout << "\n" << endl;
    }

    //!< save data
    char szDirPath[] = "E:\\";
    pZYJ7Server->SavingRawData( szDirPath );

    delete pZYJ7Server;
    pZYJ7Server = NULL;
}

int monitorClientFrame::MakeDir( wxString* pstr4dataDir )
{
    //these for built a directory and a file
    wxDateTime DT = wxDateTime::Now();
    wxString str4date = DT.FormatISODate();
    wxString str4time = DT.FormatISOTime();
    str4time.Replace(":","-");
    wxString str4dir=_T("D:\\SwitchData\\")+str4date;
    *pstr4dataDir = str4dir + "\\" + str4time;
    wxFileName::SetCwd(_T("D:\\"));    //we should set the current working directory before build a sub-directory
    if(!wxFileName::DirExists(_T("SwitchData")))
    {
        if(!wxFileName::Mkdir(_T("SwitchData")))
        {
            wxMessageBox(_T("Fail to build directory SwitchData!"),_T("Error"));
        }
    }
    wxFileName::SetCwd(_T("D:\\SwitchData"));
    if(!wxFileName::DirExists(str4date))
    {
        if(!wxFileName::Mkdir(str4date))
        {
            wxMessageBox(_T("Fail to build directory by date!"),_T("Error"));
        }
    }
    wxFileName::SetCwd(str4dir);
    if(!wxFileName::Mkdir(str4time))
    {
        wxMessageBox(_T("Fail to build directory by date!"),_T("Error"));
    }
    wxFileName::SetCwd( *pstr4dataDir );

    return 0;
}

int monitorClientFrame::InsertDB()
{
    return 0;
}
