// NewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WebDownloader.h"
#include "NewDlg.h"
#include <fstream>
#include <Wininet.h>
#include <string>
#include "md5.h"
#include <time.h>
#include <map> 
#include "PictureEx.h"
#include "NotifyWnd.h"
#include "About.h"
#pragma  comment(lib,"Wininet.lib")
using namespace std;
// CNewDlg dialog

IMPLEMENT_DYNAMIC(CNewDlg, CDialog)

CNewDlg::CNewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CNewDlg::~CNewDlg()
{
	if(m_hNotifyWnd)
		::DestroyWindow(m_hNotifyWnd);
}

void CNewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX); 
	//{{AFX_DATA_MAP(CPictureExDemoDlg)
	//ע��: �ڶ��������Ǵ���ռλԪ�ص�ID,����ͼ�����ID
	DDX_Control(pDX, IDC_GIFFIRST, m_PictureWW);
	DDX_Control(pDX, IDC_GIFLOADING, m_PictureLoading);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CNewDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD, OnBnClickedButtonDownload)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULT,  OnBnClickedButtonDefault)
	ON_BN_CLICKED(IDC_BUTTON_RESET,    OnBnClickedButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_SUSPEND,  OnBnClickedButtonSuspend)
//	ON_WM_ERASEBKGND()
ON_BN_CLICKED(IDC_BUTTON4, &CNewDlg::OnAbout)
END_MESSAGE_MAP()

BOOL CNewDlg::OnInitDialog()
{

	CDialog::OnInitDialog();
 
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
 
	///��ʾͼƬ
	if (m_PictureWW.Load(MAKEINTRESOURCE(IDR_WENWEN),_T("GIF")))
		m_PictureWW.Draw();

	//����Notify Window
	
	//��õ�ǰʵ���ľ��^_^
	HINSTANCE hInstance = AfxGetInstanceHandle();

	m_hNotifyWnd=::CreateDialog(hInstance,MAKEINTRESOURCE(IDD_NOTIFYWND),NULL,(DLGPROC)NotifyWndProc);
	::ShowWindow(m_hNotifyWnd, SW_HIDE);
	SetNotifyCloseButton(m_hNotifyWnd, ::LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_CLOSEBUTTON)));
	SetNotifyBackground(::LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_NOTIFYBG)));

	return TRUE;

}

void CNewDlg::OnOK()
{
	int ret;
	if(!m_mapList.empty())
	{
		LogIt(m_mapList);
		ret = ::MessageBox(m_hWnd,"Log �ѵǼǵ� log.txt ~ ","WebDownloader",1);
	}
	else
	{
		ret = ::MessageBox(m_hWnd,"ȷ���˳��� ~ ","WebDownloader",1);
	}
	//1 ȷ��,2 ȡ��
	if(ret == 1)
	{
		CDialog::OnOK();
		DestroyWindow();
	}
	else
	{
	}
}

void CNewDlg::OnCancel()
{
	CDialog::OnCancel();
	DestroyWindow();
}
HBRUSH CNewDlg::OnCtlColor(CDC *pDC,CWnd * pWnd,UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC,pWnd,nCtlColor);
	return hbr;
}

// CNewDlg message handlers

void CNewDlg::OnBnClickedButton1()
{
    ShellExecute(NULL,"Open","http://blog.faryang.cn",NULL,NULL,SW_SHOWNORMAL);  
	// TODO: Add your control notification handler code here
}

void CNewDlg::OnBnClickedButtonDownload()
{
	CString strUrl,strTargetName;

	CWnd *pWnd = GetDlgItem(IDC_EDIT1);
	pWnd->GetWindowText(strUrl);

	GetDlgItem(IDC_EDIT2)->GetWindowTextA(strTargetName);

	if(strTargetName == "" )
	{
		strTargetName = MD5Sum(strUrl).GetHash();

		m_strOutFileName   = strTargetName + ".html";

		GetDlgItem(IDC_EDIT2)->SetWindowTextA(m_strOutFileName);

	} else {

		m_strOutFileName = strTargetName;
	}

	m_strUrl = strUrl;

	StartTask();

	//���߳�,���ⴰ�ڿ�ס
	RECVPARAM *pRecvParam= new RECVPARAM;

	pRecvParam->url      = strUrl;
	pRecvParam->fileName = m_strOutFileName;
	pRecvParam->hwnd     = this->m_hWnd;
	pRecvParam->my       = this;

	m_hThread=CreateThread(NULL,0,DownloadThread,(LPVOID)pRecvParam,0,NULL);

	//TerminateThread(m_hThread,0);
	//CloseHandle(m_hThread);

	this->m_mapList[strTargetName.GetBuffer()] = strUrl.GetBuffer();

	//MessageBox(strUrl);
}

void CNewDlg::OnBnClickedButtonSuspend()
{
	// TODO: Add your control notification handler code here
	//if(m_hThread)
	//{
		//MessageBox("ֹͣ����~");
	//}
	Callback("ȡ������~");

}

void CNewDlg::OnBnClickedButtonDefault()
{
	GetDlgItem(IDC_EDIT1)->SetWindowTextA("http://www.google.cn");
	GetDlgItem(IDC_EDIT2)->SetWindowTextA("index.html");
}

