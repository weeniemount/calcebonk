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


    #define EXIT 128
    #define ABOUT 256

    HMENU menu = CreateMenu();
    HMENU options = CreateMenu();

    AppendMenu(menu, MF_POPUP, (UINT_PTR)options, "optionz");

    AppendMenu(options, MF_STRING, EXIT, "exitearino");
    AppendMenu(options, MF_STRING, ABOUT, "abaut");

	// Creating the window
	HWND hwnd = CreateWindowEx(
		0,                              // Optional window styles
		CLASS_NAME,                    // Window class
		"calcebonk",      // Window text
		WS_OVERLAPPEDWINDOW & ~(WS_SIZEBOX | WS_MAXIMIZEBOX),           // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, 241, 280,

		NULL,                          // Parent window
		menu,                          // Menu
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
                10, 10, 215, 30,
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
                    (HMENU)((UINT_PTR)(i + 1)),
                    (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
                    NULL
                );
                SendMessage(hwndButton, WM_SETFONT, (WPARAM)hComicSansFont, TRUE);
                SendMessage(hwndDisplay, WM_SETFONT, (WPARAM)hComicSansFont, TRUE);
            }
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            RECT rect;
            GetClientRect(hwnd, &rect);

            HBRUSH hBrush = CreateSolidBrush(RGB(255, 165, 0));
            FillRect(hdc, &rect, hBrush);

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
                    // Random chance to make button disappear
                    if (rand() % 50 == 0) {
                        // Button disappears
                        HWND hwndButton = GetDlgItem(hwnd, id);
                        ShowWindow(hwndButton, SW_HIDE);
                        
                        // Message box
                        MessageBox(hwnd, "sorry pal, the durabillity on that button was eh?\nit broke cuz you pressed it a teeny tiny bit too hard, yknow?", "Error", MB_OK | MB_ICONERROR);
                    } else {
                        AppendToDisplay(hwndDisplay, text);
                    }
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
