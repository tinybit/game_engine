#include "../sys/stdafx.h"
#include "../sys/cookiemachine.h"
#include "TesselatedMeshDlg.h"
#include "CookieMachineDlg.h"

static int prevSizeX = 20;
static int prevSizeY = 20;
static int prevSizeZ = 20;

static int prevTessU = 1;
static int prevTessV = 1;
static int prevTessBase = 1;

IMPLEMENT_DYNAMIC(CTesselatedMeshDlg, CDialog)
CTesselatedMeshDlg::CTesselatedMeshDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTesselatedMeshDlg::IDD, pParent)
{
	m_mode	 = TMDM_UNDEFINED;
	m_pScene = NULL;
	m_objPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
}

CTesselatedMeshDlg::~CTesselatedMeshDlg()
{
}

bool CTesselatedMeshDlg::ValidateData()
{
	if( m_mode == TMDM_UNDEFINED || !m_pScene )
		return false;

	return true;
}

void CTesselatedMeshDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTesselatedMeshDlg, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_HSCROLL()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()

void CTesselatedMeshDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	//TMDM_UNDEFINED= 0,
	//TMDM_BOX		= 1,
	//TMDM_CONE		= 2,
	//TMDM_CYLINDER	= 3,
	//TMDM_SPHERE	= 4,
	//TMDM_QUAD		= 5

	CEdit* pEditID = (CEdit*)GetDlgItem( IDC_EDIT_ID );
	pEditID->SetWindowText( "" );
	pEditID->SetFocus();

	CSliderCtrl* pSliderSizeX = (CSliderCtrl*)GetDlgItem( IDC_SLIDER_SIZE_X );
	CSliderCtrl* pSliderSizeY = (CSliderCtrl*)GetDlgItem( IDC_SLIDER_SIZE_Y );
	CSliderCtrl* pSliderSizeZ = (CSliderCtrl*)GetDlgItem( IDC_SLIDER_SIZE_Z );
	CSliderCtrl* pSliderTessU = (CSliderCtrl*)GetDlgItem( IDC_SLIDER_TESSELATION_U );
	CSliderCtrl* pSliderTessV = (CSliderCtrl*)GetDlgItem( IDC_SLIDER_TESSELATION_V );
	CSliderCtrl* pSliderTessBase = (CSliderCtrl*)GetDlgItem( IDC_SLIDER_TESSELATION_BASE );

	CEdit* pEditSizeX = (CEdit*)GetDlgItem( IDC_EDIT_SIZE_X );
	CEdit* pEditSizeY = (CEdit*)GetDlgItem( IDC_EDIT_SIZE_Y );
	CEdit* pEditSizeZ = (CEdit*)GetDlgItem( IDC_EDIT_SIZE_Z );
	CEdit* pEditTessU = (CEdit*)GetDlgItem( IDC_EDIT_TESSELATION_U );
	CEdit* pEditTessV = (CEdit*)GetDlgItem( IDC_EDIT_TESSELATION_V );
	CEdit* pEditTessBase = (CEdit*)GetDlgItem( IDC_EDIT_TESSELATION_BASE );

	CStatic* pLblSizeX = (CStatic*)GetDlgItem( IDC_STATIC_SIZE_X );
	CStatic* pLblSizeY = (CStatic*)GetDlgItem( IDC_STATIC_SIZE_Y );
	CStatic* pLblSizeZ = (CStatic*)GetDlgItem( IDC_STATIC_SIZE_Z );
	CStatic* pLblTessU = (CStatic*)GetDlgItem( IDC_STATIC_TESS_U );
	CStatic* pLblTessV = (CStatic*)GetDlgItem( IDC_STATIC_TESS_V );
	CStatic* pLblTessBase = (CStatic*)GetDlgItem( IDC_STATIC_TESS_BASE );

	CButton* pLockSize = (CButton*)GetDlgItem( IDC_CHECK_LOCK_SIZE );
	CButton* pLockTess = (CButton*)GetDlgItem( IDC_CHECK_LOCK_TESSELATION );

	if( m_mode == TMDM_BOX )
	{
		pSliderSizeX->SetRange( 1, 300, true );
		pSliderSizeY->SetRange( 1, 300, true );
		pSliderSizeZ->SetRange( 1, 300, true );
		pSliderTessU->SetRange( 1, 24, true );
		pSliderTessV->SetRange( 1, 24, true );
		pSliderTessBase->SetRange( 1, 24, true );

		pSliderSizeX->SetPos( 20 );
		pSliderSizeY->SetPos( 20 );
		pSliderSizeZ->SetPos( 20 );
		pSliderTessU->SetPos( 1 );
		pSliderTessV->SetPos( 1 );
		pSliderTessBase->SetPos( 1 );

		pEditSizeX->SetWindowText( "1.00" );
		pEditSizeY->SetWindowText( "1.00" );
		pEditSizeZ->SetWindowText( "1.00" );
		pEditTessU->SetWindowText( "1" );
		pEditTessV->SetWindowText( "1" );
		pEditTessBase->SetWindowText( "1" );

		pLblSizeX->SetWindowText( "X" );
		pLblSizeY->SetWindowText( "Y" );
		pLblSizeZ->SetWindowText( "Z" );
		pLblTessU->SetWindowText( "U" );
		pLblTessV->SetWindowText( "V" );
		pLblTessBase->SetWindowText( "Base" );

		pSliderSizeX->EnableWindow( true );
		pEditSizeX->EnableWindow( true );
		pSliderSizeY->EnableWindow( true );
		pEditSizeY->EnableWindow( true );
		pSliderSizeZ->EnableWindow( true );
		pEditSizeZ->EnableWindow( true );

		pLockSize->SetCheck( true );
		pLockTess->SetCheck( false );


		prevSizeX = 20;
		prevSizeY = 20;
		prevSizeZ = 20;

		prevTessU = 1;
		prevTessV = 1;
		prevTessBase = 1;

		this->SetWindowText( "Box properties" );

		if( ValidateData() )
		{
			m_pScene->BeginShadowMeshBoxDrawing( &m_objPos );
			m_pScene->SetShadowMeshParams( &D3DXVECTOR3( 1, 1, 1 ), &D3DXVECTOR3( 1, 1, 1 ) );
		}
	}
	else if( m_mode == TMDM_CYLINDER )
	{
		this->SetWindowText( "Cylinder properties" );

		if( ValidateData() )
		{
			//m_pScene->BeginShadowMeshBoxDrawing( &m_objPos );
			//m_pScene->SetShadowMeshParams( &D3DXVECTOR3( 1, 1, 1 ), &D3DXVECTOR3( 1, 1, 1 ) );
		}

		prevSizeX = 20;
		prevSizeY = 10;
		prevSizeZ = 0;

		prevTessU = 1;
		prevTessV = 1;
		prevTessBase = 1;

		pLblSizeX->SetWindowText( "Height" );
		pLblSizeY->SetWindowText( "Radius" );
		pLblSizeZ->SetWindowText( "--" );
		pLblTessU->SetWindowText( "U" );
		pLblTessV->SetWindowText( "V" );
		pLblTessBase->SetWindowText( "--" );

		pSliderSizeX->SetRange( 1, 300, true );
		pSliderSizeY->SetRange( 1, 300, true );
		pSliderSizeZ->SetRange( 0, 0, true );

		pSliderTessU->SetRange( 1, 24, true );
		pSliderTessV->SetRange( 1, 24, true );
		pSliderTessBase->SetRange( 1, 1, true );

		pSliderSizeX->SetPos( 20 );
		pSliderSizeY->SetPos( 10 );
		pSliderSizeZ->SetPos( 0 );
		pSliderTessU->SetPos( 1 );
		pSliderTessV->SetPos( 1 );
		pSliderTessBase->SetPos( 1 );

		pEditSizeX->SetWindowText( "1.00" );
		pEditSizeY->SetWindowText( "0.50" );
		pEditSizeZ->SetWindowText( "" );
		pEditTessU->SetWindowText( "1" );
		pEditTessV->SetWindowText( "1" );
		pEditTessBase->SetWindowText( "" );


		pSliderSizeX->EnableWindow( true );
		pEditSizeX->EnableWindow( true );
		pSliderSizeY->EnableWindow( true );
		pEditSizeY->EnableWindow( true );
		pSliderSizeZ->EnableWindow( false );
		pEditSizeZ->EnableWindow( false );

		pSliderTessU->EnableWindow( true );
		pEditTessU->EnableWindow( true );
		pSliderTessV->EnableWindow( true );
		pEditTessV->EnableWindow( true );
		pSliderTessBase->EnableWindow( false );
		pEditTessBase->EnableWindow( false );

		pLockSize->SetCheck( true );
		pLockTess->SetCheck( false );
	}

	if( bShow == SW_HIDE )
	{
		if( m_pScene )
			m_pScene->EndShadowMeshDrawing();
	}
}

