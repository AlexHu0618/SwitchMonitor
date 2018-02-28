/***************************************************************
 * Name:      SMUIMain.h
 * Purpose:   Defines Application Frame
 * Author:    Alex (jthu4alex@163.com)
 * Created:   2017-06-01
 * Copyright: Alex (www.gzrobot.net)
 * License:
 **************************************************************/

#ifndef SMUIMAIN_H
#define SMUIMAIN_H

//(*Headers(SMUIFrame)
#include <wx/notebook.h>
#include <wx/sizer.h>
#include <wx/menu.h>
#include <wx/tglbtn.h>
#include <wx/panel.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/statbmp.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/statusbr.h>
//*)
#include "wxPLplotwindow.h"

class SMUIFrame: public wxFrame
{
    public:

        SMUIFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~SMUIFrame();

    private:

        //(*Handlers(SMUIFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnNotebook1PageChanged(wxNotebookEvent& event);
        //*)

        //(*Identifiers(SMUIFrame)
        static const long ID_STATICBITMAP1;
        static const long ID_PANEL3;
        static const long ID_PANEL4;
        static const long ID_PANEL5;
        static const long ID_PANEL6;
        static const long ID_PANEL7;
        static const long ID_PANEL8;
        static const long ID_PANEL9;
        static const long ID_NOTEBOOK1;
        static const long ID_PANEL1;
        static const long ID_PANEL2;
        static const long ID_RICHTEXTCTRL1;
        static const long ID_BUTTON1;
        static const long ID_TOGGLEBUTTON1;
        static const long ID_BUTTON2;
        static const long ID_MENUITEM1;
        static const long idMenuAbout;
        static const long ID_STATUSBAR1;
        //*)

        //(*Declarations(SMUIFrame)
        wxPanel* Panel5;
        wxNotebook* Notebook1;
        wxPanel* Panel4;
        wxRichTextCtrl* RichTextCtrl1;
        wxButton* Button1;
        wxStaticBitmap* StaticBitmap1;
        wxPanel* Panel9;
        wxPanel* Panel8;
        wxToggleButton* ToggleButton1;
        wxPanel* Panel1;
        wxButton* Button2;
        wxPanel* Panel6;
        wxPanel* Panel3;
        wxPanel* Panel7;
        wxStatusBar* StatusBar1;
        wxPanel* Panel2;
        //*)

        template< class WXWINDOW >
        void Plot( wxPLplotwindow<WXWINDOW> *plotwindow );

        DECLARE_EVENT_TABLE()
};

#endif // SMUIMAIN_H
