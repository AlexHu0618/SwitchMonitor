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

    m_pTcpServer = new CNetController( "localhost", 1024, server );
    m_pTcpServer->Initial();

    m_pDBCtrler = new CSqlController( "10.3.3.144", 3306, "yfzx", "yfzx3305" );
    m_pDBCtrler->Initial("switchmonitordb", "tab4alldata" );
}


monitorServerFrame::~monitorServerFrame()
{
    delete m_pTcpServer;
    m_pTcpServer = NULL;
    delete m_pDBCtrler;
    m_pDBCtrler = NULL;
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
    SWITCH_TYPE typeofSwitch = ZD6;
    int nResult = 0;
    while( true )
    {
        nResult = Diagnosing( typeofSwitch );   //!<  diagnose and save data and result
        if( nResult != 0 )
        {
            break;
        }
    }
}

void monitorServerFrame::OnS700K(wxCommandEvent &event)
{
    SWITCH_TYPE typeofSwitch = S700K;
    int nResult = 0;
    while( true )
    {
        nResult = Diagnosing( typeofSwitch );   //!<  diagnose and save data and result
        if( nResult != 0 )
        {
            break;
        }
    }
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

int monitorServerFrame::Diagnosing( SWITCH_TYPE typeofSwitch )
{
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_INTENSITY | FOREGROUND_GREEN );
    cout << "START A NEW TASK" << endl;
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN |FOREGROUND_BLUE );

    //!< receive block and wait for TCP
    char szRecvBuf[1024]={0};
    m_pTcpServer->Recv( szRecvBuf, 1024 );
    cout << szRecvBuf << endl;
    string strDataDirPath = szRecvBuf;

    if( szRecvBuf[0] == 'Q' )
    {
        return -1;
    }
    bool bIsDefault = false;
    if( strDataDirPath.substr(strDataDirPath.length()-8) == "_default" )
    {
        bIsDefault = true;
        strDataDirPath.erase( strDataDirPath.length()-8, 8 );
    }

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

    printf("ZD6 fault confidences for provided data:\n\n");
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
    if( bIsDefault )
    {
        faultAnalyzer.SetBaseData( bIsDefault );
        int result = faultAnalyzer.SaveRealData( parrdTransformRatio );
        if (result != 0)
        {
            cout << "save real data fail!" << endl;
            return 1;
        }
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
