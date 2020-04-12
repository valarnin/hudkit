#ifndef __DRAW_HH_
#define __DRAW_HH_

#include <gtkmm/drawingarea.h>

class DrawArea : public Gtk::DrawingArea
{
public:
    DrawArea();
    virtual ~DrawArea();
    void update_texture(const void *texture, int width, int height);

protected:
    bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr);
    Glib::RefPtr<Gdk::Pixbuf> buffer;
};

#endif