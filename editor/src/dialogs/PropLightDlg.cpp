#include "..\sys\stdafx.h"
#include "..\sys\CookieMachine.h"
#include "..\SceneFile.h"
#include "PropLightDlg.h"
#include "CookieMachineDlg.h"
#include ".\proplightdlg.h"



IMPLEMENT_DYNAMIC(CPropLightDlg, CDialog)
CPropLightDlg::CPropLightDlg(CWnd* pParent /*=NULL*/)
	: CBaseDialog(CPropLightDlg::IDD, pParent)
{
	m_pLight	= NULL;
	m_pPosX		= NULL;
	m_pPosY		= NULL;
	m_pPosZ		= NULL;
	m_pTargX	= NULL;
	m_pTargY	= NULL;
	m_pTargZ	= NULL;
	m_pAttenuation	= NULL;
	m_pMultiplier	= NULL;
	m_pRange	= NULL;
	m_pTheta	= NULL;
	m_pPhi		= NULL;
	m_pType		= NULL;

	m_btnRange	= NULL;

	m_pParent	= pParent;
}

CPropLightDlg::~CPropLightDlg()
{
}

void CPropLightDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_SPIN_ATTEN, m_spinAtten);
	DDX_Control(pDX, IDC_SPIN_MULT, m_spinMult);
}


BEGIN_MESSAGE_MAP(CPropLightDlg, CDialog)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_CHK_RANGE, OnBnClickedChkRange)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BTN_AMB, OnBnClickedBtnAmb)
	ON_BN_CLICKED(IDC_BTN_DIFF, OnBnClickedBtnDiff)
	ON_BN_CLICKED(IDC_BTN_SPEC, OnBnClickedBtnSpec)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ATTEN, OnDeltaposSpinAtten)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MULT, OnDeltaposSpinMult)
END_MESSAGE_MAP()


HBRUSH CPropLightDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CBaseDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	pDC->SetTextColor( RGB(0, 0, 0) );
	pDC->SetBkMode( TRANSPARENT );


	if( pWnd->GetDlgCtrlID() == IDC_RANGE )
	{
		if( m_btnRange->GetCheck() )
			hbr = m_brWhite;
	}

	return hbr;
}

BOOL CPropLightDlg::OnInitDialog()
{
	CBaseDialog::OnInitDialog();

	m_pID		= GetDlgItem( IDC_EDIT_ID );
	m_pType		= GetDlgItem( IDC_EDIT_TYPE );
	m_pPosX		= GetDlgItem( IDC_POS_X );
	m_pPosY		= GetDlgItem( IDC_POS_Y );
	m_pPosZ		= GetDlgItem( IDC_POS_Z );
	m_pTargX	= GetDlgItem( IDC_DIR_X );
	m_pTargY	= GetDlgItem( IDC_DIR_Y );
	m_pTargZ	= GetDlgItem( IDC_DIR_Z );
	m_pAttenuation	= GetDlgItem( IDC_ATTENUATION );
	m_pMultiplier	= GetDlgItem( IDC_MULTIPLIER );
	m_pRange	= GetDlgItem( IDC_RANGE );
	m_pTheta	= GetDlgItem( IDC_THETA );
	m_pPhi		= GetDlgItem( IDC_PHI );
	m_btnRange	= (CButton*)GetDlgItem( IDC_CHK_RANGE );

	VERIFY( m_cbAmb.Attach(IDC_BTN_AMB, this, RGB(0,0,0), RGB(0,0,0), RGB(0,0,0)) );
	VERIFY( m_cbDiff.Attach(IDC_BTN_DIFF, this, RGB(0,0,0), RGB(0,0,0), RGB(0,0,0)) );
	VERIFY( m_cbSpec.Attach(IDC_BTN_SPEC, this, RGB(0,0,0), RGB(0,0,0), RGB(0,0,0)) );

	return TRUE;
}

