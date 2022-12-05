#include "../sys/stdafx.h"
#include "../sys/CookieMachine.h"
#include "FramesCropToolDlg.h"
#include ".\framescroptooldlg.h"


IMPLEMENT_DYNAMIC(CFramesCropToolDlg, CDialog)
CFramesCropToolDlg::CFramesCropToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFramesCropToolDlg::IDD, pParent)
{
	m_iFramesCount	= 0;
	ranges_del		= NULL;
}

CFramesCropToolDlg::~CFramesCropToolDlg()
{
}

void CFramesCropToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFramesCropToolDlg, CDialog)
	ON_BN_CLICKED(IDC_ADD_RANGE, OnBnClickedAddRange)
	ON_BN_CLICKED(IDC_DEL_RANGE, OnBnClickedDelRange)
	ON_BN_CLICKED(IDOK, OnBnClickedCrop)
END_MESSAGE_MAP()


void CFramesCropToolDlg::UpdateUI()
{
	CListBox* pList = (CListBox*)GetDlgItem( IDC_LIST_RANGES );

	pList->ResetContent();

	if( ranges.empty() == false )
	{
		for( UINT i = 0; i < ranges.size() / 2; i++ )
		{
			CString strTemp;
			strTemp.Format( "%0.4d -- %0.4d", ranges[i * 2], ranges[i * 2 + 1] );

			pList->AddString( strTemp );
		}
	}
}

void CFramesCropToolDlg::OnBnClickedAddRange()
{
	if( !ranges_del )
		return;

	if( m_iFramesCount == 0 )
	{
		AfxMessageBox( "Таймлайн пуст, кроп запрещён!" );
		return;
	}

	CString strTemp;
	int		iMin, iMax;
	CWnd*	pMin = GetDlgItem( IDC_EDIT_MIN );
	CWnd*	pMax = GetDlgItem( IDC_EDIT_MAX );

	pMin->GetWindowText( strTemp );
	iMin = atoi( strTemp.GetBuffer() );
	
	pMax->GetWindowText( strTemp );
	iMax = atoi( strTemp.GetBuffer() );

	if( iMin > 9999 || iMax > 9999 )
	{
		AfxMessageBox( "Максимальное значение -- 9999!" );
		return;
	}

	if( iMin < 0 || iMin >= m_iFramesCount || iMax < 0 || iMax >= m_iFramesCount )
	{
		AfxMessageBox( "Значения за пределами таймлайна!" );
		return;
	}

	if( iMin > iMax )
	{
		AfxMessageBox( "Начальное значение не может быть больше конечного!" );
		return;
	}

	if( ranges.empty() == false )
	{
		for( UINT i = 0; i < ranges.size() / 2; i++ )
		{
			if( ranges[i * 2] == iMin && ranges[i * 2 + 1] == iMax )
			{
				AfxMessageBox( "Значения уже добавлены!" );
				return;
			}
		}

		for( UINT i = 0; i < ranges.size() / 2; i++ )
		{
			if( (iMin >= ranges[i * 2] && iMin <= ranges[i * 2 + 1]) || (iMax >= ranges[i * 2] && iMax <= ranges[i * 2 + 1]) || 
				(ranges[i * 2] >= iMin && ranges[i * 2] <= iMax) || (ranges[i * 2 + 1] >= iMin && ranges[i * 2 + 1] <= iMax ) )
			{
				AfxMessageBox( "Значения пересекаются с уже добавленными!" );
				return;
			}
		}
	}

	ranges.push_back( iMin );
	ranges.push_back( iMax );

	UpdateUI();
}

void CFramesCropToolDlg::OnBnClickedDelRange()
{
	CListBox* pList = (CListBox*)GetDlgItem( IDC_LIST_RANGES );
	
	CString strMin, strMax;
	int iMin, iMax;

	if( pList->GetCount() == 0 || pList->GetSelCount() == 0 || pList->GetCurSel() == LB_ERR )
		return;

	pList->GetText( pList->GetCurSel(), strMin );

	strMax = strMin.Right(4);
	strMin = strMin.Left(4);

	iMin = atoi( strMin.GetBuffer() );
	iMax = atoi( strMax.GetBuffer() );

	if( ranges.empty() == false )
	{
		for( UINT i = 0; i < ranges.size() / 2; i++ )
		{
			if( ranges[i * 2] == iMin && ranges[i * 2 + 1] == iMax )
			{
				ranges.erase( ranges.begin() + i * 2 );
				ranges.erase( ranges.begin() + i * 2 );

				pList->DeleteString( pList->GetCurSel() );
				return;
			}
		}
	}
}

void CFramesCropToolDlg::OnBnClickedCrop()
{
	if( ranges.empty() || ranges.size() == 0 || ranges_del == NULL )
		return;

	for( int i = 0; i < ((int)ranges.size() / 2) - 1; i++ )
	{
		for( int j = i + 1; j < (int)ranges.size() / 2; j++ )
		{
			if( (int)ranges[i * 2] > (int)ranges[j * 2] )
			{
				int iTemp	= (int)ranges[i * 2];
				int iTemp2	= (int)ranges[i * 2 + 1];

				(int)ranges[i * 2]		= (int)ranges[j * 2];
				(int)ranges[i * 2 + 1]	= (int)ranges[j * 2 + 1];

				(int)ranges[j * 2]		= iTemp;
				(int)ranges[j * 2 + 1]	= iTemp2;
			}
		}
	}

	ranges_del->clear();

	for( int i = 0; i < (int)ranges.size() / 2; i++ )
	{
		if( i == 0 && (int)ranges[i * 2] > 0 )
		{
			ranges_del->push_back(0);
			ranges_del->push_back(ranges[i * 2] - 1);
		}
		
		if( i == (int)ranges.size() / 2 - 1 && (int)ranges[i * 2 + 1] < m_iFramesCount - 1 )
		{
			ranges_del->push_back((int)ranges[i * 2 + 1] + 1);
			ranges_del->push_back(m_iFramesCount - 1);
		}

		if( i < (int)ranges.size() / 2 - 1 && (int)ranges[(i + 1) * 2] - (int)ranges[i * 2 + 1] > 1 )
		{
			ranges_del->push_back((int)ranges[i * 2 + 1] + 1);
			ranges_del->push_back((int)ranges[(i + 1) * 2] - 1);
		}
	}

	OnOK();
}
