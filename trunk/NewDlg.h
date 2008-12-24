#include "Resource.h"
#include <time.h>
#include <string>
#include <map>
#include "PictureEx.h"

#pragma once
using namespace std;
// CNewDlg dialog

class CNewDlg : public CDialog
{
	DECLARE_DYNAMIC(CNewDlg)

public:
	CNewDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNewDlg();
	static DWORD WINAPI DownloadThread( LPVOID lpParam );

	CPictureEx m_PictureWW;
	
	CPictureEx m_PictureLoading;

private:

	void LogIt( map<string,string> &lmap );

// Dialog Data
	enum { IDD = IDD_DIALOG_MAIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	void OnOK();
	void OnCancel();
	HBRUSH OnCtlColor(CDC *pDC,CWnd * pWnd, UINT nCtlColor);

	void OnDestroy()
	{
		CDialog::OnDestroy();
		delete this;
	}

private:

protected:

	LPCTSTR m_strUrl;
	LPCTSTR m_strOutFileName;

	map<string,string> m_mapList;
	HANDLE m_hThread;

	HICON m_hIcon;

	HWND m_hNotifyWnd;

	void StartTask(void);
	void Callback(CString cs = "");

	//afx_msg bool OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButtonDownload();
	afx_msg void OnBnClickedButtonDefault();
	afx_msg void OnBnClickedButtonReset();
	afx_msg void OnBnClickedButtonSuspend();

	virtual BOOL PreTranslateMessage(MSG* pMsg); 
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnAbout();
};

struct RECVPARAM
{
	LPCTSTR url;
	LPCTSTR fileName;
	HWND hwnd;
	CNewDlg *my;
};