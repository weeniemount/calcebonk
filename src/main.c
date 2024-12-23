#include <windows.h>
#include <stdio.h>
#include "resource/resource.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

char displayBuffer[256] = "";
char currentOperator = 0;
double operand1 = 0, operand2 = 0;
BOOL operatorSet = FALSE;

void UpdateDisplay(HWND hwndDisplay) {
	SetWindowText(hwndDisplay, displayBuffer);
}

void AppendToDisplay(HWND hwndDisplay, const char* text) {
	strcat_s(displayBuffer, sizeof(displayBuffer), text);
	UpdateDisplay(hwndDisplay);
}

void ClearDisplay(HWND hwndDisplay) {
	displayBuffer[0] = '\0';
	UpdateDisplay(hwndDisplay);
}

void PerformCalculation(HWND hwndDisplay) {
	operand2 = atof(displayBuffer);
	double result = 0;
	switch (currentOperator) {
		case '+': result = operand1 + operand2; break;
		case '-': result = operand1 - operand2; break;
		case '*': result = operand1 * operand2; break;
		case '/': result = (operand2 != 0) ? operand1 / operand2 : 0; break;
	}
	
	// Ensure no scientific notation
	if (result == (int)result) {
		snprintf(displayBuffer, sizeof(displayBuffer), "%.0f", result); // Integer format
	} else {
		_snprintf_s(displayBuffer, sizeof(displayBuffer), _TRUNCATE, "%.15f", result); // High precision
	}

	// Trim trailing zeros
	for (int i = strlen(displayBuffer) - 1; i > 0; i--) {
		if (displayBuffer[i] == '0' && displayBuffer[i - 1] != '.') {
			displayBuffer[i] = '\0';
		} else {
			break;
		}
	}

	UpdateDisplay(hwndDisplay);
	currentOperator = 0;
	operatorSet = FALSE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	const char CLASS_NAME[] = "calcebonkclass";

	WNDCLASS wc = { 0 };

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON)); // Load the icon

	RegisterClass(&wc);

	// Creating the window
	HWND hwnd = CreateWindowEx(
		0,                              // Optional window styles
		CLASS_NAME,                    // Window class
		"calcebonk",      // Window text
		WS_OVERLAPPEDWINDOW,           // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, 300, 400,

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
	static HWND hwndDisplay;

	switch (uMsg) {
	case WM_CREATE: {
		// Create a display
		hwndDisplay = CreateWindow(
			"STATIC",
			"",
			WS_VISIBLE | WS_CHILD | SS_RIGHT,
			10, 10, 260, 30,
			hwnd,
			(HMENU)100, // Unique ID for the display
			(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
			NULL
		);

		// Create buttons
		const char* buttonTexts[] = {
			"7", "8", "9", "/",
			"4", "5", "6", "*",
			"1", "2", "3", "-",
			"0", ".", "=", "+"
		};

		for (int i = 0; i < 16; i++) {
			CreateWindow(
				"BUTTON",
				buttonTexts[i],
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				10 + (i % 4) * 65, 50 + (i / 4) * 50, 60, 40,
				hwnd,
				(HMENU)(i + 1),
				(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
				NULL
			);
		}
		return 0;
	}
	case WM_COMMAND: {
		int id = LOWORD(wParam);

		if (id >= 1 && id <= 16) {
			const char* buttonTexts[] = {
				"7", "8", "9", "/",
				"4", "5", "6", "*",
				"1", "2", "3", "-",
				"0", ".", "=", "+"
			};

			const char* text = buttonTexts[id - 1];
			
			if (strcmp(text, "+") == 0 || strcmp(text, "-") == 0 || strcmp(text, "*") == 0 || strcmp(text, "/") == 0) {
				if (!operatorSet) {
					operand1 = atof(displayBuffer);
					currentOperator = text[0];
					operatorSet = TRUE;
					ClearDisplay(hwndDisplay);
				}
			} else if (strcmp(text, "=") == 0) {
				if (operatorSet) {
					PerformCalculation(hwndDisplay);
				}
			} else {
				AppendToDisplay(hwndDisplay, text);
			}
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
