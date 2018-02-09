/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : win_procs.h
***********************************************************************/

#pragma once

#include "config.h"
#include <windows.h>

// Window procedure for our main window.
LRESULT CALLBACK WndProc_Main(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Dialog procedure for our "about" dialog.
INT_PTR CALLBACK DialogProc_About(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
