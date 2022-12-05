#include "../sys/stdafx.h"
#include "../sys/CookieMachine.h"
#include "PropNullDlg.h"
#include ".\propnulldlg.h"


IMPLEMENT_DYNAMIC(CPropNullDlg, CDialog)
CPropNullDlg::CPropNullDlg(CWnd* pParent /*=NULL*/)
	: CBaseDialog(CPropNullDlg::IDD, pParent)
{
	m_pNull2	= NULL;
	m_pNull		= NULL;
}

CPropNullDlg::~CPropNullDlg()
{
}

void CPropNullDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPropNullDlg, CDialog)
	ON_WM_CTLCOLOR()
	ON_EN_KILLFOCUS(IDC_SCALE_X, OnEnKillfocusScaleX)
	ON_EN_KILLFOCUS(IDC_SCALE_Y, OnEnKillfocusScaleY)
	ON_EN_KILLFOCUS(IDC_SCALE_Z, OnEnKillfocusScaleZ)
	ON_EN_KILLFOCUS(IDC_ROT_X, OnEnKillfocusRotX)
	ON_EN_KILLFOCUS(IDC_ROT_Y, OnEnKillfocusRotY)
	ON_EN_KILLFOCUS(IDC_ROT_Z, OnEnKillfocusRotZ)
	ON_EN_KILLFOCUS(IDC_TRANS_X, OnEnKillfocusTransX)
	ON_EN_KILLFOCUS(IDC_TRANS_Y, OnEnKillfocusTransY)
	ON_EN_KILLFOCUS(IDC_TRANS_Z, OnEnKillfocusTransZ)
	ON_EN_SETFOCUS(IDC_SCALE_X, OnEnSetfocusScaleX)
	ON_EN_SETFOCUS(IDC_SCALE_Y, OnEnSetfocusScaleY)
	ON_EN_SETFOCUS(IDC_SCALE_Z, OnEnSetfocusScaleZ)
	ON_EN_SETFOCUS(IDC_ROT_X, OnEnSetfocusRotX)
	ON_EN_SETFOCUS(IDC_ROT_Y, OnEnSetfocusRotY)
	ON_EN_SETFOCUS(IDC_ROT_Z, OnEnSetfocusRotZ)
	ON_EN_SETFOCUS(IDC_TRANS_X, OnEnSetfocusTransX)
	ON_EN_SETFOCUS(IDC_TRANS_Y, OnEnSetfocusTransY)
	ON_EN_SETFOCUS(IDC_TRANS_Z, OnEnSetfocusTransZ)
END_MESSAGE_MAP()


void CPropNullDlg::ReleaseLinkedData()
{
	m_pNull2	= NULL;
	m_pNull		= NULL;
}

void CPropNullDlg::Populate( int iFrameIndex, int iStartFrame, int iEndFrame )
{
	if( ( !m_pNull2 && !m_pNull ) || ( iFrameIndex < 0 && iFrameIndex != -1 ) )
		return;

	if( iStartFrame != -1 && iEndFrame != -1 )
	{
		m_iStartFrame	= iStartFrame;
		m_iEndFrame		= iEndFrame;
	}

	if( iFrameIndex != -1 )
		m_iFrameIndex = iFrameIndex;
	else
		iFrameIndex = m_iFrameIndex;

	CString strTemp;

	if( m_pNull2 )
	{
		m_pID->SetWindowText( m_pNull2->id.data() );

		//scale
		strTemp.Format( "%0.4f", m_pNull2->matrices[iFrameIndex].scl.x );
		m_pScaleX->SetWindowText( strTemp );

		strTemp.Format( "%0.4f", m_pNull2->matrices[iFrameIndex].scl.y );
		m_pScaleY->SetWindowText( strTemp );

		strTemp.Format( "%0.4f", m_pNull2->matrices[iFrameIndex].scl.z );
		m_pScaleZ->SetWindowText( strTemp );


		//rot
		strTemp.Format( "%0.4f", m_pNull2->matrices[iFrameIndex].rot.x );
		m_pRotX->SetWindowText( strTemp );

		strTemp.Format( "%0.4f", m_pNull2->matrices[iFrameIndex].rot.y );
		m_pRotY->SetWindowText( strTemp );

		strTemp.Format( "%0.4f", m_pNull2->matrices[iFrameIndex].rot.z );
		m_pRotZ->SetWindowText( strTemp );


		//trans
		strTemp.Format( "%0.4f", m_pNull2->matrices[iFrameIndex].pos.x );
		m_pTranslX->SetWindowText( strTemp );

		strTemp.Format( "%0.4f", m_pNull2->matrices[iFrameIndex].pos.y );
		m_pTranslY->SetWindowText( strTemp );

		strTemp.Format( "%0.4f", m_pNull2->matrices[iFrameIndex].pos.z );
		m_pTranslZ->SetWindowText( strTemp );
	}
	else if( m_pNull )
	{
		m_pID->SetWindowText( m_pNull->id.data() );


		//scale
		strTemp.Format( "%0.4f", m_pNull->matrices[iFrameIndex][6] );
		m_pScaleX->SetWindowText( strTemp );

		strTemp.Format( "%0.4f", m_pNull->matrices[iFrameIndex][7] );
		m_pScaleY->SetWindowText( strTemp );

		strTemp.Format( "%0.4f", m_pNull->matrices[iFrameIndex][8] );
		m_pScaleZ->SetWindowText( strTemp );


		//rot
		strTemp.Format( "%0.4f", m_pNull->matrices[iFrameIndex][3] );
		m_pRotX->SetWindowText( strTemp );

		strTemp.Format( "%0.4f", m_pNull->matrices[iFrameIndex][4] );
		m_pRotY->SetWindowText( strTemp );

		strTemp.Format( "%0.4f", m_pNull->matrices[iFrameIndex][5] );
		m_pRotZ->SetWindowText( strTemp );


		//trans
		strTemp.Format( "%0.4f", m_pNull->matrices[iFrameIndex][0] );
		m_pTranslX->SetWindowText( strTemp );

		strTemp.Format( "%0.4f", m_pNull->matrices[iFrameIndex][1] );
		m_pTranslY->SetWindowText( strTemp );

		strTemp.Format( "%0.4f", m_pNull->matrices[iFrameIndex][2] );
		m_pTranslZ->SetWindowText( strTemp );	
	}

	UpdateWindow();
}

