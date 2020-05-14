#ifndef __hudkit_instance_hh_
#define __hudkit_instance_hh_

#include "hudkit_window.hh"
#include "hudkit_config.hh"
#include "include/internal/cef_ptr.h"

class HudkitInstance {
public:
  HudkitInstance(CefRefPtr<CefBrowser> browser, CefBrowserSettings browser_settings, HudkitConfig config);
  CefRefPtr<CefBrowser> browser;
  CefBrowserSettings browser_settings;
  HudkitConfig config;
};

#endif