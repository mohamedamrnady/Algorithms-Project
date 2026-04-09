#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

#include <algorithm>
#include <string>

#include "Board.hpp"

namespace
{
    constexpr int kMinN = 1; // 2x2
    constexpr int kMaxN = 4; // 16x16 (keeps brute-force coloring responsive)

    constexpr int kTopBarHeight = 44;
    constexpr int kPadding = 12;

    constexpr int kBtnW = 32;
    constexpr int kBtnH = 24;
    constexpr int kBtnGap = 8;

    constexpr int ID_BTN_MINUS = 1001;
    constexpr int ID_BTN_PLUS = 1002;

    struct AppState
    {
        int n = 1;
        int missing_x = 0;
        int missing_y = 0;
        Board *board = nullptr;
        HWND btnMinus = nullptr;
        HWND btnPlus = nullptr;
    };

    AppState g;

    static int clampInt(int v, int lo, int hi)
    {
        return (v < lo) ? lo : (v > hi) ? hi
                                        : v;
    }

    static std::wstring makeTitle()
    {
        const int size = 1 << g.n;
        return L"Task01 Tromino Tiling (" + std::to_wstring(size) + L"x" + std::to_wstring(size) + L")";
    }

    struct GridLayout
    {
        bool ok = false;
        int startX = 0;
        int startY = 0;
        int cell = 0;
        int gridW = 0;
        int gridH = 0;
        int boardSize = 0; // number of cells per side
        RECT boardArea{};
    };

    static GridLayout computeGridLayout(const RECT &client)
    {
        GridLayout gl;
        if (!g.board)
            return gl;

        const int w = g.board->getWidth();
        if (w <= 0)
            return gl;

        RECT boardArea = client;
        boardArea.top += kTopBarHeight;
        gl.boardArea = boardArea;

        const int areaW = std::max(0L, boardArea.right - boardArea.left - 2 * kPadding);
        const int areaH = std::max(0L, boardArea.bottom - boardArea.top - 2 * kPadding);
        if (areaW <= 0 || areaH <= 0)
            return gl;

        const int cell = std::max(1, std::min(areaW / w, areaH / w));
        const int gridW = cell * w;
        const int gridH = cell * w;

        const int startX = boardArea.left + kPadding + (areaW - gridW) / 2;
        const int startY = boardArea.top + kPadding + (areaH - gridH) / 2;

        gl.ok = true;
        gl.startX = startX;
        gl.startY = startY;
        gl.cell = cell;
        gl.gridW = gridW;
        gl.gridH = gridH;
        gl.boardSize = w;
        return gl;
    }

    static void positionButtons(HWND hwnd)
    {
        RECT client;
        GetClientRect(hwnd, &client);

        const int topY = 10;
        const int plusX = (int)client.right - kPadding - kBtnW;
        const int minusX = plusX - kBtnGap - kBtnW;

        if (g.btnMinus)
            SetWindowPos(g.btnMinus, nullptr, minusX, topY, kBtnW, kBtnH, SWP_NOZORDER);
        if (g.btnPlus)
            SetWindowPos(g.btnPlus, nullptr, plusX, topY, kBtnW, kBtnH, SWP_NOZORDER);
    }

    static void recreateBoard(HWND hwnd)
    {
        delete g.board;
        g.board = nullptr;

        const int size = 1 << g.n;
        g.missing_x = clampInt(g.missing_x, 0, size - 1);
        g.missing_y = clampInt(g.missing_y, 0, size - 1);

        g.board = new Board(g.n, g.missing_x, g.missing_y);
        g.board->initializeBoardWithTrominos();

        SetWindowTextW(hwnd, makeTitle().c_str());
        InvalidateRect(hwnd, nullptr, TRUE);
    }

    static COLORREF colorFromChar(char c)
    {
        switch (c)
        {
        case 'R':
            return RGB(220, 60, 60);
        case 'G':
            return RGB(60, 190, 90);
        case 'B':
            return RGB(70, 120, 230);
        case 'W':
            return RGB(245, 245, 245);
        default:
            return RGB(200, 200, 200);
        }
    }

