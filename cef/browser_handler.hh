#ifndef __BROWSER_HANDLER_H_
#define __BROWSER_HANDLER_H_

#include "include/cef_client.h"
#include "render_handler.hh"

#include <list>

class BrowserHandler : public CefClient, public CefDisplayHandler, public CefLifeSpanHandler, public CefLoadHandler {
 public:
  explicit BrowserHandler(bool use_views, CefRefPtr<CefRenderHandler> renderHandler);
  ~BrowserHandler();

  // Provide access to the single global instance of this object.
  static BrowserHandler* GetInstance();

  // CefClient methods:
  virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE {
    return this;
  }
  virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE {
    return this;
  }
  virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE {
    return this;
  }

  virtual CefRefPtr<CefRenderHandler> GetRenderHandler() OVERRIDE
  {
    return m_renderHandler;
  }

  virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) OVERRIDE;

  virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
  virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
  virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

  virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame,
                           ErrorCode errorCode,
                           const CefString& errorText,
                           const CefString& failedUrl) OVERRIDE;

  void CloseAllBrowsers(bool force_close);

  bool IsClosing() const { return is_closing_; }

 private:
  void PlatformTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title);

  const bool use_views_;

  typedef std::list<CefRefPtr<CefBrowser>> BrowserList;
  BrowserList browser_list_;

  bool is_closing_;

  CefRefPtr<CefRenderHandler> m_renderHandler;

  IMPLEMENT_REFCOUNTING(BrowserHandler);
};

#endif
