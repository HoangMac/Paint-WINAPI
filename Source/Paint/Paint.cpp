// Paint.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Paint.h"
#include <windowsx.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PAINT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PAINT));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PAINT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PAINT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//


#include <iostream>
#include <vector>
using namespace std;

#define LINE 0
#define RECTANGLE 1
#define ELLIPSE 2


class CPoint
{
public:
	int x;
	int y;
};

class CShape 
{
public:
	CPoint p1;
	CPoint p2;

	virtual void Draw(HDC hdc) = 0;
	virtual CShape* Create() = 0;
	virtual void SetData(CPoint a, CPoint b) = 0;
};

class CLine : public CShape
{
public:
	void Draw(HDC hdc) 
	{
		MoveToEx(hdc, p1.x, p1.y, NULL);
		LineTo(hdc, p2.x, p2.y);
	}

	CShape* Create() { return new CLine; }

	void SetData(CPoint a, CPoint b) 
	{
		p1 = a;
		p2 = b;
	}
};

class CRectangle : public CShape
{
public:
	void Draw(HDC hdc) { Rectangle(hdc, p1.x, p1.y, p2.x, p2.y); }
		
	CShape* Create() { return new CRectangle; }

	void SetData(CPoint a, CPoint b) 
	{
		p1 = a;
		p2 = b;
	}
};

class CEllipse : public CShape
{
public:
	void Draw(HDC hdc) { Ellipse(hdc, p1.x, p1.y, p2.x, p2.y); }

	CShape* Create() { return new CEllipse; }

	void SetData(CPoint a, CPoint b) 
	{
		p1 = a;
		p2 = b;
	}
};

vector<CShape*> Shapes;
vector<CShape*> Prototypes;

int ShapeSelect;
BOOL isDrawing = FALSE;

CPoint startP;
CPoint currentP;
CPoint endP;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		Prototypes.push_back(new CLine);
		Prototypes.push_back(new CRectangle);
		Prototypes.push_back(new CEllipse);
		ShapeSelect = 0;
		break;

	case WM_LBUTTONDOWN:
		startP.x = GET_X_LPARAM(lParam);
		startP.y = GET_Y_LPARAM(lParam);
		
		if (!isDrawing)
			isDrawing = TRUE;
		break;

	case WM_MOUSEMOVE:
		{
			currentP.x = GET_X_LPARAM(lParam);
			currentP.y = GET_Y_LPARAM(lParam);

			if (isDrawing)
			{
				BOOL shft = GetKeyState(VK_SHIFT) & 0x800;
				if (ShapeSelect == RECTANGLE || ShapeSelect == ELLIPSE)
				{
					if (!shft)
					{
						if (ShapeSelect == RECTANGLE)
							Prototypes[RECTANGLE]->SetData(startP, currentP);
						else Prototypes[ELLIPSE]->SetData(startP, currentP);
					}
					else
					{
						CPoint newP1, newP2;

						newP1.x = min(startP.x, currentP.x);
						newP1.y = min(startP.y, currentP.y);
						newP2.x = max(startP.x, currentP.x);
						newP2.y = max(startP.y, currentP.y);

						int length = min(abs(currentP.x - startP.x), abs(currentP.y - startP.y));
						if (currentP.x < startP.x)
							newP1.x = newP2.x - length;
						else newP2.x = newP1.x + length;

						if (currentP.y < startP.y)
							newP1.y = newP2.y - length;
						else newP2.y = newP1.y + length;

						if (ShapeSelect == RECTANGLE)
							Prototypes[RECTANGLE]->SetData(newP1, newP2);
						else Prototypes[ELLIPSE]->SetData(newP1, newP2);
					}
				}
				else Prototypes[LINE]->SetData(startP, currentP);

				WCHAR buffer[200];
				wsprintf(buffer, L"%d : %d", currentP.x, currentP.y);
				SetWindowText(hWnd, buffer);
			}

			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
		
	case WM_LBUTTONUP:
		{
			endP.x = GET_X_LPARAM(lParam);
			endP.y = GET_Y_LPARAM(lParam);

			isDrawing = FALSE;
			BOOL shft = GetKeyState(VK_SHIFT) & 0x800;
			CShape *shape;


			if (ShapeSelect == RECTANGLE)
				shape = new CRectangle;
			else if (ShapeSelect == ELLIPSE)
				shape = new CEllipse;
			else shape = new CLine;

			if (ShapeSelect == RECTANGLE || ShapeSelect == ELLIPSE)
			{
				if (shft)
				{
					CPoint newP1, newP2;

					newP1.x = min(startP.x, currentP.x);
					newP1.y = min(startP.y, currentP.y);
					newP2.x = max(startP.x, currentP.x);
					newP2.y = max(startP.y, currentP.y);

					int length = min(abs(currentP.x - startP.x), abs(currentP.y - startP.y));
					if (currentP.x < startP.x)
						newP1.x = newP2.x - length;
					else newP2.x = newP1.x + length;

					if (currentP.y < startP.y)
						newP1.y = newP2.y - length;
					else newP2.y = newP1.y + length;

					shape->SetData(newP1, newP2);
					Shapes.push_back(shape);
					InvalidateRect(hWnd, NULL, TRUE);
					break;
				}
			}
			shape->SetData(startP, endP);
			Shapes.push_back(shape);
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;

	case WM_COMMAND:
		{
			HMENU hMenu = GetMenu(hWnd);
			int wmId = LOWORD(wParam);
			BOOL isChecked;
			// Parse the menu selections:
			switch (wmId)
			{
			case ID_LINE:
				CheckMenuItem(hMenu, ID_LINE, MF_CHECKED);
				ShapeSelect = LINE;
				break;

			case ID_RECTANGLE:
				CheckMenuItem(hMenu, ID_RECTANGLE, MF_CHECKED);
				ShapeSelect = RECTANGLE;
				break;

			case ID_ELLIPSE:
				CheckMenuItem(hMenu, ID_ELLIPSE, MF_CHECKED);
				ShapeSelect = ELLIPSE;
				break;

			case IDM_ABOUT:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;

			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;

			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;

	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);

			for (int i = 0; i < Shapes.size(); i++)
				Shapes[i]->Draw(hdc);

			if (ShapeSelect == RECTANGLE)
				Prototypes[1]->Draw(hdc);
			else if (ShapeSelect == ELLIPSE)
				Prototypes[2]->Draw(hdc);
			else Prototypes[0]->Draw(hdc);

			EndPaint(hWnd, &ps);
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
    return (INT_PTR)FALSE;
}

