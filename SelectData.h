#pragma once


// CSelectData 对话框

class CSelectData : public CDialogEx
{
	DECLARE_DYNAMIC(CSelectData)

public:
	CSelectData(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSelectData();

// 对话框数据
	enum { IDD = IDD_Select_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedDefault();
	afx_msg void OnBnClickedUserdef();
	CString TableName_User;
};
