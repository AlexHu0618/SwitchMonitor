/***************************************************************
 * Name:      SMUIApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Alex (jthu4alex@163.com)
 * Created:   2017-06-01
 * Copyright: Alex (www.gzrobot.net)
 * License:
 **************************************************************/

#include "SMUIApp.h"

//(*AppHeaders
#include "SMUIMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(SMUIApp);

bool SMUIApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	SMUIFrame* Frame = new SMUIFrame(0);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