BOOL CTesselatedMeshDlg::Create()
{
	return CDialog::Create(CTesselatedMeshDlg::IDD);
}

void CTesselatedMeshDlg::UpdateShadowMesh()
{
	CSliderCtrl* pSliderSizeX = (CSliderCtrl*)GetDlgItem( IDC_SLIDER_SIZE_X );
	CSliderCtrl* pSliderSizeY = (CSliderCtrl*)GetDlgItem( IDC_SLIDER_SIZE_Y );
	CSliderCtrl* pSliderSizeZ = (CSliderCtrl*)GetDlgItem( IDC_SLIDER_SIZE_Z );
	CSliderCtrl* pSliderTessU = (CSliderCtrl*)GetDlgItem( IDC_SLIDER_TESSELATION_U );
	CSliderCtrl* pSliderTessV = (CSliderCtrl*)GetDlgItem( IDC_SLIDER_TESSELATION_V );
	CSliderCtrl* pSliderTessBase = (CSliderCtrl*)GetDlgItem( IDC_SLIDER_TESSELATION_BASE );

	if( m_pScene )
		m_pScene->SetShadowMeshParams( &D3DXVECTOR3( pSliderSizeX->GetPos() * 0.05f, pSliderSizeY->GetPos() * 0.05f, pSliderSizeZ->GetPos() * 0.05f ), &D3DXVECTOR3( (float)pSliderTessU->GetPos(), (float)pSliderTessV->GetPos(), (float)pSliderTessBase->GetPos() ) );
}

void CTesselatedMeshDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if( ValidateData() )
	{
		CEdit* pEditSizeX = (CEdit*)GetDlgItem( IDC_EDIT_SIZE_X );
		CEdit* pEditSizeY = (CEdit*)GetDlgItem( IDC_EDIT_SIZE_Y );
		CEdit* pEditSizeZ = (CEdit*)GetDlgItem( IDC_EDIT_SIZE_Z );
		CEdit* pEditTessU = (CEdit*)GetDlgItem( IDC_EDIT_TESSELATION_U );
		CEdit* pEditTessV = (CEdit*)GetDlgItem( IDC_EDIT_TESSELATION_V );
		CEdit* pEditTessBase = (CEdit*)GetDlgItem( IDC_EDIT_TESSELATION_BASE );

		CButton* pLockSize = (CButton*)GetDlgItem( IDC_CHECK_LOCK_SIZE );
		CButton* pLockTess = (CButton*)GetDlgItem( IDC_CHECK_LOCK_TESSELATION );

		CSliderCtrl* pSliderSizeX = (CSliderCtrl*)GetDlgItem( IDC_SLIDER_SIZE_X );
		CSliderCtrl* pSliderSizeY = (CSliderCtrl*)GetDlgItem( IDC_SLIDER_SIZE_Y );
		CSliderCtrl* pSliderSizeZ = (CSliderCtrl*)GetDlgItem( IDC_SLIDER_SIZE_Z );
		CSliderCtrl* pSliderTessU = (CSliderCtrl*)GetDlgItem( IDC_SLIDER_TESSELATION_U );
		CSliderCtrl* pSliderTessV = (CSliderCtrl*)GetDlgItem( IDC_SLIDER_TESSELATION_V );
		CSliderCtrl* pSliderTessBase = (CSliderCtrl*)GetDlgItem( IDC_SLIDER_TESSELATION_BASE );

		CString strTemp;

		strTemp.Format( "%d", ((CSliderCtrl*)pScrollBar)->GetPos() );

		if( pScrollBar->GetDlgCtrlID() == IDC_SLIDER_SIZE_X )
		{
			strTemp.Format( "%.2f", ((CSliderCtrl*)pScrollBar)->GetPos() * 0.05f );
			pEditSizeX->SetWindowText( strTemp );

			if( pLockSize->GetCheck() )
			{
				int Xp = ((CSliderCtrl*)pScrollBar)->GetPos() - prevSizeX;

				if( pSliderSizeY->IsWindowEnabled() )
				{
					pSliderSizeY->SetPos( pSliderSizeY->GetPos() + Xp );
					strTemp.Format( "%.2f", pSliderSizeY->GetPos() * 0.05f );
					pEditSizeY->SetWindowText( strTemp );
					prevSizeY = pSliderSizeY->GetPos();
				}

				if( pSliderSizeZ->IsWindowEnabled() )
				{
					pSliderSizeZ->SetPos( pSliderSizeZ->GetPos() + Xp );
					strTemp.Format( "%.2f", pSliderSizeZ->GetPos() * 0.05f );
					pEditSizeZ->SetWindowText( strTemp );
					prevSizeZ = pSliderSizeZ->GetPos();
				}
			}

			prevSizeX = ((CSliderCtrl*)pScrollBar)->GetPos();

			UpdateShadowMesh();
		}
		else if( pScrollBar->GetDlgCtrlID() == IDC_SLIDER_SIZE_Y )
		{
			strTemp.Format( "%.2f", ((CSliderCtrl*)pScrollBar)->GetPos() * 0.05f );
			pEditSizeY->SetWindowText( strTemp );

			if( pLockSize->GetCheck() )
			{
				int Yp = ((CSliderCtrl*)pScrollBar)->GetPos() - prevSizeY;

				if( pSliderSizeX->IsWindowEnabled() )
				{
					pSliderSizeX->SetPos( pSliderSizeX->GetPos() + Yp );
					strTemp.Format( "%.2f", pSliderSizeX->GetPos() * 0.05f );
					pEditSizeX->SetWindowText( strTemp );
					prevSizeX = pSliderSizeX->GetPos();
				}

				if( pSliderSizeZ->IsWindowEnabled() )
				{
					pSliderSizeZ->SetPos( pSliderSizeZ->GetPos() + Yp );
					strTemp.Format( "%.2f", pSliderSizeZ->GetPos() * 0.05f );
					pEditSizeZ->SetWindowText( strTemp );
					prevSizeZ = pSliderSizeZ->GetPos();
				}
			}

			prevSizeY = ((CSliderCtrl*)pScrollBar)->GetPos();

			UpdateShadowMesh();
		}
		else if( pScrollBar->GetDlgCtrlID() == IDC_SLIDER_SIZE_Z )
		{
			strTemp.Format( "%.2f", ((CSliderCtrl*)pScrollBar)->GetPos() * 0.05f );
			pEditSizeZ->SetWindowText( strTemp );

			if( pLockSize->GetCheck() )
			{
				int Zp = ((CSliderCtrl*)pScrollBar)->GetPos() - prevSizeZ;

				if( pSliderSizeX->IsWindowEnabled() )
				{
					pSliderSizeX->SetPos( pSliderSizeX->GetPos() + Zp );
					strTemp.Format( "%.2f", pSliderSizeX->GetPos() * 0.05f );
					pEditSizeX->SetWindowText( strTemp );
					prevSizeX = pSliderSizeX->GetPos();
				}

				if( pSliderSizeY->IsWindowEnabled() )
				{
					pSliderSizeY->SetPos( pSliderSizeY->GetPos() + Zp );
					strTemp.Format( "%.2f", pSliderSizeY->GetPos() * 0.05f );
					pEditSizeY->SetWindowText( strTemp );
					prevSizeY = pSliderSizeY->GetPos();
				}
			}

			prevSizeZ = ((CSliderCtrl*)pScrollBar)->GetPos();

			UpdateShadowMesh();
		}
		else if( pScrollBar->GetDlgCtrlID() == IDC_SLIDER_TESSELATION_U )
		{
			strTemp.Format( "%d", ((CSliderCtrl*)pScrollBar)->GetPos() );
			pEditTessU->SetWindowText( strTemp );

			if( pLockTess->GetCheck() )
			{
				int Up = ((CSliderCtrl*)pScrollBar)->GetPos() - prevTessU;

				if( pSliderTessV->IsWindowEnabled() )
				{
					pSliderTessV->SetPos( pSliderTessV->GetPos() + Up );
					strTemp.Format( "%d", pSliderTessV->GetPos() );
					pEditTessV->SetWindowText( strTemp );
					prevTessV = pSliderTessV->GetPos();
				}

				if( pSliderTessBase->IsWindowEnabled() )
				{
					pSliderTessBase->SetPos( pSliderTessBase->GetPos() + Up );
					strTemp.Format( "%d", pSliderTessBase->GetPos() );
					pEditTessBase->SetWindowText( strTemp );
					prevTessBase = pSliderTessBase->GetPos();
				}
			}

			prevTessU = ((CSliderCtrl*)pScrollBar)->GetPos();

			UpdateShadowMesh();
		}
		else if( pScrollBar->GetDlgCtrlID() == IDC_SLIDER_TESSELATION_V )
		{
			strTemp.Format( "%d", ((CSliderCtrl*)pScrollBar)->GetPos() );
			pEditTessV->SetWindowText( strTemp );

			if( pLockTess->GetCheck() )
			{
				int Vp = ((CSliderCtrl*)pScrollBar)->GetPos() - prevTessV;

				if( pSliderTessU->IsWindowEnabled() )
				{
					pSliderTessU->SetPos( pSliderTessU->GetPos() + Vp );
					strTemp.Format( "%d", pSliderTessU->GetPos() );
					pEditTessU->SetWindowText( strTemp );
					prevTessU = pSliderTessU->GetPos();
				}

				if( pSliderTessBase->IsWindowEnabled() )
				{
					pSliderTessBase->SetPos( pSliderTessBase->GetPos() + Vp );
					strTemp.Format( "%d", pSliderTessBase->GetPos() );
					pEditTessBase->SetWindowText( strTemp );
					prevTessBase = pSliderTessBase->GetPos();
				}
			}

			prevTessV = ((CSliderCtrl*)pScrollBar)->GetPos();

			UpdateShadowMesh();
		}
		else if( pScrollBar->GetDlgCtrlID() == IDC_SLIDER_TESSELATION_BASE )
		{
			strTemp.Format( "%d", ((CSliderCtrl*)pScrollBar)->GetPos() );
			pEditTessBase->SetWindowText( strTemp );

			if( pLockTess->GetCheck() )
			{
				int Bp = ((CSliderCtrl*)pScrollBar)->GetPos() - prevTessBase;

				if( pSliderTessU->IsWindowEnabled() )
				{
					pSliderTessU->SetPos( pSliderTessU->GetPos() + Bp );
					strTemp.Format( "%d", pSliderTessU->GetPos() );
					pEditTessU->SetWindowText( strTemp );
					prevTessU = pSliderTessU->GetPos();
				}

				if( pSliderTessV->IsWindowEnabled() )
				{
					pSliderTessV->SetPos( pSliderTessV->GetPos() + Bp );
					strTemp.Format( "%d", pSliderTessV->GetPos() );
					pEditTessV->SetWindowText( strTemp );
					prevTessV = pSliderTessV->GetPos();
				}
			}

			prevTessBase = ((CSliderCtrl*)pScrollBar)->GetPos();

			UpdateShadowMesh();
		}
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CTesselatedMeshDlg::OnClose()
{
	if( m_pScene )
		m_pScene->EndShadowMeshDrawing();

	CDialog::OnClose();
}

void CTesselatedMeshDlg::OnDestroy()
{
	CDialog::OnDestroy();

	if( m_pScene )
		m_pScene->EndShadowMeshDrawing();
}

void CTesselatedMeshDlg::OnBnClickedOk()
{
	CEdit* pEditID = (CEdit*)GetDlgItem( IDC_EDIT_ID );

	CSliderCtrl* pSliderSizeX = (CSliderCtrl*)GetDlgItem( IDC_SLIDER_SIZE_X );
	CSliderCtrl* pSliderSizeY = (CSliderCtrl*)GetDlgItem( IDC_SLIDER_SIZE_Y );
	CSliderCtrl* pSliderSizeZ = (CSliderCtrl*)GetDlgItem( IDC_SLIDER_SIZE_Z );
	CSliderCtrl* pSliderTessU = (CSliderCtrl*)GetDlgItem( IDC_SLIDER_TESSELATION_U );
	CSliderCtrl* pSliderTessV = (CSliderCtrl*)GetDlgItem( IDC_SLIDER_TESSELATION_V );
	CSliderCtrl* pSliderTessBase = (CSliderCtrl*)GetDlgItem( IDC_SLIDER_TESSELATION_BASE );

	//check for existing mesh
	CString ID;
	pEditID->GetWindowText( ID );

	ID.Trim();

	if( ((CEfnMeshlist*)m_pScene->GetMeshlist())->GetMeshByName2( ID.GetBuffer() )  )
	{
		MessageBox( "Объект с таким именем уже существует!", "Внимание!", MB_OK | MB_ICONWARNING );
		return;
	}
	else if( ID.GetLength() == 0 )
	{
		MessageBox( "Укажите имя объекта!", "Внимание!", MB_OK | MB_ICONWARNING );
		return;
	}

	if( m_mode == TMDM_BOX )
	{
		D3DXVECTOR3 vSize( pSliderSizeX->GetPos() * 0.05f, pSliderSizeY->GetPos() * 0.05f, pSliderSizeZ->GetPos() * 0.05f );
		D3DXVECTOR3 vTess( (float)pSliderTessU->GetPos(), (float)pSliderTessV->GetPos(), (float)pSliderTessBase->GetPos() );

		m_pScene->CreateMeshFromShadowMesh( ID, &m_objPos, &vSize, &vTess, ((CCookieMachineDlg*)GetParent())->GetD3DDeviceGlob() );

		if( m_pScene )
			m_pScene->EndShadowMeshDrawing();

		UINT	  uiMask		= TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		HTREEITEM hMeshes	= ((CCookieMachineDlg*)GetParent())->GetSceneExplorerGlob()->FindTreeItemByName( "Meshes" );
		HTREEITEM hMesh		= ((CCookieMachineDlg*)GetParent())->GetSceneExplorerGlob()->InsertItem( uiMask, ID, 0, 0, 0, 0, 0, hMeshes, TVI_SORT ); 

		((CCookieMachineDlg*)GetParent())->GetSceneExplorerGlob()->SelectItem( hMesh );
		((CCookieMachineDlg*)GetParent())->GetSceneExplorerGlob()->EnsureVisible( hMesh );
	}

	OnOK();
}

void CTesselatedMeshDlg::OnBnClickedCancel()
{
	if( m_pScene )
		m_pScene->EndShadowMeshDrawing();

	OnCancel();
}
