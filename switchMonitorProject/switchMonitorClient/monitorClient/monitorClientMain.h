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
            idMenuAbout
        };
        void OnClose(wxCloseEvent& event);
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        DECLARE_EVENT_TABLE()
};


#endif // MONITORCLIENTMAIN_H
