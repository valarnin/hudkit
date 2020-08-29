#include <webkit2/webkit2.h>

static void web_view_javascript_finished(GObject *object, GAsyncResult *result,
                                         gpointer user_data) {
  WebKitJavascriptResult *js_result;
  JSValueRef value;
  JSGlobalContextRef context;
  GError *error = NULL;

  js_result = webkit_web_view_run_javascript_finish(WEBKIT_WEB_VIEW(object),
                                                    result, &error);
  if (!js_result) {
    g_warning("Error running JavaScript: %s", error->message);
    g_error_free(error);
    return;
  }
  webkit_javascript_result_unref(js_result);
}
