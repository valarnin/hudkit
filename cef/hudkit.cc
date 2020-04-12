#include "hudkit.hh"

#include <string>

#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_helpers.h"
#include "browser_handler.hh"
#include "draw.hh"

Hudkit::Hudkit()
{
  gtkApp = Gtk::Application::create();
  hudkitWindow = Glib::RefPtr<HudkitWindow>(new HudkitWindow());
}

Hudkit::~Hudkit()
{
}

void Hudkit::OnContextInitialized()
{
  CEF_REQUIRE_UI_THREAD();

  CefRefPtr<CefCommandLine> command_line = CefCommandLine::GetGlobalCommandLine();

  CefRefPtr<BrowserHandler> handler(new BrowserHandler(false, new HudkitRenderHandler(hudkitWindow.get()->drawArea)));

  CefBrowserSettings browser_settings;

  std::string url;

  url = command_line->GetSwitchValue("url");
  if (url.empty())
    url = "data:text/html;base64,PGh0bWw+PGJvZHkgc3R5bGU9ImJhY2tncm91bmQtY29sb3I6dHJhbnNwYXJlbnQ7Y29sb3I6d2hpdGUiPjxoMT5UZXN0aW5nIGh1ZGtpdDwvaDE+PC9ib2R5PjwvaHRtbD4=";

  // Information used when creating the native window.
  CefWindowInfo window_info;

  window_info.windowless_rendering_enabled = true;
  window_info.SetAsWindowless((unsigned long)hudkitWindow.get()->GetContainerWidget());

  browser_settings.windowless_frame_rate = 60;
  //browser_settings.background_color = CefColorSetARGB(255, 0, 0, 0);
  hudkitWindow.get()->add_tick_callback(sigc::mem_fun(*this, &Hudkit::__handler_tick));

  // Create the first browser window.
  CefRefPtr<CefClient> client(handler);

  CefRefPtr<CefBrowser> browser = CefBrowserHost::CreateBrowserSync(window_info, client, url, browser_settings, nullptr, nullptr);
}

bool Hudkit::__handler_tick(const Glib::RefPtr<Gdk::FrameClock> &clock) {
  CefDoMessageLoopWork();
  return true;
}

void Hudkit::Run()
{
  gtkApp.get()->run(*hudkitWindow.get());
}