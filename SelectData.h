#pragma once


// CSelectData �Ի���

class CSelectData : public CDialogEx
{
	DECLARE_DYNAMIC(CSelectData)

public:
	CSelectData(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSelectData();

// �Ի�������
	enum { IDD = IDD_Select_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedDefault();
	afx_msg void OnBnClickedUserdef();
	CString TableName_User;
};
