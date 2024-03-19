
#include <Windows.h>
#include <tchar.h>


// Forward declaration of the Window Procedure
// hWnd: A handle to a window
// message: A message that Windows sends to this application, such as a button being clicked
// wParam & lParam: Additional message information that is dependent on message
LRESULT CALLBACK WindowProcedure(
	_In_ HWND hWnd, 
	_In_ UINT message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
);


int WINAPI WinMain(
	_In_ HINSTANCE hInstance, 
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR,
	_In_ int nCmdShow
) {
	// Create and register window class for this application
	WNDCLASSEX WindowClassEx;
	static TCHAR szWindowClass[] = _T("DesktopApp");
	static TCHAR szTitle[] = _T("VSE Streamer");

	WindowClassEx.cbSize = sizeof(WNDCLASSEX);
	WindowClassEx.style = CS_HREDRAW | CS_VREDRAW;
	WindowClassEx.lpfnWndProc = WindowProcedure;
	WindowClassEx.cbClsExtra = 0;
	WindowClassEx.cbWndExtra = 0;
	WindowClassEx.hInstance = hInstance;
	WindowClassEx.hIcon = LoadIcon(WindowClassEx.hInstance, IDI_APPLICATION);
	WindowClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
	WindowClassEx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	WindowClassEx.lpszMenuName = NULL;
	WindowClassEx.lpszClassName = szWindowClass;
	WindowClassEx.hIconSm = LoadIcon(WindowClassEx.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&WindowClassEx)) {
		MessageBox(
			NULL, 
			_T("Failed to register window class!"),	// Popup message
			_T("VSE Streamer App Error"),			// Title of popup window
			NULL
		);
		return 1;
	}

	// Create a window using the window class we just created and registered
	int windowHeight = 300;
	int windowWidth = 300;
	HWND windowHandle = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW, 
		szWindowClass,
		szTitle, 
		WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, CW_USEDEFAULT, 
		windowWidth, windowHeight, 
		NULL, 
		NULL, 
		hInstance, 
		NULL
	);
	if (!windowHandle) {
		MessageBox(
			NULL,
			_T("Failed to instantiate window!"),
			_T("VSE Streamer App Error"),
			NULL
		);
		return 1;
	}

	// Display the window we created
	ShowWindow(windowHandle, nCmdShow);
	UpdateWindow(windowHandle);

	MSG message;
	while (GetMessage(&message, NULL, 0, 0)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
	return 0;
}

LRESULT CALLBACK WindowProcedure(
	_In_ HWND hWnd,
	_In_ UINT message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
) {
	PAINTSTRUCT ps;
	HDC deviceContextHandle;
	TCHAR greeting[] = _T("Virtual Streamer Interface");

	switch (message) {
	case WM_PAINT:
		deviceContextHandle = BeginPaint(hWnd, &ps);
		TextOut(deviceContextHandle, 5, 50, greeting, _tcslen(greeting));
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return 0;
}