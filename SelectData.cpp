// SelectData.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MySqlTest.h"
#include "SelectData.h"
#include "afxdialogex.h"
#include "MySqlTestDlg.h"

// CSelectData �Ի���

IMPLEMENT_DYNAMIC(CSelectData, CDialogEx)

CSelectData::CSelectData(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSelectData::IDD, pParent)
{

}

CSelectData::~CSelectData()
{
}

void CSelectData::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSelectData, CDialogEx)
	ON_BN_CLICKED(ID_DEFAULT, &CSelectData::OnBnClickedDefault)
	ON_BN_CLICKED(ID_USERDEF, &CSelectData::OnBnClickedUserdef)
END_MESSAGE_MAP()


// CSelectData ��Ϣ�������


void CSelectData::OnBnClickedDefault()
{
	TableName_User = "student_info";
	EndDialog(11);//defailt end
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CSelectData::OnBnClickedUserdef()
{
	GetDlgItem(IDC_EDIT_TABLE)->GetWindowTextW(TableName_User);
	EndDialog(12);//userdef end
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}
