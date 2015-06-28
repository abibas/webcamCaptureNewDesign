#include "media_foundation_camera_notifications.h"
#include "../media_foundation/media_foundation_backend.h"
#include "../utils.h"

#include <windows.h>
#include <algorithm>

namespace webcam_capture {

    LRESULT CALLBACK MediaFoundation_CameraNotifications::WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
        MediaFoundation_CameraNotifications* pParent;
        switch (uMsg) {
            case WM_CREATE: {
                pParent = (MediaFoundation_CameraNotifications*)((LPCREATESTRUCT)lParam)->lpCreateParams;
                SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)pParent);
                //TODO to create devices list with links.
                pParent->linkDeviceMap = pParent->GetAvailableCamerasWithLinks();
                break;
            }
            case WM_DEVICECHANGE: {
                pParent = (MediaFoundation_CameraNotifications*)GetWindowLongPtr(hWnd,GWLP_USERDATA);
                if(!pParent) break;

                if (wParam == DBT_DEVICEARRIVAL) {
                    pParent->CameraWasConnected((PDEV_BROADCAST_HDR)lParam);
                    DEBUG_PRINT("A device or piece of media has been inserted and is now available.\n");
                }
                if (wParam == DBT_DEVICEREMOVECOMPLETE) {
                    pParent->CameraWasRemoved((PDEV_BROADCAST_HDR)lParam);
                    DEBUG_PRINT("A device or piece of media has been removed.\n");
                }
                break;
            }
        }
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    MediaFoundation_CameraNotifications::MediaFoundation_CameraNotifications()
        :   threadStop(false),
            notif_cb(nullptr),
            messageLoopThread()
    {
    }

    MediaFoundation_CameraNotifications::~MediaFoundation_CameraNotifications(){
        this->Stop();
    }

    void MediaFoundation_CameraNotifications::Start(notifications_callback cb){
        threadStop = false;
        notif_cb = cb;
        messageLoopThread = std::thread(&MediaFoundation_CameraNotifications::MessageLoop, this);
    }

    void MediaFoundation_CameraNotifications::Stop(){
        threadStop = true;
        UnregisterDeviceNotification(hDevNotify);
        if( messageLoopThread.joinable() ) messageLoopThread.join();
        notif_cb = nullptr;
    }

    void MediaFoundation_CameraNotifications::MessageLoop(){
        this->InitMediaFoundation();

        //create stream to move to new stream
        WNDCLASS windowClass = {};
        windowClass.lpfnWndProc = &MediaFoundation_CameraNotifications::WindowProcedure;
        LPCWSTR windowClassName = L"CameraNotificationsMessageOnlyWindow";
        windowClass.lpszClassName = windowClassName;

        if (!RegisterClass(&windowClass)) {
            DEBUG_PRINT("Failed to register window class.\n");
            return;
        }

//        messageWindow = CreateWindow(windowClassName, 0, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, 0, this);
//        if (!messageWindow) {
//            DEBUG_PRINT("Failed to create message-only window.\n");
//            return;
//        }
        messageWindow = CreateWindowEx(0,windowClassName,0 ,0 ,0 ,0 ,0 ,0 , HWND_MESSAGE ,0 ,0 ,this);
        if (!messageWindow) {
            DEBUG_PRINT("Failed to create message-only window.\n");
            return;
        }

        DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
        ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));

        NotificationFilter.dbcc_size = sizeof(NotificationFilter);
        NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
        NotificationFilter.dbcc_classguid  = KSCATEGORY_CAPTURE;


        hDevNotify = RegisterDeviceNotification(messageWindow, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);

        MSG msg;
        while ( !threadStop && (GetMessage(&msg, messageWindow, 0, 0) > 0) ) {
            DEBUG_PRINT("FOOOOO.\n");
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        this->DeinitMediaFoundation();
    }


    bool MediaFoundation_CameraNotifications::InitMediaFoundation(){
        // Initialize COM
        HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
        if(FAILED(hr)) {
            DEBUG_PRINT("Error: cannot intialize COM.\n");  //or The COM is already initialized.
        }

        // Initialize MediaFoundation
        hr = MFStartup(MF_VERSION);
        if(FAILED(hr)) {
            DEBUG_PRINT("Error: cannot startup the Media Foundation.\n");
            return false;
        }
        return true;
    }

    void MediaFoundation_CameraNotifications::DeinitMediaFoundation(){
        /* Shutdown MediaFoundation */
        HRESULT hr = MFShutdown();
        if(FAILED(hr)) {
            DEBUG_PRINT("Error: failed to shutdown the MediaFoundation.\n");
        }

        /* Shutdown COM */
        CoUninitialize();
        DEBUG_PRINT("MediaFoundation_Backend Successfully deinited.\n");
    }

    void MediaFoundation_CameraNotifications::CameraWasRemoved(DEV_BROADCAST_HDR *pHdr){
        DEV_BROADCAST_DEVICEINTERFACE *pDi = (DEV_BROADCAST_DEVICEINTERFACE*)pHdr;
        std::string link = string_cast<std::string>(pDi->dbcc_name);

        //need to get all characters in uppercase, because "pDi->dbcc_name" and
        //"GetAllocatedString MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK" contatins different registers
        std::transform(link.begin(), link.end(), link.begin(), ::toupper);

        CameraInformation camInf = this->linkDeviceMap.at(link);
        this->notif_cb(camInf, CameraPlugStatus::CAMERA_DISCONNECTED);
    }

    void MediaFoundation_CameraNotifications::CameraWasConnected(DEV_BROADCAST_HDR *pHdr){
        DEV_BROADCAST_DEVICEINTERFACE *pDi = (DEV_BROADCAST_DEVICEINTERFACE*)pHdr;

    }

    std::unordered_map<std::string, CameraInformation> MediaFoundation_CameraNotifications::GetAvailableCamerasWithLinks(){
        std::unordered_map<std::string, CameraInformation> result;
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

          HRESULT hr1 = S_OK;
          HRESULT hr2 = S_OK;
          WCHAR* friendly_name = NULL;
          WCHAR* symbolic_link = NULL;
          UINT32 friendly_name_len = 0;
          UINT32 symbolic_link_len = 0;

          hr1 = devices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME,  &friendly_name, &friendly_name_len);
          hr2 = devices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK,
                                              &symbolic_link, &symbolic_link_len);
          if(SUCCEEDED(hr1) && SUCCEEDED(hr1)) {
            std::string name = string_cast<std::string>(friendly_name);
            std::string link = string_cast<std::string>(symbolic_link);
            std::transform(link.begin(), link.end(), link.begin(), ::toupper);

            CameraInformation camInfo(i, name);
            std::pair<std::string,CameraInformation> linkCam (link,camInfo);

            result.insert(linkCam);
          }

          CoTaskMemFree(friendly_name);
          CoTaskMemFree(symbolic_link);
        }

      done:
        safeReleaseMediaFoundation(&config);

        for(DWORD i = 0; i < count; ++i) {
          safeReleaseMediaFoundation(&devices[i]);
        }

        CoTaskMemFree(devices);

        return result;
    }
}
