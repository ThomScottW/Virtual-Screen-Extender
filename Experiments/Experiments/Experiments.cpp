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

#pragma comment(lib, "dxgi")
#include <iostream>
#include <dxgi1_2.h>
#include <d3d11_2.h>

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
    hr = pFactory->EnumAdapters1(2, (IDXGIAdapter1**)&pAdapter);
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
    return 0;
}

// Print information for an adapter.
int printAdapterInfo(IDXGIAdapter2* pAdapter) {
    DXGI_ADAPTER_DESC2 adapterDesc;
    HRESULT hr = pAdapter->GetDesc2(&adapterDesc);
    if (hr != S_OK) {
        std::cout << "Failed to get Adapter description!" << std::endl;
        return 1;
    }
    std::cout << "Description of Adapter:" << std::endl;
    std::cout << adapterDesc.Description << std::endl;
    std::cout << "Vendor ID: " << adapterDesc.VendorId << std::endl;
    std::cout << "Device ID: " << adapterDesc.DeviceId << std::endl;
    std::cout << "Subsystem ID: " << adapterDesc.SubSysId << std::endl;
    std::cout << "Dedicated Video Memory: " << adapterDesc.DedicatedVideoMemory << std::endl;
    std::cout << "Flag: " << adapterDesc.Flags << std::endl;
    // DXGI 1.2 attribute. Not used 
    std::cout << "GraphicsPreemptionGranular: " << adapterDesc.GraphicsPreemptionGranularity << std::endl;
}

// Experiment with creating an adapter, and printing out it's information.
int createAdapterExperiment() {

    // Create a software adapter.





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

    // Pass the handle to CreateSoftwareAdapter

    // Immediately call FreeLibrary on the module handle



    // Enumerate all adapters
    IDXGIAdapter2* pAdapter;
    int adapterIndex = 0;
    while (hr == S_OK) {
        pAdapter = nullptr;
        hr = pFactory->EnumAdapters1(adapterIndex, (IDXGIAdapter1**)&pAdapter);
        if (hr != S_OK) {
            std::cout << "Failed to enumerate adapters. Reason:" << std::endl;
            if (hr == DXGI_ERROR_NOT_FOUND) {
                std::cout << "Improper index." << std::endl;
            }
            else if (hr == DXGI_ERROR_INVALID_CALL) {
                std::cout << "NULL passed as ppAdapter argument" << std::endl;
            }
            break;
        }
        std::cout << "The adapter is now " << pAdapter << std::endl;
    }
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
