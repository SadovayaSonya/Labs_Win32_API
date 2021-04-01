#include <windows.h> // заголовочный файл, содержащий функции API
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

static TCHAR szWindowClass[] = L"SystemSoftware"; // строка с именем класса
static TCHAR szTitle[] = L"First Practice in System Software";

HINSTANCE hInst;

// Прототип функции обработки сообщений с пользовательским названием:
LRESULT CALLBACK WndProc(HWND hWnd, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildWndProc(HWND child, UINT, WPARAM, LPARAM);
LRESULT CALLBACK Child1WndProc(HWND child1, UINT, WPARAM, LPARAM);

int WINAPI WinMain // основная функция - аналог int main()
(
    HINSTANCE hInstance, // дескриптор экземпляра приложения
    HINSTANCE hPrevInstance, // в Win32 не используется
    LPSTR lpCmdLine, // нужен для запуска окна в режиме командной строки
    int nCmdShow // режим отображения окна
) 
{
    WNDCLASSEX wc; // создание экземпляра для обращения к членам класса WNDCLASSEX
    wc.cbSize = sizeof(WNDCLASSEX); // размер структуры (в байтах)
    wc.style = CS_HREDRAW | CS_VREDRAW; // стиль класса окошка
    wc.lpfnWndProc = WndProc; // указатель на пользовательскую функцию
    wc.cbClsExtra = 0; // число освобождаемых байтов в конце структуры
    wc.cbWndExtra = 0; // число освобождаемых байтов при создании экземпляра приложения
    wc.hInstance = hInstance; // указатель на строку, содержащую имя меню, применяемого для класса
    wc.hIcon = LoadIcon(hInstance, IDI_APPLICATION); // декриптор пиктограммы
    wc.hCursor = LoadCursor(NULL, IDC_ARROW); // дескриптор курсора
    wc.hbrBackground = CreateSolidBrush(RGB(51, 153, 103)); // дескриптор кисти для закраски фона окна
    wc.lpszMenuName = NULL; // указатель на имя меню
    wc.lpszClassName = szWindowClass; // указатель на имя класса
    wc.hIconSm = LoadIcon(wc.hInstance, IDI_APPLICATION); // дескриптор маленькой пиктограммы (в трэе)

    if (!RegisterClassEx(&wc)) {
        // в случае отсутствия регистрации класса:
        MessageBox(NULL, L"Не получилось зарегистрировать класс!", L"Ошибка", MB_OK);
        return NULL; // выход из WinMain
    }

    hInst = hInstance;
    
    HWND hWnd; // создание дескриптора будущего окошка
    hWnd = CreateWindow // функция, создающая окошко
    (
        szWindowClass, // имя класса
        szTitle, // имя окна
        WS_OVERLAPPEDWINDOW, // режимы отображения окошка (WS_VSCROLL)
        CW_USEDEFAULT, // положение окна по оси х (по умолчанию)
        CW_USEDEFAULT, // положение окна по оси у (по умолчанию)
        800, 600, // высота и ширина окна
        NULL, // дескриптор родительского окошка
        NULL, // дескриптор меню
        hInstance, // дескриптор экземпляра приложения
        NULL // ничего не передается из WndProc
    );
    
    if (!hWnd) {
        // в случае некорректного создания окна (неверные параметры и т. п.):
        MessageBox(NULL, L"Не получилось создать окно!", L"Ошибка", MB_OK);
        return NULL; // выход из WinMain
    }
    
    ShowWindow(hWnd, nCmdShow); // отображение окошка
    UpdateWindow(hWnd); // обновление окошка
    
    WNDCLASSEX wcc; // создание экземпляра для обращения к членам класса WNDCLASSEX
    wcc.cbSize = sizeof(WNDCLASSEX); // размер структуры (в байтах)
    wcc.style = CS_HREDRAW | CS_VREDRAW; // стиль класса окошка
    wcc.lpfnWndProc = ChildWndProc; // указатель на пользовательскую функцию
    wcc.cbClsExtra = 0; // число освобождаемых байтов в конце структуры
    wcc.cbWndExtra = 0; // число освобождаемых байтов при создании экземпляра приложения
    wcc.hInstance = hInstance; // указатель на строку, содержащую имя меню, применяемого для класса
    wcc.hIcon = LoadIcon(hInstance, IDI_APPLICATION); // декриптор пиктограммы
    wcc.hCursor = LoadCursor(NULL, IDC_ARROW); // дескриптор курсора
    wcc.hbrBackground = CreateSolidBrush(RGB(51, 102, 255)); // дескриптор кисти для закраски фона окна
    wcc.lpszMenuName = NULL; // указатель на имя меню
    wcc.lpszClassName = L"Window"; // указатель на имя класса
    wcc.hIconSm = LoadIcon(wc.hInstance, IDI_APPLICATION); // дескриптор маленькой пиктограммы (в трэе)

    if (!RegisterClassEx(&wcc)) {
        // в случае отсутствия регистрации класса:
        MessageBox(NULL, L"Не получилось зарегистрировать класс!", L"Ошибка", MB_OK);
        return NULL; // выход из WinMain
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
        // в случае некорректного создания окна (неверные параметры и т. п.):
        MessageBox(NULL, L"Не получилось дочернее создать окно!", L"Ошибка", MB_OK);
        return NULL; // выход из WinMain
    }

    ShowWindow(child, nCmdShow); // отображение окошка
    SetParent(child, hWnd);

    WNDCLASSEX wcc1; // создание экземпляра для обращения к членам класса WNDCLASSEX
    wcc1.cbSize = sizeof(WNDCLASSEX); // размер структуры (в байтах)
    wcc1.style = CS_HREDRAW | CS_VREDRAW; // стиль класса окошка
    wcc1.lpfnWndProc = ChildWndProc; // указатель на пользовательскую функцию
    wcc1.cbClsExtra = 0; // число освобождаемых байтов в конце структуры
    wcc1.cbWndExtra = 0; // число освобождаемых байтов при создании экземпляра приложения
    wcc1.hInstance = hInstance; // указатель на строку, содержащую имя меню, применяемого для класса
    wcc1.hIcon = LoadIcon(hInstance, IDI_APPLICATION); // декриптор пиктограммы
    wcc1.hCursor = LoadCursor(NULL, IDC_ARROW); // дескриптор курсора
    wcc1.hbrBackground = CreateSolidBrush(RGB(0, 255, 255)); // дескриптор кисти для закраски фона окна
    wcc1.lpszMenuName = NULL; // указатель на имя меню
    wcc1.lpszClassName = L"Window 2"; // указатель на имя класса
    wcc1.hIconSm = LoadIcon(wc.hInstance, IDI_APPLICATION); // дескриптор маленькой пиктограммы (в трэе)

    if (!RegisterClassEx(&wcc1)) {
        // в случае отсутствия регистрации класса:
        MessageBox(NULL, L"Не получилось зарегистрировать класс!", L"Ошибка", MB_OK);
        return NULL; // выход из WinMain
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
        // в случае некорректного создания окна (неверные параметры и т. п.):
        MessageBox(NULL, L"Не получилось создать дочернее окно!", L"Ошибка", MB_OK);
        return NULL; // выход из WinMain
    }

    ShowWindow(child1, nCmdShow); // отображение окошка
    SetParent(child1, hWnd);

    MSG msg; // создание экземпляра структуры MSG для обработки сообщений
    
    while (GetMessage(&msg, NULL, 0, 0)) { // извлечение сообщения из очереди, посылаемые фу-циями, ОС
        TranslateMessage(&msg); // интерпретирование сообщений
        DispatchMessage(&msg); // передача сообщения обратно в ОС
    }
    return (int)msg.wParam; // возврат кода выхода из приложения
}

LRESULT CALLBACK WndProc
(
    HWND hWnd, // дескриптор окошка
    UINT message, // сообщение, посылаемое ОС
    WPARAM wParam, LPARAM lParam // параметры сообщений для последующего обращения
)
{
    PAINTSTRUCT ps; // структура, содержащая инф. о клиентской обл. (размеры, цвет и т. д.)
    HDC hdc; // создание дескриптора ориентации текста на экране

    switch (message)
    {
    case WM_DESTROY: // при закрытии окна - делать:
        PostQuitMessage(0); // отправка WinMain() сообщения WM_QUIT
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam); // при закрытии окна
        break;
    }
    return 0; // возврат значения
}

LRESULT CALLBACK ChildWndProc
(
    HWND child, // дескриптор окошка
    UINT message, // сообщение, посылаемое ОС
    WPARAM wParam, LPARAM lParam // параметры сообщений для последующего обращения
)
{
    PAINTSTRUCT ps; // структура, содержащая инф. о клиентской обл. (размеры, цвет и т. д.)
    HDC hdc; // создание дескриптора ориентации текста на экране

    switch (message)
    {
    case WM_DESTROY: // при закрытии окна - делать:
        return 0;
        break;
    default:
        return DefWindowProc(child, message, wParam, lParam); // при закрытии окна
        break;
    }
    return 0; // возврат значения
}