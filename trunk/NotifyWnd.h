/************************************************************************
 * ֪ͨ���ڣ�����MSN�ĺ���������ʾ���ڣ���ģ̬������ȡǰ̨���㣬�Զ�������
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
//		TIMER����
//---------------------------
#define TIMER_NOTIFY_SHOWING	101	//Notify Window��ʾ�����ڸ�����
#define TIMER_NOTIFY_HIDING		102	//Notify Window��ʧ�������³���
#define TIMER_NOTIFY_TIMEOUT	103 //Notify Window��ʱ����ʾʱ���ѵ���
//ʱ����
#define INTERVAL_NOTIFYWND		20	//���ڸ���/���䶨ʱ�����

//----------------------------
//		ȫ�ֱ���
//----------------------------
//ԭʼ��NotifyWnd�ĳ��ȺͿ��
int m_NotifyWndWidth;
int m_NotifyWndHeight;
//��Ļ�ĳ��ȺͿ��
int m_ScrWidth;
int m_ScrHeight;
int m_CaptionHeight;	//�������߶�
int m_dy;				//�������߽���ʱ���������ߵݼ�����������ʼֵ
HBITMAP m_hBgBitmap;	//NotifyWnd�ı���ͼ��
//Ҫ��ʾ���ı�
HFONT m_NotifyTitleFont;	//��ʾ�ı�������
HFONT m_NotifyContentFont;	//��������
int m_NotifyTitleLeft;	//���������x=65
int m_NotifyTitleTop;	//���������y=10
RECT m_NotifyContentRect;//���ݵľ���

int m_NotifyTitleColor; //������ɫ=RGB(60,60,250)
int m_NotifyContentColor;//������ɫ
char m_NotifyTitle[20];	//�����ı�
char m_NotifyContent[256];//�����ı�

//-----------------------------
//		�����б�
//-----------------------------
//����NotifyWnd����
void SetNotifyTitle(LPCTSTR title)
{
	strcpy(m_NotifyTitle,title);
}

//����NotifyWnd����
void SetNotifyContent(LPCTSTR content)
{
	strcpy(m_NotifyContent,content);
}

//����closebutton��λͼ
void SetNotifyCloseButton(HWND hdlg, HBITMAP hbitmap)
{
	HWND hclosebutton = GetDlgItem(hdlg,IDC_CLOSEBUTTON);
	SendMessage(hclosebutton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbitmap);
}

//����notify window�ı���ͼ��
void SetNotifyBackground(HBITMAP hbitmap)
{
	m_hBgBitmap=hbitmap;
}

//��ʾNotify Window
//int timeoutMiliseconds: ������0ʱ����ʾ������ʱ�佫��ʧ��С�ڵ���0ʱ�������û�����رղŻ���ʧ
void ShowNotifyWnd(HWND hDlg,LPCTSTR title, LPCTSTR content, int timeoutMiliseconds)
{
	HWND hwnd=GetForegroundWindow();
	//���ñ�����ı�
	strcpy(m_NotifyTitle,title);
	strcpy(m_NotifyContent,content);
	ShowWindow(hDlg,SW_SHOW);
	SetWindowPos(hDlg,NULL,
		m_ScrWidth - m_NotifyWndWidth - 20,//ע��¶���Ҳ�������Ŀ�ȣ�
		m_ScrHeight - m_CaptionHeight,
		m_NotifyWndWidth,
		0,
		SWP_NOACTIVATE|SWP_NOZORDER
		);
	SetForegroundWindow(hwnd);

	//ʹ������ȫ͸����VC6��ͼ�㴰��API֧�ֲ����ã����ȡ��
	//SetLayeredWindowAttributes(hDlg,0, 0 , LWA_ALPHA);

	//��ֹ��ʱ����ͻ��kill ���е�������ʾ��ʱ��
	KillTimer(hDlg,TIMER_NOTIFY_SHOWING);
	KillTimer(hDlg,TIMER_NOTIFY_HIDING);
	KillTimer(hDlg,TIMER_NOTIFY_TIMEOUT);

	SetTimer(hDlg, TIMER_NOTIFY_SHOWING, INTERVAL_NOTIFYWND, NULL);
	//��װ��ʱ��ʱ��, ����������С�ڵ���0�����ʾ��Զ��ʾ��
	if(timeoutMiliseconds>0)
		SetTimer(hDlg, TIMER_NOTIFY_TIMEOUT, timeoutMiliseconds, NULL);
}

//��ʧNotify Window
void HideNotifyWnd(HWND hDlg)
{
	//��ֹ������ʱ����ͻ��kill ��ʾ��ʱ��
	KillTimer(hDlg,TIMER_NOTIFY_SHOWING);
	KillTimer(hDlg,TIMER_NOTIFY_HIDING);
	KillTimer(hDlg,TIMER_NOTIFY_TIMEOUT);

	SetTimer(hDlg, TIMER_NOTIFY_HIDING, INTERVAL_NOTIFYWND, NULL);
}

//��ʼ��Notify Window
void InitNotifyWnd(HWND hDlg)
{
	RECT rc;
	LOGFONT lf;
	memset(&lf,0,sizeof(lf));
	//ʹ�ô��ڳ�Ϊ���㴰�ڣ�[ע��]��BringWindowToTop( hwnd )�����Ǵ��ڳ�ΪTopMost���ڣ�
	SetWindowPos(hDlg,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);//����x��y��cx��cy����
	//��ȡԭʼ�߶ȣ����
	GetWindowRect(hDlg,&rc);
	m_NotifyWndWidth = rc.right-rc.left;
	m_NotifyWndHeight = rc.bottom-rc.top;
	SetWindowPos(hDlg,NULL,-1000,-1000,m_NotifyWndWidth,m_NotifyWndHeight,SWP_NOZORDER | SWP_NOSIZE);
	ShowWindow(hDlg, SW_HIDE);

	//��ȡ��Ļ�ߴ�
	m_ScrWidth = GetSystemMetrics(SM_CXSCREEN);
	m_ScrHeight = GetSystemMetrics(SM_CYSCREEN);
	m_CaptionHeight = GetSystemMetrics(SM_CYCAPTION)+3; //=(26+3)=29
	m_dy=4;//���ڸ�������ʱ���������ߵݼ�4������

	//���ò�����Ϣ��
	m_NotifyTitleLeft = 65;
	m_NotifyTitleTop = 10;
	m_NotifyTitleColor = RGB(60,20,140);
	m_NotifyContentColor = RGB(30,10,150);
	m_NotifyContentRect.left=16;
	m_NotifyContentRect.top=52;
	m_NotifyContentRect.right=m_NotifyWndWidth-16;
	m_NotifyContentRect.bottom=m_NotifyWndHeight-10;
	
	//��������
	//m_NotifyFont = (HFONT)SendMessage(hDlg, WM_GETFONT, 0, 0);
	lf.lfHeight = 13;//font size
	lf.lfWeight = FW_BOLD;//����
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	strcpy(lf.lfFaceName, "����"); //char[32]

	//�������壨size=10��Bold�����壩
	m_NotifyTitleFont=CreateFontIndirect(&lf);

	//�ı����壨size=10��Normal�����壩
	lf.lfWeight = FW_NORMAL;
	m_NotifyContentFont = CreateFontIndirect(&lf);

	//���ñ��⣺
	//strcpy(m_NotifyTitle,"�𾴵Ŀ��ˣ�");
	//����������Ϊͼ�㴰����ʽ, ��ע��VC6�汾̫�ͣ���ͼ�㴰�ڵ�API֧�ֲ����ã����ȡ������ͼ
    //SetWindowLong(hDlg,GWL_EXSTYLE, GetWindowLong(hDlg, GWL_EXSTYLE) | WS_EX_LAYERED);
}

//-------------------------------------------------------------------------------------
//			Notify Window's Procedure: Notify Window����Ϣѭ����
//-------------------------------------------------------------------------------------
LRESULT CALLBACK NotifyWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
 {
	WORD ctl;
	HDC hdc, hdcMem;//ʵ��DC���ڴ�DC
	PAINTSTRUCT ps;
	//HBRUSH hbrush;
	RECT rc;
	HWND hwnd;

	switch (message)
	{
		case WM_INITDIALOG:
			InitNotifyWnd(hDlg);
			return TRUE;

			//���Ʊ�������ݵ��ı�
		case WM_PAINT:
			hdc=BeginPaint(hDlg,&ps);

			/* Background Modes ( at "wingid.h" ) */
			//#define TRANSPARENT         1
			//#define OPAQUE              2
			//#define BKMODE_LAST         2
			SetBkMode(hdc,TRANSPARENT);//�������Ƶ����岻����а�ɫ����
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
				DT_LEFT|DT_WORDBREAK);//DT_WORDBREAK��ʹ�ı��Զ�����

			EndPaint(hDlg,&ps);
			break;

			
			//���Ʊ���λͼ
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
				//�û�����˹رհ�ť
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
				//���贰��λ�ã�������
				SetWindowPos(hDlg,NULL, rc.left, rc.top-m_dy, m_NotifyWndWidth, (rc.bottom-rc.top) + m_dy,
					SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE);
				hwnd=GetForegroundWindow();
				//�𽥸��֣���͸����Խ��Խ�󣬴�0~255, 255*��ǰ�߶�/���ո߶�Ϊ��ǰ�Ĳ�͸����
				//���������vc6���治̫���װ�������
				//SetLayeredWindowAttributes(hDlg,0, (BYTE)(255*((rc.bottom-rc.top) + m_dy)/m_NotifyWndHeight), LWA_ALPHA);
				break;

			case TIMER_NOTIFY_HIDING:
				if((rc.bottom-rc.top)<=0)
				{
					KillTimer(hDlg, TIMER_NOTIFY_HIDING);
					ShowWindow(hDlg, SW_HIDE);
				}
				hwnd=GetForegroundWindow();
				//���贰��λ�ã��𽥽��䣩
				SetWindowPos(hDlg,NULL,rc.left, rc.top+m_dy, m_NotifyWndWidth,  (rc.bottom-rc.top) - m_dy,
					SWP_NOZORDER|SWP_NOACTIVATE);
				hwnd=GetForegroundWindow();
				break;

			case TIMER_NOTIFY_TIMEOUT:
				//��ʾ��ʱ���Ѿ���ʱ
				HideNotifyWnd(hDlg);
				break;
			}
			return TRUE;
	}
    return FALSE;
}




