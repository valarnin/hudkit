/*
   * Original transparent window code by Mike - http://plan99.net/~mike/blog
     Dead link, but on Internet Archive most recently at:
     https://web.archive.org/web/20121127025149/http://mikehearn.wordpress.com/2006/03/26/gtk-windows-with-alpha-channels/
   * Modified by karlphillip for StackExchange -
     http://stackoverflow.com/questions/3908565/how-to-make-gtk-window-background-transparent
   * Re-worked for Gtk 3 by Louis Melahn, L.C., January 30, 2014.
   * Extended with WebKit and input shape kill by Antti Korpi <an@cyan.io>, on
     June 18, 2014.
   * Updated to WebKit 2 by Antti Korpi <an@cyan.io> on December 12, 2017.
 */

#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <webkit2/webkit2.h>

//@TODO: Make this a c++ app and do things in a better manner.
//Until then, here's some jank to make managing this a bit easier.
#include "config.c"
#include "gtk.c"
#include "webkit.c"
#include "hotkey.c"

void int_handler(int s) {
    handle_gtk_quit();
}

int main(int argc, char **argv) {
    gtk_init(&argc, &argv);
    if (argc < 2) {
        fprintf(stderr, "Expected 1 argument, got 0:\n");
        fprintf(stderr, "You should pass a json config file path.\n\n");
        fprintf(stderr, "For example,\n");
        fprintf(stderr, "    %s \"config.json\"\n\n", argv[0]);
        exit(1);
    }

    read_config(argv[1]);

    struct sigaction intHandler;
    intHandler.sa_handler = int_handler;
    sigemptyset(&intHandler.sa_mask);
    intHandler.sa_flags=0;
    sigaction(SIGINT, &intHandler, NULL);

    // Create the window, set basic properties
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_NONE);
    gtk_window_set_default_size(GTK_WINDOW(window), width, height);
    gtk_window_move(GTK_WINDOW(window), x, y);
    gtk_window_set_title(GTK_WINDOW(window), title);
    gtk_widget_add_events(GTK_WIDGET(window), GDK_CONFIGURE);
    g_signal_connect(G_OBJECT(window), "delete-event", G_CALLBACK(handle_gtk_quit), NULL);
    gtk_widget_set_app_paintable(window, TRUE);

    // Set up a callback to react to screen changes
    g_signal_connect(G_OBJECT(window), "screen-changed",
            G_CALLBACK(screen_changed), NULL);

    // Set up a callback to handle resize/move events
    g_signal_connect(G_OBJECT(window), "configure-event",
        G_CALLBACK(configure_callback), NULL);

    // Set up and add the WebKit web view widget
    // Disable caching
    WebKitWebContext *wk_context = webkit_web_context_get_default();
    webkit_web_context_set_cache_model(wk_context, WEBKIT_CACHE_MODEL_DOCUMENT_VIEWER);
    WebKitSecurityManager *security_manager = webkit_web_context_get_security_manager(wk_context);
    webkit_security_manager_register_uri_scheme_as_secure(security_manager, "ws");
    WebKitWebView *web_view = WEBKIT_WEB_VIEW(webkit_web_view_new_with_context(wk_context));
    // Enable browser console logging to stdout
    WebKitSettings *wk_settings = webkit_settings_new();
    webkit_settings_set_enable_write_console_messages_to_stdout(wk_settings, true);
    webkit_web_view_set_settings(web_view, wk_settings);
    // Make transparent
    GdkRGBA rgba = { .alpha = 0.0 };
    webkit_web_view_set_background_color(web_view, &rgba);
    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(web_view));

    // Load the specified URI
    webkit_web_view_load_uri(web_view, url);

    // Initialise the window and make it active.  We need this so it can
    // fullscreen to the correct size.
    screen_changed(window, NULL, NULL);

    gtk_widget_show_all(window);

    // Hide the window, so we can get our properties ready without the window
    // manager trying to mess with us.
    GdkWindow *gdk_window = gtk_widget_get_window(GTK_WIDGET(window));
    gdk_window_hide(GDK_WINDOW(gdk_window));

    // "Can't touch this!" - to the window manager
    //
    // The override-redirect flag prevents the window manager taking control of
    // anything, so the window remains in our control.  This should be enough
    // on its own.
    gdk_window_set_override_redirect(GDK_WINDOW(gdk_window), true);
    // But just to be careful, light up the flags like a Christmas tree, with
    // all the WM hints we can think of to try to convince whatever that's
    // reading them (probably a window manager) to keep this window on-top and
    // fullscreen but otherwise leave it alone.
    gtk_window_set_keep_above       (GTK_WINDOW(window), true);
    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(window), true);
    gtk_window_set_accept_focus     (GTK_WINDOW(window), false);
    gtk_window_set_decorated        (GTK_WINDOW(window), false);
    gtk_window_set_resizable        (GTK_WINDOW(window), false);

    // "Can't touch this!" - to the user
    //
    // Set the input shape (area where clicks are recognised) to a zero-width,
    // zero-height region a.k.a. nothing.  This makes clicks pass through the
    // window onto whatever's below.
    gdk_window_input_shape_combine_region(GDK_WINDOW(gdk_window),
            cairo_region_create(), 0,0);

    // Now it's safe to show the window again.  It should be click-through, and
    // the WM should ignore it.
    gdk_window_show(GDK_WINDOW(gdk_window));

    // XXX KLUDGE WARNING! XXX
    //
    // This sleep is necessary at least on my system with a proprietary Nvidia
    // driver.  Without this sleep, the transparent overlay window will usually
    // randomly not show.  No errors from it or the compositor, or anything;
    // there's no discernible reason why it doesn't work.  Except sometimes it
    // does, rarely, like once every 30 tries, with again no other difference
    // from what I can tell.  It's clearly a race condition of some sort.
    //
    // If I `watch xwininfo -tree -root`, I see the Hudkit window and
    // WebKitWebProcess windows get created, but I don't understand GTK
    // internals well enough to understand if it's right.  The same windows are
    // created both when successful and when not.
    //
    // Things I tried that didn't help:
    //  - adding calls to XSync all over the place,
    //  - doing `gdk_x11_grab_server` and `gdk_x11_ungrab_server` around
    //    various large and small bits of code, and
    //  - forcing full composition pipeline in `nvidia-settings`.
    register_hotkey();
    usleep(200000);
    
    gtk_main();
    return 0;
}
