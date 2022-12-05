#include "MemMappingsMgr.h"
#include "dialogs\CookieMachineDlg.h"

CMemMappingsMgr::CMemMappingsMgr(void) : m_pParentWnd(NULL), QEM_SEND_FIXED_STRING_64K(0), QEM_QUEST_UPDATE_CUSTOM_MESH(0)
{
	QEM_SEND_FIXED_STRING_64K		= RegisterWindowMessage( "QEM_SEND_FIXED_STRING_64K" );
	QEM_QUEST_UPDATE_CUSTOM_MESH	= RegisterWindowMessage( "QEM_QUEST_UPDATE_CUSTOM_MESH" );
	QEM_QUEST_UPDATE_MTRL			= RegisterWindowMessage( "QEM_QUEST_UPDATE_MTRL" );

	if( QEM_SEND_FIXED_STRING_64K == 0 ||
		QEM_QUEST_UPDATE_CUSTOM_MESH == 0 ||
		QEM_QUEST_UPDATE_MTRL == 0 )
	{
		AfxMessageBox( "Error message registering!" );
	}

	ReaquireMappings();
	g_MemMappingsMgr->m_MeshData->Reset();
}

CMemMappingsMgr::~CMemMappingsMgr(void)
{

}

void CMemMappingsMgr::ReaquireMappings()
{
	BOOL a = m_MeshData.Acquire( "QuestRenderCustomMesh" );
	BOOL b = m_StrData.Acquire( "FixedString64K" );
	BOOL c = m_ExactSceneData.Acquire( "CurrentQuestSceneFullPath" );

	if( !a || !b || !c ) 
		m_pParentWnd->SetWindowText( "Acquire failed!" );
}

void CMemMappingsMgr::PostMessageToEngine( UINT message, WPARAM wParam, LPARAM lParam )
{
	HWND hWnd = FindWindow( NULL, "ßיצא װאבונזו" );

	if( !IsWindow( hWnd ) )
		return;	

	static DWORD time	= 0;
	static buff_count	= 0;
	DWORD timeSample	= timeGetTime();

	if(  timeSample - time < 20 && buff_count > 2 )
	{
		buff_count = 0;
		return;
	}

	time = timeSample;
	buff_count++;

	PostMessage( hWnd, message, wParam, lParam );
}

void CMemMappingsMgr::PostScriptToEngine( CString strScript, bool bCheckExactScene )
{
	if( bCheckExactScene )
	{
		if( IsExactScene() == false )
			return;
		
		//AfxMessageBox( "scene ok" );
	}

	m_StrData.Acquire( "FixedString64K" );
	sprintf( m_StrData->text, "%s", strScript.GetBuffer() );

	PostMessageToEngine( QEM_SEND_FIXED_STRING_64K );
}

bool CMemMappingsMgr::IsExactScene()
{
	if( !m_pParentWnd )
		return false;

	CBaseScene* pScene = ((CCookieMachineDlg*)m_pParentWnd)->GetSceneGlob();

	if( !pScene )
		return false;

	str_t path = pScene->GetFolder() + pScene->GetFilename();

	if( path == "" )
		return false;
		
	m_ExactSceneData.Acquire( "CurrentQuestSceneFullPath" );

	CString strEditorScenePath = path.data();
	CString strEngineScenePath = m_ExactSceneData->text;

	//trim and uppercase
	strEditorScenePath = strEditorScenePath.Trim();
	strEditorScenePath.MakeUpper();

	strEngineScenePath = strEngineScenePath.Trim();
	strEngineScenePath.MakeUpper();

	if( strEditorScenePath == strEngineScenePath )
		return true;

	return false;
}