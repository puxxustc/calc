#include <windows.h>
#include "resource.h"
#include "global.h"
#include "button.h"
#include "outbox.h"


/***********************************************************
*
*函数：wnd_proc
*功能：主窗口窗口函数
*备注：
*返回：
*
***********************************************************/
static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT  ps;
	LRESULT  result;

	switch (message)
	{
		case WM_PAINT :
			// 重绘消息
			BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
			result = 0;
			break;

		case WM_SETFOCUS :
			// 窗口获得焦点
			SetFocus(__textbox);
			result = 0;
			break;

		case WM_ACTIVATE :
			// 窗口激活消息
			SetFocus(__textbox);
			result = 0;
			break;

		case WM_CLOSE :
			// 窗口关闭消息
			AnimateWindow(__wnd_main, 200, AW_BLEND + AW_ACTIVATE + AW_HIDE);
			// 删除按钮
			destroy_button();
			// 删除文本框
			destroy_outbox();
			// 销毁主窗体
			DestroyWindow(__wnd_main);
	
			result = 0;
			break;

		case WM_DESTROY :
			// 窗口销毁消息
			PostQuitMessage(0);
			result = 0;
			break;

		default :
			result = DefWindowProc(hwnd, message, wParam, lParam);
			break;
	}

	return result;
}


/***********************************************************
*
*函数：init_wnd_main
*功能：初始化主窗体
*备注：
*返回：
*
***********************************************************/
int init_wnd_main(void)
{
	WNDCLASSEX  wc;						// 窗口类
	RECT  rect;


	// 初始化窗口类
	wc.cbSize = sizeof(wc);					// 结构的大小
	wc.style = CS_HREDRAW | CS_VREDRAW;		// 窗口风格
	wc.lpfnWndProc = wnd_proc;				// 窗口函数指针
	wc.cbClsExtra = 0;						// 额外的类内存
	wc.cbWndExtra = 0;						// 额外的窗口内存
	wc.hInstance = __instance;				// 实例句柄
	wc.hIcon = LoadIcon(__instance, (LPCSTR)IDI_CALC);		// 窗口图标
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);				// 窗口光标
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);		// 背景刷子
	wc.lpszClassName = "HPCalc";			// 类名
	wc.lpszMenuName = NULL;					// 菜单
	wc.hIconSm = NULL;						// 小图标

	// 注册窗口类
	RegisterClassEx(&wc);

	//获取桌面窗口大小，以便让窗口显示在桌面中央
	GetWindowRect(GetDesktopWindow(), &rect);

	//创建窗口
	 __wnd_main = CreateWindowEx(
			0,							// 扩展样式
			"HPCalc",					// 类名
			"高精度计算器",				// 窗口标题
			// 窗口风格
			0
			| WS_MINIMIZEBOX			// 有最小化按钮
			| WS_CAPTION				// 有标题栏
			| WS_SYSMENU				// 有系统菜单
			,
			(rect.right - 400) / 2,		// 初始 X 坐标
			(rect.bottom - 500) / 2,	// 初始 Y 坐标
			400,						// 宽度
			500,						// 高度
			NULL,						// 父窗口句柄
			NULL,						// 菜单句柄
			__instance,					// 程序实例句柄
			NULL);						// 用户数据
	
	if (__wnd_main == NULL)
	{
		return -1;
	}
	else
	{
		// 显示窗口，刷新窗口客户区
		ShowWindow(__wnd_main, SW_SHOW);
		UpdateWindow(__wnd_main);
		return 0;
	}
}