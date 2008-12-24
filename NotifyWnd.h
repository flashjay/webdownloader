/************************************************************************
 * 通知窗口，类似MSN的好友上线提示窗口（非模态，不夺取前台焦点，自动浮出）
 * 
 * blog:	http://hoodlum1980.cnblogs.com
 * Email:	jinfd@126.com
 ************************************************************************/
#include <windows.h>
#include <commctrl.h>
#include <stdlib.h>
#include <stdio.h>
#include "Resource.h"
//---------------------------
//		TIMER定义
//---------------------------
#define TIMER_NOTIFY_SHOWING	101	//Notify Window显示（正在浮出）
#define TIMER_NOTIFY_HIDING		102	//Notify Window消失（正在下沉）
#define TIMER_NOTIFY_TIMEOUT	103 //Notify Window超时（显示时间已到）
//时间间隔
#define INTERVAL_NOTIFYWND		20	//窗口浮出/降落定时器间隔

//----------------------------
//		全局变量
//----------------------------
//原始的NotifyWnd的长度和宽度
int m_NotifyWndWidth;
int m_NotifyWndHeight;
//屏幕的长度和宽度
int m_ScrWidth;
int m_ScrHeight;
int m_CaptionHeight;	//标题栏高度
int m_dy;				//浮出或者降落时，递增或者递减的纵坐标起始值
HBITMAP m_hBgBitmap;	//NotifyWnd的背景图像
//要显示的文本
HFONT m_NotifyTitleFont;	//显示文本的字体
HFONT m_NotifyContentFont;	//内容字体
int m_NotifyTitleLeft;	//标题坐标的x=65
int m_NotifyTitleTop;	//标题坐标的y=10
RECT m_NotifyContentRect;//内容的矩形

int m_NotifyTitleColor; //标题颜色=RGB(60,60,250)
int m_NotifyContentColor;//内容颜色
char m_NotifyTitle[20];	//标题文本
char m_NotifyContent[256];//内容文本

//-----------------------------
//		方法列表
//-----------------------------
//设置NotifyWnd标题
void SetNotifyTitle(LPCTSTR title)
{
	strcpy(m_NotifyTitle,title);
}

//设置NotifyWnd标题
void SetNotifyContent(LPCTSTR content)
{
	strcpy(m_NotifyContent,content);
}

//设置closebutton的位图
void SetNotifyCloseButton(HWND hdlg, HBITMAP hbitmap)
{
	HWND hclosebutton = GetDlgItem(hdlg,IDC_CLOSEBUTTON);
	SendMessage(hclosebutton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbitmap);
}

//设置notify window的背景图像
void SetNotifyBackground(HBITMAP hbitmap)
{
	m_hBgBitmap=hbitmap;
}

//显示Notify Window
//int timeoutMiliseconds: 当大于0时，表示超过该时间将消失，小于等于0时必须由用户点击关闭才会消失
void ShowNotifyWnd(HWND hDlg,LPCTSTR title, LPCTSTR content, int timeoutMiliseconds)
{
	HWND hwnd=GetForegroundWindow();
	//设置标题和文本
	strcpy(m_NotifyTitle,title);
	strcpy(m_NotifyContent,content);
	ShowWindow(hDlg,SW_SHOW);
	SetWindowPos(hDlg,NULL,
		m_ScrWidth - m_NotifyWndWidth - 20,//注意露出右侧滚动条的宽度！
		m_ScrHeight - m_CaptionHeight,
		m_NotifyWndWidth,
		0,
		SWP_NOACTIVATE|SWP_NOZORDER
		);
	SetForegroundWindow(hwnd);

	//使窗口完全透明，VC6对图层窗口API支持不够好，因此取消
	//SetLayeredWindowAttributes(hDlg,0, 0 , LWA_ALPHA);

	//防止定时器冲突，kill 现有的所有显示定时器
	KillTimer(hDlg,TIMER_NOTIFY_SHOWING);
	KillTimer(hDlg,TIMER_NOTIFY_HIDING);
	KillTimer(hDlg,TIMER_NOTIFY_TIMEOUT);

	SetTimer(hDlg, TIMER_NOTIFY_SHOWING, INTERVAL_NOTIFYWND, NULL);
	//安装超时定时器, 如果这个参数小于等于0，则表示永远显示！
	if(timeoutMiliseconds>0)
		SetTimer(hDlg, TIMER_NOTIFY_TIMEOUT, timeoutMiliseconds, NULL);
}

