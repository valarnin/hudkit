#include "hudkit_config.hh"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

HudkitConfig::HudkitConfig(std::string file)
{
    if(!std::filesystem::exists(file)) {

        std::cerr << "Config file does not exist!" << std::endl;
        std::ofstream outFile(file);
        if(!outFile.is_open())
        {
            std::cerr << "No write access to directory of config file, aborting!" << std::endl;
            exit(1);
        }
        std::cerr << "Creating a new default config file for you to customize at:" << std::endl << file << std::endl;
        std::string defaultConfig = R"({
  "title":"example overlay",
  "url":"file:///path/to/file.html?OVERLAY_WS=ws://127.0.0.1:10501/ws",
  "x":100,
  "y":100,
  "width":200,
  "height":200,
  "lockHotkey":"<Ctrl>L",
  "refreshHotkey":"<Ctrl><Alt>L",
}
)";
        outFile << defaultConfig;
        outFile.close();
        exit(1);
    }

    std::ifstream configFileStream;
    configFileStream.open(file);

    std::stringstream configStringStream;
    configStringStream << configFileStream.rdbuf();

    cef_json_parser_error_t errorCode;
    CefString errorMessage;

    jsonPointer = CefParseJSONAndReturnError(configStringStream.str(), cef_json_parser_options_t::JSON_PARSER_ALLOW_TRAILING_COMMAS, errorCode, errorMessage);
    if (jsonPointer.get() == NULL || !jsonPointer->IsValid() || jsonPointer->GetType() != VTYPE_DICTIONARY || jsonPointer->IsOwned())
    {
        std::cerr << "Failed to parse config file at " << file << "!" << std::endl
                  << std::endl;
        std::cerr << "Error code: " << errorCode << std::endl;
        std::cerr << "Error message:" << std::endl
                  << errorMessage.c_str() << std::endl;
        exit(1);
    }
    jsonDict = jsonPointer->GetDictionary();

    this->file = file;
}

HudkitConfig::~HudkitConfig()
{
}

int HudkitConfig::GetX() {
    return GetIntValue(jsonDict, "x");
}
int HudkitConfig::GetY() {
    return GetIntValue(jsonDict, "y");
}
int HudkitConfig::GetWidth() {
    return GetIntValue(jsonDict, "width");
}
int HudkitConfig::GetHeight() {
    return GetIntValue(jsonDict, "height");
}
std::string HudkitConfig::GetTitle() {
    return GetStringValue(jsonDict, "title");
}
std::string HudkitConfig::GetURL() {
    return GetStringValue(jsonDict, "url");
}
std::string HudkitConfig::GetLockHotkey() {
    return GetStringValue(jsonDict, "lockHotkey");
}
std::string HudkitConfig::GetRefreshHotkey() {
    return GetStringValue(jsonDict, "refreshHotkey");
}

void HudkitConfig::SetX(int x) {
    jsonDict->SetInt("x", x);
}
void HudkitConfig::SetY(int y) {
    jsonDict->SetInt("y", y);
}
void HudkitConfig::SetWidth(int width) {
    jsonDict->SetInt("width", width);
}
void HudkitConfig::SetHeight(int height) {
    jsonDict->SetInt("height", height);
}

int HudkitConfig::GetIntValue(CefRefPtr<CefDictionaryValue> &json, const char *key)
{
    if (!json->HasKey(key))
    {
        std::cerr << "Key \"" << key << "\" does not exist in JSON config!" << std::endl;
        exit(1);
    }
    if (json->GetType(key) != VTYPE_INT)
    {
        std::cerr << "Key \"" << key << "\" is not an integer value!" << std::endl;
        exit(1);
    }
    return json->GetInt(key);
}
std::string HudkitConfig::GetStringValue(CefRefPtr<CefDictionaryValue> &json, const char *key)
{
    if (!json->HasKey(key))
    {
        std::cerr << "Key " << key << "does not exist in JSON config!" << std::endl;
        exit(1);
    }
    if (json->GetType(key) != VTYPE_STRING)
    {
        std::cerr << "Key \"" << key << "\" is not a string value!" << std::endl;
        exit(1);
    }
    return json->GetString(key);
}

void HudkitConfig::WriteConfig()
{
    CefString json = CefWriteJSON(jsonPointer, cef_json_writer_options_t::JSON_WRITER_PRETTY_PRINT);
    
    std::ofstream output(file);
    output << json;
    output.flush();
    output.close();
}