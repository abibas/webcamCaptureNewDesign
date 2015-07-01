/**
    This source is using code from the https://github.com/roxlu/video_capture
    distributed under the Apache 2.0 license
    http://www.apache.org/licenses/LICENSE-2.0
  */
#include <iostream>


#include "media_foundation_camera.h"
#include "media_foundation_backend.h"
#include "media_foundation_unique_id.h"

#include <functional>

namespace webcam_capture {

    MediaFoundation_Backend::MediaFoundation_Backend()
        :mfDeinitializer(this, MediaFoundation_Backend::DeinitBackend),
         notificationManager(NULL)
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
        delete notificationManager;
    }

    void MediaFoundation_Backend::DeinitBackend(void *){
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
        if(FAILED(hr)) goto done;

        // Filter capture devices.
        hr = config->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,  MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
        if(FAILED(hr)) goto done;

        // Enumerate devices
        hr = MFEnumDeviceSources(config, &devices, &count);
        if(FAILED(hr)) goto done;

        if(count == 0) goto done;

        for(DWORD i = 0; i < count; ++i) {
            HRESULT hr1 = S_OK;
            HRESULT hr2 = S_OK;
            WCHAR* friendly_name = NULL;
            WCHAR* symbolic_link = NULL;
            UINT32 friendly_name_len = 0;
            UINT32 symbolic_link_len = 0;

            hr1 = devices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME,  &friendly_name, &friendly_name_len);
            hr2 = devices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK,
                                                &symbolic_link, &symbolic_link_len);

            if(SUCCEEDED(hr1) && SUCCEEDED(hr2)) {
                std::string name = string_cast<std::string>(friendly_name);
                //MediaFoundation_UniqueId mfUniqueId(symbolic_link);
                UniqueId * uniqueId = new MediaFoundation_UniqueId(symbolic_link);
                CameraInformation camInfo(uniqueId, name);
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
        return MediaFoundation_Camera::createCamera(mfDeinitializer, information);
    }

    int MediaFoundation_Backend::setAvaliableCamerasChangedCallback(notifications_callback n_callback){
        if (notificationManager == nullptr) {
            notificationManager = new MediaFoundation_CameraNotifications();
        }
        //IF n_callback is null_ptr or n_callback function is empty
        if ( !n_callback ) {
            notificationManager->Stop();
            DEBUG_PRINT("Notifications capturing was stopped.\n");
            return -1;      //TODO Err code
        }
        notificationManager->Start(n_callback);
        DEBUG_PRINT("Notifications apturing was started.\n");
        return 1; //TODO ERR code (success)
    }

    int MediaFoundation_Backend::getCameraNameBySymbolicLink(WCHAR * symbolicLink, std::string cameraName){
        //TODO WRONG WAY!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//        UINT32 count = 0;
//        IMFAttributes *pAttributes = NULL;
//        IMFMediaSource **pSource = NULL;
//        IMFActivate **ppDevices = NULL;

//        HRESULT hr = MFCreateAttributes(&pAttributes, 2);

//        // Set the device type to video.
//        if (SUCCEEDED(hr))
//        {
//            hr = pAttributes->SetGUID(
//                MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
//                MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID
//                );
//        }

//        // Enumerate the devices,
//        if (SUCCEEDED(hr))
//        {
//            hr = MFEnumDeviceSources(pAttributes, &ppDevices, &count);
//        }

//        // Set the symbolic link.
//        if (SUCCEEDED(hr))
//        {
//            hr = pAttributes->SetString(
//                MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK,
//                (LPCWSTR)symbolicLink
//                );
//        }

//        if (SUCCEEDED(hr))
//        {
//            hr = MFCreateDeviceSource(pAttributes, pSource);
//        }

//        pDevice->ActivateObject(IID_PPV_ARGS(pSource));
//        if (FAILED(hr)) {
//            return -5; //TODO err codes
//        }

//        WCHAR* friendly_name = NULL;
//        UINT32 friendly_name_len = 0;

//        hr = pDevice->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME,  &friendly_name, &friendly_name_len);
//        if (FAILED(hr)) {
//            return -6; //TODO err codes
//        }


//        safeReleaseMediaFoundation(&pAttributes);
        return 1;
    }

} // namespace webcam_capture
