#include "hudkit_window.hh"
#include "include/cef_app.h"
#include <thread>
#include <gtk/gtk.h>

int handle_gtk_quit();

HudkitWindow::HudkitWindow()
{
    frame_time = std::chrono::nanoseconds(1000000 / fps);
}

HudkitWindow::~HudkitWindow()
{
}

void HudkitWindow::Initialize()
{
    // Create the window, set basic properties
    widgetWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtkWindow = GTK_WINDOW(widgetWindow);
    containerWindow = GTK_CONTAINER(widgetWindow);
    gtk_window_set_position(gtkWindow, GTK_WIN_POS_NONE);
    gtk_window_set_default_size(gtkWindow, width, height);
    gtk_window_move(gtkWindow, x, y);
    gtk_window_set_title(gtkWindow, "overlay");
    gtk_widget_add_events(widgetWindow, GDK_CONFIGURE);
    g_signal_connect(G_OBJECT(gtkWindow), "delete-event",
                     G_CALLBACK(handle_gtk_quit), NULL);
    gtk_widget_set_app_paintable(widgetWindow, TRUE);

    // Set up a callback to react to screen changes
    g_signal_connect(G_OBJECT(gtkWindow), "screen-changed",
                     G_CALLBACK(&HudkitWindow::__handle_screen_changed), this);

    // Set up a callback to handle resize/move events
    g_signal_connect(G_OBJECT(gtkWindow), "configure-event",
                     G_CALLBACK(&HudkitWindow::__handle_configure_event), this);

    drawArea.SetSize(width, height);

    drawArea.Register(containerWindow);

    HudkitWindow::__handle_screen_changed(widgetWindow, NULL, this);

    gtk_widget_show_all(widgetWindow);

    gdkWindow = gtk_widget_get_window(widgetWindow);
    gdk_window_hide(gdkWindow);

    // "Can't touch this!" - to the window manager
    //
    // The override-redirect flag prevents the window manager taking control of
    // anything, so the window remains in our control.  This should be enough
    // on its own.
    gdk_window_set_override_redirect(gdkWindow, true);
    // But just to be careful, light up the flags like a Christmas tree, with
    // all the WM hints we can think of to try to convince whatever that's
    // reading them (probably a window manager) to keep this window on-top and
    // fullscreen but otherwise leave it alone.
    gtk_window_set_keep_above(gtkWindow, true);
    gtk_window_set_skip_taskbar_hint(gtkWindow, true);
    gtk_window_set_accept_focus(gtkWindow, false);
    //gtk_window_set_decorated(gtkWindow, false);
    //gtk_window_set_resizable(gtkWindow, false);

    // "Can't touch this!" - to the user
    //
    // Set the input shape (area where clicks are recognised) to a zero-width,
    // zero-height region a.k.a. nothing.  This makes clicks pass through the
    // window onto whatever's below.
    gdk_window_input_shape_combine_region(gdkWindow,
                                          cairo_region_create(), 0, 0);
}

void HudkitWindow::Run()
{
    gdk_window_show(gdkWindow);
    gtk_main();
}

void HudkitWindow::Close()
{
}

void HudkitWindow::DrawWindow()
{
}

void HudkitWindow::Resize()
{
    fprintf(stdout, "Moving window to %i,%i at size %i,%i.\n", x, y, width, height);

    bool state = gtk_window_get_resizable(gtkWindow);
    if (!state)
    {
        gdk_window_hide(gdkWindow);
        gtk_window_set_resizable(gtkWindow, true);
        gdk_window_show(gdkWindow);
    }
    gtk_window_move(gtkWindow, x, y);
    gtk_window_resize(gtkWindow, width, height);
    if (!state)
    {
        gdk_window_hide(gdkWindow);
        gtk_window_set_resizable(gtkWindow, false);
        gdk_window_show(gdkWindow);
    }
}

void HudkitWindow::__handle_screen_changed(GtkWidget *widget, GdkScreen *old_screen, gpointer userdata)
{
    HudkitWindow *instance = (HudkitWindow *)userdata;
    // Die unless the screen supports compositing (alpha blending)
    GdkScreen *screen = gtk_widget_get_screen(widget);
    if (!gdk_screen_is_composited(screen))
    {
        fprintf(stderr, "Your screen does not support transparency.\n");
        fprintf(stderr, "Maybe your compositor isn't running?\n");
        exit(2);
    }

    gtk_widget_set_visual(widget, gdk_screen_get_rgba_visual(screen));

    instance->Resize();
}

void HudkitWindow::__handle_configure_event(GtkWindow *window, GdkEvent *event, gpointer data)
{
    HudkitWindow *instance = (HudkitWindow *)data;

    instance->x = event->configure.x;
    instance->y = event->configure.y;
    instance->width = event->configure.width;
    instance->height = event->configure.height;

    instance->Resize();
}

void HudkitWindow::EnableMoveResize()
{
    int hx, hy, hw, hh;

    gtk_window_get_position(gtkWindow, &hx, &hy);
    gtk_window_get_size(gtkWindow, &hw, &hh);

    gdk_window_hide(gdkWindow);
    gtk_window_set_keep_above(gtkWindow, true);
    gdk_window_set_override_redirect(gdkWindow, false);
    gtk_window_set_decorated(gtkWindow, true);
    gtk_window_set_accept_focus(gtkWindow, true);
    gtk_window_set_resizable(gtkWindow, true);
    gdk_window_show(gdkWindow);

    Resize();

    gdk_window_input_shape_combine_region(gdkWindow, NULL, 0, 0);

    //@TODO: Get decoration size as dx,dy so we can reposition window properly on toggle
}

void HudkitWindow::DisableMoveResize()
{
    int hx, hy, hw, hh;

    gtk_window_get_position(gtkWindow, &hx, &hy);
    gtk_window_get_size(gtkWindow, &hw, &hh);

    gdk_window_hide(gdkWindow);
    gtk_window_set_keep_above(gtkWindow, true);
    gdk_window_set_override_redirect(gdkWindow, true);
    gtk_window_set_decorated(gtkWindow, false);
    gtk_window_set_accept_focus(gtkWindow, false);
    gtk_window_set_resizable(gtkWindow, false);

    cairo_region_t *region = cairo_region_create();
    gdk_window_input_shape_combine_region(gdkWindow, region, 0, 0);
    cairo_region_destroy(region);
    gdk_window_show(gdkWindow);

    Resize();
}