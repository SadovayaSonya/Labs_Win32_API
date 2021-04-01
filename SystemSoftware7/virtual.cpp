#define _CRT_SECURE_NO_WARNINGS
#define STRICT
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <winbase.h>
#include <iostream>
#include <tchar.h>
#include "resource.h"
#include "virtual.h"
#include "afxres.h"

// ������ ����� ���������� ������, ��� ������� ����� ����������� ��������
#define MEMBLOCK_SIZE 4096

HINSTANCE hInst;
static TCHAR szAppName[] = L"VirtualApp";
static TCHAR szAppTitle[] = L"Working with Virtual Memory";

// ��������� �� ���� ������
LPVOID lpMemoryBuffer;

// ������������� ���� Set protection
HMENU hSetMenu;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hWnd;
    MSG msg;

    // ��������� ������������� ����������
    hInst = hInstance;

    // ���������, �� ���� �� ��� ���������� �������� �����
    hWnd = FindWindow(szAppName, NULL);
    if (hWnd)
    {
        // ���� ����, ��������� ���� ���������� �� �������� ����
        if (IsIconic(hWnd))
            ShowWindow(hWnd, SW_RESTORE);
        SetForegroundWindow(hWnd);
        return FALSE;
    }

    // ������������ ����� ����
    memset(&wc, 0, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASS);
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInst;
    wc.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_APPICON));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = MAKEINTRESOURCE(IDR_APPMENU);
    wc.lpszClassName = szAppName;
    if (!RegisterClassEx(&wc))
        if (!RegisterClass((LPWNDCLASS)&wc.style))
            return FALSE;

    // ������� ������� ���� ����������
    hWnd = CreateWindow
    (
        szAppName,
        szAppTitle,
        WS_OVERLAPPEDWINDOW,
        700,
        350,
        535,
        350,
        NULL, NULL,
        hInst, NULL
    );
    if (!hWnd) return(FALSE);

    // ���������� ���� � ��������� ���� ��������� ���������
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        HANDLE_MSG(hWnd, WM_CREATE, WndProc_OnCreate);
        HANDLE_MSG(hWnd, WM_DESTROY, WndProc_OnDestroy);
        HANDLE_MSG(hWnd, WM_COMMAND, WndProc_OnCommand);

    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
}

