
// MySqlTestDlg.h : 头文件
//
#include<mysql.h>
#include<string>
#include"CMyButton.h"
#pragma once





// CMySqlTestDlg 对话框
class CMySqlTestDlg : public CDialogEx
{
// 构造
public:
	CMySqlTestDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MYSQLTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//User ：：：：：：
public:
	void InitMySQL();
	void WindowR();
	void MovEdit();

	void VarInit();
	void DefaultSearch();
	void SaveData();

	void getTitle();
	void ReColorBtn();
	void ShowText();


	CString InputInfo;//输入信息
	CString SaveInput;//保存输入
	CString g_Info;//显示消息
	std::string g_order; //sql命令
	CString TableName;//表名 获取来自selectDlg传送过来的表名字
	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnsrh();

	
	//内容变量
	CString m_college_info;
	CString m_class_info;
	CString m_cardnum_info;
	CString m_name_info;
	CString m_exp1;
	CString m_exp2;
	CString m_exp3;
	CString m_exp4;
	CString m_exp5;
	CString m_exp6;
	CString m_exp7;
	CString m_exp8;
	CString m_exp9;
	CString m_exp10;
	CString m_exp11;
	CString m_exp12;
	CString m_exp13;
	CString m_exp14;
	afx_msg void OnBnClickedSavadat();
	afx_msg void OnBnClickedBtnnew();
	afx_msg void OnBnClickedDeldat();

	//标题变量
	CString tm_college_info,TmAlter_college_info;
	CString tm_class_info,TmAlter_class_info;
	CString tm_cardnum_info,TmAlter_cardnum_info;
	CString tm_name_info,TmAlter_name_info;
	CString tm_exp1,TmAlter_exp1;
	CString tm_exp2,TmALter_exp2;
	CString tm_exp3,TmAlter_exp3;
	CString tm_exp4,TmAlter_exp4;
	CString tm_exp5,TmAlter_exp5;
	CString tm_exp6,TmAlter_exp6;
	CString tm_exp7,TmAlter_exp7;
	CString tm_exp8,TmAlter_exp8;
	CString tm_exp9,TmAlter_exp9;
	CString tm_exp10,TmAlter_exp10;
	CString tm_exp11,TmAlter_exp11;
	CString tm_exp12,TmAlter_exp12;
	CString tm_exp13,TmAlter_exp13;
	CString tm_exp14,TmAlter_exp14;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	CFont mFont;

	CMyButton m_BtnNew;
	CMyButton m_BtnSave;
	CMyButton m_BtnSrh;
	CMyButton m_BtnDel;
	CMyButton m_BtnOne;
	CMyButton m_BtnAll;
	CMyButton m_ETitle;
	afx_msg void OnBnClickedBtnall();
	afx_msg void OnBnClickedEtitle();

	//resource.h里的IDC_EDF类是表头
};
