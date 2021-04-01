#include <windows.h> // ������������ ����, ���������� ������� API
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

static TCHAR szWindowClass[] = L"SystemSoftware"; // ������ � ������ ������
static TCHAR szTitle[] = L"First Practice in System Software";

HINSTANCE hInst;

// �������� ������� ��������� ��������� � ���������������� ���������:
LRESULT CALLBACK WndProc(HWND hWnd, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildWndProc(HWND child, UINT, WPARAM, LPARAM);
LRESULT CALLBACK Child1WndProc(HWND child1, UINT, WPARAM, LPARAM);

int WINAPI WinMain // �������� ������� - ������ int main()
(
    HINSTANCE hInstance, // ���������� ���������� ����������
    HINSTANCE hPrevInstance, // � Win32 �� ������������
    LPSTR lpCmdLine, // ����� ��� ������� ���� � ������ ��������� ������
    int nCmdShow // ����� ����������� ����
) 
{
    WNDCLASSEX wc; // �������� ���������� ��� ��������� � ������ ������ WNDCLASSEX
    wc.cbSize = sizeof(WNDCLASSEX); // ������ ��������� (� ������)
    wc.style = CS_HREDRAW | CS_VREDRAW; // ����� ������ ������
    wc.lpfnWndProc = WndProc; // ��������� �� ���������������� �������
    wc.cbClsExtra = 0; // ����� ������������� ������ � ����� ���������
    wc.cbWndExtra = 0; // ����� ������������� ������ ��� �������� ���������� ����������
    wc.hInstance = hInstance; // ��������� �� ������, ���������� ��� ����, ������������ ��� ������
    wc.hIcon = LoadIcon(hInstance, IDI_APPLICATION); // ��������� �����������
    wc.hCursor = LoadCursor(NULL, IDC_ARROW); // ���������� �������
    wc.hbrBackground = CreateSolidBrush(RGB(51, 153, 103)); // ���������� ����� ��� �������� ���� ����
    wc.lpszMenuName = NULL; // ��������� �� ��� ����
    wc.lpszClassName = szWindowClass; // ��������� �� ��� ������
    wc.hIconSm = LoadIcon(wc.hInstance, IDI_APPLICATION); // ���������� ��������� ����������� (� ����)

    if (!RegisterClassEx(&wc)) {
        // � ������ ���������� ����������� ������:
        MessageBox(NULL, L"�� ���������� ���������������� �����!", L"������", MB_OK);
        return NULL; // ����� �� WinMain
    }

    hInst = hInstance;
    
    HWND hWnd; // �������� ����������� �������� ������
    hWnd = CreateWindow // �������, ��������� ������
    (
        szWindowClass, // ��� ������
        szTitle, // ��� ����
        WS_OVERLAPPEDWINDOW, // ������ ����������� ������ (WS_VSCROLL)
        CW_USEDEFAULT, // ��������� ���� �� ��� � (�� ���������)
        CW_USEDEFAULT, // ��������� ���� �� ��� � (�� ���������)
        800, 600, // ������ � ������ ����
        NULL, // ���������� ������������� ������
        NULL, // ���������� ����
        hInstance, // ���������� ���������� ����������
        NULL // ������ �� ���������� �� WndProc
    );
    
    if (!hWnd) {
        // � ������ ������������� �������� ���� (�������� ��������� � �. �.):
        MessageBox(NULL, L"�� ���������� ������� ����!", L"������", MB_OK);
        return NULL; // ����� �� WinMain
    }
    
    ShowWindow(hWnd, nCmdShow); // ����������� ������
    UpdateWindow(hWnd); // ���������� ������
    
    WNDCLASSEX wcc; // �������� ���������� ��� ��������� � ������ ������ WNDCLASSEX
    wcc.cbSize = sizeof(WNDCLASSEX); // ������ ��������� (� ������)
    wcc.style = CS_HREDRAW | CS_VREDRAW; // ����� ������ ������
    wcc.lpfnWndProc = ChildWndProc; // ��������� �� ���������������� �������
    wcc.cbClsExtra = 0; // ����� ������������� ������ � ����� ���������
    wcc.cbWndExtra = 0; // ����� ������������� ������ ��� �������� ���������� ����������
    wcc.hInstance = hInstance; // ��������� �� ������, ���������� ��� ����, ������������ ��� ������
    wcc.hIcon = LoadIcon(hInstance, IDI_APPLICATION); // ��������� �����������
    wcc.hCursor = LoadCursor(NULL, IDC_ARROW); // ���������� �������
    wcc.hbrBackground = CreateSolidBrush(RGB(51, 102, 255)); // ���������� ����� ��� �������� ���� ����
    wcc.lpszMenuName = NULL; // ��������� �� ��� ����
    wcc.lpszClassName = L"Window"; // ��������� �� ��� ������
    wcc.hIconSm = LoadIcon(wc.hInstance, IDI_APPLICATION); // ���������� ��������� ����������� (� ����)

    if (!RegisterClassEx(&wcc)) {
        // � ������ ���������� ����������� ������:
        MessageBox(NULL, L"�� ���������� ���������������� �����!", L"������", MB_OK);
        return NULL; // ����� �� WinMain
    }

    HWND child;
    child = CreateWindow
    (
        L"Window",
        TEXT("Child window"),
        WS_OVERLAPPEDWINDOW,
        100, 60,
        400, 400,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!child) {
        // � ������ ������������� �������� ���� (�������� ��������� � �. �.):
        MessageBox(NULL, L"�� ���������� �������� ������� ����!", L"������", MB_OK);
        return NULL; // ����� �� WinMain
    }

    ShowWindow(child, nCmdShow); // ����������� ������
    SetParent(child, hWnd);

    WNDCLASSEX wcc1; // �������� ���������� ��� ��������� � ������ ������ WNDCLASSEX
    wcc1.cbSize = sizeof(WNDCLASSEX); // ������ ��������� (� ������)
    wcc1.style = CS_HREDRAW | CS_VREDRAW; // ����� ������ ������
    wcc1.lpfnWndProc = ChildWndProc; // ��������� �� ���������������� �������
    wcc1.cbClsExtra = 0; // ����� ������������� ������ � ����� ���������
    wcc1.cbWndExtra = 0; // ����� ������������� ������ ��� �������� ���������� ����������
    wcc1.hInstance = hInstance; // ��������� �� ������, ���������� ��� ����, ������������ ��� ������
    wcc1.hIcon = LoadIcon(hInstance, IDI_APPLICATION); // ��������� �����������
    wcc1.hCursor = LoadCursor(NULL, IDC_ARROW); // ���������� �������
    wcc1.hbrBackground = CreateSolidBrush(RGB(0, 255, 255)); // ���������� ����� ��� �������� ���� ����
    wcc1.lpszMenuName = NULL; // ��������� �� ��� ����
    wcc1.lpszClassName = L"Window 2"; // ��������� �� ��� ������
    wcc1.hIconSm = LoadIcon(wc.hInstance, IDI_APPLICATION); // ���������� ��������� ����������� (� ����)

    if (!RegisterClassEx(&wcc1)) {
        // � ������ ���������� ����������� ������:
        MessageBox(NULL, L"�� ���������� ���������������� �����!", L"������", MB_OK);
        return NULL; // ����� �� WinMain
    }

    HWND child1;
    child1 = CreateWindow
    (
        L"Window 2",
        TEXT("Mini window"),
        WS_OVERLAPPEDWINDOW,
        585, 250,
        200, 200,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!child1) {
        // � ������ ������������� �������� ���� (�������� ��������� � �. �.):
        MessageBox(NULL, L"�� ���������� ������� �������� ����!", L"������", MB_OK);
        return NULL; // ����� �� WinMain
    }

    ShowWindow(child1, nCmdShow); // ����������� ������
    SetParent(child1, hWnd);

    MSG msg; // �������� ���������� ��������� MSG ��� ��������� ���������
    
    while (GetMessage(&msg, NULL, 0, 0)) { // ���������� ��������� �� �������, ���������� ��-�����, ��
        TranslateMessage(&msg); // ����������������� ���������
        DispatchMessage(&msg); // �������� ��������� ������� � ��
    }
    return (int)msg.wParam; // ������� ���� ������ �� ����������
}

LRESULT CALLBACK WndProc
(
    HWND hWnd, // ���������� ������
    UINT message, // ���������, ���������� ��
    WPARAM wParam, LPARAM lParam // ��������� ��������� ��� ������������ ���������
)
{
    PAINTSTRUCT ps; // ���������, ���������� ���. � ���������� ���. (�������, ���� � �. �.)
    HDC hdc; // �������� ����������� ���������� ������ �� ������

    switch (message)
    {
    case WM_DESTROY: // ��� �������� ���� - ������:
        PostQuitMessage(0); // �������� WinMain() ��������� WM_QUIT
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam); // ��� �������� ����
        break;
    }
    return 0; // ������� ��������
}

LRESULT CALLBACK ChildWndProc
(
    HWND child, // ���������� ������
    UINT message, // ���������, ���������� ��
    WPARAM wParam, LPARAM lParam // ��������� ��������� ��� ������������ ���������
)
{
    PAINTSTRUCT ps; // ���������, ���������� ���. � ���������� ���. (�������, ���� � �. �.)
    HDC hdc; // �������� ����������� ���������� ������ �� ������

    switch (message)
    {
    case WM_DESTROY: // ��� �������� ���� - ������:
        return 0;
        break;
    default:
        return DefWindowProc(child, message, wParam, lParam); // ��� �������� ����
        break;
    }
    return 0; // ������� ��������
}