bool CPropNullDlg::IsEditedBox( int id )
{
	if( id == IDC_SCALE_X ||
		id == IDC_SCALE_Y ||
		id == IDC_SCALE_Z ||
		id == IDC_TRANS_X ||
		id == IDC_TRANS_Y ||
		id == IDC_TRANS_Z ||
		id == IDC_ROT_X ||
		id == IDC_ROT_Y ||
		id == IDC_ROT_Z )
	{
		return true;
	}

	return false;
}

HBRUSH CPropNullDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CBaseDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	pDC->SetTextColor( RGB(0, 0, 0) );
	pDC->SetBkMode( TRANSPARENT );

	CWnd* pFocWnd = GetFocus();

	if( pFocWnd && pWnd->GetDlgCtrlID() == pFocWnd->GetDlgCtrlID() && IsEditedBox( pWnd->GetDlgCtrlID() ) )
	{
		hbr = m_brWhite;
	}

	return hbr;
}

void CPropNullDlg::UpdateDataFromControl( UINT id )
{
	CString wnd_txt;
	CWnd*	pWnd = GetDlgItem( id );

	pWnd->GetWindowText( wnd_txt );

	float fValue = (float)atof( wnd_txt.GetBuffer() );

	if( !m_pNull && !m_pNull2 )
		return;

	switch( id )
	{
		case IDC_SCALE_X:
			{
				if( m_pNull2 )
				{
					if( m_iStartFrame == -1 && m_iEndFrame == -1 )
						m_pNull2->matrices[m_iFrameIndex].scl.x = fValue;
					else
					{
						for( int i = m_iStartFrame; i <= m_iEndFrame; i++ )
							m_pNull2->matrices[i].scl.x = fValue;
					}
				}
				else
				{
					if( m_iStartFrame == -1 && m_iEndFrame == -1 )
						m_pNull->matrices[m_iFrameIndex][6] = fValue;
					else
					{
						for( int i = m_iStartFrame; i <= m_iEndFrame; i++ )
							m_pNull->matrices[i][6] = fValue;
					}
				}
			}
			break;

		case IDC_SCALE_Y:
			{
				if( m_pNull2 )
				{
					if( m_iStartFrame == -1 && m_iEndFrame == -1 )
						m_pNull2->matrices[m_iFrameIndex].scl.y = fValue;
					else
					{
						for( int i = m_iStartFrame; i <= m_iEndFrame; i++ )
							m_pNull2->matrices[i].scl.y = fValue;
					}
				}
				else
				{
					if( m_iStartFrame == -1 && m_iEndFrame == -1 )
						m_pNull->matrices[m_iFrameIndex][7] = fValue;
					else
					{
						for( int i = m_iStartFrame; i <= m_iEndFrame; i++ )
							m_pNull->matrices[i][7] = fValue;
					}
				}
			}
			break;

		case IDC_SCALE_Z:
			{
				if( m_pNull2 )
				{
					if( m_iStartFrame == -1 && m_iEndFrame == -1 )
						m_pNull2->matrices[m_iFrameIndex].scl.z = fValue;
					else
					{
						for( int i = m_iStartFrame; i <= m_iEndFrame; i++ )
							m_pNull2->matrices[i].scl.z = fValue;
					}
				}
				else
				{
					if( m_iStartFrame == -1 && m_iEndFrame == -1 )
						m_pNull->matrices[m_iFrameIndex][8] = fValue;
					else
					{
						for( int i = m_iStartFrame; i <= m_iEndFrame; i++ )
							m_pNull->matrices[i][8] = fValue;
					}
				}
			}
			break;

		case IDC_ROT_X:
			{
				if( m_pNull2 )
				{
					if( m_iStartFrame == -1 && m_iEndFrame == -1 )
						m_pNull2->matrices[m_iFrameIndex].rot.x = fValue;
					else
					{
						for( int i = m_iStartFrame; i <= m_iEndFrame; i++ )
							m_pNull2->matrices[i].rot.x = fValue;
					}
				}
				else
				{
					if( m_iStartFrame == -1 && m_iEndFrame == -1 )
						m_pNull->matrices[m_iFrameIndex][3] = fValue;
					else
					{
						for( int i = m_iStartFrame; i <= m_iEndFrame; i++ )
							m_pNull->matrices[i][3] = fValue;
					}
				}
			}
			break;

		case IDC_ROT_Y:
			{
				if( m_pNull2 )
				{
					if( m_iStartFrame == -1 && m_iEndFrame == -1 )
						m_pNull2->matrices[m_iFrameIndex].rot.y = fValue;
					else
					{
						for( int i = m_iStartFrame; i <= m_iEndFrame; i++ )
							m_pNull2->matrices[i].rot.y = fValue;
					}
				}
				else
				{
					if( m_iStartFrame == -1 && m_iEndFrame == -1 )
						m_pNull->matrices[m_iFrameIndex][4] = fValue;
					else
					{
						for( int i = m_iStartFrame; i <= m_iEndFrame; i++ )
							m_pNull->matrices[i][4] = fValue;
					}
				}
			}
			break;

		case IDC_ROT_Z:
			{
				if( m_pNull2 )
				{
					if( m_iStartFrame == -1 && m_iEndFrame == -1 )
						m_pNull2->matrices[m_iFrameIndex].rot.z = fValue;
					else
					{
						for( int i = m_iStartFrame; i <= m_iEndFrame; i++ )
							m_pNull2->matrices[i].rot.z = fValue;
					}				
				}
				else
				{
					if( m_iStartFrame == -1 && m_iEndFrame == -1 )
						m_pNull->matrices[m_iFrameIndex][5] = fValue;
					else
					{
						for( int i = m_iStartFrame; i <= m_iEndFrame; i++ )
							m_pNull->matrices[i][5] = fValue;
					}
				}
			}
			break;

		case IDC_TRANS_X:
				if( m_pNull2 )
				{
					if( m_iStartFrame == -1 && m_iEndFrame == -1 )
						m_pNull2->matrices[m_iFrameIndex].pos.x = fValue;
					else
					{
						for( int i = m_iStartFrame; i <= m_iEndFrame; i++ )
							m_pNull2->matrices[i].pos.x = fValue;
					}						
				}
				else
				{
					if( m_iStartFrame == -1 && m_iEndFrame == -1 )
						m_pNull->matrices[m_iFrameIndex][0] = fValue;
					else
					{
						for( int i = m_iStartFrame; i <= m_iEndFrame; i++ )
							m_pNull->matrices[i][0] = fValue;
					}				
				}
			break;

		case IDC_TRANS_Y:
				if( m_pNull2 )
				{
					if( m_iStartFrame == -1 && m_iEndFrame == -1 )
						m_pNull2->matrices[m_iFrameIndex].pos.y = fValue;
					else
					{
						for( int i = m_iStartFrame; i <= m_iEndFrame; i++ )
							m_pNull2->matrices[i].pos.y = fValue;
					}					
				}
				else
				{
					if( m_iStartFrame == -1 && m_iEndFrame == -1 )
						m_pNull->matrices[m_iFrameIndex][1] = fValue;
					else
					{
						for( int i = m_iStartFrame; i <= m_iEndFrame; i++ )
							m_pNull->matrices[i][1] = fValue;
					}					
				}
			break;

		case IDC_TRANS_Z:
				if( m_pNull2 )
				{
					if( m_iStartFrame == -1 && m_iEndFrame == -1 )
						m_pNull2->matrices[m_iFrameIndex].pos.z = fValue;
					else
					{
						for( int i = m_iStartFrame; i <= m_iEndFrame; i++ )
							m_pNull2->matrices[i].pos.z = fValue;
					}									
				}
				else
				{
					if( m_iStartFrame == -1 && m_iEndFrame == -1 )
						m_pNull->matrices[m_iFrameIndex][2] = fValue;
					else
					{
						for( int i = m_iStartFrame; i <= m_iEndFrame; i++ )
							m_pNull->matrices[i][2] = fValue;
					}					
				}
			break;
	}

	Populate( m_iFrameIndex );
}

