#include "hudkit.hh"

#include <gtk/gtk.h>
#include <X11/Xlib.h>
#include <csignal>

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

int handle_gtk_quit() {
    //write_config();
    gtk_main_quit();
    //unregister_hotkey();
    return 0;
}

void int_handler(int s) {
    handle_gtk_quit();
    exit(s);
}

int main(int argc, char *argv[])
{
    signal(SIGINT, int_handler);
    gtk_init(&argc, &argv);

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

    printf("Pre-Init\n");
    CefInitialize(main_args, settings, app.get(), nullptr);
    printf("Post-Init\n");

    CefShutdown();

    return 0;
}