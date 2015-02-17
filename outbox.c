/*********************************************************************
*
*文件：outbox.c
*功能：输出文本框（系统提供的文本框）
*备注：
*
*********************************************************************/

#include <windows.h>
#include "global.h"

/***********************全局变量***************************/
static HFONT _font;		// 字体


/***********************************************************
*
*函数：init_outbox
*功能：初始化输出文本框
*备注：
*返回：
*
***********************************************************/
int init_outbox(void)
{
	// 创建给文本框使用的字体
	_font = CreateFont(20, 10, 0, 0, FW_NORMAL, 0, 0, 0, 
			ANSI_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_SWISS,
			"宋体");
	// 创建文本框
	__outbox = CreateWindowEx(
			0,					// 扩展样式
			"edit",				// 类名
			NULL,				// 窗口标题
			// 窗口风格
			  WS_CHILD			// 子窗体
			| WS_VISIBLE		// 可见
			| WS_BORDER			// 有边框
			| WS_VSCROLL		// 有竖直滚动条
			| ES_READONLY		// 只读
			| ES_AUTOVSCROLL	// 自动竖直滚动
			| ES_MULTILINE		// 多行
			,
			40,					// 初始 X 坐标
			215,				// 初始 Y 坐标
			320,				// 宽度
			90,					// 高度
			__wnd_main,			// 父窗口句柄
			NULL,				// 菜单句柄
			__instance,			// 程序实例句柄
			NULL);				// 用户数据
	if (__outbox == NULL)
	{
		return -1;
	}
	else
	{
		// 设置字体
		SendMessage(__outbox, WM_SETFONT, (WPARAM)_font, 0);
		return 0;
	}
}


/***********************************************************
*
*函数：destroy_outbox
*功能：销毁输出文本框
*备注：
*返回：
*
***********************************************************/
int destroy_outbox(void)
{
	// 删除字体
	DeleteObject(_font);
	return 0;
}