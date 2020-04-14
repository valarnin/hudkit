#ifndef __HUDKIT_CONFIG_HH_
#define __HUDKIT_CONFIG_HH_

#include <string>
#include "include/cef_parser.h"

class HudkitConfig
{
public:
    HudkitConfig(std::string file);
    virtual ~HudkitConfig();

    std::string file;

    int GetX(), GetY(), GetWidth(), GetHeight();
    std::string GetTitle(), GetURL(), GetLockHotkey(), GetRefreshHotkey();

    void SetX(int x), SetY(int y), SetWidth(int width), SetHeight(int height);

    void WriteConfig();

private:
    static int GetIntValue(CefRefPtr<CefDictionaryValue> &json, const char *key);
    static std::string GetStringValue(CefRefPtr<CefDictionaryValue> &json, const char *key);
    CefRefPtr<CefDictionaryValue> jsonDict;
    CefRefPtr<CefValue> jsonPointer;
};

#endif