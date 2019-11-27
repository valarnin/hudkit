#include <gtk/gtk.h>

GtkWidget *window;

bool have_determined_decoration_size = false;
int dx=0,dy=0;

void unregister_hotkey();
void disable_move_resize();
void enable_move_resize();

void resize() {
    fprintf(stdout, "Moving window to %i,%i at size %i,%i.\n", x, y, width, height);

    bool state = gtk_window_get_resizable(GTK_WINDOW(window));

    GdkWindow *gdk_window = gtk_widget_get_window(GTK_WIDGET(window));
    if(!state) {
        gdk_window_hide(gdk_window);
        gtk_window_set_resizable        (GTK_WINDOW(window), true);
        gdk_window_show(gdk_window);
    }
    gtk_window_move(GTK_WINDOW(window), x, y);
    gtk_window_resize(GTK_WINDOW(window), width, height);
    if(!state) {
        gdk_window_hide(gdk_window);
        gtk_window_set_resizable        (GTK_WINDOW(window), false);
        gdk_window_show(gdk_window);
    }
}

static void configure_callback(GtkWindow *window, GdkEvent *event, gpointer data) {
   x = event->configure.x;
   y = event->configure.y;
   width = event->configure.width;
   height = event->configure.height;
}

// This callback runs when the window is first set to appear on some screen, or
// when it's moved to appear on another.
static void screen_changed(GtkWidget *widget, GdkScreen *old_screen, gpointer userdata) {
    // Die unless the screen supports compositing (alpha blending)
    GdkScreen *screen = gtk_widget_get_screen(widget);
    if (!gdk_screen_is_composited(screen)) {
        fprintf(stderr, "Your screen does not support transparency.\n");
        fprintf(stderr, "Maybe your compositor isn't running?\n");
        exit(2);
    }

    // Ensure the widget (the window, actually) can take RGBA
    gtk_widget_set_visual(widget, gdk_screen_get_rgba_visual(screen));

    GtkWindowPosition position;

    GtkWindow* window = GTK_WINDOW(widget);

    resize();
}

void enable_move_resize() {
    GdkWindow *gdk_window = gtk_widget_get_window(GTK_WIDGET(window));

    int hx, hy, hw, hh;

    gtk_window_get_position(GTK_WINDOW(window), &hx, &hy);
    gtk_window_get_size(GTK_WINDOW(window), &hw, &hh);

    gdk_window_hide(gdk_window);
    gtk_window_set_keep_above       (GTK_WINDOW(window), true);
    gdk_window_set_override_redirect(GDK_WINDOW(gdk_window), false);
    gtk_window_set_decorated        (GTK_WINDOW(window), true);
    gtk_window_set_accept_focus     (GTK_WINDOW(window), true);
    gtk_window_set_resizable        (GTK_WINDOW(window), true);
    gdk_window_show(gdk_window);

    resize();

    gdk_window_input_shape_combine_region(GDK_WINDOW(gdk_window), NULL, 0,0);

    //@TODO: Get decoration size as dx,dy so we can reposition window properly on toggle
}

void disable_move_resize() {
    GdkWindow *gdk_window = gtk_widget_get_window(GTK_WIDGET(window));

    int hx, hy, hw, hh;

    gtk_window_get_position(GTK_WINDOW(window), &hx, &hy);
    gtk_window_get_size(GTK_WINDOW(window), &hw, &hh);

    gdk_window_hide(gdk_window);
    gtk_window_set_keep_above       (GTK_WINDOW(window), true);
    gdk_window_set_override_redirect(GDK_WINDOW(gdk_window), true);
    gtk_window_set_decorated        (GTK_WINDOW(window), false);
    gtk_window_set_accept_focus     (GTK_WINDOW(window), false);
    gtk_window_set_resizable        (GTK_WINDOW(window), false);

    cairo_region_t* region = cairo_region_create();
    gdk_window_input_shape_combine_region(GDK_WINDOW(gdk_window), region, 0,0);
    cairo_region_destroy(region);
    gdk_window_show(gdk_window);

    resize();
}

static int handle_gtk_quit() {
    write_config();
    gtk_main_quit();
    unregister_hotkey();
    return 0;
}