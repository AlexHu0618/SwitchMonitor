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

#include "monitorServerApp.h"

class monitorServerFrame: public wxFrame
{
    public:
        monitorServerFrame(wxFrame *frame, const wxString& title);
        ~monitorServerFrame();
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


#endif // MONITORSERVERMAIN_H
