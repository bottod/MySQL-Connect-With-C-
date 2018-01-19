
// MySqlTestDlg.cpp : 实现文件
//

//为了避免热键冲突 少注册系统热键

#include "stdafx.h"
#include "MySqlTest.h"
#include "MySqlTestDlg.h"
#include "afxdialogex.h"
#include<mysql.h>
#include<winsock.h>
#include"LoginDlg.h"
#include"CMyButton.h"
#include"SelectData.h"
#include<fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


MYSQL MyData; //SQL 数据类型
DWORD WINAPI PINGPROC(PVOID pParam);
HANDLE hTheadPing;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
CLoginDlg m_LoginDLG; //声明登录窗口
CSelectData m_SelectDataDlg;


// wchar_t to string  
void Wchar_tToString(std::string& szDst, wchar_t *wchar)  
{  
wchar_t * wText = wchar;  
DWORD dwNum = WideCharToMultiByte(CP_OEMCP,NULL,wText,-1,NULL,0,NULL,FALSE);// WideCharToMultiByte的运用  
char *psText; // psText为char*的临时数组，作为赋值给std::string的中间变量  
psText = new char[dwNum];  
WideCharToMultiByte (CP_OEMCP,NULL,wText,-1,psText,dwNum,NULL,FALSE);// WideCharToMultiByte的再次运用  
szDst = psText;// std::string赋值  
delete []psText;// psText的清除  
}  

//剧中窗口
void CenterWindowUser(HWND hWnd, int nSizeX, int nSizeY)
{
	int nWinX,nWinY,nClientX,nClientY;
	RECT rect;
	int nScreenX =GetSystemMetrics(SM_CXSCREEN);
	int nScreenY =GetSystemMetrics(SM_CYSCREEN);

	GetWindowRect(hWnd,&rect);
	nWinX = rect.right-rect.left;
	nWinY = rect.bottom-rect.top;
	
	GetClientRect(hWnd,&rect);
	nClientX = rect.right-rect.left;
	nClientY = rect.bottom-rect.top;
	
	nSizeX += (nWinX-nClientX);
	nSizeY += (nWinY-nClientY);
	
	MoveWindow(hWnd,(nScreenX-nSizeX)/2,(nScreenY-nSizeY)/2,nSizeX,nSizeY,TRUE);	
}

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMySqlTestDlg 对话框



CMySqlTestDlg::CMySqlTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMySqlTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMySqlTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMySqlTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTNSRH, &CMySqlTestDlg::OnBnClickedBtnsrh)
	ON_BN_CLICKED(IDC_SAVADAT, &CMySqlTestDlg::OnBnClickedSavadat)
	ON_BN_CLICKED(IDC_BTNNEW, &CMySqlTestDlg::OnBnClickedBtnnew)
	ON_BN_CLICKED(IDC_DELDAT, &CMySqlTestDlg::OnBnClickedDeldat)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTNALL, &CMySqlTestDlg::OnBnClickedBtnall)
	ON_BN_CLICKED(IDC_ETiTle, &CMySqlTestDlg::OnBnClickedEtitle)
END_MESSAGE_MAP()


// CMySqlTestDlg 消息处理程序

BOOL CMySqlTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	if (m_LoginDLG.DoModal() != IDOK)//DoMdal只有在模态窗口销毁时才返回
	{
		exit(-1);//出现内存泄漏问题
		//PostQuitMessage(-1);//正常退出
	}
	InitMySQL();//初始化连接数据库在登录后就执行 连接到特定库
	if (m_SelectDataDlg.DoModal()==IDCANCEL)//不处理两个按钮的退出事件 右上角的差默认响应IDCANCEL
	{
		//DoModal事件是在窗口退出时触发的 要在两个按钮上也加上退出事件
		//PostQuitMessage(-1);//正常退出
		exit(-1);//出现内存泄漏问题
	}
	TableName = m_SelectDataDlg.TableName_User;
	//MessageBox(TableName); //测试赋值进来的表名字有无错误 不允许出现一些非法字符
	WindowR(); //窗口
	ReColorBtn();
	CenterWindowUser(this->GetSafeHwnd(),700,540);
	//初始化变量
	VarInit();

	getTitle();
	
	hTheadPing = CreateThread(NULL,0,PINGPROC,NULL,0,NULL);

	return false;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMySqlTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMySqlTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		/*ShowText();
		GetDlgItem(IDC_INFO)->SetWindowTextW(g_Info);

		将鼠标移动到行尾
		CEdit *edit1 = (CEdit*)GetDlgItem(IDC_INFO);
		int nLen = GetDlgItem(IDC_INFO)->GetWindowTextLengthW();
		edit1->SetSel(nLen,nLen,true);
		edit1->SetFocus();*/

		MovEdit();//调整静态Edit

		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMySqlTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//键盘消息响应
