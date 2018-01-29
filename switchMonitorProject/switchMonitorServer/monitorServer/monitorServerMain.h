/***************************************************************
 * Name:      monitorServerMain.h
 * Purpose:   Defines Application Frame
 * Author:    Alex (jthu4alex@163.com)
 * Created:   2017-03-09
 * Copyright: Alex (www.gzrobot.net)
 * License:
 **************************************************************/

#ifndef MONITORSERVERMAIN_H
#define MONITORSERVERMAIN_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/socket.h>
#include <wx/string.h>
#include <wx/thread.h>
#include "monitorServerApp.h"
#include "CNetController.h"
#include "CSqlController.h"
#include "CFaultAnalyzer.hpp"

class monitorServerFrame: public wxFrame
{
    public:
        monitorServerFrame(wxFrame *frame, const wxString& title);
        ~monitorServerFrame();
    private:
        enum
        {
            idMenuQuit = 1000,
            idMenuZD6,
            idMenuS700K,
            idMenuZYJ7,
            // id for sockets
            SERVER_ID = 100,
            SOCKET_ID,
            TIMER_ID
        };
        CNetController* m_pTcpServer;
        CSqlController* m_pDBCtrler;
        CFaultAnalyzer* m_pAnalyzer;
        wxSocketServer* m_server;
        wxSocketBase *m_sockUI;
        wxSocketBase *m_sockAcquirer;
        wxSocketBase *m_sockDTU;
        wxTimer m_timer;
        bool m_bACQIsRunning;
        bool m_busy;
        int m_numClients;
        SWITCH_TYPE m_emTypeofSwitch;

        void SendMSG2UI( const void *buffer, wxUint32 nbytes );
        void SendERR2DTU(const void *buffer, wxUint32 nbytes);
        bool HaveSetAllDefault( void );
        int CheckACQ( void );

        void OnClose(wxCloseEvent& event);
        void OnQuit(wxCommandEvent& event);
        void OnZD6(wxCommandEvent& event);
        void OnS700K(wxCommandEvent& event);
        void OnZYJ7(wxCommandEvent& event);
        void OnServerEvent(wxSocketEvent& event);
        void OnSocketEvent(wxSocketEvent& event);
        void OnAnalyzeFinishEvent(wxCommandEvent& event);
        void OnTimer(wxTimerEvent& event);
        DECLARE_EVENT_TABLE()
};

class CAnalyzeThread: public wxThread
{
public:
    CAnalyzeThread(monitorServerFrame *pFrame, CFaultAnalyzer *pAnalyzer, wxString strPath):
        m_pFrame(pFrame), m_pAnalyzer(pAnalyzer), m_strPath(strPath) {}
    virtual void *Entry();

private:
    monitorServerFrame* m_pFrame;
    CFaultAnalyzer* m_pAnalyzer;
    wxString m_strPath;
};
#define ANALYZE_FINISH_ID 144

DECLARE_APP(monitorServerApp);
#endif // MONITORSERVERMAIN_H
