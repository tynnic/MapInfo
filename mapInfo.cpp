// mapInfo.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "mapInfo.h"
#include <vector>

#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
struct RealPOINT {
	float x;
	float y;
};

bool IsDragging = false;
enum TOOL{TOOL_DEFAULT,TOOL_MOVE,TOOL_ADD};
TOOL CurrentTool;
int iSelectedPolygon = -1, iSelectedVertex = -1, iHoverPolygon = -1;
int MouseXwhenSelected = 0, MouseYwhenSelected = 0;
int MouseXdragStart = 0, MouseXdragEnd = 0, MouseYdragStart = 0, MouseYdragEnd = 0;
using namespace std;
vector<int> SelectedVertexes = {};
vector<RealPOINT> PolygonBeingModified;
vector<std::vector<RealPOINT>> Polygons(0);
vector<RealPOINT> PolygonBeingAdded;


// 此代码模块中包含的函数的前向声明: 
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

    // TODO: 在此放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MAPINFO, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MAPINFO));

    MSG msg;

    // 主消息循环: 
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
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAPINFO));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MAPINFO);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

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

void fillPolygon()
{

}


WCHAR* GetToolString(enum TOOL currentTool)
{
	switch (currentTool)
	{
	case TOOL_ADD:
		return L"Tool_ADD";
		break;
	case TOOL_MOVE:
		return L"Tool_Move";
		break;
	default:
		return L"Tool_Default";
	}
}

void SaveMapInfo()
{

}

void LoadMapInfo()
{
	//CreateFile
	//Just two layered vectors easy to store. one vector by one vector sequentially.  easy to handle. 
}

void DrawInfo(HDC hdc)
{
	WCHAR szBuffer[20];
	//wsprintf(szBuffer, L"X:%d Y:%d", x, y);
	//TextOut(hdc, 33, 33, szBuffer, lstrlen(szBuffer));
	wsprintf(szBuffer, L"Tool:%s", GetToolString(CurrentTool));
	TextOut(hdc, 33, 63, szBuffer, lstrlen(szBuffer));
}



bool isPointInPolygon(vector<RealPOINT> &polygon, int testx, int testy)
{
	bool c = false;
	int i, j;
	for (i = 0, j = polygon.size() - 1; i < polygon.size(); j = i++)
	{
		if (((polygon[i].y > testy) != (polygon[j].y > testy)) &&
			(testx < (polygon[j].x - polygon[i].x) * (testy - polygon[i].y) / (polygon[j].y - polygon[i].y) + polygon[i].x))
			c = !c;
	}
	return c;
}



void DrawAddedPolygon(HDC hdc)
{
	POINT useless;
	auto &polygon = PolygonBeingAdded;
	if (polygon.size() <= 0)
		return;
	
	for (int i = 0; i < polygon.size(); i++)
	{
		if(i == 0)
			MoveToEx(hdc, polygon[0].x, polygon[0].y, &useless);
		else
			LineTo(hdc, polygon[i].x, polygon[i].y);
		Ellipse(hdc, polygon[i].x - 4, polygon[i].y - 4, polygon[i].x + 4, polygon[i].y + 4);
	}
}



