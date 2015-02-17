/*********************************************************************
*
*文件：main.c
*功能：存放主函数的文件
*备注：
*
*********************************************************************/

#include <windows.h>
#include <string.h>
#include "global.h"
#include "wnd_main.h"
#include "button.h"
#include "textbox.h"
#include "outbox.h"
#include "paintboard.h"



/***********************************************************
*
*函数：WinMain
*功能：主函数
*备注：
*返回：
*
***********************************************************/
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG   msg;		// 消息

	// 保存实例句柄到全局变量
	__instance = hInstance;
	
	// 初始化主窗体
	init_wnd_main();
	// 初始化按钮
	init_button();
	// 初始化文本框
	init_textbox();
	// 初始化输出文本框
	init_outbox();
	// 初始化画板
	init_paintboard();

	// 消息循环
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}