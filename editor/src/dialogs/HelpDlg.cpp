#include "../sys/stdafx.h"
#include "../sys/CookieMachine.h"
#include "HelpDlg.h"



IMPLEMENT_DYNAMIC(CHelpDlg, CDialog)
CHelpDlg::CHelpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHelpDlg::IDD, pParent)
{
}

CHelpDlg::~CHelpDlg()
{
}

void CHelpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CHelpDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_CLOSE, OnBnClickedBtnClose)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CHelpDlg message handlers

void CHelpDlg::OnBnClickedBtnClose()
{
	OnOK();
}

BOOL CHelpDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CHelpDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	CEdit* help_text = (CEdit*)GetDlgItem( IDC_EDIT_HELP );
	str_t strHelpText;

	strHelpText.append( "F1\t\t - �������� ���� ������\r\n" );
	strHelpText.append( "Ctrl - Enter\t - �������� ���� ��������\r\n" );
	strHelpText.append( "Alt - F4\t\t - ����� �� ���������\r\n" );

	strHelpText.append( "\r\n\r\n*************** ���������� ������� *************************\r\n" );
	strHelpText.append( "������ ������ ����\t - ����������� �����/����/������/����� \r\n" );
	strHelpText.append( "Ctrl + ������ ������ ����\t - �������� ������������ ��� Y \r\n" );
	strHelpText.append( "Shift + ������ ������ ���� - zooming +/- \r\n" );

	strHelpText.append( "\r\n\r\n*************** �������� � ������� **************************\r\n" );
	strHelpText.append( "Ctrl - O\t - ������� ����\r\n" );
	strHelpText.append( "Ctrl - S\t - ��������� ����\r\n" );

	strHelpText.append( "\r\n\r\n*************** ������ ����������� ��������� *****************\r\n" );
	strHelpText.append( "F2\t - Wireframe\r\n" );
	strHelpText.append( "F3\t - Solid\r\n" );
	strHelpText.append( "F4\t - Solid & wireframe\r\n" );
	strHelpText.append( "F5\t - ��������� ��������� ������������\r\n" );

	strHelpText.append( "\r\n\r\n*************** ������ � EFN Animation ������� *****************\r\n" );
	strHelpText.append( "Alt - A\t - �������� ��� �����\r\n" );
	strHelpText.append( "Alt - Z\t - �������� ������ ������� ����\r\n" );
	strHelpText.append( "\r\n*����� �������������� ����������� �������: \r\n1) ������� � ���� - ������->Edit global raduis\r\n" );
	strHelpText.append( "2) ��������� ������ \"C\" � ������ ������ ����, ������ ����� ������\r\n" );
	strHelpText.append( "3) ����� ������� � ���� - ������->Edit global raduis, ��������� ����\r\n    ������ ����������.\r\n" );

	strHelpText.append( "\r\n\r\n*************** ������ � XSI ������� ************************\r\n" );
	strHelpText.append( "Ctrl - P\t - ��������� � ������������� ������\r\n" );

	strHelpText.append( "\r\n\r\n*************** ������ � SCN ������� ************************\r\n" );
	strHelpText.append( "Ctrl - Q\t - �������� ���� ��������\r\n" );
	strHelpText.append( "Ctrl - W\t - �������� ���� ������\r\n" );
	strHelpText.append( "Alt - X\t - ������� ����� �� ��������� �������� ( ������ ������ )\r\n" );

	strHelpText.append( "\r\n\r\n*************** �������� �������� ************************\r\n" );
	strHelpText.append( "Man\t\t - mn\r\n" );
	strHelpText.append( "Camera\t\t - cm\r\n" );
	strHelpText.append( "Light\t\t - lt\r\n" );
	strHelpText.append( "ShadowVolume\t - sv\r\n" );
	strHelpText.append( "LightVolume\t - lv\r\n" );
	strHelpText.append( "LayerGeom\t - lg\r\n" );
	strHelpText.append( "Action\t - oa\r\n" );
	strHelpText.append( "Background\t - bg\r\n" );
	strHelpText.append( "Scene\t\t - sc\r\n" );
	strHelpText.append( "Floor\t\t - fl\r\n" );
	strHelpText.append( "Glass\t\t - gl\r\n" );
	strHelpText.append( "Reflection\t - rf\r\n" );
	strHelpText.append( "CameraSelect\t - cs\r\n" );

	help_text->SetSel( -1, 0, false );
	help_text->ReplaceSel( (char*)strHelpText.data(), false );
}