void CNewDlg::OnBnClickedButtonReset()
{
	//TODO:�������url��
	GetDlgItem(IDC_EDIT1)->SetWindowTextA("http://");
	GetDlgItem(IDC_EDIT1)->SetFocus();
	((CEdit *)GetDlgItem(IDC_EDIT1))->SetSel(7,-1);
	GetDlgItem(IDC_EDIT2)->SetWindowTextA("");
}
//���ػس�ʱ�Զ�����ONOK~
BOOL CNewDlg::PreTranslateMessage(MSG* pMsg)   
{
	if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_ESCAPE) return TRUE;   
    if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN)
	{
		OnBnClickedButtonDownload();
		return TRUE;
	}
    return   CDialog::PreTranslateMessage(pMsg);    
} 

//��ʼ����Ҫ������...
void CNewDlg::StartTask()
{
	//��ʾLoadingͼƬ
	if (m_PictureLoading.Load(MAKEINTRESOURCE(IDR_LOADING),_T("GIF")))
		m_PictureLoading.Draw();

	CButton *pButton=(CButton *)GetDlgItem(IDC_BUTTON_DOWNLOAD);
	pButton->EnableWindow(false);
}
//���������Ļص�
void CNewDlg::Callback(CString cs)
{
	int alertTimespan = 6000;
	CString _content;
	_content += cs;
	if(cs == "")
	{
		_content.Append("���سɹ�\n");
		//�ǳ���̬,strUrlΪʲô���޸�?
		_content += m_strUrl;
		alertTimespan = 12000;
	}

	ShowNotifyWnd(m_hNotifyWnd, "WebDownoader ", _content, alertTimespan);
	m_PictureLoading.UnLoad();
	((CButton *)GetDlgItem(IDC_BUTTON_DOWNLOAD))->EnableWindow(true);
	TerminateThread(m_hThread,0);
	CloseHandle(m_hThread);
	/*DWORD nRet = ::WaitForSingleObject( m_handleSetMsgReadThread, 2000 );
	if( nRet != WAIT_OBJECT_0 )
	{
		::TerminateThread( m_handleSetMsgReadThread, 0 );	
	}
	::CloseHandle( m_handleSetMsgReadThread ); */
}


//static
DWORD WINAPI  CNewDlg::DownloadThread(LPVOID lpParam)
{
	
	LPCTSTR url         = ((RECVPARAM*)lpParam)->url;
	LPCTSTR outFileName = ((RECVPARAM*)lpParam)->fileName;
	HWND    hwnd        = ((RECVPARAM*)lpParam)->hwnd;
	CNewDlg *my         = ((RECVPARAM*)lpParam)->my;

	//::MessageBox(hwnd,url,"url",0);

	const int buf_size = 1024;//1�ζ���1k������

	double time_span;

	clock_t start, end;

	string buffer("");

	//��ʼ��ʱ

	start = clock();

	//DWORD bytes_read;
	HINTERNET internet = InternetOpen("HTTP Downloader", INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,NULL);

	if(!internet)
	{
		//::MessageBox(hwnd, "Internet Open error!","error",0);
		my->Callback("Internet Open error!");
		return 0;
	}
	
	HINTERNET file_handle = InternetOpenUrl(internet,url,NULL,0,INTERNET_FLAG_RELOAD,0);

	if(!file_handle)
	{
		//::MessageBox(hwnd, "Internet Open Url error! - Maybe you should andd Http:// or Ftp://","error",0);
		my->Callback("Internet Open Url error! - Maybe you should andd Http:// or Ftp://");
		return 0;
	}

	while(1)
	{
		char szBuffer[buf_size+1];

		DWORD bytes_read;

		BOOL b = InternetReadFile(file_handle,szBuffer,buf_size,&bytes_read);

		if(!b)
		{
			::MessageBox(hwnd, "InternetReadFile error!","error",0);
			my->Callback("InternetReadFile error!");
			return 0;
		}

		if(bytes_read == 0)
			break;

		szBuffer[bytes_read] = 0;

		buffer += szBuffer;
	}

	end = clock();

	time_span = (double)(end-start)/CLOCKS_PER_SEC;

	ofstream out(outFileName);

	out.write(buffer.c_str(),strlen(buffer.c_str()));

	out.close();

	InternetCloseHandle(internet);

	//::MessageBox(hwnd, " Download Success ~ " ,outFileName ,0);

	my->Callback();

	return 0;
}


//��־
void CNewDlg::LogIt( map<string,string> &lmap )
{
	//log here
	string strLogFile = "downlog.txt";

	string strLogContent = "";

	map<string,string>::const_iterator map_it = lmap.begin();

	while(map_it != lmap.end())
	{
		strLogContent += map_it->first + " " + map_it->second + "\r\n";
	
		++map_it;
	}

	if(strLogContent == "") return;

	try
	{
		ofstream out(strLogFile.c_str());

		out.write(strLogContent.c_str(),strlen(strLogContent.c_str()));

		out.close();
	}
	catch(exception &e)
	{
		MessageBox(e.what());
	}

}




void CNewDlg::OnAbout()
{
	// TODO: Add your control notification handler code here
	CAboutDlg dlg;
	dlg.DoModal();
}
