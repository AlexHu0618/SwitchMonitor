/***************************************************************
 * Name:      monitorClientApp.h
 * Purpose:   Defines Application Class
 * Author:    Alex (jthu4alex@163.com)
 * Created:   2017-04-13
 * Copyright: Alex (www.gzrobot.net)
 * License:
 **************************************************************/

#ifndef MONITORCLIENTAPP_H
#define MONITORCLIENTAPP_H

#include <wx/app.h>

class monitorClientApp : public wxApp
{
    public:
        virtual bool OnInit();
        virtual void OnUnhandledException();
};

#endif // MONITORCLIENTAPP_H
