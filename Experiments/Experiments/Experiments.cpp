// T
// his file is for examining the outputs of the various functions and 
// 
// interfaces in the desktop duplication API.
// TODO:
// Explore the output of IDXGIFactory1::EnumAdapters1
// Experiment with IDXGIFactor::CreateSoftwareAdapter
// Explore the output of IDXGIOutputDuplication::AcquireNextFrame
// Explore the output of IDXGIOutputDuplication::GetFrameDirtyRects
// Explore the output of IDXGIOutputDuplication::GetFrameMoveRects
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX  // To allow std::max() to work

#include <chrono>
// Necessary includes for basic desktop duplication API
#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3d11")
#include <iostream>
#include <dxgi1_2.h>
#include <d3d11_2.h>
// Include for saving an ID3D11Texture2D to a png on desktop
#include <wincodec.h>
#include <directxtk/ScreenGrab.h>
// Includes for networking
#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <tchar.h>
// For string conversions
#include <string>
#include <codecvt>

// EnumAdapters1 needs an Adapter index as input (if you have a single
// graphics card, the only valid index will be 0), and you pass it a 
// an IDXGIAdapter1 to use as output.
// It returns an S_OK if successful, DXGI_ERROR_NOT_FOUND if the index
// is greater than or rqual to the number of adapters in the system (if it
// exceeds 0 in the case from above), or DXGI_ERROR_INVALID_CALL you pass
// it a null pointer for the IDXGIAdapter1


// Print out whatever gets returned by IDXGIFactory1::EnumAdapters1
int enumAdaptersExperiment() {
    // Create an IDXGI Factory. There are other ways to do this. The main
    // takeaway is to use the __uuidof function to get a reference to the
    // interface identifier for for the IDXGIFactory1 interface.
    HRESULT hr;
    IDXGIFactory2* pFactory = nullptr;
    hr = CreateDXGIFactory1(__uuidof(IDXGIFactory2), (void**)&pFactory);
    if (hr != S_OK) {
        std::cout << "Failed to create IDXGIFactory!" << std::endl;
        return 1;
    }
    // EnumAdapters1 needs an Adapter index as input (if you have a single
    // graphics card, the only valid index will be 0), and you pass it a 
    // an IDXGIAdapter1 to use as output.
    // It returns an S_OK if successful, DXGI_ERROR_NOT_FOUND if the index
    // is greater than or rqual to the number of adapters in the system (if it
    // exceeds 0 in the case from above), or DXGI_ERROR_INVALID_CALL you pass
    // it a null pointer for the IDXGIAdapter1
    IDXGIAdapter2* pAdapter = nullptr;
    std::cout << "The adapter before is: " << pAdapter << std::endl;
    hr = pFactory->EnumAdapters1(0, (IDXGIAdapter1**)&pAdapter);
    if (hr != S_OK) {
        std::cout << "Failed to enumerate adapters. Reason:" << std::endl;
        if (hr == DXGI_ERROR_NOT_FOUND) {
            std::cout << "Improper index." << std::endl;
        } 
        else if (hr == DXGI_ERROR_INVALID_CALL) {
            std::cout << "NULL passed as ppAdapter argument" << std::endl;
        }
        return 1;
    }
    std::cout << "The adapter is now " << pAdapter << std::endl;

    // Release the resources for the factory, since we no longer need it
    pFactory->Release();

    // Now that we have an adapter, let's inspect it.
    DXGI_ADAPTER_DESC2 adapterDesc;
    hr = pAdapter->GetDesc2(&adapterDesc);
    if (hr != S_OK) {
        std::cout << "Failed to get Adapter description!" << std::endl;
        if (hr == E_INVALIDARG) {
            std::cout << "pDesc can not be NULL" << std::endl;
        }
        return 1;
    }
    std::cout << "Description of Adapter:" << std::endl;
    std::cout << adapterDesc.Description << std::endl;
    std::cout << "Vendor ID: " << adapterDesc.VendorId << std::endl;
    std::cout << "Device ID: "<< adapterDesc.DeviceId << std::endl;
    std::cout << "Subsystem ID: " << adapterDesc.SubSysId << std::endl;
    std::cout << "Dedicated Video Memory: " << adapterDesc.DedicatedVideoMemory << std::endl;
    std::cout << "Flag: " << adapterDesc.Flags << std::endl;
    // DXGI 1.2 attribute. Not used 
    std::cout << "GraphicsPreemptionGranular: " << adapterDesc.GraphicsPreemptionGranularity << std::endl;

    // Always cleanup an adapter after it is no longer needed
    pAdapter->Release();
    return 0;
}

