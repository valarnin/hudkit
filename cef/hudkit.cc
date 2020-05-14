#include "hudkit.hh"

#include <string>

#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_helpers.h"
#include "browser_handler.hh"
#include "hudkit_instance.hh"
#include "draw.hh"
#include "hudkit_config.hh"

Hudkit* Hudkit::instance;

Hudkit::Hudkit(HudkitConfig &config) : config(config), hudkitWindow(config)
{
  Hudkit::instance = this;
}

Hudkit::~Hudkit()
{
}

void Hudkit::OnContextInitialized()
{
  CEF_REQUIRE_UI_THREAD();

  hudkitWindow.Initialize();

  CefRefPtr<CefCommandLine> command_line = CefCommandLine::GetGlobalCommandLine();

  HudkitRenderHandler* renderHandler = new HudkitRenderHandler(hudkitWindow.drawArea);

  hudkitWindow.CEFRenderHandler = renderHandler;

  CefRefPtr<BrowserHandler> handler(new BrowserHandler(false, renderHandler));

  // Information used when creating the native window.
  CefWindowInfo window_info;

  window_info.windowless_rendering_enabled = true;
  window_info.SetAsWindowless(0);

  // Create the first browser window.
  CefRefPtr<CefClient> client(handler);

  CefBrowserSettings browser_settings;

  browser_settings.windowless_frame_rate = 60;
  //browser_settings.background_color = CefColorSetARGB(255, 0, 0, 0);

  CefRefPtr<CefBrowser> browser = CefBrowserHost::CreateBrowserSync(window_info, client, config.GetURL(), browser_settings, nullptr, nullptr);

  HudkitInstance* instance = new HudkitInstance(browser, browser_settings, config);

  instances.push_back(instance);
}

void Hudkit::Run()
{
  hudkitWindow.Run();
  hudkitWindow.Close();
}

HudkitInstance* Hudkit::GetInstance(CefRefPtr<CefBrowser> browser) {
  for (HudkitInstance* instance : instances) {
    if (instance->browser->GetIdentifier() == browser->GetIdentifier()) {
      return instance;
    }
  }
  return nullptr;
}