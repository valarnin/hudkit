#include "draw.hh"
#include <gtk/gtk.h>
#include <cstring>
#include <cstdlib>

DrawArea::DrawArea() : texture(NULL)
{
}
DrawArea::~DrawArea() {}

void DrawArea::update_texture(const void *texture, int width, int height)
{
    //Skip this frame, screen size changed between frames?
    if (width != this->width || height != this->height)
    {
        return;
    }

    memcpy(this->texture, texture, width * height * 4);

    gtk_widget_queue_draw(drawingArea);
}

void DrawArea::SetSize(int width, int height)
{
    if (width != this->width || height != this->height)
    {
        if (this->texture != NULL)
        {
            free(this->texture);
        }
        this->texture = malloc(width * height * 4);
        this->width = width;
        this->height = height;
        this->pixbuf = gdk_pixbuf_new_from_data((const guchar*)this->texture, GDK_COLORSPACE_RGB, true, 8, width, height, width*4, NULL, NULL);
    }
}

void DrawArea::Register(GtkContainer *container)
{
    drawingArea = gtk_drawing_area_new();
    gtk_widget_set_hexpand(drawingArea, true);
    gtk_widget_set_vexpand(drawingArea, true);
    gtk_container_add(GTK_CONTAINER(container), drawingArea);
    g_signal_connect(G_OBJECT(drawingArea), "draw",
                     G_CALLBACK(&DrawArea::draw_callback), (void *)(this));
}

gboolean DrawArea::draw_callback(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    DrawArea *instance = (DrawArea *)data;

    cairo_set_source_rgba(cr, 0, 0, 0, 0);
    cairo_paint(cr);

    if (instance->texture == NULL)
    {
        return false;
    }

    GtkAllocation allocation;

    gtk_widget_get_allocation(GTK_WIDGET(instance->drawingArea), &allocation);

    const int width = allocation.width;
    const int height = allocation.height;

    gdk_cairo_set_source_pixbuf(cr, instance->pixbuf, 0, 0);

    cairo_paint(cr);

    return true;
}