// Print information for an adapter.
int printAdapter2Info(IDXGIAdapter2* pAdapter) {
    DXGI_ADAPTER_DESC2 adapterDesc;
    HRESULT hr = pAdapter->GetDesc2(&adapterDesc);
    if (hr != S_OK) {
        std::cout << "Failed to get Adapter description!" << std::endl;
        return 1;
    }
    std::cout << "Description of Adapter:" << std::endl;
    std::wcout << adapterDesc.Description << std::endl;
    std::cout << "Vendor ID: " << adapterDesc.VendorId << std::endl;
    std::cout << "Device ID: " << adapterDesc.DeviceId << std::endl;
    std::cout << "Subsystem ID: " << adapterDesc.SubSysId << std::endl;
    std::cout << "Dedicated Video Memory: " << adapterDesc.DedicatedVideoMemory << std::endl;
    std::cout << "Flag: " << adapterDesc.Flags << std::endl;
    // DXGI 1.2 attribute. Not used 
    std::cout << "GraphicsPreemptionGranular: " << adapterDesc.GraphicsPreemptionGranularity << std::endl;
    return 0;
}

int printAdapterInfo(IDXGIAdapter* pAdapter) {
    DXGI_ADAPTER_DESC adapterDesc;
    HRESULT hr = pAdapter->GetDesc(&adapterDesc);
    if (hr != S_OK) {
        std::cout << "Failed to get Adapter description!" << std::endl;
        return 1;
    }
    std::cout << "Description of Adapter:" << std::endl;
    std::wcout << adapterDesc.Description << std::endl;
    std::cout << "Vendor ID: " << adapterDesc.VendorId << std::endl;
    std::cout << "Device ID: " << adapterDesc.DeviceId << std::endl;
    std::cout << "Subsystem ID: " << adapterDesc.SubSysId << std::endl;
    std::cout << "Dedicated Video Memory: " << adapterDesc.DedicatedVideoMemory << std::endl;
    return 0;
}

// Experiment with creating an adapter, and printing out it's information.
int createAdapterExperiment() {
    HRESULT hr;

    // Create a factory so we can create and enumerate an adapter
    IDXGIFactory2* pFactory = nullptr;
    hr = CreateDXGIFactory1(__uuidof(IDXGIFactory2), (void**)&pFactory);
    if (hr != S_OK) {
        std::cout << "Failed to create IDXGIFactory!" << std::endl;
        return 1;
    }
    // Creating a software adapter is a 3 step process
    // Call LoadLibrary to get a module handle
    LPCWSTR DLLPath = TEXT("IddSampleDriver\\IddSampleDriver.dll");
    HMODULE hModule = LoadLibrary(DLLPath);
    if (!hModule) {
        std::cout << "Failed to load library: " << DLLPath << std::endl;
        return 1;
    }
    // Pass the handle to CreateSoftwareAdapter
    IDXGIAdapter* pNewAdapter = nullptr;
    hr = pFactory->CreateSoftwareAdapter(hModule, &pNewAdapter);
    // Immediately call FreeLibrary on the module handle
    FreeLibrary(hModule);
    if (hr != S_OK) {
        std::cout << "Failed to create new adapter!" << std::endl;
        return 1;
    }
    printAdapterInfo(pNewAdapter);
    // Cleanup pNewAdapter
    pNewAdapter->Release();
    pNewAdapter = nullptr;
    return 0;
}

// Some helper functions to neatly print some information about DirectX interfaces
void printReferenceCountAndPointer(IUnknown* DXDIObject) {
    std::cout << "Pointer to DXGI Device: " << DXDIObject << std::endl;
    std::cout << "Reference count of DXGI Device: ";
    DXDIObject->AddRef();
    std::cout << DXDIObject->Release() << std::endl;
}

