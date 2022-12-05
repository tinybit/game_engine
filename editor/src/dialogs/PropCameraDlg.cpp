#include "..\sys\stdafx.h"
#include "..\sys\CookieMachine.h"
#include "..\SceneFile.h"
#include "PropCameraDlg.h"
#include "CookieMachineDlg.h"


IMPLEMENT_DYNAMIC(CPropCameraDlg, CDialog)
CPropCameraDlg::CPropCameraDlg(CWnd* pParent /*=NULL*/)
	: CBaseDialog(CPropCameraDlg::IDD, pParent)
{
	m_pCamera	= NULL;
	m_pID		= NULL;
	m_pPosX		= NULL;
	m_pPosY		= NULL;
	m_pPosZ		= NULL;
	m_pDirX		= NULL;	
	m_pDirY		= NULL;
	m_pDirZ		= NULL;
	m_pUpvecX	= NULL;
	m_pUpvecY	= NULL;
	m_pUpvecZ	= NULL;
	m_pFovV		= NULL;
	m_pFovH		= NULL;
	m_pZoomNear	= NULL;
	m_pZoomFar	= NULL;
	m_pClipNear	= NULL;
	m_pClipFar	= NULL;

	m_pParent	= pParent;
}

CPropCameraDlg::~CPropCameraDlg()
{
}

void CPropCameraDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPropCameraDlg, CDialog)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_CHK_ZOOM_NEAR, OnBnClickedChkZoomNear)
	ON_BN_CLICKED(IDC_CHK_ZOOM_FAR, OnBnClickedChkZoomFar)
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


void CPropCameraDlg::Populate( int iFrameIndex )
{
	if( !m_pCamera )
		return;

	CString strTemp;

	m_pID->SetWindowText( m_pCamera->name.data() );

	strTemp.Format( "%0.4f", m_pCamera->pos.x );
	m_pPosX->SetWindowText( strTemp );

	strTemp.Format( "%0.4f", m_pCamera->pos.y );
	m_pPosY->SetWindowText( strTemp );

	strTemp.Format( "%0.4f", m_pCamera->pos.z );
	m_pPosZ->SetWindowText( strTemp );



	strTemp.Format( "%0.4f", m_pCamera->targ.x );
	m_pDirX->SetWindowText( strTemp );

	strTemp.Format( "%0.4f", m_pCamera->targ.y );
	m_pDirY->SetWindowText( strTemp );

	strTemp.Format( "%0.4f", m_pCamera->targ.z );
	m_pDirZ->SetWindowText( strTemp );


	strTemp.Format( "%0.4f", m_pCamera->upvec.x );
	m_pUpvecX->SetWindowText( strTemp );

	strTemp.Format( "%0.4f", m_pCamera->upvec.y );
	m_pUpvecY->SetWindowText( strTemp );

	strTemp.Format( "%0.4f", m_pCamera->upvec.z );
	m_pUpvecZ->SetWindowText( strTemp );


	strTemp.Format( "%0.4f", m_pCamera->fov.y );
	m_pFovV->SetWindowText( strTemp );

	strTemp.Format( "%0.4f", m_pCamera->fov.x );
	m_pFovH->SetWindowText( strTemp );


	strTemp.Format( "%0.4f", m_pCamera->zoom_near );
	m_pZoomNear->SetWindowText( strTemp );

	strTemp.Format( "%0.4f", m_pCamera->zoom_far );
	m_pZoomFar->SetWindowText( strTemp );


	strTemp.Format( "%0.4f", m_pCamera->z_near );
	m_pClipNear->SetWindowText( strTemp );

	strTemp.Format( "%0.4f", m_pCamera->z_far );
	m_pClipFar->SetWindowText( strTemp );

	UpdateWindow();
}