//消失Notify Window
void HideNotifyWnd(HWND hDlg)
{
	//防止两个定时器冲突，kill 显示定时器
	KillTimer(hDlg,TIMER_NOTIFY_SHOWING);
	KillTimer(hDlg,TIMER_NOTIFY_HIDING);
	KillTimer(hDlg,TIMER_NOTIFY_TIMEOUT);

	SetTimer(hDlg, TIMER_NOTIFY_HIDING, INTERVAL_NOTIFYWND, NULL);
}

//初始化Notify Window
void InitNotifyWnd(HWND hDlg)
{
	RECT rc;
	LOGFONT lf;
	memset(&lf,0,sizeof(lf));
	//使该窗口成为顶层窗口！[注意]：BringWindowToTop( hwnd )不会是窗口成为TopMost窗口！
	SetWindowPos(hDlg,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);//忽略x，y，cx，cy参数
	//获取原始高度，宽度
	GetWindowRect(hDlg,&rc);
	m_NotifyWndWidth = rc.right-rc.left;
	m_NotifyWndHeight = rc.bottom-rc.top;
	SetWindowPos(hDlg,NULL,-1000,-1000,m_NotifyWndWidth,m_NotifyWndHeight,SWP_NOZORDER | SWP_NOSIZE);
	ShowWindow(hDlg, SW_HIDE);

	//获取屏幕尺寸
	m_ScrWidth = GetSystemMetrics(SM_CXSCREEN);
	m_ScrHeight = GetSystemMetrics(SM_CYSCREEN);
	m_CaptionHeight = GetSystemMetrics(SM_CYCAPTION)+3; //=(26+3)=29
	m_dy=4;//窗口浮出或降落时，递增或者递减4个像素

	//设置布局信息：
	m_NotifyTitleLeft = 65;
	m_NotifyTitleTop = 10;
	m_NotifyTitleColor = RGB(60,20,140);
	m_NotifyContentColor = RGB(30,10,150);
	m_NotifyContentRect.left=16;
	m_NotifyContentRect.top=52;
	m_NotifyContentRect.right=m_NotifyWndWidth-16;
	m_NotifyContentRect.bottom=m_NotifyWndHeight-10;
	
	//创建字体
	//m_NotifyFont = (HFONT)SendMessage(hDlg, WM_GETFONT, 0, 0);
	lf.lfHeight = 13;//font size
	lf.lfWeight = FW_BOLD;//粗体
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	strcpy(lf.lfFaceName, "宋体"); //char[32]

	//标题字体（size=10，Bold，宋体）
	m_NotifyTitleFont=CreateFontIndirect(&lf);

	//文本字体（size=10，Normal，宋体）
	lf.lfWeight = FW_NORMAL;
	m_NotifyContentFont = CreateFontIndirect(&lf);

	//设置标题：
	//strcpy(m_NotifyTitle,"尊敬的客人：");
	//将窗口设置为图层窗口样式, 备注：VC6版本太低，对图层窗口的API支持不够好，因此取消该意图
    //SetWindowLong(hDlg,GWL_EXSTYLE, GetWindowLong(hDlg, GWL_EXSTYLE) | WS_EX_LAYERED);
}

