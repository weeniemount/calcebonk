#include <windows.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	const char CLASS_NAME[] = "calcebonkclass";

	WNDCLASS wc = { 0 };

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClass(&wc);

	// Creating the window
	HWND hwnd = CreateWindowEx(
		0,                              // Optional window styles
		CLASS_NAME,                    // Window class
		"calcebonk",      // Window text
		WS_OVERLAPPEDWINDOW,           // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, 100, 200,

		NULL,                          // Parent window
		NULL,                          // Menu
		hInstance,                     // Instance handle
		NULL                           // Additional application data
	);

	if (hwnd == NULL) {
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);

	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CREATE: {
		CreateWindow(
			"BUTTON",                  // Predefined class
			"7",                // Button text
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, // Styles
			10, 10, 100, 30,            // x, y, width, height
			hwnd,                       // Parent window
			(HMENU)1,                   // Button ID
			(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
			NULL                        // Pointer not needed
		);
		return 0;
	}
	case WM_COMMAND: {
		if (LOWORD(wParam) == 1) {
			MessageBox(hwnd, "eighbv...", "yeah", MB_OK | MB_ICONINFORMATION);
		}
		return 0;
	}
	case WM_DESTROY: {
		PostQuitMessage(0);
		return 0;
	}
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
