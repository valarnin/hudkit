#include "draw.hh"
#include <gtk/gtk.h>
#include <cstring>
#include <cstdlib>

DrawArea::DrawArea() : texture(NULL)
{}
DrawArea::~DrawArea() {}

void DrawArea::update_texture(const void *texture, int width, int height)
{
    printf("update0\n");
    //Skip this frame, screen size changed between frames?
    if (width != this->width || height != this->height)
    {
        printf("update1\n");
        return;
    }
    printf("update2\n");
    memcpy(this->texture, texture, width * height * 4);
}

void DrawArea::SetSize(int width, int height)
{
    if (width != this->width || height != this->height)
    {
        if(this->texture != NULL) {
            free(this->texture);
        }
        this->texture = malloc(width * height * 4);
        this->width = width;
        this->height = height;
        if (this->pixbuf != NULL)
        {
            //@TODO: Uhh, should probably free pixbuf but no clue how?
        }
        this->pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, true, 8, width, height);
    }
}

void DrawArea::Register(GtkContainer *container)
{
    drawingArea = gtk_drawing_area_new();
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

    printf("hit draw 2, %u, %u\n", width, height);

    gdk_cairo_set_source_pixbuf(cr, instance->pixbuf, 0, 0);

    cairo_paint(cr);

    return true;
}