void CPropLightDlg::Populate( int iFrameIndex ) 
{
	if( !m_pLight )
		return;

	CString strTemp;

	m_pID->SetWindowText( m_pLight->name.data() );

	strTemp.Format( "%0.4f", m_pLight->targ.x );
	m_pTargX->SetWindowText( strTemp );

	strTemp.Format( "%0.4f", m_pLight->targ.y );
	m_pTargY->SetWindowText( strTemp );

	strTemp.Format( "%0.4f", m_pLight->targ.z );
	m_pTargZ->SetWindowText( strTemp );

	switch( m_pLight->type )
	{
		case LT_SPOT:
			m_pType->SetWindowText( "SPOT" );
			break;

		case LT_POINT:
			m_pType->SetWindowText( "OMNI" );

			m_pTargX->SetWindowText( "0" );
			m_pTargY->SetWindowText( "0" );
			m_pTargZ->SetWindowText( "0" );
			break;

		case LT_DIRECTED:
			m_pType->SetWindowText( "DIRECTED" );
			break;
	}

	strTemp.Format( "%0.4f", m_pLight->pos.x );
	m_pPosX->SetWindowText( strTemp );

	strTemp.Format( "%0.4f", m_pLight->pos.y );
	m_pPosY->SetWindowText( strTemp );

	strTemp.Format( "%0.4f", m_pLight->pos.z );
	m_pPosZ->SetWindowText( strTemp );


	strTemp.Format( "%0.4f", m_pLight->attenuation );
	m_pAttenuation->SetWindowText( strTemp );

	strTemp.Format( "%0.4f", m_pLight->multiplier );
	m_pMultiplier->SetWindowText( strTemp );

	strTemp.Format( "%0.4f", m_pLight->range );
	m_pRange->SetWindowText( strTemp );


	strTemp.Format( "%0.4f", 0.0f );
	m_pTheta->SetWindowText( strTemp );

	strTemp.Format( "%0.4f", 0.0f );
	m_pPhi->SetWindowText( strTemp );

	m_cbAmb.SetColor( RGB( m_pLight->ambient.r, m_pLight->ambient.g, m_pLight->ambient.b ) );
	m_cbDiff.SetColor( RGB( m_pLight->diffuse.r, m_pLight->diffuse.g, m_pLight->diffuse.b ) );
	m_cbSpec.SetColor( RGB( m_pLight->specular.r, m_pLight->specular.g, m_pLight->specular.b ) );

	m_cbAmb.Invalidate();
	m_cbDiff.Invalidate();
	m_cbSpec.Invalidate();

	UpdateWindow();
}

void CPropLightDlg::Reset()
{
	CSceneFile*	pScene	= (CSceneFile*)((CCookieMachineDlg*)m_pParent)->GetSceneGlob();

	if( !pScene )
		return;

	pScene->ShowIsLightRangeVisible( false );
	m_btnRange->SetCheck( false );
}

void CPropLightDlg::OnBnClickedChkRange()
{
	CSceneFile*	pScene	= (CSceneFile*)((CCookieMachineDlg*)m_pParent)->GetSceneGlob();

	if( !pScene )
		return;

	m_btnRange->Invalidate( true );

	if( m_btnRange->GetCheck() )
		pScene->ShowIsLightRangeVisible( true );
	else
		pScene->ShowIsLightRangeVisible( false );

	m_pRange->Invalidate( true );
}

void CPropLightDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CBaseDialog::OnShowWindow(bShow, nStatus);

	Populate();
	Reset();
}

void CPropLightDlg::OnBnClickedBtnAmb()
{
	CColorDialog dlg( RGB( m_pLight->ambient.r, m_pLight->ambient.g, m_pLight->ambient.b ), CC_FULLOPEN );
	
	if( dlg.DoModal() != IDOK )
		return;

	m_cbAmb.SetColor( dlg.GetColor() );
	m_cbAmb.Invalidate( true );

	m_pLight->ambient.r = GetRValue( dlg.GetColor() );
	m_pLight->ambient.g = GetGValue( dlg.GetColor() );
	m_pLight->ambient.b = GetBValue( dlg.GetColor() );


	CString script;
	script.Format(	"QuestOMgr.Light_%s:SetAmbient( D3DXCOLOR( %f, %f, %f, 1 ) );",
					m_pLight->name.data(), 
					m_pLight->ambient.r / 255.0f,
					m_pLight->ambient.g / 255.0f,
					m_pLight->ambient.b / 255.0f );

	g_MemMappingsMgr->PostScriptToEngine( script, true );
}

