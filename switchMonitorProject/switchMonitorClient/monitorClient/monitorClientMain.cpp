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

    SetMenuBar(mbar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar with some information about the used wxWidgets version
    CreateStatusBar(2);
    SetStatusText(_("Hello Code::Blocks user!"),0);
    SetStatusText(wxbuildinfo(short_f), 1);
#endif // wxUSE_STATUSBAR

}


monitorClientFrame::~monitorClientFrame()
{
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
    //CUdpServer *pZD6Server = new CUdpServer( "10.3.3.116", "10.3.3.106", 1024, typeofSwitch );

    wxString msg = _(" YES --- Acquiring directly\n NO --- Trigger");
    int answer = wxMessageBox(msg, _("choose the style of acquiring"), wxYES_NO | wxCENTER );

    //!< send command to continue asquiring
    char startCmd[4] = {0x03,0x00,0x00,0x03};
    int result = pZD6Server->SendData(startCmd);
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
        result = pZD6Server->SendData(acqCmd);
        if(result<0)
        {
            cout << "send command 0X04 false!" << endl;
        }
        cout << "success to send command 0X04" << endl;
    }

    int frameCout = pZD6Server->RecvData();

    //!< send command to stop acquiring
    char stopCmd[4] = {0x02,0x00,0x00,0x02};
    result=pZD6Server->SendData(stopCmd);
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
    pZD6Server->SavingRawData();

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

    int frameCout = pS700KServer->RecvData();

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
    pS700KServer->SavingRawData();

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

    int frameCout = pZYJ7Server->RecvData();

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
    pZYJ7Server->SavingRawData();

    delete pZYJ7Server;
    pZYJ7Server = NULL;
}
