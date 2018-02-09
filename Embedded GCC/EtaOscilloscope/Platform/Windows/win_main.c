/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : win_main.c
 ***********************************************************************/

#include <windows.h>
#include <commctrl.h>
#include "resource.h"
#include "win_procs.h"

HINSTANCE hInstance_app;
SIZE sz_MinimalSize;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	hInstance_app = hInstance;

	LPCTSTR MainWndClass = TEXT("EtaOscil Emulator pltfrm. ") TEXT(VER_PLATFORM) TEXT(" EtaOS. ") TEXT(VER_ETAOS) TEXT(" app. ") TEXT(VER_APP);
	HWND hWnd;
	HACCEL hAccelerators;
	HMENU hSysMenu;
	MSG msg;
	DWORD style = WS_OVERLAPPEDWINDOW, exStyle = 0;

	// Initialise common controls.
	INITCOMMONCONTROLSEX icc = {
		.dwSize = sizeof(icc),
		.dwICC = ICC_WIN95_CLASSES,
	};
	InitCommonControlsEx(&icc);

	// Class for our main window.
	WNDCLASSEX wc = {
		.cbSize = sizeof(wc),
		.style = 0,
		.lpfnWndProc = &WndProc_Main,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = hInstance,
		.hIcon = LoadImage(hInstance, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_DEFAULTCOLOR | LR_SHARED),
		.hCursor = LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED),
		.hbrBackground = NULL,
		.lpszMenuName = MAKEINTRESOURCE(IDR_MAINMENU),
		.lpszClassName = MainWndClass,
		.hIconSm = LoadImage(hInstance, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR | LR_SHARED),
	};
	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, TEXT("Error registering window class."), TEXT("Error"), MB_ICONERROR | MB_OK);
		return 0;
	}

	RECT rc_MinimalRect = {
		.left = 0,
		.top = 0,
		.right = 0,
		.bottom = 0,
	};
	AdjustWindowRectEx(&rc_MinimalRect, style, TRUE, exStyle);
	sz_MinimalSize.cx = rc_MinimalRect.right - rc_MinimalRect.left; sz_MinimalSize.cy = rc_MinimalRect.bottom - rc_MinimalRect.top;

	hWnd = CreateWindowEx(exStyle, MainWndClass, MainWndClass, style, CW_USEDEFAULT, CW_USEDEFAULT, sz_MinimalSize.cx, sz_MinimalSize.cy, NULL, NULL, hInstance, NULL);
	if (!hWnd) {
		MessageBox(NULL, TEXT("Error creating main window."), TEXT("Error"), MB_ICONERROR | MB_OK);
		return 0;
	}

	hAccelerators = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR));

	hSysMenu = GetSystemMenu(hWnd, FALSE);
	InsertMenu(hSysMenu, 5, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
	InsertMenu(hSysMenu, 6, MF_BYPOSITION, ID_HELP_ABOUT, TEXT("About"));

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		if (!TranslateAccelerator(msg.hwnd, hAccelerators, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}
