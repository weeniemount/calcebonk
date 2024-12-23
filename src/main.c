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

            HFONT hComicSansFont = CreateFont(
                20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                "Comic Sans MS"
            );

            // Create buttons
            const char* buttonTexts[] = {
                "7", "8", "9", "/",
                "4", "5", "6", "*",
                "1", "2", "3", "-",
                "0", ".", "+", "=",
            };

            int buttonWidth = 50;   // Button width
            int buttonHeight = 40;  // Button height
            int spacingX = 5;       // Horizontal spacing between buttons
            int spacingY = 5;       // Vertical spacing between buttons
            int offsetX = 10;       // X offset for the first column
            int offsetY = 50;       // Y offset for the first row

            for (int i = 0; i < 16; i++) {
                HWND hwndButton = CreateWindow(
                    "BUTTON",
                    buttonTexts[i],
                    WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                    offsetX + (i % 4) * (buttonWidth + spacingX), 
                    offsetY + (i / 4) * (buttonHeight + spacingY), 
                    buttonWidth, buttonHeight,
                    hwnd,
                    (HMENU)(i + 1),
                    (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
                    NULL
                );
                SendMessage(hwndButton, WM_SETFONT, (WPARAM)hComicSansFont, TRUE);

                // Modify the "=" button
                if (strcmp(buttonTexts[i], "=") == 0) {
                    HBRUSH hBrush = CreateSolidBrush(RGB(255, 165, 0)); // Orange color
                    SetClassLongPtr(hwndButton, GCLP_HBRBACKGROUND, (LONG_PTR)hBrush);
                    SetWindowPos(hwndButton, NULL, offsetX + (i % 4) * (buttonWidth + spacingX), 
                                offsetY + (i / 4) * (buttonHeight + spacingY), 
                                buttonWidth, buttonHeight, SWP_NOZORDER);
                }
            }
            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // Highlight "=" button in orange
            HWND hwndEqualsButton = GetDlgItem(hwnd, 16);  // Assuming "=" button has ID 16
            RECT rect;
            GetWindowRect(hwndEqualsButton, &rect);
            MapWindowPoints(NULL, hwnd, (POINT*)&rect, 2);
            FillRect(hdc, &rect, (HBRUSH)GetClassLongPtr(hwndEqualsButton, GCLP_HBRBACKGROUND));

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_COMMAND: {
            int id = LOWORD(wParam);

            if (id >= 1 && id <= 16) {
                const char* buttonTexts[] = {
                    "7", "8", "9", "/",
                    "4", "5", "6", "*",
                    "1", "2", "3", "-",
                    "0", ".", "+", "=",
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
