#include "render_handler.hh"

HudkitRenderHandler::HudkitRenderHandler(DrawArea draw)
    : draw(draw)
{
}

void HudkitRenderHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect)
{
    rect = CefRect(0, 0, draw.width, draw.height);
}
void HudkitRenderHandler::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height)
{
    draw.update_texture(buffer, width, height);
}