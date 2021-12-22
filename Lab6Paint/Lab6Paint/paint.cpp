#include "stdafx.h"
#include "paint.h"
#include <windows.h>

#define MAX_LOADSTRING 100


HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HDC hdc, realDC, virtualDC, saveVirtualDC, saveDC, bufDC1, bufDC2;
RECT rect,screen;
int  iWidthMM, iHeightMM, iWidthPels, iHeightPels;
static CHOOSECOLOR colorBox;
static COLORREF penColor;
HPEN hPen;
static int lineWidth = 1;
OPENFILENAME ofn;
char szFile[100];
static DWORD rgbCurrent;
PRINTDLG pd;
HWND hWnd;

ATOM				MyRegisterClass(HINSTANCE hInstance);
HWND				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_OSISPPAINTER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OSISPPAINTER));

	COLORREF acrCustClr[16]; // array of custom colors 

	ZeroMemory(&colorBox, sizeof(colorBox));
	colorBox.lStructSize = sizeof(colorBox);
	colorBox.hwndOwner = InitInstance(hInstance, nCmdShow);
	colorBox.lpCustColors = (LPDWORD)acrCustClr;
	colorBox.rgbResult = rgbCurrent;
	colorBox.Flags = CC_FULLOPEN | CC_RGBINIT;


	ZeroMemory(&pd, sizeof(PRINTDLG));
	pd.lStructSize = sizeof(PRINTDLG);
	pd.hwndOwner = InitInstance(hInstance, nCmdShow);
	pd.hDevMode = NULL;      	
	pd.hDevNames = NULL;    	
	pd.Flags = PD_USEDEVMODECOPIESANDCOLLATE | PD_RETURNDC;
	pd.nCopies = 1;
	pd.nFromPage = 0xFFFF;
	pd.nToPage = 0xFFFF;
	pd.nMinPage = 1;
	pd.nMaxPage = 0xFFFF;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OSISPPAINTER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_OSISPPAINTER);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}


HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; 

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return hWnd;
}


COLORREF ColorDialogCreate(HWND hwnd)
{
	if (ChooseColor(&colorBox))
	{
		return colorBox.rgbResult;
	}
	return NULL;
}