//-------------------------------------------------------------------------------------
//			Notify Window's Procedure: Notify Window的消息循环！
//-------------------------------------------------------------------------------------
LRESULT CALLBACK NotifyWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
 {
	WORD ctl;
	HDC hdc, hdcMem;//实际DC和内存DC
	PAINTSTRUCT ps;
	//HBRUSH hbrush;
	RECT rc;
	HWND hwnd;

	switch (message)
	{
		case WM_INITDIALOG:
			InitNotifyWnd(hDlg);
			return TRUE;

			//绘制标题和内容的文本
		case WM_PAINT:
			hdc=BeginPaint(hDlg,&ps);

			/* Background Modes ( at "wingid.h" ) */
			//#define TRANSPARENT         1
			//#define OPAQUE              2
			//#define BKMODE_LAST         2
			SetBkMode(hdc,TRANSPARENT);//这样绘制的字体不会带有白色背景
			//draw title
			SetTextColor(hdc, m_NotifyTitleColor);
			if(m_NotifyTitleFont)
				SelectObject(hdc, m_NotifyTitleFont);
			TextOut(hdc,m_NotifyTitleLeft, m_NotifyTitleTop,m_NotifyTitle,strlen(m_NotifyTitle));

			//draw content
			SetTextColor(hdc, m_NotifyContentColor);
			if(m_NotifyContentFont)
				SelectObject(hdc, m_NotifyContentFont);
			DrawText(hdc,m_NotifyContent, strlen(m_NotifyContent),
				&m_NotifyContentRect,
				DT_LEFT|DT_WORDBREAK);//DT_WORDBREAK会使文本自动换行

			EndPaint(hDlg,&ps);
			break;

			
			//复制背景位图
		case WM_ERASEBKGND:
			hdc = (HDC)wParam;
			
			if(m_hBgBitmap)
			{
				hdcMem = CreateCompatibleDC(hdc);
				SelectObject(hdcMem, m_hBgBitmap);
				BitBlt(hdc, 0, 0, m_NotifyWndWidth, m_NotifyWndHeight, hdcMem, 0, 0, SRCCOPY);
				DeleteDC(hdcMem);
			}
			
			return TRUE;

		case WM_COMMAND:
			ctl=LOWORD(wParam);
			switch(ctl)
			{
			case IDC_CLOSEBUTTON:
				//用户点击了关闭按钮
				HideNotifyWnd(hDlg);
				break;
			}
			return TRUE;

		case WM_DESTROY:
			if(m_NotifyTitleFont)
				DeleteObject(m_NotifyTitleFont);
			if(m_NotifyContentFont)
				DeleteObject(m_NotifyContentFont);
			return TRUE;


		case WM_TIMER:
			GetWindowRect(hDlg,&rc);
			switch(wParam)
			{
			case TIMER_NOTIFY_SHOWING:
				if((rc.bottom-rc.top)>=m_NotifyWndHeight)
				{
					KillTimer(hDlg, TIMER_NOTIFY_SHOWING);					
				}
				hwnd=GetForegroundWindow();
				//重设窗口位置（逐渐升起）
				SetWindowPos(hDlg,NULL, rc.left, rc.top-m_dy, m_NotifyWndWidth, (rc.bottom-rc.top) + m_dy,
					SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE);
				hwnd=GetForegroundWindow();
				//逐渐浮现！不透明度越来越大，从0~255, 255*当前高度/最终高度为当前的不透明度
				//这个函数在vc6里面不太容易包含进来
				//SetLayeredWindowAttributes(hDlg,0, (BYTE)(255*((rc.bottom-rc.top) + m_dy)/m_NotifyWndHeight), LWA_ALPHA);
				break;

			case TIMER_NOTIFY_HIDING:
				if((rc.bottom-rc.top)<=0)
				{
					KillTimer(hDlg, TIMER_NOTIFY_HIDING);
					ShowWindow(hDlg, SW_HIDE);
				}
				hwnd=GetForegroundWindow();
				//重设窗口位置（逐渐降落）
				SetWindowPos(hDlg,NULL,rc.left, rc.top+m_dy, m_NotifyWndWidth,  (rc.bottom-rc.top) - m_dy,
					SWP_NOZORDER|SWP_NOACTIVATE);
				hwnd=GetForegroundWindow();
				break;

			case TIMER_NOTIFY_TIMEOUT:
				//显示的时间已经超时
				HideNotifyWnd(hDlg);
				break;
			}
			return TRUE;
	}
    return FALSE;
}




