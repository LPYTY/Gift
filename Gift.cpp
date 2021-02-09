// Gift.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "Gift.h"
#include "name_generator.h"

#define MAX_LOADSTRING 100



class GdiplusMgr
{
private:
	Gdiplus::GdiplusStartupInput m_gdiplusStartupInput;
	ULONG_PTR token;
public:
	GdiplusMgr()
	{
		Gdiplus::GdiplusStartup(&token, &m_gdiplusStartupInput, NULL);
	}
	~GdiplusMgr()
	{
		Gdiplus::GdiplusShutdown(token);
	}
};

struct buttonstatus
{
	int run_botton = 1;//0:need reset, 1:not running, 2:running
	int reset_botton = 0;//0:disabled, 1.enabled
};

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
CString DirectoryName;
int present_id = 0;
string present_name;
name_generator ng;
bool started = false, lbtndown = false, putback = false;
buttonstatus bs;
GdiplusMgr n;
int charrec = 0;//"M","E","N","U"

void change_reset_button(int i) { bs.reset_botton = i; } // used for win7 debugging
void copy_to_clipboard(const string&);

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Menu(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    NameList(HWND, UINT, WPARAM, LPARAM);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
					 _In_opt_ HINSTANCE hPrevInstance,
					 _In_ LPWSTR    lpCmdLine,
					 _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 在此处放置代码。

	TCHAR lpDirectoryName[MAX_LOADSTRING];
	GetModuleFileName(hInstance, lpDirectoryName, MAX_LOADSTRING);
	DirectoryName = lpDirectoryName;
	int i = DirectoryName.ReverseFind('\\');
	DirectoryName = DirectoryName.Left(i);
	CStringA def_fnameA;
	def_fnameA = DirectoryName;
	def_fnameA += "\\files\\default.txt";
	ng.init_file(def_fnameA.GetString());
   /* ofstream offf("C:\\Users\\csepdi\\desktop\\x.txt", ios::out);
	for(int pppp = 0;pppp <= 500; pppp ++)
	{
		string ss = ng.get_name(&present_id);
		if (!ss.size()) 
			ss.append("empty");
		offf << ss << present_id << endl;
		ng.disable(present_id);
	}*/ //debug

	// 初始化全局字符串
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_GIFT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);


	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}



	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GIFT));

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



void copy_to_clipboard(const string& str)
{
	HGLOBAL hGlobal = GlobalAlloc(GHND, str.size() + 1);
	if (hGlobal == INVALID_HANDLE_VALUE) return;
	char* pGlobal = (char*)GlobalLock(hGlobal);
	strcpy_s(pGlobal, str.size() + 1, str.c_str());
	GlobalUnlock(hGlobal);

	OpenClipboard(NULL);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hGlobal);
	CloseClipboard();
}