BOOL CMySqlTestDlg::PreTranslateMessage(MSG* pMsg)
{
		if ((GetAsyncKeyState(48)==-32767) || (GetAsyncKeyState(96)==-32767))
		{
			InputInfo += "0";
		}
		if ((GetAsyncKeyState(49)==-32767) || (GetAsyncKeyState(97)==-32767))
		{
			InputInfo += "1";
		}
		if ((GetAsyncKeyState(50)==-32767) || (GetAsyncKeyState(98)==-32767))
		{
			InputInfo += "2";
		}
		if ((GetAsyncKeyState(51)==-32767) || (GetAsyncKeyState(99)==-32767))
		{
			InputInfo += "3";
		}
		if ((GetAsyncKeyState(52)==-32767) || (GetAsyncKeyState(100)==-32767))
		{
			InputInfo += "4";
		}
		if ((GetAsyncKeyState(53)==-32767) || (GetAsyncKeyState(101)==-32767))
		{
			InputInfo += "5";
		}
		if ((GetAsyncKeyState(54)==-32767) || (GetAsyncKeyState(102)==-32767))
		{
			InputInfo += "6";
		}
		if ((GetAsyncKeyState(55)==-32767) || (GetAsyncKeyState(103)==-32767))
		{
			InputInfo += "7";
		}
		if ((GetAsyncKeyState(56)==-32767) || (GetAsyncKeyState(104)==-32767))
		{
			InputInfo += "8";
		}
		if ((GetAsyncKeyState(57)==-32767) || (GetAsyncKeyState(105)==-32767))
		{
			InputInfo += "9";
		}
	// TODO: 在此添加专用代码和/或调用基类
	//if (pMsg->wParam == VK_RETURN)
	//{
	//	return true; //取消默认响应
	//}
	//需求变更需要 响应return
	if (pMsg->message == WM_KEYDOWN)
	{
		 //焦点在该窗口时响应
		switch (pMsg->wParam)
		{
	/*	case 48:
			InputInfo += "0";
			break;
		case 49:
			InputInfo += "1";	
			break;
		case 50:
			InputInfo += "2";
			break;
		case 51:
			InputInfo +="3";
			break;
		case 52:
			InputInfo +="4";	
			break;
		case 53:
			InputInfo +="5";
			break;
		case 54:
			InputInfo +="6";
			break;
		case 55:
			InputInfo +="7";
			break;
		case 56:
			InputInfo +="8";
			break;
		case 57:
			InputInfo +="9";
			break;*/
		case VK_RETURN:
			{
				CWnd *pWnd = GetFocus();
				int FocusID = pWnd->GetDlgCtrlID();
				if (FocusID == IDC_EDITCARDNUM)
				{
					VarInit();//运行一次后要 初始化变量清空InputInfo
					return 0;
				}
				else
				{
						//MessageBox(InputInfo);
						SaveInput = InputInfo; //保存键盘输入
						if( GetKeyState(VK_CONTROL)&0x80 )   //如果ctrl也按下
						{
							OnBnClickedBtnsrh();
							VarInit();
						}
						else
						{
							/*InputInfo = L"";	
							g_Info = L"";
							ShowOne();*/
							DefaultSearch(); //默认按照卡号查找  如果有同卡号的 默认第一个 请精确查找
							VarInit();//查找完重新初始化变量
						}
						//将鼠标移动到行尾
						/*CEdit *edit1 = (CEdit*)GetDlgItem(IDC_INFO);
						int nLen = GetDlgItem(IDC_INFO)->GetWindowTextLengthW();
						edit1->SetSel(nLen,nLen,true);

						edit1->SetFocus();*/

						return 0;//得加上 否则当焦点在edit上时 按下回车退出程序
				}
			}
			break;
		default:
			break;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

//初始化MySQL 并连接到user_info 数据库
void CMySqlTestDlg::InitMySQL()
{
	CString TempUsr,TempPSWD;
	TempUsr = m_LoginDLG.m_Usr;
	TempPSWD = m_LoginDLG.m_PassWD;
	std::string TempStringUsr(CW2A(TempUsr.GetString()));
	std::string TempStringPSWD(CW2A(TempPSWD.GetString()));

	if (mysql_library_init(0,NULL,NULL))
	{
		MessageBox(L"mysql_libray_init() failed");
		PostQuitMessage(-1);//正常退出
	}
	if (mysql_init(&MyData)==nullptr)
	{
		MessageBox(L"mysql_init() failed");
		PostQuitMessage(-1);//正常退出
	}
	if (mysql_options(&MyData,MYSQL_SET_CHARSET_NAME,"gbk"))
	{
		MessageBox(L"mysql_options() failed");
		PostQuitMessage(-1);//正常退出
	}
	//加以下两句使得断线能重连
	my_bool reConnect = 1;  
	mysql_options(&MyData, MYSQL_OPT_RECONNECT, &reConnect); 
	if (mysql_real_connect(&MyData,"101.132.70.232",TempStringUsr.c_str(),TempStringPSWD.c_str(),"user_info",0,NULL,0) == nullptr)//倒数第三个是端口号 user_info为数据库名
	{
		MessageBox(L"connect Failed!");
		PostQuitMessage(-1);//正常退出
		//用exit(-1)老是提示内存泄漏
	}
	
}

int CMySqlTestDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

		return 0;
}

void CMySqlTestDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	mysql_close(&MyData);
	mysql_server_end();  
	CloseHandle(hTheadPing);
	// TODO: 在此处添加消息处理程序代码
}

//调整窗口属性
void CMySqlTestDlg::WindowR()
{
	  //ModifyStyle(WS_SYSMENU, 0);   
    // 隐藏标题栏  
    //ModifyStyle(WS_CAPTION, 0);  
	SetWindowText(L"工程训练中心");
    // 去掉菜单栏  
    SetMenu(NULL);  
	mFont.CreatePointFont(90,L"微软雅黑");

	if(TableName != "student_info")
	{
		GetDlgItem(IDC_BTNALL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTNONE)->ShowWindow (SW_HIDE);
		GetDlgItem(IDC_BTNSRH)->ShowWindow (SW_HIDE);
		GetDlgItem(IDC_SAVADAT)->ShowWindow (SW_HIDE);
		GetDlgItem(IDC_BTNNEW)->ShowWindow (SW_HIDE);
		GetDlgItem(IDC_DELDAT)->ShowWindow (SW_HIDE);
		GetDlgItem(IDC_ETiTle)->ShowWindow (SW_HIDE);
		GetDlgItem(IDC_ETiTle)->SetFocus();
	}

	/*rectE.top = 600;
	rectE.left = 520;
	rectE.bottom = 1000;
	rectE.right = 1480;
	this->MoveWindow(&rect);
	GetDlgItem(IDC_INFO)->MoveWindow(&rectE);*/
}

//创建Edit 静态 ；；；；；；动态挺占CPU资源
void CMySqlTestDlg::MovEdit()
{
		//静态移动编辑框
	GetDlgItem(IDC_EDF1)->MoveWindow(CRect(10,10,120,40),false);
	GetDlgItem(IDC_EDITCOLLEGE)->MoveWindow(CRect(130,10,260,40),false);

	GetDlgItem(IDC_EDF2)->MoveWindow(CRect(10,60,120,90),false);
	GetDlgItem(IDC_EDITCLASS)->MoveWindow(CRect(130,60,260,90),false);

	GetDlgItem(IDC_EDF3)->MoveWindow(CRect(10,110,120,140),false);
	GetDlgItem(IDC_EDITCARDNUM)->MoveWindow(CRect(130,110,260,140),false);

	GetDlgItem(IDC_EDF4)->MoveWindow(CRect(10,160,120,190),false);
	GetDlgItem(IDC_EDITNAME)->MoveWindow(CRect(130,160,260,190),false);

	GetDlgItem(IDC_EDF5)->MoveWindow(CRect(10,210,120,240),false);
	GetDlgItem(IDC_EXP1)->MoveWindow(CRect(130,210,260,240),false);

	GetDlgItem(IDC_EDF6)->MoveWindow(CRect(10,260,120,290),false);
	GetDlgItem(IDC_EXP2)->MoveWindow(CRect(130,260,260,290),false);

	GetDlgItem(IDC_EDF7)->MoveWindow(CRect(10,310,120,340),false);
	GetDlgItem(IDC_EXP3)->MoveWindow(CRect(130,310,260,340),false);

	GetDlgItem(IDC_EDF8)->MoveWindow(CRect(10,360,120,390),false);
	GetDlgItem(IDC_EXP4)->MoveWindow(CRect(130,360,260,390),false);

	GetDlgItem(IDC_EDF9)->MoveWindow(CRect(10,410,120,440),false);
	GetDlgItem(IDC_EXP5)->MoveWindow(CRect(130,410,260,440),false);

	GetDlgItem(IDC_EDF10)->MoveWindow(CRect(270,10,380,40),false);
	GetDlgItem(IDC_EXP6)->MoveWindow(CRect(390,10,520,40),false);//这里所有的rect是相对窗口来看的

	GetDlgItem(IDC_EDF11)->MoveWindow(CRect(270,60,380,90),false);
	GetDlgItem(IDC_EXP7)->MoveWindow(CRect(390,60,520,90),false);
	
	GetDlgItem(IDC_EDF12)->MoveWindow(CRect(270,110,380,140),false);
	GetDlgItem(IDC_EXP8)->MoveWindow(CRect(390,110,520,140),false);
	
	GetDlgItem(IDC_EDF13)->MoveWindow(CRect(270,160,380,190),false);
	GetDlgItem(IDC_EXP9)->MoveWindow(CRect(390,160,520,190),false);
	
	GetDlgItem(IDC_EDF14)->MoveWindow(CRect(270,210,380,240),false);
	GetDlgItem(IDC_EXP10)->MoveWindow(CRect(390,210,520,240),false);
	
	GetDlgItem(IDC_EDF15)->MoveWindow(CRect(270,260,380,290),false);
	GetDlgItem(IDC_EXP11)->MoveWindow(CRect(390,260,520,290),false);
	
	GetDlgItem(IDC_EDF16)->MoveWindow(CRect(270,310,380,340),false);
	GetDlgItem(IDC_EXP12)->MoveWindow(CRect(390,310,520,340),false);
	
	GetDlgItem(IDC_EDF17)->MoveWindow(CRect(270,360,380,390),false);
	GetDlgItem(IDC_EXP13)->MoveWindow(CRect(390,360,520,390),false);
	
	GetDlgItem(IDC_EDF18)->MoveWindow(CRect(270,410,380,440),false);
	GetDlgItem(IDC_EXP14)->MoveWindow(CRect(390,410,520,440),false);

	//修改按钮位置
	GetDlgItem(IDC_DELDAT)->MoveWindow(CRect(540,110,680,140),false);
	GetDlgItem(IDC_BTNSRH)->MoveWindow(CRect(540,160,680,190),false);
	GetDlgItem(IDC_SAVADAT)->MoveWindow(CRect(540,210,680,240),false);
	GetDlgItem(IDC_BTNNEW)->MoveWindow(CRect(540,260,680,290),false);
	GetDlgItem(IDC_ETiTle)->MoveWindow(CRect(540,310,680,340),false);
	//大按钮位置
	GetDlgItem(IDC_BTNALL)->MoveWindow(CRect(600,445,690,535),false);
	GetDlgItem(IDC_BTNONE)->MoveWindow(CRect(500,445,590,535),false);

	GetDlgItem(IDC_STATIC_INFO)->MoveWindow(CRect(10,480,400,510),false);
	GetDlgItem(IDC_STATIC_INFO)->SetWindowTextW(L"等待动作 ...");
}

//姓名查找按钮
void CMySqlTestDlg::OnBnClickedBtnsrh()
{
	VarInit();

	//执行query前先检查是否与mysql还建立连接
	mysql_ping(&MyData);
	
	MYSQL_RES *result = NULL;
	CString TempCard = L"";
	GetDlgItem(IDC_EDITCARDNUM)->GetWindowTextW(TempCard);
	if (TempCard == L"")
	{
			CString TempInfo = L"";
			g_order = "SELECT * FROM ";
			std::string tempTableString;
			tempTableString = CW2A(TableName.GetString());
			g_order += tempTableString;
			g_order += " WHERE ";

			GetDlgItem(IDC_EDITNAME)->GetWindowTextW(m_name_info);
			std::string TempString;
			TempString = CW2A(m_name_info.GetString());
			std::string TempStringTitle;
			TempStringTitle = CW2A(tm_name_info.GetString());
			g_order += TempStringTitle;
			g_order += "='";
			g_order += TempString;
			g_order +="';";

			if (mysql_query(&MyData,g_order.c_str()) == 0)
			{
				 //一次性取得数据集  
				result = mysql_store_result(&MyData);
				 //取得并打印行数  
				//int rowcount = mysql_num_rows(result); 
				 //取得并打印各字段的名称  
				unsigned int fieldcount = mysql_num_fields(result);  //列数
				// MYSQL_FIELD *field = NULL;  
				//for (unsigned int i = 0; i < fieldcount; i++) 
				//{  
			   //     field = mysql_fetch_field_direct(result, i);  
			   //     TempInfo = field->name;
				//	g_Info += TempInfo + L"\t\t";
			  //  } 
			//	g_Info += L"\r\n";
				 //打印各行  
				MYSQL_ROW row = NULL;  
				row = mysql_fetch_row(result);  
				if(NULL != row) 
				{  
					TempInfo = row[0];
					m_college_info = TempInfo;
					TempInfo = row[1];
					m_class_info = TempInfo;  
					TempInfo = row[2];
					m_cardnum_info = TempInfo;  
					TempInfo = row[3];
					m_name_info = TempInfo;      
					TempInfo = row[4];
					m_exp1 = TempInfo; 
					TempInfo = row[5];
					m_exp2 = TempInfo;  
					TempInfo = row[6];
					m_exp3 = TempInfo; 
					TempInfo = row[7];
					m_exp4 = TempInfo;   
					TempInfo = row[8];
					m_exp5 = TempInfo;  
					TempInfo = row[9];
					m_exp6 = TempInfo;  
					TempInfo = row[10];
					m_exp7 = TempInfo; 
					TempInfo = row[11];
					m_exp8 = TempInfo;  
					TempInfo = row[12];
					m_exp9 = TempInfo;
					TempInfo = row[13];
					m_exp10 = TempInfo;  
					TempInfo = row[14];
					m_exp11 = TempInfo;   
					TempInfo = row[15];
					m_exp12 = TempInfo; 
					TempInfo = row[16];
					m_exp13 = TempInfo;
					TempInfo = row[17];
					m_exp14 = TempInfo;   

					GetDlgItem(IDC_STATIC_INFO)->SetWindowTextW(L"成功找到数据!");
				}  
				else
				{
					GetDlgItem(IDC_STATIC_INFO)->SetWindowTextW(L"未找到该数据!");
				}
			}
	}
	else
	{
			CString TempInfo = L"";
			g_order = "SELECT * FROM ";
			std::string tempTableString;
			tempTableString = CW2A(TableName.GetString());
			g_order += tempTableString;
			g_order += " WHERE ";

			GetDlgItem(IDC_EDITNAME)->GetWindowTextW(m_name_info);
			std::string TempString;
			TempString = CW2A(m_name_info.GetString());
			std::string TempStringTitle;
			TempStringTitle = CW2A(tm_name_info.GetString());
			g_order += TempStringTitle;
			g_order += "='";
			g_order += TempString;
			TempStringTitle = CW2A(tm_cardnum_info.GetString());
			g_order +="' AND ";
			g_order += TempStringTitle;
			g_order +="='";
			TempString = CW2A(TempCard.GetString());
			g_order += TempString;
			g_order +="';";
			result = NULL;
			if (mysql_query(&MyData,g_order.c_str()) == 0)
			{
				 //一次性取得数据集  
				result = mysql_store_result(&MyData);
				 //取得并打印行数  
				//int rowcount = mysql_num_rows(result); 
				 //取得并打印各字段的名称  
				unsigned int fieldcount = mysql_num_fields(result);  //列数
				// MYSQL_FIELD *field = NULL;  
				//for (unsigned int i = 0; i < fieldcount; i++) 
				//{  
			   //     field = mysql_fetch_field_direct(result, i);  
			   //     TempInfo = field->name;
				//	g_Info += TempInfo + L"\t\t";
			  //  } 
			//	g_Info += L"\r\n";
				 //打印各行  
				MYSQL_ROW row = NULL;  
				row = mysql_fetch_row(result);  
				if(NULL != row) 
				{  
					TempInfo = row[0];
					m_college_info = TempInfo;
					TempInfo = row[1];
					m_class_info = TempInfo;  
					TempInfo = row[2];
					m_cardnum_info = TempInfo;  
					TempInfo = row[3];
					m_name_info = TempInfo;      
					TempInfo = row[4];
					m_exp1 = TempInfo; 
					TempInfo = row[5];
					m_exp2 = TempInfo;  
					TempInfo = row[6];
					m_exp3 = TempInfo; 
					TempInfo = row[7];
					m_exp4 = TempInfo;   
					TempInfo = row[8];
					m_exp5 = TempInfo;  
					TempInfo = row[9];
					m_exp6 = TempInfo;  
					TempInfo = row[10];
					m_exp7 = TempInfo; 
					TempInfo = row[11];
					m_exp8 = TempInfo;  
					TempInfo = row[12];
					m_exp9 = TempInfo;
					TempInfo = row[13];
					m_exp10 = TempInfo;  
					TempInfo = row[14];
					m_exp11 = TempInfo;   
					TempInfo = row[15];
					m_exp12 = TempInfo; 
					TempInfo = row[16];
					m_exp13 = TempInfo;
					TempInfo = row[17];
					m_exp14 = TempInfo;   

					GetDlgItem(IDC_STATIC_INFO)->SetWindowTextW(L"成功找到数据!");
				}  
				else
				{
					GetDlgItem(IDC_STATIC_INFO)->SetWindowTextW(L"未找到该数据!");
				}
			}
	}

	GetDlgItem(IDC_EDITCOLLEGE)->SetWindowTextW(m_college_info);
	GetDlgItem(IDC_EDITCLASS)->SetWindowTextW(m_class_info);
	GetDlgItem(IDC_EDITCARDNUM)->SetWindowTextW(m_cardnum_info);
	GetDlgItem(IDC_EDITNAME)->SetWindowTextW(m_name_info);
	GetDlgItem(IDC_EXP1)->SetWindowTextW(m_exp1);
	GetDlgItem(IDC_EXP2)->SetWindowTextW(m_exp2);
	GetDlgItem(IDC_EXP3)->SetWindowTextW(m_exp3);
	GetDlgItem(IDC_EXP4)->SetWindowTextW(m_exp4);
	GetDlgItem(IDC_EXP5)->SetWindowTextW(m_exp5);
	GetDlgItem(IDC_EXP6)->SetWindowTextW(m_exp6);
	GetDlgItem(IDC_EXP7)->SetWindowTextW(m_exp7);
	GetDlgItem(IDC_EXP8)->SetWindowTextW(m_exp8);
	GetDlgItem(IDC_EXP9)->SetWindowTextW(m_exp9);
	GetDlgItem(IDC_EXP10)->SetWindowTextW(m_exp10);
	GetDlgItem(IDC_EXP11)->SetWindowTextW(m_exp11);
	GetDlgItem(IDC_EXP12)->SetWindowTextW(m_exp12);
	GetDlgItem(IDC_EXP13)->SetWindowTextW(m_exp13);
	GetDlgItem(IDC_EXP14)->SetWindowTextW(m_exp14);

	mysql_free_result(result);  

	getTitle();
	// TODO: 在此添加控件通知处理程序代码
}

//默认卡号查找
void CMySqlTestDlg::DefaultSearch()
{

	mysql_ping(&MyData);

	CString TempInfo = L"";
	g_order = "SELECT * FROM ";
	std::string tempTableString;
	tempTableString = CW2A(TableName.GetString());
	g_order += tempTableString;
	g_order += " WHERE ";
	std::string TempString(CW2A(SaveInput.GetString()));
	std::string TempStringTitle;
	TempStringTitle = CW2A(tm_cardnum_info.GetString());
	g_order += TempStringTitle;
	g_order +="='";
	//以下两句是为了 在字符输入为空时 不查找全部并输出第一个
	if (strlen(TempString.c_str())==0)
		TempString = "****";

	g_order += TempString;
	g_order +="';";

	MYSQL_RES *result = NULL;
	if (mysql_query(&MyData,g_order.c_str()) == 0)
	{
		 //一次性取得数据集  
		result = mysql_store_result(&MyData);
		 //取得并打印行数  
        //int rowcount = mysql_num_rows(result); 
		 //取得并打印各字段的名称  
        unsigned int fieldcount = mysql_num_fields(result);  //列数
		// MYSQL_FIELD *field = NULL;  
        //for (unsigned int i = 0; i < fieldcount; i++) 
		//{  
       //     field = mysql_fetch_field_direct(result, i);  
       //     TempInfo = field->name;
		//	g_Info += TempInfo + L"\t\t";
      //  } 
	//	g_Info += L"\r\n";
		 //打印各行  
        MYSQL_ROW row = NULL;  
        row = mysql_fetch_row(result);  
        if(NULL != row) 
		{  
			TempInfo = row[0];
			m_college_info = TempInfo;
			TempInfo = row[1];
			m_class_info = TempInfo;  
			TempInfo = row[2];
			m_cardnum_info = TempInfo;  
			if(fieldcount>3)
			{
				TempInfo = row[3];
				m_name_info = TempInfo; 
			}
			else
				m_name_info = ""; 
			if (fieldcount>4)
			{
				TempInfo = row[4];
				m_exp1 = TempInfo; 
			}
			else
				m_exp1 = ""; 
			if (fieldcount>5)
			{
				TempInfo = row[5];
				m_exp2 = TempInfo; 
			}
			else
				m_exp2 = ""; 
			if (fieldcount>6)
			{
				TempInfo = row[6];
				m_exp3 = TempInfo; 
			}
			else
				m_exp3 = ""; 
			if (fieldcount>7)
			{
				TempInfo = row[7];
				m_exp4 = TempInfo; 
			}
			else
				m_exp4 = ""; 
			if (fieldcount>8)
			{
				TempInfo = row[8];
				m_exp5 = TempInfo; 
			}
			else
				m_exp5 = ""; 
			if (fieldcount>9)
			{
				TempInfo = row[9];
				m_exp6 = TempInfo; 
			}
			else
				m_exp6 = ""; 
			if (fieldcount>10)
			{
				TempInfo = row[10];
				m_exp7 = TempInfo; 
			}
			else
				m_exp7 = ""; 
			if (fieldcount>11)
			{
				TempInfo = row[11];
				m_exp8 = TempInfo; 
			}
			else
				m_exp8 = ""; 
			if (fieldcount>12)
			{
				TempInfo = row[12];
				m_exp9 = TempInfo; 
			}
			else
				m_exp9 = ""; 
			if (fieldcount>13)
			{
				TempInfo = row[13];
				m_exp10 = TempInfo; 
			}
			else
				m_exp10 = "";   
			if (fieldcount>14)
			{
				TempInfo = row[14];
				m_exp11 = TempInfo; 
			}
			else
				m_exp11 = "";  
			if (fieldcount>15)
			{
				TempInfo = row[15];
				m_exp12 = TempInfo; 
			}
			else
				m_exp12 = ""; 
			if (fieldcount>16)
			{
				TempInfo = row[16];
				m_exp13 = TempInfo; 
			}
			else
				m_exp13 = ""; 
			if (fieldcount>17)
			{
				TempInfo = row[17];
				m_exp14 = TempInfo; 
			}
			else
				m_exp14 = ""; 
			GetDlgItem(IDC_STATIC_INFO)->SetWindowTextW(L"成功找到数据!");
        } 
		else
		{
			GetDlgItem(IDC_STATIC_INFO)->SetWindowTextW(L"未找到该数据!");
		}
	}
	GetDlgItem(IDC_EDITCOLLEGE)->SetWindowTextW(m_college_info);
	GetDlgItem(IDC_EDITCLASS)->SetWindowTextW(m_class_info);
	GetDlgItem(IDC_EDITCARDNUM)->SetWindowTextW(m_cardnum_info);
	GetDlgItem(IDC_EDITNAME)->SetWindowTextW(m_name_info);
	GetDlgItem(IDC_EXP1)->SetWindowTextW(m_exp1);
	GetDlgItem(IDC_EXP2)->SetWindowTextW(m_exp2);
	GetDlgItem(IDC_EXP3)->SetWindowTextW(m_exp3);
	GetDlgItem(IDC_EXP4)->SetWindowTextW(m_exp4);
	GetDlgItem(IDC_EXP5)->SetWindowTextW(m_exp5);
	GetDlgItem(IDC_EXP6)->SetWindowTextW(m_exp6);
	GetDlgItem(IDC_EXP7)->SetWindowTextW(m_exp7);
	GetDlgItem(IDC_EXP8)->SetWindowTextW(m_exp8);
	GetDlgItem(IDC_EXP9)->SetWindowTextW(m_exp9);
	GetDlgItem(IDC_EXP10)->SetWindowTextW(m_exp10);
	GetDlgItem(IDC_EXP11)->SetWindowTextW(m_exp11);
	GetDlgItem(IDC_EXP12)->SetWindowTextW(m_exp12);
	GetDlgItem(IDC_EXP13)->SetWindowTextW(m_exp13);
	GetDlgItem(IDC_EXP14)->SetWindowTextW(m_exp14);
	if(TableName != "student_info")
	{
		SYSTEMTIME sys;
		wchar_t TempTime[200];
		GetLocalTime( &sys ); 
		swprintf_s(TempTime,L"%4d/%2d/%2d/%2d:%2d",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute);
		//GetDlgItem(IDC_EDITCOLLEGE)->SetWindowTextW(TempTime);

		g_order = "UPDATE ";
		std::string tempTableString;
		tempTableString = CW2A(TableName.GetString());
		g_order += tempTableString;
		g_order += " SET `";
		std::string TempStringTitle;
		TempStringTitle = CW2A(tm_class_info.GetString());
		g_order += TempStringTitle;
		g_order += "`='";
		Wchar_tToString(TempStringTitle,TempTime);
		g_order += TempStringTitle;
		g_order +="' WHERE `";
		TempStringTitle = CW2A(tm_cardnum_info.GetString());
		g_order += TempStringTitle;
		g_order += "`='";
		g_order += TempString;
		g_order += "';";

			if (mysql_query(&MyData,g_order.c_str()) == 0)
			{
				GetDlgItem(IDC_STATIC_INFO)->SetWindowTextW(L"保存数据成功!");
			}
			else
			{
				GetDlgItem(IDC_STATIC_INFO)->SetWindowTextW(L"错误！保存失败！");
			}
			//VarInit();
	}

	mysql_free_result(result);  

	getTitle();
	// TODO: 在此添加控件通知处理程序代码
}

//变量初始化
void CMySqlTestDlg::VarInit()
{
	g_Info = "";
	g_order = "";
	InputInfo = L"";
	SaveInput = L"";

	m_college_info = L"";
	m_class_info = L"";
	m_name_info = L"";
	m_cardnum_info = L"";
	m_exp1 = L"";
	m_exp2 = L"";
	m_exp3 = L"";
	m_exp4 = L"";
	m_exp5 = L"";
	m_exp6 = L"";
	m_exp7 = L"";
	m_exp8 = L"";
	m_exp9 = L"";
	m_exp10 = L"";
	m_exp11 = L"";
	m_exp12 = L"";
	m_exp13 = L"";
	m_exp14 = L"";
}

//保存信息 姓名不能变
void CMySqlTestDlg::SaveData()
{

	mysql_ping(&MyData);

	MYSQL_RES *result;
	//得到信息
	GetDlgItem(IDC_EDITCOLLEGE)->GetWindowTextW(m_college_info);
	GetDlgItem(IDC_EDITCLASS)->GetWindowTextW(m_class_info);
	GetDlgItem(IDC_EDITCARDNUM)->GetWindowTextW(m_cardnum_info);
	GetDlgItem(IDC_EDITNAME)->GetWindowTextW(m_name_info);
	GetDlgItem(IDC_EXP1)->GetWindowTextW(m_exp1);
	GetDlgItem(IDC_EXP2)->GetWindowTextW(m_exp2);
	GetDlgItem(IDC_EXP3)->GetWindowTextW(m_exp3);
	GetDlgItem(IDC_EXP4)->GetWindowTextW(m_exp4);
	GetDlgItem(IDC_EXP5)->GetWindowTextW(m_exp5);
	GetDlgItem(IDC_EXP6)->GetWindowTextW(m_exp6);
	GetDlgItem(IDC_EXP7)->GetWindowTextW(m_exp7);
	GetDlgItem(IDC_EXP8)->GetWindowTextW(m_exp8);
	GetDlgItem(IDC_EXP9)->GetWindowTextW(m_exp9);
	GetDlgItem(IDC_EXP10)->GetWindowTextW(m_exp10);
	GetDlgItem(IDC_EXP11)->GetWindowTextW(m_exp11);
	GetDlgItem(IDC_EXP12)->GetWindowTextW(m_exp12);
	GetDlgItem(IDC_EXP13)->GetWindowTextW(m_exp13);
	GetDlgItem(IDC_EXP14)->GetWindowTextW(m_exp14);
	
	CString TempInfo = L"";
	std::string TempString;
		//保存前先查找有无该人
					g_order = "SELECT * FROM ";
					std::string tempTableString;
					tempTableString = CW2A(TableName.GetString());
					g_order += tempTableString;
					g_order += " WHERE ";
					TempString = CW2A(m_name_info.GetString());
					std::string TempStringTitle;
					TempStringTitle = CW2A(tm_name_info.GetString());
					g_order += TempStringTitle;
					g_order += "='";
					g_order += TempString;
					g_order +="';";

					result = NULL;
					//if查找语句执行错误就不继续执行
					if (mysql_query(&MyData,g_order.c_str()) == 0)
					{
								 //一次性取得数据集  
							result = mysql_store_result(&MyData);
							//取得并打印行数  
								int rowcount = mysql_num_rows(result); 
								 //取得并打印各字段的名称  
							unsigned int fieldcount = mysql_num_fields(result);  //列数
								// MYSQL_FIELD *field = NULL;  
								//for (unsigned int i = 0; i < fieldcount; i++) 
								//{  
							   //     field = mysql_fetch_field_direct(result, i);  
							   //     TempInfo = field->name;
								//	g_Info += TempInfo + L"\t\t";
							  //  } 
							//	g_Info += L"\r\n";
								 //打印各行  
							MYSQL_ROW row = NULL;  
							row = mysql_fetch_row(result);  
							if(NULL != row && rowcount == 1) //如果只有一条数据
							{
								std::string TempStringTitle;
								TempStringTitle = CW2A(tm_class_info.GetString());
								g_order = "UPDATE ";
								std::string tempTableString;
								tempTableString = CW2A(TableName.GetString());
								g_order += tempTableString;
								g_order += " SET ";
								g_order += TempStringTitle;
								g_order += "='";
								TempString = CW2A(m_class_info.GetString());
								g_order += TempString;
								g_order +="',";

								TempStringTitle = CW2A(tm_college_info.GetString());
								g_order += TempStringTitle;
								g_order += "='";
								TempString = CW2A(m_college_info.GetString());
								g_order += TempString;
								g_order +="',";

								TempStringTitle = CW2A(tm_cardnum_info.GetString());
								g_order += TempStringTitle;
								g_order += "='";
								TempString = CW2A(m_cardnum_info.GetString());
								g_order += TempString;
								g_order +="',";

								TempStringTitle = CW2A(tm_name_info.GetString());
								g_order += TempStringTitle;
								g_order += "='";
								TempString = CW2A(m_name_info.GetString());
								g_order += TempString;
								g_order +="',";

								TempStringTitle = CW2A(tm_exp1.GetString());
								g_order += TempStringTitle;
								g_order += "='";
								TempString = CW2A(m_exp1.GetString());
								g_order += TempString;
								g_order +="',";

								TempStringTitle = CW2A(tm_exp2.GetString());
								g_order += TempStringTitle;
								g_order += "='";
								TempString = CW2A(m_exp2.GetString());
								g_order += TempString;
								g_order +="',";

								TempStringTitle = CW2A(tm_exp3.GetString());
								g_order += TempStringTitle;
								g_order += "='";
								TempString = CW2A(m_exp3.GetString());
								g_order += TempString;
								g_order +="',";

								TempStringTitle = CW2A(tm_exp4.GetString());
								g_order += TempStringTitle;
								g_order += "='";
								TempString = CW2A(m_exp4.GetString());
								g_order += TempString;
								g_order +="',";

								TempStringTitle = CW2A(tm_exp5.GetString());
								g_order += TempStringTitle;
								g_order += "='";
								TempString = CW2A(m_exp5.GetString());
								g_order += TempString;
								g_order +="',";

								TempStringTitle = CW2A(tm_exp6.GetString());
								g_order += TempStringTitle;
								g_order += "='";
								TempString = CW2A(m_exp6.GetString());
								g_order += TempString;
								g_order +="',";

								TempStringTitle = CW2A(tm_exp7.GetString());
								g_order += TempStringTitle;
								g_order += "='";
								TempString = CW2A(m_exp7.GetString());
								g_order += TempString;
								g_order +="',";

								TempStringTitle = CW2A(tm_exp8.GetString());
								g_order += TempStringTitle;
								g_order += "='";
								TempString = CW2A(m_exp8.GetString());
								g_order += TempString;
								g_order +="',";

								TempStringTitle = CW2A(tm_exp9.GetString());
								g_order += TempStringTitle;
								g_order += "='";
								TempString = CW2A(m_exp9.GetString());
								g_order += TempString;
								g_order +="',";

								TempStringTitle = CW2A(tm_exp10.GetString());
								g_order += TempStringTitle;
								g_order += "='";
								TempString = CW2A(m_exp10.GetString());
								g_order += TempString;
								g_order +="',";

								TempStringTitle = CW2A(tm_exp11.GetString());
								g_order += TempStringTitle;
								g_order += "='";
								TempString = CW2A(m_exp11.GetString());
								g_order += TempString;
								g_order +="',";

								TempStringTitle = CW2A(tm_exp12.GetString());
								g_order += TempStringTitle;
								g_order += "='";
								TempString = CW2A(m_exp12.GetString());
								g_order += TempString;
								g_order +="',";

								TempStringTitle = CW2A(tm_exp13.GetString());
								g_order += TempStringTitle;
								g_order += "='";
								TempString = CW2A(m_exp13.GetString());
								g_order += TempString;
								g_order +="',";

								TempStringTitle = CW2A(tm_exp14.GetString());
								g_order += TempStringTitle;
								g_order += "='";
								TempString = CW2A(m_exp14.GetString());
								g_order += TempString;
								g_order +="' WHERE ";
								TempStringTitle = CW2A(tm_name_info.GetString());
								g_order += TempStringTitle;
								g_order += "='";
	
								TempString = CW2A(m_name_info.GetString());
								g_order += TempString;
								g_order +="';";

								if (mysql_query(&MyData,g_order.c_str()) == 0)
								{
									GetDlgItem(IDC_STATIC_INFO)->SetWindowTextW(L"保存数据成功!");
								}
								else
								{
									GetDlgItem(IDC_STATIC_INFO)->SetWindowTextW(L"错误！保存失败！");
								}

								VarInit();
							}
							else //如果有多条数据
							{
								g_order = "SELECT * FROM ";
								std::string tempTableString;
								tempTableString = CW2A(TableName.GetString());
								g_order += tempTableString;
								g_order += " WHERE ";
								TempString = CW2A(m_name_info.GetString());
								std::string TempStringTitle;
								TempStringTitle = CW2A(tm_name_info.GetString());
								g_order += TempStringTitle;
								g_order += "='";
								g_order += TempString;
								g_order +="' AND ";
								TempStringTitle = CW2A(m_cardnum_info.GetString());
								g_order += TempStringTitle;
								g_order += "='";
								TempString = CW2A(m_cardnum_info.GetString());
								g_order += TempString;
								g_order +="';";

								result = NULL;
								//if查找语句执行错误就不继续执行
								if (mysql_query(&MyData,g_order.c_str()) == 0)
								{
									 //一次性取得数据集  
									result = mysql_store_result(&MyData);
									//取得并打印行数  
									int rowcount = mysql_num_rows(result); 
									if (rowcount == 1)
									{
											g_order = "UPDATE ";
											std::string tempTableString;
											tempTableString = CW2A(TableName.GetString());
											g_order += tempTableString;
											g_order += " SET ";
											std::string TempStringTitle;
											TempStringTitle = CW2A(tm_class_info.GetString());
											g_order += TempStringTitle;
											g_order += "='";
											TempString = CW2A(m_class_info.GetString());
											g_order += TempString;
											g_order +="',";

											TempStringTitle = CW2A(tm_college_info.GetString());
											g_order += TempStringTitle;
											g_order += "='";
											TempString = CW2A(m_college_info.GetString());
											g_order += TempString;
											g_order +="',";

											TempStringTitle = CW2A(tm_cardnum_info.GetString());
											g_order += TempStringTitle;
											g_order += "='";
											TempString = CW2A(m_cardnum_info.GetString());
											g_order += TempString;
											g_order +="',";

											TempStringTitle = CW2A(tm_name_info.GetString());
											g_order += TempStringTitle;
											g_order += "='";
											TempString = CW2A(m_name_info.GetString());
											g_order += TempString;
											g_order +="',";

											TempStringTitle = CW2A(tm_exp1.GetString());
											g_order += TempStringTitle;
											g_order += "='";
											TempString = CW2A(m_exp1.GetString());
											g_order += TempString;
											g_order +="',";

											TempStringTitle = CW2A(tm_exp2.GetString());
											g_order += TempStringTitle;
											g_order += "='";
											TempString = CW2A(m_exp2.GetString());
											g_order += TempString;
											g_order +="',";

											TempStringTitle = CW2A(tm_exp3.GetString());
											g_order += TempStringTitle;
											g_order += "='";
											TempString = CW2A(m_exp3.GetString());
											g_order += TempString;
											g_order +="',";

											TempStringTitle = CW2A(tm_exp4.GetString());
											g_order += TempStringTitle;
											g_order += "='";
											TempString = CW2A(m_exp4.GetString());
											g_order += TempString;
											g_order +="',";

											TempStringTitle = CW2A(tm_exp5.GetString());
											g_order += TempStringTitle;
											g_order += "='";
											TempString = CW2A(m_exp5.GetString());
											g_order += TempString;
											g_order +="',";

											TempStringTitle = CW2A(tm_exp6.GetString());
											g_order += TempStringTitle;
											g_order += "='";
											TempString = CW2A(m_exp6.GetString());
											g_order += TempString;
											g_order +="',";

											TempStringTitle = CW2A(tm_exp7.GetString());
											g_order += TempStringTitle;
											g_order += "='";
											TempString = CW2A(m_exp7.GetString());
											g_order += TempString;
											g_order +="',";

											TempStringTitle = CW2A(tm_exp8.GetString());
											g_order += TempStringTitle;
											g_order += "='";
											TempString = CW2A(m_exp8.GetString());
											g_order += TempString;
											g_order +="',";

											TempStringTitle = CW2A(tm_exp9.GetString());
											g_order += TempStringTitle;
											g_order += "='";
											TempString = CW2A(m_exp9.GetString());
											g_order += TempString;
											g_order +="',";

											TempStringTitle = CW2A(tm_exp10.GetString());
											g_order += TempStringTitle;
											g_order += "='";
											TempString = CW2A(m_exp10.GetString());
											g_order += TempString;
											g_order +="',";

											TempStringTitle = CW2A(tm_exp11.GetString());
											g_order += TempStringTitle;
											g_order += "='";
											TempString = CW2A(m_exp11.GetString());
											g_order += TempString;
											g_order +="',";

											TempStringTitle = CW2A(tm_exp12.GetString());
											g_order += TempStringTitle;
											g_order += "='";
											TempString = CW2A(m_exp12.GetString());
											g_order += TempString;
											g_order +="',";

											TempStringTitle = CW2A(tm_exp13.GetString());
											g_order += TempStringTitle;
											g_order += "='";
											TempString = CW2A(m_exp13.GetString());
											g_order += TempString;
											g_order +="',";

											TempStringTitle = CW2A(tm_exp14.GetString());
											g_order += TempStringTitle;
											g_order += "='";
											TempString = CW2A(m_exp14.GetString());
											g_order += TempString;
											g_order +="' WHERE ";
											TempStringTitle = CW2A(tm_name_info.GetString());
											g_order += TempStringTitle;
											g_order += "='";
	
											TempString = CW2A(m_name_info.GetString());
											g_order += TempString;
											g_order +="' AND ";
											TempStringTitle = CW2A(tm_cardnum_info.GetString());
											g_order += TempStringTitle;
											g_order += "='";

											TempString = CW2A(m_cardnum_info.GetString());
											g_order += TempString;
											g_order +="';";

											if (mysql_query(&MyData,g_order.c_str()) == 0)
											{
												GetDlgItem(IDC_STATIC_INFO)->SetWindowTextW(L"保存数据成功!");
											}
											else
											{
												GetDlgItem(IDC_STATIC_INFO)->SetWindowTextW(L"错误! 保存失败");
											}

											VarInit();
									}
									else
									{
										GetDlgItem(IDC_STATIC_INFO)->SetWindowTextW(L"错误！未找到 或有多个！");
									}
							}
							else
							{
								GetDlgItem(IDC_STATIC_INFO)->SetWindowTextW(L"未知错误(查找出错)");
							}
					}
					
			}
			else
			{
				GetDlgItem(IDC_STATIC_INFO)->SetWindowTextW(L"未知错误(网络延迟断线重连)"); //查找语句出错
			}
	}

//保存按钮 基于现有数据
void CMySqlTestDlg::OnBnClickedSavadat()
{
	SaveData();
	// TODO: 在此添加控件通知处理程序代码
}

//新增数据
void CMySqlTestDlg::OnBnClickedBtnnew()
{

	mysql_ping(&MyData);

	//得到信息
	GetDlgItem(IDC_EDITCOLLEGE)->GetWindowTextW(m_college_info);
	GetDlgItem(IDC_EDITCLASS)->GetWindowTextW(m_class_info);
	GetDlgItem(IDC_EDITCARDNUM)->GetWindowTextW(m_cardnum_info);
	GetDlgItem(IDC_EDITNAME)->GetWindowTextW(m_name_info);
	GetDlgItem(IDC_EXP1)->GetWindowTextW(m_exp1);
	GetDlgItem(IDC_EXP2)->GetWindowTextW(m_exp2);
	GetDlgItem(IDC_EXP3)->GetWindowTextW(m_exp3);
	GetDlgItem(IDC_EXP4)->GetWindowTextW(m_exp4);
	GetDlgItem(IDC_EXP5)->GetWindowTextW(m_exp5);
	GetDlgItem(IDC_EXP6)->GetWindowTextW(m_exp6);
	GetDlgItem(IDC_EXP7)->GetWindowTextW(m_exp7);
	GetDlgItem(IDC_EXP8)->GetWindowTextW(m_exp8);
	GetDlgItem(IDC_EXP9)->GetWindowTextW(m_exp9);
	GetDlgItem(IDC_EXP10)->GetWindowTextW(m_exp10);
	GetDlgItem(IDC_EXP11)->GetWindowTextW(m_exp11);
	GetDlgItem(IDC_EXP12)->GetWindowTextW(m_exp12);
	GetDlgItem(IDC_EXP13)->GetWindowTextW(m_exp13);
	GetDlgItem(IDC_EXP14)->GetWindowTextW(m_exp14);
	
	CString TempInfo = L"";
	std::string TempString;

	if (m_cardnum_info == L"")
	{
		GetDlgItem(IDC_STATIC_INFO)->SetWindowTextW(L"请输入你的卡号 !!");
	}
	else if(m_name_info == L"")
	{
		GetDlgItem(IDC_STATIC_INFO)->SetWindowTextW(L"请输入你的姓名  !!");
	}
	else
	{
			g_order = "INSERT INTO ";
			std::string tempTableString;
			tempTableString = CW2A(TableName.GetString());
			g_order += tempTableString;
			g_order += " VALUES('";
			TempString = CW2A(m_college_info.GetString());
			g_order += TempString;
			g_order +="','";

			TempString = CW2A(m_class_info.GetString());
			g_order += TempString;
			g_order +="','";

			TempString = CW2A(m_cardnum_info.GetString());
			g_order += TempString;
			g_order +="','";

			TempString = CW2A(m_name_info.GetString());
			g_order += TempString;
			g_order +="','";

			TempString = CW2A(m_exp1.GetString());
			g_order += TempString;
			g_order +="','";

			TempString = CW2A(m_exp2.GetString());
			g_order += TempString;
			g_order +="','";

			TempString = CW2A(m_exp3.GetString());
			g_order += TempString;
			g_order +="','";

			TempString = CW2A(m_exp4.GetString());
			g_order += TempString;
			g_order +="','";

			TempString = CW2A(m_exp5.GetString());
			g_order += TempString;
			g_order +="','";

			TempString = CW2A(m_exp6.GetString());
			g_order += TempString;
			g_order +="','";

			TempString = CW2A(m_exp7.GetString());
			g_order += TempString;
			g_order +="','";

			TempString = CW2A(m_exp8.GetString());
			g_order += TempString;
			g_order +="','";

			TempString = CW2A(m_exp9.GetString());
			g_order += TempString;
			g_order +="','";

			TempString = CW2A(m_exp10.GetString());
			g_order += TempString;
			g_order +="','";

			TempString = CW2A(m_exp11.GetString());
			g_order += TempString;
			g_order +="','";

			TempString = CW2A(m_exp12.GetString());
			g_order += TempString;
			g_order +="','";

			TempString = CW2A(m_exp13.GetString());
			g_order += TempString;
			g_order +="','";

			TempString = CW2A(m_exp14.GetString());
			g_order += TempString;
			g_order +="');";
	

			if (mysql_query(&MyData,g_order.c_str()) == 0)
			{
				GetDlgItem(IDC_STATIC_INFO)->SetWindowTextW(L"新增数据成功!");
			}
			else
			{
				GetDlgItem(IDC_STATIC_INFO)->SetWindowTextW(L"错误！ 新增数据失败！");
			}

			VarInit();
	}

	// TODO: 在此添加控件通知处理程序代码
}

//删除数据 按名字查找
void CMySqlTestDlg::OnBnClickedDeldat()
{

	mysql_ping(&MyData);

	if(MessageBox(L"确定要删除吗！？",0,MB_OKCANCEL)==IDOK)
	{
		GetDlgItem(IDC_EDITNAME)->GetWindowTextW(m_name_info);

		CString TempInfo = L"";
		std::string TempString;

//删除前先查找有无该人
	g_order = "SELECT * FROM ";
	std::string tempTableString;
	tempTableString = CW2A(TableName.GetString());
	g_order += tempTableString;
	g_order += " WHERE ";
	TempString = CW2A(m_name_info.GetString());
	std::string TempStringTitle;
	TempStringTitle = CW2A(tm_name_info.GetString());
	g_order += TempStringTitle;
	g_order += "='";
	g_order += TempString;
	g_order +="';";

	MYSQL_RES *result = NULL;
	//if查找语句执行错误就不继续执行
	if (mysql_query(&MyData,g_order.c_str()) == 0)
	{
		 //一次性取得数据集  
		result = mysql_store_result(&MyData);
		 //取得并打印行数  
        //int rowcount = mysql_num_rows(result); 
		 //取得并打印各字段的名称  
        unsigned int fieldcount = mysql_num_fields(result);  //列数
		// MYSQL_FIELD *field = NULL;  
        //for (unsigned int i = 0; i < fieldcount; i++) 
		//{  
       //     field = mysql_fetch_field_direct(result, i);  
       //     TempInfo = field->name;
		//	g_Info += TempInfo + L"\t\t";
      //  } 
	//	g_Info += L"\r\n";
		 //打印各行  
        MYSQL_ROW row = NULL;  
        row = mysql_fetch_row(result);  
        if(NULL != row) 
		{
				g_order = "DELETE FROM ";
				std::string tempTableString;
				tempTableString = CW2A(TableName.GetString());
				g_order += tempTableString;
				g_order += " WHERE ";
				std::string TempStringTitle;
				TempStringTitle = CW2A(tm_name_info.GetString());
				g_order += TempStringTitle;
				g_order += "='";
				TempString = CW2A(m_name_info.GetString());
				g_order += TempString;
				g_order +="';";

				if (mysql_query(&MyData,g_order.c_str()) == 0)
				{
					MessageBox(L"成功删除数据!");
				}
				else
				{
					MessageBox(L"错误！删除失败！");
				}
		}	
		else
		{
			MessageBox(L"错误！ 删除失败 未找到数据");
		}
		mysql_free_result(result);  
		VarInit();
	// TODO: 在此添加控件通知处理程序代码
	}
	}
	else
	{
		MessageBox(L"删除取消！");
	}
}

//得到表名字并显示
void CMySqlTestDlg::getTitle()
{

	mysql_ping(&MyData);

	tm_college_info = "";
	TmAlter_college_info = "";
	tm_class_info = "";
	tm_cardnum_info = "";
	tm_name_info = "";
	tm_exp1 = "";
	tm_exp2 = "";
	tm_exp3 = "";
	tm_exp4 = "";
	tm_exp5 = "";
	tm_exp6 = "";
	tm_exp7 = "";
	tm_exp8 = "";
	tm_exp9 = "";
	tm_exp10 = "";
	tm_exp11 = "";
	tm_exp12 = "";
	tm_exp13 = "";
	tm_exp14 = "";

	CString TempInfo = L"";
	std::string TempString;
	g_order = "SELECT * FROM ";
	TempString = CW2A(TableName.GetString());
	g_order += TempString;
	MYSQL_RES *result = NULL;
	if (mysql_query(&MyData,g_order.c_str()) == 0)
	{
		 //一次性取得数据集  
		result = mysql_store_result(&MyData);
		 //取得并打印各字段的名称  
        unsigned int fieldcount = mysql_num_fields(result);  //列数
		MYSQL_FIELD *field = NULL;  
		if(fieldcount>0)
		{
			field = mysql_fetch_field_direct(result, 0);  
			TempInfo = field->name;
			tm_college_info = TempInfo;
		}
		else
			tm_college_info="";

		if(fieldcount>1)
		{
			field = mysql_fetch_field_direct(result, 1);  
			TempInfo = field->name;
			tm_class_info = TempInfo;
		}
		else
			tm_class_info="";

		if(fieldcount>2)
		{
			field = mysql_fetch_field_direct(result, 2);  
			TempInfo = field->name;
			tm_cardnum_info = TempInfo;
		}
		else
			tm_cardnum_info="";

		if(fieldcount>3)
		{
			field = mysql_fetch_field_direct(result, 3);  
			TempInfo = field->name;
			tm_name_info = TempInfo;
		}
		else
			tm_name_info="";

		if(fieldcount>4)
		{
			field = mysql_fetch_field_direct(result, 4);  
			TempInfo = field->name;
			tm_exp1 = TempInfo;
		}
		else
			tm_exp1="";

		if(fieldcount>5)
		{
			field = mysql_fetch_field_direct(result, 5);  
			TempInfo = field->name;
			tm_exp2 = TempInfo;
		}
		else
			tm_exp2="";

		if(fieldcount>6)
		{
			field = mysql_fetch_field_direct(result, 6);  
			TempInfo = field->name;
			tm_exp3 = TempInfo;
		}
		else
			tm_exp3="";

		if(fieldcount>7)
		{
			field = mysql_fetch_field_direct(result, 7);  
			TempInfo = field->name;
			tm_exp4 = TempInfo;
		}
		else
			tm_exp4="";

		if(fieldcount>8)
		{
			field = mysql_fetch_field_direct(result, 8);  
			TempInfo = field->name;
			tm_exp5 = TempInfo;
		}
		else
			tm_exp5="";

		if(fieldcount>9)
		{
			field = mysql_fetch_field_direct(result, 9);  
			TempInfo = field->name;
			tm_exp6 = TempInfo;
		}
		else
			tm_exp6="";

		if(fieldcount>10)
		{
			field = mysql_fetch_field_direct(result, 10);  
			TempInfo = field->name;
			tm_exp7 = TempInfo;
		}
		else
			tm_exp7="";

		if(fieldcount>11)
		{
			field = mysql_fetch_field_direct(result, 11);  
			TempInfo = field->name;
			tm_exp8 = TempInfo;
		}
		else
			tm_exp8="";

		if(fieldcount>12)
		{
			field = mysql_fetch_field_direct(result, 12);  
			TempInfo = field->name;
			tm_exp9 = TempInfo;
		}
		else
			tm_exp9="";

		if(fieldcount>13)
		{
			field = mysql_fetch_field_direct(result, 13);  
			TempInfo = field->name;
			tm_exp10 = TempInfo;
		}
		else
			tm_exp10="";

		if(fieldcount>14)
		{
			field = mysql_fetch_field_direct(result, 14);  
			TempInfo = field->name;
			tm_exp11 = TempInfo;
		}
		else
			tm_exp11="";

		if(fieldcount>15)
		{
			field = mysql_fetch_field_direct(result, 15);  
			TempInfo = field->name;
			tm_exp12 = TempInfo;
		}
		else
			tm_exp12="";

		if(fieldcount>16)
		{
			field = mysql_fetch_field_direct(result, 16);  
			TempInfo = field->name;
			tm_exp13 = TempInfo;
		}
		else
			tm_exp13="";

		if(fieldcount>17)
		{
			field = mysql_fetch_field_direct(result, 17);  
			TempInfo = field->name;
			tm_exp14 = TempInfo;
		}
		else
			tm_exp14="";


		GetDlgItem(IDC_EDF1)->SetWindowTextW(tm_college_info);
		GetDlgItem(IDC_EDF2)->SetWindowTextW(tm_class_info);
		GetDlgItem(IDC_EDF3)->SetWindowTextW(tm_cardnum_info);
		GetDlgItem(IDC_EDF4)->SetWindowTextW(tm_name_info);
		GetDlgItem(IDC_EDF5)->SetWindowTextW(tm_exp1);
		GetDlgItem(IDC_EDF6)->SetWindowTextW(tm_exp2);
		GetDlgItem(IDC_EDF7)->SetWindowTextW(tm_exp3);
		GetDlgItem(IDC_EDF8)->SetWindowTextW(tm_exp4);
		GetDlgItem(IDC_EDF9)->SetWindowTextW(tm_exp5);
		GetDlgItem(IDC_EDF10)->SetWindowTextW(tm_exp6);
		GetDlgItem(IDC_EDF11)->SetWindowTextW(tm_exp7);
		GetDlgItem(IDC_EDF12)->SetWindowTextW(tm_exp8);
		GetDlgItem(IDC_EDF13)->SetWindowTextW(tm_exp9);
		GetDlgItem(IDC_EDF14)->SetWindowTextW(tm_exp10);
		GetDlgItem(IDC_EDF15)->SetWindowTextW(tm_exp11);
		GetDlgItem(IDC_EDF16)->SetWindowTextW(tm_exp12);
		GetDlgItem(IDC_EDF17)->SetWindowTextW(tm_exp13);
		GetDlgItem(IDC_EDF18)->SetWindowTextW(tm_exp14);

	}
	else
	{
		//GetDlgItem(IDC_STATIC_INFO)->SetWindowTextW(L"Err");
		//试图进入到不存在的表 提示并退出 
		MessageBox(L"This Table has not been created！！ Or Maybe Have Some illegal character！！");
		exit(0);
	}
	mysql_free_result(result);  
}

//修改颜色
HBRUSH CMySqlTestDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	// TODO:  在此更改 DC 的任何特性
		if (nCtlColor == CTLCOLOR_DLG)
	{
		//return Prue_brush;
		hbr = (HBRUSH)CreateSolidBrush(RGB(166,243,248));
	}
	//不指定特定控件 全部指定控件都一样
	if (nCtlColor == CTLCOLOR_STATIC)//nCtrColor 用于指定控件的类型
	{
			pDC->SetTextColor(RGB(0,0,0));
			//pDC->SetBkMode(TRANSPARENT);
			pDC->SetBkColor(RGB(166,243,248));
			hbr = (HBRUSH)CreateSolidBrush(RGB(166,243,248));
	}
	if (nCtlColor == CTLCOLOR_EDIT)//nCtrColor 用于指定控件的类型
	{
			pDC->SetTextColor(RGB(0,0,0));
			//pDC->SetBkMode(TRANSPARENT);
			pDC->SetBkColor(RGB(166,243,248));
			pDC->SelectObject(&mFont);
			hbr = (HBRUSH)CreateSolidBrush(RGB(166,243,248));
	}
	//如果要指定某个特定控件就这么写
	//if (pWnd-> GetDlgCtrlID()==IDC_INFO_TXT)
	//{
	//		pDC->SetTextColor(RGB(0,0,0));
	//		//pDC->SetBkMode(TRANSPARENT);
	//		pDC->SetBkColor(RGB(0,255,230));
	//		hbr = (HBRUSH)CreateSolidBrush(RGB(0,255,230));	
	//}
	//
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

//自绘制按钮
void CMySqlTestDlg::ReColorBtn()
{
	
	GetDlgItem(IDC_BTNNEW)->ModifyStyle(0,BS_OWNERDRAW,0);
	m_BtnNew.Attach(IDC_BTNNEW,this);
	m_BtnNew.SetDownColor(RGB(166,243,248));
	m_BtnNew.SetUpColor(RGB(255,243,0));

	GetDlgItem(IDC_DELDAT)->ModifyStyle(0,BS_OWNERDRAW,0);
	m_BtnDel.Attach(IDC_DELDAT,this);
	m_BtnDel.SetDownColor(RGB(166,243,248));
	m_BtnDel.SetUpColor(RGB(255,50,0));

	GetDlgItem(IDC_BTNSRH)->ModifyStyle(0,BS_OWNERDRAW,0);
	m_BtnSrh.Attach(IDC_BTNSRH,this);
	m_BtnSrh.SetDownColor(RGB(166,243,248));
	m_BtnSrh.SetUpColor(RGB(166,243,0));

	GetDlgItem(IDC_SAVADAT)->ModifyStyle(0,BS_OWNERDRAW,0);
	m_BtnSave.Attach(IDC_SAVADAT,this);
	m_BtnSave.SetDownColor(RGB(166,243,248));
	m_BtnSave.SetUpColor(RGB(255,150,0));

	GetDlgItem(IDC_ETiTle)->ModifyStyle(0,BS_OWNERDRAW,0);
	m_ETitle.Attach(IDC_ETiTle,this);
	m_ETitle.SetDownColor(RGB(166,243,248));
	m_ETitle.SetUpColor(RGB(255,150,0));

	GetDlgItem(IDC_BTNONE)->ModifyStyle(0,BS_OWNERDRAW,0);
	m_BtnOne.Attach(IDC_BTNONE,this);
	m_BtnOne.SetDownColor(RGB(166,243,248));
	m_BtnOne.SetUpColor(RGB(90,225,100));

	GetDlgItem(IDC_BTNALL)->ModifyStyle(0,BS_OWNERDRAW,0);
	m_BtnAll.Attach(IDC_BTNALL,this);
	m_BtnAll.SetDownColor(RGB(166,243,248));
	m_BtnAll.SetUpColor(RGB(90,225,100));
}

//导出到excel
void CMySqlTestDlg::OnBnClickedBtnall()
{
	ShowText();
	SYSTEMTIME st;
	wchar_t TempName[MAXBYTE];
	GetLocalTime(&st);
	swprintf_s(TempName,L"%4d%2d%2d%2d%2d%2d.xls",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	std::ofstream out_stream;
	out_stream.open(TempName);
	std::string TempString;
	TempString = CW2A(g_Info.GetString());
	out_stream.write(TempString.c_str(),TempString.length());
	out_stream.close();
	// TODO: 在此添加控件通知处理程序代码
}

//获取所有信息
void CMySqlTestDlg::ShowText()
{
	CString TempInfo = L"";
	g_order = "SELECT * FROM ";
	std::string tempTableString;
	tempTableString = CW2A(TableName.GetString());
	g_order += tempTableString;
	MYSQL_RES *result = NULL;
	if (mysql_query(&MyData,g_order.c_str()) == 0)
	{
		 //一次性取得数据集  
		result = mysql_store_result(&MyData);
		 //取得并打印行数  
        int rowcount = mysql_num_rows(result); 
		 //取得并打印各字段的名称  
        unsigned int fieldcount = mysql_num_fields(result);  //列数
		 MYSQL_FIELD *field = NULL;  
        for (unsigned int i = 0; i < fieldcount; i++) 
		{  
            field = mysql_fetch_field_direct(result, i);  
            TempInfo = field->name;
			g_Info += TempInfo + L"\t";
        } 
		g_Info += L"\r\n";
		 //打印各行  
        MYSQL_ROW row = NULL;  
        row = mysql_fetch_row(result);  
        while (NULL != row) {  
            for (int i = 0; i < fieldcount; i++) 
			{  
					TempInfo = row[i];
					g_Info += TempInfo + L"\t";
            }  
            g_Info += L"\n"; 
            row = mysql_fetch_row(result);  
        }  
	}
	mysql_free_result(result);  
}

//修改表头
void CMySqlTestDlg::OnBnClickedEtitle()
{
	// TODO: 在此添加控件通知处理程序代码
	mysql_ping(&MyData);

	//得到更改后的表头信息
	GetDlgItem(IDC_EDF1)->GetWindowTextW(TmAlter_college_info);
	GetDlgItem(IDC_EDF2)->GetWindowTextW(TmAlter_class_info);
	GetDlgItem(IDC_EDF3)->GetWindowTextW(TmAlter_cardnum_info);
	GetDlgItem(IDC_EDF4)->GetWindowTextW(TmAlter_name_info);
	GetDlgItem(IDC_EDF5)->GetWindowTextW(TmAlter_exp1);
	GetDlgItem(IDC_EDF6)->GetWindowTextW(TmALter_exp2);
	GetDlgItem(IDC_EDF7)->GetWindowTextW(TmAlter_exp3);
	GetDlgItem(IDC_EDF8)->GetWindowTextW(TmAlter_exp4);
	GetDlgItem(IDC_EDF9)->GetWindowTextW(TmAlter_exp5);
	GetDlgItem(IDC_EDF10)->GetWindowTextW(TmAlter_exp6);
	GetDlgItem(IDC_EDF11)->GetWindowTextW(TmAlter_exp7);
	GetDlgItem(IDC_EDF12)->GetWindowTextW(TmAlter_exp8);
	GetDlgItem(IDC_EDF13)->GetWindowTextW(TmAlter_exp9);
	GetDlgItem(IDC_EDF14)->GetWindowTextW(TmAlter_exp10);
	GetDlgItem(IDC_EDF15)->GetWindowTextW(TmAlter_exp11);
	GetDlgItem(IDC_EDF16)->GetWindowTextW(TmAlter_exp12);
	GetDlgItem(IDC_EDF17)->GetWindowTextW(TmAlter_exp13);
	GetDlgItem(IDC_EDF18)->GetWindowTextW(TmAlter_exp14);
	
		std::string TempString;
		g_order = "ALTER TABLE ";
		std::string tempTableString;
		tempTableString = CW2A(TableName.GetString());
		g_order += tempTableString;
		g_order += " CHANGE COLUMN ";
		TempString = CW2A(tm_college_info.GetString());
		g_order += TempString;	
		g_order +=" ";
		TempString = CW2A(TmAlter_college_info.GetString());
		g_order += TempString;
		g_order +=" TEXT  NOT NULL,CHANGE COLUMN ";
		TempString = CW2A(tm_class_info.GetString());
		g_order += TempString;	
		g_order +=" ";
		TempString = CW2A(TmAlter_class_info.GetString());
		g_order += TempString;
		g_order +=" TEXT  NOT NULL,CHANGE COLUMN ";
		TempString = CW2A(tm_cardnum_info.GetString());
		g_order += TempString;	
		g_order +=" ";
		TempString = CW2A(TmAlter_cardnum_info.GetString());
		g_order += TempString;
		g_order +=" TEXT  NOT NULL,CHANGE COLUMN ";
		TempString = CW2A(tm_name_info.GetString());
		g_order += TempString;	
		g_order +=" ";
		TempString = CW2A(TmAlter_name_info.GetString());
		g_order += TempString;
		g_order +=" TEXT  NOT NULL,CHANGE COLUMN ";
		TempString = CW2A(tm_exp1.GetString());
		g_order += TempString;	
		g_order +=" ";
		TempString = CW2A(TmAlter_exp1.GetString());
		g_order += TempString;
		g_order +=" TEXT,CHANGE COLUMN ";
		TempString = CW2A(tm_exp2.GetString());
		g_order += TempString;	
		g_order +=" ";
		TempString = CW2A(TmALter_exp2.GetString());
		g_order += TempString;
		g_order +=" TEXT,CHANGE COLUMN ";
		TempString = CW2A(tm_exp3.GetString());
		g_order += TempString;	
		g_order +=" ";
		TempString = CW2A(TmAlter_exp3.GetString());
		g_order += TempString;
		g_order +=" TEXT,CHANGE COLUMN ";
		TempString = CW2A(tm_exp4.GetString());
		g_order += TempString;	
		g_order +=" ";
		TempString = CW2A(TmAlter_exp4.GetString());
		g_order += TempString;
		g_order +=" TEXT,CHANGE COLUMN ";
		TempString = CW2A(tm_exp5.GetString());
		g_order += TempString;	
		g_order +=" ";
		TempString = CW2A(TmAlter_exp5.GetString());
		g_order += TempString;
		g_order +=" TEXT,CHANGE COLUMN ";
		TempString = CW2A(tm_exp6.GetString());
		g_order += TempString;	
		g_order +=" ";
		TempString = CW2A(TmAlter_exp6.GetString());
		g_order += TempString;
		g_order +=" TEXT,CHANGE COLUMN ";
		TempString = CW2A(tm_exp7.GetString());
		g_order += TempString;	
		g_order +=" ";
		TempString = CW2A(TmAlter_exp7.GetString());
		g_order += TempString;
		g_order +=" TEXT,CHANGE COLUMN ";
		TempString = CW2A(tm_exp8.GetString());
		g_order += TempString;	
		g_order +=" ";
		TempString = CW2A(TmAlter_exp8.GetString());
		g_order += TempString;
		g_order +=" TEXT,CHANGE COLUMN ";
		TempString = CW2A(tm_exp9.GetString());
		g_order += TempString;	
		g_order +=" ";
		TempString = CW2A(TmAlter_exp9.GetString());
		g_order += TempString;
		g_order +=" TEXT,CHANGE COLUMN ";
		TempString = CW2A(tm_exp10.GetString());
		g_order += TempString;	
		g_order +=" ";
		TempString = CW2A(TmAlter_exp10.GetString());
		g_order += TempString;
		g_order +=" TEXT,CHANGE COLUMN ";
		TempString = CW2A(tm_exp11.GetString());
		g_order += TempString;	
		g_order +=" ";
		TempString = CW2A(TmAlter_exp11.GetString());
		g_order += TempString;
		g_order +=" TEXT,CHANGE COLUMN ";
		TempString = CW2A(tm_exp12.GetString());
		g_order += TempString;	
		g_order +=" ";
		TempString = CW2A(TmAlter_exp12.GetString());
		g_order += TempString;
		g_order +=" TEXT,CHANGE COLUMN ";
		TempString = CW2A(tm_exp13.GetString());
		g_order += TempString;	
		g_order +=" ";
		TempString = CW2A(TmAlter_exp13.GetString());
		g_order += TempString;
		g_order +=" TEXT,CHANGE COLUMN ";
		TempString = CW2A(tm_exp14.GetString());
		g_order += TempString;	
		g_order +=" ";
		TempString = CW2A(TmAlter_exp14.GetString());
		g_order += TempString;
		g_order +=" TEXT;";
		mysql_query(&MyData,g_order.c_str());
	

		getTitle();

}

//新建线程 每隔一段时间ping一次连接数据库
DWORD WINAPI PINGPROC(PVOID pParam)
{
	while (true)
	{
		mysql_ping(&MyData);
		Sleep(20000);
	}
	
	return 1;
}