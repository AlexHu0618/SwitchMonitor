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
#include "CFaultAnalyzer.hpp"
#include <wx/dirdlg.h>

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
}


monitorServerFrame::~monitorServerFrame()
{
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
    SWITCH_TYPE typeofSwitch = ZD6;
    CFaultAnalyzer faultAnalyzer( std::string(strDataDirPath.mb_str()), typeofSwitch );
    double *parrdScore = faultAnalyzer.GetScore();

    printf("ZD6 fault confidences for provided data:\n\n");
	printf("Actuating fault: %.2f %%\n", parrdScore[0]);
	printf("Engage difficult: %.2f %%\n", parrdScore[1]);
	printf("Indicating fault: %.2f %%\n", parrdScore[2]);
	printf("Jam: %.2f %%\n", parrdScore[3]);
	printf("Motor fault: %.2f %%\n", parrdScore[4]);
	printf("Movement resistance: %.2f %%\n", parrdScore[5]);
	printf("Power fault: %.2f %%\n", parrdScore[6]);
	printf("Unlock difficult: %.2f %%\n", parrdScore[7]);
}

void monitorServerFrame::OnS700K(wxCommandEvent &event)
{

}

void monitorServerFrame::OnZYJ7(wxCommandEvent &event)
{

}