void printID3D11Texture2DDescription(ID3D11Texture2D* texture) {
    D3D11_TEXTURE2D_DESC pTextureDescription;
    texture->GetDesc(&pTextureDescription);
    std::cout << "*****ID3D11Texture2D Information*****" << std::endl;
    std::cout << "Width: " << pTextureDescription.Width << std::endl;
    std::cout << "Height: " << pTextureDescription.Height << std::endl;
    std::cout << "Array size: " << pTextureDescription.ArraySize << std::endl;
    std::cout << "Format : " << pTextureDescription.Format << std::endl;
}

void printDuplicationFrameInfo(const DXGI_OUTDUPL_FRAME_INFO frameInfo) {
    std::cout << "*****DuplicatedFrameInformation*****" << std::endl;
    POINT cursorPosition = frameInfo.PointerPosition.Position;
    UINT cursorXPosition = cursorPosition.x;
    UINT cursorYPosition = cursorPosition.y;
    std::cout << "Cursor position: (" << cursorXPosition << ", " << cursorYPosition << " )" << std::endl;
    std::cout << "Cursor visible?: " << frameInfo.PointerPosition.Visible << std::endl;
}

HRESULT saveTextureToPNG(ID3D11Texture2D* pTexture, const wchar_t* filePath)
{
    if (!pTexture || !filePath)
        return E_INVALIDARG;  
    HRESULT hr;

    // Retrieve the device context
    ID3D11Device* pDevice = nullptr;
    ID3D11DeviceContext* pContext = nullptr;
    pTexture->GetDevice(&pDevice);
    if (pDevice)
    {
        pDevice->GetImmediateContext(&pContext);
    }

    if (!pContext)
    {
        if (pDevice) pDevice->Release();
        return E_FAIL;
    }

    // Save the texture
    hr = DirectX::SaveWICTextureToFile(pContext, pTexture, GUID_ContainerFormatPng, filePath, &GUID_WICPixelFormat32bppBGRA);
    if (FAILED(hr)) {
        std::cout << "Saving failed!" << std::endl;
    }

    // Clean up
    pContext->Release();
    pDevice->Release();

    return hr;
}


#define DUPL_FRAME_READY(FrameInfo) ((FrameInfo.TotalMetadataBufferSize > 0))

