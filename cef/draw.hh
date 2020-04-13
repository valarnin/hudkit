#ifndef __DRAW_HH_
#define __DRAW_HH_

#include <gtk/gtk.h>

class DrawArea
{
public:
    DrawArea();
    virtual ~DrawArea();
    void update_texture(const void *texture, int width, int height);
    int width, height;
    void* texture;
    void SetSize(int width, int height);
    GtkWidget* widget;
    void Register(GtkContainer* container);
protected:
    GtkWidget* drawingArea;
    static gboolean draw_callback(GtkWidget* widget, cairo_t* cr, gpointer data);
    GdkPixbuf* pixbuf;
};

#endif