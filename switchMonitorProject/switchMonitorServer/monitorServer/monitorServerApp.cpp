/***************************************************************
 * Name:      monitorServerApp.cpp
 * Purpose:   Code for Application Class
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

#include "monitorServerApp.h"
#include "monitorServerMain.h"

IMPLEMENT_APP(monitorServerApp);

bool monitorServerApp::OnInit()
{
    monitorServerFrame* frame = new monitorServerFrame(0L, _("wxWidgets Application Template"));
    frame->SetIcon(wxICON(aaaa)); // To Set App Icon
    frame->Show();
    
    return true;
}
