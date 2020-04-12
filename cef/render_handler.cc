#include "render_handler.hh"

HudkitRenderHandler::HudkitRenderHandler(Glib::RefPtr<DrawArea> draw)
    : draw(draw)
{
}

void HudkitRenderHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect)
{
    rect = CefRect(0, 0, draw.get()->get_width(), draw.get()->get_height());
}
void HudkitRenderHandler::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height)
{
    draw.get()->update_texture(buffer, width, height);
}