void CPropLightDlg::OnBnClickedBtnDiff()
{
	CColorDialog dlg( RGB( m_pLight->diffuse.r, m_pLight->diffuse.g, m_pLight->diffuse.b ), CC_FULLOPEN );

	if( dlg.DoModal() != IDOK )
		return;

	m_cbDiff.SetColor( dlg.GetColor() );
	m_cbDiff.Invalidate( true );

	m_cbSpec.SetColor( dlg.GetColor() );
	m_cbSpec.Invalidate( true );

	m_pLight->diffuse.r = GetRValue( dlg.GetColor() );
	m_pLight->diffuse.g = GetGValue( dlg.GetColor() );
	m_pLight->diffuse.b = GetBValue( dlg.GetColor() );

	m_pLight->specular = m_pLight->diffuse;

	CString script;
	script.Format(	"QuestOMgr.Light_%s:SetDiffuse( D3DXCOLOR( %f, %f, %f, 1 ) );",
					m_pLight->name.data(), 
					m_pLight->diffuse.r / 255.0f,
					m_pLight->diffuse.g / 255.0f,
					m_pLight->diffuse.b / 255.0f );

	g_MemMappingsMgr->PostScriptToEngine( script, true );

	Sleep( 10 );

	script.Format(	"QuestOMgr.Light_%s:SetSpecular( D3DXCOLOR( %f, %f, %f, 1 ) );",
					m_pLight->name.data(), 
					m_pLight->diffuse.r / 255.0f,
					m_pLight->diffuse.g / 255.0f,
					m_pLight->diffuse.b / 255.0f );

	g_MemMappingsMgr->PostScriptToEngine( script, true );
}

void CPropLightDlg::OnBnClickedBtnSpec()
{
	CColorDialog dlg( RGB( m_pLight->specular.r, m_pLight->specular.g, m_pLight->specular.b ), CC_FULLOPEN );

	if( dlg.DoModal() != IDOK )
		return;
	
	m_cbSpec.SetColor( dlg.GetColor() );
	m_cbSpec.Invalidate( true );

	m_pLight->specular.r = GetRValue( dlg.GetColor() );
	m_pLight->specular.g = GetGValue( dlg.GetColor() );
	m_pLight->specular.b = GetBValue( dlg.GetColor() );

	CString script;
	script.Format(	"QuestOMgr.Light_%s:SetSpecular( D3DXCOLOR( %f, %f, %f, 1 ) );",
					m_pLight->name.data(), 
					m_pLight->specular.r / 255.0f,
					m_pLight->specular.g / 255.0f,
					m_pLight->specular.b / 255.0f );

	g_MemMappingsMgr->PostScriptToEngine( script, true );
}

void CPropLightDlg::OnDeltaposSpinAtten(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	if( pNMUpDown->iDelta == -1 )
	{
		m_pLight->attenuation += 0.1f;
	}
	else
	{
		m_pLight->attenuation -= 0.1f;
	}

	if( m_pLight->attenuation < 0.0f )
		m_pLight->attenuation = 0.0f;

	
	CString script;
	script.Format(  "QuestOMgr.Light_%s:SetAttenuation1( %f );",
					m_pLight->name.data(), m_pLight->attenuation );

	g_MemMappingsMgr->PostScriptToEngine( script, true );

	Populate();

	*pResult = 0;
}

void CPropLightDlg::OnDeltaposSpinMult(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	if( pNMUpDown->iDelta == -1 )
	{
		m_pLight->multiplier += 0.1f;
	}
	else
	{
		m_pLight->multiplier -= 0.1f;
	}

	if( m_pLight->multiplier < 0.0f )
		m_pLight->multiplier = 0.0f;

	CString script;
	script.Format(  "QuestOMgr.Light_%s:SetIntensity( %f );",
					m_pLight->name.data(), m_pLight->multiplier );

	g_MemMappingsMgr->PostScriptToEngine( script, true );

	Populate();

	*pResult = 0;
}
