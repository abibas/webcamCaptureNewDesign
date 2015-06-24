/**
    This source is using code from the https://github.com/roxlu/video_capture
    distributed under the Apache 2.0 license
    http://www.apache.org/licenses/LICENSE-2.0
  */
#include <iostream>
#include <Windows.h>
#include <tchar.h>
#include <dbt.h>
#include <ks.h>

#pragma comment(lib, "user32.lib")

#include "media_foundation_camera.h"
#include "media_foundation_backend.h"

#include <functional>

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (uMsg == WM_DEVICECHANGE) {
      if (wParam == DBT_DEVICEARRIVAL) {
          DEBUG_PRINT("A device or piece of media has been inserted and is now available.\n");
      }
      if (wParam == DBT_DEVICEREMOVECOMPLETE) {
          DEBUG_PRINT("A device or piece of media has been removed.\n");
      }
  }
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

namespace webcam_capture {

  MediaFoundation_Backend::MediaFoundation_Backend()
      :mfDeinitializer(this, MediaFoundation_Backend::DeinitBackend)
    {
      // Initialize COM
      HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
      if(FAILED(hr)) {
          DEBUG_PRINT("Error: cannot intialize COM.\n");  //or The COM is already initialized.
      }

      // Initialize MediaFoundation
      hr = MFStartup(MF_VERSION);
      if(FAILED(hr)) {
          DEBUG_PRINT("Error: cannot startup the Media Foundation.\n");
      }
  }

  MediaFoundation_Backend::~MediaFoundation_Backend() {
  }

  void MediaFoundation_Backend::DeinitBackend(void*){
      /* Shutdown MediaFoundation */
      HRESULT hr = MFShutdown();
      if(FAILED(hr)) {
          DEBUG_PRINT("Error: failed to shutdown the MediaFoundation.\n");
      }

      /* Shutdown COM */
      CoUninitialize();
      DEBUG_PRINT("MediaFoundation_Backend Successfully deinited.\n");
  }

  std::vector<CameraInformation> MediaFoundation_Backend::getAvailableCameras() const{

      std::vector<CameraInformation> result;
      UINT32 count = 0;
      IMFAttributes* config = NULL;
      IMFActivate** devices = NULL;

      HRESULT hr = MFCreateAttributes(&config, 1);
      if(FAILED(hr)) {
        goto done;
      }

      // Filter capture devices.
      hr = config->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,  MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
      if(FAILED(hr)) {
        goto done;
      }

      // Enumerate devices
      hr = MFEnumDeviceSources(config, &devices, &count);
      if(FAILED(hr)) {
        goto done;
      }

      if(count == 0) {
        goto done;
      }

      for(DWORD i = 0; i < count; ++i) {

        HRESULT hr = S_OK;
        WCHAR* friendly_name = NULL;
        UINT32 friendly_name_len = 0;
        hr = devices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME,  &friendly_name, &friendly_name_len);
        if(SUCCEEDED(hr)) {
          std::string name = string_cast<std::string>(friendly_name);

          CameraInformation camInfo(i, name);

          result.push_back(camInfo);
        }

        CoTaskMemFree(friendly_name);
      }

    done:
      safeReleaseMediaFoundation(&config);

      for(DWORD i = 0; i < count; ++i) {
        safeReleaseMediaFoundation(&devices[i]);
      }

      CoTaskMemFree(devices);

      return result;
  }

  CameraInterface* MediaFoundation_Backend::getCamera(const CameraInformation &information) const{
      CameraInformation inf(1, "dfsdf");
      cb_notif(inf);
      return MediaFoundation_Camera::createCamera(mfDeinitializer, information);
  }

  void MediaFoundation_Backend::setAvaliableCamerasChangedCallback(notifications_callback n_callback){
      //IF n_callback is null_ptr or n_callback function is empty
      if ( !n_callback ) {
          cb_notif = nullptr;
          DEBUG_PRINT("Error: The callback function is empty. Capturing was not started.\n");
          return;      //TODO Err code
      }

      cb_notif = n_callback;


      //create stream to move to new stream
      WNDCLASS windowClass = {};
      windowClass.lpfnWndProc = WindowProcedure;
      LPCWSTR windowClassName = L"CameraNotificationsMessageOnlyWindow";
      windowClass.lpszClassName = windowClassName;

      if (!RegisterClass(&windowClass)) {
          DEBUG_PRINT("Failed to register window class.\n");
          return;
      }
      HWND messageWindow = CreateWindow(windowClassName, 0, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, 0, 0);
      if (!messageWindow) {
          DEBUG_PRINT("Failed to create message-only window.\n");
          return;
      }

      DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
      ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));

      NotificationFilter.dbcc_size = sizeof(NotificationFilter);
      NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
      NotificationFilter.dbcc_classguid  = KSCATEGORY_CAPTURE;


      //NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_USB_DEVICE;

      HDEVNOTIFY hDevNotify = RegisterDeviceNotification(messageWindow, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);

      //mesagesDispatchThread()

      MSG msg;
//      while (GetMessage(&msg, 0, 0, 0) > 0) {
//          TranslateMessage(&msg);
//          DispatchMessage(&msg);
//      }
      return;
  }

} // namespace webcam_capture
