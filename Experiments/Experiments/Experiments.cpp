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

// Necessary includes for basic desktop duplication API
#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3d11")
#include <iostream>
#include <dxgi1_2.h>
#include <d3d11_2.h>
// Include for saving an ID3D11Texture2D to a png on desktop
#include <DirectXTex.h>

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
}

// Some helper functions to neatly print some information about DirectX interfaces
void printReferenceCountAndPointer(IUnknown* DXDIObject) {
    std::cout << "Pointer to DXGI Device: " << DXDIObject << std::endl;
    std::cout << "Reference count of DXGI Device: ";
    DXDIObject->AddRef();
    std::cout << DXDIObject->Release() << std::endl;
}

void printID3D11Texture2DDescription(ID3D11Texture2D* texture) {
    HRESULT hr;
    D3D11_TEXTURE2D_DESC* pTextureDescription = nullptr;
    texture->GetDesc(pTextureDescription);
    std::cout << "*****ID3D11Texture2D Information*****" << std::endl;
    std::cout << "Width: " << pTextureDescription->Width << std::endl;
    std::cout << "Height: " << pTextureDescription->Height << std::endl;
    std::cout << "Array size: " << pTextureDescription->ArraySize << std::endl;
    std::cout << "Format : " << pTextureDescription->Format << std::endl;
}

void printDuplicationFrameInfo(const DXGI_OUTDUPL_FRAME_INFO frameInfo) {
    std::cout << "*****DuplicatedFrameInformation*****" << std::endl;
    POINT cursorPosition = frameInfo.PointerPosition.Position;
    UINT cursorXPosition = cursorPosition.x;
    UINT cursorYPosition = cursorPosition.y;
    std::cout << "Cursor position: (" << cursorXPosition << ", " << cursorYPosition << " )" << std::endl;
    std::cout << "Cursor visible?: " << frameInfo.PointerPosition.Visible << std::endl;
}

int saveID3D11Texture2DasPNG(ID3D11Texture2D* texture2D) {
    if (!texture2D) {
        return 1;
    }

    HRESULT hr;
    ID3D11Device* pDevice = nullptr;
    ID3D11DeviceContext* pContext = nullptr;
    texture2D->GetDevice(&pDevice);
    pDevice->GetImmediateContext(&pContext);

    // Get the description of the texture
    D3D11_TEXTURE2D_DESC desc;
    texture2D->GetDesc(&desc);

    // Create a staging texture with the same dimensions and format
    desc.BindFlags = 0;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
    desc.Usage = D3D11_USAGE_STAGING;
    desc.MiscFlags = 0;
    desc.MipLevels = 1;
    desc.ArraySize = 1;

    ID3D11Texture2D* stagingTexture = nullptr;
    hr = pDevice->CreateTexture2D(&desc, nullptr, &stagingTexture);
    if (FAILED(hr)) {
        pContext->Release();
        pDevice->Release();
        return 1;
    }

    // Copy the texture to the staging texture
    pContext->CopyResource(stagingTexture, texture2D);

    // Use DirectXTex to save the texture
    DirectX::ScratchImage scratchImage;
    hr = DirectX::CaptureTexture(pDevice, pContext, stagingTexture, scratchImage);
    if (FAILED(hr)) {
        stagingTexture->Release();
        pContext->Release();
        pDevice->Release();
        return 1;
    }

    // Save the image to a file
    hr = DirectX::SaveToWICFile(
        *scratchImage.GetImage(0, 0, 0),
        DirectX::WIC_FLAGS_NONE,
        GUID_ContainerFormatPng,
        L"frames/frame.png"
    );

    // Release resources
    stagingTexture->Release();
    pContext->Release();
    pDevice->Release();

    return SUCCEEDED(hr) ? 0 : 1;
}

// Set up everything necessary to call IDXGIOutput1::DuplicateOutput()
int outputDuplicationExperiment() {
    // Initialize DirectX
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
    // Here we create the D3D11 device. It's an interface for a virtual adapter
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
    // EnumOutputs returns an IDXGIOutput interface, and we need this to obtain
    // an IDXGIOutput1 interface, the "1" means that it supports output duplication
    // through the DuplicateOutput method
    IDXGIOutput* pOutput = nullptr;
    hr = pAdapter->EnumOutputs(0, &pOutput);
    pAdapter->Release();
    pAdapter = nullptr;
    if (hr != S_OK) {
        std::cout << "Failed to get IDXGIOutput" << std::endl;
        return 1;
    }
    
    // This is used to represent a single output destination, such as a monitor.
    // Notice here we use 0 for the index, which grabs the first output.
    // See the outputEnumerationExperiment function for a look at outputs.
    IDXGIOutput1* pOutput1 = nullptr;
    hr = pOutput->QueryInterface(__uuidof(IDXGIOutput1), reinterpret_cast<void**>(&pOutput1));
    pOutput->Release();
    pOutput = nullptr;
    if (hr != S_OK) {
        std::cout << "Failed to query interface for IDXGIOutput1!" << std::endl;
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
    // We no longer need these interfaces
    pDevice->Release();
    pDeviceContext->Release();

    // Now it's time to grab a frame; let's start by declaring an IDXGIResource
    // and an ID3D11Texture 2D to hold the image.
    IDXGIResource* pDesktopResource = nullptr;
    ID3D11Texture2D* pAcquiredDesktopImage = nullptr;
    DXGI_OUTDUPL_FRAME_INFO FrameInfo;

    hr = pOutputDuplication->AcquireNextFrame(500, &FrameInfo, &pDesktopResource);
    if (hr == DXGI_ERROR_WAIT_TIMEOUT) {
        std::cout << "Timeout elapsed without acquiring a frame. Not necessarily an error.";
        return 1;
    }
    else if (hr != S_OK) {
        std::cout << "There was an error acquiring a frame.";
        return 1;
    }
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
    }

    
    // Save the frame to a file
    

    





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


// Usage:
// Run with one command argument, which is a single number specifying
// which experiment to run.
int main(int argc, char* argv[])
{
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
            outputEnumerationExperiment();
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