LPCWSTR GetFileName(LPCSTR flag){	
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof (ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = (LPWSTR)szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (flag == "open")
		GetOpenFileName(&ofn);
	else
	if (flag == "save")
		GetSaveFileName(&ofn);
	return (LPCWSTR)ofn.lpstrFile;
}

void SaveFile(LPCTSTR dest){
	RECT windowRect;
	GetClientRect(hWnd, &windowRect);

	// bitmap dimensions
	int bitmap_dx = windowRect.right - windowRect.left;
	int bitmap_dy = windowRect.bottom - windowRect.top;

	// create file
	HANDLE hFile = CreateFile(dest,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);

	// save bitmap file headers
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;

	fileHeader.bfType = 0x4d42;
	fileHeader.bfSize = 0;
	fileHeader.bfReserved1 = 0;
	fileHeader.bfReserved2 = 0;
	fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	infoHeader.biSize = sizeof(infoHeader);
	infoHeader.biWidth = bitmap_dx;
	infoHeader.biHeight = bitmap_dy;
	infoHeader.biPlanes = 1;
	infoHeader.biBitCount = 24;
	infoHeader.biCompression = BI_RGB;
	infoHeader.biSizeImage = 0;
	infoHeader.biXPelsPerMeter = 0;
	infoHeader.biYPelsPerMeter = 0;
	infoHeader.biClrUsed = 0;
	infoHeader.biClrImportant = 0;

	DWORD dwBytesWritten = 0;
	WriteFile(hFile, &fileHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, nullptr);
	WriteFile(hFile, &infoHeader, sizeof(BITMAPINFOHEADER), &dwBytesWritten, nullptr);


	BITMAPINFO info;
	info.bmiHeader = infoHeader;


	HDC winDC = GetWindowDC(hWnd);
	HDC memDC = CreateCompatibleDC(winDC);
	BYTE* memory = 0;
	HBITMAP bitmap = CreateDIBSection(winDC, &info, DIB_RGB_COLORS, (void**)&memory, 0, 0);
	SelectObject(memDC, bitmap);
	BitBlt(memDC, 0, 0, bitmap_dx, bitmap_dy, winDC, 0, 0, SRCCOPY);
	DeleteDC(memDC);
	ReleaseDC(hWnd, winDC);

	int bytes = (((24 * bitmap_dx + 31) & (~31)) / 8) * bitmap_dy;
	WriteFile(hFile, memory, bytes, &dwBytesWritten, nullptr);

	CloseHandle(hFile);
	DeleteObject(bitmap);
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	
	static POINT oldPoint, newPoint;	
	static int figureDrawIndex, xDelta = 0, yDelta = 0, key, delta;
	static double scaleMultiplier = 1.0;
	static BOOL isMouseDraw, isEMFwrite;
	int x, y;
	

	switch (message)
	{
	case WM_CREATE:
		ShowWindow(hWnd, SW_SHOWMAXIMIZED);
		GetClientRect(hWnd, &rect);
		realDC = GetDC(hWnd);
		virtualDC = CreateCompatibleDC(realDC);
		saveVirtualDC = CreateCompatibleDC(virtualDC);		
		FillRect(saveDC, &rect, WHITE_BRUSH);
		SelectObject(virtualDC, CreateCompatibleBitmap(realDC, rect.right, rect.bottom));
		SelectObject(saveVirtualDC, CreateCompatibleBitmap(realDC, rect.right, rect.bottom));
		FillRect(virtualDC, &rect, WHITE_BRUSH);
		FillRect(saveVirtualDC, &rect, WHITE_BRUSH);
		ReleaseDC(hWnd, realDC);
		break;
	
	
	case WM_LBUTTONDOWN:
		GetCursorPos(&oldPoint);
		ScreenToClient(hWnd, &oldPoint);
		isMouseDraw = true;
		oldPoint.x = (oldPoint.x * scaleMultiplier) + xDelta;
		oldPoint.y = (oldPoint.y * scaleMultiplier) + yDelta;
		MoveToEx(virtualDC, oldPoint.x, oldPoint.y, NULL);
		MoveToEx(saveDC, oldPoint.x, oldPoint.y, NULL);
		BitBlt(saveVirtualDC, 0, 0, rect.right, rect.bottom, virtualDC, 0, 0, SRCCOPY);
		InvalidateRect(hWnd, NULL, false);
		UpdateWindow(hWnd);
		break;
	case WM_LBUTTONUP:
		isMouseDraw = false;
		switch (figureDrawIndex)
		{
		case ID_TOOLS_PENCIL:

			break;
		case ID_TOOLS_LINE:			
			MoveToEx(saveDC, oldPoint.x, oldPoint.y, NULL);
			LineTo(saveDC, (newPoint.x * scaleMultiplier) + xDelta, (newPoint.y * scaleMultiplier) + yDelta);			
			break;
		default:
			break;
		}
		break;
		break;
	case WM_MOUSEMOVE:
		if (isMouseDraw)
		{
			switch (figureDrawIndex)
			{
			case ID_TOOLS_PENCIL:
				GetCursorPos(&newPoint);
				ScreenToClient(hWnd, &newPoint);
				x = newPoint.x * scaleMultiplier + xDelta;
				y = newPoint.y * scaleMultiplier + yDelta;				
				LineTo(virtualDC, x, y);
				LineTo(saveDC, x, y);
				oldPoint.x = x;
				oldPoint.y = y;

				InvalidateRect(hWnd, NULL, false);
				UpdateWindow(hWnd);
				break;
			case ID_TOOLS_LINE:				
				BitBlt(virtualDC, 0, 0, rect.right, rect.bottom, saveVirtualDC, 0, 0, SRCCOPY);
				GetCursorPos(&newPoint);
				ScreenToClient(hWnd, &newPoint);
				MoveToEx(virtualDC, oldPoint.x, oldPoint.y, NULL);
				LineTo(virtualDC, (newPoint.x * scaleMultiplier) + xDelta, (newPoint.y * scaleMultiplier) + yDelta);
				InvalidateRect(hWnd, NULL, false);
				UpdateWindow(hWnd);
				break;
			case ID_TOOLS_RECT:
				BitBlt(virtualDC, 0, 0, rect.right, rect.bottom, saveVirtualDC, 0, 0, SRCCOPY);
				GetCursorPos(&newPoint);
				ScreenToClient(hWnd, &newPoint);
				MoveToEx(virtualDC, oldPoint.x, oldPoint.y, NULL);
				Rectangle(virtualDC, oldPoint.x, oldPoint.y, (newPoint.x * scaleMultiplier) + xDelta, (newPoint.y * scaleMultiplier) + yDelta);
				InvalidateRect(hWnd, NULL, false);
				UpdateWindow(hWnd);
				break;
			case ID_TOOLS_ELLIPSE:
				BitBlt(virtualDC, 0, 0, rect.right, rect.bottom, saveVirtualDC, 0, 0, SRCCOPY);
				GetCursorPos(&newPoint);
				ScreenToClient(hWnd, &newPoint);
				MoveToEx(virtualDC, oldPoint.x, oldPoint.y, NULL);
				Ellipse(virtualDC, oldPoint.x, oldPoint.y, (newPoint.x * scaleMultiplier) + xDelta, (newPoint.y * scaleMultiplier) + yDelta);
				InvalidateRect(hWnd, NULL, false);
				UpdateWindow(hWnd);
				break;
			default:
				break;
			}
		}
		break;
		
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_TOOLS_PENCIL:
			figureDrawIndex = ID_TOOLS_PENCIL;			
			break;
		case ID_TOOLS_LINE:
			figureDrawIndex = ID_TOOLS_LINE;			
			break;
		case ID_TOOLS_RECT:
			figureDrawIndex = ID_TOOLS_RECT;
			break;
		case ID_TOOLS_ELLIPSE:
			figureDrawIndex = ID_TOOLS_ELLIPSE;
			break;
		case ID_SETTINGS_COLOR:
			penColor = ColorDialogCreate(hWnd);
			hPen = CreatePen(PS_SOLID, lineWidth, penColor);
			DeleteObject(SelectObject(virtualDC, hPen));
			DeleteObject(SelectObject(saveDC, hPen));			
			break;
		case ID_LINEWIDTH_SMALL:
			lineWidth = 1;
			hPen = CreatePen(PS_SOLID, lineWidth, penColor);
			DeleteObject(SelectObject(virtualDC, hPen));
			DeleteObject(SelectObject(saveDC, hPen));
			break;
		case ID_LINEWIDTH_MEDIUM:
			lineWidth = 5;
			hPen = CreatePen(PS_SOLID, lineWidth, penColor);
			DeleteObject(SelectObject(virtualDC, hPen));
			DeleteObject(SelectObject(saveDC, hPen));
			break;
		case ID_LINEWIDTH_LARGE:
			lineWidth = 10;
			hPen = CreatePen(PS_SOLID, lineWidth, penColor);
			DeleteObject(SelectObject(virtualDC, hPen));
			DeleteObject(SelectObject(saveDC, hPen));
			break;
		case ID_FILE_SAVE:			
			OPENFILENAME ofn;
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFilter = _T("Bitmap Files (*.bmp)\0*.bmp\0All Files (*.*)\0*.*\0");

			TCHAR fileName[MAX_PATH] = { 0, };
			ofn.lpstrFile = (LPWSTR)fileName;
			ofn.nMaxFile = MAX_PATH;
			ofn.Flags = OFN_OVERWRITEPROMPT;
			ofn.lpstrInitialDir = nullptr;
			ofn.lpstrDefExt = (LPCWSTR)L"txt";
			if (GetSaveFileName(&ofn) != FALSE)
			{
				SaveFile(ofn.lpstrFile);
			}
			break;
		
		}
		break;
	case WM_PAINT:
		realDC = BeginPaint(hWnd, &ps);
		StretchBlt(realDC, 0, 0, rect.right, rect.bottom, virtualDC, xDelta, yDelta,
			rect.right * scaleMultiplier, rect.bottom * scaleMultiplier, SRCCOPY);		
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