void DrawAllPolygons(HDC hdc)
{
	HPEN hSelectedVertPen = CreatePen(PS_SOLID, 2, 0xEEEEEE);
	HPEN hNormalPen = CreatePen(PS_SOLID, 2, 0xFF);
	HPEN hHoverPen = CreatePen(PS_SOLID, 2, 0xFF00);
	HPEN hSelectedPen = CreatePen(PS_SOLID, 2, 0xFF0000);
	int iPolygon = 0;
	for(auto polygon : Polygons)
	{
		if (polygon.size() <= 0)
			continue;

		if (iSelectedPolygon == iPolygon)
			SelectObject(hdc, hSelectedPen);
		else if (iHoverPolygon == iPolygon)
			SelectObject(hdc, hHoverPen);
		else
			SelectObject(hdc, hNormalPen);
	
		POINT Non;
		MoveToEx(hdc, polygon[polygon.size()-1].x, polygon[polygon.size() - 1].y,&Non);
		int iInSelectedVertexes = 0;
		for (int i = 0 ;i < polygon.size(); i++)
		{
			LineTo(hdc, polygon[i].x, polygon[i].y);
			if (iSelectedPolygon == iPolygon)
			{
				if (iInSelectedVertexes < SelectedVertexes.size() && i == SelectedVertexes[iInSelectedVertexes])
				{
					SelectObject(hdc, hSelectedVertPen);
					iInSelectedVertexes++;
				}
				else
					SelectObject(hdc, hNormalPen);
			}
			Ellipse(hdc, polygon[i].x - 4, polygon[i].y - 4, polygon[i].x + 4, polygon[i].y + 4);
		}
		iPolygon++;
	}
	DrawAddedPolygon(hdc);
	DeleteObject(hSelectedPen);
	DeleteObject(hSelectedVertPen);
	DeleteObject(hHoverPen);
	DeleteObject(hNormalPen);
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//

void SwitchTool(enum TOOL targetTool)
{
	CurrentTool = targetTool;
}

vector<RealPOINT>& BuildPolygon(float Vertx[], float Verty[], int nVert)
{
	auto *ThisPolygon = new vector<RealPOINT>;
	for(int i = 0 ;i < nVert ;i++)
		ThisPolygon->push_back({ Vertx[i],Verty[i] });
	
	return *ThisPolygon;
}

void StartAddingNewPolygon()
{
	PolygonBeingAdded.clear();
}

void EndAddingNewPolygon()
{
	if(PolygonBeingAdded.size() >= 3)
		Polygons.push_back(PolygonBeingAdded);
	PolygonBeingAdded.clear();
}

int GetiPolygonUnderMouse(int MouseX,int MouseY)
{
	int iPolygon = 0;
	for (auto polygon : Polygons)
	{
		if (isPointInPolygon(polygon,MouseX, MouseY))
			return iPolygon;	
		iPolygon++;
	}
	return -1;
}

bool isPtInRect(RECT &DragArea, RealPOINT Pt)
{
	if (Pt.x > DragArea.left &&
		Pt.x < DragArea.right &&
		Pt.y > DragArea.top &&
		Pt.y < DragArea.bottom)
		return true;

	return false;
}

int inline PointDistanceSquared(RealPOINT &Pt1, RealPOINT Pt2)
{
	return pow((Pt1.x - Pt2.x), 2) + pow((Pt1.y - Pt2.y), 2);
}


void GenerateSelectedVertexesFromDragArea(RECT &DragArea)
{
	if (iSelectedPolygon == -1)
		return;
	int i = 0;
	for (auto Vertex : Polygons[iSelectedPolygon])
	{
		if (isPtInRect(DragArea, Vertex))
			SelectedVertexes.push_back(i);
		i++;
	}
}

bool ProcessKeyMsgs(UINT message, WPARAM wParam, LPARAM lParam)
{
	bool isRefresh = false;
	switch (message)
	{
	case WM_KEYUP:
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'A':
			SwitchTool(TOOL_ADD);
			StartAddingNewPolygon();
			break;
		case 'M':
			SwitchTool(TOOL_MOVE);
			break;
		case 'D':
			SwitchTool(TOOL_DEFAULT);
			break;
		case VK_DELETE:
			if (iSelectedPolygon != -1)
				Polygons.erase(Polygons.begin() + iSelectedPolygon);
			break;
		case VK_RETURN:
			EndAddingNewPolygon();
			break;
		}
		break;

	}
	return isRefresh = true;
}

bool ProcessMouseMsgs(UINT message, WPARAM wParam, LPARAM lParam)
{
	bool isRefresh = false;
	int MouseX, MouseY;
	MouseX = LOWORD(lParam);
	MouseY = HIWORD(lParam);
	iHoverPolygon = GetiPolygonUnderMouse(MouseX, MouseY);
	switch (CurrentTool)
	{
	case TOOL_ADD:
		switch (message)
		{
		case WM_LBUTTONDBLCLK:
			EndAddingNewPolygon();
			break;
		case WM_LBUTTONDOWN:
			PolygonBeingAdded.push_back({ (float)MouseX, (float)MouseY });
			break;
		case WM_RBUTTONDOWN:
			if (PolygonBeingAdded.size() > 0)
				PolygonBeingAdded.erase(PolygonBeingAdded.end()-1);
		}
		//InvalidateRect(hWnd, NULL, TRUE);
		break;
	case TOOL_MOVE:
		switch (message) 
		{
		case WM_LBUTTONDOWN:
			MouseXwhenSelected = MouseX;
			MouseYwhenSelected = MouseY;
			if (iSelectedPolygon != -1 && iHoverPolygon != -1)
				PolygonBeingModified = Polygons[iHoverPolygon];
			break;
		case WM_MOUSEMOVE:
			if ((wParam & MK_LBUTTON) && iHoverPolygon != -1 && iSelectedPolygon != -1)
			{
				for (int i = 0 ;i < Polygons[iSelectedPolygon].size() ;i++)
				{
					Polygons[iSelectedPolygon][i].x = PolygonBeingModified[i].x + (MouseX - MouseXwhenSelected);
					Polygons[iSelectedPolygon][i].y = PolygonBeingModified[i].y + (MouseY - MouseYwhenSelected);
				}
			}
			break;
		case WM_LBUTTONUP:
			bool isAnyVertexSelected = false;
			if (iSelectedPolygon != -1)
			{
				int iVertex = 0;
				for(auto Vertex : Polygons[iSelectedPolygon])
				{ 
					if (PointDistanceSquared(Vertex, { (float)MouseX,(float)MouseY }) < 15)
					{
						isAnyVertexSelected = true;
						SelectedVertexes.clear();
						SelectedVertexes.push_back(iVertex);
						break;
					}
					iVertex++;
				}
			}
			if(!isAnyVertexSelected)
				iSelectedPolygon = iHoverPolygon;
			break;
		}
		break;
	case TOOL_DEFAULT:
		//Dragging  
		switch (message) {
		case WM_LBUTTONDOWN:
			IsDragging = true;
			MouseXdragStart = MouseXdragEnd = MouseX;
			MouseYdragStart = MouseYdragEnd = MouseY;
			break;
		case WM_MOUSEMOVE:
			if(wParam & MK_LBUTTON && IsDragging)
			{
				MouseXdragEnd = MouseX;
				MouseYdragEnd = MouseY;
				RECT DragArea;
				DragArea.left = MouseXdragStart > MouseXdragEnd ? MouseXdragEnd : MouseXdragStart;
				DragArea.right = MouseXdragStart > MouseXdragEnd ? MouseXdragStart : MouseXdragEnd;
				DragArea.top = MouseYdragStart > MouseYdragEnd ? MouseYdragEnd : MouseYdragStart;
				DragArea.bottom = MouseYdragStart > MouseYdragEnd ? MouseYdragStart : MouseYdragEnd;
				GenerateSelectedVertexesFromDragArea(DragArea);
			}
			break;
		case WM_LBUTTONUP:
			IsDragging = false;
			break;
		}
		break;
	}
	//
	return isRefresh = true;
}

