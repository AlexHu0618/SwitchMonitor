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
            SOCKET_ID
        };
        CNetController* m_pTcpServer;
        CSqlController* m_pDBCtrler;
        wxSocketServer *m_server;
        bool m_busy;
        int m_numClients;
        bool m_bIsDefault;

        int Diagnosing( SWITCH_TYPE typeofSwitch, wxString strPath );
        void ZD6Analyzing( wxString strPath );

        void OnClose(wxCloseEvent& event);
        void OnQuit(wxCommandEvent& event);
        void OnZD6(wxCommandEvent& event);
        void OnS700K(wxCommandEvent& event);
        void OnZYJ7(wxCommandEvent& event);
        void OnServerEvent(wxSocketEvent& event);
        void OnSocketEvent(wxSocketEvent& event);
        DECLARE_EVENT_TABLE()
};


#endif // MONITORSERVERMAIN_H
