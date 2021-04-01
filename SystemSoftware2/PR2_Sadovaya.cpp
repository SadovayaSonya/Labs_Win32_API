#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <winbase.h>
#include <iostream>
#include <tchar.h>
#include <fstream>
#include <stdio.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void CreateMenubar(HWND);
void OpenDialog(HWND);
void OpenDumpDialog(HWND);
void LoadFile(LPSTR);
void LoadDump(LPSTR);
void AddFile(HWND);
void AddBinFile(HWND);

#define IDM_FILE_NEW 1
#define IDM_FILE_CREATE 2
#define IDM_BFILE_CREATE 3
#define IDM_DUMP_FILE 4
#define IE_DUMP 5
HWND ghwndEdit;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wc = { 0 };
	wc.lpszClassName = TEXT("Dialog window");
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(0, IDC_ARROW);

	RegisterClass(&wc);
	CreateWindow(wc.lpszClassName, TEXT("Dialog window"),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		150, 150, 265, 200, 0, 0, hInstance, 0);

	while (GetMessage(&msg, NULL, 0, 0)) {
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		ghwndEdit = CreateWindowEx
		(
			WS_EX_RIGHTSCROLLBAR,
			TEXT("edit"),
			NULL,
			WS_VISIBLE | WS_CHILD | WS_VSCROLL | ES_MULTILINE,
			0, 0, 800, 600,
			hwnd,
			(HMENU)1,
			NULL,
			NULL
		);
		CreateMenubar(hwnd);
		break;

	case WM_SIZE:
		SetWindowPos
		(
			ghwndEdit,
			NULL,
			0,
			0,
			LOWORD(lParam),
			HIWORD(lParam),
			SWP_NOMOVE | SWP_NOZORDER
		);
		break;

	case WM_COMMAND:
		if (wParam == IDM_FILE_NEW)
		{
			OpenDialog(hwnd);
		}
		if (wParam == IDM_FILE_CREATE)
		{
			AddFile(hwnd);
		}
		if (wParam == IDM_BFILE_CREATE)
		{
			AddBinFile(hwnd);
		}
		if (wParam == IDM_DUMP_FILE)
		{
			OpenDumpDialog(hwnd);
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void CreateMenubar(HWND hwnd)
{
	HMENU hMenubar;
	HMENU hMenu;

	hMenubar = CreateMenu();
	hMenu = CreateMenu();
	AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hMenu, TEXT("&File"));
	AppendMenu(hMenu, MF_STRING, IDM_FILE_NEW, TEXT("&Open"));
	AppendMenu(hMenu, MF_STRING, IDM_DUMP_FILE, TEXT("&Open file dump"));
	AppendMenu(hMenu, MF_STRING, IDM_FILE_CREATE, TEXT("&Create txt-file"));
	AppendMenu(hMenu, MF_STRING, IDM_BFILE_CREATE, TEXT("&Create bin-file"));
	SetMenu(hwnd, hMenubar);
}

void OpenDialog(HWND hwnd)
{
	OPENFILENAME ofn;
	TCHAR szFile[MAX_PATH];

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.hwndOwner = hwnd;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = TEXT("All files(*.*)\0*.*\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrFileTitle = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn))
		LoadFile(ofn.lpstrFile);
}

void OpenDumpDialog(HWND hwnd)
{
	OPENFILENAME ofn;
	TCHAR szFile[MAX_PATH];

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.hwndOwner = hwnd;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = TEXT("All files(*.*)\0*.*\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrFileTitle = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn))
		LoadDump(ofn.lpstrFile);
}

void LoadFile(LPSTR file)
{
	HANDLE hFile;
	DWORD dwSize;
	DWORD dw;

	LPBYTE lpBuffer = NULL;

	hFile = CreateFile(file, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	dwSize = GetFileSize(hFile, NULL);
	lpBuffer = (LPBYTE)HeapAlloc(GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS, dwSize + 1);
	ReadFile(hFile, (LPWSTR)lpBuffer, dwSize, &dw, NULL);
	CloseHandle(hFile);
	lpBuffer[dwSize] = 0;
	SetWindowText(ghwndEdit, (LPSTR)lpBuffer);
	HeapFree(GetProcessHeap(), 0, lpBuffer);
}

void LoadDump(LPSTR file)
{
	HANDLE hFile;
	DWORD dwSize;
	DWORD dw;
	CHAR lpBuffer [2000];

	hFile = CreateFile(file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	dwSize = GetFileSize(hFile, NULL); // длина файла
	SetFilePointer(hFile, 0, NULL, FILE_BEGIN); // установка указателя на начало файла
	ReadFile(hFile, lpBuffer, dwSize, &dw, NULL);
	int i = 0, k = 0;
	char buffer[2000];
	while (i < dwSize)
	{
		k += sprintf(buffer + k, "%X ", (unsigned char)lpBuffer[i]);
		i++;
	}
	SetWindowText(ghwndEdit, buffer);
	CloseHandle(hFile);
	lpBuffer[dwSize] = 0; // обнуление буфера
}

void AddFile(HWND hwnd)
{
	HANDLE hFile;

	hFile = CreateFile
	(
		"NewFile.txt",
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_WRITE | FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
		0
	);
	CloseHandle(hFile);

	hFile = CreateFile
	(
		"NewFile.txt",
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		0,
		NULL
	);
	LPCSTR str = "test message...\r\nHello, World!\r\nPR2_SadovayaSS\r\n1010.101012345";
	DWORD iNumWrite = 0;
	WriteFile(hFile, str, strlen(str), &iNumWrite, NULL);
	CloseHandle(hFile);
}

void AddBinFile(HWND hwnd)
{
	HANDLE hFile;
	hFile = CreateFile
	(
		"NewFile.bin",
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_WRITE | FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
		0
	);

	CloseHandle(hFile);

	hFile = CreateFile
	(
		"NewFile.bin",
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		0,
		NULL
	);
	int svalue = 1234;
	DWORD iNumWrite = 0;
	WriteFile(hFile, &svalue, sizeof(svalue), &iNumWrite, NULL);
	CloseHandle(hFile);
}