void DrawDragging(HDC hdc)
{
	if (!IsDragging)
		return;

	HPEN hPen = CreatePen(PS_DASH, 2, 0xFF0000);
	SelectObject(hdc, GetStockObject(NULL_BRUSH));
	SelectObject(hdc, hPen);
	Rectangle(hdc, MouseXdragStart, MouseYdragStart, MouseXdragEnd, MouseYdragEnd);
}

void Draw(HWND hWnd,HDC hdc)
{
	RECT Rect;
	GetClientRect(hWnd, &Rect);
	static HDC hDCbuffer = CreateCompatibleDC(hdc);
	static HBITMAP hBitmap = CreateCompatibleBitmap(hdc, Rect.right, Rect.bottom);
	SelectObject(hDCbuffer, hBitmap);
	FillRect(hDCbuffer, &Rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
	//Rectangle(hDCbuffer, 0, 0, Rect.right, Rect.bottom);
	DrawAllPolygons(hDCbuffer);
	DrawInfo(hDCbuffer);
	DrawDragging(hDCbuffer);
	BitBlt(hdc, 0, 0, Rect.right, Rect.bottom, hDCbuffer, 0, 0, SRCCOPY);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	float ArrVertx[] = {30,100,200,150,44};
	float ArrVerty[] = {55,100,44,32,22};
	
	auto f = [](float vertx[], float verty[], int nvert, int biasx, int biasy) {for (int i = 0; i < nvert; i++) { vertx[i] += biasx; verty[i] += biasy; }};

	static int MouseX = 0, MouseY = 0;

	switch (message)
	{
	case WM_CREATE:
		Polygons.push_back(BuildPolygon(ArrVertx, ArrVerty, 5));
		f(ArrVertx, ArrVerty, 5, 150, 0);
		Polygons.push_back(BuildPolygon(ArrVertx, ArrVerty, 5));
		f(ArrVertx, ArrVerty, 5, 150, 0);
		Polygons.push_back(BuildPolygon(ArrVertx, ArrVerty, 5));
		f(ArrVertx, ArrVerty, 5, 0, 150);
		Polygons.push_back(BuildPolygon(ArrVertx, ArrVerty, 5));
		f(ArrVertx, ArrVerty, 5, -150, 0);
		Polygons.push_back(BuildPolygon(ArrVertx, ArrVerty, 5));
		f(ArrVertx, ArrVerty, 5, -150, 0);
		Polygons.push_back(BuildPolygon(ArrVertx, ArrVerty, 5));
		f(ArrVertx, ArrVerty, 5, 0, 150);
		Polygons.push_back(BuildPolygon(ArrVertx, ArrVerty, 5));

		break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 分析菜单选择: 
		switch (wmId)
		{
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
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
	case WM_MOUSEMOVE:
		if (ProcessMouseMsgs(message, wParam, lParam))
			InvalidateRect(hWnd, NULL, false);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		if (ProcessKeyMsgs(message, wParam, lParam))
			InvalidateRect(hWnd, NULL, false);
		break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			Draw(hWnd,hdc);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
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

// “关于”框的消息处理程序。
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
