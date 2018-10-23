// Win32Table.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "Win32Table.h"


#define MAX_LOADSTRING 100
#define FILE_NAME "D:\\LR\\sem5(NOW)\\OSISP\\LR2\\Lab2\\Win32Table\\Debug\\2.csv"  

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

vector <vector<string> > textTable;
int minWidth;
CFont *m_pFont = new CFont;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void                LoadCSVTable(vector<vector<string> > &, const char *);
void                DrawTable(HDC, HWND, const vector<vector<string> > &);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: разместите код здесь.

	// Инициализация глобальных строк
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_WIN32TABLE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32TABLE));

	MSG msg;

	//если -1, то обработать
	// Цикл основного сообщения:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  НАЗНАЧЕНИЕ: регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32TABLE));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

void LoadCSVTable(vector<vector<string> > &textTable, const char *fileName)
{
	std::ifstream file(fileName);
	string line;
	string cell;
	vector<string> row;
	while (!file.eof())
	{
		std::getline(file, line);
		std::stringstream lineStream(line);
		row.clear();
		while (std::getline(lineStream, cell, ','))
		{
			row.push_back(cell);
		}
		/*if (!lineStream && cell.empty())
		{
			row.push_back("");
		}*/
		textTable.push_back(row);
	}
	textTable.pop_back();
}


//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   НАЗНАЧЕНИЕ: сохраняет обработку экземпляра и создает главное окно.
//
//   КОММЕНТАРИИ:
//
//        В данной функции дескриптор экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится на экран главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменной

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	LoadCSVTable(textTable, FILE_NAME);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

vector<int> CalcWidths(const vector<vector<string> > &textTable, const RECT clientRect, int padding, int borderSize)
{
	size_t maxRowSize = 0;
	for (size_t i = 0; i < textTable.size(); i++)
		if (maxRowSize < textTable[i].size())
			maxRowSize = textTable[i].size();
	vector<int> widths;
	widths.resize(maxRowSize);
	int width = (clientRect.right - clientRect.left - borderSize) / maxRowSize - padding;
	for (size_t j = 0; j < maxRowSize; j++)
		widths[j] = width;
	widths[maxRowSize - 1] += clientRect.right - clientRect.left - borderSize - (width + padding)*maxRowSize;
	return widths;
}

vector<int> CalcHeights(HDC hdc, const vector<vector<string> > &textTable, const vector<int> &widths)
{
	vector<int> heights;
	heights.resize(textTable.size());
	for (size_t i = 0; i < textTable.size(); i++)
	{
		int maxCellHeight = 0;
		for (size_t j = 0; j < textTable[i].size(); j++)
		{
			std::wstring stemp = std::wstring(textTable[i][j].begin(), textTable[i][j].end());
			LPCWSTR text = stemp.c_str();
			RECT rect = { 0, 0, widths[j], 0 };
			int cellHeight;
			if ((cellHeight = DrawText(hdc, text, -1, &rect, DT_CALCRECT | DT_WORDBREAK)) > maxCellHeight)
				maxCellHeight = cellHeight;
		}
		heights[i] = maxCellHeight;
	}
	return heights;
}


void DrawBorders(HDC hdc, const vector<int> &widths, const vector<int> &heights, const int padding, const int borderSize)
{
	int x = 0, y = 0;
	for (size_t i = 0; i < heights.size(); i++)
	{
		x = 0;
		for (size_t j = 0; j < widths.size(); j++)
		{
			RECT rect = { x, y, x + widths[j] + 2 * padding, y + heights[i] + 2 * padding };
			FrameRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
			x += widths[j] + 2 * padding - borderSize;
		}
		y += heights[i] + 2 * padding - borderSize;
	}
}


void DrawTextTable(HDC hdc, const vector<int> &widths, const vector<int> &heights, const int padding, const int borderSize)
{
	HFONT hFont;
	int maxWidth = 30;
	int minWidth = 5;
	int y = borderSize + padding;
	for (size_t i = 0; i < textTable.size(); i++) 
	{
		int x = borderSize + padding;
		for (size_t j = 0; j < textTable[i].size(); j++)
		{
			int letterWidth = 0;
			if (textTable[i][j].length() != 0) {
				letterWidth = (int)((widths[j] - 2* borderSize - 2*padding) / textTable[i][j].length());
				if (letterWidth > maxWidth) {
					letterWidth = maxWidth;
				}
			}
			//int letterHeight = 0;
			int letterHeight = heights[i] - 2 * borderSize - 2 * padding;

			hFont = CreateFont(letterHeight, letterWidth, 10, 0, FW_DONTCARE, FALSE, FALSE,
				FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
				CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, 
				TEXT("Times New Roman"));
			SelectObject(hdc, hFont);
			std::wstring stemp = std::wstring(textTable[i][j].begin(), textTable[i][j].end());
			LPCWSTR text = stemp.c_str();
			RECT rect = { x, y, x + widths[j], y + heights[i]};
			DrawText(hdc, text, -1, &rect, DT_CENTER | DT_WORDBREAK);
			x += widths[j] + 2 * padding - borderSize;
			DeleteObject(hFont);
		}
		y += heights[i] + 2 * padding - borderSize;
	}	
}


void DrawTable(HDC hdc, HWND hWnd, const vector<vector<string> > &textTable)
{
	int borderSize = 1;
	int padding = 2;
	RECT clientRect = { 0, 0, 0, 0 };
	if (!GetClientRect(hWnd, &clientRect))
		return;
	vector<int> widths = CalcWidths(textTable, clientRect, padding, borderSize);
	vector<int> heights = CalcHeights(hdc, textTable, widths);
	DrawBorders(hdc, widths, heights, padding, borderSize);
	DrawTextTable(hdc, widths, heights, padding, borderSize);
}


//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  НАЗНАЧЕНИЕ:  обрабатывает сообщения в главном окне.
//
//  WM_PAINT — отрисовать главное окно
//  WM_DESTROY — отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		DrawTable(hdc, hWnd, textTable);
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