bool CPropNullDlg::IsEdited()
{
	CWnd* pFocWnd = GetFocus();

	if( pFocWnd && IsWindow( pFocWnd->GetSafeHwnd() ) ) 
		return IsEditedBox( pFocWnd->GetDlgCtrlID() );

	return true;
}

void CPropNullDlg::UpdateDataFromControl()
{
	CString wnd_txt;
	CWnd*	pWnd = GetFocus();

	if( pWnd )
		UpdateDataFromControl( pWnd->GetDlgCtrlID() );
}

BOOL CPropNullDlg::PreTranslateMessage(MSG* pMsg)
{
	switch( pMsg->wParam )
	{
		case VK_ESCAPE:
			{
				m_pID->SetFocus();
				return false;
			}
			break;

		case VK_RETURN:
			{
				CEdit* edit = (CEdit*)GetFocus();
				edit->SetSel( 0, -1 );
				
				UpdateDataFromControl();
				m_pID->SetFocus();
				return false;
			}
			break;

		case VK_F1:
			{
				return false;
			}
			break;
	}

	return CBaseDialog::PreTranslateMessage(pMsg);
}

BOOL CPropNullDlg::OnInitDialog()
{
	CBaseDialog::OnInitDialog();

	m_pID		= GetDlgItem( IDC_EDIT_ID );
	m_pScaleX	= GetDlgItem( IDC_SCALE_X );
	m_pScaleY	= GetDlgItem( IDC_SCALE_Y );
	m_pScaleZ	= GetDlgItem( IDC_SCALE_Z );
	m_pRotX		= GetDlgItem( IDC_ROT_X );
	m_pRotY		= GetDlgItem( IDC_ROT_Y );
	m_pRotZ		= GetDlgItem( IDC_ROT_Z );
	m_pTranslX	= GetDlgItem( IDC_TRANS_X );
	m_pTranslY	= GetDlgItem( IDC_TRANS_Y );
	m_pTranslZ	= GetDlgItem( IDC_TRANS_Z );

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CPropNullDlg::OnEnKillfocusScaleX()
{
	UpdateDataFromControl( IDC_SCALE_X );
}

void CPropNullDlg::OnEnKillfocusScaleY()
{
	UpdateDataFromControl( IDC_SCALE_Y );
}

void CPropNullDlg::OnEnKillfocusScaleZ()
{
	UpdateDataFromControl( IDC_SCALE_Z );
}

void CPropNullDlg::OnEnKillfocusRotX()
{
	UpdateDataFromControl( IDC_ROT_X );
}

void CPropNullDlg::OnEnKillfocusRotY()
{
	UpdateDataFromControl( IDC_ROT_Y );
}

void CPropNullDlg::OnEnKillfocusRotZ()
{
	UpdateDataFromControl( IDC_ROT_Z );
}

void CPropNullDlg::OnEnKillfocusTransX()
{
	UpdateDataFromControl( IDC_TRANS_X );
}

void CPropNullDlg::OnEnKillfocusTransY()
{
	UpdateDataFromControl( IDC_TRANS_Y );
}

void CPropNullDlg::OnEnKillfocusTransZ()
{
	UpdateDataFromControl( IDC_TRANS_Z );
}

void CPropNullDlg::OnEnSetfocusScaleX()
{
	m_pScaleX->Invalidate();
}

void CPropNullDlg::OnEnSetfocusScaleY()
{
	m_pScaleY->Invalidate();
}

void CPropNullDlg::OnEnSetfocusScaleZ()
{
	m_pScaleZ->Invalidate();
}

void CPropNullDlg::OnEnSetfocusRotX()
{
	m_pRotX->Invalidate();
}

void CPropNullDlg::OnEnSetfocusRotY()
{
	m_pRotY->Invalidate();
}

void CPropNullDlg::OnEnSetfocusRotZ()
{
	m_pRotZ->Invalidate();
}

void CPropNullDlg::OnEnSetfocusTransX()
{
	m_pTranslX->Invalidate();
}

void CPropNullDlg::OnEnSetfocusTransY()
{
	m_pTranslY->Invalidate();
}

void CPropNullDlg::OnEnSetfocusTransZ()
{
	m_pTranslZ->Invalidate();
}