BOOL WndProc_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
    // ��������� ���������
    LPVOID lpReserved;

    // ����������� �������� ����������� ������
    lpReserved = VirtualAlloc(NULL, MEMBLOCK_SIZE, MEM_RESERVE, PAGE_NOACCESS);

    // ��� ������ ��������� ������ ����������
    if (lpReserved == NULL)
    {
        MessageBox(hWnd, L"������ ��� �������������� ������", szAppTitle, MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    // �������� ������ � �����������
    lpMemoryBuffer = VirtualAlloc(lpReserved, MEMBLOCK_SIZE, MEM_COMMIT, PAGE_NOACCESS);

    // ��� ������ ����������� ����������������� ����� ������ � ��������� ������ ����������
    if (lpMemoryBuffer == NULL)
    {
        MessageBox(hWnd, L"������ ��� ��������� ������ ��� �������������", szAppTitle, MB_OK | MB_ICONEXCLAMATION);
        VirtualFree(lpReserved, 0, MEM_RELEASE);
        return FALSE;
    }

    // �������� ������������� ���� Set protection
    hSetMenu = GetSubMenu(GetMenu(hWnd), 1);

    // �������� ������ PAGE_NOACCESS ����� ���� 
    CheckMenuItem(hSetMenu, ID_SETPROTECTION_PAGENOACCESS, MF_CHECKED);
    return TRUE;
}

#pragma warning(disable: 4098)
void WndProc_OnDestroy(HWND hWnd)
{
    // ����� ����������� ������ ���������� ����������� ���������� ����� ������
    if (lpMemoryBuffer != NULL)
        VirtualFree(lpMemoryBuffer, 0, MEM_RELEASE);
    PostQuitMessage(0);
}

#pragma warning(disable: 4098)
void WndProc_OnCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify)
{
    int   test;
    DWORD dwOldProtect;
    char  chBuff[256];

    switch (id)
    {
        // ������������� ��� ������� PAGE_NOACCESS
    case ID_SETPROTECTION_PAGENOACCESS:
    {
        VirtualProtect(lpMemoryBuffer, MEMBLOCK_SIZE, PAGE_NOACCESS, &dwOldProtect);
        // �������� ������ PAGE_NOACCESS ���� Set protection
        CheckMenuItem(hSetMenu, ID_SETPROTECTION_PAGENOACCESS, MF_CHECKED);
        // ������� ������� � ������ ����� ���� Set protection
        CheckMenuItem(hSetMenu, ID_SETPROTECTION_PAGEREADONLY, MF_UNCHECKED);
        CheckMenuItem(hSetMenu, ID_SETPROTECTION_PAGEREADWRITE, MF_UNCHECKED);
        CheckMenuItem(hSetMenu, ID_SETPROTECTION_PAGEGUARD, MF_UNCHECKED);
        break;
    }

    // ������������� ��� ������� PAGE_READONLY
    case ID_SETPROTECTION_PAGEREADONLY:
    {
        VirtualProtect(lpMemoryBuffer, MEMBLOCK_SIZE, PAGE_READONLY, &dwOldProtect);
        // �������� ������ PAGE_READONLY ���� Set protection
        CheckMenuItem(hSetMenu, ID_SETPROTECTION_PAGEREADONLY, MF_CHECKED);
        // ������� ������� � ������ ����� ���� Set protection
        CheckMenuItem(hSetMenu, ID_SETPROTECTION_PAGENOACCESS, MF_UNCHECKED);
        CheckMenuItem(hSetMenu, ID_SETPROTECTION_PAGEREADWRITE, MF_UNCHECKED);
        CheckMenuItem(hSetMenu, ID_SETPROTECTION_PAGEGUARD, MF_UNCHECKED);
        break;
    }

    // ������������� ��� ������� PAGE_READWRITE
    case ID_SETPROTECTION_PAGEREADWRITE:
    {
        VirtualProtect(lpMemoryBuffer, MEMBLOCK_SIZE, PAGE_READWRITE, &dwOldProtect);
        // �������� ������ PAGE_READWRITE ���� Set protection
        CheckMenuItem(hSetMenu, ID_SETPROTECTION_PAGEREADWRITE, MF_CHECKED);
        // ������� ������� � ������ ����� ���� Set protection
        CheckMenuItem(hSetMenu, ID_SETPROTECTION_PAGENOACCESS, MF_UNCHECKED);
        CheckMenuItem(hSetMenu, ID_SETPROTECTION_PAGEREADONLY, MF_UNCHECKED);
        CheckMenuItem(hSetMenu, ID_SETPROTECTION_PAGEGUARD, MF_UNCHECKED);
        break;
    }

    // ������������� ��� ������� PAGE_READWRITE | PAGE_GUARD
    case ID_SETPROTECTION_PAGEGUARD:
    {
        VirtualProtect(lpMemoryBuffer, MEMBLOCK_SIZE, PAGE_READWRITE | PAGE_GUARD, &dwOldProtect);
        // �������� ������ PAGE_READWRITE & PAGE_GUARD ���� Set protection
        CheckMenuItem(hSetMenu, ID_SETPROTECTION_PAGEGUARD, MF_CHECKED);
        // ������� ������� � ������ ����� ���� Set protection
        CheckMenuItem(hSetMenu, ID_SETPROTECTION_PAGENOACCESS, MF_UNCHECKED);
        CheckMenuItem(hSetMenu, ID_SETPROTECTION_PAGEREADONLY, MF_UNCHECKED);
        CheckMenuItem(hSetMenu, ID_SETPROTECTION_PAGEREADWRITE, MF_UNCHECKED);
        break;
    }

    // ��������� ������� ������
    case ID_MEMORY_READ:
    {
        __try
        {
            test = *((int*)lpMemoryBuffer);
        }

        // ���� �������� ����������, �������� � ���������� ��� ���
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            MessageBox(hWnd, L"������ ��� ������ ����� ������", szAppTitle, MB_OK | MB_ICONEXCLAMATION);
            break;
        }

        // ���� �������� ����������� �������, �������� �� ���� ������������
        MessageBox(hWnd, L"������ ���������", szAppTitle, MB_OK | MB_ICONEXCLAMATION);
        break;
    }

    // ��������� ������� ������
    case ID_MEMORY_WRITE:
    {
        __try
        {
            *((int*)lpMemoryBuffer) = 1;
        }

        // ���� �������� ����������, �������� � ���������� ��� ���
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            MessageBox(hWnd, L"������ ��� ������ � ���� ������", szAppTitle, MB_OK | MB_ICONEXCLAMATION);
            break;
        }

        // ���� �������� ����������� �������, �������� �� ���� ������������
        MessageBox(hWnd, L"������ ���������", szAppTitle, MB_OK | MB_ICONEXCLAMATION);
        break;
    }

    // ��������� ������� ������������ ����� ������
    case ID_MEMORY_LOCK:
    {
        if (VirtualLock(lpMemoryBuffer, MEMBLOCK_SIZE) == FALSE)
        {
            MessageBox(hWnd, L"������ ��� ������������ ������� � ������", szAppTitle, MB_OK | MB_ICONEXCLAMATION);
            break;
        }

        MessageBox(hWnd, L"������������ ���������", szAppTitle, MB_OK | MB_ICONEXCLAMATION);
        break;
    }

    // ��������� ������� ��������������� ����� ������
    case ID_MEMORY_UNLOCK:
    {
        if (VirtualUnlock(lpMemoryBuffer, MEMBLOCK_SIZE) == FALSE)
        {
            MessageBox(hWnd, L"������ ��� ��������������� ������� � ������", szAppTitle, MB_OK | MB_ICONEXCLAMATION);
            break;
        }

        MessageBox(hWnd, L"��������������� ���������", szAppTitle, MB_OK | MB_ICONEXCLAMATION);
        break;
    }

    case ID_FILE_EXIT:
    {
        // ��������� ������ ����������
        PostQuitMessage(0);
        break;
    }

    case ID_HELP_ABOUT:
    {
        MessageBox
        (
            hWnd,
            L"Working with Virtual Memory\n"
            "(C) Sofiya Sadovaya, 2021\n",
            szAppTitle,
            MB_OK | MB_ICONINFORMATION
        );
        break;
    }
    default:
        break;
    }
    return FORWARD_WM_COMMAND(hWnd, id, hwndCtl, codeNotify, DefWindowProc);
}