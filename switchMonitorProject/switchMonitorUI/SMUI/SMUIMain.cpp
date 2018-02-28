/***************************************************************
 * Name:      SMUIMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Alex (jthu4alex@163.com)
 * Created:   2017-06-01
 * Copyright: Alex (www.gzrobot.net)
 * License:
 **************************************************************/

#include "SMUIMain.h"
#include <wx/msgdlg.h>

//(*InternalHeaders(SMUIFrame)
#include <wx/bitmap.h>
#include <wx/intl.h>
#include <wx/image.h>
#include <wx/string.h>
//*)
#include <cmath>

#define MAX( a, b )    ( ( a ) < ( b ) ? ( b ) : ( a ) )
#define MIN( a, b )    ( ( a ) < ( b ) ? ( a ) : ( b ) )

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

//(*IdInit(SMUIFrame)
const long SMUIFrame::ID_STATICBITMAP1 = wxNewId();
const long SMUIFrame::ID_PANEL3 = wxNewId();
const long SMUIFrame::ID_PANEL4 = wxNewId();
const long SMUIFrame::ID_PANEL5 = wxNewId();
const long SMUIFrame::ID_PANEL6 = wxNewId();
const long SMUIFrame::ID_PANEL7 = wxNewId();
const long SMUIFrame::ID_PANEL8 = wxNewId();
const long SMUIFrame::ID_PANEL9 = wxNewId();
const long SMUIFrame::ID_NOTEBOOK1 = wxNewId();
const long SMUIFrame::ID_PANEL1 = wxNewId();
const long SMUIFrame::ID_PANEL2 = wxNewId();
const long SMUIFrame::ID_RICHTEXTCTRL1 = wxNewId();
const long SMUIFrame::ID_BUTTON1 = wxNewId();
const long SMUIFrame::ID_TOGGLEBUTTON1 = wxNewId();
const long SMUIFrame::ID_BUTTON2 = wxNewId();
const long SMUIFrame::ID_MENUITEM1 = wxNewId();
const long SMUIFrame::idMenuAbout = wxNewId();
const long SMUIFrame::ID_STATUSBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(SMUIFrame,wxFrame)
    //(*EventTable(SMUIFrame)
    //*)
END_EVENT_TABLE()

SMUIFrame::SMUIFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(SMUIFrame)
    wxBoxSizer* BoxSizer4;
    wxBoxSizer* BoxSizer6;
    wxBoxSizer* BoxSizerTotal;
    wxBoxSizer* BoxSizer5;
    wxMenuItem* MenuItem2;
    wxMenuItem* MenuItem1;
    wxBoxSizer* BoxSizer2;
    wxMenu* Menu1;
    wxBoxSizer* BoxSizer1;
    wxMenuBar* MenuBar1;
    wxStaticBoxSizer* StaticBoxSizer1;
    wxFlexGridSizer* FlexGridSizer1;
    wxBoxSizer* BoxSizer3;
    wxMenu* Menu2;

    Create(parent, wxID_ANY, _("Quit"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    BoxSizerTotal = new wxBoxSizer(wxVERTICAL);
    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    StaticBitmap1 = new wxStaticBitmap(this, ID_STATICBITMAP1, wxBitmap(wxImage(_T("E:\\Project\\GZMetro\\Switch\\softwareDesign\\switchMonitorProject\\switchMonitorUI\\SMUI\\pic\\title.png")).Rescale(wxSize(800,60).GetWidth(),wxSize(800,60).GetHeight())), wxDefaultPosition, wxSize(800,60), wxSIMPLE_BORDER, _T("ID_STATICBITMAP1"));
    BoxSizer1->Add(StaticBitmap1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
    BoxSizerTotal->Add(BoxSizer1, 0, wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    Notebook1 = new wxNotebook(this, ID_NOTEBOOK1, wxDefaultPosition, wxDefaultSize, 0, _T("ID_NOTEBOOK1"));
    Panel3 = new wxPanel(Notebook1, ID_PANEL3, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL3"));
    Panel4 = new wxPanel(Notebook1, ID_PANEL4, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL4"));
    Panel5 = new wxPanel(Notebook1, ID_PANEL5, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL5"));
    Panel6 = new wxPanel(Notebook1, ID_PANEL6, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL6"));
    Panel7 = new wxPanel(Notebook1, ID_PANEL7, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL7"));
    Panel8 = new wxPanel(Notebook1, ID_PANEL8, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL8"));
    Panel9 = new wxPanel(Notebook1, ID_PANEL9, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL9"));
    Notebook1->AddPage(Panel3, _("I1"), false);
    Notebook1->AddPage(Panel4, _("I2"), false);
    Notebook1->AddPage(Panel5, _("I3"), false);
    Notebook1->AddPage(Panel6, _("U1"), false);
    Notebook1->AddPage(Panel7, _("U2"), false);
    Notebook1->AddPage(Panel8, _("U3"), false);
    Notebook1->AddPage(Panel9, _("P"), false);
    BoxSizer2->Add(Notebook1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizerTotal->Add(BoxSizer2, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    Panel1 = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxSize(250,157), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    BoxSizer4->Add(Panel1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer3->Add(BoxSizer4, 2, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    Panel2 = new wxPanel(this, ID_PANEL2, wxDefaultPosition, wxSize(188,109), wxTAB_TRAVERSAL, _T("ID_PANEL2"));
    BoxSizer5->Add(Panel2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer3->Add(BoxSizer5, 2, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
    FlexGridSizer1->AddGrowableCol(0);
    FlexGridSizer1->AddGrowableRow(0);
    StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Label"));
    RichTextCtrl1 = new wxRichTextCtrl(this, ID_RICHTEXTCTRL1, _("Text\nText\nText\nText\nText\nText\nText\nText\nText\nText\nText"), wxDefaultPosition, wxDefaultSize, wxRE_MULTILINE, wxDefaultValidator, _T("ID_RICHTEXTCTRL1"));
    wxRichTextAttr rchtxtAttr_1;
    rchtxtAttr_1.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_ALIGN_LEFT);
    RichTextCtrl1->Disable();
    StaticBoxSizer1->Add(RichTextCtrl1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(StaticBoxSizer1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button1 = new wxButton(this, ID_BUTTON1, _("Query"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    FlexGridSizer1->Add(Button1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ToggleButton1 = new wxToggleButton(this, ID_TOGGLEBUTTON1, _("Start"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TOGGLEBUTTON1"));
    FlexGridSizer1->Add(ToggleButton1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button2 = new wxButton(this, ID_BUTTON2, _("Quit"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
    FlexGridSizer1->Add(Button2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer6->Add(FlexGridSizer1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer3->Add(BoxSizer6, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizerTotal->Add(BoxSizer3, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(BoxSizerTotal);
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItem1 = new wxMenuItem(Menu1, ID_MENUITEM1, _("Quit\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar1->Append(Menu1, _("&File"));
    Menu2 = new wxMenu();
    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("About\tF1"), _("Show info about this application"), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar1->Append(Menu2, _("Help"));
    SetMenuBar(MenuBar1);
    StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[1] = { -1 };
    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
    StatusBar1->SetFieldsCount(1,__wxStatusBarWidths_1);
    StatusBar1->SetStatusStyles(1,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar1);
    BoxSizerTotal->Fit(this);
    BoxSizerTotal->SetSizeHints(this);

    Connect(ID_NOTEBOOK1,wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,(wxObjectEventFunction)&SMUIFrame::OnNotebook1PageChanged);
    Connect(ID_MENUITEM1,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&SMUIFrame::OnQuit);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&SMUIFrame::OnAbout);
    //*)
}

SMUIFrame::~SMUIFrame()
{
    //(*Destroy(SMUIFrame)
    //*)
}

void SMUIFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void SMUIFrame::OnAbout(wxCommandEvent& event)
{

}

void SMUIFrame::OnNotebook1PageChanged(wxNotebookEvent& event)
{
}







///***************************************************************
// * Name:      SMUIMain.cpp
// * Purpose:   Code for Application Frame
// * Author:    Alex (jthu4alex@163.com)
// * Created:   2017-06-01
// * Copyright: Alex (www.gzrobot.net)
// * License:
// **************************************************************/
//
//#include "SMUIMain.h"
//#include <wx/msgdlg.h>
//
////(*InternalHeaders(SMUIFrame)
//#include <wx/bitmap.h>
//#include <wx/intl.h>
//#include <wx/image.h>
//#include <wx/string.h>
////*)
//#include <cmath>
//
//#define MAX( a, b )    ( ( a ) < ( b ) ? ( b ) : ( a ) )
//#define MIN( a, b )    ( ( a ) < ( b ) ? ( a ) : ( b ) )
//
////helper functions
//enum wxbuildinfoformat {
//    short_f, long_f };
//
//wxString wxbuildinfo(wxbuildinfoformat format)
//{
//    wxString wxbuild(wxVERSION_STRING);
//
//    if (format == long_f )
//    {
//#if defined(__WXMSW__)
//        wxbuild << _T("-Windows");
//#elif defined(__UNIX__)
//        wxbuild << _T("-Linux");
//#endif
//
//#if wxUSE_UNICODE
//        wxbuild << _T("-Unicode build");
//#else
//        wxbuild << _T("-ANSI build");
//#endif // wxUSE_UNICODE
//    }
//
//    return wxbuild;
//}
//
////(*IdInit(SMUIFrame)
//const long SMUIFrame::ID_STATICBITMAP1 = wxNewId();
//const long SMUIFrame::ID_PANEL3 = wxNewId();
//const long SMUIFrame::ID_PANEL4 = wxNewId();
//const long SMUIFrame::ID_PANEL5 = wxNewId();
//const long SMUIFrame::ID_PANEL6 = wxNewId();
//const long SMUIFrame::ID_PANEL7 = wxNewId();
//const long SMUIFrame::ID_PANEL8 = wxNewId();
//const long SMUIFrame::ID_PANEL9 = wxNewId();
//const long SMUIFrame::ID_NOTEBOOK1 = wxNewId();
//const long SMUIFrame::ID_PANEL1 = wxNewId();
//const long SMUIFrame::ID_PANEL2 = wxNewId();
//const long SMUIFrame::ID_RICHTEXTCTRL1 = wxNewId();
//const long SMUIFrame::ID_BUTTON1 = wxNewId();
//const long SMUIFrame::ID_TOGGLEBUTTON1 = wxNewId();
//const long SMUIFrame::ID_BUTTON2 = wxNewId();
//const long SMUIFrame::ID_MENUITEM1 = wxNewId();
//const long SMUIFrame::idMenuAbout = wxNewId();
//const long SMUIFrame::ID_STATUSBAR1 = wxNewId();
////*)
//
//BEGIN_EVENT_TABLE(SMUIFrame,wxFrame)
//    //(*EventTable(SMUIFrame)
//    //*)
//END_EVENT_TABLE()
//
//SMUIFrame::SMUIFrame(wxWindow* parent,wxWindowID id)
//{
//    //(*Initialize(SMUIFrame)
//    wxBoxSizer* BoxSizer4;
//    wxBoxSizer* BoxSizer6;
//    wxBoxSizer* BoxSizerTotal;
//    wxBoxSizer* BoxSizer5;
//    wxMenuItem* MenuItem2;
//    wxMenuItem* MenuItem1;
//    wxBoxSizer* BoxSizer2;
//    wxMenu* Menu1;
//    wxBoxSizer* BoxSizer1;
//    wxMenuBar* MenuBar1;
//    wxStaticBoxSizer* StaticBoxSizer1;
//    wxFlexGridSizer* FlexGridSizer1;
//    wxBoxSizer* BoxSizer3;
//    wxMenu* Menu2;
//
//    Create(parent, wxID_ANY, _("Quit"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
//    BoxSizerTotal = new wxBoxSizer(wxVERTICAL);
//    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
//    StaticBitmap1 = new wxStaticBitmap(this, ID_STATICBITMAP1, wxBitmap(wxImage(_T("E:\\Project\\GZMetro\\Switch\\softwareDesign\\switchMonitorProject\\switchMonitorUI\\SMUI\\pic\\title.png")).Rescale(wxSize(800,60).GetWidth(),wxSize(800,60).GetHeight())), wxDefaultPosition, wxSize(800,60), wxSIMPLE_BORDER, _T("ID_STATICBITMAP1"));
//    BoxSizer1->Add(StaticBitmap1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
//    BoxSizerTotal->Add(BoxSizer1, 0, wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
//    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
//    Notebook1 = new wxNotebook(this, ID_NOTEBOOK1, wxDefaultPosition, wxDefaultSize, 0, _T("ID_NOTEBOOK1"));
//    Panel3 = new wxPanel(Notebook1, ID_PANEL3, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL3"));
//    Panel4 = new wxPanel(Notebook1, ID_PANEL4, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL4"));
//    Panel5 = new wxPanel(Notebook1, ID_PANEL5, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL5"));
//    Panel6 = new wxPanel(Notebook1, ID_PANEL6, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL6"));
//    Panel7 = new wxPanel(Notebook1, ID_PANEL7, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL7"));
//    Panel8 = new wxPanel(Notebook1, ID_PANEL8, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL8"));
//    Panel9 = new wxPanel(Notebook1, ID_PANEL9, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL9"));
//    Notebook1->AddPage(Panel3, _("I1"), false);
//    Notebook1->AddPage(Panel4, _("I2"), false);
//    Notebook1->AddPage(Panel5, _("I3"), false);
//    Notebook1->AddPage(Panel6, _("U1"), false);
//    Notebook1->AddPage(Panel7, _("U2"), false);
//    Notebook1->AddPage(Panel8, _("U3"), false);
//    Notebook1->AddPage(Panel9, _("P"), false);
//    BoxSizer2->Add(Notebook1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
//    BoxSizerTotal->Add(BoxSizer2, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
//    BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
//    BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
//    Panel1 = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxSize(250,157), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
//    BoxSizer4->Add(Panel1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
//    BoxSizer3->Add(BoxSizer4, 2, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
//    BoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
//    Panel2 = new wxPanel(this, ID_PANEL2, wxDefaultPosition, wxSize(188,109), wxTAB_TRAVERSAL, _T("ID_PANEL2"));
//    BoxSizer5->Add(Panel2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
//    BoxSizer3->Add(BoxSizer5, 2, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
//    BoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
//    FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
//    FlexGridSizer1->AddGrowableCol(0);
//    FlexGridSizer1->AddGrowableRow(0);
//    StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Label"));
//    RichTextCtrl1 = new wxRichTextCtrl(this, ID_RICHTEXTCTRL1, _("Text\nText\nText\nText\nText\nText\nText\nText\nText\nText\nText"), wxDefaultPosition, wxDefaultSize, wxRE_MULTILINE, wxDefaultValidator, _T("ID_RICHTEXTCTRL1"));
//    wxRichTextAttr rchtxtAttr_1;
//    rchtxtAttr_1.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_ALIGN_LEFT);
//    RichTextCtrl1->Disable();
//    StaticBoxSizer1->Add(RichTextCtrl1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
//    FlexGridSizer1->Add(StaticBoxSizer1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
//    Button1 = new wxButton(this, ID_BUTTON1, _("Query"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
//    FlexGridSizer1->Add(Button1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
//    ToggleButton1 = new wxToggleButton(this, ID_TOGGLEBUTTON1, _("Start"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TOGGLEBUTTON1"));
//    FlexGridSizer1->Add(ToggleButton1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
//    Button2 = new wxButton(this, ID_BUTTON2, _("Quit"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
//    FlexGridSizer1->Add(Button2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
//    BoxSizer6->Add(FlexGridSizer1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
//    BoxSizer3->Add(BoxSizer6, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
//    BoxSizerTotal->Add(BoxSizer3, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
//    SetSizer(BoxSizerTotal);
//    MenuBar1 = new wxMenuBar();
//    Menu1 = new wxMenu();
//    MenuItem1 = new wxMenuItem(Menu1, ID_MENUITEM1, _("Quit\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
//    Menu1->Append(MenuItem1);
//    MenuBar1->Append(Menu1, _("&File"));
//    Menu2 = new wxMenu();
//    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("About\tF1"), _("Show info about this application"), wxITEM_NORMAL);
//    Menu2->Append(MenuItem2);
//    MenuBar1->Append(Menu2, _("Help"));
//    SetMenuBar(MenuBar1);
//    StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
//    int __wxStatusBarWidths_1[1] = { -1 };
//    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
//    StatusBar1->SetFieldsCount(1,__wxStatusBarWidths_1);
//    StatusBar1->SetStatusStyles(1,__wxStatusBarStyles_1);
//    SetStatusBar(StatusBar1);
//    BoxSizerTotal->Fit(this);
//    BoxSizerTotal->SetSizeHints(this);
//
//    Connect(ID_NOTEBOOK1,wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,(wxObjectEventFunction)&SMUIFrame::OnNotebook1PageChanged);
//    Connect(ID_MENUITEM1,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&SMUIFrame::OnQuit);
//    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&SMUIFrame::OnAbout);
//    //*)
//}
//
//SMUIFrame::~SMUIFrame()
//{
//    //(*Destroy(SMUIFrame)
//    //*)
//}
//
//void SMUIFrame::OnQuit(wxCommandEvent& event)
//{
//    Close();
//}
//
//void SMUIFrame::OnAbout(wxCommandEvent& event)
//{
//    wxPLplotwindow<wxPanel> *frame = new wxPLplotwindow<wxPanel>();
//    Plot( frame );
//    delete frame;
//}
//
//void SMUIFrame::OnNotebook1PageChanged(wxNotebookEvent& event)
//{
//}
//
//template< class WXWINDOW >
//void SMUIFrame::Plot( wxPLplotwindow<WXWINDOW> *plotwindow )
//{
//    wxPLplotstream* pls = plotwindow->GetStream();
//
//    const size_t  np = 500;
//    PLFLT         x[np], y[np];
//    PLFLT         xmin, xmax;
//    PLFLT         ymin = 1e30, ymax = 1e-30;
//
//    xmin = -2.0;
//    xmax = 10.0;
//    for ( size_t i = 0; i < np; i++ )
//    {
//        x[i] = ( xmax - xmin ) * i / np + xmin;
//        y[i] = 1.0;
//        if ( x[i] != 0.0 )
//            y[i] = sin( x[i] ) / x[i];
//        ymin = MIN( ymin, y[i] );
//        ymax = MAX( ymax, y[i] );
//    }
//
//    pls->adv( 0 );
//    pls->col0( 1 );
//    pls->env( xmin, xmax, ymin, ymax, 0, 0 );
//    pls->col0( 2 );
//    pls->lab( "x", "y", "sin(x)/x" );
//
//    pls->col0( 3 );
//    pls->width( 2 );
//    pls->line( np, x, y );
//
//    plotwindow->RenewPlot();
//}

