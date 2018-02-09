/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : win_procs.c
 ***********************************************************************/

#include "win_procs.h"
#include "resource.h"
#include "CPeriphWindows.h"
#include "CConverter.h"

#define RESOLUTION_MIN_WIDTH 400
#define RESOLUTION_MIN_HEIGHT 240

extern HINSTANCE hInstance_app;
extern SIZE sz_MinimalSize;
static HWND wnd_main;
static unsigned int res_width = RESOLUTION_MIN_WIDTH, res_height = RESOLUTION_MIN_HEIGHT;
static volatile bool do_quit, is_quited;
static volatile bool do_stop_app, is_stoped_app, is_inited_app;

DWORD WINAPI Thread_Draw(HANDLE handle) {
	is_quited = false;
	while(!do_quit) {
		Periph_TIM_20ms();
		Periph_KBD_20ms();
		if(wnd_main && lcd_is_need_draw) { InvalidateRect(wnd_main, nullptr, false); lcd_is_need_draw = false; }
		Sleep(20);
	}
	is_quited = true; do_quit = false;
	return 0;
}
DWORD WINAPI Thread_AppCycle(HANDLE handle) {
	__extension__ void _yield() { Sleep(1); };

	is_stoped_app = is_inited_app = false;
	extern void EtaOsciloscopeMainLoop(volatile bool *is_abort_requested, volatile bool *is_inited, void(*yield)());
	EtaOsciloscopeMainLoop(&do_stop_app, &is_inited_app, _yield);
	is_stoped_app = true; do_stop_app = is_inited_app = false;
	return 0;
}

LRESULT CALLBACK WndProc_Main(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			LCD_PaintToDC(hdc);
			EndPaint(hWnd, &ps);
			return 0;
		}

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case ID_HELP_ABOUT:
					DialogBox(hInstance_app, MAKEINTRESOURCE(IDD_ABOUTDIALOG), hWnd, &DialogProc_About);
					return 0;

				case ID_FILE_EXIT:
					DestroyWindow(hWnd);
					return 0;
			}
			break;

		case WM_GETMINMAXINFO: {
			MINMAXINFO *_fit_max_info = (MINMAXINFO *)lParam;
			_fit_max_info->ptMinTrackSize.x = sz_MinimalSize.cx + RESOLUTION_MIN_WIDTH;
			_fit_max_info->ptMinTrackSize.y = sz_MinimalSize.cy + RESOLUTION_MIN_HEIGHT;
			return 0;
		}

		case WM_SIZE: {
			unsigned int _new_res_width = LOWORD(lParam), _new_res_height = HIWORD(lParam);
			if(_new_res_width < RESOLUTION_MIN_WIDTH) _new_res_width = RESOLUTION_MIN_WIDTH;
			if(_new_res_height < RESOLUTION_MIN_HEIGHT) _new_res_height = RESOLUTION_MIN_HEIGHT;
			if(res_width != _new_res_width || res_height != _new_res_height) {
				res_width = _new_res_width; res_height = _new_res_height;
				do_stop_app = true; while(!is_stoped_app) Sleep(1);
				LCD_CreateContext(res_width, res_height);
				CreateThread(NULL, 0, &Thread_AppCycle, NULL, 0, 0);
				while(!is_inited_app) Sleep(1);
			}
			return 0;
		}

		case WM_KEYDOWN: {
			if(!(lParam & 0x40000000) && KBD_KeyDown((unsigned char)wParam)) return 0;
			break;
		}

		case WM_KEYUP: {
			if(KBD_KeyUp((unsigned char)wParam)) return 0;
			break;
		}

		case WM_ACTIVATE: {
			if(LOWORD(wParam) == WA_INACTIVE) { KBD_Reset(); return 0; }
			break;
		}

		case WM_SYSCOMMAND:
			switch (LOWORD(wParam)) {
				case ID_HELP_ABOUT: {
					DialogBox(hInstance_app, MAKEINTRESOURCE(IDD_ABOUTDIALOG), hWnd, &DialogProc_About);
					return 0;
				}
			}
			break;

		case WM_CREATE:
			wnd_main = hWnd;
			LCD_CreateContext(res_width, res_height); Periph_Init(); Converter_Init();
			CreateThread(NULL, 0, &Thread_Draw, NULL, 0, 0);
			CreateThread(NULL, 0, &Thread_AppCycle, NULL, 0, 0);
			return 0;

		case WM_DESTROY:
			wnd_main = nullptr;
			do_quit = do_stop_app = true; while(!is_quited || !is_stoped_app) Sleep(1);
			LCD_DestroyContext();
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

INT_PTR CALLBACK DialogProc_About(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_COMMAND: {
			switch (LOWORD(wParam)) {
				case IDOK:
				case IDCANCEL: {
					EndDialog(hwndDlg, (INT_PTR) LOWORD(wParam));
					return TRUE;
				}
			}
			break;
		}
		case WM_INITDIALOG:
			return TRUE;
	}

	return FALSE;
}
