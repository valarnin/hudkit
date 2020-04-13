#ifndef __hudkit_window_hh_
#define __hudkit_window_hh_

#include "draw.hh"
#include <chrono>

class HudkitWindow {
public:
    HudkitWindow();
    virtual ~HudkitWindow();
    void Initialize();
    void Run();
    void Close();
    DrawArea drawArea;
    void DisableMoveResize();
    void EnableMoveResize();
    void Resize();
    GtkWidget* widgetWindow;
protected:
    GtkWindow* gtkWindow;
    GdkWindow* gdkWindow;
    GtkContainer* containerWindow;
    int x = 100, y = 100, width = 200, height = 200;
    int fps = 60;
    std::chrono::nanoseconds frame_time;
    void DrawWindow();
    static void __handle_screen_changed(GtkWidget *widget, GdkScreen *old_screen, gpointer userdata);
    static void __handle_configure_event(GtkWindow *window, GdkEvent *event, gpointer data);
};

#endif