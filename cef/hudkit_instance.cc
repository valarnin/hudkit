#include "hudkit_instance.hh"

HudkitInstance::HudkitInstance(CefRefPtr<CefBrowser> browser, CefBrowserSettings browser_settings, HudkitConfig config)
  : browser(browser), browser_settings(browser_settings), config(config) {
  
}