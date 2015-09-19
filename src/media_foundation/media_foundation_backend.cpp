/**
    This source is using code from the https://github.com/roxlu/video_capture
    distributed under the Apache 2.0 license
    http://www.apache.org/licenses/LICENSE-2.0
  */

#include <atlbase.h>
#include "media_foundation_backend.h"

#include "../utils.h"
#include "../winapi_shared/winapi_shared_unique_id.h"
#include "media_foundation_camera.h"
#include "media_foundation_utils.h"

#include <string>

#include <mfapi.h>
#include <mferror.h>
#include <mfidl.h>
#include <mfplay.h>
#include <mfreadwrite.h>
#include <shlwapi.h>
#include <windows.h>

namespace webcam_capture {

MediaFoundation_Backend::MediaFoundation_Backend(std::shared_ptr<void> &deinitializer) :
    BackendInterface(BackendImplementation::MediaFoundation),
    deinitializer(deinitializer),
    notificationManager(BackendImplementation::MediaFoundation)
{
    // empty
}

std::unique_ptr<BackendInterface> MediaFoundation_Backend::create()
{
    HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        DEBUG_PRINT_HR_ERROR("Failed to initialize COM.", hr);

        // let RPC_E_CHANGED_MODE pass
        if (hr != RPC_E_CHANGED_MODE) {
            return nullptr;
        }
    }

    // "To close the COM library gracefully on a thread, each successful call to CoInitialize or CoInitializeEx,
    // including any call that returns S_FALSE, must be balanced by a corresponding call to CoUninitialize." --msdn
    bool deinitializeCom = hr == S_OK || hr == S_FALSE;

    hr = MFStartup(MF_VERSION);

    // "Before your application quits, call MFShutdown once for every previous call to MFStartup." --msdn
    // so assuming we need to shutdown it even on failures, thus no conditionals like `deinitializeCom`
    std::shared_ptr<void> deinitializer = std::shared_ptr<void>(nullptr,
                                                                std::bind(&MediaFoundation_Backend::deinitialize,
                                                                          std::placeholders::_1, deinitializeCom));

    if (FAILED(hr)) {
        DEBUG_PRINT_HR_ERROR("Failed to start Media Foundation.", hr);
        return nullptr;
    }

    return std::unique_ptr<BackendInterface>(new MediaFoundation_Backend(deinitializer));
}

void MediaFoundation_Backend::deinitialize(void *, bool deinitializeCom)
{
    HRESULT hr = MFShutdown();
    if (FAILED(hr)) {
        DEBUG_PRINT_HR_ERROR("Failed to shutdown the MediaFoundation.", hr);
    }

    if (deinitializeCom) {
        CoUninitialize();
    }

    DEBUG_PRINT("Successfully deinitilized.");
}

std::vector<CameraInformation> MediaFoundation_Backend::getAvailableCameras() const
{
    std::vector<CameraInformation> result;

    CComPtr<IMFAttributes> config;
    IMFActivate **devices = nullptr;
    UINT32 count = 0;

    HRESULT hr = MFCreateAttributes(&config, 1);
    if (FAILED(hr)) {
        DEBUG_PRINT_HR_ERROR("Failed to create attribute store.", hr);
        goto done;
    }

    // Filter capture devices.
    hr = config->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
    if (FAILED(hr)) {
        DEBUG_PRINT_HR_ERROR("Failed to set GUID on an attribute.", hr);
        goto done;
    }

    // Enumerate devices
    hr = MFEnumDeviceSources(config, &devices, &count);
    if (FAILED(hr)) {
        DEBUG_PRINT_HR_ERROR("Failed to get an enumeration of cameras.", hr);
        goto done;
    }

    if (count == 0) {
        DEBUG_PRINT("No video capture devices found.");
        goto done;
    }

    for (UINT32 i = 0; i < count; i++) {
        HRESULT hrFriendlyName;
        HRESULT hrSymbolicLink;
        WCHAR *friendlyName = nullptr;
        WCHAR *symbolicLink = nullptr;
        UINT32 unusedLength;

        hrFriendlyName = devices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &friendlyName, &unusedLength);
        hrSymbolicLink = devices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK,
                                             &symbolicLink, &unusedLength);

        if (SUCCEEDED(hrFriendlyName) && SUCCEEDED(hrSymbolicLink)) {
            std::string name = MediaFoundation_Utils::string_cast<std::string>(friendlyName);
            result.push_back({std::make_shared<WinapiShared_UniqueId>(symbolicLink, BackendImplementation::MediaFoundation),
                              name});
        }

        if (FAILED(hrFriendlyName)) {
            DEBUG_PRINT_HR_ERROR("Failed to get name of a camera.", hrFriendlyName);
        }

        if (FAILED(hrSymbolicLink)) {
            DEBUG_PRINT_HR_ERROR("Failed to get unique id of a camera.", hrSymbolicLink);
        }

        CoTaskMemFree(friendlyName);
        CoTaskMemFree(symbolicLink);
    }

done:
    for (UINT32 i = 0; i < count; i++) {
        MediaFoundation_Utils::safeRelease(&devices[i]);
    }

    CoTaskMemFree(devices);

    return result;
}

std::unique_ptr<webcam_capture::CameraInterface> MediaFoundation_Backend::getCamera(const CameraInformation &information) const
{
    return MediaFoundation_Camera::create(deinitializer, information);
}

int MediaFoundation_Backend::setCameraConnectionStateCallback(CameraConnectionStateCallback callback)
{
    // if callback is nullptr or callback has no function set
    if (!callback) {
        notificationManager.stop();
        return -1;      //TODO Err code
    }

    notificationManager.start(callback);

    return 1; //TODO ERR code (success)
}

} // namespace webcam_capture
