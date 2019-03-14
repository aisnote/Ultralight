#include <AppCore/CAPI.h>
#include <AppCore/AppCore.h>

using namespace ultralight;

// Undocumented functions from Ultralight's CAPI, each must be destroyed
ULExport ULRenderer C_WrapRenderer(Ref<Renderer> renderer);
ULExport ULView C_WrapView(Ref<View> view);

struct C_App : public AppListener {
	Ref<App> val;
  ULRenderer c_renderer = nullptr; // owned by C_App, destroyed in destructor
  ULWindow c_window = nullptr; // owned by user, cached in ULAppSetWindow
  ULUpdateCallback update_callback = nullptr;
  void* update_callback_data = nullptr;

  C_App(Ref<App> app) : val(app) {
    val->set_listener(this);
    c_renderer = C_WrapRenderer(app->renderer());
  }

  virtual ~C_App() {
    ulDestroyRenderer(c_renderer);
    val->set_listener(nullptr);
  }

  virtual void OnUpdate() override {
    if (update_callback)
      update_callback(update_callback_data);
  }
};

struct C_Window : public WindowListener {
  Ref<Window> val;
  ULCloseCallback close_callback = nullptr;
  void* close_callback_data = nullptr;
  ULResizeCallback resize_callback = nullptr;
  void* resize_callback_data = nullptr;

  C_Window(Ref<Window> window) : val(window) {
    window->set_listener(this);
  }

  virtual ~C_Window() {
    val->set_listener(nullptr);
  }

  virtual void OnClose() override {
    if (close_callback)
      close_callback(close_callback_data);
  }

  virtual void OnResize(int width, int height) override {
    if (resize_callback)
      resize_callback(resize_callback_data, width, height);
  }
};

struct C_Overlay {
  Ref<Overlay> val;
  ULView c_view = nullptr;

  C_Overlay(Ref<Overlay> overlay) : val(overlay) {
    c_view = C_WrapView(val->view());
  }

  virtual ~C_Overlay() {
    ulDestroyView(c_view);
  }
};


ULApp ULCreateApp() {
  return new C_App{ App::Create() };
}

void ULDestroyApp(ULApp app) {
  delete app;
}

void ULAppSetWindow(ULApp app, ULWindow window) {
  app->val->set_window(window->val);
  app->c_window = window;
}

ULWindow ULAppGetWindow(ULApp app) {
  return app->c_window;
}

void ULAppSetUpdateCallback(ULApp app, ULUpdateCallback callback, void* user_data) {
  app->update_callback = callback;
  app->update_callback_data = user_data;
}

bool ULAppIsRunning(ULApp app) {
  return app->val->is_running();
}

ULMonitor ULAppGetMainMonitor(ULApp app) {
  return reinterpret_cast<ULMonitor>(app->val->main_monitor());
}

ULRenderer ULAppGetRenderer(ULApp app) {
  return app->c_renderer;
}

void ULAppRun(ULApp app) {
  app->val->Run();
}

void ULAppQuit(ULApp app) {
  app->val->Quit();
}

double ULMonitorGetScale(ULMonitor monitor) {
  return reinterpret_cast<Monitor*>(monitor)->scale();
}

int ULMonitorGetWidth(ULMonitor monitor) {
  return reinterpret_cast<Monitor*>(monitor)->width();
}

int ULMonitorGetHeight(ULMonitor monitor) {
  return reinterpret_cast<Monitor*>(monitor)->height();
}

ULWindow ULCreateWindow(ULMonitor monitor, unsigned int width,
  unsigned int height, bool fullscreen, unsigned int window_flags) {
  return new C_Window(Window::Create(reinterpret_cast<Monitor*>(monitor), width, height,
    fullscreen, window_flags));
}

void ULDestroyWindow(ULWindow window) {
  delete window;
}

void ULWindowSetCloseCallback(ULWindow window, ULCloseCallback callback, void* user_data) {
  window->close_callback = callback;
  window->close_callback_data = user_data;
}

void ULWindowSetResizeCallback(ULWindow window, ULResizeCallback callback, void* user_data) {
  window->resize_callback = callback;
  window->resize_callback_data = user_data;
}

int ULWindowGetWidth(ULWindow window) {
  return window->val->width();
}

int ULWindowGetHeight(ULWindow window) {
  return window->val->height();
}

bool ULWindowIsFullscreen(ULWindow window) {
  return window->val->is_fullscreen();
}

double ULWindowGetScale(ULWindow window) {
  return window->val->scale();
}

void ULWindowSetTitle(ULWindow window, const char* title) {
  window->val->SetTitle(title);
}

void ULWindowSetCursor(ULWindow window, ULCursor cursor) {
  window->val->SetCursor((Cursor)cursor);
}

void ULWindowClose(ULWindow window) {
  window->val->Close();
}

int ULWindowDeviceToPixel(ULWindow window, int val) {
  return window->val->DeviceToPixels(val);
}

int ULWindowPixelsToDevice(ULWindow window, int val) {
  return window->val->PixelsToDevice(val);
}

ULOverlay ULCreateOverlay(int width, int height, int x, int y) {
  return new C_Overlay(Overlay::Create(width, height, x, y));
}

void ULDestroyOverlay(ULOverlay overlay) {
  delete overlay;
}

ULView ULOverlayGetView(ULOverlay overlay) {
  return overlay->c_view;
}

int ULOverlayGetWidth(ULOverlay overlay) {
  return overlay->val->width();
}

int ULOverlayGetHeight(ULOverlay overlay) {
  return overlay->val->height();
}

int ULOverlayGetX(ULOverlay overlay) {
  return overlay->val->x();
}

int ULOverlayGetY(ULOverlay overlay) {
  return overlay->val->y();
}

void ULOverlayMoveTo(ULOverlay overlay, int x, int y) {
  overlay->val->MoveTo(x, y);
}

void ULOverlayResize(ULOverlay overlay, int width, int height) {
  overlay->val->Resize(width, height);
}
