#include "hudkit_window.hh"
#include "include/cef_app.h"
#include <thread>
#include <gtk/gtk.h>
#include <keybinder-3.0/keybinder.h>

int handle_gtk_quit();

HudkitWindow::HudkitWindow(HudkitConfig &config) : config(config)
{
    frame_time = std::chrono::nanoseconds(1000000000 / fps);
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
    gtk_window_set_gravity(gtkWindow, GDK_GRAVITY_STATIC);
    gtk_window_set_position(gtkWindow, GTK_WIN_POS_NONE);
    gtk_window_set_default_size(gtkWindow, config.GetWidth(), config.GetHeight());
    gtk_window_move(gtkWindow, config.GetX(), config.GetY());
    gtk_window_set_title(gtkWindow, "overlay");
    gtk_widget_add_events(widgetWindow, GDK_CONFIGURE);

    gtk_widget_set_app_paintable(widgetWindow, TRUE);

    drawArea.SetSize(config.GetWidth(), config.GetHeight());

    drawArea.Register(containerWindow);

    HudkitWindow::__handle_screen_changed(widgetWindow, NULL, this);

    gtk_widget_show_all(widgetWindow);

    gdkWindow = gtk_widget_get_window(widgetWindow);

    gdk_window_hide(gdkWindow);

    if (false) //config.locked?
    {
        // Tell the WM that we'll handle decorations and such
        gdk_window_set_override_redirect(gdkWindow, true);

        // Set individual hints in case the above override redirect didn't work
        gtk_window_set_keep_above(gtkWindow, true);
        gtk_window_set_skip_taskbar_hint(gtkWindow, true);
        gtk_window_set_accept_focus(gtkWindow, false);
        gtk_window_set_decorated(gtkWindow, false);
        gtk_window_set_resizable(gtkWindow, false);
    }

    // Disallow user inputs from interacting with the window
    gdk_window_input_shape_combine_region(gdkWindow,
                                          cairo_region_create(), 0, 0);

    keybinder_init();
    keybinder_bind(config.GetHotkey().c_str(), &HudkitWindow::__handle_hotkey, this);
}

void HudkitWindow::Run()
{
    gdk_window_show(gdkWindow);

    gtk_main_iteration_do(true);

    g_signal_connect(G_OBJECT(gtkWindow), "delete-event",
                     G_CALLBACK(&HudkitWindow::__handle_delete_event), this);

    g_signal_connect(G_OBJECT(gtkWindow), "screen-changed",
                     G_CALLBACK(&HudkitWindow::__handle_screen_changed), this);

    g_signal_connect(G_OBJECT(gtkWindow), "configure-event",
                     G_CALLBACK(&HudkitWindow::__handle_configure_event), this);

    gtk_main_iteration_do(true);

    destroyed = false;

    while (!destroyed)
    {
        auto frameStart = std::chrono::high_resolution_clock::now();

        UpdateDecorationSize();

        if (resizePending)
        {
            Resize();
            resizePending = false;
        }

        if (movePending)
        {
            Move();
            movePending = false;
        }

        gtk_main_iteration_do(false);
        CefDoMessageLoopWork();

        DrawWindow();

        auto frameEnd = std::chrono::high_resolution_clock::now();
        auto frameTime = frameEnd - frameStart;
        if (frameTime < frame_time)
        {
            std::this_thread::sleep_for(frame_time - frameTime);
        }
    }
}

void HudkitWindow::Close()
{
    keybinder_unbind(config.GetHotkey().c_str(), &HudkitWindow::__handle_hotkey);
}

void HudkitWindow::DrawWindow()
{
}

void HudkitWindow::Resize()
{
    fprintf(stdout, "Resizing window to %i,%i.\n", config.GetWidth(), config.GetHeight());
    gtk_window_resize(gtkWindow, config.GetWidth(), config.GetHeight());
    drawArea.SetSize(config.GetWidth(), config.GetHeight());
}

void HudkitWindow::Move()
{
    int x = config.GetX();
    int y = config.GetY();

    fprintf(stdout, "Moving inner window frame to %i, %i.\n", x, y);

    gtk_window_move(gtkWindow, x, y);
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
}

void HudkitWindow::__handle_configure_event(GtkWindow *window, GdkEvent *event, gpointer data)
{
    static int skippedEvents = 0;
    static bool lastLockedState = false;
    static bool firstAfterLock = false;

    if (skippedEvents < 2)
    {
        ++skippedEvents;
        return;
    }

    HudkitWindow *instance = (HudkitWindow *)data;

    if (event->configure.send_event == 0)
    {
        if (lastLockedState != instance->lockedState)
        {
            lastLockedState = instance->lockedState;
            if (instance->lockedState)
            {
                firstAfterLock = true;
            }
            return;
        }
        bool resize = false;

        int x = event->configure.x;
        int y = event->configure.y;
        int w = event->configure.width;
        int h = event->configure.height;

        if (!firstAfterLock)
        {
            x += instance->decorationSizeLeft;
            y += instance->decorationSizeTop;
        }
        firstAfterLock = false;

        printf("config event, %i %i %i %i, %i %i, %i\n", x, y, w, h, event->configure.x, event->configure.y, instance->lockedState);

        if (instance->config.GetX() != x)
        {
            instance->config.SetX(x);
        }
        if (instance->config.GetY() != y)
        {
            instance->config.SetY(y);
        }
        if (instance->config.GetWidth() != w)
        {
            instance->config.SetWidth(w);
            resize = true;
        }
        if (instance->config.GetHeight() != h)
        {
            instance->config.SetHeight(h);
            resize = true;
        }

        if (resize)
        {
            instance->resizePending = true;
        }
    }
}

void HudkitWindow::__handle_delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    handle_gtk_quit();
    HudkitWindow *instance = (HudkitWindow *)data;
    instance->destroyed = true;
}

void HudkitWindow::__handle_hotkey(const char *keystring, void *data)
{
    HudkitWindow *instance = (HudkitWindow *)data;
    instance->ToggleMoveResize();
}

void HudkitWindow::UpdateDecorationSize()
{
    if (!haveDecorationSizes && gtk_window_get_resizable(gtkWindow))
    {
        GtkAllocation allocation;
        GdkRectangle frameExtents;
        GdkRectangle frame;
        int originX, originY;
        gtk_widget_get_allocation(widgetWindow, &allocation);
        gdk_window_get_frame_extents(gdkWindow, &frameExtents);
        gdk_window_get_origin(gdkWindow, &originX, &originY);
        decorationSizeTop = originY - frameExtents.y;
        decorationSizeLeft = originX - frameExtents.x;
        decorationSizeBottom = (frameExtents.height - allocation.height) - decorationSizeTop;
        decorationSizeRight = (frameExtents.width - allocation.width) - decorationSizeLeft;
        if (decorationSizeTop || decorationSizeRight || decorationSizeBottom || decorationSizeLeft)
        {
            haveDecorationSizes = true;
        }
    }
}

void HudkitWindow::ToggleMoveResize()
{
    if (lockedState)
    {
        EnableMoveResize();
    }
    else
    {
        DisableMoveResize();
    }
}

void HudkitWindow::EnableMoveResize()
{
    if (lockedState)
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

        movePending = true;

        gdk_window_input_shape_combine_region(gdkWindow, NULL, 0, 0);

        lockedState = false;
    }
}

void HudkitWindow::DisableMoveResize()
{
    if (!lockedState)
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

        movePending = true;
        lockedState = true;
    }
}