// Set up everything necessary to call IDXGIOutput1::DuplicateOutput()::AcquireNextFrame()
int outputDuplicationExperiment() {
    HRESULT hr = S_OK;

    // Supported driver types
    // The HARDWARE type utilizes the GPU
    // The WARP type stands for Windows Advanced Rasterization Platform,
    //  and is a good fallback if the GPU is unavailable
    // The REFERENCE type implements the entire Direct3D feature set, but
    // runs very slowly. 
    D3D_DRIVER_TYPE driverTypes[] = {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE
    };
    UINT numDriverTypes = ARRAYSIZE(driverTypes);
    // Supported feature levels
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_1
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);
    // The ID3D11Device is an interface for a virtual adapter. It allows 
    // access to the GPU or a software adapter.
    ID3D11Device* pDevice = nullptr;
    ID3D11DeviceContext* pDeviceContext = nullptr;
    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; ++driverTypeIndex) {
        hr = D3D11CreateDevice(
            nullptr,                            // Use default adapter
            driverTypes[driverTypeIndex],       // Which driver type to use
            nullptr,                            // Software rasterizer DLL handle, unused here
            0,                                  // Flags, unused for this
            featureLevels, numFeatureLevels,    // Setting the supported feature levels defined above
            D3D11_SDK_VERSION,                  // This is always set to this value
            &pDevice,                           // Output. This is the device we're creating
            nullptr,                            // The feature level that ended up being supported
            &pDeviceContext                     // Output. The device context (unused here)
        );
        if (hr == S_OK) {
            break;
        }
    }
    if (hr != S_OK) {
        std::cout << "Direct3D device creation failed!" << std::endl;
        return 1;
    }

    // Query for IDXGIDevice interface. This interface is used by the DXGI API
    // to handle tasks like enumerating adapters and monitors and presenting
    // frames to output.
    IDXGIDevice2* pDXGIDevice = nullptr;
    hr = pDevice->QueryInterface(__uuidof(IDXGIDevice2), reinterpret_cast<void**>(&pDXGIDevice));
    if (hr != S_OK) {
        std::cerr << "Failed to query IDXGIDevice!" << std::endl;
        return 1;
    }

    // Get the adapter associated with the DXGI device. This adapter is an
    // interface that represents a display adapter (GPU). This can be thought of
    // as the connection between DXGI and the hardware.
    IDXGIAdapter2* pAdapter = nullptr;
    hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter2), reinterpret_cast<void**>(&pAdapter));
    pDXGIDevice->Release();
    pDXGIDevice = nullptr;
    if (hr != S_OK) {
        std::cout << "Failed to get adapter!" << hr << std::endl;
        return 1;
    }

    // EnumOutputs returns an IDXGIOutput interface, and this is needed to obtain
    // an IDXGIOutput1 interface, the "1" means that it supports output duplication
    // through the DuplicateOutput method.
    IDXGIOutput* pOutput = nullptr;
    hr = pAdapter->EnumOutputs(0, &pOutput);
    pAdapter->Release();
    pAdapter = nullptr;
    if (hr != S_OK) {
        std::cout << "Failed to get IDXGIOutput" << std::endl;
        return 1;
    }
    
    // This is used to represent a single output destination, such as a monitor.
    // 0 is the index for the first output.
    // See the outputEnumerationExperiment function for a look at outputs.
    IDXGIOutput1* pOutput1 = nullptr;
    hr = pOutput->QueryInterface(__uuidof(IDXGIOutput1), reinterpret_cast<void**>(&pOutput1));
    pOutput->Release();
    pOutput = nullptr;
    if (hr != S_OK) {
        std::cout << "Failed to query interface for IDXGIOutput1!" << std::endl;
        return 1;
    }

    // This interface is specifically for the Desktop Duplication API, and
    // allows efficient capturing and streaming of the content of a monitor.
    IDXGIOutputDuplication* pOutputDuplication = nullptr;
    hr = pOutput1->DuplicateOutput(pDevice, &pOutputDuplication);
    pOutput1->Release();
    pOutput1 = nullptr;
    if (hr != S_OK) {
        std::cout << "Failed to get output duplication!" << std::endl;
        return 1;
    }

    IDXGIResource* pDesktopResource = nullptr;
    ID3D11Texture2D* pAcquiredDesktopImage = nullptr;
    DXGI_OUTDUPL_FRAME_INFO FrameInfo;
    do {
        // Normally wouldn't call ReleaseFrame, but it's necessary
        // for the timeout to work. With a timeout of 0, there is an invalid
        // call to ReleaseFrame for every blank frame until a frame with
        // data is returned.
        pOutputDuplication->ReleaseFrame();
        hr = pOutputDuplication->AcquireNextFrame(50, &FrameInfo, &pDesktopResource);
        if (hr == DXGI_ERROR_WAIT_TIMEOUT) {
            std::cout << "Timeout elapsed without acquiring a frame. Not necessarily an error." << std::endl;
        }
        else if (hr == DXGI_ERROR_INVALID_CALL) {
            std::cout << "Release previous frame before re-calling AcquireNextFrame" << std::endl;
            return 1;
        }
        else if (hr != S_OK) {
            std::cout << "There was an error acquiring a frame.";
            return 1;
        }
        
    } while (!DUPL_FRAME_READY(FrameInfo));
    std::cout << "Frame Duplication Success!" << std::endl;


    // The AcquireNextFrame function gave us two things: 1) a Frame Data struct, which
    // is only useful for the pointer position and 2) an IDXGIResource*. The latter
    // is a generic interface that can represent different types of resources,
    // not just a texture. So to get a texture interface, we query for I3D11Texture2D below.
    hr = pDesktopResource->QueryInterface(
        __uuidof(ID3D11Texture2D),
        reinterpret_cast<void**>(&pAcquiredDesktopImage)
    );
    pDesktopResource->Release();
    pDesktopResource = nullptr;
    if (hr != S_OK) {
        std::cout << "Failed to query ID3D11Texture2D interface!" << std::endl;
        return 1;
    }

    // Save the frame to a file. It will save in the same directory
    // as this cpp file.
    std::cout << "Saving frame to file" << std::endl;
    //// Save the texture
    hr = DirectX::SaveWICTextureToFile(pDeviceContext, pAcquiredDesktopImage, GUID_ContainerFormatPng, L"frame2.png", &GUID_WICPixelFormat32bppBGRA);
    if (FAILED(hr)) {
        std::cout << "Saving failed!" << std::endl;
    }

    // This function does the same as above, given a pointer
    // to the desktop resource.
    //saveTextureToPNG(pAcquiredDesktopImage, L"TestFrame.png");

    pDevice->Release();
    pDeviceContext->Release();
    pAcquiredDesktopImage->Release();
    pOutputDuplication->ReleaseFrame();
    pOutputDuplication->Release();
    return 0;
}

