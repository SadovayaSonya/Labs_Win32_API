#include <windows.h>
#include <tchar.h>

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildProc(HWND, UINT, WPARAM, LPARAM);
/*  Make the class name into a global variable  */
TCHAR szClassName[] = L"WindowsApp";
TCHAR szClassName2[] = L"Window";

HINSTANCE hInstance;

HWND child;

int WINAPI WinMain
(
    HINSTANCE hThisInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpszArgument,
    int nFunsterStil
)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof(WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default color as the background of the window */
    wincl.hbrBackground = CreateHatchBrush(HS_VERTICAL, RGB(220, 30, 200));

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx(&wincl))
        return 0;
    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx
    (
        0,                   /* Extended possibilites for variation */
        szClassName,         /* Classname */
        L"Windows App",       /* Title Text */
        WS_OVERLAPPEDWINDOW, /* default window */
        CW_USEDEFAULT,       /* Windows decides the position */
        CW_USEDEFAULT,       /* where the window ends up on the screen */
        600,                 /* The programs width */
        600,                 /* and height in pixels */
        HWND_DESKTOP,        /* The window is a child-window to desktop */
        NULL,                /* No menu */
        hThisInstance,       /* Program Instance handler */
        NULL                 /* No Window Creation data */
    );
    ShowWindow(hwnd, nFunsterStil);

    WNDCLASSEX wchcl;

    TCHAR szClassName2[] = L"Mini Window";
    wchcl.cbSize = sizeof(wchcl);
    wchcl.style = CS_HREDRAW | CS_HREDRAW;
    wchcl.lpfnWndProc = ChildProc;
    wchcl.cbClsExtra = 0;
    wchcl.cbWndExtra = 0;
    wchcl.hInstance = hThisInstance;
    wchcl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wchcl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wchcl.hbrBackground = CreateSolidBrush(RGB(5, 102, 255));;
    wchcl.lpszMenuName = NULL;
    wchcl.lpszClassName = szClassName2;
    wchcl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    if (!RegisterClassEx(&wchcl))
        return 0;

    child = CreateWindowEx
    (
        0,                   /* Extended possibilites for variation */
        szClassName2,         /* Classname */
        L"Mini Window",       /* Title Text */
        WS_OVERLAPPEDWINDOW | WS_CHILD, /* default window */
        200,       /* Windows decides the position */
        190,       /* where the window ends up on the screen */
        200,                 /* The programs width */
        200,                 /* and height in pixels */
        hwnd,        /* The window is a child-window to desktop */
        NULL,                /* No menu */
        hThisInstance,       /* Program Instance handler */
        NULL                 /* No Window Creation data */
    );
    ShowWindow(hwnd, SW_SHOW);
    ShowWindow(child, SW_HIDE);

    /* Make the window visible on the screen */

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage(&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message)                  /* handle the messages */
    {
    case WM_CREATE:
        SetTimer(hwnd, 0, 1000, NULL);
        break;
    case WM_TIMER:
        ShowWindow(child, SW_SHOW);
        KillTimer(hwnd, 0);
        break;
    case WM_LBUTTONUP:
        CloseWindow(child);
        break;
    case WM_RBUTTONUP:
        ShowWindow(child, SW_RESTORE);
        break;
    case WM_KEYDOWN:
        if (wParam == VK_RETURN)
        {
            HDC hdc;
            hdc = GetDC(child);
            // Выводим текстовую строку
            TextOut(hdc, 10, 40, L"Height: 200; width: 200!", 25);
            // Отдаем индекс контекста устройства
            ReleaseDC(child, hdc);
            return 0;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);       /* send a WM_QUIT to the message queue */
        break;
    default:                      /* for messages that we don't deal with */
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK ChildProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
    case WM_DESTROY:
        SendMessage(child, WM_CLOSE, 0, 0);
        break;
    default:                      /* for messages that we don't deal with */
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}
