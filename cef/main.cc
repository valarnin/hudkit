#include "hudkit.hh"

#include <X11/Xlib.h>

namespace
{

int XErrorHandlerImpl(Display *display, XErrorEvent *event)
{
    LOG(WARNING) << "X error received: "
                 << "type " << event->type << ", "
                 << "serial " << event->serial << ", "
                 << "error_code " << static_cast<int>(event->error_code) << ", "
                 << "request_code " << static_cast<int>(event->request_code)
                 << ", "
                 << "minor_code " << static_cast<int>(event->minor_code);
    return 0;
}

int XIOErrorHandlerImpl(Display *display)
{
    return 0;
}

} // namespace

int main(int argc, char *argv[])
{
    CefMainArgs main_args(argc, argv);
    int exit_code = CefExecuteProcess(main_args, nullptr, nullptr);
    if (exit_code >= 0)
    {
        return exit_code;
    }
    XSetErrorHandler(XErrorHandlerImpl);
    XSetIOErrorHandler(XIOErrorHandlerImpl);

    CefSettings settings;

    settings.no_sandbox = true;
    settings.windowless_rendering_enabled = true;

    CefRefPtr<Hudkit> app(new Hudkit);

    CefInitialize(main_args, settings, app.get(), nullptr);

    app.get()->Run();

    CefShutdown();

    return 0;
}