#ifndef __MAIN_HH_
#define __MAIN_HH_

#include "hudkit_window.hh"
#include "include/cef_base.h"
#include "include/cef_app.h"

class Hudkit : public CefApp, public CefBrowserProcessHandler
{
public:
    Hudkit(HudkitConfig &config);
    virtual ~Hudkit();

    // CefApp methods:
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler()
        OVERRIDE
    {
        return this;
    }
    virtual void OnBeforeCommandLineProcessing(const CefString &process_type, CefRefPtr<CefCommandLine> command_line)
        OVERRIDE
    {
        //Ignore the audio issues
        command_line->AppendSwitchWithValue("autoplay-policy", "no-user-gesture-required");

        //Allow insecure requests for websocket connection from https source
        command_line->AppendSwitch("allow-running-insecure-content");
        command_line->AppendSwitch("disable-web-security");
    }

    // CefBrowserProcessHandler methods:
    virtual void OnContextInitialized() OVERRIDE;
    void Run();

private:
    HudkitConfig &config;
    HudkitWindow hudkitWindow;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Hudkit);
};

#endif