    static void drawBoard(HDC hdc, const RECT &client)
    {
        if (!g.board)
            return;

        const GridLayout gl = computeGridLayout(client);
        if (!gl.ok)
            return;

        // Light gray background for the board area.
        {
            HBRUSH bg = CreateSolidBrush(RGB(250, 250, 250));
            FillRect(hdc, &gl.boardArea, bg);
            DeleteObject(bg);
        }

        // Draw squares.
        for (int y = 0; y < gl.boardSize; ++y)
        {
            for (int x = 0; x < gl.boardSize; ++x)
            {
                const char c = g.board->getColorAt(x, y);
                const COLORREF fill = colorFromChar(c);

                RECT r;
                r.left = gl.startX + x * gl.cell;
                r.top = gl.startY + y * gl.cell;
                r.right = r.left + gl.cell;
                r.bottom = r.top + gl.cell;

                HBRUSH b = CreateSolidBrush(fill);
                FillRect(hdc, &r, b);
                DeleteObject(b);

                // Cell border
                FrameRect(hdc, &r, (HBRUSH)GetStockObject(GRAY_BRUSH));
            }
        }

        // Legend / info text.
        std::wstring info = L"n=" + std::to_wstring(g.n) + L"  (2^n x 2^n), missing=(" +
                            std::to_wstring(g.missing_x) + L"," + std::to_wstring(g.missing_y) + L")";
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(20, 20, 20));
        TextOutW(hdc, client.left + kPadding, client.top + 12, info.c_str(), (int)info.size());
    }

    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_CREATE:
        {
            g.btnMinus = CreateWindowW(L"BUTTON", L"-", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                       12, 10, kBtnW, kBtnH, hwnd, (HMENU)ID_BTN_MINUS, nullptr, nullptr);
            g.btnPlus = CreateWindowW(L"BUTTON", L"+", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                      52, 10, kBtnW, kBtnH, hwnd, (HMENU)ID_BTN_PLUS, nullptr, nullptr);

            positionButtons(hwnd);

            recreateBoard(hwnd);
            return 0;
        }

        case WM_SIZE:
            positionButtons(hwnd);
            InvalidateRect(hwnd, nullptr, TRUE);
            return 0;

        case WM_COMMAND:
        {
            const int id = LOWORD(wParam);
            if (id == ID_BTN_MINUS)
            {
                g.n = std::max(kMinN, g.n - 1);
                recreateBoard(hwnd);
                return 0;
            }
            if (id == ID_BTN_PLUS)
            {
                g.n = std::min(kMaxN, g.n + 1);
                recreateBoard(hwnd);
                return 0;
            }
            break;
        }

        case WM_LBUTTONDOWN:
        {
            // Click a cell to set the missing square.
            RECT client;
            GetClientRect(hwnd, &client);
            const GridLayout gl = computeGridLayout(client);
            if (!gl.ok)
                return 0;

            const int px = GET_X_LPARAM(lParam);
            const int py = GET_Y_LPARAM(lParam);

            if (px < gl.startX || py < gl.startY || px >= gl.startX + gl.gridW || py >= gl.startY + gl.gridH)
                return 0;

            const int x = (px - gl.startX) / gl.cell;
            const int y = (py - gl.startY) / gl.cell;
            if (x < 0 || x >= gl.boardSize || y < 0 || y >= gl.boardSize)
                return 0;

            g.missing_x = x;
            g.missing_y = y;
            recreateBoard(hwnd);
            return 0;
        }

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT client;
            GetClientRect(hwnd, &client);

            // Window background
            HBRUSH bg = CreateSolidBrush(RGB(255, 255, 255));
            FillRect(hdc, &client, bg);
            DeleteObject(bg);

            drawBoard(hdc, client);

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_DESTROY:
            delete g.board;
            g.board = nullptr;
            PostQuitMessage(0);
            return 0;

        default:
            break;
        }
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

} // namespace

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    const wchar_t kClassName[] = L"TrominoBoardWindow";

    WNDCLASSW wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = kClassName;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    if (!RegisterClassW(&wc))
        return 1;

    HWND hwnd = CreateWindowExW(0, kClassName, makeTitle().c_str(),
                                WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                                820, 880, nullptr, nullptr, hInstance, nullptr);

    if (!hwnd)
        return 1;

    ShowWindow(hwnd, nCmdShow);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}
