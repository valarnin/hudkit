#include "hudkit_window.hh"
#include <gtkmm/cssprovider.h>

HudkitWindow::HudkitWindow()
{
    signal_window_state_event().connect(sigc::mem_fun(*this, &HudkitWindow::handler__OnExit));

    set_opacity(0);

    drawArea = Glib::RefPtr<DrawArea>(new DrawArea());
    add(*drawArea.get());
    drawArea.get()->set_hexpand(true);
    drawArea.get()->set_halign(Gtk::ALIGN_FILL);
    drawArea.get()->set_vexpand(true);
    drawArea.get()->set_valign(Gtk::ALIGN_FILL);
    
    show_all();
    //set_app_paintable(true);
    /*auto provider = Gtk::CssProvider::create();
    provider->load_from_data("* { background-color: rgba(0,0,0,0); }");
    //@TODO: 600 is supposed to be GTK_STYLE_PROVIDER_PRIORITY_APPLICATION but can't find that in gtkmm
    get_style_context()->add_provider_for_screen(Gdk::Screen::get_default(), provider, 10000);*/
    hide();
    gdk_window_set_override_redirect(gtk_widget_get_window(GTK_WIDGET(gobj())), true);
    set_keep_above(true);
    set_skip_taskbar_hint(true);
    set_accept_focus(false);
    set_decorated(false);
    set_resizable(false);
    input_shape_combine_region(Cairo::Region::create());
    show();
}

HudkitWindow::~HudkitWindow()
{
}

GtkWidget *HudkitWindow::GetContainerWidget()
{
    return GTK_WIDGET(gobj());
}

bool HudkitWindow::handler__OnExit(GdkEventWindowState *state)
{
    //Magic number. Need to figure out what I'm actually looking for here.
    if(state->new_window_state == 87041) {
        printf("hit quit\n");
        this->get_application()->quit();
        return false;
    }
    printf("%u\n", state->new_window_state);
    return true;
}

bool HudkitWindow::on_draw(const Cairo::RefPtr<Cairo::Context> &cr)
{
    cr->set_source_rgba(0, 0, 0, 0);
    cr->paint();
    return Gtk::Window::on_draw(cr);
}
