#include <windows.h>
#include <tchar.h>
#include <commctrl.h>
#include "resource.h"

#define IDM_EXIT 1
#define ID_RED 2
#define ID_GREEN 3
#define ID_BLUE 4

HINSTANCE g_hinst;
COLORREF g_color;

LONG WINAPI WndProc(HWND hWnd, UINT, WPARAM, LPARAM); // функция обработки сообщений окна
void CreateMenubar(HWND);

// Стартовая функция
int  WINAPI  WinMain(HINSTANCE  hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    HWND hwnd; // дескриптор окна
    MSG msg; // структура сообщения
    
    WNDCLASS w; // структура класса окна
    memset(&w, 0, sizeof(WNDCLASS)); // очистка памяти для структуры
    w.style = CS_HREDRAW | CS_VREDRAW;
    w.lpfnWndProc = WndProc;
    w.hInstance = hInstance;
    w.hbrBackground = CreateSolidBrush(0x00FFFFFF);
    w.lpszClassName = L"MyClass";
    RegisterClass(&w); // регистрация класса окна
    // Создание окна
    hwnd = CreateWindow
    (
        L"MyClass",
        L"Практика 3 - Садовая",
        WS_OVERLAPPEDWINDOW | WS_SYSMENU,
        630, 400,
        660, 330,
        NULL,
        NULL,
        hInstance,
        NULL
    );
    ShowWindow(hwnd, nCmdShow); // отображение окна
    UpdateWindow(hwnd); // перерисовка окна
    // Цикл обработки сообщений
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}
// Функция обработки сообщений
LONG WINAPI WndProc(HWND hwnd, UINT Message, WPARAM wparam, LPARAM lparam)
{
    int i;
    RECT rect;
    static HWND slider[3] = {0, 0, 0};
    CHAR spos[256], spos1[256], spos2[256];

    HDC hdc;
    HINSTANCE hInst;
    PAINTSTRUCT ps;
    HBRUSH hBrush, holdBrush;
    HPEN hPen, holdPen;
    static HWND hBtn, hBtn1, hBtn2; // дескриптор кнопки
    static HWND hEdt1; // дескрипторы полей редактирования
    static HWND hStat; // дескриптор статического текста
    
    switch (Message)
    {
    case WM_CREATE: // сообщение создания окна
        GetClientRect(hwnd, &rect);
        for (i = 0; i < sizeof(slider) / sizeof(slider[0]); i++)
        {
            slider[i] = CreateWindow
            (
                TRACKBAR_CLASS,
                L"",
                WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_NOTICKS,
                245, 10 + i * 40,
                180, 30,
                hwnd,
                0,
                (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
                NULL
            );
            ShowWindow(slider[i], SW_SHOW);
            UpdateWindow(slider[i]);
            SendMessage(slider[i], TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 255));
        }
        hInst = ((LPCREATESTRUCT)lparam)->hInstance; // дескриптор приложения
        CreateMenubar(hwnd);
        // Создаем и показываем первое поле редактирования
        hEdt1 = CreateWindow
        (
            L"edit",
            L"Введите текст...",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT | ES_AUTOHSCROLL,
            20, 10,
            200, 22,
            hwnd,
            0,
            hInst,
            NULL
        );
        ShowWindow(hEdt1, SW_SHOWNORMAL);
        // Создаем и показываем кнопку
        hBtn = CreateWindow
        (
            L"button",
            L"Применить",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            440, 150,
            200, 30,
            hwnd,
            0,
            hInst,
            NULL
        );
        ShowWindow(hBtn, SW_SHOWNORMAL);
        hBtn1 = CreateWindow
        (
            L"button",
            L"Изменить",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            440, 190,
            200, 30,
            hwnd,
            0,
            hInst,
            NULL
        );
        ShowWindow(hBtn1, SW_SHOWNORMAL);
        hBtn2 = CreateWindow
        (
            L"button",
            L"Выход",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            440, 230,
            200, 30,
            hwnd,
            0,
            hInst,
            NULL
        );
        ShowWindow(hBtn2, SW_SHOWNORMAL);
        // Создаем и показываем поле текста для результата
        hStat = CreateWindow
        (
            L"static",
            L"История текста",
            WS_CHILD | WS_VISIBLE | WS_VSCROLL,
            20, 42,
            200, 215,
            hwnd,
            0,
            hInst,
            NULL
        );
        ShowWindow(hStat, SW_SHOWNORMAL);
        CreateWindowW
        (
            L"Button", L"Выберите цвет",
            WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
            235, 137, 190, 123, hwnd, (HMENU)0, hInst, NULL
        );
        CreateWindowW
        (
            L"Button", L"Красный",
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            245, 157, 170, 41, hwnd, (HMENU)ID_RED, hInst, NULL
        );
        CreateWindowW
        (
            L"Button", L"Зеленый",
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            245, 182, 170, 41, hwnd, (HMENU)ID_GREEN, hInst, NULL
        );
        CreateWindowW
        (
            L"Button", L"Синий",
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            245, 207, 170, 41, hwnd, (HMENU)ID_BLUE, hInst, NULL
        );
        break;
    case WM_HSCROLL:
        switch (LOWORD(wparam))
        {
        case TB_THUMBTRACK:
        case TB_THUMBPOSITION:
            if (slider[0] == (HWND)lparam)
            { // обработка события 1-го trackbar
                wsprintfA(spos, "%d", HIWORD(wparam));
                SetWindowTextA(hwnd, spos);
            }
            if (slider[1] == (HWND)lparam)
            {  // обработка события 2-ого trackbar
                wsprintfA(spos1, "%d", HIWORD(wparam));
                SetWindowTextA(hwnd, spos1);
            }
            if (slider[2] == (HWND)lparam)
            {  // обработка события 3-ого trackbar
                wsprintfA(spos2, "%d", HIWORD(wparam));
                SetWindowTextA(hwnd, spos2);
            }
            break;
        }
        break;
    case WM_COMMAND:  // сообщение о команде
        if (lparam == (LPARAM)hBtn)    // если нажали на кнопку
        {
            TCHAR tmp[MAX_PATH];
            GetWindowText(hEdt1, tmp, GetWindowTextLength(hEdt1) + 1);
            SetWindowText(hStat, tmp); // выводим результат в статическое поле
            MessageBox(NULL, L"Вы изменили текст!", L"Информация", MB_OK);// системное модальное окно
        }
        if (lparam == (LPARAM)hBtn1)    // если нажали на кнопку
        {
            g_color = RGB(spos, spos1, spos2);
            InvalidateRect(hwnd, NULL, TRUE);
        }
        if (lparam == (LPARAM)hBtn2)    // если нажали на кнопку
        {
            PostQuitMessage(0);
        }
        if (wparam == IDM_EXIT)
        {
            PostQuitMessage(0);
        }
        if (HIWORD(wparam) == BN_CLICKED)
        {
            switch (LOWORD(wparam))
            {
            case ID_RED:
                g_color = RGB(242, 61, 61);
                break;
            case ID_GREEN:
                g_color = RGB(30, 217, 37);
                break;
            case ID_BLUE:
                g_color = RGB(30, 37, 217);
                break;
            }
            InvalidateRect(hwnd, NULL, TRUE);
        }
        break;
    case WM_PAINT: // перерисовка окна
        hdc = BeginPaint(hwnd, &ps); // начало перерисовки
        hBrush = CreateSolidBrush(g_color);
        hPen = CreatePen(PS_NULL, 1, RGB(0, 0, 0));
        holdPen = (HPEN)SelectObject(hdc, hPen);
        holdBrush = (HBRUSH)SelectObject(hdc, hBrush);
        TextOut(hdc, 230, 14, L"R", 1); // вывод текстовых сообщений
        TextOut(hdc, 230, 55, L"G", 1);
        TextOut(hdc, 230, 95, L"B", 1);
        Rectangle(hdc, 440, 10, 640, 140);
        SelectObject(hdc, holdBrush);
        SelectObject(hdc, holdPen);
        DeleteObject(hPen);
        DeleteObject(hBrush);
        EndPaint(hwnd, &ps); // конец перерисовки
        break;
    case WM_DESTROY: // закрытие окна
        PostQuitMessage(0);
        break;
    default: // обработка сообщения по умолчанию
        return DefWindowProc(hwnd, Message, wparam, lparam);
    }
    return 0;
}

void CreateMenubar(HWND hwnd)
{
    HMENU hMenubar;

    hMenubar = CreateMenu();
    AppendMenu(hMenubar, MF_POPUP, IDM_EXIT, TEXT("&Выход"));
    SetMenu(hwnd, hMenubar);
}