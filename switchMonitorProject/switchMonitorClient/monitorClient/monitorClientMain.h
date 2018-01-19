/***************************************************************
 * Name:      monitorClientMain.h
 * Purpose:   Defines Application Frame
 * Author:    Alex (jthu4alex@163.com)
 * Created:   2017-04-13
 * Copyright: Alex (www.gzrobot.net)
 * License:
 **************************************************************/

#ifndef MONITORCLIENTMAIN_H
#define MONITORCLIENTMAIN_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/socket.h>
#include "monitorClientApp.h"
#include "CNetController.h"
#include "CSqlController.h"
#include "CUdpServer.h"

enum ACQUIRE_MODE {TRIGGER, DIRECT};

/** \brief 本类主要负责采集客户端的数据前处理和保存，并与分析服务端通信
 */
class monitorClientFrame: public wxFrame
{
    public:
        monitorClientFrame(wxFrame *frame, const wxString& title);
        ~monitorClientFrame();

    private:
        enum
        {
            idMenuQuit = 1000,
            idMenuZD6,
            idMenuS700K,
            idMenuZYJ7,
            idMenuDefault,
            // id for sockets
            SOCKET_ID = 100

        };
        wxMenuItem* m_MenuItemDefault;
        CNetController* m_pTcpClient;
        CSqlController* m_pDBCtrler;
        CUdpServer *m_pUDPServer;
        int m_nAcqCounter;
        bool m_bIsConnUDP;
        SWITCH_TYPE m_emTypeofSwitch;

        void OnClose(wxCloseEvent& event);
        void OnQuit(wxCommandEvent& event);
        void OnZD6(wxCommandEvent& event);
        void OnS700K(wxCommandEvent& event);
        void OnZYJ7(wxCommandEvent& event);
        void OnSocketEvent(wxSocketEvent& event);
        DECLARE_EVENT_TABLE()

        int InitializeAll( void );
        int MakeDir( wxString* pstr4dataDir );
        int SaveData( ACQUIRE_MODE emAcqMode );
        void AcquireLoop( void );
};


#endif // MONITORCLIENTMAIN_H
