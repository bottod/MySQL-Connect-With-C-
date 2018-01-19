// LoginDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MySqlTest.h"
#include "LoginDlg.h"
#include "afxdialogex.h"


// CLoginDlg 对话框

IMPLEMENT_DYNAMIC(CLoginDlg, CDialogEx)

CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLoginDlg::IDD, pParent)
{

}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CLoginDlg::OnBnClickedLogin)
END_MESSAGE_MAP()


// CLoginDlg 消息处理程序


void CLoginDlg::OnBnClickedLogin()
{
	GetDlgItem(IDC_USRPSW)->GetWindowTextW(m_PassWD);
	GetDlgItem(IDC_USRNAME)->GetWindowTextW(m_Usr);
	CDialogEx::OnOK();
	// TODO: 在此添加控件通知处理程序代码
}


BOOL CLoginDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	GetDlgItem(IDC_USRNAME)->SetWindowTextW(L"XFLM");
	GetDlgItem(IDC_USRPSW)->SetWindowTextW(L"147258liu");
	GetDlgItem(IDC_USRPSW)->SetFocus();
	return false;//false为生效
}