//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
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
	wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GIFT));
	wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_GIFT);
	wcex.lpszClassName  = szWindowClass;
	wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_EX_LAYERED | WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX,
	   CW_USEDEFAULT, 0, 400, 400, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
	  return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static LOGFONT logfont;
	static HFONT hfont;
	static CString fstart(DirectoryName + L"\\files\\start.bmp"), fpause(DirectoryName + L"\\files\\pause.bmp"), freset(DirectoryName + L"\\files\\reset.bmp"),
					fstartinvalid(DirectoryName + L"\\files\\startinvalid.bmp"), fresetinvalid(DirectoryName + L"\\files\\resetinvalid.bmp");
	static Gdiplus::Image bmpstart(fstart.GetString()), bmppause(fpause.GetString()), bmpreset(freset.GetString()),
						bmpstartinvalid(fstartinvalid.GetString()), bmpresetinvalid(fresetinvalid.GetString());

	switch (message)
	{
	case WM_CREATE:
	{
		SetTimer(hWnd, 1, 100, nullptr);
		memset(&logfont, 0, sizeof(LOGFONT));
		logfont.lfHeight = 100;  //字体的高度
		logfont.lfWeight = FW_HEAVY;//字体的粗细
		lstrcpy(logfont.lfFaceName, L"宋体");
		hfont = CreateFontIndirect(&logfont);
		break;
	}
	case WM_TIMER:
	{
		if(started && ng.available() && !ng.need_reset())
		{
			present_name = ng.get_name(&present_id); 
			RECT rc = { 0, 0, 400, 220 };
			RedrawWindow(hWnd, &rc, nullptr, RDW_INVALIDATE|RDW_ERASE);
		}

		break;
	}
	case WM_PAINT:
	{
		RECT CliRect;
		GetClientRect(hWnd, &CliRect);
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		SelectObject(hdc, hfont);
		Gdiplus::Graphics g(hdc);

		switch (bs.run_botton)
		{
		case 0:
		{
			g.DrawImage(&bmpstartinvalid, 50, 220, 100, 100);
			break;
		}
		case 1:
		{
			g.DrawImage(&bmpstart, 50, 220, 100, 100);
			break;
		}
		case 2:
		{
			g.DrawImage(&bmppause, 50, 220, 100, 100);
			break;
		}
		default:
			break;
		}

		switch (bs.reset_botton)
		{
		case 0:
		{
			g.DrawImage(&bmpresetinvalid, 240, 220, 100, 100);
			break;
		}
		case 1:
		{
			g.DrawImage(&bmpreset, 240, 220, 100, 100);
			break;
		}
		default:
			break;
		}

		POINT MidPt;
		MidPt.x = CliRect.right / 2;
		MidPt.y = CliRect.bottom / 2;
		//Gdiplus::Font font(hdc, &logfont);
		//PointF point(MidPt.x, MidPt.y-50);//坐标点
		CString stemp;
		stemp = present_name.c_str();
		SIZE strSize;
		GetTextExtentPointA(hdc, present_name.c_str(), present_name.size(), &strSize);
		//g.DrawString(stemp, 4, &font, point, &brush);
		TextOutA(hdc, MidPt.x - strSize.cx / 2, MidPt.y - 120, present_name.c_str(), present_name.size());

		EndPaint(hWnd, &ps);
	}
		break;
	case WM_LBUTTONDOWN:
		{
			lbtndown = true;
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_LBUTTONUP:
		{
			if(lbtndown == true)
			{
				lbtndown = false;
				POINTS ms_pt = MAKEPOINTS(lParam);
				RECT CliRect;
				GetClientRect(hWnd, &CliRect);
				ms_pt.x -= CliRect.left;
				ms_pt.y -= CliRect.top;

				if(ms_pt.x <= 150 && ms_pt.x >= 50 && ms_pt.y <= 320 && ms_pt.y >= 220) // Click on start
				{
					switch(bs.run_botton)
					{
					case 0:
						break;
					case 1:
						{
							started = true;
							bs.run_botton = 2;
							bs.reset_botton = 0;
							RECT rc = { 0,0,400,400 };
							RedrawWindow(hWnd, &rc, NULL, RDW_INVALIDATE);                             
						}
						break;
					case 2:
						{
							started = false;
							bs.run_botton = 1;
							bs.reset_botton = 1;
							if(!putback)
								ng.disable(present_id);
							if (ng.need_reset())
								bs.run_botton = 0;
							RECT rc = { 0,0,400,400 };
							RedrawWindow(hWnd, &rc, NULL, RDW_INVALIDATE);
						}
						break;
					default:
						break;
					}
				}

				if(ms_pt.x <= 340 && ms_pt.x >= 240 && ms_pt.y <= 320 && ms_pt.y >= 220) // Click on reset
				{//TODO:ddd
						switch(bs.reset_botton)
						{
						case 0:
							break;
						case 1:
							{
								ng.reset();
								if(ng.available())
									bs.run_botton = 1;
								present_name.clear();
								RECT rc = { 0,0,400,400 };
								RedrawWindow(hWnd, &rc, NULL, RDW_INVALIDATE|RDW_ERASE);
							}
							break;
						default:
							break;
						}
				}
				lbtndown = false;
			}
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_RBUTTONUP:
	{
		if (!started)
		{
			POINTS ms_pt = MAKEPOINTS(lParam);
			RECT CliRect;
			GetClientRect(hWnd, &CliRect);
			ms_pt.x -= CliRect.left;
			ms_pt.y -= CliRect.top;

			if (ms_pt.x <= 150 && ms_pt.x >= 50 && ms_pt.y <= 320 && ms_pt.y >= 220) // Rclick on start
			{
				ng.cancel();
				bs.run_botton = 1;
				present_name.clear();
				RECT rc = { 0,0,400,400 };
				RedrawWindow(hWnd, &rc, NULL, RDW_INVALIDATE | RDW_ERASE);
			}
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	break;
	case WM_KEYUP:
	{
		switch (wParam)
		{
		case 0x4D: // M
		{
			if (charrec == 0) charrec++;
			else charrec = 0;
		}
		break;
		case 0x45: // E
		{
			if (charrec == 1) charrec++;
			else charrec = 0;
		}
		break;
		case 0x4E: // N
		{
			if (charrec == 2) charrec++;
			else charrec = 0;
		}
		break;
		case 0x55: // U
		{
			if (charrec == 3)
			{
				DialogBox(hInst, MAKEINTRESOURCE(IDD_MENU), hWnd, Menu);
			}
			charrec = 0;
		}
		break;
		case 0x4C: // L
		{
			DialogBox(hInst, MAKEINTRESOURCE(IDD_NAMELIST), hWnd, NameList);
		}
		break;
		default:
			break;
		}
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

INT_PTR Menu(HWND hDialog, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
	{
		CheckDlgButton(hDialog, IDC_SAME, putback ? BST_CHECKED : BST_UNCHECKED);
		SetDlgItemTextA(hDialog, IDC_NAMEFILE, ng.get_file_name().c_str());
		return INT_PTR(TRUE);
	}
	break;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_SAME:
		{
			UINT uichecked = IsDlgButtonChecked(hDialog, IDC_SAME);
			if (uichecked == BST_CHECKED)
			{
				putback = true;
			}
			else
			{
				putback = false;
			}
		}
		break;
		case IDC_SELECTFILE:
		{
			OPENFILENAMEA ofn;
			CHAR szFileName[MAX_PATH];

			ZeroMemory(&ofn, sizeof(ofn));
			szFileName[0] = 0;

			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hDialog;
			ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0";
			ofn.lpstrFile = szFileName;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrDefExt = "txt";


			ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
			if (GetOpenFileNameA(&ofn))
			{
				if (!ng.init_file(szFileName))
				{
					MessageBoxA(hDialog, "Init Failed!", "Error", MB_ICONERROR | MB_OK);
					break;
				}
				SetDlgItemTextA(hDialog, IDC_NAMEFILE, szFileName);
			}
		}
		break;
		case IDC_DEFAULTFILE:
		{
			TCHAR lpDirectoryName[MAX_LOADSTRING];
			GetModuleFileName(hInst, lpDirectoryName, MAX_LOADSTRING);
			DirectoryName = lpDirectoryName;
			int i = DirectoryName.ReverseFind('\\');
			DirectoryName = DirectoryName.Left(i);
			CStringA def_fnameA;
			def_fnameA = DirectoryName;
			def_fnameA += "\\files\\default.txt";
			ng.init_file(def_fnameA.GetString());
			SetDlgItemTextA(hDialog, IDC_NAMEFILE, def_fnameA.GetString());
		}
		break;
		case IDOK:
		{
			EndDialog(hDialog, LOWORD(wParam));
			return INT_PTR(TRUE);
		}
		break;
		default:
			break;
		}
	}
	break;
	case WM_CLOSE:
	{
		EndDialog(hDialog, LOWORD(wParam));
		return INT_PTR(TRUE);
	}
	break;
	default:		
		break;
	}
	return INT_PTR(FALSE);
}


INT_PTR CALLBACK NameList(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		SetDlgItemInt(hDlg, IDC_NUMBER, 10, FALSE);
		SetDlgItemTextA(hDlg, IDC_NAMELIST, "");
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
		case IDC_GENERATE:
		{
			UINT num;
			BOOL bsuccess;
			num = GetDlgItemInt(hDlg, IDC_NUMBER, &bsuccess, FALSE);
			if(!bsuccess)
			{
				MessageBoxA(hDlg, "Invalid Number!", "Error", MB_ICONERROR | MB_OK);
				break;
			}
			string str;
			int tmpid;
			name_generator tmpng(ng.get_file_name());
			for (int i = 0; i < num; i++)
			{
				str += tmpng.get_name(&tmpid) + " ";
				tmpng.disable(tmpid);
			}
			SetDlgItemTextA(hDlg, IDC_NAMELIST, str.c_str());
		}
		break;
		case IDCOPY:
		{
			char pstr[10000] = { 0 };
			GetDlgItemTextA(hDlg, IDC_NAMELIST, pstr, 10000);
			copy_to_clipboard(pstr);
		}
		break;
		default:
			break;
		}
		break;
	case WM_CLOSE:
	{
		EndDialog(hDlg, LOWORD(wParam));
		return (INT_PTR)TRUE;
	}
	break;
	default:
		break;
	}
	return (INT_PTR)FALSE;
}



