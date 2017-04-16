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

#include "monitorClientApp.h"

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
            idMenuZYJ7
        };
        void OnClose(wxCloseEvent& event);
        void OnQuit(wxCommandEvent& event);
        void OnZD6(wxCommandEvent& event);
        void OnS700K(wxCommandEvent& event);
        void OnZYJ7(wxCommandEvent& event);
        DECLARE_EVENT_TABLE()
};


#endif // MONITORCLIENTMAIN_H