// Print the left, top, right, and bottom coordinates of a RECT
int printRect(const RECT rect) {
    std::cout << "****RECT INFO****" << std::endl;
    std::cout << "Left coordinate: " << rect.left << std::endl;
    std::cout << "Top coordinate: " << rect.top << std::endl;
    std::cout << "Right coordinate: " << rect.right << std::endl;
    std::cout << "Bottom coordinate: " << rect.bottom << std::endl;
    std::cout << "*****************" << std::endl;
    return 0;
}

int printOutputInfo(IDXGIOutput* pOutput) {
    DXGI_OUTPUT_DESC outputDesc;
    HRESULT hr = pOutput->GetDesc(&outputDesc);
    if (hr != S_OK) {
        std::cout << "Failed to get IDXGIOutput1 description!" << std::endl;
        return 1;
    }
    std::wcout << "Device name: " << outputDesc.DeviceName << std::endl;
    printRect(outputDesc.DesktopCoordinates);
    std::cout << "Attached to desktop?: " << outputDesc.AttachedToDesktop << std::endl;
    std::cout << "DXGI Rotation Mode: " << outputDesc.Rotation << std::endl;
    return 0;
}

int printOutput1Info(IDXGIOutput1* pOutput) {
    DXGI_OUTPUT_DESC outputDesc;
    HRESULT hr = pOutput->GetDesc(&outputDesc);
    if (hr != S_OK) {
        std::cout << "Failed to get IDXGIOutput1 description!" << std::endl;
        return 1;
    }
    std::wcout << "Device name: " << outputDesc.DeviceName << std::endl;
    printRect(outputDesc.DesktopCoordinates);
    std::cout << "Attached to desktop?: " << outputDesc.AttachedToDesktop << std::endl;
    std::cout << "DXGI Rotation Mode: " << outputDesc.Rotation << std::endl;
    return 0;
}

// Show the information in an IDXGIOutput1 object
int outputEnumerationExperiment() {
    // The first step is to create a factory so we can get an adapter
    HRESULT hr;
    IDXGIFactory2* pFactory = nullptr;
    hr = CreateDXGIFactory1(__uuidof(IDXGIFactory2), (void**)&pFactory);
    if (hr != S_OK) {
        std::cout << "Failed to create IDXGIFactory!" << std::endl;
        return 1;
    }

    // Then we use the factory to get access to an adapter. Here we use
    // index 0 to get the first adapter.
    IDXGIAdapter2* pAdapter = nullptr;
    hr = pFactory->EnumAdapters1(0, (IDXGIAdapter1**)&pAdapter);
    if (hr != S_OK) {
        std::cout << "Failed to enumerate adapters." << std::endl;
        return 1;
    }

    // Once we have the adapter, we can use it to enumerate outputs
    UINT i = 0;
    IDXGIOutput* pOutput = nullptr;
    IDXGIOutput1* pOutput1 = nullptr;
    while (pAdapter->EnumOutputs(i, &pOutput) == S_OK) {
        ++i;
        // We have an IDXGIOutput, but we need to query the interface
        // for an IDXGIOutput1, since that is the output that enables
        // desktop duplication.
        hr = pOutput->QueryInterface(__uuidof(IDXGIOutput1), reinterpret_cast<void**>(&pOutput1));
        pOutput->Release();
        pOutput = nullptr;
        if (hr != S_OK) {
            std::cout << "Failed to query interface!" << std::endl;
        }
        printOutput1Info(pOutput1);
        pOutput1->Release();
        pOutput1 = nullptr;
    }
    return 0;
}

