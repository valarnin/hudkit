#include "draw.hh"
#include <cairomm/context.h>
#include <gdkmm/general.h>

DrawArea::DrawArea() {}
DrawArea::~DrawArea() {}

void DrawArea::update_texture(const void *texture, int width, int height)
{
    printf("hit update, %u, %u\n", width, height);
    buffer = Gdk::Pixbuf::create(Gdk::Colorspace::COLORSPACE_RGB, true, 8, width, height);
    memcpy(buffer->get_pixels(), texture, width*height*4);
    queue_draw();
}

bool DrawArea::on_draw(const Cairo::RefPtr<Cairo::Context> &cr)
{
    cr->set_source_rgba(0, 0, 0, 0);
    cr->paint();
    
    if (!buffer)
    {
        return false;
    }

    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();

    printf("hit draw 2, %u, %u\n", width, height);

    Gdk::Cairo::set_source_pixbuf(cr, buffer);
    cr->paint();

    return true;
}