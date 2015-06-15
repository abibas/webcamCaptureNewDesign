/**
    This source is using code from the https://github.com/roxlu/video_capture
    distributed under the Apache 2.0 license
    http://www.apache.org/licenses/LICENSE-2.0
  */
#include <iostream>
#include "media_foundation_camera.h"
#include "media_foundation_backend.h"
#include <functional>


#ifdef DEBUG_VERSION
    #define DEBUG_PRINT(x) std::cerr << x << std::endl
#else
    #define DEBUG_PRINT(x)
#endif

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
      return new MediaFoundation_Camera(mfDeinitializer, information);
  }

  void MediaFoundation_Backend::setAvaliableCamerasChangedCallback(notifications_callback n_callback){
      //TODO start notifications by setting callback. If b_notif == NULL - stop notifications
  }

} // namespace webcam_capture
