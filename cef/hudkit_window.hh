#ifndef __hudkit_window_hh_
#define __hudkit_window_hh_

#include "draw.hh"
#include "hudkit_config.hh"
#include <chrono>

class HudkitWindow {
public:
    HudkitWindow(HudkitConfig &config);
    virtual ~HudkitWindow();
    void Initialize();
    void Run();
    void Close();
    DrawArea drawArea;
    void ToggleMoveResize();
    void DisableMoveResize();
    void EnableMoveResize();
    GtkWidget* widgetWindow;
    void UpdateDecorationSize();
protected:
    bool resizePending = false, movePending = false;
    int decorationSizeTop, decorationSizeRight, decorationSizeBottom, decorationSizeLeft;
    HudkitConfig config;
    GtkWindow* gtkWindow;
    GdkWindow* gdkWindow;
    GtkContainer* containerWindow;
    int fps = 60;
    std::chrono::nanoseconds frame_time;
    void DrawWindow();
    static void __handle_screen_changed(GtkWidget *widget, GdkScreen *old_screen, gpointer userdata);
    static void __handle_configure_event(GtkWindow *window, GdkEvent *event, gpointer data);
    static void __handle_delete_event(GtkWidget *widget, GdkEvent *event, gpointer data);
    static void __handle_hotkey(const char* keystring, void* data);
    bool destroyed = true, lockedState = false, haveDecorationSizes = false;
    void Resize(), Move();
};

#endif