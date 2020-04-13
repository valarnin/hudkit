#include "hudkit.hh"

#include <string>

#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_helpers.h"
#include "browser_handler.hh"
#include "draw.hh"
#include "hudkit_config.hh"

Hudkit::Hudkit(HudkitConfig &config) : config(config), hudkitWindow(config)
{
}

Hudkit::~Hudkit()
{
}

void Hudkit::OnContextInitialized()
{
  CEF_REQUIRE_UI_THREAD();

  hudkitWindow.Initialize();

  CefRefPtr<CefCommandLine> command_line = CefCommandLine::GetGlobalCommandLine();

  CefRefPtr<BrowserHandler> handler(new BrowserHandler(false, new HudkitRenderHandler(hudkitWindow.drawArea)));

  CefBrowserSettings browser_settings;

  // Information used when creating the native window.
  CefWindowInfo window_info;

  window_info.windowless_rendering_enabled = true;
  window_info.SetAsWindowless(0);

  browser_settings.windowless_frame_rate = 60;
  //browser_settings.background_color = CefColorSetARGB(255, 0, 0, 0);

  // Create the first browser window.
  CefRefPtr<CefClient> client(handler);

  CefRefPtr<CefBrowser> browser = CefBrowserHost::CreateBrowserSync(window_info, client, config.GetURL(), browser_settings, nullptr, nullptr);
}

void Hudkit::Run()
{
  hudkitWindow.Run();
  hudkitWindow.Close();
}