HBRUSH CPropCameraDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CBaseDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	CButton* btnFar = (CButton*)GetDlgItem( IDC_CHK_ZOOM_FAR );
	CButton* btnNear = (CButton*)GetDlgItem( IDC_CHK_ZOOM_NEAR );

	pDC->SetTextColor( RGB(0, 0, 0) );
	pDC->SetBkMode( TRANSPARENT );

	if( pWnd->GetDlgCtrlID() == IDC_ZOOM_NEAR )
	{
		if( btnNear->GetCheck() )
			hbr = m_brWhite;
	}

	if( pWnd->GetDlgCtrlID() == IDC_ZOOM_FAR )
	{
		if( btnFar->GetCheck() )
			hbr = m_brWhite;
	}

	return hbr;
}

BOOL CPropCameraDlg::OnInitDialog()
{
	CBaseDialog::OnInitDialog();

	m_pID		= GetDlgItem( IDC_EDIT_ID );
	m_pPosX		= GetDlgItem( IDC_POS_X );
	m_pPosY		= GetDlgItem( IDC_POS_Y );
	m_pPosZ		= GetDlgItem( IDC_POS_Z );
	m_pDirX		= GetDlgItem( IDC_DIR_X );
	m_pDirY		= GetDlgItem( IDC_DIR_Y );
	m_pDirZ		= GetDlgItem( IDC_DIR_Z );
	m_pUpvecX	= GetDlgItem( IDC_UPVEC_X );
	m_pUpvecY	= GetDlgItem( IDC_UPVEC_Y );
	m_pUpvecZ	= GetDlgItem( IDC_UPVEC_Z );
	m_pFovV		= GetDlgItem( IDC_FOV_V );
	m_pFovH		= GetDlgItem( IDC_FOV_H );
	m_pZoomNear = GetDlgItem( IDC_ZOOM_NEAR );
	m_pZoomFar	= GetDlgItem( IDC_ZOOM_FAR );
	m_pClipNear = GetDlgItem( IDC_CLIP_NEAR );
	m_pClipFar	= GetDlgItem( IDC_CLIP_FAR );

	

	return TRUE;
}

void CPropCameraDlg::OnBnClickedChkZoomNear()
{
	CButton* btnFar = (CButton*)GetDlgItem( IDC_CHK_ZOOM_FAR );
	CButton* btnNear = (CButton*)GetDlgItem( IDC_CHK_ZOOM_NEAR );
	btnFar->SetCheck( false );

	CSceneFile* pScene = (CSceneFile*)((CCookieMachineDlg*)m_pParent)->GetSceneGlob();

	if( !pScene )
		return;

	if( btnNear->GetCheck() )
		pScene->SetCamZoomNear( true );
	else
		pScene->SetCamZoomNear( false );

	m_pZoomNear->Invalidate( true );
	m_pZoomFar->Invalidate( true );
}

void CPropCameraDlg::OnBnClickedChkZoomFar()
{
	CButton* btnFar = (CButton*)GetDlgItem( IDC_CHK_ZOOM_FAR );
	CButton* btnNear = (CButton*)GetDlgItem( IDC_CHK_ZOOM_NEAR );
	btnNear->SetCheck( false );

	CSceneFile* pScene = (CSceneFile*)((CCookieMachineDlg*)m_pParent)->GetSceneGlob();

	if( !pScene )
		return;
	
	if( btnFar->GetCheck() )
		pScene->SetCamZoomFar( true );
	else
		pScene->SetCamZoomFar( false );

	m_pZoomNear->Invalidate( true );
	m_pZoomFar->Invalidate( true );
}

void CPropCameraDlg::Reset()
{
	CSceneFile* pScene = (CSceneFile*)((CCookieMachineDlg*)m_pParent)->GetSceneGlob();

	if( !pScene )
		return;

	CButton* btnFar = (CButton*)GetDlgItem( IDC_CHK_ZOOM_FAR );
	CButton* btnNear = (CButton*)GetDlgItem( IDC_CHK_ZOOM_NEAR );

	btnFar->SetCheck( false );
	btnNear->SetCheck( false );

	pScene->SetCamZoomNear( false );
	pScene->SetCamZoomFar( false );
}

void CPropCameraDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CBaseDialog::OnShowWindow(bShow, nStatus);

	Reset();
}

void CPropCameraDlg::OnDestroy()
{
	CBaseDialog::OnDestroy();

	m_brush.DeleteObject();
}
