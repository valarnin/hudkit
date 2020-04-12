#ifndef __hudkit_window_hh_
#define __hudkit_window_hh_

#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include "draw.hh"

class HudkitWindow : public Gtk::Window {
public:
    HudkitWindow();
    virtual ~HudkitWindow();
    GtkWidget* GetContainerWidget();
    Glib::RefPtr<DrawArea> drawArea;
protected:
    bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr);
private:
    bool handler__OnExit(GdkEventWindowState* state);
};

#endif