// Send a message over a TCP connection
int SimpleTCPExperiment() {
    // This struct receives details about the windows socket implementation,
    // as well as error codes
    WSADATA wsaData;
    int wsaErr;
    // This makes the version of the Windows Socket specification, 2.2
    WORD wVersionRequested = MAKEWORD(2, 2);
    // This must be the first windows socket function that gets called. It
    // loads the Winsock DLL which implements the Windows Socket API (WSA).
    wsaErr = WSAStartup(wVersionRequested, &wsaData);
    if (wsaErr != 0) {
        std::cout << "WSAStartup failed!" << std::endl;
        return 1;
    }
    std::cout << "WSAStartup successful!" << std::endl;
    std::cout << "Status: " << wsaData.szSystemStatus << std::endl;

    // Create socket for TCP. A socket is a pipe between two devices on a
    // network, and allows dataflow in either direction.
    SOCKET serverSocket = INVALID_SOCKET;
    // AF_INET specifies the address family. In this case, IPv4.
    // SOCK_STREAM and IPPROTO_TCP together specify type to be 
    // for TCP connections
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        std::cout << "Failed to create socket. Error: " << WSAGetLastError() << std::endl;
        WSACleanup();   // Free the DLL up
        return 1;
    }
    std::cout << "Socket creation successful!" << std::endl;

    // Bind ip address and port to socket
    sockaddr_in service;
    // Set the address family (AF) to AF_INET
    service.sin_family = AF_INET;
    // The first argument is the address family, like before.
    // The second argument is a string for the IPv4 address we're trying
    // to convert to numeric form. PtoN stands for text Presentation to Numeric.
    InetPton(AF_INET, _T("127.0.0.1"), &service.sin_addr.s_addr);
    int port = 55555;
    service.sin_port = htons(port);  // Convert from host to network byte order
    if (bind(serverSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
        std::cout << "Failed to bind socket! Error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }
    std::cout << "Socket successfully bound!" << std::endl;
    std::cout << "IP Address as Numeric decimal: " << ntohl(service.sin_addr.s_addr) << std::endl;
    std::cout << "As hex: " << std::hex << ntohl(service.sin_addr.s_addr) << std::endl;

    // Listen on socket
    if (listen(serverSocket, 1) == SOCKET_ERROR) {
        std::cout << "Failed to listen on socket! Error: " << WSAGetLastError() << std::endl;
    }
    std::cout << "Listening successful. Waiting for connections..." << std::endl;
    SOCKET clientSocket;
    sockaddr connectingEntityAddress;
    clientSocket = accept(serverSocket, &connectingEntityAddress, NULL);
    if (clientSocket == INVALID_SOCKET) {
        std::cout << "Failed to accept socket. Error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }
    std::cout << "Socket successfully accepted!" << std::endl;

    // Since a connection has been established with the client, it is now
    // possible to send messages back and forth.
    char receiveFromClientBuffer[200] = "";
    int bytesReceived = recv(clientSocket, receiveFromClientBuffer, 200, 0);
    if (bytesReceived < 0) {
        std::cout << "Error receiving from client! Error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }
    std::cout << "Received \"" << receiveFromClientBuffer << "\" from the client." << std::endl;


    char sendToClientBuffer[200] = "Server successfully received message from client";
    int bytesSent = send(clientSocket, sendToClientBuffer, 200, 0);
    if (bytesSent == SOCKET_ERROR) {
        std::cout << "Server failed to send! Error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }
    std::cout << "Server sent " << bytesSent << " bytes." << std::endl;
    return 0;
}

SOCKET setupTCPServer(_In_ const int portNumber, _In_ PCWSTR serverIPAddress) {
    WSADATA wsaData;
    int wsaErr;
    WORD wVersionRequested = MAKEWORD(2, 2);
    wsaErr = WSAStartup(wVersionRequested, &wsaData);
    if (wsaErr != 0) {
        std::cout << "WSAStartup failed!" << std::endl;
        return INVALID_SOCKET;
    }

    SOCKET serverSocket = INVALID_SOCKET;
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        std::cout << "Failed to create socket. Error: " << WSAGetLastError() << std::endl;
        WSACleanup();   // Free the DLL up
        return INVALID_SOCKET;
    }

    sockaddr_in service;
    service.sin_family = AF_INET;
    InetPton(AF_INET, serverIPAddress, &service.sin_addr.s_addr);
    service.sin_port = htons(portNumber);
    if (bind(serverSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
        std::cout << "Failed to bind socket! Error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return INVALID_SOCKET;
    }

    if (listen(serverSocket, 1) == SOCKET_ERROR) {
        std::cout << "Failed to listen on socket! Error: " << WSAGetLastError() << std::endl;
    }
    std::cout << "Listening successful. Waiting for connections..." << std::endl;
    SOCKET clientSocket;
    sockaddr connectingEntityAddress;
    clientSocket = accept(serverSocket, &connectingEntityAddress, NULL);
    if (clientSocket == INVALID_SOCKET) {
        std::cout << "Failed to accept socket. Error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return INVALID_SOCKET;
    }
    std::cout << "Socket successfully accepted!" << std::endl;
    return clientSocket;
}


// A simple TCP server for sending and receiving messages until a halt
// message is received from one end of the connection.
int simpleTCPContinuousChatApp() {
    SOCKET clientSocket;
    if ((clientSocket = setupTCPServer(55555, L"127.0.0.1")) == INVALID_SOCKET) {
        std::cout << "Failed to setup TCP Server!" << std::endl;
    }

    const int bufferSize = 500;
    char receiveFromClientBuffer[bufferSize] = "";
    char sendToClientBuffer[bufferSize] = "";
    int bytesReceived;
    int bytesSent;

    while (strcmp(receiveFromClientBuffer, "DISCONNECT") != 0) {
        bytesReceived = recv(clientSocket, receiveFromClientBuffer, bufferSize, 0);
        if (bytesReceived < 0) {
            std::cout << "Error receiving from client! Error: " << WSAGetLastError() << std::endl;
            WSACleanup();
            return 1;
        }
        std::cout << "Client: \"" << receiveFromClientBuffer << "\"" << std::endl;

        std::cout << "Type what you would like to send to the client: ";
        std::cin.getline(sendToClientBuffer, bufferSize);
        if (std::cin.fail()) {
            std::cin.clear();  // Clear the fail bit
            // Discard characters until a newline is encountered
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        bytesSent = send(clientSocket, sendToClientBuffer, bufferSize, 0);
        if (bytesSent == SOCKET_ERROR) {
            std::cout << "Server failed to send! Error: " << WSAGetLastError() << std::endl;
            WSACleanup();
            return 1;
        }
    }
    WSACleanup();
    return 0;
}

// Send an image over a UDP connection
int ImageUDPExperiment() {
    return 0;
}

// A small example to show how to handle the case where cin.getline() receives
// more characters than the buffer can hold. Without the calls to cin.clear()
// and cin.ignore, this loop will break if a message longer than 10 characters
// is sent. The null terminator is included in those 10 characters. So if 
// "12345678910" is sent, all that will be printed out is "123456789".
int cinExperiment() {
    char buffer[10] = "";
    
    while (true) {
        std::cout << "Type something: ";
        std::cin.getline(buffer, 10);
        if (std::cin.fail()) {
            std::cin.clear();  // Clear the fail bit
            // Discard characters until a newline is encountered
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        std::cout << "You typed \"" << buffer << "\"" << std::endl;
    }
}


// Usage:
// Run with one command argument, which is a single number specifying
// which experiment to run.
int main(int argc, char* argv[])
{
    HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
    if (FAILED(hr)) {
        std::cout << "Failed to initialize COM" << std::endl;
    }
    std::cout << "The amount of arguments is " << argc << std::endl;
    std::cout << "The argument received is " << *(argv[1]) << std::endl;

    switch (*(argv[1])) {
        case '1':
            enumAdaptersExperiment();
            break;
        case '2':
            createAdapterExperiment();
            break;
        case '3':
            outputDuplicationExperiment();
            break;
        case '4':
            SimpleTCPExperiment();
            break;
        case '5':
            outputEnumerationExperiment();
            break;
        case '6':
            simpleTCPContinuousChatApp();
            break;
        case '7':
            cinExperiment();
            break;
    }
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
