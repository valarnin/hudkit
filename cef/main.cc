#include "hudkit.hh"
#include "hudkit_config.hh"

#include <gtk/gtk.h>
#include <X11/Xlib.h>
#include <csignal>
#include <iostream>

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

int handle_gtk_quit()
{
    //write_config();
    gtk_main_quit();
    //unregister_hotkey();
    return 0;
}

void int_handler(int s)
{
    handle_gtk_quit();
    exit(s);
}

int main(int argc, char *argv[])
{
    CefMainArgs main_args(argc, argv);
    int exit_code = CefExecuteProcess(main_args, nullptr, nullptr);
    if (exit_code >= 0)
    {
        return exit_code;
    }

    if (argc < 2)
    {
        std::cerr << "Expected 1 argument, got 0:" << std::endl
                  << "You should pass a json config file path." << std::endl
                  << std::endl
                  << "For example," << std::endl
                  << "    " << argv[0] << " \"config.json\"" << std::endl
                  << std::endl;
        exit(1);
    }

    std::string file(argv[1]);

    HudkitConfig config(file);

    signal(SIGINT, int_handler);
    gtk_init(&argc, &argv);

    XSetErrorHandler(XErrorHandlerImpl);
    XSetIOErrorHandler(XIOErrorHandlerImpl);

    CefSettings settings;

    //settings.multi_threaded_message_loop = true;
    settings.no_sandbox = true;
    settings.windowless_rendering_enabled = true;

    Hudkit *hudkit = new Hudkit(config);

    CefRefPtr<Hudkit> app(hudkit);

    CefInitialize(main_args, settings, app.get(), nullptr);

    usleep(200000);

    app.get()->Run();

    CefShutdown();

    return 0;
}