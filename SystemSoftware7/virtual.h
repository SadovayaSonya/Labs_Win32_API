#pragma once
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WndProc_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
void WndProc_OnDestroy(HWND hWnd);
void WndProc_OnCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify);