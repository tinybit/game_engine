#define STRICT

#pragma warning(disable: 4244)

#include "../sys/stdafx.h"
#include "../sys/CookieMachine.h"
#include "CookieMachineDlg.h"
#include <d3d9.h>
#include <d3dx9math.h>
#include <mmsystem.h>

#include "../utils/DXUtil.h"
#include "../utils/Common.h"

#include "Shlwapi.h"
#include "Windows.h"
#include "shlobj.h"

int			g_iMouseX, g_iMouseY;
CPoint		mouse_point;
int			firstSelFrame	= -1;
int			lastSelFrame	= -1;
bool		g_bDragging		= false;

LPDIRECT3D9				g_pD3D			= NULL;
LPDIRECT3DDEVICE9		g_pd3dDevice	= NULL; 
D3DPRESENT_PARAMETERS	g_d3dpp;
LPDIRECT3DVERTEXBUFFER9	g_pGridVB		= NULL;
LPD3DXMESH				ppMesh			= NULL;
LPD3DXMESH				ppCylinder		= NULL;
CVector					isect;

#define KEYDOWN(vk_code)		((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEYUP(vk_code)			((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

BOOL DoNotify( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );
void ShowAxisAlignMenuGlob();
void ShowFloorsMenuGlob();

#define ID_MENU_RECENT_FILE_1 6661
#define ID_MENU_RECENT_FILE_2 6662
#define ID_MENU_RECENT_FILE_3 6663
#define ID_MENU_RECENT_FILE_4 6664
#define ID_MENU_RECENT_FILE_5 6665
#define ID_MENU_RECENT_FILE_6 6666
#define ID_MENU_RECENT_FILE_7 6667
#define ID_MENU_RECENT_FILE_8 6668
#define ID_MENU_RECENT_FILE_9 6669

#define ID_MENU_RECENT_DIR_1 6671
#define ID_MENU_RECENT_DIR_2 6672
#define ID_MENU_RECENT_DIR_3 6673
#define ID_MENU_RECENT_DIR_4 6674
#define ID_MENU_RECENT_DIR_5 6675
#define ID_MENU_RECENT_DIR_6 6676
#define ID_MENU_RECENT_DIR_7 6677
#define ID_MENU_RECENT_DIR_8 6678
#define ID_MENU_RECENT_DIR_9 6679


CCookieMachineDlg::CCookieMachineDlg(CWnd* pParent /*=NULL*/) : CDialog(CCookieMachineDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCookieMachineDlg)
	//}}AFX_DATA_INIT

	//InitializeGlobalVars();

	//g_MemMappingsMgr->SetParent(this);

	//m_hIcon			= AfxGetApp()->LoadIcon( IDR_MAINFRAME );
	//m_hAccelerator	= LoadAccelerators( AfxGetResourceHandle(), MAKEINTRESOURCE( IDR_ACCELERATOR ) );
	

	//CString cmd_line = GetCommandLine();
	//int offset = 0, fTemp;

	//for( int i = 1; i < cmd_line.GetLength(); i++ )
	//{
	//	if( cmd_line[i] == '\"' )
	//	{
	//		offset	= i;
	//		fTemp	= i;
	//		break;
	//	}
	//}
	//	
	//char Buffer[ 2 * MAX_PATH ]; // Allow space for Path AND FileName here.
	//GetModuleFileName( GetModuleHandle(NULL), Buffer, sizeof(Buffer) );
	//
	//int iOff = 0;
	//m_strAppPath = Buffer;

	//iOff = m_strAppPath.ReverseFind( '\\' );
	//
	//m_strAppPath = m_strAppPath.Left( iOff );


	////#if defined(DEBUG) | defined(_DEBUG)
	////	m_strAppPath = "E:\\!FABERGE PROJECT\\!Cookie Machine\\bin";
	////#endif

	//
	//SetCurrentDirectory( m_strAppPath );
	//
	//m_hLexDll = LoadLibrary( m_strAppPath + "\\SciLexer.DLL" );

	//if( !m_hLexDll )
	//{
	//	MessageBox( "The SciLexer.DLL could not be loaded.", "Error loading SciLexer.DLL", MB_OK | MB_ICONERROR );
	//}

	//cmd_line = cmd_line.Right( cmd_line.GetLength() - fTemp );

	//if( cmd_line[cmd_line.GetLength() - 1] == '\"' )
	//	cmd_line = cmd_line.Left( cmd_line.GetLength() - 1 );

	//cmd_line = cmd_line.Trim();

	//if( cmd_line[0] == '\"' )
	//	cmd_line = cmd_line.Right( cmd_line.GetLength() - 1 );

	//cmd_line = cmd_line.Trim();

	//if( cmd_line[0] == '\"' )
	//	cmd_line = cmd_line.Right( cmd_line.GetLength() - 1 );

	//cmd_line = cmd_line.Trim();

	//m_strCmdLine = cmd_line;

	//SettingsReadFromFile();
}

CCookieMachineDlg::~CCookieMachineDlg()
{

}

LPDIRECT3DDEVICE9 CCookieMachineDlg::GetD3DDeviceGlob()
{
	return g_pd3dDevice; 
};

CBaseScene* CCookieMachineDlg::Get3DIconGlob( enum Icon3DType IconType )
{
	switch( IconType )
	{
		case ICN_3D_CAMERA:
				return pCamBox;
			break;

		case ICN_3D_LIGHT:
			return pLightBulb;
			break;

		case ICN_3D_NULL:
			return pNullIco;
			break;

		default:
			return NULL;
	}
}

bool CCookieMachineDlg::ProcessTreeDrag( HTREEITEM dragItem, HTREEITEM dropItem )
{
	//if( !pBaseScene )
	//	return false;

	//if( pBaseScene->GetType() == FT_EFN_MESHLIST )
	//{
	//	HTREEITEM parent		= m_rtSceneContents.GetParentItem( m_rtSceneContents.GetParentItem( dragItem ) );
	//	HTREEITEM parent2		= m_rtSceneContents.GetParentItem( dropItem );
	//	HTREEITEM srcMesh		= m_rtSceneContents.GetParentItem( dragItem );

	//	if( m_rtSceneContents.GetItemText( parent2 ) == "Meshes" && m_rtSceneContents.GetItemText( parent ) == "Meshes" )
	//	{
	//		((CEfnMeshlist*)pBaseScene)->MoveNull( m_rtSceneContents.GetItemText( srcMesh ).GetBuffer(),
	//																	 m_rtSceneContents.GetItemText( dragItem ).GetBuffer(),
	//																	 m_rtSceneContents.GetItemText( dropItem ).GetBuffer() );
	//		return true;
	//	}
	//}

	return false;
}

void CCookieMachineDlg::DoDataExchange(CDataExchange* pDX)
{
	str_t func_decl_str = "void CCookieMachineDlg::DoDataExchange(CDataExchange* pDX)";

	try
	{
		CDialog::DoDataExchange(pDX);

		//{{AFX_DATA_MAP(CCookieMachineDlg)
		DDX_Control(pDX, IDC_SCENE_TREE, m_rtSceneContents);
		DDX_Control(pDX, IDC_ANIM_SLIDER, m_sliderAnim);
		DDX_Control(pDX, IDC_BTN_ANIM_STOP, m_btnStopAnim);
		DDX_Control(pDX, IDC_BTN_ANIM_PLAY, m_btnPlayAnim);
		DDX_Control(pDX, IDC_CHK_LOOP_ANIM, m_btnLoopAnim);
		DDX_Control(pDX, IDC_CURR_FRM, m_eFrameCounter);
		DDX_Control(pDX, IDC_CURR_Z_OFF, m_eFrameOffset);
		DDX_Control(pDX, IDC_BUTN_SET_Z_OFF, m_btnSetOffset);
		DDX_Control(pDX, IDC_CHK_MOVE_CHAR, m_btnMoveAnim);
		DDX_Control(pDX, IDC_CMB_CAMERAS, m_cmbCameras);
		DDX_Control(pDX, IDC_CHK_LOOP_TRANSITION, m_btnTransitionAnim);
		//}}AFX_DATA_MAP
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}
}

BEGIN_MESSAGE_MAP(CCookieMachineDlg, CDialog)
//{{AFX_MSG_MAP(CCookieMachineDlg)

	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_COMMAND(ID_FILE_EXIT, OnMenuExit)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_SHOWMESH, OnMenuShowMesh)
	ON_COMMAND(ID_HIDEMESH, OnMenuHideMesh)
	ON_COMMAND(ID_MENU_SHOWMESHES, OnMenuShowAllMeshes)
	ON_COMMAND(ID_MENU_HIDEMESHES, OnMenuHideMeshes)
	ON_COMMAND(ID_MENU_BACKFACE_CULLING, OnMenuBackfaceCulling)
	ON_COMMAND(ID_T_WIREFRAME, OnTWireframe)
	ON_COMMAND(ID_T_SHADED, OnTShaded)
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_CLOSE, OnMenuFileClose)
	ON_WM_HSCROLL()
	ON_COMMAND(ID_MENU_SAVE_AS, OnMenuFileSaveAs)
	ON_BN_CLICKED(IDC_BTN_ANIM_PLAY, OnBnClickedBtnAnimPlay)
	ON_BN_CLICKED(IDC_BTN_ANIM_STOP, OnBnClickedBtnAnimStop)
	ON_BN_CLICKED(IDC_CHK_LOOP_ANIM, OnBnClickedChkLoopAnim)
	ON_COMMAND(ID_CREATE_SEQUENCE, OnCreateSequence)
	ON_COMMAND(ID_ILDAR_WINDOW, OnMenuIldarWindow)
	ON_BN_CLICKED(IDC_BUTN_SET_Z_OFF, OnBnClickedButnSetZOff)
	ON_COMMAND(ID_MENU_EDIT_MODEL_NAME, ObjectRename)
	ON_COMMAND(ID_MENU_DISCARD_VERT_COL, OnMenuDiscardVerteciesColor)
	ON_COMMAND(ID_MENU_ADD_ANIM, OnMenuEfnAnimationAdd)
	ON_COMMAND(ID_MENU_DEL_ALL_ANIM, OnMenuEfnAnimationsDeleteAll)
	ON_COMMAND(ID_MENU_DEL_ANIM_ITEM, OnMenuEfnAnimationDelete)
	ON_COMMAND(ID_MENU_EDIT_ANIM_ITEM, OnMenuEfnAnimationEdit)
	ON_BN_CLICKED(IDC_CHK_MOVE_CHAR, OnBnClickedChkMoveChar)
	ON_COMMAND(ID_MENU_RUN_ANIM_ITEM, OnMenuEfnAnimationRun)
	ON_COMMAND(ID_MENU_AA_NONE, OnMenuAANone)
	ON_COMMAND(ID_MENU_AA_X2, OnMenuAaX2)
	ON_COMMAND(ID_MENU_AA_X4, OnMenuAaX4)
	ON_COMMAND(ID_MENU_AA_X6, OnMenuAaX6)
	ON_COMMAND(ID_MENU_TEST_WALK, OnMenuTestWalk)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_MENU_CONV_COORDS_SYS, OnMenuConvertCoordsSystem)
	ON_COMMAND(ID_MENU_SCALE, OnMenuScale)
	ON_COMMAND(ID_T_WIREFRAME_SHADED, OnTWireframeShaded)
	ON_COMMAND(ID_MENU_SWAPYZ, OnMenuSwapyz)
	ON_NOTIFY(TVN_SELCHANGED, IDC_SCENE_TREE, OnTvnSelchangedSceneTree)
	ON_COMMAND(ID_MENU_MESH_INVERSE_NORMALS, OnMenuMeshInverseNormals)
	ON_COMMAND(ID_MENU_SET_CAMERA_TOCURRENT, OnMenuSetCameraTocurrent)
	ON_COMMAND(ID_MENU_RETURN_TO_PERSPECTIVEVIEW, OnMenuReturnToPerspectiveview)
	ON_COMMAND(ID_MENU_SHOW_AABB, OnMenuShowAabb)
	ON_COMMAND(ID_MENU_SHOW_BSPHERE, OnMenuShowBsphere)
	ON_WM_CTLCOLOR()
	ON_COMMAND(ID_MENU_RECALC_GLOBAL_RADIUS, OnMenuRecalcGlobalRadius)
	ON_WM_SHOWWINDOW()
	ON_WM_GETDLGCODE()
	ON_COMMAND(ID_MENU_SHOW_ALL_CAMS, OnMenuShowAllCameras)
	ON_COMMAND(ID_MENU_HIDE_ALL_CAMS, OnMenuHideAllCameras)
	ON_COMMAND(ID_MENU_SHOW_ALL_LIGHT, OnMenuShowAllLights)
	ON_COMMAND(ID_MENU_HIDE_ALL_LIGHT, OnMenuHideAllLights)
	ON_CBN_SELCHANGE( IDC_CMB_CAMERAS, OnCbnSelchangeCmbCameras )


	//accelerators
	ON_COMMAND( ID_ACCL_SWITCH_2_PERSPECTIVE_CAM, OnMenuReturnToPerspectiveview )
	ON_COMMAND( ID_ACCL_FILE_OPEN, OnMenuFileOpen )
	ON_COMMAND( ID_ACCL_SELECT_ALL_OBJECTS, SelectAllObjects )
	ON_COMMAND( ID_ACCL_DESELECT_ALL_OBJECTS, DeselectAllObjects )
	ON_COMMAND( ID_ACCL_DELETE_SELECTED_OBJECTS, SelectedObjectsDelete )
	ON_COMMAND( ID_ACCL_SAVE, OnMenuFileSave )
	ON_COMMAND( ID_ACCL_OPEN_SCRIPTS_DLG, ShowScriptsWindow )
	ON_COMMAND( ID_ACCL_WIRE, OnTWireframe )
	ON_COMMAND( ID_ACCL_FLAT, OnTShaded )
	ON_COMMAND( ID_ACCL_FLAT_AND_WIRE, OnTWireframeShaded )
	ON_COMMAND( ID_ACCL_SWITCH_CULLING, OnMenuBackfaceCulling )
	ON_COMMAND( ID_ACCL_OPEN_BINDS_DLG, ShowBindsWindow )
	ON_COMMAND( ID_ACCL_HELP, ShowHelpWindow )
	ON_COMMAND( ID_ACCL_OPTIONS, OnMenuOptions )
	ON_COMMAND( ID_ACCL_SWITCH_EDITING_MESH_VERTICES, SwitchVerteciesEditingMode )
	ON_COMMAND( ID_ACCL_ESCAPE, CancelAction )
	ON_COMMAND( ID_ACCL_SHOW_BBOX, OnMenuShowAabb )
	ON_COMMAND( ID_ACCL_SHOW_BSPH, OnMenuShowBsphere )
	ON_COMMAND( ID_ACCL_AXES_X, OnMenuAxisX )
	ON_COMMAND( ID_ACCL_AXES_Y, OnMenuAxisY )
	ON_COMMAND( ID_ACCL_AXES_Z, OnMenuAxisZ )
	ON_COMMAND( ID_ACCL_AXES_XY, OnMenuAxisXY )
	ON_COMMAND( ID_ACCL_AXES_XZ, OnMenuAxisXZ )
	ON_COMMAND( ID_ACCL_AXES_YZ, OnMenuAxisYZ )
	ON_COMMAND( ID_ACCL_HIDE_SELECTED, OnMenuObjectHide )
	ON_COMMAND( ID_ACCL_UNHIDE_ALL, OnMenuObjectShowAll )
	ON_COMMAND( ID_ACCL_SHOW_ONLY, OnMenuObjectShowOnly )
	ON_COMMAND( ID_ACCL_MATERIAL, OnMenuObjectMaterial )
	ON_COMMAND( ID_ACCL_SHOW_OBJ_PROP_WND, OnMenuShowObjPropertiesWnd )
	ON_COMMAND( ID_ACCL_SHOW_GRID, OnMenuShowGrid )
	ON_COMMAND( ID_ACCL_RUN_ENGINE, RunGame )
	ON_COMMAND( ID_ACCL_RUN_ENGINE_CURR, RunCurrentGame )
	ON_COMMAND( ID_ACCL_OPEN_TEXT_RES_DLG, ShowTextResourcesWindowWrp )
	ON_COMMAND( ID_ACCL_CURVE_NULL_UP, OnMenuScnCurveNullMoveUP )
	ON_COMMAND( ID_ACCL_CURVE_NULL_DOWN, OnMenuScnCurveNullMoveDOWN )
	ON_COMMAND( ID_ACCL_ENGINE_MESSAGING, OnMenuEngineMessaging )
	ON_COMMAND( ID_ACCL_STOP_ENGINE, StopGame )

	ON_COMMAND( IDT_ENGINE_CONSOLE, OnMenuEngineMessaging )

	ON_COMMAND( ID_ACCL_RENAME, ObjectRename )
	ON_COMMAND( ID_ACCL_CENTER_CAM_ON_OBJ, LockCamOnObject )
	ON_COMMAND( ID_ACCL_CENTER_CAM_ON_OBJ_ALL_VIEWS, LockCamOnObjectAllViews )


	ON_COMMAND( ID_ACCL_PERSPECTIVE, SetViewPerspective )
	ON_COMMAND( ID_ACCL_FRONT, SetViewFront )
	ON_COMMAND( ID_ACCL_TOP, SetViewTop )
	ON_COMMAND( ID_ACCL_RIGHT, SetViewRight )
	ON_COMMAND( ID_ACCL_TOGGLE_EXPLORER, ToggleSceneExplorer )


	//recent files
	ON_COMMAND( ID_MENU_RECENT_FILE_1, OnMenuRecentItem1 )
	ON_COMMAND( ID_MENU_RECENT_FILE_2, OnMenuRecentItem2 )
	ON_COMMAND( ID_MENU_RECENT_FILE_3, OnMenuRecentItem3 )
	ON_COMMAND( ID_MENU_RECENT_FILE_4, OnMenuRecentItem4 )
	ON_COMMAND( ID_MENU_RECENT_FILE_5, OnMenuRecentItem5 )
	ON_COMMAND( ID_MENU_RECENT_FILE_6, OnMenuRecentItem6 )
	ON_COMMAND( ID_MENU_RECENT_FILE_7, OnMenuRecentItem7 )
	ON_COMMAND( ID_MENU_RECENT_FILE_8, OnMenuRecentItem8 )
	ON_COMMAND( ID_MENU_RECENT_FILE_9, OnMenuRecentItem9 )


	//recent dirs
	ON_COMMAND( ID_MENU_RECENT_DIR_1, OnMenuRecentDir1 )
	ON_COMMAND( ID_MENU_RECENT_DIR_2, OnMenuRecentDir2 )
	ON_COMMAND( ID_MENU_RECENT_DIR_3, OnMenuRecentDir3 )
	ON_COMMAND( ID_MENU_RECENT_DIR_4, OnMenuRecentDir4 )
	ON_COMMAND( ID_MENU_RECENT_DIR_5, OnMenuRecentDir5 )
	ON_COMMAND( ID_MENU_RECENT_DIR_6, OnMenuRecentDir6 )
	ON_COMMAND( ID_MENU_RECENT_DIR_7, OnMenuRecentDir7 )
	ON_COMMAND( ID_MENU_RECENT_DIR_8, OnMenuRecentDir8 )
	ON_COMMAND( ID_MENU_RECENT_DIR_9, OnMenuRecentDir9 )

	
	ON_COMMAND( IDT_FILEOPEN, OnMenuFileOpen )
	ON_COMMAND( ID_MENU_FILE_OPEN, OnMenuFileOpen )

	ON_COMMAND( IDT_FILESAVE, OnMenuFileSave )
	ON_COMMAND( ID_MENU_SAVE, OnMenuFileSave )

	ON_COMMAND( IDT_FILESAVEALL, OnBnClickedBtnFilesaveAll )


	ON_COMMAND( IDT_FILECONV_2EFN, OnMenuConvToEfn )
	ON_COMMAND( ID_MENU_CONV_TO_EFN_ANIM, OnMenuConvToEfn )

	ON_COMMAND( IDT_FILECONV_2MSH, OnMenuConvToEfnMeshlist )
	ON_COMMAND( ID_MENU_CONV_TO_EFN_MESHLIST, OnMenuConvToEfnMeshlist )

	ON_COMMAND( IDT_FILECONV_2SCN, OnMenuConvToScn )
	ON_COMMAND( ID_MENU_CONV_TO_SCN, OnMenuConvToScn )
	
	ON_COMMAND( IDT_BINDS, ShowBindsWindow )
	ON_COMMAND( IDT_SCRIPTS, ShowScriptsWindow )

	ON_COMMAND(ID_MENU_ABOUT, OnMenuAbout)
	ON_COMMAND(ID_MENU_OPTIONS, OnMenuOptions)
	
	ON_COMMAND(ID_MENU_OPTIONS, OnMenuOptions)

	ON_COMMAND( IDT_SHOWALL, OnBnClickedBtnShowall )
	ON_COMMAND( IDT_HIDEALL, OnBnClickedBtnHideall )
	ON_COMMAND( IDT_AXIS_ALIGN, ShowAxisAlignMenu )
	ON_COMMAND( IDT_FLOORS, ShowFloorsMenu )
	ON_COMMAND( IDT_RUN_ENGINE, RunGame )
	ON_COMMAND( IDT_RUN_ENGINE_CURR, RunCurrentGame )
	ON_COMMAND( IDT_OBJ_PROPERTIES, OnMenuShowObjPropertiesWnd )
	ON_COMMAND( IDT_SETTINGS, OnMenuOptions )
	ON_COMMAND( IDT_HELP, ShowHelpWindow )
	ON_COMMAND( IDT_ENGINE_LOGS, ShowLogsWindow )
	ON_COMMAND( IDT_TEXT_RES, ShowTextResourcesWindowWrp )

	ON_COMMAND( IDT_RELOAD_SCENE, ScnReloadScene )
	ON_COMMAND( IDT_RELOAD_MESHES, ScnReloadMeshes )
	ON_COMMAND( IDT_STOP_ENGINE, StopGame )

	ON_COMMAND(ID_MENU_MESHLIST_MESHES_SHOW, OnMenuMeshlistMeshesShow)
	ON_COMMAND(ID_MENU_MESHLIST_MESHES_HIDE, OnMenuMeshlistMeshesHide)
	ON_COMMAND(ID_MENU_MESHLIST_MESH_SHOW, OnMenuMeshlistMeshShow)
	ON_COMMAND(ID_MENU_MESHLIST_MESH_HIDE, OnMenuObjectHide)
	ON_COMMAND(ID_MENU_MESHLIST_MESH_RENAME, ObjectRename)
	ON_COMMAND(ID_MENU_MESHLIST_MESH_DELETE, OnMenuMeshlistMeshDelete)
	ON_COMMAND(ID_MENU_FPS_CHANGE, OnMenuFpsChange)
	ON_COMMAND(ID_MENU_LOAD_TEXTURE, OnMenuEfnLoadTexture)
	ON_COMMAND(ID_MENU_MESHLIST_SCENETAG, OnMenuMeshlistScenetag)
    ON_COMMAND(ID_MENU_SCN_TEXTURE_ADD, OnMenuScnTextureAdd)
    ON_COMMAND(ID_MENU_TEXTURE_REMOVE_ALL, OnMenuScnTexturesDeleteAll)
	ON_COMMAND(ID_MENU_FLOOR_RECALC, OnMenuScnFloorRecalc)
	ON_COMMAND(ID_MENU_FLOOR_SHOWONLY, OnMenuScnFloorShowOnly)
	ON_COMMAND(ID_MENU_FLOOR_SHOW_GRID, OnMenuScnFloorShowGrid)
	ON_COMMAND(ID_MENU_FLOOR_SHOW_SELECTOR, OnMenuScnFloorShowSelector)
	ON_COMMAND(ID_MENU_FLOOR_SELECTOR_SHOWONLY, OnMenuScnFloorSelectorShowOnly)
	ON_COMMAND(ID_MENU_SCENE_CAMS_OBJECT_DELETE, OnMenuSceneCamsObjectDelete)
	ON_COMMAND(ID_MENU_SCENE_CAMS_OBJECT_SCRIPT, OnMenuSceneCamsObjectScript)
	ON_COMMAND(ID_MENU_SCENE_CAMS_OBJECT_BIND, OnMenuSceneCamsObjectBind)
	ON_COMMAND(ID_MENU_SCN_MESHES_SHOW, OnMenuScnMeshesShow)
	ON_COMMAND(ID_MENU_SCN_MESHES_HIDE, OnMenuScnMeshesHide)
	ON_COMMAND(ID_MENU_SCN_MESH_SHOW, OnMenuScnMeshShow)
	ON_COMMAND(ID_MENU_SCN_MESH_HIDE, OnMenuScnMeshHide)
	ON_COMMAND(ID_MENU_SCN_CAMERAS_SHOW, OnMenuScnCamerasShow)
	ON_COMMAND(ID_MENU_SCN_CAMERAS_HIDE, OnMenuScnCamerasHide)
	ON_COMMAND(ID_MENU_SCN_CAMERA_SHOW, OnMenuScnCameraShow)
	ON_COMMAND(ID_MENU_SCN_CAMERA_HIDE, OnMenuScnCameraHide)
	ON_COMMAND(ID_MENU_SCN_CAMERA_OBJECTS, OnMenuScnCameraObjects)
	ON_COMMAND(ID_MENU_SCN_LIGHTS_SHOW, OnMenuScnLightsShow)
	ON_COMMAND(ID_MENU_SCN_LIGHTS_HIDE, OnMenuScnLightsHide)
	ON_COMMAND(ID_MENU_SCN_LIGHT_SHOW, OnMenuScnLightShow)
	ON_COMMAND(ID_MENU_SCN_LIGHT_HIDE, OnMenuScnLightHide)
	ON_COMMAND(ID_MENU_SCN_CAM_ZOOMNEAR, OnMenuScnCamZoomnear)
	ON_COMMAND(ID_MENU_SCN_CAM_ZOOMFAR, OnMenuScnCamZoomfar)
	ON_COMMAND(ID_MENU_SCN_LIGHT_DIFFUSE, OnMenuScnLightDiffuse)
	ON_COMMAND(ID_MENU_SCN_LIGHT_AMB, OnMenuScnLightAmb)
	ON_COMMAND(ID_MENU_SCN_MESH_VCOL, OnMenuScnMeshVcol)
	ON_COMMAND(ID_MENU_SCN_LIGHT_MULTIPLIER, OnMenuScnLightMultiplier)
	ON_COMMAND(ID_MENU_SCN_LIGHT_ATTENUATION, OnMenuScnLightAttenuation)
	ON_COMMAND(ID_MENU_SCN_LIGHT_RANGE, OnMenuScnLightRange)
	ON_COMMAND(ID_MENU_SCN_CUST_OBJ_ADDNEW, OnMenuScnCustObjAddnew)
	ON_COMMAND(ID_MENU_SCN_CUST_OBJ_DELALL, OnMenuScnCustObjDelall)
	ON_COMMAND(ID_MENU_SCN_CUST_TYPE_ADDNEW, OnMenuScnCustTypeAddnew)
	ON_COMMAND(ID_MENU_SCN_CUST_TYPE_DELALL, OnMenuScnCustTypeDelall)
	ON_COMMAND(ID_MENU_SCN_CUST_TYPE_DEL, OnMenuScnCustTypeDel)
	ON_COMMAND(ID_MENU_SCN_CUST_OBJ_DEL, OnMenuScnCustObjDel)
	ON_COMMAND(ID_MENU_SCN_BINDS_SHOW_WND, OnMenuScnBindsShowWnd)
	ON_COMMAND(ID_MENU_SCN_SCRIPTS_SHOW_WND, OnMenuScnScriptsShowWnd)
	ON_COMMAND(ID_MENU_FLOOR_UV_RECALC, OnMenuScnFloorUVRecalc)
	ON_COMMAND(ID_MENU_SCN_BACK_ADD, OnMenuScnTextureAddBackground)
	ON_COMMAND(ID_MENU_SCN_TEXTURE_DEL, OnMenuScnTextureDel)
	ON_COMMAND(ID_MENU_IMPORT, OnMenuImport)
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_EFN_EDIT_MATERIAL, OnMenuEfnEditMaterial)
	ON_COMMAND(ID_MENU_EFN_NULL_ATTACH, OnMenuEfnNullAttachTo)
	ON_COMMAND(ID_MENU_EFN_NULL_DETACH_ALL, OnMenuEfnNullDetachAll)
	ON_COMMAND(ID_EFN_NULLS_DETACH_ALL, OnMenuEfnNullsDetachAll)
	ON_COMMAND(ID_MENU_EFN_NULL_SCALING, OnMenuEfnNullScaling)
	ON_COMMAND(ID_EFN_MESHLIST_NULL_RENAME, ObjectRename)
	ON_COMMAND(ID_EFN_MESHLIST_NULL_REMOVE, OnMenuMeshlistNullRemove)
	ON_COMMAND(ID_EFN_MESHLIST_NULL_ATTACH, OnMenuMeshlistNullAttach)
	ON_COMMAND(ID_EFN_MESHLIST_NULL_DETACH, OnMenuMeshlistNullDetach)
	ON_COMMAND(ID_EFN_MESHLIST_NULLS_NEW, OnMenuMeshlistNullsNew)
	ON_COMMAND(ID_EFN_MESHLIST_NULLS_REMOVE_ALL, OnMenuMeshlistNullsRemoveAll)
	ON_COMMAND(ID_EFN_MESHLIST_NULLS_DETACH_ALL, OnMenuMeshlistNullsDetachAll)
	ON_COMMAND(ID_MENU_MESHLIST_MESH_EDIT_VERTICES, OnMenuMeshlistMeshEditVertices)
	ON_COMMAND(ID_MENU_MESHLIST_MESH_SHOW_ONLY, OnMenuObjectShowOnly)
	ON_COMMAND(ID_MESHLIST_NULL_SHOW, OnMenuMeshlistNullShow)
	ON_COMMAND(ID_MESHLIST_NULL_HIDE, OnMenuMeshlistNullHide)
	ON_COMMAND(ID_MENU_MESHLIST_MESH_MATERIAL, OnMenuMeshlistMeshMaterial)
	ON_COMMAND(ID_MENU_SCN_MESH_SHOWONLY, OnMenuScnMeshShowOnly)
	ON_COMMAND(ID_MENU_SCN_CAMERA_SHOW_ONLY, OnMenuScnCameraShowOnly)
	ON_COMMAND(ID_MENU_SCN_LIGHT_SHOW_ONLY, OnMenuScnLightShowOnly)
	ON_COMMAND(ID_MENU_SCN_MESH_EDIT_MATERIAL, OnMenuScnMeshEditMaterial)
	ON_COMMAND(ID_MENU_SCN_MESH_EDIT_VERTECIES, OnMenuScnMeshEditVertecies)
	ON_COMMAND(ID_MENU_SCN_NULLS_SHOW, OnMenuScnNullsShow)
	ON_COMMAND(ID_MENU_SCN_NULLS_HIDE, OnMenuScnNullsHide)
	ON_COMMAND(ID_MENU_SCN_NULL_ATTACH, OnMenuScnNullAttach)
	ON_COMMAND(ID_MENU_SCN_NULL_DETACH, OnMenuScnNullDetach)
	ON_COMMAND(ID_MENU_SCN_NULLS_CREATE_NEW, OnMenuScnNullsCreateNew)
	ON_COMMAND(ID_MENU_SCN_NULLS_DETACH_ALL, OnMenuScnNullsDetachAll)
	ON_COMMAND(ID_MENU_EFN_NULL_RENAME, ObjectRename)
	ON_COMMAND(ID_MENU_FILE_ASSOCIATIONS, OnMenuFileAssociations)
	ON_COMMAND(ID_MENU_MESHLIST_MESH_DUPLICATE, OnMenuMeshlistMeshDuplicate)
	ON_COMMAND(ID_MENU_MESHLIST_MESHES_CREATE_CUBE, OnMenuMeshlistMeshesCreateCube)
	ON_COMMAND(ID_MENU_MESHLIST_MESHES_CREATE_POLY, OnMenuMeshlistMeshesCreatePoly)
	ON_COMMAND(ID_MENU_MESHLIST_MESHES_CREATE_QUAD, OnMenuMeshlistMeshesCreateQuad)
	ON_COMMAND(ID_MENU_SCN_NULL_RENAME, ObjectRename)
	ON_COMMAND(ID_MENU_SCN_NULL_DELETE, OnMenuScnNullDelete)
	ON_COMMAND(ID_MENU_SCN_MESH_DUPLICATE, OnMenuScnMeshDuplicate)
	ON_COMMAND(ID_MENU_SCN_MESH_RENAME, ObjectRename)
	ON_COMMAND(ID_MENU_SCN_MESH_DELETE, OnMenuScnMeshDelete)
	ON_COMMAND(ID_MENU_SCN_MESHES_CREATE_BOX, OnMenuScnMeshesCreateBox)
	ON_COMMAND(ID_MENU_SCN_MESHES_CREATE_TRIANGLE, OnMenuScnMeshesCreateTriangle)
	ON_COMMAND(ID_MENU_SCN_MESHES_CREATE_QUAD, OnMenuScnMeshesCreateQuad)
	ON_COMMAND(ID_MENU_SCN_TEXTURE_BIND_2_CAM, OnMenuScnTextureBind2Cam)
	ON_COMMAND(ID_MENU_SCN_MESH_BIND_2_CAMERA, OnMenuScnMeshBind2Camera)
	ON_COMMAND(ID_MENU_SCN_MESH_BIND_2_TEXTURE, OnMenuScnMeshBind2Texture)
	ON_COMMAND(ID_MENU_SCN_LIGHTS_BIND_2_CAMERA, OnMenuScnLightBind2Camera)
	ON_COMMAND(ID_MENU_START_MACRO_REC, OnMenuMacroRecStart)
	ON_COMMAND(ID_MENU_END_MACRO_REC, OnMenuMacroRecEnd)
	ON_COMMAND(ID_MENU_LOAD_MACRO, OnMenuMacroLoadFromFile)
	ON_COMMAND(ID_MENU_SCN_CAMERA_SCRIPT, OnMenuScnCameraScript)
	ON_COMMAND(ID_MENU_SCN_LIGHT_SCRIPT, OnMenuScnLightScript)
	ON_COMMAND(ID_MENU_SCN_TEXTURE_SCRIPT, OnMenuScnTextureScript)
	ON_COMMAND(ID_MENU_SCALE_NEW, OnMenuEfnScaleNew)
	ON_COMMAND(ID_EFN_NULLS_RESCALE_ALL, OnMenuEfnNullsRescaleAll)
	ON_COMMAND(ID_MENU_OBJ_PROPERTIES_WND, OnMenuShowObjPropertiesWnd)
	ON_COMMAND(ID_MENU_AXIS_X, OnMenuAxisX)
	ON_COMMAND(ID_MENU_AXIS_Y, OnMenuAxisY)
	ON_COMMAND(ID_MENU_AXIS_Z, OnMenuAxisZ)
	ON_COMMAND(ID_MENU_AXIS_XY, OnMenuAxisXY)
	ON_COMMAND(ID_MENU_AXIS_XZ, OnMenuAxisXZ)
	ON_COMMAND(ID_MENU_XSI_MESH_DISCONNECT_TRIS, OnMenuXSIMeshDisconnectAllTris)
	ON_COMMAND(ID_MENU_XSI_MESH_CONNECT_TRIS, OnMenuXSIMeshConnectAllTris)
	ON_COMMAND(ID_MENU_EFN_MESH_DISCONNECTPOLYS, OnMenuEfnMeshDisconnectAllTris)
	ON_COMMAND(ID_MENU_EFN_MESH_CONNECTPOLYS, OnMenuEfnMeshConnectAllTris)
	ON_CBN_SELCHANGE(IDC_CMB_VIEWS, OnCbnSelchangeCmbViews)
	ON_COMMAND(ID_MENU_FLOOR_MATERIAL, OnMenuScnFloorMaterial)
	ON_COMMAND(ID_MENU_SCN_MESH_INVERSE_NORMALS, OnMenuScnMeshInverseNormals)
	ON_COMMAND(ID_MENU_SCN_MESH_CONNECT_TRIS, OnMenuScnMeshConnectTris)
	ON_COMMAND(ID_MENU_SCN_MESH_DISCONNECT_TRIS, OnMenuScnMeshDisconnectTris)
	ON_COMMAND(ID_MENU_MESHLIST_MESH_CONNECT_TRIS, OnMenuMeshlistMeshConnectTris)
	ON_COMMAND(ID_MENU_MESHLIST_MESH_DISCONNECT_TRIS, OnMenuMeshlistMeshDisconnectTris)
	ON_COMMAND(ID_MENU_MESHLIST_MESH_INVERSE_NORMALS, OnMenuMeshlistMeshInverseNormals)
	ON_COMMAND(ID_EFN_MESHLIST_NULLS_SHOW_ALL, OnMenuMeshlistNullsShow)
	ON_COMMAND(ID_EFN_MESHLIST_NULLS_HIDE_ALL, OnMenuMeshlistNullsHide)
	ON_COMMAND(ID_MENU_MESHLIST_MESHES_DELETE_ALL, OnMenuMeshlistMeshesDeleteAll)
	ON_COMMAND(ID_MENU_SEL_DELETE_OBJECTS, SelectedObjectsDelete)
	ON_COMMAND(ID_MENU_SEL_GEOM_MERGE, SelectedObjectsMerge)
	ON_NOTIFY(NM_CLICK, IDC_SCENE_TREE, OnNMClickSceneTree)
	ON_COMMAND(ID_MENU_SEL_GEOM_MATERIAL, SelectedObjectsMaterial)
	ON_COMMAND(ID_MENU_SEL_GEOM_CONNECT_TRIS, SelectedObjectsConnectTris)
	ON_COMMAND(ID_MENU_SEL_GEOM_DISCONNECT_TRIS, SelectedObjectsDisconnectTris)
	ON_COMMAND(ID_MENU_SEL_GEOM_FLIP_NORMALS, SelectedObjectsFlipNormals)
	ON_COMMAND(ID_MENU_SCN_CAMERA_OBJ_SCRIPT, OnMenuScnCameraMiscObjectScript)
	ON_COMMAND(ID_MENU_SCN_CAMERA_OBJ_BIND, OnMenuScnCameraMiscObjectBind)
	ON_COMMAND(ID_MENU_SCN_CURVES_NEW, OnMenuScnCurvesCreateNew)
	ON_COMMAND(ID_MENU_SCN_CURVE_ADDSELECTEDNULL, OnMenuScnCurveAddselectednull)
	ON_COMMAND(ID_MENU_SCN_CURVES_SHOW_ALL, OnMenuScnCurvesShow)
	ON_COMMAND(ID_MENU_SCN_CURVES_HIDE_ALL, OnMenuScnCurvesHide)
	ON_COMMAND(ID_MENU_SCN_CURVES_DELETE_ALL, OnMenuScnCurvesDeleteAll)
	ON_COMMAND(ID_MENU_SCN_CURVE_REMOVE_ALL_NULLS, OnMenuScnCurveRemoveAllNulls)
	ON_COMMAND(ID_MENU_SCN_CURVE_DELETE, OnMenuScnCurveDelete)
	ON_COMMAND(ID_MENU_SCN_CURVE_RENAME, ObjectRename)
	ON_COMMAND(ID_MENU_SCN_CURVE_TYPE_LINEAR, OnMenuScnCurveSetTypeLinear)
	ON_COMMAND(ID_MENU_SCN_CURVE_TYPE_CATMUL, OnMenuScnCurveSetTypeCatmul)
	ON_COMMAND(ID_MENU_SCN_CURVE_NULL_REMOVE, OnMenuScnCurveNullRemove)
	ON_COMMAND(ID_MENU_SCN_CURVE_NULL_MOVEUP, OnMenuScnCurveNullMoveUP)
	ON_COMMAND(ID_MENU_SCN_CURVE_NULL_MOVEDOWN, OnMenuScnCurveNullMoveDOWN)
	ON_COMMAND(ID_MENU_ENGINE_MESSAGING, OnMenuEngineMessaging)
	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, ToolbarTooltipProc )
	ON_WM_MBUTTONDOWN()
	ON_COMMAND(ID_MENU_SEL_INVERSE, SelectedObjectsInverseSelection)
	ON_COMMAND(ID_MENU_SEL_HIDE, SelectedObjectsHide)
	ON_COMMAND(ID_MENU_SELECT_VISIBLE, OnMenuSelectVisibleObjects)
	ON_COMMAND(ID_MENU_RENAME, OnMenuRename)
	ON_COMMAND(ID_MENU_DUPLICATE, OnMenuDuplicate)
	ON_COMMAND(ID_EFN_NULLS_RELOAD_ATTACHES, OnMenuEfnNullsReloadAttaches)
	ON_STN_CLICKED(IDC_BMP, OnStnClickedBmp)
	ON_COMMAND(ID_MENU_SCN_LIGHTS_DELETE, OnMenuScnLightsDeleteAll)
	ON_COMMAND(ID_MENU_SCN_CAMERAS_DELETE, OnMenuScnCamerasDeleteAll)
	ON_COMMAND(ID_MENU_SCN_MESHES_DELETE, OnMenuScnMeshesDeleteAll)
	ON_COMMAND(ID_MENU_SCN_NULLS_DELETE, OnMenuScnNullsDeleteAll)
	ON_COMMAND(ID_MENU_SCN_TEXTURE_RENAME, ObjectRename)
	ON_COMMAND(ID_MENU_SCN_CUST_OBJ_RENAME, ObjectRename)
	ON_COMMAND(ID_MENU_SCN_CUST_TYPE_RENAME, ObjectRename)
	ON_COMMAND(ID_MENU_SCN_LIGHT_RENAME, ObjectRename)
	ON_COMMAND(ID_MENU_SCN_CAMERA_RENAME, ObjectRename)
	ON_COMMAND(ID_MENU_SCN_LIGHT_DELETE, OnMenuScnLightDelete)
	ON_COMMAND(ID_MENU_SCN_CAMERA_DELETE, OnMenuScnCameraDelete)
	ON_COMMAND(ID_MENU_VIEW_SPOTLIGHTCONE, OnMenuViewSpotlightcone)
	ON_COMMAND(ID_MENU_SCN_RESTORE_ENGINE_CAMS, OnMenuScnRestoreEngineCams)
	ON_WM_MBUTTONUP()
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_MENU_VIEW_CAMERA_INTEREST, OnMenuViewCameraInterest)
	ON_COMMAND(ID_MENU_CREATE_SCENE, OnMenuCreateScene)
	ON_COMMAND(ID_MENU_HIDEBYTYPE_CAMERAS, OnMenuHidebytypeCameras)
	ON_COMMAND(ID_MENU_SHOWBYTYPE_CAMERAS, OnMenuShowbytypeCameras)
	ON_COMMAND(ID_MENU_HIDEBYTYPE_LIGHTS, OnMenuHidebytypeLights)
	ON_COMMAND(ID_MENU_SHOWBYTYPE_LIGHTS, OnMenuShowbytypeLights)
	ON_COMMAND(ID_MENU_HIDEBYTYPE_MESHES, OnMenuHidebytypeMeshes)
	ON_COMMAND(ID_MENU_SHOWBYTYPE_MESHES, OnMenuShowbytypeMeshes)
	ON_COMMAND(ID_MENU_SHOWBYTYPE_NULLS, OnMenuShowbytypeNulls)
	ON_COMMAND(ID_MENU_HIDEBYTYPE_NULLS, OnMenuHidebytypeNulls)
	ON_COMMAND(ID_MENU_HIDEBYTYPE_CURVES, OnMenuHidebytypeCurves)
	ON_COMMAND(ID_MENU_SHOWBYTYPE_CURVES, OnMenuShowbytypeCurves)
	ON_COMMAND(ID_MENU_HIDEBYTYPE_ALL, OnMenuHidebytypeAll)
	ON_COMMAND(ID_MENU_SHOWBYTYPE_ALL, OnMenuShowbytypeAll)
	ON_COMMAND(ID_MENU_HIDEBYTYPE_FLOORS, OnMenuHidebytypeFloors)
	ON_COMMAND(ID_MENU_SHOWBYTYPE_FLOORS, OnMenuShowbytypeFloors)
	ON_COMMAND(ID_MENU_HIDEBYTYPE_LAYERGEOM, OnMenuHidebytypeLayergeom)
	ON_COMMAND(ID_MENU_SHOWBYTYPE_LAYERGEOM, OnMenuShowbytypeLayergeom)
	ON_COMMAND(ID_MENU_SHOWBYTYPE_CAMERASELECTORS, OnMenuShowbytypeCameraselectors)
	ON_COMMAND(ID_MENU_HIDEBYTYPE_CAMERASELECTORS, OnMenuHidebytypeCameraselectors)
	ON_COMMAND(ID_MENU_HIDEBYTYPE_ACTIONOBJECTS, OnMenuHidebytypeActionobjects)
	ON_COMMAND(ID_MENU_SHOWBYTYPE_ACTIONOBJECTS, OnMenuShowbytypeActionobjects)
	ON_COMMAND(ID_MENU_BATCHCONVERTATION_XSI2ANIMEFN, OnMenuBatchConvertXSI2EFNAnim)
	ON_COMMAND(ID_MENU_CREATEOBJECT_NULL, OnMenuCreateobjectNull)
	ON_COMMAND(ID_MENU_CREATEOBJECT_BOX, OnMenuCreateobjectBox)
	ON_COMMAND(ID_MENU_CREATEOBJECT_QUAD, OnMenuCreateobjectQuad)
	ON_COMMAND(ID_MENU_CREATEOBJECT_TRIANGLE, OnMenuCreateobjectTriangle)
	ON_COMMAND(ID_MENU_SCN_CUST_OBJ_BIND_2_CAMERA, OnMenuScnCustObjBind2Camera)
	ON_COMMAND(ID_MENU_SEL_GEOM_ALIGN_POINTS, OnMenuSelGeomAlignPoints)
	ON_COMMAND(ID_MENU_UNLOAD_TEXTURE, OnMenuEfnUnloadTexture)
	ON_COMMAND(ID_MENU_CREATEOBJECT_NULL_ON_CURVE, OnMenuCreateobjectNullOnCurve)
	ON_COMMAND(ID_MENU_EFN_USE_UV1, OnMenuEfnUseUV1)
	ON_COMMAND(ID_MENU_EFN_USE_UV2, OnMenuEfnUseUV2)
	ON_COMMAND(ID_MENU_HIDEBYTYPE_OCCLUDERS, OnMenuHidebytypeOccluders)
	ON_COMMAND(ID_MENU_SHOWBYTYPE_OCCLUDERS, OnMenuShowbytypeOccluders)
	ON_COMMAND(ID_MENU_EFN_NULL_DELETE, OnMenuEfnNullDelete)
	ON_COMMAND(ID_MENU_EFN_NULLS_CREATE_NEW, OnMenuEfnNullsCreateNew)
	ON_NOTIFY(NM_DBLCLK, IDC_SCENE_TREE, OnNMDblclkSceneTree)
	ON_COMMAND(ID_MENU_SCN_CAMERA_OBJ_EDIT_POS, OnMenuScnCameraMiscObjectEditPos)
	ON_COMMAND(ID_MENU_SCN_FINISH_OBJECT_POS_EDITING, OnMenuScnFinishObjectPosEditing)
	ON_COMMAND(ID_MENU_SCN_EDIT_DUDE_POS, OnMenuScnEditDudePos)
	ON_COMMAND(ID_MENU_CREATE_MESHLIST, OnMenuCreateMeshlist)
	ON_COMMAND(ID_MENU_SEL_GEOM_BIND2CAM, OnMenuSelGeomBind)
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_MENU_VIEW_NORMALS, OnMenuViewNormals)
	ON_COMMAND(ID_TEMP_ADDLOD, OnTempAddlod)
	ON_COMMAND(ID_MENU_BATCHCONVERTATION_XSI2MESHLISTEFN, OnMenuBatchConvertXSI2EFNMeshlist)
	ON_COMMAND(ID_MENU_BATCHCONVERTATION_ERRORCORRECTION, OnMenuBatchConvertErrorCorrection)
	ON_COMMAND(ID_MENU_SEL_GEOM_MERGE_POINTS, OnMenuSelGeomMergePoints)
	ON_COMMAND(ID_MENU_SCN_CAMERA_OBJ_DELETE, OnMenuScnCameraMiscObjectDelete)
	ON_COMMAND(ID_MENU_CROP_TOOL, OnMenuEfnCropTool)
	ON_COMMAND(ID_MENU_ANIM_DEGRADE_TOOL, OnMenuEfnAnimDegradeTool)
	ON_COMMAND(ID_MENU_HACK_TRANSLATE_SCENE, OnMenuHackTranslateScene)
	ON_COMMAND(ID_MENU_SCN_LIGHT_DUPLICATE, OnMenuScnLightDuplicate)
	ON_COMMAND(ID_MENU_SCN_LIGHTS_CREATE_OMNI, OnMenuScnLightsCreateOmni)
	ON_COMMAND(ID_MENU_SCN_LIGHTS_CREATE_DIRECTIONAL, OnMenuScnLightsCreateDirectional)
	ON_COMMAND(ID_MENU_EFN_NULLS_DELETE_ALL, OnMenuEfnNullsDeleteAll)
	ON_COMMAND(ID_MENU_EFN_MESH_DELETE, OnMenuEfnMeshDelete)
	ON_COMMAND(ID_MENU_ROTATE_Y_180, OnMenuRotateModelY_by_180deg)
	ON_COMMAND(ID_MENU_ROTATE_X_90, OnMenuRotateModelX90)
	ON_COMMAND(ID_MENU_ROTATE_Y_90, OnMenuRotateModelY90)
	ON_COMMAND(ID_MENU_ROTATE_Z_90, OnMenuRotateModelZ90)
	ON_COMMAND(ID_MENU_SCN_MESHES_CREATE_BOX_TESS, OnMenuScnMeshesCreateBoxTess)
	ON_COMMAND(ID_MENU_SCN_MESHES_CREATE_CYLINDER_TESS, OnMenuScnMeshesCreateCylinderTess)
	ON_COMMAND(ID_MENU_HIDEBYTYPE_DYNFLOORS, OnMenuHidebytypeDynFloors)
	ON_COMMAND(ID_MENU_HIDEBYTYPE_SHADOW_RECEIVERS, OnMenuHidebytypeShadowReceivers)
	ON_COMMAND(ID_MENU_SHOWBYTYPE_DYNFLOORS, OnMenuShowbytypeDynFloors)
	ON_COMMAND(ID_MENU_SHOWBYTYPE_SHADOW_RECEIVERS, OnMenuShowbytypeShadowReceivers)
	ON_COMMAND(ID_MENU_SCALE_Z_OFFSETS, OnMenuScaleZOffsets)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CCookieMachineDlg::OnMenuShowGrid()
{
	m_appSettings.GridVisible = !m_appSettings.GridVisible;
}

void CCookieMachineDlg::SetViewPerspective()
{
	CComboBox* pViewsBox = (CComboBox*)GetDlgItem( IDC_CMB_VIEWS );

	if( !pViewsBox )
		return;

	pViewsBox->SetCurSel( 0 );

	m_ViewMgr.ChangeRenderView( RV_PERSPECTIVE );
}

void CCookieMachineDlg::ScnReloadScene()
{
	OnMenuFileSave();
	g_MemMappingsMgr->PostScriptToEngine( "Quest_ReloadScene();", true );
}

void CCookieMachineDlg::ScnReloadMeshes()
{
	OnMenuFileSave();
	g_MemMappingsMgr->PostScriptToEngine( "FreeEfnAll();", true );
}

void CCookieMachineDlg::SetViewFront()
{
	CComboBox* pViewsBox = (CComboBox*)GetDlgItem( IDC_CMB_VIEWS );

	if( !pViewsBox )
		return;

	pViewsBox->SetCurSel( 1 );

	m_ViewMgr.ChangeRenderView( RV_FRONT );
}

void CCookieMachineDlg::SetViewTop()
{
	CComboBox* pViewsBox = (CComboBox*)GetDlgItem( IDC_CMB_VIEWS );

	if( !pViewsBox )
		return;

	pViewsBox->SetCurSel( 2 );

	m_ViewMgr.ChangeRenderView( RV_TOP );
}

void CCookieMachineDlg::SetViewRight()
{
	CComboBox* pViewsBox = (CComboBox*)GetDlgItem( IDC_CMB_VIEWS );

	if( !pViewsBox )
		return;

	pViewsBox->SetCurSel( 3 );

	m_ViewMgr.ChangeRenderView( RV_RIGHT );
}

void CCookieMachineDlg::ShowHelpWindow()
{
	CHelpDlg dlg;
	dlg.DoModal();
}

void CCookieMachineDlg::OpenRecentFile( int iRecentFileIndex )
{
	//get path, filename and extension
	str_t	path, name, ext, temp;
	int		ext_offset;

	temp = m_appSettings.RecentFiles[iRecentFileIndex].GetBuffer();
	
	
	//extract extention
	for( int i = temp.length() - 1; i >=0; i-- )
	{
		if( temp.data()[i] == '.' )
		{
			ext_offset = i;
			break;
		}
	}

	ext = temp.substr( ext_offset + 1, temp.length() - ext_offset );


	//extract path
	for( int i = temp.length() - 1; i >=0; i-- )
	{
		if( temp.data()[i] == '\\' )
		{
			ext_offset = i;
			break;
		}
	}

	path	= temp.substr( 0, ext_offset + 1 );
	name	= temp.substr( ext_offset + 1, temp.length() - ext_offset );

	ext_offset = 0;

	OpenFile( path, name, ext );
}

void CCookieMachineDlg::OpenRecentDir( int iRecentDirIndex )
{
	OpenFileFromDir( m_appSettings.RecentDirs[iRecentDirIndex].GetBuffer() );
}

void CCookieMachineDlg::OpenFile( str_t sPath, str_t sFile, str_t sExt )
{
	if( !FileExists( sPath + sFile ) )
	{
		MessageBox( "Файл не найден!", "Ошибка.", MB_OK | MB_ICONWARNING ); 
		return;
	}

	str_t			temp_path;
	CString			temp;
	bool			bFileSuccessfullyOpened	= false;
	bool			bIsIncorrectFileVersion	= false;
	CValidateDlg*	dlgValidate				= NULL;
	str_t			file_dir				= sPath;
	str_t			file_name				= sFile;
	CString			extention				= sExt.data();

	extention.MakeUpper();

	//show validation window
	if( !dlgValidate )
	{
		dlgValidate = new CValidateDlg( this );
		dlgValidate->SetMessageStr( "Проверка файла..." );
		ProcessMessages();
	}

	SetCursor( LoadCursor( NULL, IDC_WAIT ) );

	// HACK!!! // -- this sould be in SettingsApplyToApp
	SwitchCulling( m_appSettings.ViewportBackfaceCull );
	// HACK!!! //

	/**********************************************************/
	/******                   XSI FILE OPEN             *******/
	/**********************************************************/
	if( extention == "XSI" )
	{
		CXSIFile* pXsiFile = new CXSIFile( this, dlgValidate );

		if( pXsiFile->LoadFromFile( file_dir, file_name, m_appSettings.RepairXSIFiles ) )
		{
			CloseCurrentFile();

			pBaseScene = pXsiFile;
			pBaseScene->Prepare( g_pd3dDevice );
			pBaseScene->SetPaths( file_dir, file_name );

			bFileSuccessfullyOpened = true;
		}
		else
		{
			SAFE_DELETE( pXsiFile );
		}
	} 
	else if( extention == "EFN" )
	{
		/*********************************************************/
		/******            EFN ANIM	FILE OPEN				******/
		/*********************************************************/
		//check file prefix
		if( GetEfnFiletype( file_dir + file_name ) == EF_ANIMATION )
		{
			CEfnCharacter* efn_character = new CEfnCharacter;

			if( dlgValidate )
			{
				dlgValidate->SetMessageStr( "Загрузка файла..." );
				ProcessMessages();
			}

			efn_character->SetValidationDlg( dlgValidate );

			if( efn_character->LoadFromFile( file_dir + file_name ) )
			{
				efn_character->SetParentWnd( this );

				CloseCurrentFile();

				pBaseScene = efn_character;
				pBaseScene->SetPaths( file_dir, file_name );

				if( dlgValidate )
				{
					dlgValidate->SetProgressMessageStrA( "step 5 of 5" );
					dlgValidate->SetProgressMessageStrC( "Preparing buffers..." );
					dlgValidate->Step();
					ProcessMessages();
				}

				pBaseScene->Prepare( g_pd3dDevice );

				bFileSuccessfullyOpened = true;
			}
			else
			{
				// load failed -- cleanup
				SAFE_DELETE( efn_character ); 
			}

			efn_character->SetValidationDlg( NULL );

			LockCamOnObject();
		}
		else if( GetEfnFiletype( file_dir + file_name ) == EF_MESHLIST )
		{
			CEfnMeshlist*	efn_meshlist = new CEfnMeshlist;	

			if( efn_meshlist->LoadFromFile( file_dir + file_name ) )
			{
				//close any currently open file
				CloseCurrentFile();

				efn_meshlist->SetParent( this );

				if( m_appSettings.ScnAlternateNullIco )
					efn_meshlist->SetNullIcon( pNullIco );

				pBaseScene = efn_meshlist;
				pBaseScene->SetPaths( file_dir, file_name );


				//prepare data for rendering
				pBaseScene->Prepare( g_pd3dDevice );

				bFileSuccessfullyOpened = true;	
			}
			else
			{
				// load failed -- cleanup
				SAFE_DELETE( efn_meshlist ); 
			}
		}
		else
		{
		}
		
	} // 	else if (extention == "EFN")
	else if( extention == "SCN" )
	{
		/**********************************************************/
		/******                   SCN FILE OPEN              ******/
		/**********************************************************/
		CSceneFile* scene = NULL;
		DWORD		file_version = GetScnVersion( file_dir + file_name );

		if( scene->IsSupportedVersion( file_version ) )
		{
			CloseCurrentFile();

			scene = new CSceneFile;

			scene->SetPaths( file_dir, file_name );

			if( scene->LoadFromFile( file_dir + file_name ) )
			{
				scene->SetParent( this );

				CEfnMeshlist* meshlist = (CEfnMeshlist*)scene->GetMeshlist();

				if( m_appSettings.ScnAlternateNullIco )
					meshlist->SetNullIcon( pNullIco );

				pBaseScene = scene;

				//prepare data for rendering
				pBaseScene->Prepare( g_pd3dDevice );

				((CSceneFile*)pBaseScene)->SetLightBulbCone( pLightBulb, pLightCone );
				((CSceneFile*)pBaseScene)->SetCamBox( pCamBox );

				bFileSuccessfullyOpened = true;
			}
			else
			{
				// load failed -- cleanup
				SAFE_DELETE( scene ); 
			}
		}
		else
		{
			MessageBox( "Unsupported SCN file version! File can not be loaded.", "Warning!", MB_OK | MB_ICONEXCLAMATION );

			bIsIncorrectFileVersion = true;
		}
	} // 	else if (extention == "SCN")

	//restore cursor
	SetCursor(LoadCursor(NULL, IDC_ARROW));

	if( !bIsIncorrectFileVersion )
	{
		//in case file failed to open
		if( !bFileSuccessfullyOpened )
			MessageBox( "Файл неизвестного формата!", "Ошибка открытия файла", MB_OK | MB_ICONERROR);
		else
		{
			// create new entry in the list of recently opened files
			AddPathToRecentFilesList( file_dir + file_name );
			AddPathToRecentDirsList( file_dir );

			// change window caption
			temp_path = "Cookie Machine -- " + pBaseScene->GetFolder() + pBaseScene->GetFilename(); 
			this->SetWindowText( temp_path.data() );

			ResetStatusBarInfo();
			UpdateUI();
			EnableControls(true);
			UpdateUISizing();
		}

		//in case file opened ok, and application is set to reset camera each time new file opened
		if( bFileSuccessfullyOpened && m_bResetCameraOnFileOpen )
		{
			//reset cam to default one
			*m_ViewMgr.m_camCurrent = m_appSettings.DefaultCamera;
			//worldStates		= m_appSettings.DefaultWorldStates;
		}
	}

    // kill validation window
	SAFE_DELETE( dlgValidate );
}

ListStrVec* CCookieMachineDlg::GetBindsGlob()
{ 
	if( !pBaseScene || pBaseScene->GetType() != FT_SCN ) 
		return NULL; 
	
	return ((CSceneFile*)pBaseScene)->GetBinds();
};

BOOL CCookieMachineDlg::ToolbarTooltipProc( UINT id, NMHDR* pTTTStruct, LRESULT* pResult )
{
	TOOLTIPTEXT*	pTTT		= (TOOLTIPTEXT *)pTTTStruct;
    UINT			nID			= pTTTStruct->idFrom;
	str_t			tool_tip	= GetTooltip( nID );

	if( tool_tip != "" )
	{
		sprintf( pTTT->lpszText, "%s", tool_tip.data() );
		return(TRUE);
	}
	else
		return(FALSE);

    //if (pTTT->uFlags & TTF_IDISHWND)
    //{
    //    // idFrom is actually the HWND of the tool
    //    nID = ::GetDlgCtrlID((HWND)nID);
    //    if(nID)
    //    {
    //        pTTT->lpszText = "sdf";
    //        pTTT->hinst = AfxGetResourceHandle();
    //        return(TRUE);
    //    }
    //}
}

BOOL CCookieMachineDlg::OnInitDialog()
{
	str_t func_decl_str = "BOOL CCookieMachineDlg::OnInitDialog()";

	try
	{
		CDialog::OnInitDialog();

		ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
		ASSERT(IDM_ABOUTBOX < 0xF000);

		CMenu* pSysMenu = GetSystemMenu(FALSE);
		if (pSysMenu != NULL)
		{
			CString strAboutMenu;
			strAboutMenu.LoadString(IDS_ABOUTBOX);
			if (!strAboutMenu.IsEmpty())
			{
				pSysMenu->AppendMenu(MF_SEPARATOR);
				pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
			}
		}

		SetIcon(m_hIcon, TRUE);		// Set big icon
		SetIcon(m_hIcon, FALSE);	// Set small icon

		//InitializeControls();

		//if( Initialize() == false )
		//	exit(0);

		//m_bOnInitDialogFinished = true;

		return TRUE;
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}
}

void CCookieMachineDlg::SetupMatrices()
{ 
	//attach system light to camera
	if( m_appSettings.AttachToCamera )
	{
		D3DXVECTOR3 vecDir = m_ViewMgr.m_camCurrent->pos - m_ViewMgr.m_camCurrent->dir;
		D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );
		D3DXVec3Scale( (D3DXVECTOR3*)&light.Direction, (D3DXVECTOR3*)&light.Direction, -1.0f );
		g_pd3dDevice->SetLight( 0, &light );
	}

	D3DXMatrixLookAtLH( &m_matView, &m_ViewMgr.m_camCurrent->pos, &m_ViewMgr.m_camCurrent->dir, &m_ViewMgr.m_camCurrent->upvector );
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );

	UpdateRenderView();
}

void CCookieMachineDlg::UpdateRenderView()
{
	CRect rndWndRect;
	::GetWindowRect( m_hRenderWnd, rndWndRect );
	ScreenToClient( rndWndRect );
	m_fAspect = (float)rndWndRect.Height() / rndWndRect.Width();

	switch( m_ViewMgr.m_CurrView )
	{
		case RV_PERSPECTIVE:
			D3DXMatrixPerspectiveFovLH( &matProj, D3DXToRadian( m_ViewMgr.m_camCurrent->hfov ), 1.0f / m_fAspect, 0.1f, 500.0f );
			g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
			break;

		case RV_FRONT:
			D3DXMatrixOrthoLH( &matProj, m_ViewMgr.m_fZoomFront, m_ViewMgr.m_fZoomFront * m_fAspect, -1000.0f, 1000.0f );
			g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
			break;

		case RV_TOP:
			D3DXMatrixOrthoLH( &matProj, m_ViewMgr.m_fZoomTop, m_ViewMgr.m_fZoomTop * m_fAspect, -1000.0f, 1000.0f );
			g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
			break;

		case RV_RIGHT:
			D3DXMatrixOrthoLH( &matProj, m_ViewMgr.m_fZoomRight, m_ViewMgr.m_fZoomRight * m_fAspect, -1000.0f, 1000.0f );
			g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
			break;
	}
}

void CCookieMachineDlg::OnCbnSelchangeCmbViews()
{
	CComboBox* pViewsBox = (CComboBox*)GetDlgItem( IDC_CMB_VIEWS );

	if( !pViewsBox )
		return;

	switch( pViewsBox->GetCurSel() )
	{
		case 0:
			m_ViewMgr.ChangeRenderView( RV_PERSPECTIVE );
			break;

		case 1:
			m_ViewMgr.ChangeRenderView( RV_FRONT );
			break;

		case 2:
			m_ViewMgr.ChangeRenderView( RV_TOP );
			break;

		case 3:
			m_ViewMgr.ChangeRenderView( RV_RIGHT );
			break;
	}
}

void CCookieMachineDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if( (nID & 0xFFF0) == IDM_ABOUTBOX )
	{
		SAFE_DELETE( m_dlgAbout );

		m_dlgAbout = new CAboutBoxDlg( this );
	}
	else
	{
		CDialog::OnSysCommand( nID, lParam );
	}
}

void CCookieMachineDlg::OnPaint() 
{
	str_t func_decl_str = "void CCookieMachineDlg::OnPaint()";

	try
	{
		// device context for painting
		CPaintDC dc(this);

		//HDC ddc = (HDC)this->GetDC();
		//SetBkColor( ddc, RGB( 171, 168, 166 ) );

		if( IsIconic() )
		{
			SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

			// Center icon in client rectangle
			int cxIcon = GetSystemMetrics(SM_CXICON);
			int cyIcon = GetSystemMetrics(SM_CYICON);
			CRect rect;
			GetClientRect(&rect);
			int x = (rect.Width() - cxIcon + 1) / 2;
			int y = (rect.Height() - cyIcon + 1) / 2;

			// Draw the icon
			dc.DrawIcon(x, y, m_hIcon);
		}
		else
		{
			//Disable
			CDialog::OnPaint();
		}
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
//HCURSOR CCookieMachineDlg::OnQueryDragIcon()
//{
//	return (HCURSOR) m_hIcon;
//}

float CCookieMachineDlg::GetIconsScale()
{
	return m_appSettings.IconsScaling;
}

void CCookieMachineDlg::OnMenuFileOpen()
{
	OpenFileFromDir( m_strLastFileOpenDirectory.GetBuffer() );
}

void CCookieMachineDlg::ShowScriptsEditorGlob()
{
	if( !m_dlgScriptingDlg )
		ShowScriptsWindow();
	else
	{
		m_dlgScriptingDlg->SetWindowPos( &CWnd::wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
	}
}

void CCookieMachineDlg::ShowTextResEditorGlob( str_t strInitialItem, int iErrLine )
{
	if( m_dlgTextResourcesDlg && m_dlgTextResourcesDlg->IsWindowVisible() )
	{
		m_dlgTextResourcesDlg->SetCurrResource( strInitialItem, iErrLine );
		m_dlgTextResourcesDlg->OnShowWindow( true, 0 );
		m_dlgTextResourcesDlg->SetWindowPos( &CWnd::wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
	}
	else
		ShowTextResourcesWindow( strInitialItem, iErrLine ); 
}

void CCookieMachineDlg::ShowTextResourcesWindow( str_t strInitialItem, int iErrLine )
{
	if( !pBaseScene || pBaseScene->GetType() != FT_SCN )
		return;

	if( !m_dlgTextResourcesDlg )
	{
		m_dlgTextResourcesDlg = new CTextResourcesDlg( this );
		
		m_dlgTextResourcesDlg->SetScene( pBaseScene );
		m_dlgTextResourcesDlg->SetCurrResource( strInitialItem, iErrLine );

		if( m_dlgTextResourcesDlg->Create() == false )
			SAFE_DELETE( m_dlgTextResourcesDlg );
	}

	if( m_dlgTextResourcesDlg )
		m_dlgTextResourcesDlg->ShowWindow( SW_SHOW );
}

//OPENFILENAME ofn;
//HWND hPar;
//HWND hPars;
//
//CCustomFileDialog* dlg = NULL;

UINT CALLBACK OpenDlgExProc( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	//switch( uiMsg )
	//{
	//	case WM_COMMAND:
	//		switch( LOWORD(wParam) )
	//		{
	//			case IDC_BTN_MODELS:
	//				{
	//					CWnd* pWnd = CWnd::FromHandle(hDlg)->GetParent()->GetDlgItem( 0x047C );

	//					//CEdit* cb = (CEdit*)pWnd;

	//					//if( cb )
	//					//{
	//					//	cb->SetWindowText( (EngineFolder + "\\data\\models").GetBuffer() );
	//					//	cb->SendMessage( WM_COMMAND, IDOK, 0 );
	//					//	cb->SetWindowText( "" );
	//					//}
	//				}
	//				break;

	//			case IDC_BTN_SCENES:
	//				{
	//					CWnd* pWnd = CWnd::FromHandle(hDlg)->GetParent()->GetDlgItem( 0x047C );

	//					//CEdit* cb = (CEdit*)pWnd;

	//					//if( cb )
	//					//{
	//					//	cb->SetWindowText( (EngineFolder + "\\data\\models\\scenes").GetBuffer() );
	//					//	cb->SendMessage( WM_COMMAND, IDOK, 0 );
	//					//	cb->SetWindowText( "" );
	//					//}
	//				}
	//				break;
	//		}
	//		break;

	//}

//	//LPOFNOTIFY pN;
//	
//
//	hPars = hDlg;
//
	//switch( uiMsg )
	//{
	//	case WM_SHOWWINDOW:
	//		{
	//			SetTimer( hDlg, 999, 0, NULL );
	//		}
	//		break;

	//	case WM_TIMER:
	//		{
	//			if( wParam == 999 )
	//			{
	//				KillTimer( hDlg, 999 );

	//				CWnd* pDlg = CWnd::FromHandle( hDlg );
	//				CWnd* pWndTB = pDlg->GetParent()->GetDlgItem( 0x04A0 );
	//				CToolBarCtrl* __tb = (CToolBarCtrl*)pWndTB;
	//	
	//				while( __tb->GetButtonCount() > 0 )
	//				{
	//					__tb->DeleteButton( 0 );
	//				}

	//				TBBUTTON btns;

	//				btns.fsState = TBSTATE_ENABLED;
	//				btns.fsStyle = TBSTYLE_BUTTON;
	//				btns.iBitmap = 10;
	//				btns.idCommand = 666;
	//				btns.iString	= 0;

	//				//btns[1].fsState = TBSTATE_ENABLED;
	//				//btns[1].fsStyle = TBSTYLE_BUTTON;
	//				//btns[1].iBitmap = 10;
	//				//btns[1].idCommand = 667;
	//				//btns[1].iString	= 1;

	//				__tb->InsertButton( 0, &btns );
	//			}
	//		}
	//		break;

	//	case WM_COMMAND:
	//		switch( LOWORD(wParam) )
	//		{
	//			case 666:
	//				{
	//					hPar = GetParent( hDlg );

	//					//ofn.lpstrInitialDir = (EngineFolder + "\\data\\models").GetBuffer();

	//					//SendMessage( hPar, WM_SHOWWINDOW, 0, 0 );
	//					//SendMessage( hPar, WM_NOTIFY, IDCANCEL, 0 );
	//					//GetOpenFileName( &ofn );
	//				}
	//				break;
	//		}
	//		break;

	//	//default:
	//	//	if( dlg && dlg->GetSafeHwnd() && dlg->GetParent() )//IsWindow( dlg->GetSafeHwnd() ) )
	//	//	{
	//	//		dlg->GetParent()->SendMessage( uiMsg, wParam, lParam );
	//	//		return 0;
	//	//	}
	//	//	break;
	//}

	return 0;
}

void CCookieMachineDlg::OpenFileFromDir( str_t strDir )
{
	str_t	szFilter;
	CString	temp;


	//show open file dialog
	switch( lastFileType )
	{
		case FT_ALLSUPPORTED:
			temp = "*.efn";
			szFilter = "All Supported Files (*.efn;*.xsi;*.scn)|*.efn; *.xsi; *.scn|EFN Files (*.efn)|*.efn|Softimage dotXSI Files (*.xsi)|*.xsi|Scene Files (*.scn)|*.scn|All Files (*.*)|*.*|";
		break;

		case FT_EFN:
			temp = "*.efn";
			szFilter = "EFN Files (*.efn)|*.efn|Softimage dotXSI Files (*.xsi)|*.xsi|Scene Files (*.scn)|*.scn|All Supported Files (*.efn;*.xsi;*.scn)|*.efn; *.xsi; *.scn|All Files (*.*)|*.*|";
		break;

		case FT_XSI:
			temp = "*.xsi";
			szFilter = "Softimage dotXSI Files (*.xsi)|*.xsi|EFN Files (*.efn)|*.efn|Scene Files (*.scn)|*.scn|All Supported Files (*.efn;*.xsi;*.scn)|*.efn; *.xsi; *.scn|All Files (*.*)|*.*|";
		break;

		case FT_SCN:
			temp = "*.scn";
			szFilter = "Scene Files (*.scn)|*.scn|Softimage dotXSI Files (*.xsi)|*.xsi|EFN Files (*.efn)|*.efn|All Supported Files (*.efn;*.xsi;*.scn)|*.efn; *.xsi; *.scn|All Files (*.*)|*.*|";
		break;

		case FT_UNDEFINED:
			temp = "*.efn";
			szFilter = "EFN Files (*.efn)|*.efn|Softimage dotXSI Files (*.xsi)|*.xsi|Scene Files (*.scn)|*.scn|All Supported Files (*.efn;*.xsi;*.scn)|*.efn; *.xsi; *.scn|All Files (*.*)|*.*|";
		break;
	}

	//TCHAR	szFile[MAX_PATH] = {0};

	//ZeroMemory( &ofn, sizeof( OPENFILENAME ) );

	//ofn.lStructSize = sizeof( OPENFILENAME );
	//ofn.lpstrFile	= szFile;
	//ofn.nMaxFile	= sizeof( szFile );
	//ofn.lpstrFilter	= __TEXT( "All files\0*.*\0" );
	//ofn.Flags		= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_ENABLEHOOK | OFN_ENABLETEMPLATE | OFN_ENABLESIZING;
	//ofn.lpTemplateName = MAKEINTRESOURCE( IDD_OPENFILE_EXT );
	//ofn.lpfnHook	= OpenDlgExProc;
	//ofn.hInstance	= GetModuleHandle(NULL);
	//ofn.FlagsEx		= OFN_EX_NOPLACESBAR;
	//ofn.lpstrInitialDir = "C:\\kav";

	//GetOpenFileName( &ofn );

	//::DestroyWindow( hPars );

	CCustomFileDialog* dlg = new CCustomFileDialog( TRUE, temp.GetBuffer(0), 0, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_ENABLETEMPLATE | OFN_ENABLESIZING, szFilter.data(), NULL );
	dlg->m_pOFN->lpstrInitialDir	= strDir.data();
	//dlg->m_pOFN->lpfnHook			= OpenDlgExProc;
	dlg->m_pOFN->FlagsEx			= OFN_EX_NOPLACESBAR;
	dlg->m_pOFN->lpTemplateName		= MAKEINTRESOURCE( IDD_OPENFILE_EXT );
	dlg->m_pOFN->hInstance			= GetModuleHandle(NULL);

	m_bIsAppPaused = false;

	//user confirmed "open file dialog"
	if( dlg->DoModal() == IDOK )
	{
		//get file dir
		str_t file_dir	= dlg->GetPathName().Left( dlg->GetPathName().GetLength() - dlg->GetFileName().GetLength() );
		m_strLastFileOpenDirectory = file_dir.data();

		//get filename
		str_t file_name	= dlg->GetFileName();

		OpenFile( file_dir, file_name, dlg->GetFileExt().GetBuffer() );
	}

	SAFE_DELETE( dlg );
}

void CCookieMachineDlg::Load3DIcons()
{
	str_t func_decl_str = "void CCookieMachineDlg::Load3DIcons()";

	try
	{
		/////////////////////////
		//	load lightbulb	   //
		/////////////////////////
		str_t IconDir = m_strAppPath.GetBuffer();
		IconDir += "\\data\\";
		str_t IconFile = "LightIcon.efn";
		
		if( FileExists( IconDir + IconFile ) )
		{
			CEfnCharacter* lightIco = new CEfnCharacter;
			lightIco->LoadFromFile( IconDir + IconFile );
			lightIco->SetPaths( IconDir, IconFile );
			lightIco->Prepare( g_pd3dDevice );
			pLightBulb = lightIco;
		}

		/////////////////////////
		//	load light cone    //
		/////////////////////////
		str_t ConeDir = m_strAppPath.GetBuffer();
		ConeDir += "\\data\\";
		str_t ConeFile = "LightCone.efn";
		
		if( FileExists( ConeDir + ConeFile ) )
		{
			CEfnCharacter* lightIco = new CEfnCharacter;
			lightIco->LoadFromFile(  ConeDir + ConeFile  );
			lightIco->SetPaths( ConeDir, ConeFile );
			lightIco->Prepare( g_pd3dDevice );
			pLightCone = lightIco;
		}

		/////////////////////////
		//		load cambox	   //
		/////////////////////////
		str_t CamDir = m_strAppPath.GetBuffer();
		CamDir += "\\data\\";
		str_t CamFile = "CameraIcon.efn";
		
		if( FileExists( CamDir + CamFile ) )
		{
			CEfnCharacter* camIco = new CEfnCharacter;
			camIco->LoadFromFile(  CamDir + CamFile  );
			camIco->SetPaths( CamDir, CamFile );
			camIco->Prepare( g_pd3dDevice );
			pCamBox = camIco;
		}


		//////////////////
		// load null //
		//////////////////
		str_t NullDir = m_strAppPath.GetBuffer();
		NullDir += "\\data\\";
		str_t NullFile = "NullIcon.efn";
		
		if( FileExists( NullDir + NullFile ) )
		{
			CEfnCharacter* NullIco = new CEfnCharacter;
			NullIco->LoadFromFile(  NullDir + NullFile  );
			NullIco->SetPaths( NullDir, NullFile );
			NullIco->Prepare( g_pd3dDevice );
			pNullIco = NullIco;
		}
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}
}

void CCookieMachineDlg::ShowObjectPropertiesWnd( int iShowMode )
{
	if( m_bIsObjPropertiesWndVisible == false )
		return;

	switch( m_ObjPropWnd )
	{
		case OWE_UNDEFINED:
			m_pPropCustomParamDlg->ShowWindow( SW_HIDE );
			m_pPropCustomParamDlg->ReleaseLinkedData();

			m_pPropNullDlg->ShowWindow( SW_HIDE );
			m_pPropNullDlg->ReleaseLinkedData();

			m_pPropCameraDlg->ShowWindow( SW_HIDE );
			m_pPropCameraDlg->ReleaseLinkedData();
			
			m_pPropLightDlg->ShowWindow( SW_HIDE );
			m_pPropLightDlg->ReleaseLinkedData();

			m_pObjPropWnd = NULL;
			break;	

		case OWE_CUSTPARAM:
			m_pPropNullDlg->ShowWindow( SW_HIDE );
			m_pPropCameraDlg->ShowWindow( SW_HIDE );
			m_pPropLightDlg->ShowWindow( SW_HIDE );

			m_pPropCustomParamDlg->ShowWindow( iShowMode );
			m_pObjPropWnd = m_pPropCustomParamDlg;
			break;

		case OWE_NULL:
			m_pPropLightDlg->ShowWindow( SW_HIDE );
			m_pPropCustomParamDlg->ShowWindow( SW_HIDE );
			m_pPropCameraDlg->ShowWindow( SW_HIDE );

			m_pPropNullDlg->ShowWindow( iShowMode );
			m_pObjPropWnd = m_pPropNullDlg;
			break;

		case OWE_CAMERA:
			m_pPropLightDlg->ShowWindow( SW_HIDE );
			m_pPropNullDlg->ShowWindow( SW_HIDE );
			m_pPropCustomParamDlg->ShowWindow( SW_HIDE );

			m_pPropCameraDlg->ShowWindow( iShowMode );
			m_pObjPropWnd = m_pPropCameraDlg;
			break;

		case OWE_LIGHT:
			m_pPropNullDlg->ShowWindow( SW_HIDE );
			m_pPropCustomParamDlg->ShowWindow( SW_HIDE );
			m_pPropCameraDlg->ShowWindow( SW_HIDE );

			m_pPropLightDlg->ShowWindow( iShowMode );
			m_pObjPropWnd = m_pPropLightDlg;
			break;
	}

	if( m_pObjPropWnd )
	{
		CWnd* pWnd = GetDlgItem( IDC_OBJPROP_WND_FRAME );
		CRect rect;

		pWnd->GetWindowRect( &rect );
		ScreenToClient( &rect );

		m_pObjPropWnd->MoveWindow( rect.left, rect.top, rect.Width(), rect.Height(), true );
		m_pObjPropWnd->SetAccelerator( m_hAccelerator );

		m_pObjPropWnd->UpdateWindow();
	}
}

void CCookieMachineDlg::InitializeGlobalVars()
{
	//// app states
	//m_bIsSceneExplorerVisible		= true;
	//m_bIsAppPaused				= false;
	//m_bIsAppMinimized				= false;
	//m_bIsVertexEditingModeOn		= false;
	//m_bIsMacroRecStarted			= false;
	//m_bIsGRadEditorModeOn			= false;
	//m_bIsPerspectiveCameraOn		= true;
	//m_appSettings.RecentFilesClear	= false;
	//m_bIsAnimationLooped			= false;
	//m_bIsAnimationMoving			= false;
	//m_bIsPlayingFinished			= true;
	//m_bScrollSelectionStarted		= false;
	//modelStates.RenderMode		= RM_SHADED;
	//m_AxisAlign					= AAE_X;
	//m_bSCNLoaded					= false;
	//m_bIsCameraInterestVisible	= false;
	//m_bOnInitDialogFinished		= false;
	//m_bNormalsRendered			= false;
	//m_bClosing					= false;

	m_fGlobRadScale		= 1.0f;
	m_iCurrentFrame		= 0;
	m_bIsPlaying		= false;
	
	// renderer related stuff
	pBaseScene		= NULL;
	pCamBox			= NULL;
	pLightBulb		= NULL;
	pLightCone		= NULL;
	pNullIco		= NULL;
	m_pFont			= NULL;
	m_pTextSprite	= NULL;
	m_txt			= NULL;
	
	// mouse states
	m_bIsLMouseButtonDown		= false;
	m_bDoubleClicked			= false;
	m_bIsMouseEditingOpStarted	= false;

	// utility windows
	m_dlgScriptingDlg		= NULL;
	m_dlgBindingDlg			= NULL;
	m_dlgOptions			= NULL;
	m_dlgTextResourcesDlg	= NULL;
	m_dlgAbout				= NULL;

	// child windows used in main dialog
	m_bIsObjPropertiesWndVisible = false;
	m_ObjPropWnd			= OWE_UNDEFINED;
	m_pObjPropWnd			= NULL;
	m_pPropCustomParamDlg	= NULL;
	m_pPropNullDlg			= NULL;
	m_pPropCameraDlg		= NULL;
	m_pPropLightDlg			= NULL;

	
	// controls
	m_pToolbarImageList		= NULL;
	m_pToolbarImageListHot	= NULL;
	m_pToolbarImageListDis	= NULL;
	m_pToolbar				= NULL;
	m_StatusBar				= NULL;
	m_pImageList			= NULL;
	m_iSplitterX			= 220;

	m_hLexDll = NULL;

	pSelectedEfnAnimNull	= NULL;

	ZeroMemory( Vertices, sizeof( VERTEX2D_RHW ) * 8 );

	m_strIniFilename	= "Cookie Machine.ini";
	m_strCmdLine		= "";


	// D3D vars
	ZeroMemory( &m_Bmtrl, sizeof(m_Bmtrl) );
	m_Bmtrl.Diffuse.r = 0.0f;
	m_Bmtrl.Diffuse.g = 0.0f;
	m_Bmtrl.Diffuse.b = 0.0f;
	m_Bmtrl.Diffuse.a = 1.0f;

	m_Bmtrl.Ambient.r = 0.0f;
	m_Bmtrl.Ambient.g = 0.0f;
	m_Bmtrl.Ambient.b = 0.0f;
	m_Bmtrl.Ambient.a = 1.0f;

	ZeroMemory( &m_Gmtrl, sizeof(m_Gmtrl) );
	m_Gmtrl.Diffuse.r = 1.0f;
	m_Gmtrl.Diffuse.g = 1.0f;
	m_Gmtrl.Diffuse.b = 1.0f;
	m_Gmtrl.Diffuse.a = 0.0f;

	m_Gmtrl.Ambient.r = 0.5f;
	m_Gmtrl.Ambient.g = 0.5f;
	m_Gmtrl.Ambient.b = 0.5f;
	m_Gmtrl.Ambient.a = 1.0f;

	m_Gmtrl.Specular.r = 0.8f;
	m_Gmtrl.Specular.g = 0.8f;
	m_Gmtrl.Specular.b = 0.8f;

	m_Gmtrl.Power = 18.2f;


	ZeroMemory( &m_Wmtrl, sizeof(m_Wmtrl) );
	m_Wmtrl.Diffuse.r = 1.0f;
	m_Wmtrl.Diffuse.g = 0.0f;
	m_Wmtrl.Diffuse.b = 0.0f;
	m_Wmtrl.Diffuse.a = 1.0f;

	m_Wmtrl.Ambient.r = 1.0f;
	m_Wmtrl.Ambient.g = 0.0f;
	m_Wmtrl.Ambient.b = 0.0f;
	m_Wmtrl.Ambient.a = 1.0f;

	D3DXVECTOR3 vecDir;
	ZeroMemory( &light, sizeof(light) );
	light.Type = D3DLIGHT_DIRECTIONAL;

	light.Diffuse.r = 1.0f;
	light.Diffuse.g = 1.0f;
	light.Diffuse.b = 1.0f;

	light.Ambient.r = 0.2f;
	light.Ambient.g = 0.2f;
	light.Ambient.b = 0.2f;

	light.Specular.r = 0.9f;
	light.Specular.g = 0.9f;
	light.Specular.b = 0.9f;


	// setup materials
	ZeroMemory( &m_BlackMtrl, sizeof( m_BlackMtrl ) );
	m_BlackMtrl.Diffuse = D3DXCOLOR( 0xff000000 );
	m_BlackMtrl.Ambient = D3DXCOLOR( 0xff000000 );

	ZeroMemory( &m_WhiteMtrl, sizeof( m_WhiteMtrl ) );
	m_WhiteMtrl.Diffuse = D3DXCOLOR( 0xffffffff );
	m_WhiteMtrl.Ambient = D3DXCOLOR( 0xffffffff );

	ZeroMemory( &m_RedMtrl, sizeof( m_RedMtrl ) );
	m_RedMtrl.Diffuse = D3DXCOLOR( 0xffff0000 );
	m_RedMtrl.Ambient = D3DXCOLOR( 0xffff0000 );

	ZeroMemory( &m_GreenMtrl, sizeof( m_GreenMtrl ) );
	m_GreenMtrl.Diffuse = D3DXCOLOR( 0xff00ff00 );
	m_GreenMtrl.Ambient = D3DXCOLOR( 0xff00ff00 );

	ZeroMemory( &m_BlueMtrl, sizeof( m_BlueMtrl ) );
	m_BlueMtrl.Diffuse = D3DXCOLOR( 0xff0000ff );
	m_BlueMtrl.Ambient = D3DXCOLOR( 0xff0000ff );


	vecDir = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );
	light.Range = 3000.0f;
}

void CCookieMachineDlg::InitializeControls()
{
	str_t func_decl_str = "void CCookieMachineDlg::InitializeControls()";

	try
	{
		// coords view menu, used in toolbar
		HBITMAP hb = LoadBitmap( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_VIEW_MENU) );

		CStatic* pWnd = (CStatic*)GetDlgItem( IDC_BMP );
		pWnd->SetBitmap( hb );

		m_menuViewPopup.LoadMenu( IDR_MENU_VIEW );

		// cameras combo
		m_cmbCameras.AddString( DEFAULT_CAMERA_ID );
		m_cmbCameras.SetCurSel( 0 );

		// predefined cam views combo
		CComboBox* pViewsBox = (CComboBox*)GetDlgItem( IDC_CMB_VIEWS );

		if( pViewsBox )
		{
			pViewsBox->ResetContent();
			pViewsBox->AddString( "Perspective" );
			pViewsBox->AddString( "Front" );
			pViewsBox->AddString( "Top" );
			pViewsBox->AddString( "Right" );
			pViewsBox->SetCurSel( 0 );
		}

		CreateToolbar();
		CreateTreeViewImages();
		CreateStatusBar();

		// scene explorer treeview control
		m_rtSceneContents.SetParent( this );

		// setup splitter
		CRect rc;
		CWnd* pWndSplit = GetDlgItem( IDC_SPLITTER );

		pWndSplit->GetWindowRect( &rc );
		ScreenToClient( &rc );
		m_wndSplitter1.Create( WS_CHILD | WS_VISIBLE, rc, this, IDC_SPLITTER );
		m_wndSplitter1.SetRange( 0, 200, -1 );

		CButton* pBtnStop = (CButton*)GetDlgItem( IDC_BTN_ANIM_STOP );
		CButton* pBtnPlay = (CButton*)GetDlgItem( IDC_BTN_ANIM_PLAY );
		CButton* pBtnZOff = (CButton*)GetDlgItem( IDC_BUTN_SET_Z_OFF );

		pBtnStop->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(), MAKEINTRESOURCE( IDB_ANIM_CTRLS_STOP ) ) );
		pBtnPlay->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(), MAKEINTRESOURCE( IDB_ANIM_CTRLS_PLAY ) ) );
		pBtnZOff->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(), MAKEINTRESOURCE( IDB_ANIM_CTRLS_ZOFFSET ) ) );
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}
}

bool CCookieMachineDlg::Initialize()
{
	str_t func_decl_str = "bool CCookieMachineDlg::Initialize()";

	try
	{
		m_ViewMgr.SetParent( this );

		m_brush.CreateSolidBrush( RGB( 171, 168, 166 ) );

		SHChangeNotify( SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL );

		EnableWalkMode(false);

		m_hRenderWnd = GetDlgItem( IDC_RENDERWND )->GetSafeHwnd();

		SetTimer( MEMORY_STAT_TIMER, 100, (TIMERPROC) NULL );     

		EnableControls( false );

		//open file from cmd line
		m_strCmdLine = m_strCmdLine.Trim();
		
		CString dir, file, ext;

		//extension
		ext = m_strCmdLine.Right( 3 );
		ext.MakeUpper();

		//path and file
		for( int i = m_strCmdLine.GetLength() - 1; i >= 0; i-- )
		{
			if( m_strCmdLine[i] == '\\' )
			{
				dir = m_strCmdLine.Left( m_strCmdLine.GetLength() - (m_strCmdLine.GetLength() - i - 1) );
				file = m_strCmdLine.Right( m_strCmdLine.GetLength() - i - 1 );
				break;
			}
		}

		//initialize direct3d
		D3DInit( m_hRenderWnd );

		SetTimer( RENDERING_TIMER, 0, (TIMERPROC) NULL );		
		SettingsApplyToApp();
		Load3DIcons();

		if( m_strCmdLine != "" )
			OpenFile( dir.GetBuffer(), file.GetBuffer(), ext.GetBuffer() );

		m_dlgTesselatedMesh = new CTesselatedMeshDlg(this);
		m_dlgTesselatedMesh->Create();

		return true;
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}
}

void CCookieMachineDlg::CreateStatusBar()
{
	//status bar
	static UINT BASED_CODE indicators[] =
	{
		ID_INDICATOR_PAGE,
		ID_INDICATOR_PAGE2,
		ID_INDICATOR_PAGE3,
		ID_INDICATOR_PAGE4,
		ID_INDICATOR_PAGE5,
		ID_INDICATOR_PAGE6,
	};

	m_StatusBar = new CStatusBar;

	m_StatusBar->Create( (CWnd*)this, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM, SB );
	m_StatusBar->SetIndicators( indicators, sizeof(indicators)/sizeof(UINT) );
	m_StatusBar->SetPaneText( 0, "Memory status:", true );
	m_StatusBar->SetPaneInfo( 0, 0, SBPS_NOBORDERS, 85 ); 
	m_StatusBar->SetPaneInfo( 1, 0, 0, 145 );
	m_StatusBar->SetPaneInfo( 2, 0, 0, 145 );
	m_StatusBar->SetPaneInfo( 3, 0, SBPS_STRETCH, 50 );
	m_StatusBar->SetPaneInfo( 4, 0, 0, 200 );
	m_StatusBar->SetPaneInfo( 5, 0, 0, 150 );

	m_StatusBar->SetPaneText( 5, "Ready.", true );
}

void CCookieMachineDlg::CreateTreeViewImages()
{
	CBitmap		bitmap;
	vector<int>	images;

	m_pImageList = new CImageList();
	m_pImageList->Create( 16, 16, ILC_COLOR24 | ILC_MASK, 0, 4 );
	

	images.push_back(IDB_MESH);
	images.push_back(IDB_GROUP);
	images.push_back(IDB_INFO);
	images.push_back(IDB_VAR);
	images.push_back(IDB_MESH_DIS); 
	images.push_back(IDB_ANIM); //5
	images.push_back(IDB_MAT);
	images.push_back(IDB_SEQUENCE);
	images.push_back(IDB_ANIM_ITEM);
	images.push_back(IDB_LIGHT); 
	images.push_back(IDB_NULL_ITEM); //10
	images.push_back(IDB_ANIM_DIS);
	images.push_back(IDB_LIGHT_DIS);
	images.push_back(IDB_BIND);
	images.push_back(IDB_GRID); 
	images.push_back(IDB_SCRIPT); //15
	images.push_back(IDB_TEXTURE);
	images.push_back(IDB_ACT_OBJ);
	images.push_back(IDB_LAYER_GEOM);
	images.push_back(IDB_CAMSELECT); 
	images.push_back(IDB_GROUP_INFO); //20
	images.push_back(IDB_TEXTURE2);
	images.push_back(IDB_FOLDER);
	images.push_back(IDB_INFO2);
	images.push_back(IDB_NULL_ITEM_DIS); 
	images.push_back(IDB_CUST_PARAM_SET); //25
	images.push_back(IDB_CUST_PARAM);
	images.push_back(IDB_CUST_PARAM_STATIC);
	images.push_back(IDB_ATTACH); //28
	images.push_back(IDB_CUST_PARAM_DIS);

	for( UINT i = 0; i < images.size(); i++ )
	{
		bitmap.LoadBitmap( images[i] );
		m_pImageList->Add( &bitmap, RGB(255, 255, 0) );
		bitmap.DeleteObject();
	}

	m_rtSceneContents.SetImageList( m_pImageList, TVSIL_NORMAL );
}

void CCookieMachineDlg::HandlePossibleSizeChange()
{
	str_t func_decl_str = "HRESULT CCookieMachineDlg::HandlePossibleSizeChange()";

	try
	{
		if( !g_pd3dDevice )
			return;

		::GetClientRect( m_hRenderWnd, &m_rcWindowClient );
		//::GetWindowRect( m_hRenderWnd, &m_rcWindowClient );

		//ScreenToClient( &m_rcWindowClient );

		g_d3dpp.BackBufferWidth  = m_rcWindowClient.Width();
		g_d3dpp.BackBufferHeight = m_rcWindowClient.Height();

		SAFE_RELEASE( m_pTextSprite );
		SAFE_DELETE( m_txt );
		SAFE_RELEASE( m_pFont );

		g_pd3dDevice->Reset( &g_d3dpp );

		OnResetDevice();

		/*hr = g_pd3dDevice->Reset( &g_d3dpp );

		if( hr != D3D_OK )
			OnLostDevice();*/
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}
}

void CCookieMachineDlg::Render()
{
	if( m_bClosing )
		return;

	str_t func_decl_str = "void CCookieMachineDlg::Render()";

	try
	{
		if( m_bIsAppMinimized )
			return;

		if( !g_pD3D )
			return;

		if( !g_pd3dDevice || g_pd3dDevice->TestCooperativeLevel() != D3D_OK )
		{
			OnLostDevice();
			return;
		}

		D3DXMATRIXA16 m_WorldYRotMatrix;
		D3DXMATRIXA16 m_WorldXZRotMatrix;

		static long last_call_time = 0;

		float fps_limit			= 1000 / 60;
		float fps_limit_idle	= 1000 / 1;

		if( m_bIsAppPaused )
			fps_limit = fps_limit_idle;
		
		if( last_call_time == 0 )
		{
			last_call_time = timeGetTime();
		}
		else
		{
			if( timeGetTime() - last_call_time < fps_limit )
				return;
			else
				last_call_time = timeGetTime();
		}

		// Clear the backbuffer to a blue color
		DWORD	dwClearFlags	= D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;
		LONG	lClearColor		= D3DCOLOR_XRGB(	GetRValue( m_appSettings.ViewportBackColor ), 
													GetGValue( m_appSettings.ViewportBackColor ), 
													GetBValue( m_appSettings.ViewportBackColor ) );

		g_pd3dDevice->Clear( 0, NULL, dwClearFlags, lClearColor, 1.0f, 0 );
		g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, false );

		D3DXMATRIX identity;
		D3DXMatrixIdentity( &identity );

		// Begin the scene
		if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
		{
			//Setup the world, view, and projection matrices
			SetupMatrices();

			if( m_appSettings.GridVisible )
			{
				//lights off
				g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, false );
				//g_pd3dDevice->LightEnable( 0, false );
				g_pd3dDevice->SetTexture( 0, NULL );

				D3DXMATRIX rot;

				if( m_ViewMgr.m_CurrView == RV_FRONT )
				{
					D3DXMatrixRotationX( &rot, D3DX_PI / 2.0f );
					g_pd3dDevice->SetTransform( D3DTS_WORLD, &rot );
				}
				else if( m_ViewMgr.m_CurrView == RV_RIGHT )
				{
					D3DXMatrixRotationZ( &rot, D3DX_PI / 2.0f );
					g_pd3dDevice->SetTransform( D3DTS_WORLD, &rot );
				}


				//draw grid
				g_pd3dDevice->SetStreamSource( 0, g_pGridVB, 0, sizeof(CVERTEX) );
				g_pd3dDevice->SetFVF( D3DFVF_VERTEX_DESCRIPTOR );
				g_pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, m_appSettings.GridSize * 2 + 2 );


				g_pd3dDevice->SetTransform( D3DTS_WORLD, &identity );

				if( m_ViewMgr.m_CurrView != RV_PERSPECTIVE )
					g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, lClearColor, 1.0f, 0 );
			}

			if( pBaseScene && pBaseScene->GetType() == FT_EFN && m_bIsGRadEditorModeOn )
			{
				D3DXMATRIX rot;
				D3DXMATRIX scale;
				D3DXMatrixRotationYawPitchRoll( &rot, 0.0f, D3DX_PI / 2.0f, 0.0f );

				float glob_rad = ((CEfnCharacter*)pBaseScene)->GetGlobalRadius();
				D3DXMatrixScaling( &scale, glob_rad, glob_rad, 1.0f );

				g_pd3dDevice->SetTransform( D3DTS_WORLD, &D3DXMATRIX( scale * rot ) );
				ppCylinder->DrawSubset( 0 );

				g_pd3dDevice->SetTransform( D3DTS_WORLD, &identity );
			}

			g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, true );
			g_pd3dDevice->LightEnable( 0, true );
			
			// Render the vertex buffer contents
			if( pBaseScene )
			{
				if( !g_pd3dDevice || g_pd3dDevice->TestCooperativeLevel() != D3D_OK )
				{
					OnLostDevice();
					return;
				}  

				if( pBaseScene->GetType() == FT_XSI )
					RenderXSI();

				if( pBaseScene->GetType() == FT_EFN )
					RenderEfn();

				if( pBaseScene->GetType() == FT_EFN_MESHLIST )
					RenderMeshlist();

				if( pBaseScene->GetType() == FT_SCN )
					RenderSCN();
			}

			
			//cam interest
			if( m_bIsCameraInterestVisible )
			{
				g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, false );

				CVERTEX_XYZ cam_int[6] = { {-1,0,0}, {1,0,0}, {0,-1,0}, {0,1,0}, {0,0,-1}, {0,0,1} };

				D3DXMATRIX tr;
				D3DXMatrixTranslation( &tr, m_ViewMgr.m_camCurrent->dir.x, m_ViewMgr.m_camCurrent->dir.y, m_ViewMgr.m_camCurrent->dir.z );

				g_pd3dDevice->SetTransform( D3DTS_WORLD, &tr );

				g_pd3dDevice->SetFVF( D3DFVF_XYZ );
				g_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 3, &cam_int, sizeof(CVERTEX_XYZ) ); 

				g_pd3dDevice->SetTransform( D3DTS_WORLD, &identity );
			}

			RenderVertexSelectionFrame();
			RenderCoordAxis();

			g_pd3dDevice->EndScene();
		}

		// Present the backbuffer contents to the display
		if( g_pd3dDevice->Present( NULL, NULL, NULL, NULL ) == D3DERR_DEVICELOST )
			OnLostDevice();
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}
}

void CCookieMachineDlg::LockCamOnObjectAllViews()
{
	if( !pBaseScene )
	{
		m_ViewMgr.LockCameraOnPoint( RV_PERSPECTIVE,	D3DXVECTOR3( 0, 0, 0 ), 10.0f, D3DXVECTOR3( -5, -5, -5 ), D3DXVECTOR3( 5, 5, 5 ), m_fAspect );
		m_ViewMgr.LockCameraOnPoint( RV_TOP,			D3DXVECTOR3( 0, 0, 0 ), 10.0f, D3DXVECTOR3( -5, -5, -5 ), D3DXVECTOR3( 5, 5, 5 ), m_fAspect );
		m_ViewMgr.LockCameraOnPoint( RV_RIGHT,			D3DXVECTOR3( 0, 0, 0 ), 10.0f, D3DXVECTOR3( -5, -5, -5 ), D3DXVECTOR3( 5, 5, 5 ), m_fAspect );
		m_ViewMgr.LockCameraOnPoint( RV_FRONT,			D3DXVECTOR3( 0, 0, 0 ), 10.0f, D3DXVECTOR3( -5, -5, -5 ), D3DXVECTOR3( 5, 5, 5 ), m_fAspect );

		return;
	}

	if( pBaseScene->GetType() != FT_SCN && pBaseScene->GetType() != FT_EFN_MESHLIST && pBaseScene->GetType() != FT_EFN )
		return;

	D3DXVECTOR3		vOOBBCenter;
	CSPHERE			pBSphere;
	D3DXVECTOR3		oobMin, oobMax;

	CEfnMeshlist*	pMeshlist	= NULL;
	CEfnCharacter*	pEfnAnim	= NULL;

	
	if( pBaseScene->GetType() == FT_SCN )
	{
		CSceneFile* pScene = (CSceneFile*)pBaseScene;
		pMeshlist = (CEfnMeshlist*)pScene->GetMeshlist();

		if( !pMeshlist )
			return;
	}
	else if( pBaseScene->GetType() == FT_EFN_MESHLIST )
	{
		pMeshlist = (CEfnMeshlist*)pBaseScene;

		if( !pMeshlist )
			return;
	}
	else if( pBaseScene->GetType() == FT_EFN )
	{
		pEfnAnim = (CEfnCharacter*)pBaseScene;

		if( !pEfnAnim )
			return;
	}

	if( pMeshlist )
	{
		if( pMeshlist->GetSelectedMeshesCount() < 1 )
			return;
		
		pMeshlist->GetSelectedMeshesOOBB( &oobMin, &oobMax );

		vOOBBCenter	= pMeshlist->GetSelectedMeshesOOBBCenter();
		pBSphere	= pMeshlist->GetSelectedMeshesBSphere();
	}

	if( pEfnAnim )
	{
		pEfnAnim->GetSelectedMeshesOOBB( &oobMin, &oobMax );

		vOOBBCenter	= pEfnAnim->GetSelectedMeshesOOBBCenter();
		pBSphere	= pEfnAnim->GetSelectedMeshesBSphere();
	}

	m_ViewMgr.LockCameraOnPoint( RV_PERSPECTIVE,	vOOBBCenter, pBSphere.radius * 2.6f, oobMin, oobMax, m_fAspect );
	m_ViewMgr.LockCameraOnPoint( RV_TOP,			vOOBBCenter, pBSphere.radius * 2.6f, oobMin, oobMax, m_fAspect );
	m_ViewMgr.LockCameraOnPoint( RV_RIGHT,			vOOBBCenter, pBSphere.radius * 2.6f, oobMin, oobMax, m_fAspect );
	m_ViewMgr.LockCameraOnPoint( RV_FRONT,			vOOBBCenter, pBSphere.radius * 2.6f, oobMin, oobMax, m_fAspect );
}

void CCookieMachineDlg::LockCamOnObject()
{
	//CString strTemp, strRes;

	//strRes += "pos: ";
	//strTemp.Format( "%.4f, %.4f, %.4f", m_ViewMgr.m_camCurrent->pos.x, m_ViewMgr.m_camCurrent->pos.y, m_ViewMgr.m_camCurrent->pos.z );
	//strRes += strTemp;

	//strRes += "  dir: ";
	//strTemp.Format( "%.4f, %.4f, %.4f", m_ViewMgr.m_camCurrent->dir.x, m_ViewMgr.m_camCurrent->dir.y, m_ViewMgr.m_camCurrent->dir.z );
	//strRes += strTemp;

	//strRes += "  up: ";
	//strTemp.Format( "%.4f, %.4f, %.4f", m_ViewMgr.m_camCurrent->upvector.x, m_ViewMgr.m_camCurrent->upvector.y, m_ViewMgr.m_camCurrent->upvector.z );
	//strRes += strTemp;

	//SetWindowText( strRes );

	if( !pBaseScene )
	{
		m_ViewMgr.LockCameraOnPoint( RV_CURRENT, D3DXVECTOR3( 0, 0, 0 ), 10.0f, D3DXVECTOR3( -5, -5, -5 ), D3DXVECTOR3( 5, 5, 5 ), m_fAspect );
		return;
	}

	if( pBaseScene->GetType() != FT_SCN && pBaseScene->GetType() != FT_EFN_MESHLIST && pBaseScene->GetType() != FT_EFN )
		return;

	D3DXVECTOR3		vOOBBCenter;
	CSPHERE			pBSphere;
	D3DXVECTOR3		oobMin, oobMax;

	CEfnMeshlist*	pMeshlist	= NULL;
	CEfnCharacter*	pEfnAnim	= NULL;

	
	if( pBaseScene->GetType() == FT_SCN )
	{
		CSceneFile* pScene = (CSceneFile*)pBaseScene;
		pMeshlist = (CEfnMeshlist*)pScene->GetMeshlist();

		if( !pMeshlist )
			return;
	}
	else if( pBaseScene->GetType() == FT_EFN_MESHLIST )
	{
		pMeshlist = (CEfnMeshlist*)pBaseScene;

		if( !pMeshlist )
			return;
	}
	else if( pBaseScene->GetType() == FT_EFN )
	{
		pEfnAnim = (CEfnCharacter*)pBaseScene;

		if( !pEfnAnim )
			return;
	}

	if( pMeshlist )
	{
		if( pMeshlist->GetSelectedMeshesCount() < 1 )
			return;
		
		pMeshlist->GetSelectedMeshesOOBB( &oobMin, &oobMax );

		vOOBBCenter	= pMeshlist->GetSelectedMeshesOOBBCenter();
		pBSphere	= pMeshlist->GetSelectedMeshesBSphere();
	}

	if( pEfnAnim )
	{
		pEfnAnim->GetSelectedMeshesOOBB( &oobMin, &oobMax );

		vOOBBCenter	= pEfnAnim->GetSelectedMeshesOOBBCenter();
		pBSphere	= pEfnAnim->GetSelectedMeshesBSphere();
	}

	m_ViewMgr.LockCameraOnPoint( RV_CURRENT, vOOBBCenter, pBSphere.radius * 2.6f, oobMin, oobMax, m_fAspect );
}

void CCookieMachineDlg::RenderVertexSelectionFrame()
{
	// render selection frame
	if( m_bIsLMouseButtonDown && !m_ViewMgr.IsCameraOperationInProgress() )
	{
		Vertices[0].x = m_ptTopLeft.x;
		Vertices[0].y = m_ptTopLeft.y;
		Vertices[0].color = 0xff0000ff;

		Vertices[1].x = m_ptBottomRight.x;
		Vertices[1].y = m_ptTopLeft.y;
		Vertices[1].color = 0xff0000ff;

		Vertices[2].x = m_ptTopLeft.x;
		Vertices[2].y = m_ptTopLeft.y;
		Vertices[2].color = 0xff0000ff;

		Vertices[3].x = m_ptTopLeft.x;
		Vertices[3].y = m_ptBottomRight.y;
		Vertices[3].color = 0xff0000ff;

		Vertices[4].x = m_ptBottomRight.x;
		Vertices[4].y = m_ptTopLeft.y;
		Vertices[4].color = 0xff0000ff;

		Vertices[5].x = m_ptBottomRight.x;
		Vertices[5].y = m_ptBottomRight.y;
		Vertices[5].color = 0xff0000ff;

		Vertices[6].x = m_ptTopLeft.x;
		Vertices[6].y = m_ptBottomRight.y;
		Vertices[6].color = 0xff0000ff;

		Vertices[7].x = m_ptBottomRight.x;
		Vertices[7].y = m_ptBottomRight.y;
		Vertices[7].color = 0xff0000ff;

		//render
		D3DXMATRIX tr;
		D3DXMatrixIdentity( &tr );
		g_pd3dDevice->SetTransform( D3DTS_WORLD, &tr );

		g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff );
		g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, false );
		SetVColorFromMaterial( true );
		g_pd3dDevice->SetMaterial( &m_BlueMtrl );
		g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, true );

		g_pd3dDevice->SetFVF( D3DFVF_VERTEX2D_RHW );
		g_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 4, Vertices, sizeof( VERTEX2D_RHW ) );
	}
}

void CCookieMachineDlg::RenderCoordAxis()
{
	str_t func_decl_str = "void CCookieMachineDlg::RenderCoordAxis()";

	try
	{
		//cross

		//get our vector out into the world
		CRect		rcRenderWnd;
		CStatic*	render_wnd = (CStatic*)GetDlgItem( IDC_RENDERWND );
		render_wnd->GetWindowRect( &rcRenderWnd );
		ScreenToClient( &rcRenderWnd );

		D3DXMATRIX oldProjection, matProjection;
		D3DXVECTOR3 out, scr = D3DXVECTOR3( 55, rcRenderWnd.bottom - 130, 0 );
		D3DVIEWPORT9 viewport;
		D3DXMATRIX world;
		D3DXMatrixIdentity( &world );
		g_pd3dDevice->GetViewport( &viewport );

		g_pd3dDevice->GetTransform( D3DTS_PROJECTION, &oldProjection );


		if( m_ViewMgr.m_CurrView != RV_PERSPECTIVE )
		{
			D3DXMatrixOrthoLH( &matProjection, 1, m_fAspect, -1000.0f, 1000.0f );
			g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProjection );

			D3DXVec3Unproject( &out, &scr, &viewport, &matProjection, &m_matView, &world );
		}
		else
			D3DXVec3Unproject( &out, &scr, &viewport, &matProj, &m_matView, &world );


		//move it a bit away from cam
		D3DXVECTOR3 out0 = m_ViewMgr.m_camCurrent->pos - out;
		float length = D3DXVec3Length( &out0 );

		length -= 1.0f;

		D3DXVec3Normalize( &out0, &out0 );
		D3DXVec3Scale( &out0, &out0, length );

		if( m_ViewMgr.m_CurrView == RV_PERSPECTIVE )
		{
			out = out + out0;
		}
		else if( m_ViewMgr.m_CurrView == RV_TOP )
		{
			out.y = 999;
		}
		else if( m_ViewMgr.m_CurrView == RV_FRONT )
		{
			out.z = -999;
		}
		else if( m_ViewMgr.m_CurrView == RV_RIGHT )
		{
			out.x = 999;
		}

		//draw coord arrows

		//set world transform
		D3DXMATRIX tr, sc;
		D3DXMatrixTranslation( &tr, out.x, out.y, out.z );
		D3DXMatrixScaling( &sc, 0.03f, 0.03f, 0.03f );
		g_pd3dDevice->SetTransform( D3DTS_WORLD, &( sc * tr ) );

		//get vectors back to screen ( used to draw labels )
		D3DXVECTOR3 xLBL, yLBL, zLBL;

		if( m_ViewMgr.m_CurrView == RV_PERSPECTIVE )
		{
			D3DXVec3Project( &xLBL, &D3DXVECTOR3( 1.2f, 0.0f, 0.0f ), &viewport, &matProj, &m_matView, &( sc * tr ) );
			D3DXVec3Project( &yLBL, &D3DXVECTOR3( 0.0f, 1.2f, 0.0f ), &viewport, &matProj, &m_matView, &( sc * tr ) );
			D3DXVec3Project( &zLBL, &D3DXVECTOR3( 0.0f, 0.0f, 1.2f ), &viewport, &matProj, &m_matView, &( sc * tr ) );
		}
		else
		{
			D3DXVec3Project( &xLBL, &D3DXVECTOR3( 1.2f, 0.0f, 0.0f ), &viewport, &matProjection, &m_matView, &( sc * tr ) );
			D3DXVec3Project( &yLBL, &D3DXVECTOR3( 0.0f, 1.2f, 0.0f ), &viewport, &matProjection, &m_matView, &( sc * tr ) );
			D3DXVec3Project( &zLBL, &D3DXVECTOR3( 0.0f, 0.0f, 1.2f ), &viewport, &matProjection, &m_matView, &( sc * tr ) );	
		}

		D3DMATERIAL9 oldMat;
		g_pd3dDevice->GetMaterial( &oldMat );

		g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0 );
		g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, true );
		g_pd3dDevice->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );
		g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff );
		//SetVColorFromMaterial( true );

		CVERTEX_XYZ_COL cam_x[2] = { {0,0,0,0}, {1,0,0,0} };
		g_pd3dDevice->SetMaterial( &m_RedMtrl );
		g_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, &cam_x, sizeof(CVERTEX_XYZ_COL) );

		CVERTEX_XYZ_COL cam_y[2] = { {0,0,0,0}, {0,1,0,0} };
		g_pd3dDevice->SetMaterial( &m_GreenMtrl );
		g_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, &cam_y, sizeof(CVERTEX_XYZ_COL) );

		CVERTEX_XYZ_COL cam_z[2] = { {0,0,0,0}, {0,0,1,0} };
		g_pd3dDevice->SetMaterial( &m_BlueMtrl );
		g_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, &cam_z, sizeof(CVERTEX_XYZ_COL) );


		g_pd3dDevice->SetTransform( D3DTS_WORLD, &world );
		g_pd3dDevice->SetMaterial( &oldMat );
		SetVColorFromMaterial( false );

		// Output statistics
		if( !m_txt )
			return;

		m_txt->Begin();
	    
		m_txt->SetInsertionPos( xLBL.x - 4, xLBL.y - 6 );
		m_txt->SetForegroundColor( D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ) );
		m_txt->DrawTextLine( "X" );

		m_txt->SetInsertionPos( yLBL.x - 4, yLBL.y - 8 );
		m_txt->SetForegroundColor( D3DXCOLOR( 0.0f, 1.0f, 0.0f, 1.0f ) );
		m_txt->DrawTextLine( "Y" );

		m_txt->SetInsertionPos( zLBL.x - 4, zLBL.y - 6 );
		m_txt->SetForegroundColor( D3DXCOLOR( 0.0f, 0.0f, 1.0f, 1.0f ) );
		m_txt->DrawTextLine( "Z" );

		m_txt->SetInsertionPos( rcRenderWnd.Width() / 2.0f - 50, rcRenderWnd.bottom - 110 );
		m_txt->SetForegroundColor( D3DXCOLOR( 0.0f, 1.0f, 0.0f, 1.0f ) );

		switch( m_ViewMgr.m_CurrView )
		{
			case RV_PERSPECTIVE:
				m_txt->DrawTextLine( "Perspective" );
				break;

			case RV_RIGHT:
				m_txt->DrawTextLine( "Right" );
				break;

			case RV_TOP:
				m_txt->DrawTextLine( "Top" );
				break;	

			case RV_FRONT:
				m_txt->DrawTextLine( "Front" );
				break;	
		}

		m_txt->End();

		g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &oldProjection );

		SetVColorFromMaterial( true );

	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}
}

inline void CCookieMachineDlg::SetVColorFromMaterial( bool	bValue )
{
	if( bValue )
	{
		g_pd3dDevice->SetRenderState( D3DRS_COLORVERTEX, FALSE );
		g_pd3dDevice->SetRenderState( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL );
	 	g_pd3dDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL );
		g_pd3dDevice->SetRenderState( D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL );
		g_pd3dDevice->SetRenderState( D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL );
	}
	else
	{
		g_pd3dDevice->SetRenderState( D3DRS_COLORVERTEX, TRUE );
		g_pd3dDevice->SetRenderState( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL );
		g_pd3dDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );
		g_pd3dDevice->SetRenderState( D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR2 );
		g_pd3dDevice->SetRenderState( D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL );
	}
}
void CCookieMachineDlg::ShowLogsWindow()
{
	HWND desk_wnd = (HWND)GetDesktopWindow();

	CString path = m_appSettings.EngineFolder + "\\Log\\log.html";

	if( FileExists( path.GetBuffer() ) )
		ShellExecute( desk_wnd, __TEXT("open"), __TEXT( path ), __TEXT(""), __TEXT("c:\\"), SW_SHOW );
}

void CCookieMachineDlg::OnLostDevice()
{
	 str_t func_decl_str = "void CCookieMachineDlg::OnLostDevice()";

	try
	{
		if( !m_bOnInitDialogFinished )
			return;

		if( !g_pd3dDevice )
			D3DInit( m_hRenderWnd );

		if( D3D_OK == g_pd3dDevice->TestCooperativeLevel() )
			return;
		
		if( D3DERR_DEVICENOTRESET == g_pd3dDevice->TestCooperativeLevel() )
		{
			if( D3D_OK == g_pd3dDevice->Reset( &g_d3dpp ) )
			{
				OnResetDevice();
				return;
			}

			Sleep( 10 );
		}

		SAFE_RELEASE( m_pTextSprite );
		SAFE_DELETE( m_txt );
		SAFE_RELEASE( m_pFont );

		if( D3DERR_DEVICENOTRESET == g_pd3dDevice->TestCooperativeLevel() )
		{
			if( D3D_OK == g_pd3dDevice->Reset( &g_d3dpp ) )
			{
				if( D3D_OK == g_pd3dDevice->TestCooperativeLevel() )
				{
					OnResetDevice();
					return;
				}
			}
		}
		else if( D3DERR_DEVICELOST == g_pd3dDevice->TestCooperativeLevel() )
		{
			if( !g_pd3dDevice )
				return;

			//grid
			SAFE_RELEASE( g_pGridVB );
			GridCreate();

			SAFE_RELEASE( ppMesh );
			D3DXCreateSphere( g_pd3dDevice, 0.5f, 10, 10, &ppMesh, NULL );
			
			SAFE_RELEASE( ppCylinder );
			D3DXCreateCylinder( g_pd3dDevice, 1.0f, 1.0f, 0.2f, 20, 1, &ppCylinder, NULL );


			if( pBaseScene )
				pBaseScene->Prepare( g_pd3dDevice );

			if( pLightBulb )
				pLightBulb->Prepare( g_pd3dDevice );

			if( pCamBox )
				pCamBox->Prepare( g_pd3dDevice );

			if( pNullIco )
				pNullIco->Prepare( g_pd3dDevice );

			OnResetDevice();
		}
		else if( D3DERR_DRIVERINTERNALERROR == g_pd3dDevice->TestCooperativeLevel() )
		{
			g_ExeptLog->__data.push_back( "Internal driver error." );
			throw(0);
		}
		else
		{
			//D3DInit( m_hRenderWnd );
		}
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}
}

void CCookieMachineDlg::OnResetDevice()
{
	str_t func_decl_str = "void CCookieMachineDlg::OnResetDevice()";

	try
	{
		if( !g_pd3dDevice )
			return;

		SAFE_RELEASE( m_pTextSprite );
		SAFE_DELETE( m_txt );
		SAFE_RELEASE( m_pFont );

		D3DXCreateSprite( g_pd3dDevice, &m_pTextSprite );
		D3DXCreateFont( g_pd3dDevice, 18, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, 
							OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, 
							"Arial", &m_pFont );
		
		m_txt = new CD3Dtext( m_pFont, m_pTextSprite, 15 );

		g_pd3dDevice->SetMaterial( &m_Gmtrl );
		g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xFFFFFFFF ); 
		g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

		g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR ); 
		g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

		g_pd3dDevice->SetLight( 0, &light );

		g_pd3dDevice->LightEnable( 0, TRUE );

		//enable z-buffering
		g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
		//D3DZB_USEW

		g_pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );

		float PointSize = 4;
		g_pd3dDevice->SetRenderState( D3DRS_POINTSIZE, *((DWORD*)&PointSize ) );

		if( modelStates.IsCullingOn )
			g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
		else
			g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}
}

void CCookieMachineDlg::SelectAllObjects()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() == FT_EFN )
	{
		SelectAllAnimFrames();
	}
	else if( pBaseScene->GetType() == FT_EFN_MESHLIST )
	{
		m_rtSceneContents.SetFocus();

		if( !m_bIsVertexEditingModeOn )
		{
			HTREEITEM hMeshes	= m_rtSceneContents.FindTreeItemByName( "Meshes" );
			HTREEITEM hMesh		= m_rtSceneContents.GetChildItem( hMeshes );
			m_rtSceneContents.SelectItem( hMesh );

			while( hMesh )
			{
				hMesh = m_rtSceneContents.GetNextItem( hMesh, TVGN_NEXT );
				m_rtSceneContents.SetItemState( hMesh, TVIS_SELECTED, TVIS_SELECTED );
			}

			((CEfnMeshlist*)pBaseScene)->SelectAllMeshes();
			((CEfnMeshlist*)pBaseScene)->SelectAllNulls();
		}
		else
		{
			HTREEITEM	hMesh	= m_rtSceneContents.GetSelectedItem();
			CMesh*		pMesh	= ((CEfnMeshlist*)pBaseScene)->GetMeshByName( m_rtSceneContents.GetItemText( hMesh ).GetBuffer() );

			if( pMesh )
				pMesh->SelectAllVertecies();
		}

		UpdateStatusBarInfoMESHLIST();
	}
	else if( pBaseScene->GetType() == FT_SCN )
	{
		CEfnMeshlist* pMeshlist	= (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();
		
		if( !pMeshlist )
			return;

		m_rtSceneContents.SetFocus();

		if( !m_bIsVertexEditingModeOn )
		{
			HTREEITEM hMeshes	= m_rtSceneContents.FindTreeItemByName( "Meshes" );
			HTREEITEM hMesh		= m_rtSceneContents.GetChildItem( hMeshes );
			m_rtSceneContents.SelectItem( hMesh );

			while( hMesh )
			{
				hMesh = m_rtSceneContents.GetNextItem( hMesh, TVGN_NEXT );
				m_rtSceneContents.SetItemState( hMesh, TVIS_SELECTED, TVIS_SELECTED );
			}

			pMeshlist->SelectAllMeshes();
		}
		else
		{
			HTREEITEM	hMesh	= m_rtSceneContents.GetSelectedItem();
			CMesh*		pMesh	= pMeshlist->GetMeshByName( m_rtSceneContents.GetItemText( hMesh ).GetBuffer() );

			if( pMesh )
				pMesh->SelectAllVertecies();
		}

		UpdateStatusBarInfoSCN();
	}
	else
	{}
}

void CCookieMachineDlg::DeselectAllObjects()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() == FT_EFN )
	{
		DeselectAllAnimFrames();
	}
	else if( pBaseScene->GetType() == FT_EFN_MESHLIST )
	{
		((CEfnMeshlist*)pBaseScene)->DeselectAllMeshes();
		((CEfnMeshlist*)pBaseScene)->DeselectAllNulls();
	}
	else if( pBaseScene->GetType() == FT_SCN )
	{
        ((CSceneFile*)pBaseScene)->DeselectAllCurves();
		CEfnMeshlist* pMeshlist = (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();

		if( !pMeshlist )
			return;

		pMeshlist->DeselectAllMeshes();
		pMeshlist->DeselectAllNulls();
	}
}

void CCookieMachineDlg::RenderSCN()
{
	switch( modelStates.RenderMode )
	{
		case RM_SHADED_WIREFRAME:
			{
				if( m_appSettings.LightingEnabled )
				{
					g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
					g_pd3dDevice->LightEnable( 0, TRUE );
					SettingsApplyLight();
				}
				else
				{
					g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
					g_pd3dDevice->LightEnable( 0, FALSE );
					g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_ARGB( 0, 0, 0, 0 ) );
				}

				g_pd3dDevice->SetMaterial( &m_Gmtrl );
				g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );

				pBaseScene->Render( g_pd3dDevice );


				g_pd3dDevice->SetMaterial(&m_Wmtrl);
				g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
				g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00000000 );
				g_pd3dDevice->LightEnable( 0, FALSE );


				D3DXMatrixPerspectiveFovLH( &matProj, D3DXToRadian( m_ViewMgr.m_camCurrent->hfov ), 1.0f / m_fAspect, 0.1001f, 500.0001f );
				g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

				((CSceneFile*)pBaseScene)->GetMeshlist()->Render( g_pd3dDevice );

				D3DXMatrixPerspectiveFovLH( &matProj, D3DXToRadian( m_ViewMgr.m_camCurrent->hfov ), 1.0f / m_fAspect, 0.1f, 500.0f );
				g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
			}
		break;

		case RM_SHADED:
			if( m_appSettings.LightingEnabled )
			{
				g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
				g_pd3dDevice->LightEnable( 0, TRUE );
				SettingsApplyLight();
			}
			else
			{
				g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
				g_pd3dDevice->LightEnable( 0, FALSE );
				g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_ARGB( 0, 0, 0, 0 ) );
			}

			g_pd3dDevice->SetMaterial(&m_Gmtrl);
			g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );

			pBaseScene->Render( g_pd3dDevice );

		break;

		case RM_WIREFRAME:
			g_pd3dDevice->SetMaterial( &m_Bmtrl );
			g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
			g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00000000 );
			g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
			g_pd3dDevice->LightEnable( 0, FALSE );

			pBaseScene->Render( g_pd3dDevice );
		break;
	}
}

void CCookieMachineDlg::OnMenuExit() 
{
	CloseCurrentFile();
	OnOK();
}

void CCookieMachineDlg::SettingsApplyLight()
{
	int rA = GetRValue( m_appSettings.LightAmbientColor );
	int gA = GetGValue( m_appSettings.LightAmbientColor );
	int bA = GetBValue( m_appSettings.LightAmbientColor );

	int rD = GetRValue( m_appSettings.LightDiffuseColor );
	int gD = GetGValue( m_appSettings.LightDiffuseColor );
	int bD = GetBValue( m_appSettings.LightDiffuseColor );

	light.Ambient.r = rA / 255.0f;
	light.Ambient.g = gA / 255.0f;
	light.Ambient.b = bA / 255.0f;

	light.Diffuse.r = rD / 255.0f;
	light.Diffuse.g = gD / 255.0f;
	light.Diffuse.b = bD / 255.0f;

	g_pd3dDevice->SetLight( 0, &light );

	if( m_appSettings.LightingEnabled )
	{
		g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
		g_pd3dDevice->LightEnable( 0, TRUE );
	}
	else
	{
		g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		g_pd3dDevice->LightEnable( 0, FALSE );
	}

	g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_ARGB( 0,
		                                                        GetRValue(m_appSettings.AmbientColor), 
																GetGValue(m_appSettings.AmbientColor), 
																GetBValue(m_appSettings.AmbientColor) ) );
}

void CCookieMachineDlg::RenderMeshlist()
{
	switch ( modelStates.RenderMode )
	{
		case RM_SHADED_WIREFRAME:
			if( m_appSettings.LightingEnabled )
			{
				g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
				g_pd3dDevice->LightEnable( 0, TRUE );
				SettingsApplyLight();
			}
			else
			{
				g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
				g_pd3dDevice->LightEnable( 0, FALSE );
				g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xFF000000 );
			}

			g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xFF000000 );

			g_pd3dDevice->SetMaterial( &m_Gmtrl );
			g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );

			pBaseScene->Render( g_pd3dDevice );

			g_pd3dDevice->SetMaterial(&m_Wmtrl);
			g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
			g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xFF000000 );
			g_pd3dDevice->LightEnable( 0, FALSE );

			D3DXMatrixPerspectiveFovLH( &matProj, D3DXToRadian( m_ViewMgr.m_camCurrent->hfov ), 1.0f / m_fAspect, 0.1002f, 500.0001f );
			g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

			pBaseScene->Render( g_pd3dDevice );

			D3DXMatrixPerspectiveFovLH( &matProj, D3DXToRadian( m_ViewMgr.m_camCurrent->hfov ), 1.0f / m_fAspect, 0.1f, 500.0f );
			g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

		break;

		case RM_SHADED:
			if( m_appSettings.LightingEnabled )
			{
				g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
				g_pd3dDevice->LightEnable( 0, TRUE );
				SettingsApplyLight();
			}
			else
			{
				g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
				g_pd3dDevice->LightEnable( 0, FALSE );
				g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_ARGB( 0, 0, 0, 0 ) );
			}

			g_pd3dDevice->SetMaterial(&m_Gmtrl);
			g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );

			pBaseScene->Render( g_pd3dDevice );

		break;

		case RM_WIREFRAME:
			g_pd3dDevice->SetMaterial( &m_Bmtrl );
			g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
			g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00000000 );
			g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
			g_pd3dDevice->LightEnable( 0, FALSE );

			((CEfnMeshlist*)pBaseScene)->Render( g_pd3dDevice, false );
		break;
	}
}

void CCookieMachineDlg::D3DInit( HWND hWnd )
{
	str_t func_decl_str = "void CCookieMachineDlg::D3DInit( HWND hWnd )";

	try
	{
		D3DCleanup();

		if( g_pd3dDevice != NULL )
			SAFE_RELEASE( g_pd3dDevice );

		if( g_pD3D != NULL )
			SAFE_RELEASE( g_pD3D );

		if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		{
			g_ExeptLog->__data.push_back( "\"Direct3DCreate9\" failed!" );
			throw(0);
		}

		ZeroMemory( &g_d3dpp, sizeof(g_d3dpp) );

		g_d3dpp.Windowed				= TRUE;
		g_d3dpp.SwapEffect				= D3DSWAPEFFECT_DISCARD;
		g_d3dpp.EnableAutoDepthStencil	= TRUE;
		g_d3dpp.AutoDepthStencilFormat	= D3DFMT_D24S8;
		g_d3dpp.BackBufferFormat		= D3DFMT_X8R8G8B8;
		g_d3dpp.PresentationInterval	= D3DPRESENT_INTERVAL_IMMEDIATE;

		if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice ) ) )
		{
			g_ExeptLog->__data.push_back( "\"CreateDevice\" failed!" );
			throw(0);
		}

		GridCreate();

		// global radius cylinder
		SAFE_RELEASE( ppCylinder );
		D3DXCreateCylinder( g_pd3dDevice, 1.0f, 1.0f, 0.2f, 20, 1, &ppCylinder, NULL );

		SwitchRenderMode( RM_SHADED );

		DXUtil_Timer( TIMER_RESET );
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}
}

void CCookieMachineDlg::UpdateUI_EFN_MESHLIST()
{
	HTREEITEM		hInfo, hMeshes, hMesh, hNulls;
	UINT			uiMask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

	char buff[512];
	int iImageTemp;

	/*-------------- clean up --------------- */
	m_rtSceneContents.DeleteAllItems();


	/*-------------- insert root nodes ------ */
	//insert root node *info*
	hInfo = m_rtSceneContents.InsertItem( uiMask, "Info", 23, 23, 0, 0, 0, NULL, NULL );
	//insert root node *meshes*
	hMeshes = m_rtSceneContents.InsertItem( uiMask, "Meshes", 1, 1, 0, 0, 0, NULL, NULL );
	//insert and check root node *nulls*
	hNulls = m_rtSceneContents.InsertItem( uiMask, "Nulls", 1, 1, 0, 0, 0, NULL, NULL );


	//insert *info* nodes
	sprintf( buff, "File: %s", pBaseScene->GetFilename().data() );
	m_rtSceneContents.InsertItem( uiMask, buff, 3, 3, 0, 0, 0, hInfo, NULL );

	sprintf( buff, "Format: EFN_MESHLIST" );
	m_rtSceneContents.InsertItem( uiMask, buff, 3, 3, 0, 0, 0, hInfo, NULL );

	sprintf( buff, "Version: [%s]", ((CEfnMeshlist*)pBaseScene)->GetVersion().data() );
	m_rtSceneContents.InsertItem( uiMask, buff, 3, 3, 0, 0, 0, hInfo, NULL );

	sprintf( buff, "Meshes count: %i", ((CEfnMeshlist*)pBaseScene)->GetMeshesCount() - 1 );
	m_rtSceneContents.InsertItem( uiMask, buff, 3, 3, 0, 0, 0, hInfo, NULL );
	
	sprintf( buff, "Total polycount: %i", ((CEfnMeshlist*)pBaseScene)->GetTotalPolycount() );
	m_rtSceneContents.InsertItem( uiMask, buff, 3, 3, 0, 0, 0, hInfo, NULL );

	//sprintf( buff, "Scene name: %s", ((CEfnMeshlist*)pBaseScene)->GetSceneTag().data() );
	//m_rtSceneContents.InsertItem( uiMask, buff, 3, 3, 0, 0, 0, hInfo, NULL );

	//insert *mesh* nodes
	for( int i = 0; i < ((CEfnMeshlist*)pBaseScene)->GetMeshesCount(); i++ )
	{
		CMesh* mesh	= ((CEfnMeshlist*)pBaseScene)->GetMeshByIndex( i );
		CString nm	= mesh->GetName().data();

		if( nm == "Nulls" )
			continue;

		// set image depending on mesh visibility
		if( ((CEfnMeshlist*)pBaseScene)->GetMeshByIndex( i )->GetVisible() )
			iImageTemp = 0;
		else
			iImageTemp = 4;

		hMesh = m_rtSceneContents.InsertItem( uiMask, nm.GetBuffer(), iImageTemp, iImageTemp, 0, 0, 0, hMeshes, TVI_SORT );

		// insert null nodes
		for( int j = 0; j < mesh->GetNullsCount(); j++ )
		{
			nm = mesh->GetNullByIndex( j )->id.data();
			m_rtSceneContents.InsertItem( uiMask, nm.GetBuffer(), 10, 10, 0, 0, 0, hMesh, TVI_SORT );
		}
	}

	CMesh* mesh	= ((CEfnMeshlist*)pBaseScene)->GetMeshByName( "Nulls" );

	for( int i = 0; i < mesh->GetNullsCount(); i++ )
	{	
		CNULL* pNull = mesh->GetNullByIndex( i );

		if( !pNull )
			continue;

		m_rtSceneContents.InsertItem( uiMask, pNull->id.data(), 10, 10, 0, 0, 0, hNulls, NULL );
	}

	m_rtSceneContents.Expand( hInfo, TVE_EXPAND );
	//m_rtSceneContents.Expand( hMeshes, TVE_EXPAND );

	/* cleanup */
	firstSelFrame		= -1;
    lastSelFrame		= -1;
	m_iStartingFrame	= 0;

	m_eFrameCounter.SetWindowText( "0" );

	m_sliderAnim.SetRange( 0, 0, 0 );
	m_sliderAnim.SetPos( 0 );
	m_sliderAnim.ClearSel( true );

	m_cmbCameras.ResetContent();
	m_cmbCameras.AddString( DEFAULT_CAMERA_ID );
	m_cmbCameras.SetCurSel( 0 );
	/* cleanup END */
	
}

void CCookieMachineDlg::UpdateUI()
{
	if( !pBaseScene )
		return;

	switch( pBaseScene->GetType() )
	{
		case FT_XSI:
			UpdateUI_XSI();
		break;

		case FT_EFN:
			UpdateUI_EFN();
		break;

		case FT_SCN:
			UpdateUI_SCN();
		break;

		case FT_EFN_MESHLIST:
			UpdateUI_EFN_MESHLIST();
		break;
	}
}

void CCookieMachineDlg::UpdateUI_SCN()
{
	HTREEITEM		hInfo, hLight, hCamera, hTexture, hMesh, hNull, hTemp, hTemp2, hBinds, hCustObj, hCustTypes, hCurves;
	char			szTemp[256];
	bool			bTemp;

	DWORD	uiMask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

	CSceneFile*		pScn		= ((CSceneFile*)pBaseScene);
	CEfnMeshlist*	meshlist	= (CEfnMeshlist*)pScn->GetMeshlist();
	CMesh*			nulls_mesh	= meshlist->GetMeshByName( "Nulls" );

	//clear tree view
	m_rtSceneContents.DeleteAllItems();

	
	//insert root node *info*
	hInfo = m_rtSceneContents.InsertItem( uiMask, "Info", 23, 23, 0, 0, 0, NULL, NULL );

	sprintf( szTemp, "Filename: %s",  pScn->GetFilename().c_str() );
	m_rtSceneContents.InsertItem( uiMask, szTemp, 3, 3, 0, 0, 0, hInfo, NULL );

	m_rtSceneContents.InsertItem( uiMask, "Format: SCN", 3, 3, 0, 0, 0, hInfo, NULL );
	
	sprintf( szTemp, "Version: [%s]", ((CSceneFile*)pBaseScene)->GetVersion().data() );
	m_rtSceneContents.InsertItem( uiMask, szTemp, 3, 3, 0, 0, 0, hInfo, NULL );

	sprintf( szTemp, "Level ID: %s",  pScn->GetLevelID().c_str() );
	m_rtSceneContents.InsertItem( uiMask, szTemp, 3, 3, 0, 0, 0, hInfo, NULL );


	//insert root node *cameras*
	hCamera = m_rtSceneContents.InsertItem( uiMask, "Cameras", 1, 1, 0, 0, 0, NULL, NULL );

	for( int i = 0; i < pScn->GetCamerasCount(); i++ )
	{
		CCAMERA2* cam = pScn->GetCameraByIndex( i );

		hTemp = m_rtSceneContents.InsertItem( uiMask, cam->name.data(), (cam->visible ? 5 : 11), (cam->visible ? 5 : 11), 0, 0, 0, hCamera, TVI_SORT );

		// add binded object nodes 
		//loop throught binds, looking for children objects
		
		// add misc binds
		hBinds = m_rtSceneContents.InsertItem( uiMask, "Binds", 22, 22, 0, 0, 0, hTemp, TVI_SORT );

		ListStrVec_iter iter;

		StrVec supported_types;
		supported_types.push_back( "Background" );
		supported_types.push_back( "Action" );
		supported_types.push_back( "Light" );
		supported_types.push_back( "LayerGeom" );
		supported_types.push_back( "Floor" );
		supported_types.push_back( "CameraSelect" );
		int	image_idx[] = { 21, 17, 9, 18, 14, 19 };

		for( unsigned int u = 0; u < supported_types.size() ; u++ )
		{
			iter = pScn->GetBinds()->begin();

			while( iter != pScn->GetBinds()->end() )
			{
				if( (*iter)[2] == "Camera" && (*iter)[1] == pScn->GetCameraByIndex( i )->name )
				{
					if( (*iter)[4] == supported_types[u] )
						hTemp2 = m_rtSceneContents.InsertItem( uiMask, _T( (*iter)[3].data() ), image_idx[u], image_idx[u], 0, 0, 0, hTemp, NULL );	

					//layer geometry attached to backs
					if( supported_types[u] == "Background" )
					{
						ListStrVec_iter iter2 = pScn->GetBinds()->begin();
						
						while( iter2 != pScn->GetBinds()->end() )
						{
							if( (*iter2)[2] == "Background" && (*iter2)[1] == (*iter)[3] && (*iter2)[4] == "LayerGeom" )
								m_rtSceneContents.InsertItem( uiMask, _T( (*iter2)[3].data() ), 18, 18, 0, 0, 0, hTemp2, TVI_SORT );	

							iter2++;
						}
					}
				}

				iter++;
			}
		} // for( unsigned int u = 0; u < supported_types.size() ; u++ )


		iter = pScn->GetBinds()->begin();

		while( iter != pScn->GetBinds()->end() )
		{
			if( (*iter)[2] == "Camera" && (*iter)[1] == pScn->GetCameraByIndex( i )->name )
			{
				bool supp = false;
				unsigned int u = 0;

				for( u = 0; u < supported_types.size() ; u++ )
				{
					if( (*iter)[4] == supported_types[u] )
					{
						supp = true;
						break;
					}
				} // for( unsigned int u = 0; u < supported_types.size() ; u++ )				

				if( supp == false )
					m_rtSceneContents.InsertItem( uiMask, _T( ((*iter)[4] + "." + (*iter)[3]).data() ), 3, 3, 0, 0, 0, hBinds, TVI_SORT );
			}

			iter++;
		}
	}

	
	//insert root node *lights*
	hLight = m_rtSceneContents.InsertItem( uiMask, "Lights", 1, 1, 0, 0, 0, NULL, NULL );

	for( int i = 0; i < pScn->GetLightsCount(); i++ )
	{
		CLIGHT* light = pScn->GetLightByIndex( i );

		hTemp = m_rtSceneContents.InsertItem( uiMask, light->name.data(), ( light->visible ? 9 : 12 ), ( light->visible ? 9 : 12 ), 0, 0, 0, hLight, TVI_SORT );
	}


	//insert root node *nulls*
	hNull = m_rtSceneContents.InsertItem( uiMask, "Nulls", 1, 1, 0, 0, 0, NULL, NULL );
	
	if( nulls_mesh )
	{
		for( int i = 0; i < nulls_mesh->GetNullsCount(); i++ )
		{
			// null
			CNULL* pNull = nulls_mesh->GetNullByIndex( i );
			hTemp = m_rtSceneContents.InsertItem( uiMask, pNull->id.data(), 10, 10, 0, 0, 0, hNull, TVI_SORT );
		}
	}


	//insert root node *meshes*
	hMesh = m_rtSceneContents.InsertItem( uiMask, "Meshes", 1, 1, 0, 0, 0, NULL, NULL );

	if( meshlist )
	{
		for( int i = 0; i < meshlist->GetMeshesCount(); i++ )
		{
			if( meshlist->GetMeshByIndex( i )->GetName() == "Nulls" )
				continue;

			bTemp = meshlist->GetMeshByIndex( i )->GetVisible();

			hTemp = m_rtSceneContents.InsertItem( uiMask, meshlist->GetMeshByIndex( i )->GetName().data(), (bTemp ? 0 : 4), (bTemp ? 0 : 4), 0, 0, 0, hMesh, TVI_SORT );

			/*
			//insert vertecies color
			DWORD col = meshlist->GetMeshByIndex( i )->GetVColor();

			int a, r, g, b;
			GetARGBFromDWORD( a, r, g, b, col );

			sprintf( szTemp, "VColor:  a:%i  r:%i  g:%i  b:%i", a, r, g, b );
			m_rtSceneContents.InsertItem( uiMask, szTemp, 3, 3, 0, 0, 0, hTemp, TVI_SORT );
			*/

			// null nodes
			for( int j = 0; j < meshlist->GetMeshByIndex( i )->GetNullsCount(); j++ )
			{
				CNULL* null = meshlist->GetMeshByIndex( i )->GetNullByIndex( j );

				if( null )
					m_rtSceneContents.InsertItem( uiMask, null->id.data(), 10, 10, 0, 0, 0, hTemp, TVI_SORT );
			}
		}
	}
	
	
	//insert root node *textures*
	hTexture = m_rtSceneContents.InsertItem( uiMask, "Textures", 1, 1, 0, 0, 0, NULL, NULL );

    StrVec*	tex_vec = pScn->GetTextures();
    StrVec	tex_vec2 = *tex_vec;

    for( unsigned int i = 0; i < tex_vec->size(); i++ )
        m_rtSceneContents.InsertItem( uiMask, _T( tex_vec2[i].data() ), 21, 21, 0, 0, 0, hTexture, TVI_SORT );
	

	//curves
	hCurves = m_rtSceneContents.InsertItem( uiMask, "Curves", 1, 1, 0, 0, 0, NULL, NULL );

	for( int i = 0; i < pScn->GetCurvesCount(); i++ )
	{
		CurveItem* crv = pScn->GetCurveByIndex(i);

		hTemp = m_rtSceneContents.InsertItem( uiMask, crv->id.data(), 26, 26, 0, 0, 0, hCurves, NULL );
	
		for( UINT j = 0; j < pScn->GetCurveByIndex(i)->nulls.size(); j++ )
		{
			str_t nl_id = pScn->GetCurveByIndex(i)->nulls[j];
			m_rtSceneContents.InsertItem( uiMask, nl_id.data(), 10, 10, 0, 0, 0, hTemp, NULL );
		}
	}

	//custom objects
	hCustObj = m_rtSceneContents.InsertItem( uiMask, _T( "Custom objects" ), 22, 22, 0, 0, 0, NULL, NULL );

	StrVec* m_CustomObjects = ((CSceneFile*)pBaseScene)->GetCustObjList();

	for( unsigned int i = 0; i < m_CustomObjects->size(); i++ )
		hTemp = m_rtSceneContents.InsertItem( uiMask, _T( (*m_CustomObjects)[i].data() ), 3, 3, 0, 0, 0, hCustObj, TVI_SORT );



	//custom types
	hCustTypes = m_rtSceneContents.InsertItem( uiMask, _T( "Custom types" ), 22, 22, 0, 0, 0, NULL, NULL );

	StrVec* m_CustomTypes = ((CSceneFile*)pBaseScene)->GetCustTypesList();

	for( unsigned int i = 0; i < m_CustomTypes->size(); i++ )
		m_rtSceneContents.InsertItem( uiMask, _T( (*m_CustomTypes)[i].data() ), 3, 3, 0, 0, 0, hCustTypes, TVI_SORT );



	m_rtSceneContents.Expand( hInfo, TVE_EXPAND );

	if( !m_bSCNLoaded )
	{
		/* cleanup */
		firstSelFrame		= -1;
		lastSelFrame		= -1;
		m_iStartingFrame	= 0;

		m_cmbCameras.ResetContent();
		m_cmbCameras.AddString( DEFAULT_CAMERA_ID );
		m_cmbCameras.SetCurSel( 0 );

		m_eFrameCounter.SetWindowText( "0" );

		m_sliderAnim.SetRange( 0, 0, 0 );
		m_sliderAnim.SetPos( 0 );
		m_sliderAnim.ClearSel( true );
		/* cleanup END */

		//fill cameras combobox
		for( int i = 0; i < ((CSceneFile*)pBaseScene)->GetCamerasCount(); i++ )
		{
			CCAMERA2* cam = ((CSceneFile*)pBaseScene)->GetCameraByIndex( i );

			m_cmbCameras.AddString( cam->name.data() );
		}
	}

	if( !m_bSCNLoaded )
		m_bSCNLoaded = true;
}

str_t CCookieMachineDlg::GetTooltip( int ctrlID )
{
	str_t res = "";

	IStrMap_iter item = m_ToolbarTooltips.find( ctrlID );

	if( item != m_ToolbarTooltips.end() )
		res = m_ToolbarTooltips[ctrlID];

	return res;
}

void CCookieMachineDlg::CreateToolbar()
{
	//CREATE TOOLBAR IMAGES
	CBitmap		bitmap;
	vector<int> images;
	vector<int> imagesDis;
	vector<int> imagesHot;

	//----norm----
	images.push_back( IDB_TB_FILEOPEN );
	images.push_back( IDB_TB_FILESAVE );
	images.push_back( IDB_TB_FILESAVEALL );
	images.push_back( IDB_TB_FILECONV_2EFN );
	images.push_back( IDB_TB_FILECONV_2MSH );
	images.push_back( IDB_TB_FILECONV_2SCN );
	images.push_back( IDB_TB_UNDO );
	images.push_back( IDB_TB_REDO );
	images.push_back( IDB_TB_HIDEALL );
	images.push_back( IDB_TB_SHOWALL );
	images.push_back( IDB_TB_AXIS_X );
	images.push_back( IDB_TB_AXIS_Y );
	images.push_back( IDB_TB_AXIS_Z );
	images.push_back( IDB_TB_AXIS_XY );
	images.push_back( IDB_TB_AXIS_XZ );
	images.push_back( IDB_TB_AXIS_YZ );
	images.push_back( IDB_TB_OBJPROP );
	images.push_back( IDB_TB_SETTINGS );
	images.push_back( IDB_TB_RUN );
	images.push_back( IDB_TB_RUNCURR );
	images.push_back( IDB_TB_HELP );
	images.push_back( IDB_TB_SCRIPTS );
	images.push_back( IDB_TB_BINDS );
	images.push_back( IDB_TB_FLOORS );
	images.push_back( IDB_TB_LOGS );
	images.push_back( IDB_TB_TEXT_RESOURCES );
	images.push_back( IDB_TB_ENGINE_CONSOLE );
	images.push_back( IDB_TB_RELOAD_SCENE );
	images.push_back( IDB_TB_RELOAD_MESHES );
	images.push_back( IDB_TB_STOP );
	
	//----dis----
	imagesDis.push_back( IDB_TB_FILEOPEN_DIS );
	imagesDis.push_back( IDB_TB_FILESAVE_DIS );
	imagesDis.push_back( IDB_TB_FILESAVEALL_DIS );
	imagesDis.push_back( IDB_TB_FILECONV_2EFN_DIS );
	imagesDis.push_back( IDB_TB_FILECONV_2MSH_DIS );
	imagesDis.push_back( IDB_TB_FILECONV_2SCN_DIS );
	imagesDis.push_back( IDB_TB_UNDO_DIS );
	imagesDis.push_back( IDB_TB_REDO_DIS );
	imagesDis.push_back( IDB_TB_HIDEALL );
	imagesDis.push_back( IDB_TB_SHOWALL );
	imagesDis.push_back( IDB_TB_AXIS_X );
	imagesDis.push_back( IDB_TB_AXIS_Y );
	imagesDis.push_back( IDB_TB_AXIS_Z );
	imagesDis.push_back( IDB_TB_AXIS_XY );
	imagesDis.push_back( IDB_TB_AXIS_XZ );
	imagesDis.push_back( IDB_TB_AXIS_YZ );
	imagesDis.push_back( IDB_TB_OBJPROP_DIS );
	imagesDis.push_back( IDB_TB_SETTINGS_DIS );
	imagesDis.push_back( IDB_TB_RUN_DIS );
	imagesDis.push_back( IDB_TB_RUNCURR_DIS );
	imagesDis.push_back( IDB_TB_HELP_DIS );
	imagesDis.push_back( IDB_TB_SCRIPTS_DIS );
	imagesDis.push_back( IDB_TB_BINDS_DIS );
	imagesDis.push_back( IDB_TB_FLOORS_DIS );
	imagesDis.push_back( IDB_TB_LOGS_DIS );
	imagesDis.push_back( IDB_TB_TEXT_RESOURCES_DIS );
	imagesDis.push_back( IDB_TB_ENGINE_CONSOLE );
	imagesDis.push_back( IDB_TB_RELOAD_SCENE );
	imagesDis.push_back( IDB_TB_RELOAD_MESHES );
	imagesDis.push_back( IDB_TB_STOP_DIS );

	//----hot----
	imagesHot.push_back( IDB_TB_FILEOPEN_HOT );
	imagesHot.push_back( IDB_TB_FILESAVE_HOT );
	imagesHot.push_back( IDB_TB_FILESAVEALL_HOT );
	imagesHot.push_back( IDB_TB_FILECONV_2EFN_HOT );
	imagesHot.push_back( IDB_TB_FILECONV_2MSH_HOT );
	imagesHot.push_back( IDB_TB_FILECONV_2SCN_HOT );
	imagesHot.push_back( IDB_TB_UNDO_HOT );
	imagesHot.push_back( IDB_TB_REDO_HOT );
	imagesHot.push_back( IDB_TB_HIDEALL );
	imagesHot.push_back( IDB_TB_SHOWALL );
	imagesHot.push_back( IDB_TB_AXIS_X );
	imagesHot.push_back( IDB_TB_AXIS_Y );
	imagesHot.push_back( IDB_TB_AXIS_Z );
	imagesHot.push_back( IDB_TB_AXIS_XY );
	imagesHot.push_back( IDB_TB_AXIS_XZ );
	imagesHot.push_back( IDB_TB_AXIS_YZ );
	imagesHot.push_back( IDB_TB_OBJPROP_HOT );
	imagesHot.push_back( IDB_TB_SETTINGS_HOT );
	imagesHot.push_back( IDB_TB_RUN_HOT );
	imagesHot.push_back( IDB_TB_RUNCURR_HOT );
	imagesHot.push_back( IDB_TB_HELP_HOT );
	imagesHot.push_back( IDB_TB_SCRIPTS_HOT );
	imagesHot.push_back( IDB_TB_BINDS_HOT );
	imagesHot.push_back( IDB_TB_FLOORS_HOT );
	imagesHot.push_back( IDB_TB_LOGS_HOT );
	imagesHot.push_back( IDB_TB_TEXT_RESOURCES_HOT );
	imagesHot.push_back( IDB_TB_ENGINE_CONSOLE );
	imagesHot.push_back( IDB_TB_RELOAD_SCENE );
	imagesHot.push_back( IDB_TB_RELOAD_MESHES );
	imagesHot.push_back( IDB_TB_STOP_HOT );

	//----norm----
	m_pToolbarImageList = new CImageList();
	m_pToolbarImageList->Create( 16, 16, ILC_COLOR24 | ILC_MASK, 0, 4 );
	
	for( UINT i = 0; i < images.size(); i++ )
	{
		bitmap.LoadBitmap( images[i] );
		m_pToolbarImageList->Add( &bitmap, RGB(255, 255, 0) );
		bitmap.DeleteObject();
	}


	//----dis----
	m_pToolbarImageListDis = new CImageList();
	m_pToolbarImageListDis->Create( 16, 16, ILC_COLOR24 | ILC_MASK, 0, 4 );

	for( UINT i = 0; i < imagesDis.size(); i++ )
	{
		bitmap.LoadBitmap( imagesDis[i] );
		m_pToolbarImageListDis->Add( &bitmap, RGB(255, 255, 0) );
		bitmap.DeleteObject();
	}


	//----hot----
	m_pToolbarImageListHot = new CImageList();
	m_pToolbarImageListHot->Create( 16, 16, ILC_COLOR24 | ILC_MASK, 0, 4 );

	for( UINT i = 0; i < imagesHot.size(); i++ )
	{
		bitmap.LoadBitmap( imagesHot[i] );
		m_pToolbarImageListHot->Add( &bitmap, RGB(255, 255, 0) );
		bitmap.DeleteObject();
	}


	//CREATE TOOLBAR BUTTONS
	TBBUTTON btns[32];

	btns[0].fsState = TBSTATE_ENABLED;
	btns[0].fsStyle = TBSTYLE_SEP;
	btns[0].iBitmap = 0;
	btns[0].idCommand = 0;
	btns[0].iString	= -1;

	btns[1].fsState = TBSTATE_ENABLED;
	btns[1].fsStyle = TBSTYLE_BUTTON;
	btns[1].iBitmap = 0;
	btns[1].idCommand = IDT_FILEOPEN;
	btns[1].iString	= -1;
	m_ToolbarTooltips[IDT_FILEOPEN] = "Open file     Ctrl+O";

	btns[2].fsState = TBSTATE_ENABLED;
	btns[2].fsStyle = TBSTYLE_BUTTON;
	btns[2].iBitmap = 1;
	btns[2].idCommand = IDT_FILESAVE;
	btns[2].iString	= -1;
	m_ToolbarTooltips[IDT_FILESAVE] = "Save file     Ctrl+S";

	btns[3].fsState = TBSTATE_ENABLED;
	btns[3].fsStyle = TBSTYLE_BUTTON;
	btns[3].iBitmap = 2;
	btns[3].idCommand = IDT_FILESAVEALL;
	btns[3].iString	= -1;
	m_ToolbarTooltips[IDT_FILESAVEALL] = "Save all";

	btns[4].fsState = TBSTATE_ENABLED;
	btns[4].fsStyle = TBSTYLE_SEP;
	btns[4].iBitmap = 0;
	btns[4].idCommand = 0;
	btns[4].iString	= -1;

	btns[5].fsState = TBSTATE_ENABLED;
	btns[5].fsStyle = TBSTYLE_BUTTON;
	btns[5].iBitmap = 3;
	btns[5].idCommand = IDT_FILECONV_2EFN;
	btns[5].iString	= -1;
	m_ToolbarTooltips[IDT_FILECONV_2EFN] = "Convert to EFN";

	btns[6].fsState = TBSTATE_ENABLED;
	btns[6].fsStyle = TBSTYLE_BUTTON;
	btns[6].iBitmap = 4;
	btns[6].idCommand = IDT_FILECONV_2MSH;
	btns[6].iString	= -1;
	m_ToolbarTooltips[IDT_FILECONV_2MSH] = "Convert to EFN meshlist";

	btns[7].fsState = TBSTATE_ENABLED;
	btns[7].fsStyle = TBSTYLE_BUTTON;
	btns[7].iBitmap = 5;
	btns[7].idCommand = IDT_FILECONV_2SCN;
	btns[7].iString	= -1;
	m_ToolbarTooltips[IDT_FILECONV_2SCN] = "Convert to SCN";

	btns[8].fsState = TBSTATE_ENABLED;
	btns[8].fsStyle = TBSTYLE_SEP;
	btns[8].iBitmap = 0;
	btns[8].idCommand = 0;
	btns[8].iString	= -1;

	btns[9].fsState = TBSTATE_ENABLED;
	btns[9].fsStyle = TBSTYLE_BUTTON | TBSTYLE_DROPDOWN;
	btns[9].iBitmap = 6;
	btns[9].idCommand = IDT_UNDO;
	btns[9].iString	= -1;
	m_ToolbarTooltips[IDT_UNDO] = "Undo action";

	btns[10].fsState = TBSTATE_ENABLED;
	btns[10].fsStyle = TBSTYLE_BUTTON | TBSTYLE_DROPDOWN;
	btns[10].iBitmap = 7;
	btns[10].idCommand = IDT_REDO;
	btns[10].iString = -1;
	m_ToolbarTooltips[IDT_REDO] = "Redo action";

	btns[11].fsState = TBSTATE_ENABLED;
	btns[11].fsStyle = TBSTYLE_SEP;
	btns[11].iBitmap = 0;
	btns[11].idCommand = 0;
	btns[11].iString = -1;

	btns[12].fsState = TBSTATE_ENABLED;
	btns[12].fsStyle = TBSTYLE_BUTTON | BTNS_SHOWTEXT;
	btns[12].iBitmap = 8;
	btns[12].idCommand = IDT_SHOWALL;
	btns[12].iString = 0;
	m_ToolbarTooltips[IDT_SHOWALL] = "Show all objects     Shift+H";

	btns[13].fsState = TBSTATE_ENABLED;
	btns[13].fsStyle = TBSTYLE_BUTTON | BTNS_SHOWTEXT;
	btns[13].iBitmap = 9;
	btns[13].idCommand = IDT_HIDEALL;
	btns[13].iString = 1;
	m_ToolbarTooltips[IDT_HIDEALL] = "Hide all objects     Ctrl+H";

	btns[14].fsState = TBSTATE_ENABLED;
	btns[14].fsStyle = TBSTYLE_BUTTON | BTNS_SHOWTEXT | TBSTYLE_DROPDOWN;
	btns[14].iBitmap = 10;
	btns[14].idCommand = IDT_AXIS_ALIGN;
	btns[14].iString = 2;
	m_ToolbarTooltips[IDT_AXIS_ALIGN] = "Axis alignment";

	btns[15].fsState = TBSTATE_ENABLED;
	btns[15].fsStyle = TBSTYLE_BUTTON | BTNS_SHOWTEXT | TBSTYLE_DROPDOWN;
	btns[15].iBitmap = 23;
	btns[15].idCommand = IDT_FLOORS;
	btns[15].iString = 3;
	m_ToolbarTooltips[IDT_FLOORS] = "Floor tools";

	btns[16].fsState = TBSTATE_ENABLED;
	btns[16].fsStyle = TBSTYLE_SEP;
	btns[16].iBitmap = 0;
	btns[16].idCommand = 0;
	btns[16].iString = -1;

	btns[17].fsState = TBSTATE_ENABLED;
	btns[17].fsStyle = TBSTYLE_BUTTON;
	btns[17].iBitmap = 25;
	btns[17].idCommand = IDT_TEXT_RES;
	btns[17].iString = -1;
	m_ToolbarTooltips[IDT_TEXT_RES] = "Text resources editor     Ctrl+E";

	btns[18].fsState = TBSTATE_ENABLED;
	btns[18].fsStyle = TBSTYLE_BUTTON;
	btns[18].iBitmap = 22;
	btns[18].idCommand = IDT_BINDS;
	btns[18].iString = -1;
	m_ToolbarTooltips[IDT_BINDS] = "Binds editor     Ctrl+W";

	btns[19].fsState = TBSTATE_ENABLED;
	btns[19].fsStyle = TBSTYLE_BUTTON;
	btns[19].iBitmap = 21;
	btns[19].idCommand = IDT_SCRIPTS;
	btns[19].iString = -1;
	m_ToolbarTooltips[IDT_SCRIPTS] = "Scripts editor     Ctrl+Q";

	btns[20].fsState = TBSTATE_ENABLED;
	btns[20].fsStyle = TBSTYLE_BUTTON;
	btns[20].iBitmap = 16;
	btns[20].idCommand = IDT_OBJ_PROPERTIES;
	btns[20].iString = -1;
	m_ToolbarTooltips[IDT_OBJ_PROPERTIES] = "Show objects properties     Tab";

	btns[21].fsState = TBSTATE_ENABLED;
	btns[21].fsStyle = TBSTYLE_BUTTON;
	btns[21].iBitmap = 17;
	btns[21].idCommand = IDT_SETTINGS;
	btns[21].iString = -1;
	m_ToolbarTooltips[IDT_SETTINGS] = "Settings     Ctrl+Enter";

	btns[22].fsState = TBSTATE_ENABLED;
	btns[22].fsStyle = TBSTYLE_SEP;
	btns[22].iBitmap = 0;
	btns[22].idCommand = 0;
	btns[22].iString = -1;

	btns[23].fsState = TBSTATE_ENABLED;
	btns[23].fsStyle = TBSTYLE_BUTTON;
	btns[23].iBitmap = 18;
	btns[23].idCommand = IDT_RUN_ENGINE;
	btns[23].iString = -1;
	m_ToolbarTooltips[IDT_RUN_ENGINE] = "Run engine     F5";

	btns[24].fsState = TBSTATE_ENABLED;
	btns[24].fsStyle = TBSTYLE_BUTTON;
	btns[24].iBitmap = 19;
	btns[24].idCommand = IDT_RUN_ENGINE_CURR;
	btns[24].iString = -1;
	m_ToolbarTooltips[IDT_RUN_ENGINE_CURR] = "Run engine from selected null     F6";

	btns[25].fsState = TBSTATE_ENABLED;
	btns[25].fsStyle = TBSTYLE_BUTTON;
	btns[25].iBitmap = 29;
	btns[25].idCommand = IDT_STOP_ENGINE;
	btns[25].iString = -1;
	m_ToolbarTooltips[IDT_STOP_ENGINE] = "Stop engine     Shift+F5";

	btns[26].fsState = TBSTATE_ENABLED;
	btns[26].fsStyle = TBSTYLE_BUTTON;
	btns[26].iBitmap = 24;
	btns[26].idCommand = IDT_ENGINE_LOGS;
	btns[26].iString = -1;
	m_ToolbarTooltips[IDT_ENGINE_LOGS] = "Engine logs";

	btns[27].fsState = TBSTATE_ENABLED;
	btns[27].fsStyle = TBSTYLE_BUTTON;
	btns[27].iBitmap = 26;
	btns[27].idCommand = IDT_ENGINE_CONSOLE;
	btns[27].iString = -1;
	m_ToolbarTooltips[IDT_ENGINE_CONSOLE] = "Engine console     ~";

	btns[28].fsState = TBSTATE_ENABLED;
	btns[28].fsStyle = TBSTYLE_BUTTON;
	btns[28].iBitmap = 27;
	btns[28].idCommand = IDT_RELOAD_SCENE;
	btns[28].iString = -1;
	m_ToolbarTooltips[IDT_RELOAD_SCENE] = "Reload scene data in engine";

	btns[29].fsState = TBSTATE_ENABLED;
	btns[29].fsStyle = TBSTYLE_BUTTON;
	btns[29].iBitmap = 28;
	btns[29].idCommand = IDT_RELOAD_MESHES;
	btns[29].iString = -1;
	m_ToolbarTooltips[IDT_RELOAD_MESHES] = "Reload mesh data in engine";

	btns[30].fsState = TBSTATE_ENABLED;
	btns[30].fsStyle = TBSTYLE_SEP;
	btns[30].iBitmap = 0;
	btns[30].idCommand = 0;
	btns[30].iString = -1;

	btns[31].fsState = TBSTATE_ENABLED;
	btns[31].fsStyle = TBSTYLE_BUTTON;
	btns[31].iBitmap = 20;
	btns[31].idCommand = IDT_HELP;
	btns[31].iString = -1;
	m_ToolbarTooltips[IDT_HELP] = "Help";

	//CREATE TOOLBAR
	CRect rect( 0, 0, 200, 50 );
	DWORD dwStyle = TBSTYLE_TOOLTIPS | WS_CHILD | WS_VISIBLE | CCS_NODIVIDER | CCS_ADJUSTABLE | TBSTYLE_ALTDRAG | TBSTYLE_WRAPABLE | TBSTYLE_FLAT | TBSTYLE_LIST | TBSTYLE_TRANSPARENT;

	m_pToolbar = new CToolBarCtrl;
	m_pToolbar->Create( dwStyle, rect, this, ID_TOOLBAR );

	CSize sz( 24, 24 );
	m_pToolbar->SetButtonSize( sz );
	m_pToolbar->SetImageList( m_pToolbarImageList );
	m_pToolbar->SetDisabledImageList( m_pToolbarImageListDis );
	m_pToolbar->SetHotImageList( m_pToolbarImageListHot );
	m_pToolbar->AddButtons( 32, btns );
	m_pToolbar->AddStrings( "Show all\0Hide all\0axis\0floors\0" );

	m_pToolbar->SetExtendedStyle( TBSTYLE_EX_DRAWDDARROWS | TBSTYLE_EX_HIDECLIPPEDBUTTONS | TBSTYLE_EX_MIXEDBUTTONS );
}

void CCookieMachineDlg::SetToolbarButtonImg( UINT id, int iImage )
{
	if( !m_pToolbar )
		return;

	TBBUTTONINFO tbi;

	tbi.dwMask = TBIF_IMAGE;
	tbi.cbSize = sizeof( TBBUTTONINFO );
	m_pToolbar->GetButtonInfo( id, &tbi );

	tbi.iImage = iImage;

	m_pToolbar->SetButtonInfo( id, &tbi );

	m_pToolbar->Invalidate( true );
}

void CCookieMachineDlg::EnableToolbarButton( UINT id, bool bValue )
{
	if( !m_pToolbar )
		return;

	TBBUTTONINFO tbi;

	tbi.dwMask = TBIF_STATE;
	tbi.cbSize = sizeof( TBBUTTONINFO );
	m_pToolbar->GetButtonInfo( id, &tbi );

	if( bValue )
		tbi.fsState |= TBSTATE_ENABLED;
	else
	{
		if( tbi.fsState & TBSTATE_ENABLED )
			tbi.fsState ^= TBSTATE_ENABLED;
	}

	m_pToolbar->SetButtonInfo( id, &tbi );
}


void CCookieMachineDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	return;

	str_t func_decl_str = "void CCookieMachineDlg::OnDestroy()";

	try
	{
		CDialog::OnDestroy();

		m_bClosing = true;

		if( m_hLexDll )
		{
			FreeLibrary( m_hLexDll );
			m_hLexDll = NULL;
		}

		CloseCurrentFile();

		SAFE_RELEASE( ppMesh );
		SAFE_RELEASE( ppCylinder );

		SAFE_DELETE( m_pImageList );
		SAFE_DELETE( m_dlgAbout );

		SAFE_DELETE( pLightBulb );
		SAFE_DELETE( pLightCone );
		SAFE_DELETE( pCamBox );
		SAFE_DELETE( pNullIco );

		D3DCleanup();

		SAFE_DELETE( m_pToolbar );
		SAFE_DELETE( m_pToolbarImageList );
		SAFE_DELETE( m_pToolbarImageListDis );
		SAFE_DELETE( m_pToolbarImageListHot );

		if( m_dlgTextResourcesDlg )
		{
			m_dlgTextResourcesDlg->DestroyWindow();
			SAFE_DELETE( m_dlgTextResourcesDlg );
		}

		if( m_pPropCustomParamDlg )
		{
			m_pPropCustomParamDlg->DestroyWindow();
			SAFE_DELETE( m_pPropCustomParamDlg );
		}

		if( m_pPropNullDlg )
		{
			m_pPropNullDlg->DestroyWindow();
			SAFE_DELETE( m_pPropNullDlg );
		}

		if( m_pPropCameraDlg )
		{
			m_pPropCameraDlg->DestroyWindow();
			SAFE_DELETE( m_pPropCameraDlg );
		}

		if( m_pPropLightDlg )
		{
			m_pPropLightDlg->DestroyWindow();
			SAFE_DELETE( m_pPropLightDlg );
		}

		SAFE_DELETE( m_StatusBar );

		m_dlgTesselatedMesh->DestroyWindow();
		SAFE_DELETE( m_dlgTesselatedMesh );
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );

		throw;
	}
}

void CCookieMachineDlg::OnTimer(UINT nIDEvent) 
{
	if( m_bClosing )
		return;

	str_t func_decl_str = "void CCookieMachineDlg::OnTimer(UINT nIDEvent)";

	try
	{
		MEMORYSTATUSEX memStatus;
		CString        tmp;

		switch( nIDEvent ) 
		{ 
			case FILE_SAVED_TIMER:
				m_StatusBar->SetPaneText( 5, "Ready.", true );
				KillTimer(ID_DBL_CLICK_TIMER);
			break;

			case RENDERING_TIMER:
				//ProcessMessages();
				//Render();
				//::ValidateRect(m_hRenderWnd, NULL);
			break;

			case MEMORY_STAT_TIMER:
				memStatus.dwLength = sizeof (memStatus);
				GlobalMemoryStatusEx(&memStatus);

				//int mbs = (long)memStatus.ullTotalPhys / 1024 / 1024;
				//int kbs = ((long)memStatus.ullTotalPhys / 1024) - mbs * 1024;
				//tmp.Format("Total.phys.: %d.%d KB", mbs, kbs);
				//m_StatusBar->SetPaneText(1, tmp, true);

				//tmp = "";

				//int mbs2 = (long)memStatus.ullTotalVirtual / 1024 / 1024;
				//int kbs2 = ((long)memStatus.ullTotalVirtual / 1024) - mbs2 * 1024;
				//tmp.Format("Total.virt.: %d.%d KB", mbs2, kbs2);
				//m_StatusBar->SetPaneText(2, tmp, true);

				tmp = "";

				int mbs3 = (long)memStatus.ullAvailPhys / 1024 / 1024;
				int kbs3 = ((long)memStatus.ullAvailPhys / 1024) - mbs3 * 1024;
				tmp.Format("Avail.phys.: %d.%d KB", mbs3, kbs3);
				m_StatusBar->SetPaneText(1, tmp, true);

				tmp = "";

				int mbs4 = (long)memStatus.ullAvailVirtual / 1024 / 1024;
				int kbs4 = ((long)memStatus.ullAvailVirtual / 1024) - mbs4 * 1024;
				tmp.Format("Avail.virt.: %d.%d KB", mbs4, kbs4);
				m_StatusBar->SetPaneText(2, tmp, true);
			break;
		};	
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}

	CDialog::OnTimer(nIDEvent);
}

void CCookieMachineDlg::OnSize(UINT nType, int cx, int cy) 
{
	str_t func_decl_str = "void CCookieMachineDlg::OnSize(UINT nType, int cx, int cy)";

	try
	{
		//UpdateUISizing();

		CDialog::OnSize(nType, cx, cy);
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}
}

void CCookieMachineDlg::ResizeRenderWnd()
{
	return;

	CStatic* render_wnd = NULL;

	render_wnd	= (CStatic*)GetDlgItem(IDC_RENDERWND);

	CRect rect;
	GetClientRect(rect);

	if( render_wnd )
	{
		int top = 68, hght_old = rect.bottom - 132;

		if( pBaseScene && pBaseScene->GetType() == FT_SCN )
			hght_old = rect.bottom - 89;

		int lft = m_iSplitterX + 8, wdth_old = rect.right - (m_iSplitterX + 13);

		if( m_bIsObjPropertiesWndVisible )
				wdth_old = rect.right - (m_iSplitterX + 168);

		if( m_appSettings.ViewportPreserve43 )
		{
			int wdth, hght;

			
			hght = ((float)wdth_old / 4.0f) * 3.0f;

			if( (hght_old - hght) > 0 )
			{
				top += (hght_old - hght) / 2;
				hght_old = hght;
			}
			else
			{
				wdth = ((float)hght_old / 3.0f) * 4.0f;
				lft += (wdth_old - wdth) / 2;
				wdth_old = wdth;
			}
		}

		m_fAspect = (float)wdth_old / (float)hght_old;
		render_wnd->MoveWindow( lft, top, wdth_old , hght_old ); 
	}
}

void CCookieMachineDlg::UpdateUISizing()
{
	return;

	str_t func_decl_str = "void CCookieMachineDlg::UpdateUISizing()";

	try
	{
		if( m_bIsAppMinimized )
			return;

		//get dialog dimentions
		CRect rect;

		GetClientRect(rect);

		CStatic*		render_wnd		= NULL;
		CButton*		scene_frame		= NULL;
		CTreeCtrl*		scene_tree		= NULL;
		CStatic*		slider			= NULL;
		CStatic*		toolbar_top		= NULL;
		CStatic*		toolbar_bot		= NULL;
		CSliderCtrl*	anim_slider		= NULL;
		CEdit*			frm_counter		= NULL;
		CButton*		anim_stop		= NULL;
		CButton*		anim_play		= NULL;
		CButton*		anim_loop		= NULL;
		CButton*		anim_trans		= NULL;
		CButton*		set_z_off		= NULL;
		CEdit*			frm_z_off		= NULL;
		CButton*		char_move		= NULL;
		CEdit*			frm_script		= NULL;
		CButton*		frm_script_save	= NULL;
		CStatic*		pObjPropFrame	= NULL;

		render_wnd		= (CStatic*)GetDlgItem( IDC_RENDERWND );
		scene_frame		= (CButton*)GetDlgItem( IDC_CONT_FRAME );
		scene_tree		= (CTreeCtrl*)GetDlgItem( IDC_SCENE_TREE );
		slider			= (CStatic*)GetDlgItem( IDC_SPLITTER ); 
		anim_slider		= (CSliderCtrl*)GetDlgItem( IDC_ANIM_SLIDER );
		frm_counter		= (CEdit*)GetDlgItem( IDC_CURR_FRM );
		anim_stop		= (CButton*)GetDlgItem( IDC_BTN_ANIM_STOP );
		anim_play		= (CButton*)GetDlgItem( IDC_BTN_ANIM_PLAY );
		anim_loop		= (CButton*)GetDlgItem( IDC_CHK_LOOP_ANIM );
		frm_z_off		= (CEdit*)GetDlgItem( IDC_CURR_Z_OFF );
		set_z_off		= (CButton*)GetDlgItem( IDC_BUTN_SET_Z_OFF );
		char_move		= (CButton*)GetDlgItem( IDC_CHK_MOVE_CHAR );
		anim_trans		= (CButton*)GetDlgItem( IDC_CHK_LOOP_TRANSITION );
		toolbar_top		= (CStatic*)GetDlgItem( IDC_FRM_TOOLBAR_TOP );
		toolbar_bot		= (CStatic*)GetDlgItem( IDC_FRM_TOOLBAR_BOTTOM );
		pObjPropFrame	= (CStatic*)GetDlgItem( IDC_OBJPROP_WND_FRAME );

		if( toolbar_top ) 
			toolbar_top->MoveWindow( m_iSplitterX + 8, 33, rect.right - 1, 2 );

		if( toolbar_bot ) 
			toolbar_bot->MoveWindow( m_iSplitterX, 60, rect.right - 1, 2 );

		if( m_StatusBar ) 
			m_StatusBar->MoveWindow( 0, rect.bottom - 20, rect.right, 20 );

		if( scene_tree ) 
		{
			if( pBaseScene && pBaseScene->GetType() == FT_SCN )
				scene_tree->MoveWindow( 10, 45, m_iSplitterX - 30, rect.bottom - 76 );
			else
				scene_tree->MoveWindow( 10, 45, m_iSplitterX - 30, rect.bottom - 119 );
		}

		if( scene_frame )
		{
			if( pBaseScene && pBaseScene->GetType() == FT_SCN )
				scene_frame->MoveWindow( 0, 45 - 18, m_iSplitterX - 10, rect.bottom - 49 );
			else
				scene_frame->MoveWindow( 0, 45 - 18, m_iSplitterX - 10, rect.bottom - 92 );
		}

		if( render_wnd )
		{
			int top = 68, hght_old = rect.bottom - 132;

			if( pBaseScene && pBaseScene->GetType() == FT_SCN )
				hght_old = rect.bottom - 89;

			int lft = m_iSplitterX + 8, wdth_old = rect.right - (m_iSplitterX + 13);

			if( m_bIsObjPropertiesWndVisible )
					wdth_old = rect.right - (m_iSplitterX + 168);

			if( m_appSettings.ViewportPreserve43 )
			{
				int wdth, hght;

				
				hght = ((float)wdth_old / 4.0f) * 3.0f;

				if( (hght_old - hght) > 0 )
				{
					top += (hght_old - hght) / 2;
					hght_old = hght;
				}
				else
				{
					wdth = ((float)hght_old / 3.0f) * 4.0f;
					lft += (wdth_old - wdth) / 2;
					wdth_old = wdth;
				}
			}

			m_fAspect = (float)wdth_old / (float)hght_old;

			render_wnd->MoveWindow( lft, top, wdth_old, hght_old ); 
		}

		if( pObjPropFrame )
		{
			pObjPropFrame->MoveWindow( rect.right - 160, 35, 160, rect.bottom - 66 );

			CRect rcWnd;
			pObjPropFrame->GetWindowRect( &rcWnd );
			ScreenToClient( &rcWnd );


			if( m_pObjPropWnd )
				m_pObjPropWnd->MoveWindow( &rcWnd, true );
		}
		
		if( m_wndSplitter1 )
			m_wndSplitter1.Invalidate( true );

		if( anim_slider ) 
		{
			if( m_bIsObjPropertiesWndVisible )
				anim_slider->MoveWindow( 367, 10 + rect.bottom - 25 - 35 - 4, rect.right - 252 - 270, 32 ); 
			else
				anim_slider->MoveWindow( 367, 10 + rect.bottom - 25 - 35 - 4, rect.right - 367, 32 ); 
		}

		if( frm_z_off )
			frm_z_off->MoveWindow( 235, 5 + rect.bottom - 25 - 32, 50, 20 );

		if( set_z_off )
			set_z_off->MoveWindow( 290, 10 + rect.bottom - 25 - 43, 30, 30 );

		if( frm_counter )
			frm_counter->MoveWindow( 330, 5 + rect.bottom - 25 - 32, 35, 20 );

		if( anim_stop )
			anim_stop->MoveWindow( 35, 10 + rect.bottom - 25 - 43, 30, 30 ); 

		if( anim_play )
			anim_play->MoveWindow( 5, 10 + rect.bottom - 25 - 43, 30, 30 ); 

		if( anim_loop )
			anim_loop->MoveWindow( 75, 6 + rect.bottom - 25 - 40, 50, 18 ); 

		if( char_move )
			char_move->MoveWindow( 75, 21 + rect.bottom - 25 - 40, 50, 18 ); 

		if( anim_trans )
			anim_trans->MoveWindow( 125, 6 + rect.bottom - 25 - 40, 90, 38 ); 

		if( m_bOnInitDialogFinished && m_wndSplitter1 )
		{
			if( pBaseScene && pBaseScene->GetType() == FT_SCN )
				m_wndSplitter1.MoveWindow( m_iSplitterX - 3, 33, 6, rect.bottom - 52 );
			else
				m_wndSplitter1.MoveWindow( m_iSplitterX - 3, 33, 6, rect.bottom - 97 );
		}

		//HandlePossibleSizeChange();
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}
}

void CCookieMachineDlg::CancelAction()
{
	if( !pBaseScene )
		return;

	if( ( pBaseScene->GetType() == FT_EFN_MESHLIST || 
		  pBaseScene->GetType() == FT_SCN ) &&
		  m_bIsVertexEditingModeOn )
	{
		CancelEditingMeshVertices();
	}
	else
	{}
}

void CCookieMachineDlg::CancelEditingMeshVertices()
{
	if( !pBaseScene )
		return;

	CMesh* pMesh = NULL;

	if( pBaseScene->GetType() == FT_EFN_MESHLIST )
	{
		pMesh = ((CEfnMeshlist*)pBaseScene)->GetSelectedMesh();
	}
	else if( pBaseScene->GetType() == FT_SCN )
	{
		CEfnMeshlist* pMeshlist = (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();

		if( !pMeshlist )
			return;

		pMesh = pMeshlist->GetSelectedMesh();
	}
	else
	{}

	if( !pMesh || !m_bIsVertexEditingModeOn )
		return;


	pMesh->CancelVertexEditingMode();

	pMesh->Prepare( g_pd3dDevice );

	m_rtSceneContents.EnableWindow( true );
	m_rtSceneContents.UpdateWindow();
	m_rtSceneContents.SetFocus();

	m_bIsVertexEditingModeOn = false;


	if( pBaseScene->GetType() == FT_EFN_MESHLIST )
	{
		UpdateStatusBarInfoMESHLIST();
	}
	else if( pBaseScene->GetType() == FT_SCN )
	{
		UpdateStatusBarInfoSCN();
	}
}

void CCookieMachineDlg::OnLButtonDown( UINT nFlags, CPoint point )
{
	str_t func_decl_str = "void CCookieMachineDlg::OnLButtonDown( UINT nFlags, CPoint point )";

	try
	{
		UpdateRendererMousePos( &point );

		m_ptTopLeft				= CPoint( g_iMouseX, g_iMouseY );
		m_ptBottomRight			= m_ptTopLeft;
		m_point					= point;

		m_bIsLMouseButtonDown	= true;

		if( !m_ViewMgr.IsCameraOperationInProgress() && ( KEYDOWN('C') || KEYDOWN('Q') || KEYDOWN('W') || KEYDOWN('E') ) )
			m_bIsMouseEditingOpStarted	= true;


		if( KEYDOWN('S') )
			m_ViewMgr.StartCamTranslating();

		CDialog::OnLButtonDown(nFlags, point);
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}
}

void CCookieMachineDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	str_t func_decl_str = "void CCookieMachineDlg::OnLButtonUp(UINT nFlags, CPoint point)";

	try
	{
		m_bIsLMouseButtonDown = false;

		bool bWasTranslatingCam = false;
		bWasTranslatingCam = m_ViewMgr.IsCamTranslating();

		m_ViewMgr.FinishCamTranslating();

		if( m_bIsMouseEditingOpStarted || bWasTranslatingCam )
		{
			m_bIsMouseEditingOpStarted	= false;
			SetCursor( LoadCursor(NULL,IDC_ARROW) );
			m_rtSceneContents.SetFocus();

			return;
		}

		if( !pBaseScene || m_bIsVertexEditingModeOn )
			return;


		//selecting objects in render view
		D3DXVECTOR3		out, scr = D3DXVECTOR3( g_iMouseX, g_iMouseY, 0);
		D3DVIEWPORT9	viewport;
		D3DXMATRIX		world;

		D3DXMatrixIdentity( &world );
		g_pd3dDevice->GetViewport( &viewport );
		D3DXVec3Unproject( &out, &scr, &viewport, &matProj, &m_matView, &world );

		out = out - m_ViewMgr.m_camCurrent->pos;
		D3DXVec3Normalize( &out, &out );

		D3DXMATRIX ident_mtx;
		D3DXMatrixIdentity( &ident_mtx );

		//SetTimer( ID_DBL_CLICK_TIMER, 150, NULL );

		if( abs(m_ptBottomRight.x - m_ptTopLeft.x) > 3 || abs(m_ptBottomRight.y - m_ptTopLeft.y) > 3 )
		{
			CDialog::OnLButtonUp(nFlags, point);
			return;
		}

		D3DXVECTOR3 pos = m_ViewMgr.m_camCurrent->pos;

		switch( m_ViewMgr.m_CurrView )
		{
			case RV_TOP:
				out.x *= -1.0;
				out.z *= -1.0;
				D3DXVec3Scale( &out, &out, -1.0f );

				pos.y = 1000;
				break;

			case RV_FRONT:
				out.x *= -1.0;
				out.y *= -1.0;
				D3DXVec3Scale( &out, &out, -1.0f );

				pos.z = -1000;
				break;

			case RV_RIGHT:
				out.z *= -1.0;
				out.y *= -1.0;
				D3DXVec3Scale( &out, &out, -1.0f );

				pos.x = 1000;
				break;
		}

		if( pBaseScene->GetType() == FT_EFN_MESHLIST )
		{
			enum OBJECT_TYPES objType;
			str_t strObjID;

			((CEfnMeshlist*)pBaseScene)->IntersectByRay( pos, out, ident_mtx, objType, strObjID );

			if( objType == OBJ_MESH )
			{
				HandleMeshSelection( strObjID );
			}
			else if( objType == OBJ_NULL )
			{
				HTREEITEM hNulls	= m_rtSceneContents.FindTreeItemByName( "Nulls" );
				HTREEITEM hNull		= m_rtSceneContents.FindTreeItemByName( strObjID, hNulls );

				m_rtSceneContents.SelectItem( hNull );

				((CEfnMeshlist*)pBaseScene)->SetSelectedNull( "Nulls", strObjID );
			}
			else
			{
				m_rtSceneContents.SelectItem( NULL );
				((CEfnMeshlist*)pBaseScene)->DeselectAllMeshes();
				((CEfnMeshlist*)pBaseScene)->DeselectAllNulls();
			}

			UpdateStatusBarInfoMESHLIST();
		}
		else if( pBaseScene->GetType() == FT_SCN )
		{
			enum OBJECT_TYPES objType = OBJ_UNDEFINED;
			str_t strObjID;

			CEfnMeshlist* pMeshlist = (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();

			
			((CSceneFile*)pBaseScene)->IntersectByRay( pos, out, ident_mtx, objType, strObjID );
			((CSceneFile*)pBaseScene)->SetNoCameraSelected();
			((CSceneFile*)pBaseScene)->SetNoLightSelected();
			pMeshlist->DeselectAllNulls();

			if( objType == OBJ_MESH )
			{
				HandleMeshSelection( strObjID );
			}
			else if( objType == OBJ_NULL )
			{
				HTREEITEM hNulls	= m_rtSceneContents.FindTreeItemByName( "Nulls" );
				HTREEITEM hNull		= m_rtSceneContents.FindTreeItemByName( strObjID, hNulls );

				m_rtSceneContents.SelectItem( hNull );

				pMeshlist->SetSelectedNull( "Nulls", strObjID );
			}
			else if( objType == OBJ_LIGHT )
			{
				HTREEITEM hLights	= m_rtSceneContents.FindTreeItemByName( "Lights" );
				HTREEITEM hLight	= m_rtSceneContents.FindTreeItemByName( strObjID, hLights );

				m_rtSceneContents.SelectItem( hLight );

				((CSceneFile*)pBaseScene)->SetSelectedLight( strObjID );
			}
			else if( objType == OBJ_CAMERA )
			{
				HTREEITEM hCameras	= m_rtSceneContents.FindTreeItemByName( "Cameras" );
				HTREEITEM hCamera	= m_rtSceneContents.FindTreeItemByName( strObjID, hCameras );

				m_rtSceneContents.SelectItem( hCamera );

				((CSceneFile*)pBaseScene)->SetSelectedCamera( strObjID );
			}
			else
			{
				((CSceneFile*)pBaseScene)->SetNoCameraSelected();
				((CSceneFile*)pBaseScene)->SetNoLightSelected();

				CEfnMeshlist* meshlist = (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();
				meshlist->DeselectAllMeshes();
				meshlist->DeselectAllNulls();

				m_rtSceneContents.SelectItem( NULL );
			}

			UpdateStatusBarInfoSCN();
		}

		CDialog::OnLButtonUp(nFlags, point);
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}
}

void CCookieMachineDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	str_t func_decl_str = "void CCookieMachineDlg::OnRButtonDown(UINT nFlags, CPoint point)";

	try
	{
		m_point = point;
	
		m_ptTopLeft				= CPoint( g_iMouseX, g_iMouseY );
		m_ptBottomRight			= m_ptTopLeft;

		m_bIsMouseEditingOpStarted = true;

		if( KEYDOWN('S') )
		{
			m_ViewMgr.StartCamRotating();
		}

		CDialog::OnRButtonDown(nFlags, point);
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}
}

void CCookieMachineDlg::OnRButtonUp( UINT nFlags, CPoint point )
{
	str_t func_decl_str = "void CCookieMachineDlg::OnRButtonUp( UINT nFlags, CPoint point )";

	try
	{
		bool bWasRotatingCam = false;
		bWasRotatingCam = m_ViewMgr.IsCamRotating();

		m_ViewMgr.FinishCamRotating();

		if( m_bIsMouseEditingOpStarted )
			m_bIsMouseEditingOpStarted	= false;

		if( bWasRotatingCam || KEYDOWN('Q') || KEYDOWN('W') || KEYDOWN('E') )
			return;
		
		int xMax = max( m_ptBottomRight.x, m_ptTopLeft.x );
		int xMin = min( m_ptBottomRight.x, m_ptTopLeft.x );
		int yMax = max( m_ptBottomRight.y, m_ptTopLeft.y );
		int yMin = min( m_ptBottomRight.y, m_ptTopLeft.y );

		if( xMax - xMin < 10 && yMax - yMin < 10 )
		{
			if( !KEYDOWN('S') && !m_ViewMgr.IsCameraOperationInProgress() && 
				( m_bIsGRadEditorModeOn && KEYDOWN('C') ) == false && 
				pBaseScene && ( pBaseScene->GetType() == FT_EFN_MESHLIST || pBaseScene->GetType() == FT_SCN ) )
			{
				CStatic*	render_wnd	= (CStatic*)GetDlgItem( IDC_RENDERWND );
				CRect		rect;
				
				render_wnd->GetWindowRect( rect );
				ScreenToClient( &rect );
				ClientToScreen( &point );

				if( point.x > rect.left && point.x < rect.right && 
					point.y > rect.top && point.y < rect.bottom )
				{
					CMenu menuPopup;
					menuPopup.LoadMenu( IDR_MENU_CONTEXT_OPERATIONS );

					menuPopup.GetSubMenu( 0 )->TrackPopupMenu( TPM_LEFTALIGN, point.x, point.y, this );

					CDialog::OnRButtonUp(nFlags, point);
					return;
				}		
			}
		}

		SetCursor( LoadCursor(NULL,IDC_ARROW) );

		//if( pBaseScene && pBaseScene->GetType() == FT_EFN )
		//{
		//	if( m_bIsGRadEditorModeOn )
		//	{
		//		((CEfnCharacter*)pBaseScene)->SetGlobalRadius( ((CEfnCharacter*)pBaseScene)->GetGlobalRadius() * fabs( m_fGlobRadScale ) );

		//		SAFE_RELEASE( ppCylinder );
		//		D3DXCreateCylinder( g_pd3dDevice, ( (CEfnCharacter*)pBaseScene )->GetGlobalRadius(), ( (CEfnCharacter*)pBaseScene )->GetGlobalRadius(), 0.2f, 20, 1, &ppCylinder, NULL );

		//		m_fGlobRadScale = 1.0f;
		//	}
		//}

		CDialog::OnRButtonUp(nFlags, point);
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}
}

void CCookieMachineDlg::OnMButtonDown(UINT nFlags, CPoint point)
{
	str_t func_decl_str = "void CCookieMachineDlg::OnMButtonDown(UINT nFlags, CPoint point)";

	try
	{
		m_point = point;

		if( KEYDOWN('S') )
			m_ViewMgr.StartCamZooming();

		CDialog::OnMButtonDown(nFlags, point);
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}
}

void CCookieMachineDlg::OnMButtonUp(UINT nFlags, CPoint point)
{
	str_t func_decl_str = "void CCookieMachineDlg::OnMButtonUp(UINT nFlags, CPoint point)";

	try
	{
		m_ViewMgr.FinishCamZooming();

		SetCursor( LoadCursor(NULL,IDC_ARROW) );

		CDialog::OnMButtonUp(nFlags, point);
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}
}

void CCookieMachineDlg::UpdateRendererMousePos( CPoint* pt )
{
	CRect rect_render; 
	CStatic* rndr_wnd = (CStatic*)GetDlgItem( IDC_RENDERWND );

	if( rndr_wnd )
	{
		rndr_wnd->GetWindowRect( &rect_render );
		ScreenToClient( &rect_render );

		g_iMouseX = pt->x - rect_render.left - 2;
		g_iMouseY = pt->y - rect_render.top - 2;
	}

}

void CCookieMachineDlg::OnMouseMove( UINT nFlags, CPoint point )
{
	return;

	str_t func_decl_str = "void CCookieMachineDlg::OnMouseMove( UINT nFlags, CPoint point )";

	try
	{
		static float fGlobRadScale		= 0.0f;

		/********* MOUSE OFFSET calc****************/
		float move[3];

		if( !m_bIsPerspectiveCameraOn )
			return;

		if( (m_point.x - point.x) == 0 )
			move[0] = 0.0f;
		else
			move[0] = (float)( m_point.x - point.x );

		if( ( m_point.y - point.y) == 0 )
			move[1] = 0.0f;
		else
			move[1] = (float)( m_point.y - point.y );
		/********* MOUSE OFFSET calc END ********/

		UpdateRendererMousePos( &point );

		if( nFlags & MK_LBUTTON || nFlags & MK_RBUTTON )
			m_ptBottomRight = CPoint( g_iMouseX, g_iMouseY );

		m_ViewMgr.MoveCamByMouse( move[0], move[1] );

		if( !pBaseScene )
		{
			CDialog::OnMouseMove(nFlags, point);
			return;
		}

		if( m_ViewMgr.IsCameraOperationInProgress() )
		{
			CDialog::OnMouseMove(nFlags, point);
			return;
		}

		if( pBaseScene && pBaseScene->GetType() == FT_EFN )
		{
			// редактирование глобального радиуса
			if( m_bIsGRadEditorModeOn && KEYDOWN('C') && m_bIsMouseEditingOpStarted )
			{
				CString temp;

				((CEfnCharacter*)pBaseScene)->SetGlobalRadius( fabs( m_fGlobRadScale + move[0] * 0.01f ) );

				temp.Format( "Global radius: %.4f", ((CEfnCharacter*)pBaseScene)->GetGlobalRadius() ); 
				m_rtSceneContents.SetCurrNodeText( temp );
				return;
			}
			else
			{
				m_fGlobRadScale = ((CEfnCharacter*)pBaseScene)->GetGlobalRadius();
			}
			// редактирование глобального радиуса END
		}
		else if( pBaseScene && pBaseScene->GetType() == FT_EFN_MESHLIST )
		{
			MeshlistMouseInteraction( nFlags, move[0], move[1] );
		}
		else if( pBaseScene && pBaseScene->GetType() == FT_SCN )
		{
			ScnMouseInteraction( nFlags, move[0], move[1] );
		}

		if( m_bIsVertexEditingModeOn && (nFlags & MK_LBUTTON) )
			SelectVerteciesByMouse();

		if( !m_bIsVertexEditingModeOn && (nFlags & MK_LBUTTON) )
			SelectNullsByMouse();

		CDialog::OnMouseMove(nFlags, point);
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}
}

void CCookieMachineDlg::SelectNullsByMouse()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN && pBaseScene->GetType() != FT_EFN_MESHLIST )
		return;

	static str_t last_null;
	CMesh* pMesh = NULL;

	if( pBaseScene->GetType() == FT_SCN )
	{
		CEfnMeshlist* pMeshlist	= (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();

		if( !pMeshlist )
			return;

		pMesh = pMeshlist->GetMeshByName( "Nulls" );
	}
	else if( pBaseScene->GetType() == FT_EFN_MESHLIST )
	{
		pMesh = ((CEfnMeshlist*)pBaseScene)->GetMeshByName( "Nulls" );
	}

	if( !pMesh )
		return;

	int idx = -1;

	for( int i = 0; i < pMesh->GetNullsCount(); i++ )
	{
		D3DXVECTOR3	vertex = D3DXVECTOR3( pMesh->GetNullByIndex(i)->matrices[0][0], pMesh->GetNullByIndex(i)->matrices[0][1], pMesh->GetNullByIndex(i)->matrices[0][2] );
		D3DXVECTOR3	out;

		D3DVIEWPORT9 viewport;
		D3DXMATRIX world, matProj, m_matView;
		D3DXMatrixIdentity( &world );
		g_pd3dDevice->GetViewport( &viewport );
		g_pd3dDevice->GetTransform( D3DTS_VIEW, &m_matView );
		g_pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );

		D3DXVec3Project( &out, &vertex, &viewport, &matProj, &m_matView, &world );

		CPoint ptTopLeft		= m_ptTopLeft;
		CPoint ptBottomRight	= m_ptBottomRight;

		if( ptTopLeft.x > ptBottomRight.x )
		{
			int tmp = ptTopLeft.x;
			ptTopLeft.x = ptBottomRight.x;
			ptBottomRight.x = tmp;
		}

		if( ptTopLeft.y > ptBottomRight.y )
		{
			int tmp = ptTopLeft.y;
			ptTopLeft.y = ptBottomRight.y;
			ptBottomRight.y = tmp;
		}

		if( out.x > ptTopLeft.x && out.x < ptBottomRight.x && out.y > ptTopLeft.y && out.y < ptBottomRight.y )
			idx = i;
	}

	if( idx != -1  )
	{
		pMesh->DeselectAllNulls(); 
		pMesh->SetSelectedNull( pMesh->GetNullByIndex( idx )->id );
	}
	else
	{
		DeselectAllObjects();
		last_null = "";
		m_rtSceneContents.SelectItem( NULL );
		return;
	}

	CNULL* pNull = pMesh->GetSelectedNull();

	if( !pNull )
		return;

	if( last_null == pNull->id )
		return;

	HTREEITEM hNulls	= m_rtSceneContents.FindTreeItemByName( "Nulls" );
	HTREEITEM hNull		= m_rtSceneContents.FindTreeItemByName( pNull->id, hNulls );

	m_rtSceneContents.SelectItem( hNull );

	last_null = pNull->id;
}

void CCookieMachineDlg::MeshlistMouseInteraction( UINT nMouseFlags, float& fXOffset, float& fYOffset )
{
	CEfnMeshlist*	meshlist	= (CEfnMeshlist*)pBaseScene;
	CMesh*			nMesh		= meshlist->GetMeshByName( "Nulls" );

	if( !nMesh )
		return;

	// редактирование нулика
	if( m_rtSceneContents.GetCurrNodeType() == TNT_NULL )
	{
		CNULL* nl = nMesh->GetNullByName( m_rtSceneContents.GetCurrNodeText().GetBuffer() );
		
		NullMouseInteraction( nl, nMouseFlags, fXOffset, fYOffset );
	}

	// редактирование вершин
	if( m_bIsVertexEditingModeOn )
	{
		CMesh* pMesh = ((CEfnMeshlist*)pBaseScene)->GetSelectedMesh();

		if( !pMesh )
			return;

		MeshMouseInteraction( pMesh, nMouseFlags, fXOffset, fYOffset );
	}
}

void CCookieMachineDlg::LightMouseInteraction( CLIGHT* pLight, UINT nMouseFlags, float& fXOffset, float& fYOffset )
{
	static float fGlobLightRot		= 0.0f;
	static float fGlobLightTransX	= 0.0f;
	static float fGlobLightTransZ	= 0.0f;
	static float fGlobLightTransY	= 0.0f;
	static float fGlobLightTransX2	= 0.0f;
	static float fGlobLightTransZ2	= 0.0f;
	static float fGlobLightTransY2	= 0.0f;

	if( !pLight )
		return;


	// Y transl	
	if( KEYDOWN( 'E' ) && (nMouseFlags & MK_RBUTTON) )
	{
		pLight->pos.y = fGlobLightTransY + fYOffset * 0.01f;

		if( m_pObjPropWnd )
			m_pPropLightDlg->Populate();
		
		CString script;
		script.Format(  "QuestOMgr.Light_%s:SetPosition( D3DXVECTOR3( %f, %f, %f ) );",
		pLight->name.data(), pLight->pos.x, pLight->pos.y, pLight->pos.z );
		g_MemMappingsMgr->PostScriptToEngine( script, true );

		if( pLight->type == LT_DIRECTED )
		{
			D3DXVECTOR3 tmpDir;
			D3DXVec3Normalize( &tmpDir, &(pLight->targ - pLight->pos) );

			script.Format(  "QuestOMgr.Light_%s:SetDirection( D3DXVECTOR3( %f, %f, %f ) );",
			pLight->name.data(), tmpDir.x, tmpDir.y, tmpDir.z );
			g_MemMappingsMgr->PostScriptToEngine( script, true );
		}

		((CSceneFile*)pBaseScene)->PrepareLightsLines( g_pd3dDevice );

		return;
	}
	else if( KEYDOWN( 'E' ) && (nMouseFlags & MK_MBUTTON) )
	{
		pLight->targ.y = fGlobLightTransY2 + fYOffset * 0.01f;

		if( m_pObjPropWnd )
			m_pPropLightDlg->Populate();

		CString script;
		D3DXVECTOR3 tmpDir;
		D3DXVec3Normalize( &tmpDir, &(pLight->targ - pLight->pos) );

		script.Format(  "QuestOMgr.Light_%s:SetDirection( D3DXVECTOR3( %f, %f, %f ) );",
		pLight->name.data(), tmpDir.x, tmpDir.y, tmpDir.z );
		g_MemMappingsMgr->PostScriptToEngine( script, true );

		((CSceneFile*)pBaseScene)->PrepareLightsLines( g_pd3dDevice );
		return;
	}
	else
	{
		fGlobLightTransY = pLight->pos.y;
		fGlobLightTransY2 = pLight->targ.y;
	}


	// XZ transl
	if( KEYDOWN( 'W' ) && (nMouseFlags & MK_RBUTTON) )
	{
		D3DXVECTOR3 vecX, vecY;
		D3DXMATRIX	rot, transl;

		D3DXVec3Cross( &vecX, &(m_ViewMgr.m_camCurrent->pos - m_ViewMgr.m_camCurrent->dir), &m_ViewMgr.m_camCurrent->upvector );
		D3DXVec3Normalize( &vecX, &vecX );
		
		D3DXMatrixRotationY( &rot, D3DX_PI / 2.0f );
		D3DXVec3TransformCoord( &vecY, &vecX, &rot );

		D3DXVec3Scale( &vecX, &vecX, -fXOffset );
		D3DXVec3Scale( &vecY, &vecY, -fYOffset );

		pLight->pos.x  = fGlobLightTransX + (vecX.x + vecY.x) * 0.025f;
		pLight->pos.z  = fGlobLightTransZ + (vecX.z + vecY.z) * 0.025f;

		if( m_pObjPropWnd )
			m_pObjPropWnd->Populate();

		CString script;
		script.Format(  "QuestOMgr.Light_%s:SetPosition( D3DXVECTOR3( %f, %f, %f ) );",
		pLight->name.data(), pLight->pos.x, pLight->pos.y, pLight->pos.z );
		g_MemMappingsMgr->PostScriptToEngine( script, true );

		if( pLight->type == LT_DIRECTED )
		{
			D3DXVECTOR3 tmpDir;
			D3DXVec3Normalize( &tmpDir, &(pLight->targ - pLight->pos) );

			script.Format(  "QuestOMgr.Light_%s:SetDirection( D3DXVECTOR3( %f, %f, %f ) );",
			pLight->name.data(), tmpDir.x, tmpDir.y, tmpDir.z );
			g_MemMappingsMgr->PostScriptToEngine( script, true );
		}

		((CSceneFile*)pBaseScene)->PrepareLightsLines( g_pd3dDevice );
		return;
	}
	else if( KEYDOWN( 'W' ) && (nMouseFlags & MK_MBUTTON) )
	{
		D3DXVECTOR3 vecX, vecY;
		D3DXMATRIX	rot, transl;

		D3DXVec3Cross( &vecX, &(m_ViewMgr.m_camCurrent->pos - m_ViewMgr.m_camCurrent->dir), &m_ViewMgr.m_camCurrent->upvector );
		D3DXVec3Normalize( &vecX, &vecX );
		
		D3DXMatrixRotationY( &rot, D3DX_PI / 2.0f );
		D3DXVec3TransformCoord( &vecY, &vecX, &rot );

		D3DXVec3Scale( &vecX, &vecX, -fXOffset );
		D3DXVec3Scale( &vecY, &vecY, -fYOffset );

		pLight->targ.x  = fGlobLightTransX2 + (vecX.x + vecY.x) * 0.025f;
		pLight->targ.z  = fGlobLightTransZ2 + (vecX.z + vecY.z) * 0.025f;

		if( m_pObjPropWnd )
			m_pObjPropWnd->Populate();

		CString script;
		D3DXVECTOR3 tmpDir;
		D3DXVec3Normalize( &tmpDir, &(pLight->targ - pLight->pos) );

		script.Format(  "QuestOMgr.Light_%s:SetDirection( D3DXVECTOR3( %f, %f, %f ) );",
		pLight->name.data(), tmpDir.x, tmpDir.y, tmpDir.z );

		g_MemMappingsMgr->PostScriptToEngine( script, true );

		((CSceneFile*)pBaseScene)->PrepareLightsLines( g_pd3dDevice );
		return;
	}		
	else
	{
		fGlobLightTransX = pLight->pos.x;
		fGlobLightTransZ = pLight->pos.z;
		fGlobLightTransX2 = pLight->targ.x;
		fGlobLightTransZ2 = pLight->targ.z;
	}
}

void CCookieMachineDlg::NullMouseInteraction( CNULL* pNull, UINT nMouseFlags, float& fXOffset, float& fYOffset )
{
	static float fGlobNullRot		= 0.0f;
	static float fGlobNullTransX	= 0.0f;
	static float fGlobNullTransZ	= 0.0f;
	static float fGlobNullTransY	= 0.0f;

	if( !pNull )
		return;

	// Y rot
	if( KEYDOWN( 'Q' ) && (nMouseFlags & MK_RBUTTON) )
	{
		pNull->matrices[0][4]  = fGlobNullRot + fXOffset * 0.4f;

		ClampAngle( pNull->matrices[0][4] );

		if( m_pObjPropWnd )
			m_pPropNullDlg->Populate();

		return;
	}		
	else
		fGlobNullRot = pNull->matrices[0][4];


	// Y transl	
	if( KEYDOWN( 'E' ) && (nMouseFlags & MK_RBUTTON) )
	{
		pNull->matrices[0][1]  = fGlobNullTransY + fYOffset * 0.01f;

		if( m_pObjPropWnd )
			m_pPropNullDlg->Populate();

		return;
	}		
	else
		fGlobNullTransY = pNull->matrices[0][1];


	// XZ transl
	if( KEYDOWN( 'W' ) && (nMouseFlags & MK_RBUTTON) )
	{
		D3DXVECTOR3 vecX, vecY;
		D3DXMATRIX	rot, transl;

		D3DXVec3Cross( &vecX, &(m_ViewMgr.m_camCurrent->pos - m_ViewMgr.m_camCurrent->dir), &m_ViewMgr.m_camCurrent->upvector );
		D3DXVec3Normalize( &vecX, &vecX );
		
		D3DXMatrixRotationY( &rot, D3DX_PI / 2.0f );
		D3DXVec3TransformCoord( &vecY, &vecX, &rot );

		D3DXVec3Scale( &vecX, &vecX, -fXOffset );
		D3DXVec3Scale( &vecY, &vecY, -fYOffset );

		pNull->matrices[0][0]  = fGlobNullTransX + (vecX.x + vecY.x) * 0.025f;
		pNull->matrices[0][2]  = fGlobNullTransZ + (vecX.z + vecY.z) * 0.025f;

		if( m_pObjPropWnd )
			m_pObjPropWnd->Populate();

		return;
	}		
	else
	{
		fGlobNullTransX = pNull->matrices[0][0];
		fGlobNullTransZ = pNull->matrices[0][2];
	}
}

void CCookieMachineDlg::MeshMouseInteraction( CMesh* pMesh, UINT nMouseFlags, float& fXOffset, float& fYOffset )
{
	CButton* btnAxesX	= (CButton*)GetDlgItem( IDC_PUSH_AXES_X );
	CButton* btnAxesY	= (CButton*)GetDlgItem( IDC_PUSH_AXES_Y );
	CButton* btnAxesZ	= (CButton*)GetDlgItem( IDC_PUSH_AXES_Z );
	CButton* btnAxesXY	= (CButton*)GetDlgItem( IDC_PUSH_AXES_XY );
	CButton* btnAxesXZ	= (CButton*)GetDlgItem( IDC_PUSH_AXES_XZ );
	CButton* btnAxesYZ	= (CButton*)GetDlgItem( IDC_PUSH_AXES_YZ );

	if( !pMesh )
		return;

	if( m_bIsMouseEditingOpStarted ) // XZ transl
	{
		D3DXVECTOR3 vec( 0, 0, 0 );
		D3DXVECTOR4	res;
		D3DXMATRIX	rot, transl;
		float		fScaleFactor = 0.004f;
		float		angle = GetAngle( &D3DXVECTOR3( 1,0,0 ), &(m_ViewMgr.m_camCurrent->pos - m_ViewMgr.m_camCurrent->dir), &D3DXVECTOR3( 0,1,0 ) );
		D3DXMatrixRotationY( &rot, angle );				

		D3DXVECTOR3 vecX, vecY, resXY;

		switch( m_AxisAlign )
		{
			case AAE_X:
				D3DXVec3Cross( &vecX, &(m_ViewMgr.m_camCurrent->pos - m_ViewMgr.m_camCurrent->dir), &m_ViewMgr.m_camCurrent->upvector );
				D3DXVec3Normalize( &vecX, &vecX );

				vecY = m_ViewMgr.m_camCurrent->upvector;

				D3DXVec3Scale( &vecX, &vecX, -fXOffset );
				D3DXVec3Scale( &vecY, &vecY, fYOffset );

				resXY = (vecX + vecY);

				pMesh->MoveSelectedVerteciesXBegin( resXY.x * fScaleFactor );
				break;

			case AAE_Y:
				pMesh->MoveSelectedVerteciesYBegin( fYOffset * fScaleFactor );
				break;

			case AAE_Z:
				D3DXVec3Cross( &vecX, &(m_ViewMgr.m_camCurrent->pos - m_ViewMgr.m_camCurrent->dir), &m_ViewMgr.m_camCurrent->upvector );
				D3DXVec3Normalize( &vecX, &vecX );

				vecY = m_ViewMgr.m_camCurrent->upvector;

				D3DXVec3Scale( &vecX, &vecX, -fXOffset );
				D3DXVec3Scale( &vecY, &vecY, fYOffset );

				resXY = (vecX + vecY);

				pMesh->MoveSelectedVerteciesZBegin( resXY.z * fScaleFactor );
				break;

			case AAE_XY:
				D3DXVec3Cross( &vecX, &(m_ViewMgr.m_camCurrent->pos - m_ViewMgr.m_camCurrent->dir), &m_ViewMgr.m_camCurrent->upvector );
				D3DXVec3Normalize( &vecX, &vecX );

				vecY = m_ViewMgr.m_camCurrent->upvector;

				D3DXVec3Scale( &vecX, &vecX, -fXOffset );
				D3DXVec3Scale( &vecY, &vecY, fYOffset );

				resXY = (vecX + vecY);

				pMesh->MoveSelectedVerteciesXYBegin( resXY.x * fScaleFactor, resXY.y * fScaleFactor );
				break;

			case AAE_XZ:
				D3DXVec3Cross( &vecX, &(m_ViewMgr.m_camCurrent->pos - m_ViewMgr.m_camCurrent->dir), &m_ViewMgr.m_camCurrent->upvector );
				D3DXVec3Normalize( &vecX, &vecX );
					
				D3DXMatrixRotationY( &rot, D3DX_PI / 2.0f );
				D3DXVec3TransformCoord( &vecY, &vecX, &rot );

				D3DXVec3Scale( &vecX, &vecX, -fXOffset );
				D3DXVec3Scale( &vecY, &vecY, -fYOffset );

				resXY = (vecX + vecY);

				pMesh->MoveSelectedVerteciesXZBegin( resXY.x * fScaleFactor, resXY.z * fScaleFactor );
				break;

			case AAE_YZ:
				D3DXVec3Cross( &vecX, &(m_ViewMgr.m_camCurrent->pos - m_ViewMgr.m_camCurrent->dir), &m_ViewMgr.m_camCurrent->upvector );
				D3DXVec3Normalize( &vecX, &vecX );

				vecY = m_ViewMgr.m_camCurrent->upvector;

				D3DXVec3Scale( &vecX, &vecX, -fXOffset );
				D3DXVec3Scale( &vecY, &vecY, fYOffset );

				resXY = (vecX + vecY);

				pMesh->MoveSelectedVerteciesYZBegin( resXY.y * fScaleFactor, resXY.z * fScaleFactor );
				break;
		}

		return;
	}		
	else
	{
		pMesh->MoveSelectedVerteciesEnd();
	}
}

void CCookieMachineDlg::ScnMouseInteraction( UINT nMouseFlags, float& fXOffset, float& fYOffset )
{
	static float fGlobLightRange	= 0.0f;
	static float fGlobCamZoomNear	= 0.0f;
	static float fGlobCamZoomFar	= 0.0f;


	CSceneFile*		scene		= (CSceneFile*)pBaseScene;
	CEfnMeshlist*	meshlist	= (CEfnMeshlist*)scene->GetMeshlist();
	CMesh*			nMesh		= meshlist->GetMeshByName( "Nulls" );

	// редактирование нулика
	if( nMesh && ( m_rtSceneContents.GetCurrNodeType() == TNT_NULL || m_rtSceneContents.GetCurrNodeType() == TNT_SCN_CURVE_NULL ) )
	{
		CNULL* nl = nMesh->GetNullByName( m_rtSceneContents.GetCurrNodeText().GetBuffer() );
		
		if( !nl )
			return;

		CString strPos;
		CString strRot;

		strPos.Format( "%f, %f, %f", nl->matrices[0][0], nl->matrices[0][1], nl->matrices[0][2] );
		strRot.Format( "%f, %f, %f", D3DXToRadian( nl->matrices[0][3] ), D3DXToRadian( nl->matrices[0][4] ), D3DXToRadian( nl->matrices[0][5] ) );

		CString strScript, strEdObj = scene->GetEditedObject().data();

		if( ( KEYDOWN( 'Q' ) || KEYDOWN( 'E' ) || KEYDOWN( 'W' ) ) && (nMouseFlags & MK_RBUTTON) )
		{
			if( strEdObj != "" )
			{
				CWnd* pWnd = FindWindow( NULL, "Яйца Фаберже" );

				if( !IsWindow( pWnd->GetSafeHwnd() ) )
					scene->SetEditedObject( "" );

				strScript  = "if ( not(QuestOMgr." + strEdObj + " == nil ) ) then \r\n";
				strScript += "	QuestOMgr." + strEdObj + ":SetPosition( D3DXVECTOR3( " + strPos + " ) ); \r\n";
				strScript += "	QuestOMgr." + strEdObj + ":SetRotation( D3DXVECTOR3( " + strRot + " ) ); \r\n";
				//strScript += "else \r\n";
				//strScript += "	ShowMessage( \"" strTemp  "\" does not exist! ); \r\n";
				strScript += "end; \r\n";

				g_MemMappingsMgr->PostScriptToEngine( strScript, true );
			}
		}


		NullMouseInteraction( nl, nMouseFlags, fXOffset, fYOffset );
		scene->PrepareCurves();
	}
	// редактирование нулика END

	// редактирование источника света
	if( m_rtSceneContents.GetCurrNodeType() == TNT_LIGHT )
	{
		CLIGHT* light = scene->GetLightByID( m_rtSceneContents.GetCurrNodeText().GetBuffer() );
			
		if( !light )
			return;
		
		if( scene->GetIsLightRangeVisible() )
		{
			if( KEYDOWN( 'C' ) && (nMouseFlags & MK_RBUTTON) )
			{
				if( fGlobLightRange + fXOffset * 0.1f  >= 0.0f )
					light->range = fGlobLightRange + fXOffset * 0.1f ;
				else
					light->range = 0.0f;

				CString script;
				script.Format(  "QuestOMgr.Light_%s:SetRange( %f );",
				light->name.data(), light->range );

				g_MemMappingsMgr->PostScriptToEngine( script, true );

				m_pObjPropWnd->Populate();
				return;
			}		
			else
				fGlobLightRange = light->range;
		}

		if( KEYDOWN( 'E' ) || KEYDOWN( 'W' ) )
		{
			LightMouseInteraction( light, nMouseFlags, fXOffset, fYOffset );
		}
	}
	// редактирование источника света END


	// редактирование камеры
	CCAMERA2* cam = scene->GetSelectedCamera();
	
	if( cam )
	{
		if( scene->GetCamZoomNear() )
		{
			if( KEYDOWN( 'C' ) && (nMouseFlags & MK_RBUTTON) )
			{
				if( fGlobCamZoomNear + fXOffset * 0.2f >= 0.0f )
					cam->zoom_near = fGlobCamZoomNear + fXOffset * 0.2f ;
				else
					cam->zoom_near = 0.0f;

				m_pObjPropWnd->Populate();

				return;
			}		
			else
				fGlobCamZoomNear = cam->zoom_near;
		}
		else if( scene->GetCamZoomFar() ) 
		{
			if( KEYDOWN( 'C' ) && (nMouseFlags & MK_RBUTTON) )
			{
				if( fGlobCamZoomFar + fXOffset * 0.2f >= 0.0f )
					cam->zoom_far = fGlobCamZoomFar + fXOffset * 0.2f ;
				else
					cam->zoom_far = 0.0f;

				m_pObjPropWnd->Populate();

				return;
			}		
			else
				fGlobCamZoomFar = cam->zoom_far;
		}
	}
	// редактирование камеры END


	// редактирование вершин
	if( m_bIsVertexEditingModeOn && !KEYDOWN('S') )
	{
		CEfnMeshlist*	pMeshlist	= (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();

		if( !pMeshlist )
			return;

		CMesh*			pMesh		= pMeshlist->GetSelectedMesh();

		if( !pMesh )
			return;

		MeshMouseInteraction( pMesh, nMouseFlags, fXOffset, fYOffset );
	}
}

void CCookieMachineDlg::SelectVerteciesByMouse()
{
	if( !pBaseScene )
		return;

	enum VERTEX_SELECTION_MODE SelectionMode;

	if( KEYDOWN( VK_CONTROL ) )
		SelectionMode = VSM_ADDING;
	else if( KEYDOWN( VK_MENU ) )
		SelectionMode = VSM_SUBSTRACTING;
	else
		SelectionMode = VSM_DISCARDING;

	CMesh* pMesh = NULL;

	if( pBaseScene->GetType() == FT_EFN_MESHLIST )
	{
		pMesh = ((CEfnMeshlist*)pBaseScene)->GetSelectedMesh();
	}
	else if( pBaseScene->GetType() == FT_SCN )
	{
		CEfnMeshlist* pMeshlist = (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();

		if( !pMeshlist )
			return;

		pMesh = pMeshlist->GetSelectedMesh();
	}
	else
	{
		return;
	}


	if( !pMesh )
		return;

	CPoint ptTopLeft		= m_ptTopLeft;
	CPoint ptBottomRight	= m_ptBottomRight;

	if( ptTopLeft.x > ptBottomRight.x )
	{
		int tmp = ptTopLeft.x;
		ptTopLeft.x = ptBottomRight.x;
		ptBottomRight.x = tmp;
	}

	if( ptTopLeft.y > ptBottomRight.y )
	{
		int tmp = ptTopLeft.y;
		ptTopLeft.y = ptBottomRight.y;
		ptBottomRight.y = tmp;
	}

	if( pMesh )
		pMesh->SelectVerteciesByFrustrum( g_pd3dDevice, SelectionMode, &ptTopLeft, &ptBottomRight );

	if( pBaseScene && pBaseScene->GetType() == FT_EFN_MESHLIST )
	{
		UpdateStatusBarInfoMESHLIST();
	}
	else if( pBaseScene && pBaseScene->GetType() == FT_SCN )
	{
		UpdateStatusBarInfoSCN();
	}
}

void CCookieMachineDlg::SelectAllAnimFrames()
{
	if( !pBaseScene)
		return;

	if( pBaseScene->GetType() != FT_EFN )
		return;

	
	m_sliderAnim.ClearSel(true);
	m_sliderAnim.SetSelection(0, ((CEfnCharacter*)pBaseScene)->GetFramesCount() - 1);
	m_sliderAnim.Invalidate(true);

	m_iStartingFrame = 0;
	m_iEndingFrame   = ((CEfnCharacter*)pBaseScene)->GetFramesCount() - 1;

	UpdateStatusBarInfoEFN();
}

void CCookieMachineDlg::DeselectAllAnimFrames()
{
	if( !pBaseScene)
		return;

	if( pBaseScene->GetType() != FT_EFN )
		return;

	m_sliderAnim.ClearSel(true);
	m_sliderAnim.Invalidate(true);

	m_iStartingFrame = 0;
	m_iEndingFrame   = 0;

	firstSelFrame    = -1;
	lastSelFrame     = -1;

	UpdateStatusBarInfoEFN( m_sliderAnim.GetPos(), m_sliderAnim.GetPos() );
}

void CCookieMachineDlg::UpdateStatusBarInfoEFN( int iStartingFrame, int iEndingFrame )
{
	CString strTemp;

	if( !pBaseScene || pBaseScene->GetType() != FT_EFN )
		return;

	if( iStartingFrame == -1 && iEndingFrame == -1 ) 
		strTemp.Format( "Цикл анимации: с %d по %d кадр", m_iStartingFrame, m_iEndingFrame );
	else
		strTemp.Format( "Цикл анимации: с %d по %d кадр", iStartingFrame, iEndingFrame );

	m_StatusBar->SetPaneText( 4, strTemp, true );
	
	CEfnCharacter* chr = (CEfnCharacter*)pBaseScene;
	EFN_ANIMATION* anim	= chr->GetAnimationByName( m_rtSceneContents.GetCurrNodeText().GetBuffer() );

	TNODE* nodeData = (TNODE*)m_rtSceneContents.GetCurrNodeData();

	if( anim && nodeData && nodeData->_type == TREENODE::N_ANIMATION )
	{
		strTemp.Format( "Анимация: %s, цикл c %d по %d",
					m_rtSceneContents.GetCurrNodeText(),
					anim->frames_list[0], 
					anim->frames_list[anim->frames_count - 1] );

		m_StatusBar->SetPaneText( 3, strTemp, true );
	}
	else
		m_StatusBar->SetPaneText( 3, "", true );
}

void CCookieMachineDlg::UpdateStatusBarInfoMESHLIST()
{
	CString strTemp;

	if( !pBaseScene || pBaseScene->GetType() != FT_EFN_MESHLIST )
		return;

	CEfnMeshlist* pMeshlist = (CEfnMeshlist*)pBaseScene;

	if( !m_bIsVertexEditingModeOn )
	{
		if( pMeshlist->GetSelectedMeshesCount() > 0 )
		{
			int iVertCount = 0;

			for( int i = 0; i < pMeshlist->GetMeshesCount(); i++ )
			{
				CMesh* pMesh = pMeshlist->GetMeshByIndex(i);

				if( pMesh->IsSelected() )
					iVertCount += pMesh->GetVerteciesCount();
			}

			strTemp.Format( "Выбранных мешей: %d, вершин: %d", pMeshlist->GetSelectedMeshesCount(), iVertCount );
		}
		else
			strTemp = "";
	}
	else
	{
		CMesh* pMesh = pMeshlist->GetSelectedMesh();
		strTemp.Format( "Выбранных вершин: %d из %d", pMesh->GetSelectedVerteciesCount(), pMesh->GetVerteciesEdCount() );
		
	}

	m_StatusBar->SetPaneText( 3, strTemp, true );
}

void CCookieMachineDlg::UpdateStatusBarInfoSCN()
{
	CString strTemp;

	if( !pBaseScene || pBaseScene->GetType() != FT_SCN )
		return;

	CSceneFile*		pScene = (CSceneFile*)pBaseScene;
	CEfnMeshlist*	pMeshlist = (CEfnMeshlist*)pScene->GetMeshlist();

	if( !m_bIsVertexEditingModeOn )
	{
		if( pMeshlist->GetSelectedMeshesCount() > 0 )
		{
			int iVertCount = 0;

			for( int i = 0; i < pMeshlist->GetMeshesCount(); i++ )
			{
				CMesh* pMesh = pMeshlist->GetMeshByIndex(i);

				if( pMesh->IsSelected() )
					iVertCount += pMesh->GetVerteciesCount();
			}

			strTemp.Format( "Выбранных мешей: %d, вершин: %d", pMeshlist->GetSelectedMeshesCount(), iVertCount );
		}
		else
			strTemp = "";
	}
	else
	{
		CMesh* pMesh = pMeshlist->GetSelectedMesh();
		strTemp.Format( "Выбранных вершин: %d из %d", pMesh->GetSelectedVerteciesCount(), pMesh->GetVerteciesEdCount() );
		
	}

	m_StatusBar->SetPaneText( 3, strTemp, true );
}

void CCookieMachineDlg::ResetStatusBarInfo()
{
	//clear stat bar
	m_StatusBar->SetPaneText( 3, "", true );
	m_StatusBar->SetPaneText( 4, "", true );
	m_StatusBar->SetPaneText( 5, "Ready.", true );
}

BOOL CCookieMachineDlg::ProcessDialogMessages()
{
	Sleep(0);	//Yield to other system processes

	// Process the message queue 
	MSG	dlgMsg;

	if( ::PeekMessage( &dlgMsg, NULL, 0, 0, PM_NOREMOVE ) )		// If there are pending messages
	{
        if( !AfxGetApp()->PumpMessage() )						// Allow them to be processed
		{					
            ::PostQuitMessage (0);								// When no more messages exist 
			                                                    // return WM_QUIT should return 0
			return TRUE;										// Return the caller indicating that 
			                                                    // we just processed a Message 
			                                                    // and that there may be more
        }
    }

    // Simulate idle processing.
    LONG lIdle = 0;

    while( AfxGetApp()->OnIdle( lIdle++ ) );

	return FALSE;												// Return to Caller saying
	                                                            // that we did not process
	                                                            // any messages 
}

BOOL CCookieMachineDlg::PreTranslateMessage( MSG* pMsg )
{
	return CDialog::PreTranslateMessage(pMsg);

	str_t func_decl_str = "BOOL CCookieMachineDlg::PreTranslateMessage( MSG* pMsg )";

	try
	{
		if( !m_rtSceneContents.IsLabelEdited() )
		{
			if ( TranslateAccelerator( this->GetSafeHwnd(), m_hAccelerator, pMsg ) ) 
				return true;
		}

		if( pMsg->message != WM_SIZE && pMsg->message != WM_KEYDOWN && pMsg->message != WM_MOUSEMOVE )
			Render();

		switch( pMsg->wParam ) 
		{
			case VK_ESCAPE:
				return false;
				break;

			case VK_RETURN:
				return false;
				break;

			case VK_F12:
				return false;
				break;

			case VK_F1:
				return false;
				break;
		}

		return CDialog::PreTranslateMessage(pMsg);

	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		g_ExeptLog->__data.push_back( "BOOL CCookieMachineApp::InitInstance()" );

		MessageBox( g_ExeptLog->GetExeptionLog().data(), "Critical error, application will exit!", MB_OK | MB_ICONERROR );
		exit(0);

		throw;
	}
}

void CCookieMachineDlg::DoResize( int delta )
{
	CWnd* pRenderWnd	= GetDlgItem( IDC_RENDERWND );
	CWnd* pTreeFrameWnd = GetDlgItem( IDC_CONT_FRAME );
	CWnd* pTreeWnd		= GetDlgItem( IDC_SCENE_TREE );
	CWnd* pTopBevel		= GetDlgItem( IDC_FRM_TOOLBAR_TOP );
	CWnd* pCams			= GetDlgItem( IDC_CMB_CAMERAS );
	CWnd* pViewMenu		= GetDlgItem( IDC_BMP );
	CWnd* pCamsLbl		= GetDlgItem( IDC_CAM_CMB_LBL );
	CWnd* pViews		= GetDlgItem( IDC_CMB_VIEWS );
	CWnd* pViewLbl		= GetDlgItem( IDC_VIEW_CMB_LBL );
	

	CSplitterControl::ChangeWidth( pTreeFrameWnd, delta, CW_LEFTALIGN );
	CSplitterControl::ChangeWidth( pTreeWnd, delta, CW_LEFTALIGN );
	//CSplitterControl::ChangeWidth( pRenderWnd, -delta, CW_RIGHTALIGN );
	CSplitterControl::ChangeWidth( pTopBevel, -delta, CW_RIGHTALIGN );
	CSplitterControl::ChangePos( pCams, -delta, 0 );
	CSplitterControl::ChangePos( pCamsLbl, -delta, 0 );
	CSplitterControl::ChangePos( pViews, -delta, 0 );
	CSplitterControl::ChangePos( pViewLbl, -delta, 0 );
	CSplitterControl::ChangePos( pViewMenu, -delta, 0 );

	m_iSplitterX += delta;
	
	ResizeRenderWnd();

	Invalidate();
	UpdateWindow();
	
	HandlePossibleSizeChange();
}

LRESULT CCookieMachineDlg::WindowProc( UINT message, WPARAM wParam, LPARAM lParam ) 
{
	return CDialog::WindowProc(message, wParam, lParam);

	static bool exeption_occured = false;

	if( exeption_occured )
		return true;

	str_t func_decl_str = "LRESULT CCookieMachineDlg::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )";

	try
	{
		WINDOWPOS *lpwp;

		switch(message) 
		{ 
			case WM_SIZE:
				{
					if( wParam == SIZE_MAXIMIZED )
					{
						UpdateUISizing();
						HandlePossibleSizeChange();
						m_bIsAppMinimized = false;
					}
					else if( wParam == SIZE_MINIMIZED )
					{
						//OnLostDevice();
						m_bIsAppMinimized = true;
					}
					else if( wParam == SIZE_RESTORED )
					{
						UpdateUISizing();
						HandlePossibleSizeChange();
						m_bIsAppMinimized = false;
					}
				}
				break;

			case WM_NOTIFY:
				if( wParam == IDC_SPLITTER )
				{	
					SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
					DoResize( pHdr->delta );
				}
				break;

			case WM_ACTIVATEAPP:
					if( wParam == TRUE )
						m_bIsAppPaused = false;
					else
						m_bIsAppPaused = true;
				break;

			case WM_WINDOWPOSCHANGING:
				lpwp = (LPWINDOWPOS) lParam;
	            
				if( lpwp->cx < MIN_WND_WIDTH )
					lpwp->cx = MIN_WND_WIDTH;

				if( lpwp->cy < MIN_WND_HEIGHT )
					lpwp->cy = MIN_WND_HEIGHT;
				break;

			case WM_EXITSIZEMOVE:
				m_bIsAppPaused = false;
				HandlePossibleSizeChange();
				break;

			case WM_ENTERSIZEMOVE:
				m_bIsAppPaused = true;
				//HandlePossibleSizeChange();
				break;

			case WM_COMMAND:
				//if( wParam != IDC_BUTTON_SHOW_BS && wParam != IDC_BUTTON_SHOW_OOBB )
				//{
					/*if( pBaseScene && pBaseScene->GetType() == FT_EFN )
					{
						if( ((CEfnCharacter*)pBaseScene)->IsBoundingSphereVisible() )
							m_btnShowBS.SetState( true );

						if( ((CEfnCharacter*)pBaseScene)->IsAABBVisible() )
							m_btnOOBB.SetState( true );
					}*/
				//}
				break;

			case WM_ACTIVATE:
				//HandlePossibleSizeChange();
				break;
				

			default:
				break;
		}

		return CDialog::WindowProc(message, wParam, lParam);
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		g_ExeptLog->__data.push_back( "BOOL CCookieMachineApp::InitInstance()" );

		exeption_occured = true;
		MessageBox( g_ExeptLog->GetExeptionLog().data(), "Critical error, application will exit!", MB_OK | MB_ICONERROR );
		exit(0);
	}
}

void CCookieMachineDlg::SceneContextMenuProcXSI( CPoint point )
{
	int iSubMenu = -1;
	CMenu menuPopupXSI;
	menuPopupXSI.LoadMenu( IDR_MENU_CONTEXT_XSI );
		
	switch( m_rtSceneContents.GetCurrNodeType() )
	{
		case TNT_MESH:
			iSubMenu = 0;
			break;

		case TNT_MESHES:
			iSubMenu = 1;
			break;

		case TNT_LIGHTS:
			iSubMenu = 2;
			break;

		case TNT_CAMERAS:
			iSubMenu = 3;
			break;

		case TNT_CAMERA:
			iSubMenu = 4;
			break;
	}

	if( iSubMenu != -1 )
		menuPopupXSI.GetSubMenu( iSubMenu )->TrackPopupMenu( TPM_LEFTALIGN, point.x, point.y, this );
}

void CCookieMachineDlg::SceneContextMenuProcSCN( CPoint point )
{
	int iSubMenu = -1;
	CMenu menuPopupSCN;
	menuPopupSCN.LoadMenu( IDR_MENU_CONTEXT_SCN );

	bool bEnable = m_appSettings.ExpertMode;

	EnableMenuItem( &menuPopupSCN, ID_MENU_SCN_MESHES_CREATE_BOX, bEnable );
	EnableMenuItem( &menuPopupSCN, ID_MENU_SCN_MESHES_CREATE_TRIANGLE, bEnable );
	EnableMenuItem( &menuPopupSCN, ID_MENU_SCN_MESHES_CREATE_QUAD, bEnable );
	EnableMenuItem( &menuPopupSCN, ID_MENU_SCN_NULL_RENAME, bEnable );
	EnableMenuItem( &menuPopupSCN, ID_MENU_SCN_NULL_DELETE, bEnable );
	EnableMenuItem( &menuPopupSCN, ID_MENU_SCN_MESH_DELETE, bEnable );
	EnableMenuItem( &menuPopupSCN, ID_MENU_SCN_MESH_RENAME, bEnable );
	EnableMenuItem( &menuPopupSCN, ID_MENU_SCN_MESH_DUPLICATE, bEnable );

	switch( m_rtSceneContents.GetCurrNodeType() )
	{
		case TNT_MESHES:
			iSubMenu = 0;
			break;

		case TNT_MESH:
			iSubMenu = 1;
			break;

		case TNT_NULLS:
			iSubMenu = 2;
			break;

		case TNT_NULL:
			iSubMenu = 3;
			break;

		case TNT_SCN_TEXTURES:
			iSubMenu = 4;
			break;

		case TNT_TEXTURE:
			iSubMenu = 5;
			break;

		case TNT_SCN_CURVES:
			iSubMenu = 6;
			break;

		case TNT_SCN_CURVE:
			{
				iSubMenu = 7;
				
				CurveItem* crv = ((CSceneFile*)pBaseScene)->GetCurveByName( m_rtSceneContents.GetCurrNodeText().GetBuffer() );

				if( crv )
				{
					if( crv->type == LINEAR )
						CheckMenuItem( menuPopupSCN.GetSubMenu( iSubMenu )->GetSafeHmenu(), ID_MENU_SCN_CURVE_TYPE_LINEAR, MF_CHECKED );
					else if( crv->type == CATMULROM )
						CheckMenuItem( menuPopupSCN.GetSubMenu( iSubMenu )->GetSafeHmenu(), ID_MENU_SCN_CURVE_TYPE_CATMUL, MF_CHECKED );
				}
			}
			break;

		case TNT_SCN_CURVE_NULL:
			iSubMenu = 8;
			break;

		case TNT_CAMERAS:
			iSubMenu = 9;
			break;

		case TNT_CAMERA:
			iSubMenu = 10;
			break;

		case TNT_LIGHTS:
			iSubMenu = 11;
			break;

		case TNT_LIGHT:
			iSubMenu = 12;
			break;

		case TNT_SCN_CUST_OBJECTS:
			iSubMenu = 13;
			break;

		case TNT_SCN_CUST_OBJECT:
			iSubMenu = 14;
			break;

		case TNT_SCN_CUST_TYPES:
			iSubMenu = 15;
			break;

		case TNT_SCN_CUST_TYPE:
			iSubMenu = 16;
			break;

		case TNT_SCN_CAMERA_MISC_BIND:
			iSubMenu = 17;
			break;

		case TNT_SCN_CAMERA_BIND:
			iSubMenu = 18;
			break;
	}

	if( iSubMenu != -1 )
		menuPopupSCN.GetSubMenu( iSubMenu )->TrackPopupMenu( TPM_LEFTALIGN, point.x, point.y, this );
}

void CCookieMachineDlg::SceneContextMenuProcMSH( CPoint point )
{
	int iSubMenu = -1;
	CMenu menuPopupMSH;
	menuPopupMSH.LoadMenu( IDR_MENU_CONTEXT_MSH );
		
	switch( m_rtSceneContents.GetCurrNodeType() )
	{
		case TNT_MESHES:
			iSubMenu = 0;
			break;

		case TNT_MESH:
			iSubMenu = 1;
			break;

		case TNT_NULLS:
			iSubMenu = 3;
			break;

		case TNT_NULL:
			iSubMenu = 4;
			break;
	}

	if( iSubMenu != -1 )
		menuPopupMSH.GetSubMenu( iSubMenu )->TrackPopupMenu( TPM_LEFTALIGN, point.x, point.y, this );
}

void CCookieMachineDlg::SceneContextMenuProcEFN( CPoint point )
{
	int iSubMenu = -1;
	CMenu menuPopupEFN;
	menuPopupEFN.LoadMenu( IDR_MENU_CONTEXT_EFN );
	

	TNODE* data = (TNODE*)m_rtSceneContents.GetCurrNodeData();

	if( !data )
		return;

	switch( data->_type )
	{
		case TREENODE::N_ANIMATIONS:
			iSubMenu = 1;
			break;

		case TREENODE::N_MESH:
			iSubMenu = 2;
			break;

		case TREENODE::N_ANIMATION:
			iSubMenu = 3;
			break;

		case TREENODE::N_NULL:
			iSubMenu = 4;
			break;

		case TREENODE::N_NULLS:
			iSubMenu = 5;
			break;

		case TREENODE::N_TEXTURE:
			{
				iSubMenu = 7;

				if( ((CEfnCharacter*)pBaseScene)->IsUV1Used() )
				{
					CheckMenuItem( menuPopupEFN.GetSubMenu( iSubMenu )->GetSafeHmenu(), ID_MENU_EFN_USE_UV1, MF_CHECKED );
					CheckMenuItem( menuPopupEFN.GetSubMenu( iSubMenu )->GetSafeHmenu(), ID_MENU_EFN_USE_UV2, MF_UNCHECKED );
				}
				else
				{
					CheckMenuItem( menuPopupEFN.GetSubMenu( iSubMenu )->GetSafeHmenu(), ID_MENU_EFN_USE_UV1, MF_UNCHECKED );
					CheckMenuItem( menuPopupEFN.GetSubMenu( iSubMenu )->GetSafeHmenu(), ID_MENU_EFN_USE_UV2, MF_CHECKED );				
				}
			}
			break;

		case TREENODE::N_MATERIAL:
			iSubMenu = 8;
			break;

		case TREENODE::N_UNKNOWN:
			return;
			break;
	}

	if( iSubMenu != -1 )
		menuPopupEFN.GetSubMenu( iSubMenu )->TrackPopupMenu( TPM_LEFTALIGN, point.x, point.y, this );
}

void CCookieMachineDlg::OnContextMenu( CWnd* pWnd, CPoint point )
{
	str_t func_decl_str = "void CCookieMachineDlg::OnContextMenu( CWnd* pWnd, CPoint point )";

	try
	{
		CRect tree_rect;
		m_rtSceneContents.GetWindowRect( &tree_rect );

		if( !(point.x > tree_rect.left && point.x < tree_rect.right && point.y > tree_rect.top && point.y < tree_rect.bottom) )
			return;

		if( !pBaseScene )
			return;
		
		switch( pBaseScene->GetType() )
		{
			case FT_XSI:
				SceneContextMenuProcXSI( point );
				break;

			case FT_EFN:
				SceneContextMenuProcEFN( point );
				break;

			case FT_EFN_MESHLIST:
				SceneContextMenuProcMSH( point );
				break;

			case FT_SCN:
				SceneContextMenuProcSCN( point );
				break;
		}
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}
}

void CCookieMachineDlg::OnMenuShowMesh()
{
	HTREEITEM hSelected = m_rtSceneContents.GetSelectedItem();
	CString strNodeText;
	CXSIMesh *mesh = NULL;

	//get currently selected treenode name
	if(hSelected >= 0)
	{
		strNodeText = m_rtSceneContents.GetItemText(hSelected);

		//get mesh by name
		mesh = ( (CXSIFile*)pBaseScene )->GetMeshByName(strNodeText.GetBuffer(0));

		if(mesh != NULL)
		{
			m_rtSceneContents.SetItemImage( hSelected, 0, 0 );
			mesh->SetVisibility(true);
			pBaseScene->Prepare(g_pd3dDevice);
		}
	} 
}

void CCookieMachineDlg::OnMenuHideMesh()
{
	HTREEITEM hSelected = m_rtSceneContents.GetSelectedItem();
	CString strNodeText;
	CXSIMesh *mesh = NULL;

	//get currently selected treenode name
	if(hSelected >= 0)
	{
	  strNodeText = m_rtSceneContents.GetItemText(hSelected);

		//get mesh by name
		mesh =  ( (CXSIFile*)pBaseScene )->GetMeshByName(strNodeText.GetBuffer(0));
		if(mesh != NULL)
		{
			m_rtSceneContents.SetItemImage(hSelected, 4, 4);
			mesh->SetVisibility(false);
			 ( (CXSIFile*)pBaseScene )->Prepare(g_pd3dDevice);
		}
	} 
}

void CCookieMachineDlg::OnMenuAbout()
{
	SAFE_DELETE(m_dlgAbout);
	m_dlgAbout = new CAboutBoxDlg(this);
}

void CCookieMachineDlg::OnMenuShowAllMeshes()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_XSI )
		return;

	HTREEITEM	hChildNode;
	HTREEITEM	hMeshesRoot	= m_rtSceneContents.FindTreeItemByName( "Meshes" );
	bool		completed	= false;

	for( int i = 0; i < ( (CXSIFile*)pBaseScene )->GetMeshesCount(); i++ )
	   ( (CXSIFile*)pBaseScene )->GetMeshByIndex(i)->SetVisibility( true ); 

	//in case root node not found
	if( !hMeshesRoot )
		return;

	hChildNode = m_rtSceneContents.GetChildItem( hMeshesRoot );

	//in case child node not found
	if( !hChildNode )
		return;

	while( completed == false )
	{
		m_rtSceneContents.SetItemImage(hChildNode, 0, 0 );
		hChildNode = m_rtSceneContents.GetNextItem( hChildNode, TVGN_NEXT );

		if( hChildNode == NULL )
			break;
	}

	 pBaseScene->Prepare(g_pd3dDevice);
}

void CCookieMachineDlg::OnMenuHideMeshes()
{
	HTREEITEM hChildNode, hMeshesRoot = m_rtSceneContents.FindTreeItemByName( "Meshes" );
	bool completed = false;

	for (int i = 0; i < ((CXSIFile*)pBaseScene)->GetMeshesCount(); i++)
		((CXSIFile*)pBaseScene)->GetMeshByIndex(i)->SetVisibility(false);

		//in case root node not found
	if (!hMeshesRoot)
		return;

	hChildNode = m_rtSceneContents.GetChildItem(hMeshesRoot);

  //in case child node not found
	if (!hChildNode)
		return;

	while (completed == false)
	{
		m_rtSceneContents.SetItemImage(hChildNode, 4, 4); 
		hChildNode = m_rtSceneContents.GetNextItem(hChildNode, TVGN_NEXT);
		
		if (hChildNode == NULL)
			break;
	}

	pBaseScene->Prepare(g_pd3dDevice);
}

void CCookieMachineDlg::D3DCleanup()
{
	SAFE_RELEASE( m_pTextSprite );
	SAFE_RELEASE( m_pFont );
	SAFE_DELETE( m_txt );

	SAFE_RELEASE( g_pGridVB );
	SAFE_RELEASE( g_pd3dDevice );
	SAFE_RELEASE( g_pD3D );
}

void CCookieMachineDlg::OnMenuBackfaceCulling()
{
	if( modelStates.IsCullingOn == true )
	  SwitchCulling( false );
	else
	  SwitchCulling( true );
}

void CCookieMachineDlg::SwitchCulling( bool value )
{	
	CMenu* menuCull = this->GetMenu();

	modelStates.IsCullingOn = value;

	if( g_pd3dDevice )
	{
		if( value )
		{
			g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
			menuCull->CheckMenuItem( ID_MENU_BACKFACE_CULLING, MF_CHECKED );
		}
		else
		{
			g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
			menuCull->CheckMenuItem( ID_MENU_BACKFACE_CULLING, MF_UNCHECKED );
		}
	}	
	
	menuCull = NULL;
}

void CCookieMachineDlg::SwitchRenderMode(RENDER_MODE mode)
{
	CMenu *menuCull = this->GetMenu();

	modelStates.RenderMode = mode;

	if (g_pd3dDevice)
	{
		switch (mode)
		{
			case RM_SHADED:
				g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
				menuCull->CheckMenuItem(ID_T_SHADED, MF_CHECKED); 
				menuCull->CheckMenuItem(ID_T_WIREFRAME, MF_UNCHECKED);
				menuCull->CheckMenuItem(ID_T_WIREFRAME_SHADED, MF_UNCHECKED);
			break;

			case RM_WIREFRAME:
				g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
				menuCull->CheckMenuItem(ID_T_WIREFRAME, MF_CHECKED);
				menuCull->CheckMenuItem(ID_T_SHADED, MF_UNCHECKED);
				menuCull->CheckMenuItem(ID_T_WIREFRAME_SHADED, MF_UNCHECKED);
			break;

			case RM_SHADED_WIREFRAME:
				g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
				menuCull->CheckMenuItem(ID_T_WIREFRAME_SHADED, MF_CHECKED);
				menuCull->CheckMenuItem(ID_T_SHADED, MF_UNCHECKED); 
				menuCull->CheckMenuItem(ID_T_WIREFRAME, MF_UNCHECKED);
			break;
		}; 
	}	
	
	menuCull = NULL;
}

void CCookieMachineDlg::OnTWireframe()
{
	SwitchRenderMode( RM_WIREFRAME );

	if( pBaseScene && pBaseScene->GetType() == FT_EFN_MESHLIST )
	{
		CMesh* pMesh = ((CEfnMeshlist*)pBaseScene)->GetSelectedMesh();

		if( pMesh )
			pMesh->SetRenderMode( RM_WIREFRAME );
	}
}

void CCookieMachineDlg::OnTShaded()
{
	SwitchRenderMode( RM_SHADED );

	if( pBaseScene && pBaseScene->GetType() == FT_EFN_MESHLIST )
	{
		CMesh* pMesh = ((CEfnMeshlist*)pBaseScene)->GetSelectedMesh();

		if( pMesh )
			pMesh->SetRenderMode( RM_SHADED );
	}
}

void CCookieMachineDlg::OnTWireframeShaded()
{
	SwitchRenderMode( RM_SHADED_WIREFRAME );

	if( pBaseScene && pBaseScene->GetType() == FT_EFN_MESHLIST )
	{
		CMesh* pMesh = ((CEfnMeshlist*)pBaseScene)->GetSelectedMesh();

		if( pMesh )
			pMesh->SetRenderMode( RM_SHADED_WIREFRAME );
	}
}

bool CCookieMachineDlg::GridCreate()
{
	if( !g_pd3dDevice )
		return false;

	SAFE_RELEASE( g_pGridVB );

	CVERTEX*	gr_vertecies;
	VOID*		pVert;
	long		baseColor		=  D3DCOLOR_ARGB( 255, GetRValue( m_appSettings.GridWiresColor ), GetGValue( m_appSettings.GridWiresColor ), GetBValue( m_appSettings.GridWiresColor) );
	long		crossColor		=  D3DCOLOR_ARGB( 255, GetRValue( m_appSettings.GridCrossColor ), GetGValue( m_appSettings.GridCrossColor ), GetBValue( m_appSettings.GridCrossColor) );

	int lines_count = m_appSettings.GridSize * 2 + 2;
	g_pGridVB = NULL;

	float fPosOffset = ( m_appSettings.GridSize * m_appSettings.GridCellSize ) / 2;

	gr_vertecies = new CVERTEX[lines_count * 2];

	for (int i = 0; i < lines_count; i += 2)
	{
		gr_vertecies[i].x     = fPosOffset - (i / 2 * m_appSettings.GridCellSize);
		gr_vertecies[i].y     = 0.0;
		gr_vertecies[i].z     = fPosOffset;

		if( ( i % (m_appSettings.GridSize / 2 ) == 0) && i != 0 && i != lines_count - 2 )
			gr_vertecies[i].color = crossColor;
		else
			gr_vertecies[i].color = baseColor;

		gr_vertecies[i + 1].x = fPosOffset - (i / 2 * m_appSettings.GridCellSize);
		gr_vertecies[i + 1].y = 0.0;
		gr_vertecies[i + 1].z = -(fPosOffset);

		if( ( i % (m_appSettings.GridSize / 2 ) == 0) && i != 0 &&  i != lines_count - 2 )
			gr_vertecies[i + 1].color = crossColor;
		else
			gr_vertecies[i + 1].color = baseColor;
	}

	for (int i = lines_count; i < lines_count * 2; i += 2)
	{
		gr_vertecies[i].x     = -(fPosOffset);
		gr_vertecies[i].y     = 0.0;
		gr_vertecies[i].z     = -fPosOffset + ((i - lines_count) / 2 * m_appSettings.GridCellSize);

		if( ( (i - lines_count) % (m_appSettings.GridSize / 2 ) == 0) && (i - lines_count) != 0 && (i - lines_count) != lines_count - 2 )
			gr_vertecies[i].color = crossColor;
		else
			gr_vertecies[i].color = baseColor;

		gr_vertecies[i + 1].x = fPosOffset;
		gr_vertecies[i + 1].y = 0.0;
		gr_vertecies[i + 1].z = -fPosOffset + ((i - lines_count) / 2 * m_appSettings.GridCellSize);
		gr_vertecies[i + 1].color = baseColor;

		if( ( (i - lines_count) % (m_appSettings.GridSize / 2 ) == 0) && (i - lines_count) != 0 && (i - lines_count) != lines_count - 2 )
			gr_vertecies[i + 1].color = crossColor;
		else
			gr_vertecies[i + 1].color = baseColor;
	}

	if( FAILED( g_pd3dDevice->CreateVertexBuffer( sizeof(CVERTEX) * lines_count * 2,
																						D3DUSAGE_WRITEONLY, 
  																					D3DFVF_VERTEX_DESCRIPTOR,
																						D3DPOOL_MANAGED,
																						&g_pGridVB, 
																						NULL ) ) )   return false;

	if( FAILED( g_pGridVB->Lock( 0, sizeof(CVERTEX) * lines_count * 2, (void**)&pVert, 0 ) ) )
		return false;

	memcpy(pVert, gr_vertecies, sizeof(CVERTEX) * lines_count * 2);

	g_pGridVB->Unlock();

	delete []gr_vertecies;
	gr_vertecies = NULL;

	pVert = NULL;

	return 0;
}

void CCookieMachineDlg::SwitchLightingMode( bool lights_on )
{

}

void CCookieMachineDlg::OnMenuFileClose()
{	
	CloseCurrentFile();
	EnableWalkMode(false);
}

void CCookieMachineDlg::SelectedObjectsDelete()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() == FT_EFN )
	{
		CWnd* pWnd = GetFocus();

		if( pWnd->GetDlgCtrlID() != IDC_CURR_Z_OFF )
			DeleteSelectedAnimFrames();
		else
			pWnd->SendMessage( WM_KEYDOWN, VK_DELETE );
	}
	else if( pBaseScene->GetType() == FT_EFN_MESHLIST )
	{
		OnMenuMeshlistDeleteSelectedObjects();
		UpdateStatusBarInfoMESHLIST();
		UpdateMeshesInfoInExplorer();
	}
	else if( pBaseScene->GetType() == FT_SCN )
	{
		OnMenuScnDeleteSelectedObjects();
		UpdateStatusBarInfoSCN();
	}
	else
	{}
}

void CCookieMachineDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	str_t func_decl_str = "void CCookieMachineDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)";

	try
	{
		CString tmp;

		if( !pBaseScene )
			return;

		if( pScrollBar->GetDlgCtrlID() == IDC_ANIM_SLIDER )
		{
			if( pBaseScene->GetType() == FT_XSI )
			{
				((CXSIFile*)pBaseScene)->SetCurrentFrame( m_sliderAnim.GetPos() );
				tmp.Format( "%d", m_sliderAnim.GetPos() );
				m_eFrameCounter.SetWindowText( tmp );

				if( m_pObjPropWnd )
					m_pObjPropWnd->Populate( m_sliderAnim.GetPos() );
			}
			else if( pBaseScene->GetType() == FT_EFN )
			{
				m_bIsPlaying = false;
				((CEfnCharacter*)pBaseScene)->SetCurrNextFrame( m_sliderAnim.GetPos(), m_sliderAnim.GetPos() );
				m_iCurrentFrame = m_sliderAnim.GetPos();


				if( KEYDOWN(VK_CONTROL) )
				{
					m_bScrollSelectionStarted = true;
				}
				else
				{
					m_bScrollSelectionStarted = false;
				}

				if( m_bScrollSelectionStarted )
				{
					if( firstSelFrame == -1 )
					{
						firstSelFrame = m_sliderAnim.GetPos();
						lastSelFrame  = firstSelFrame;
					}
					else
					{
						lastSelFrame  = m_sliderAnim.GetPos();

						m_sliderAnim.ClearSel(true);

						m_sliderAnim.Invalidate(true);
						m_sliderAnim.SetSelection( firstSelFrame, lastSelFrame );
					}

					if (firstSelFrame != -1)
					{
						m_iStartingFrame = firstSelFrame;
						m_iEndingFrame   = lastSelFrame;
					}
				}
				else
				{
					//finished selecting, store values
					firstSelFrame = -1;
					lastSelFrame  = -1;
				}

				int iMn, iMx;
				m_sliderAnim.GetSelection( iMn, iMx );
				
				CString strTemp;

				if( iMn == -1 )
				{
					strTemp.Format( "Цикл анимации: с %d по %d кадр", m_sliderAnim.GetPos(), m_sliderAnim.GetPos() );
					m_iStartingFrame	= m_sliderAnim.GetPos();
					m_iEndingFrame		= m_sliderAnim.GetPos();
				}
				else
				{
					if( firstSelFrame == -1 && lastSelFrame == -1 ) 
						strTemp.Format( "Цикл анимации: с %d по %d кадр", m_iStartingFrame, m_iEndingFrame );
					else
						strTemp.Format( "Цикл анимации: с %d по %d кадр", firstSelFrame, lastSelFrame );				
				}

				m_StatusBar->SetPaneText( 4, strTemp, true );

				tmp.Format( "%f", ((CEfnCharacter*)pBaseScene)->GetZOffset( m_sliderAnim.GetPos()) );
				m_eFrameOffset.SetWindowText( tmp );

				tmp.Format( "%d",m_sliderAnim.GetPos() );
				m_eFrameCounter.SetWindowText( tmp );

				if( m_pObjPropWnd )
					m_pObjPropWnd->Populate( m_sliderAnim.GetPos(), m_iStartingFrame, m_iEndingFrame );
			}
			else
			{}
		}

		CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}
}

void CCookieMachineDlg::UpdateRecentFilesMenu()
{
	CMenu* menu = this->GetMenu();
	CMenu* file_menu = menu->GetSubMenu( 0 );
	CMenu* recent_files_menu = file_menu->GetSubMenu( 4 );

	int ids[9];

	CString result;

	ids[0] = ID_MENU_RECENT_FILE_1;
	ids[1] = ID_MENU_RECENT_FILE_2;
	ids[2] = ID_MENU_RECENT_FILE_3;
	ids[3] = ID_MENU_RECENT_FILE_4;
	ids[4] = ID_MENU_RECENT_FILE_5;
	ids[5] = ID_MENU_RECENT_FILE_6;
	ids[6] = ID_MENU_RECENT_FILE_7;
	ids[7] = ID_MENU_RECENT_FILE_8;
	ids[8] = ID_MENU_RECENT_FILE_9;

	//delete all recent menu items
	int menu_children_count = recent_files_menu->GetMenuItemCount();

	for( int i = 1; i < menu_children_count; i++ )
		recent_files_menu->DeleteMenu( 1, MF_BYPOSITION );

	//insert "recent files" entries to menu
	if( m_appSettings.RecentFiles.empty() == false )
	{
		for( UINT i = 0; i < m_appSettings.RecentFiles.size(); i++  )
		{
			result.Format( "%i %s", i + 1, m_appSettings.RecentFiles[i].GetBuffer() );
			recent_files_menu->InsertMenu( -1, MF_BYPOSITION | MF_STRING, ids[i], result.GetBuffer() );
		}

		recent_files_menu->InsertMenu( -1, MF_BYPOSITION | MF_SEPARATOR, 0, "" );
	}

	SettingsWriteToFile();
}

void CCookieMachineDlg::UpdateRecentDirsMenu()
{
	CMenu* menu = this->GetMenu();
	CMenu* file_menu = menu->GetSubMenu( 0 );
	CMenu* recent_files_menu = file_menu->GetSubMenu( 5 );

	int ids[9];

	CString result;

	ids[0] = ID_MENU_RECENT_DIR_1;
	ids[1] = ID_MENU_RECENT_DIR_2;
	ids[2] = ID_MENU_RECENT_DIR_3;
	ids[3] = ID_MENU_RECENT_DIR_4;
	ids[4] = ID_MENU_RECENT_DIR_5;
	ids[5] = ID_MENU_RECENT_DIR_6;
	ids[6] = ID_MENU_RECENT_DIR_7;
	ids[7] = ID_MENU_RECENT_DIR_8;
	ids[8] = ID_MENU_RECENT_DIR_9;

	//delete all recent menu items
	int menu_children_count = recent_files_menu->GetMenuItemCount();

	for( int i = 1; i < menu_children_count; i++ )
		recent_files_menu->DeleteMenu( 1, MF_BYPOSITION );

	//insert "recent files" entries to menu
	if( m_appSettings.RecentDirs.empty() == false )
	{
		for( UINT i = 0; i < m_appSettings.RecentDirs.size(); i++  )
		{
			result.Format( "%i %s", i + 1, m_appSettings.RecentDirs[i].GetBuffer() );
			recent_files_menu->InsertMenu( -1, MF_BYPOSITION | MF_STRING, ids[i], result.GetBuffer() );
		}

		recent_files_menu->InsertMenu( -1, MF_BYPOSITION | MF_SEPARATOR, 0, "" );
	}

	SettingsWriteToFile();
}

void CCookieMachineDlg::EnableControls( bool value )
{
	CMenu* menu = this->GetMenu();

	// disable most menu items
	EnableMenuItem( menu, ID_MENU_CONV_TO_EFN_MESHLIST, false );
	EnableMenuItem( menu, ID_MENU_CONV_TO_EFN_ANIM, false );
	EnableMenuItem( menu, ID_MENU_CONV_TO_SCN, false );
	EnableMenuItem( menu, ID_CLOSE, false );
	EnableMenuItem( menu, ID_MENU_SAVE, false );
	EnableMenuItem( menu, ID_MENU_SAVE_AS, false );
	EnableMenuItem( menu, ID_T_WIREFRAME, false );
	EnableMenuItem( menu, ID_T_SHADED, false );
	EnableMenuItem( menu, ID_T_WIREFRAME_SHADED, false );
	EnableMenuItem( menu, ID_MENU_SCALE, false );
	EnableMenuItem( menu, ID_MENU_SCALE_NEW, false );
	EnableMenuItem( menu, ID_MENU_DISCARD_VERT_COL, false );
	EnableMenuItem( menu, ID_MENU_CONV_COORDS_SYS, false );
	EnableMenuItem( menu, ID_MENU_TEST_WALK, false );
	EnableMenuItem( menu, ID_MENU_SHOW_AABB, false );
	EnableMenuItem( menu, ID_MENU_SHOW_BSPHERE, false );
	EnableMenuItem( menu, ID_MENU_TEST_WALK, false );	
	EnableMenuItem( menu, ID_MENU_RECALC_GLOBAL_RADIUS, false );	
	EnableMenuItem( menu, ID_MENU_IMPORT, false );	
	EnableMenuItem( menu, ID_MENU_START_MACRO_REC, false );	
	EnableMenuItem( menu, ID_MENU_END_MACRO_REC, false );	
	EnableMenuItem( menu, ID_MENU_LOAD_MACRO, false );
	EnableMenuItem( menu, ID_MENU_SCALE_Z_OFFSETS, false );
		

	EnableMenuItem( &m_menuViewPopup, ID_MENU_VIEW_GRID, false );
	EnableMenuItem( &m_menuViewPopup, ID_MENU_VIEW_FRAMERATE, false );
	EnableMenuItem( &m_menuViewPopup, ID_MENU_VIEW_LIGHTS, false );
	EnableMenuItem( &m_menuViewPopup, ID_MENU_VIEW_CAMERAS, false );
	EnableMenuItem( &m_menuViewPopup, ID_MENU_VIEW_NULLS, false );
	EnableMenuItem( &m_menuViewPopup, ID_MENU_VIEW_CURVES, false );
	EnableMenuItem( &m_menuViewPopup, ID_MENU_VIEW_CHECKALLOBJECTS, false );
	EnableMenuItem( &m_menuViewPopup, ID_MENU_VIEW_UNCHECKALLOBJECTS, false );
	EnableMenuItem( &m_menuViewPopup, ID_MENU_VIEW_NORMALS, false );
	EnableMenuItem( &m_menuViewPopup, ID_MENU_VIEW_BOUNDINGBOX, false );
	EnableMenuItem( &m_menuViewPopup, ID_MENU_VIEW_BOUNDINGSPHERE, false );
	EnableMenuItem( &m_menuViewPopup, ID_MENU_VIEW_SHADED, false );
	EnableMenuItem( &m_menuViewPopup, ID_MENU_VIEW_WIREFRAME, false );
	EnableMenuItem( &m_menuViewPopup, ID_MENU_VIEW_TEXTURED, false );
	EnableMenuItem( &m_menuViewPopup, ID_MENU_VIEW_HIDDENLINEREMOVAL, false );
	EnableMenuItem( &m_menuViewPopup, ID_MENU_VIEW_SPOTLIGHTCONE, false );

	
	menu->CheckMenuItem( ID_MENU_SHOW_AABB, MF_UNCHECKED ); 
	menu->CheckMenuItem( ID_MENU_SHOW_BSPHERE, MF_UNCHECKED );

	m_eFrameCounter.EnableWindow( false );
	m_eFrameOffset.EnableWindow( false );
	m_btnStopAnim.EnableWindow( false );
	m_btnPlayAnim.EnableWindow( false );
	m_btnLoopAnim.EnableWindow( false );
	m_btnSetOffset.EnableWindow( false );
	m_btnMoveAnim.EnableWindow( false );
	m_sliderAnim.EnableWindow( false );
	m_btnTransitionAnim.EnableWindow( false );

	m_eFrameCounter.ShowWindow( SW_SHOW );
	m_eFrameOffset.ShowWindow( SW_SHOW );
	m_btnStopAnim.ShowWindow( SW_SHOW );
	m_btnPlayAnim.ShowWindow( SW_SHOW );
	m_btnLoopAnim.ShowWindow( SW_SHOW );
	m_btnSetOffset.ShowWindow( SW_SHOW );
	m_btnMoveAnim.ShowWindow( SW_SHOW );
	m_sliderAnim.ShowWindow( SW_SHOW );
	m_btnTransitionAnim.ShowWindow( SW_SHOW );


	EnableToolbarButton( IDT_FILESAVE, false );
	EnableToolbarButton( IDT_FILESAVEALL, false );
	EnableToolbarButton( IDT_FILECONV_2EFN, false );
	EnableToolbarButton( IDT_FILECONV_2MSH, false );
	EnableToolbarButton( IDT_FILECONV_2SCN, false );
	EnableToolbarButton( IDT_UNDO, false );
	EnableToolbarButton( IDT_REDO, false );
	EnableToolbarButton( IDT_BINDS, false );
	EnableToolbarButton( IDT_SCRIPTS, false );
	EnableToolbarButton( IDT_FLOORS, false );
	EnableToolbarButton( IDT_TEXT_RES, false );

	m_bNormalsRendered = false;
	m_menuViewPopup.CheckMenuItem( ID_MENU_VIEW_NORMALS, MF_UNCHECKED );

	// in case no files opened -- do nothing
	if( !pBaseScene )
		return;

	switch( pBaseScene->GetType() )
	{
		//in case XSI file opened
		case FT_XSI:
			if( value )
			{			
				EnableMenuItem( menu, ID_MENU_CONV_TO_EFN_MESHLIST, true );
				EnableMenuItem( menu, ID_MENU_CONV_TO_EFN_ANIM, true );
				EnableMenuItem( menu, ID_MENU_CONV_TO_SCN, true );
				EnableMenuItem( menu, ID_CLOSE, true );
				EnableMenuItem( menu, ID_T_WIREFRAME, true );
				EnableMenuItem( menu, ID_T_SHADED, true );
				EnableMenuItem( menu, ID_T_WIREFRAME_SHADED, true );

				EnableToolbarButton( IDT_FILECONV_2EFN, true );
				EnableToolbarButton( IDT_FILECONV_2MSH, true );
				EnableToolbarButton( IDT_FILECONV_2SCN, true );

				m_sliderAnim.EnableWindow( true );
			}
			else
			{}
		break;


		//in case EFN file opened
		case FT_EFN:
				if( value )
				{			
					EnableMenuItem( menu, ID_MENU_SAVE_AS, true );
					EnableMenuItem( menu, ID_CLOSE, true );
					EnableMenuItem( menu, ID_T_WIREFRAME, true );
					EnableMenuItem( menu, ID_T_SHADED, true );
					EnableMenuItem( menu, ID_T_WIREFRAME_SHADED, true );
					EnableMenuItem( menu, ID_MENU_SHOW_AABB, true );
					EnableMenuItem( menu, ID_MENU_SHOW_BSPHERE, true );
					EnableMenuItem( menu, ID_MENU_SAVE, true );
					EnableMenuItem( menu, ID_MENU_CONV_COORDS_SYS, true );
					EnableMenuItem( menu, ID_MENU_TEST_WALK, true );
					EnableMenuItem( menu, ID_MENU_SCALE, true );
					EnableMenuItem( menu, ID_MENU_RECALC_GLOBAL_RADIUS, true );
					EnableMenuItem( menu, ID_MENU_DISCARD_VERT_COL, true );
					EnableMenuItem( menu, ID_MENU_START_MACRO_REC, true );	
					EnableMenuItem( menu, ID_MENU_END_MACRO_REC, true );	
					EnableMenuItem( menu, ID_MENU_LOAD_MACRO, true );
					EnableMenuItem( menu, ID_MENU_SCALE_NEW, true );
					EnableMenuItem( &m_menuViewPopup, ID_MENU_VIEW_NORMALS, true );
					EnableMenuItem( menu, ID_MENU_SCALE_Z_OFFSETS, true );

					m_eFrameOffset.EnableWindow( true );
					m_btnStopAnim.EnableWindow( true );
					m_btnPlayAnim.EnableWindow( true );
					m_btnLoopAnim.EnableWindow( true );
					m_btnTransitionAnim.EnableWindow( true );
					m_btnSetOffset.EnableWindow( true );
					m_btnMoveAnim.EnableWindow( true );
					m_sliderAnim.EnableWindow( true );

					EnableToolbarButton( IDT_FILESAVE, true );
					EnableToolbarButton( IDT_FILESAVEALL, true );
					EnableToolbarButton( IDT_SCRIPTS, true );
				}
				else
				{}
		break;

		//in case SCN file opened
		case FT_SCN:
				if( value )
				{
					EnableMenuItem( menu, ID_MENU_SAVE, true );
					EnableMenuItem( menu, ID_CLOSE, true );
					EnableMenuItem( menu, ID_MENU_SAVE_AS, true );
					EnableMenuItem( menu, ID_T_WIREFRAME, true );
					EnableMenuItem( menu, ID_T_SHADED, true );
					EnableMenuItem( menu, ID_T_WIREFRAME_SHADED, true );
					EnableMenuItem( menu, ID_MENU_SHOW_AABB, true );
					EnableMenuItem( menu, ID_MENU_SHOW_BSPHERE, true );
					EnableMenuItem( menu, ID_MENU_IMPORT, true );
					EnableMenuItem( &m_menuViewPopup, ID_MENU_VIEW_SPOTLIGHTCONE, true );
					EnableMenuItem( menu, ID_MENU_SCALE_NEW, true );

					EnableToolbarButton( IDT_FILESAVE, true );
					EnableToolbarButton( IDT_BINDS, true );
					EnableToolbarButton( IDT_SCRIPTS, true );
					EnableToolbarButton( IDT_FLOORS, true );
					EnableToolbarButton( IDT_TEXT_RES, true );

					m_eFrameCounter.ShowWindow( SW_HIDE );
					m_eFrameOffset.ShowWindow( SW_HIDE );
					m_btnStopAnim.ShowWindow( SW_HIDE );
					m_btnPlayAnim.ShowWindow( SW_HIDE );
					m_btnLoopAnim.ShowWindow( SW_HIDE );
					m_btnSetOffset.ShowWindow( SW_HIDE );
					m_btnMoveAnim.ShowWindow( SW_HIDE );
					m_sliderAnim.ShowWindow( SW_HIDE );
					m_btnTransitionAnim.ShowWindow( SW_HIDE );
				}
				else
				{}
		break;

		//in case FT_EFN_MESHLIST file opened
		case FT_EFN_MESHLIST:
				if( value )
				{
					EnableMenuItem( menu, ID_MENU_SAVE_AS, true );
					EnableMenuItem( menu, ID_CLOSE, true );
					EnableMenuItem( menu, ID_T_WIREFRAME, true );
					EnableMenuItem( menu, ID_T_SHADED, true );
					EnableMenuItem( menu, ID_T_WIREFRAME_SHADED, true );
					EnableMenuItem( menu, ID_MENU_SHOW_AABB, true );
					EnableMenuItem( menu, ID_MENU_SHOW_BSPHERE, true );
					EnableMenuItem( menu, ID_MENU_SAVE, true );
					EnableMenuItem( menu, ID_MENU_IMPORT, true );	
					EnableMenuItem( menu, ID_MENU_SCALE_NEW, true );
					EnableMenuItem( &m_menuViewPopup, ID_MENU_VIEW_NORMALS, true );
					EnableToolbarButton( IDT_FILESAVE, true );
				}
				else
				{}
		break;
	}
}

void CCookieMachineDlg::OnMenuConvToEfn()
{
	CValidateDlg*	dlgValidate		= NULL;
	CModelNameDlg*	dlgModelName	= NULL;
	CMenu*			menu			= this->GetMenu();
	CEfnCharacter*	efn_character	= NULL;

	//make sure current file type is XSI
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_XSI )
		return;

	//show validation dialog
	if( !dlgValidate )
	{
		dlgValidate = new CValidateDlg(this);
		dlgValidate->SetMessageStr("Конвертация...");
	}

	//create new efn char
	efn_character = new CEfnCharacter;

	//init efn from xsi character
	if( !efn_character->InitFromXSICharacter( (CXSIFile*)pBaseScene ) )
	{
		SAFE_DELETE( efn_character );
		SAFE_DELETE( dlgValidate );
		MessageBox("Ошибка конвертации!", "Внимание!", MB_OK | MB_ICONERROR);
		return;
	}

	//dlgModelName = new CModelNameDlg(this);
	//dlgModelName->SetCaption("Укажите имя модели...");
	//dlgModelName->DoModal();

	m_ObjPropWnd = OWE_UNDEFINED;
	ShowObjectPropertiesWnd( false );

	CString tmpName = "model";//dlgModelName->GetString();
	efn_character->SetModelName(tmpName.GetBuffer(0));
	efn_character->SetPaths( pBaseScene->GetFolder(), pBaseScene->GetFilename() );

	//SAFE_DELETE( dlgModelName );
	SAFE_DELETE( pBaseScene );

	efn_character->SetParentWnd( this );
	efn_character->Prepare( g_pd3dDevice );
	pBaseScene = efn_character;

	pBaseScene->SetPaths( pBaseScene->GetFolder(), "" );

	this->SetWindowText( "Cookie Machine -- не указано" );
	
	UpdateUI();

	EnableControls( true );

	EnableMenuItem( menu, ID_MENU_SAVE, false );
	EnableToolbarButton( IDT_FILESAVE, true ); 

	//delete validation window
	SAFE_DELETE( dlgValidate );
}

void CCookieMachineDlg::UpdateUI_XSI(void)
{
	HTREEITEM		hInfo, hMeshes, hLights, hCameras, hNulls, hNull, hTemp;
	UINT			uiMask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

	char	szTemp[128];
	int     iTemp, iTemp2;
	float   a, r, g, b;


	/*-------------- clean up --------------- */
	m_rtSceneContents.DeleteAllItems();
	iTemp	= 0;
	iTemp2	= 0;

	/*-------------- insert root nodes ------ */
	hInfo		= m_rtSceneContents.InsertItem( uiMask, "Info", 23, 23, 0, 0, 0, NULL, NULL );
	hMeshes		= m_rtSceneContents.InsertItem( uiMask, "Meshes", 1, 1, 0, 0, 0, NULL, NULL );
	hNulls		= m_rtSceneContents.InsertItem( uiMask, "Nulls", 1, 1, 0, 0, 0, NULL, NULL );
	hLights		= m_rtSceneContents.InsertItem( uiMask, "Lights", 1, 1, 0, 0, 0, NULL, NULL );
	hCameras	= m_rtSceneContents.InsertItem( uiMask, "Cameras", 1, 1, 0, 0, 0, NULL, NULL );


	/*-------------- fill *CAMERAS* node ----- */
	for( int i = 0; i < ((CXSIFile*)pBaseScene)->GetCamerasCount(); i++ )
	{
		CCAMERA* temp_camera = ((CXSIFile*)pBaseScene)->GetCameraByIndex(i);

		hTemp = m_rtSceneContents.InsertItem( uiMask, temp_camera->name.data(), (temp_camera->visible ? 5 : 11), 
												(temp_camera->visible ? 5 : 11), 0, 0, 0, hCameras, NULL );


		sprintf( szTemp, "Pos: x:%f  y:%f  z:%f", temp_camera->pos.x, temp_camera->pos.y, temp_camera->pos.z );
		m_rtSceneContents.InsertItem( uiMask, szTemp, 3, 3, 0, 0, 0, hTemp, NULL );

		sprintf( szTemp, "Dir: x:%f  y:%f  z:%f", temp_camera->dir.x, temp_camera->dir.y, temp_camera->dir.z );
		m_rtSceneContents.InsertItem( uiMask, szTemp, 3, 3, 0, 0, 0, hTemp, NULL );

		sprintf( szTemp, "Up vec: x:%f  y:%f  z:%f", temp_camera->upvector.x, temp_camera->upvector.y, temp_camera->upvector.z );
		m_rtSceneContents.InsertItem( uiMask, szTemp, 3, 3, 0, 0, 0, hTemp, NULL );

		sprintf( szTemp, "FOV H: %f", temp_camera->hfov );
		m_rtSceneContents.InsertItem( uiMask, szTemp, 3, 3, 0, 0, 0, hTemp, NULL );

		sprintf( szTemp, "FOV V: %f", temp_camera->vfov );
		m_rtSceneContents.InsertItem( uiMask, szTemp, 3, 3, 0, 0, 0, hTemp, NULL );
	}

	/*-------------- fill *LIGHTS* node ----- */
	for (int i = 0; i < ((CXSIFile*)pBaseScene)->GetLightsCount(); i++)
	{
		CLIGHT* temp_light = ((CXSIFile*)pBaseScene)->GetLightByIndex(i);

		// insert light node
		hTemp = m_rtSceneContents.InsertItem( uiMask, temp_light->name.data(), (temp_light->visible ? 9 : 11), (temp_light->visible ? 9 : 11), 
												0, 0, 0, hLights, NULL );

		//insert light type
		str_t type = "Type: OMNI";

		if( temp_light->type == LT_SPOT )
			type = "Type: SPOT";

		m_rtSceneContents.InsertItem( uiMask, type.data(), 3, 3, 0, 0, 0, hTemp, NULL );

		//insert light position
		sprintf( szTemp, "Pos: x:%f  y:%f  z:%f", temp_light->pos.x, temp_light->pos.y, temp_light->pos.z );
		m_rtSceneContents.InsertItem( uiMask, szTemp, 3, 3, 0, 0, 0, hTemp, NULL );
		
		//insert light direction
		sprintf( szTemp, "Dir: x:%f  y:%f  z:%f", temp_light->targ.x, temp_light->targ.y, temp_light->targ.z );
		m_rtSceneContents.InsertItem( uiMask, szTemp, 3, 3, 0, 0, 0, hTemp, NULL );
	}
	
	/*-------------- fill *NULLS* node ----- */
	for (int i = 0; i < ((CXSIFile*)pBaseScene)->GetNullsCount(); i++)
	{
		CNULL2* temp_null = ((CXSIFile*)pBaseScene)->GetNullByIndex( i );

		//insert null (name)
		hNull = m_rtSceneContents.InsertItem( uiMask, temp_null->id.data(), 10, 10, 0, 0, 0, hNulls, NULL );

		bool root_added = false;

		for( int i = 0; i < ((CXSIFile*)pBaseScene)->GetCustParamsCount(); i++  )
		{
			CCUSTPARAM* par = ((CXSIFile*)pBaseScene)->GetCustParamByIndex( i );
			int offset = par->id.find( ".", 0 );

			if( par->id.substr( 0, offset ) == temp_null->id )
			{
				str_t par_id = par->id.substr( offset + 1, par->id.length() - (offset + 1 ) );

				if( !root_added )
				{
					hTemp = m_rtSceneContents.InsertItem( uiMask, "Custom params", 25, 25, 0, 0, 0, hNull, NULL );
					root_added = true;
				}

				if( ((CXSIFile*)pBaseScene)->IsCustParamAnimated( i ) )
					m_rtSceneContents.InsertItem( uiMask, par_id.data(), 26, 26, 0, 0, 0, hTemp, NULL );
				else
					m_rtSceneContents.InsertItem( uiMask, par_id.data(), 27, 27, 0, 0, 0, hTemp, NULL );
			}
		}
	}

	/*-------------- fill *MESHES* node ----- */
	//insert and check *mesh* objects
	for( int i = 0; i < ((CXSIFile*)pBaseScene)->GetMeshesCount(); i++ )
	{
		CXSIMesh* pMesh = ((CXSIFile*)pBaseScene)->GetMeshByIndex(i);

		//insert mesh node
		hTemp = m_rtSceneContents.InsertItem( uiMask, pMesh->GetName(), (pMesh->IsVisible() ? 0 : 4), (pMesh->IsVisible() ? 0 : 4),
												0, 0, 0, hMeshes, NULL );

		//increment total tri counter
		iTemp += pMesh->GetTrianglesCount();

		//triangles
		sprintf( szTemp, "Triangles: %d", pMesh->GetTrianglesCount() );
		m_rtSceneContents.InsertItem( uiMask, szTemp, 3, 3, 0, 0, 0, hTemp, NULL );

		iTemp2 += pMesh->GetVerteciesCount();

		//vertecies
		sprintf( szTemp, "Vertecies: %d", pMesh->GetVerteciesCount() );
		m_rtSceneContents.InsertItem( uiMask, szTemp, 3, 3, 0, 0, 0, hTemp, NULL );

		//color
		pMesh->GetColor( a, r, g, b );

		//convert to 255 notation
		r = (float)(255.0 * r);
		g = (float)(255.0 * g);
		b = (float)(255.0 * b);

		sprintf( szTemp, "Color: RGB(%d, %d, %d)", (int)r, (int)g, (int)b );
		m_rtSceneContents.InsertItem( uiMask, szTemp, 3, 3, 0, 0, 0, hTemp, NULL );
	}  

	/*-------------- fill *INFO* node ------- */
	sprintf( szTemp, "File: %s", pBaseScene->GetFilename().data() );
	m_rtSceneContents.InsertItem( uiMask, szTemp, 3, 3, 0, 0, 0, hInfo, NULL );

	sprintf( szTemp, "Format: dotXSI" );
	m_rtSceneContents.InsertItem( uiMask, szTemp, 3, 3, 0, 0, 0, hInfo, NULL);

	sprintf(szTemp, "Frames: %d", ((CXSIFile*)pBaseScene)->GetFramesCount());
	m_rtSceneContents.InsertItem( uiMask, szTemp, 3, 3, 0, 0, 0, hInfo, NULL );

	//insert triangles in model
	sprintf( szTemp, "Triangles in scene: %d", iTemp );
	m_rtSceneContents.InsertItem( uiMask, szTemp, 3, 3, 0, 0, 0, hInfo, NULL );

	//insert vertecies in model
	sprintf( szTemp, "Vertecies in scene: %d", iTemp2 );
	m_rtSceneContents.InsertItem( uiMask, szTemp, 3, 3, 0, 0, 0, hInfo, NULL );

	////approximate resulting size in KB

	////count * x, y, x * (4 bytes : float)
	//unsigned int model_size_vert = (iTemp2 * 3 * 8); 
	////count * x, y, x, length * (4 bytes : float)
	//unsigned int model_size_norm = (iTemp2 * 4 * 8);
	////(tricount * 3) -- tri array length * (2 bytes : int)
	//unsigned int tri_array_size =  (iTemp * 4 * 4);

	//unsigned int model_size = model_size_vert + model_size_norm + tri_array_size;

	//model_size = (model_size * ((CXSIFile*)pBaseScene)->GetFramesCount()) / 1024;

	//sprintf(szTemp, "Resulting size (Kb): %d", model_size);
	//hMeshInfo = m_rtSceneContents.InsertItem(TVIF_TEXT, _T(szTemp), 0, 0, 0, 0, 0, hInfo, NULL);
	//m_rtSceneContents.SetItemImage(hMeshInfo, 3, 3);

	//expand nodes
	//m_rtSceneContents.Expand(hMeshes, TVE_EXPAND);
	m_rtSceneContents.Expand(hInfo, TVE_EXPAND);

	//update controls 
	m_sliderAnim.ClearSel(true);
	m_sliderAnim.SetRange(0, ((CXSIFile*)pBaseScene)->GetFramesCount() - 1, 1);
	m_sliderAnim.SetSelection(0, ((CXSIFile*)pBaseScene)->GetFramesCount() - 1);

	m_sliderAnim.SetPos(0);
	m_sliderAnim.Invalidate(true);

	firstSelFrame = -1;
	lastSelFrame  = -1;

	m_eFrameCounter.SetWindowText("0");

	m_cmbCameras.ResetContent();
	m_cmbCameras.AddString( DEFAULT_CAMERA_ID );

	//fill cameras combobox
	for( int i = 0; i < ((CXSIFile*)pBaseScene)->GetCamerasCount(); i++ )
	{
		CCAMERA* cam = ((CXSIFile*)pBaseScene)->GetCameraByIndex( i );

		m_cmbCameras.AddString( cam->name.data() );
	}

	m_cmbCameras.SetCurSel( 0 );
}

void CCookieMachineDlg::UpdateUI_EFN()
{	
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN )
		return;

	// clear tree view
	m_rtSceneContents.DeleteAllItems();

	CEfnCharacter*	pEfnAnim = (CEfnCharacter*)pBaseScene;

	HTREEITEM		hInfo,  hAnim,  hTempItem, hNulls, hNull;
	char			szTemp[64];
	TNODE*			node = NULL;

	// insert root nodes
	hInfo	= m_rtSceneContents.InsertItem2( "Info", 23 );
	hNulls	= m_rtSceneContents.InsertItem2( "Nulls", 1, new TNODE( TREENODE::N_NULLS, pBaseScene ) );

	m_rtSceneContents.InsertItem2( "Mesh", 0, new TNODE( TREENODE::N_MESH, pBaseScene ) );

	hAnim = m_rtSceneContents.InsertItem2( "Animations", 5, new TNODE( TREENODE::N_ANIMATIONS, pBaseScene ) );

	m_rtSceneContents.InsertItem2( "Material", 16, new TNODE( TREENODE::N_MATERIAL, pBaseScene ) );
	m_rtSceneContents.InsertItem2( "Texture", 21, new TNODE( TREENODE::N_TEXTURE, pBaseScene ) );


	/********************** INFO ***************************/
	str_t filename = pBaseScene->GetFilename();

	if( filename == "" )
		filename = "не указано";
	

	m_rtSceneContents.InsertItem2( ("File: " + filename).data(), 3, NULL, hInfo, NULL );
	m_rtSceneContents.InsertItem2( "Format: EFN", 3, NULL, hInfo, NULL );

	sprintf( szTemp, "Version: [%s]", pEfnAnim->GetVersion().data() );
	m_rtSceneContents.InsertItem2( szTemp, 3, NULL, hInfo, NULL );

	if( pEfnAnim->GetCompressionType() == ECT_NONE )
		sprintf( szTemp, "Compression: [%s]", "none" );
	else if( pEfnAnim->GetCompressionType() == ECT_SHORT )
		sprintf( szTemp, "Compression: [%s]", "Short" );
	else if( pEfnAnim->GetCompressionType() == ECT_SHORT_RLE )
		sprintf( szTemp, "Compression: [%s]", "Short + RLE" );

	m_rtSceneContents.InsertItem2( szTemp, 3, NULL, hInfo, NULL );


	sprintf( szTemp, "Frames: %d", pEfnAnim->GetFramesCount() );
	m_rtSceneContents.InsertItem2( szTemp, 3, NULL, hInfo, NULL );

	sprintf( szTemp, "Triangles in model: %d", pEfnAnim->GetTrianglesCount() );
	m_rtSceneContents.InsertItem2( szTemp, 3, NULL, hInfo, NULL );

	sprintf( szTemp, "Vertecies in model: %d", pEfnAnim->GetVerteciesCount() );
	m_rtSceneContents.InsertItem2( szTemp, 3, NULL, hInfo, NULL );

	sprintf( szTemp, "Global radius: %0.4f", pEfnAnim->GetGlobalRadius() );
	m_rtSceneContents.InsertItem2( szTemp, 3, new TNODE( TREENODE::N_GLOB_RADIUS, pBaseScene ), hInfo, NULL );

	sprintf( szTemp, "Model name: %s", pEfnAnim->GetModelName() );
	m_rtSceneContents.InsertItem2( szTemp, 3, new TNODE( TREENODE::N_MODEL_NAME, pBaseScene ), hInfo, NULL );

	sprintf( szTemp, "FPS: %.2f", pEfnAnim->GetFPS() );
	m_rtSceneContents.InsertItem2( szTemp, 3, new TNODE( TREENODE::N_FPS, pBaseScene ), hInfo, NULL );

	/********************** NULLS ********************************/
	for( int i = 0; i < pEfnAnim->GetNullsCount(); i++ )
	{
		CNULL2* temp_null = pEfnAnim->GetNullByIndex( i );

		//insert null (name)
		hNull = m_rtSceneContents.InsertItem2( temp_null->id.data(), 10, new TNODE( TREENODE::N_NULL, pBaseScene ), hNulls, TVI_SORT );

		bool root_added = false;

		for( int i = 0; i < pEfnAnim->GetCustParamsCount(); i++  )
		{
			CCUSTPARAM* par = pEfnAnim->GetCustParamByIndex(i);
			int offset = par->id.find( ".", 0 );

			if( par->id.substr( 0, offset ) == temp_null->id )
			{
				str_t par_id = par->id.substr( offset + 1, par->id.length() - (offset + 1 ) );

				if( !root_added )
				{
					hTempItem = m_rtSceneContents.InsertItem2( "Custom params", 25, new TNODE( TREENODE::N_CUST_PARAMS, pBaseScene ), hNull, NULL );
					root_added = true;
				}

				if( pEfnAnim->IsCustParamAnimated( i ) )
				{
					m_rtSceneContents.InsertItem2( par_id.data(), 26, new TNODE( TREENODE::N_CUST_PARAM, pBaseScene ), hTempItem, NULL );
				}
				else
				{
					m_rtSceneContents.InsertItem2( par_id.data(), 27, new TNODE( TREENODE::N_CUST_PARAM, pBaseScene ), hTempItem, NULL );
				}
			}
		}

		//if( temp_null->attachment )
		//{
		//	hTempItem = m_rtSceneContents.InsertItem( uiMask, "Filename:", 3, 3, 0, 0, 0, hNull, NULL);
		//}
	}

	/********************** ANIMATIONS ***************************/
	for( int i = 0; i < pEfnAnim->GetAnimationsCount(); i++ )
	{
		EFN_ANIMATION* pAnim = pEfnAnim->GetAnimationByIndex(i);

		if( !pAnim )
			continue;

		hTempItem = m_rtSceneContents.InsertItem2( pAnim->name.data(), 8, new TNODE( TREENODE::N_ANIMATION, pBaseScene ), hAnim, TVI_SORT );
	}

	//expand info and animations node
	m_rtSceneContents.Expand(hInfo, TVE_EXPAND);
	//m_rtSceneContents.Expand(hAnim, TVE_EXPAND);

	int frames_cout = pEfnAnim->GetFramesCount() - 1;

	/* cleanup */
	firstSelFrame		= -1;
    lastSelFrame		= -1;
	m_iStartingFrame	= 0;
	m_iEndingFrame		= frames_cout;

	m_eFrameCounter.SetWindowText( "0" );

	m_sliderAnim.SetRange( 0, frames_cout, 1 );
	m_sliderAnim.SetPos( 0 );
	m_sliderAnim.ClearSel( true );

	m_cmbCameras.ResetContent();
	m_cmbCameras.AddString( DEFAULT_CAMERA_ID );
	m_cmbCameras.SetCurSel( 0 );
	/* cleanup END */

	m_sliderAnim.SetSelection( 0, frames_cout );
	m_sliderAnim.Invalidate( true );

	CString temp;
	temp.Format( "%f", pEfnAnim->GetZOffset(0) );

	m_eFrameOffset.SetWindowText( temp );

	UpdateStatusBarInfoEFN();
}

void CCookieMachineDlg::OnMenuFileSaveAs()
{
	if( !pBaseScene )
		return;

	CMenu*	menu	= this->GetMenu();
	CString strFilter;
	CString ModelPath	= "";
	CString ModelFile	= "";
	str_t	save_path	= "";

	CFile tmp;

	if( pBaseScene->GetType() == FT_EFN || pBaseScene->GetType() == FT_EFN_MESHLIST )
		strFilter = "EFN Files (*.efn)|*.efn|All Files (*.*)|*.*||";
	
	if( pBaseScene->GetType() == FT_SCN )
		strFilter = "Scene Files (*.scn)|*.scn|All Files (*.*)|*.*||";


	CCustomFileDialog* dlg = new CCustomFileDialog( FALSE, "*.efn", 0, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_ENABLETEMPLATE | OFN_ENABLESIZING, strFilter, NULL );
	dlg->m_pOFN->lpstrInitialDir	= m_appSettings.FilesPath;
	//dlg->m_pOFN->lpfnHook			= OpenDlgExProc;
	dlg->m_pOFN->FlagsEx			= OFN_EX_NOPLACESBAR;
	dlg->m_pOFN->lpTemplateName		= MAKEINTRESOURCE( IDD_OPENFILE_EXT );
	dlg->m_pOFN->hInstance			= GetModuleHandle(NULL);

	//user confirmed "open file dialog"
	if( dlg->DoModal() == IDOK )
	{
		////get file dir
		//str_t file_dir	= dlg->GetPathName().Left( dlg->GetPathName().GetLength() - dlg->GetFileName().GetLength() );
		//m_strLastFileOpenDirectory = file_dir.data();

		////get filename
		//str_t file_name	= dlg->GetFileName();

		//OpenFile( file_dir, file_name, dlg->GetFileExt().GetBuffer() );

		ModelFile		= dlg->GetFileName();
		ModelPath		= dlg->GetPathName().Left( dlg->GetPathName().GetLength() - ModelFile.GetLength() );

		bool del_ok = false;

		if( FileExists( (ModelPath + ModelFile).GetBuffer() ) )
		{
			if( MessageBox( "Replace existing file?", "Warning!", MB_YESNOCANCEL | MB_ICONWARNING ) == IDYES )
			{
				del_ok = true;
			}
			else
			{
				SAFE_DELETE( dlg );
				return;
			}
		}

		m_strLastFileOpenDirectory = ModelPath;

		if( pBaseScene->GetType() == FT_SCN )
		{
			CString meshlist_filename = ModelFile.Left( ModelFile.GetLength() - 4 );
			meshlist_filename += ".geom.efn";

			CBaseScene* pMeshlist = ((CSceneFile*)pBaseScene)->GetMeshlist();
			pMeshlist->SetPaths( ModelPath.GetBuffer(), meshlist_filename.GetBuffer() );
		}

		//ask for efn compression
		if( pBaseScene->GetType() == FT_EFN )
		{
			int res = MessageBox( "Use compression?", "Question", MB_YESNOCANCEL | MB_ICONQUESTION );

			if( res == IDCANCEL )
			{
				SAFE_DELETE( dlg );
				return;
			}
			else if( res == IDYES )
			{
				((CEfnCharacter*)pBaseScene)->SetCompressionType( ECT_SHORT );
			}
			else
			{
				((CEfnCharacter*)pBaseScene)->SetCompressionType( ECT_NONE );
			}
		}

		if( del_ok )
			DeleteFile( ModelPath + ModelFile );

		save_path = ModelPath + ModelFile;

		pBaseScene->SetPaths( ModelPath.GetBuffer(), ModelFile.GetBuffer() );
		pBaseScene->SaveToFile( save_path );

		//set window caption
		this->SetWindowText( "Cookie Machine -- " + ModelPath + ModelFile );

 		menu->ModifyMenu( ID_MENU_SAVE, MF_BYCOMMAND | MF_ENABLED, ID_MENU_SAVE, "Сохранить" );
		EnableToolbarButton( IDT_FILESAVE, true );

		UpdateUI();
	}

	SAFE_DELETE( dlg );

	m_StatusBar->SetPaneText( 5, "File Saved...", true );
	SetTimer( FILE_SAVED_TIMER, 1000, (TIMERPROC) NULL );
}

void CCookieMachineDlg::OnBnClickedBtnAnimPlay()
{
	CButton* pBtnPlay = (CButton*)GetDlgItem( IDC_BTN_ANIM_PLAY );

	if( m_bIsPlaying )
		pBtnPlay->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(), MAKEINTRESOURCE( IDB_ANIM_CTRLS_PLAY ) ) );
	else
	{
		pBtnPlay->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(), MAKEINTRESOURCE( IDB_ANIM_CTRLS_PAUSE ) ) );

		if( m_bIsPlayingFinished )
		{
			m_iCurrentFrame = 0;
			m_bIsPlayingFinished = false;

			CString tmp;
			tmp.Format( "%d", m_iStartingFrame );
			m_sliderAnim.SetPos( m_iStartingFrame );
			m_eFrameCounter.SetWindowText( tmp );
		}
	}


	m_bIsPlaying = !m_bIsPlaying;
}

void CCookieMachineDlg::OnBnClickedBtnAnimStop()
{
	m_bIsPlayingFinished = true;

	CString tmp;

	tmp.Format( "%d", m_iStartingFrame );
	m_sliderAnim.SetPos( m_iStartingFrame );
	m_eFrameCounter.SetWindowText( tmp );

	CString test;
	test.Format( "%0.4f", (float)m_iStartingFrame );
	m_StatusBar->SetPaneText( 3, test, true );

	//m_iCurrentFrame = m_iStartingFrame;

	m_bIsPlaying = false;
	m_iCurrentFrame = 0;

	if( pBaseScene && pBaseScene->GetType() == FT_EFN )
		((CEfnCharacter*)pBaseScene)->SetCurrNextFrame( m_iCurrentFrame, m_iCurrentFrame );

	CButton* pBtnPlay = (CButton*)GetDlgItem( IDC_BTN_ANIM_PLAY );
	pBtnPlay->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(), MAKEINTRESOURCE( IDB_ANIM_CTRLS_PLAY ) ) );
}

void CCookieMachineDlg::OnBnClickedChkLoopAnim()
{
	m_bIsAnimationLooped = !m_bIsAnimationLooped;
}

void CCookieMachineDlg::OnCreateSequence()
{
	// TODO: Add your command handler code here
}

void CCookieMachineDlg::OnMenuIldarWindow()
{
	SetWindowPos(NULL, 0, 0, 1280, 1024 - 29, SWP_SHOWWINDOW); 
	HandlePossibleSizeChange();
}

void CCookieMachineDlg::OnBnClickedButnSetZOff()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN )
		return;

	CString tmp;
	m_eFrameOffset.GetWindowText(tmp);

	int _min, _max;
	bool single_frame = false;

	m_sliderAnim.GetSelection(_min, _max);

  if( (_min == -1 || _min == 0) && _max == -1 ) 
     single_frame = true;

	if( single_frame )
	{
		((CEfnCharacter*)pBaseScene)->SetZOffset(m_sliderAnim.GetPos(), (float)atof(tmp.GetBuffer(0)));
	}
	else
	{
		for(int i = m_iStartingFrame; i <= m_iEndingFrame; i++)
		{
            ((CEfnCharacter*)pBaseScene)->SetZOffset(i, (float)atof(tmp.GetBuffer(0)));
		}
	}
}

void CCookieMachineDlg::ProcessMessages(void)
{
	MSG msg;

	for( int t = 0; t < 20; t++ )
	{
		if(PeekMessage( &msg, this->GetSafeHwnd(), NULL, NULL, PM_REMOVE) )
		{
 			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}		
}

void CCookieMachineDlg::ImportToScn()
{
	str_t		strImportFilePath, file_dir, file_name;
	CBaseScene*	pImportData = NULL;


	/************* get importing file path ***************/
	str_t	szFilter	= "Softimage dotXSI Files (*.xsi)|*.xsi|Scene Files (*.scn)|*.scn|";
	CString	temp		= "*.xsi|*.scn";

	//show open file dialog
	CCustomFileDialog* dlg = new CCustomFileDialog( TRUE, temp.GetBuffer(0), 0, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_ENABLETEMPLATE | OFN_ENABLESIZING, szFilter.data(), NULL );
	dlg->m_pOFN->lpstrInitialDir	= m_appSettings.FilesPath;
	//dlg->m_pOFN->lpfnHook			= OpenDlgExProc;
	dlg->m_pOFN->FlagsEx			= OFN_EX_NOPLACESBAR;
	dlg->m_pOFN->lpTemplateName		= MAKEINTRESOURCE( IDD_OPENFILE_EXT );
	dlg->m_pOFN->hInstance			= GetModuleHandle(NULL);

	m_bIsAppPaused = false;

	//user confirmed "open file dialog"
	if( dlg->DoModal() == IDOK )
	{
		file_dir	= dlg->GetPathName().Left( dlg->GetPathName().GetLength() - dlg->GetFileName().GetLength() );
		file_name	= dlg->GetFileName();

		if( !FileExists( file_dir + file_name ) )
		{
			MessageBox( "Файл не найден!", "Ошибка.", MB_OK | MB_ICONWARNING ); 
			SAFE_DELETE( dlg );
			return;
		}

		strImportFilePath = file_dir + file_name;
	}
	else
	{
		SAFE_DELETE( dlg );
		return;
	}
	
	SAFE_DELETE( dlg );

	/************* load xsi file to import *********************/
	CValidateDlg* pValidateDlg = NULL;

	//show validation window
	if( !pValidateDlg )
	{
		pValidateDlg = new CValidateDlg(this);
		pValidateDlg->SetMessageStr( "Проверка файла..." );
		ProcessMessages();
	}

    str_t strTemp = file_name.substr( file_name.length() - 3, file_name.length() );
	strTemp = _2Upper( (char*)strTemp.data() );

	if( strTemp == "XSI" )
	{
		CXSIFile* pXsiFile = new CXSIFile( this, pValidateDlg );

		pXsiFile->SetPaths( file_dir, file_name );

		if( pXsiFile->LoadFromFile( file_dir, file_name, true ) )
			pImportData = pXsiFile;
		else
		{
			SAFE_DELETE( pXsiFile );
		}
	}
	if( strTemp == "SCN" )
	{
		CSceneFile* pScene = new CSceneFile();

		pScene->SetPaths( file_dir, file_name );

		if( pScene->LoadFromFile( file_dir + file_name ) )
			pImportData = pScene;
		else
		{
			SAFE_DELETE( pScene );	
		}
	}
	
	SAFE_DELETE( pValidateDlg );

	/************* load xsi file import ****END************/

	if( !pImportData )
		return;

	/************* pick data to import *********************/
	CImport2ScnDlg* pImportDlg = new CImport2ScnDlg( this );
	pImportDlg->SetTargetFileType( FT_SCN );
	pImportDlg->SetImportFile( pImportData );
	pImportDlg->SetSceneFile( pBaseScene );
	pImportDlg->DoModal();

	SAFE_DELETE( pImportDlg );
	/************* pick data to import *****END***********/

	pBaseScene->Prepare( g_pd3dDevice );
	UpdateUI();
	
	//CLEANUP
	SAFE_DELETE( pImportData );
}

void CCookieMachineDlg::ImportToMeshlist()
{
	str_t		strImportFilePath, file_dir, file_name;
	CBaseScene*	pImportData;


	/************* get importing file path ***************/
	str_t	szFilter	= "Softimage dotXSI Files (*.xsi)|*.xsi|";
	CString	temp		= "*.xsi";

	//show open file dialog
	CCustomFileDialog* dlg = new CCustomFileDialog( TRUE, temp.GetBuffer(0), 0, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_ENABLETEMPLATE | OFN_ENABLESIZING, szFilter.data(), NULL );
	dlg->m_pOFN->lpstrInitialDir	= m_appSettings.FilesPath;
	//dlg->m_pOFN->lpfnHook			= OpenDlgExProc;
	dlg->m_pOFN->FlagsEx			= OFN_EX_NOPLACESBAR;
	dlg->m_pOFN->lpTemplateName		= MAKEINTRESOURCE( IDD_OPENFILE_EXT );
	dlg->m_pOFN->hInstance			= GetModuleHandle(NULL);

	m_bIsAppPaused = false;

	//user confirmed "open file dialog"
	if( dlg->DoModal() == IDOK )
	{
		file_dir	= dlg->GetPathName().Left( dlg->GetPathName().GetLength() - dlg->GetFileName().GetLength() );
		file_name	= dlg->GetFileName();

		if( !FileExists( file_dir + file_name ) )
		{
			MessageBox( "Файл не найден!", "Ошибка.", MB_OK | MB_ICONWARNING ); 
			return;
		}

		strImportFilePath = file_dir + file_name;
	}

	SAFE_DELETE( dlg );
	/************* get importing file path *****END*******/

	
	/************* load xsi file to import *********************/
	CValidateDlg* pValidateDlg = NULL;

	//show validation window
	if( !pValidateDlg )
	{
		pValidateDlg = new CValidateDlg(this);
		pValidateDlg->SetMessageStr( "Проверка файла..." );
		ProcessMessages();
	}

	CXSIFile* pXsiFile = new CXSIFile( this, pValidateDlg );

	if( pXsiFile->LoadFromFile( file_dir, file_name, true ) )
		pImportData = pXsiFile;
	else
		SAFE_DELETE( pXsiFile );

	SAFE_DELETE( pValidateDlg );
	/************* load xsi file import ****END************/



	/************* pick data to import *********************/
	CImport2ScnDlg* pImportDlg = new CImport2ScnDlg( this );
	pImportDlg->SetTargetFileType( FT_EFN_MESHLIST );
	pImportDlg->SetImportFile( pImportData );
	pImportDlg->SetSceneFile( pBaseScene );
	pImportDlg->DoModal();

	SAFE_DELETE( pImportDlg );
	/************* pick data to import *****END***********/

	pBaseScene->Prepare( g_pd3dDevice );
	UpdateUI();
	
	//CLEANUP
	SAFE_DELETE( pImportData );
}

void CCookieMachineDlg::CloseCurrentFile()
{
	SetCursor( LoadCursor( NULL, IDC_WAIT ) );

	OnBnClickedBtnAnimStop();

	if( m_dlgTextResourcesDlg )
	{
		m_dlgTextResourcesDlg->DestroyWindow();
		SAFE_DELETE( m_dlgTextResourcesDlg );
	}

	m_ObjPropWnd = OWE_UNDEFINED;
	ShowObjectPropertiesWnd( false );

	pSelectedEfnAnimNull = NULL;

	OnMenuMacroRecEnd();
	FinishEditingMeshVertices();

	//m_dlgScriptingDlg->DestroyWindow();
	//m_dlgBindingDlg->DestroyWindow();
	SAFE_DELETE( m_dlgScriptingDlg );
	SAFE_DELETE( m_dlgBindingDlg );

	m_cmbCameras.ResetContent();
	m_cmbCameras.AddString( DEFAULT_CAMERA_ID );
	m_cmbCameras.SetCurSel( 0 );

	m_bSCNLoaded = false;

	if( pBaseScene )
	{
		m_bIsPerspectiveCameraOn = true;

		if( !m_bResetCameraOnFileOpen )
			OnMenuReturnToPerspectiveview();

		if( m_appSettings.AskForSavingOnAppQuit )
		{
			if( pBaseScene->GetType() == FT_EFN || pBaseScene->GetType() == FT_SCN || pBaseScene->GetType() == FT_EFN_MESHLIST )
			{
				if( MessageBox( "Сохранить изменения?", "Вопрос", MB_YESNO | MB_ICONQUESTION ) == IDYES )
				{
					if( pBaseScene->GetFolder() == "" || pBaseScene->GetFilename() == "" )
						OnMenuFileSaveAs();
					else
						OnMenuFileSave();
				}
			}
		}

	}

	m_bIsGRadEditorModeOn = false;

	//SAFE_RELEASE( ppCylinder );
	SAFE_DELETE( pBaseScene );

	m_sliderAnim.SetRange( 0, 0, true );
	m_rtSceneContents.ReleaseTreeData();
	m_rtSceneContents.DeleteAllItems();
	EnableControls( false );

	EnableWalkMode( false );

	this->SetWindowText( "Cookie machine" );

	SetCursor( LoadCursor( NULL, IDC_ARROW ) );

	OnMenuScnRestoreEngineCams();
	ResetStatusBarInfo();

	m_PrevQuickBind.m_strChildObjType	= "";
	m_PrevQuickBind.m_strChildType		= "";
	m_PrevQuickBind.m_strParentID		= "";
}

void CCookieMachineDlg::OnMenuEfnEditModelName( CString	strNewID )
{
	((CEfnCharacter*)pBaseScene)->SetModelName( strNewID.GetBuffer() );

	m_rtSceneContents.SetItemText( m_rtSceneContents.GetSelectedItem(), "Model name: " + strNewID );
}

void CCookieMachineDlg::OnMenuFileSave()
{
	if( !pBaseScene || pBaseScene->GetType() == FT_XSI )
		return;

	if( pBaseScene->GetFolder() == "" || pBaseScene->GetFilename() == "" )
	{
		OnMenuFileSaveAs();
		return;
	}

	str_t path = pBaseScene->GetFolder();
	path.append( pBaseScene->GetFilename() );

	//if( FileExists( path ) )
	//{
	//	if( MessageBox( "Replace existing file?", "Alarm!", MB_YESNO ) == IDYES );
	//		DeleteFile( path.c_str() );
	//}
		
	if( pBaseScene->GetType() == FT_EFN )
		((CEfnCharacter*)pBaseScene)->SaveToFile( path );

	if( pBaseScene->GetType() == FT_EFN_MESHLIST )
		((CEfnMeshlist*)pBaseScene)->SaveToFile( path );

	if( pBaseScene->GetType() == FT_SCN )
	{
		((CSceneFile*)pBaseScene)->SaveToFile( path );
	}

	m_StatusBar->SetPaneText( 5, "File Saved...", true );

	SetTimer( FILE_SAVED_TIMER, 1000, (TIMERPROC) NULL );
}

void CCookieMachineDlg::OnMenuDiscardVerteciesColor()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() == FT_EFN ) 
	{
		((CEfnCharacter*)pBaseScene)->DicardVerteciesColor();
		pBaseScene->Prepare( g_pd3dDevice );  
	}
}

void CCookieMachineDlg::OnMenuEfnAnimationAdd()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN )
		return;

	EFN_ANIMATION	animation;
	CAnimationDlg*	dlgAnimation	= new CAnimationDlg(this);
	bool			completed		= false;
	CString			strNodeText;
	CEfnCharacter*	pEfnAnim		= (CEfnCharacter*)pBaseScene;

	if( !dlgAnimation )
		return;

	dlgAnimation->m_iStart = m_iStartingFrame;
	dlgAnimation->m_iEnd   = m_iEndingFrame;
	dlgAnimation->m_iMax   = ((CEfnCharacter*)pBaseScene)->GetFramesCount();
	animation.frames_list  = NULL;


	dlgAnimation->DoModal();

	HTREEITEM hAnims = m_rtSceneContents.GetSelectedItem();
	
	if( dlgAnimation->m_bOkPressed )
	{
		//get new animation data
		if( pEfnAnim->GetAnimationByName( dlgAnimation->m_strName.GetBuffer() ) )
		{
			MessageBox( "Анимация с таким именем уже существует!", "Внимание!", MB_OK | MB_ICONHAND );
			SAFE_DELETE( dlgAnimation );

			return;
		}
		else
		{
			animation.name			= dlgAnimation->m_strName.GetBuffer();
			animation.frames_count	= dlgAnimation->m_iEnd - dlgAnimation->m_iStart + 1;
			animation.frames_list	= new int[animation.frames_count];
			animation.IsTransition	= dlgAnimation->m_bIsTransition;

			for( int i = dlgAnimation->m_iStart; i <= dlgAnimation->m_iEnd; i++ )
				animation.frames_list[i - dlgAnimation->m_iStart] = i;
			
			pEfnAnim->AddAnimation( &animation );
			
			m_rtSceneContents.InsertItem2( animation.name.data(), 8, new TNODE( TREENODE::N_ANIMATION, pBaseScene ), hAnims, TVI_SORT );
			m_rtSceneContents.UpdateWindow();
		}
	}

	SAFE_DELETE( dlgAnimation );
}

void CCookieMachineDlg::OnMenuEfnAnimationsDeleteAll()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN )
		return;

	CString tmp2 = "Вы действительно хотите удалить все анимации?";

	if( MessageBox( tmp2, "Внимание!", MB_YESNO | MB_ICONWARNING  ) != IDYES )
		return;

	CEfnCharacter* pEfnAnim = (CEfnCharacter*)pBaseScene;

	pEfnAnim->DeleteAllAnimations();

	HTREEITEM hAnims	= m_rtSceneContents.FindTreeItemByName( "Animations" );
	HTREEITEM hTemp		= m_rtSceneContents.GetChildItem( hAnims );

	while( hTemp )
	{
		m_rtSceneContents.DeleteItem( hTemp );
		hTemp = m_rtSceneContents.GetChildItem( hAnims );
	}

	UpdateStatusBarInfoEFN();
}

void CCookieMachineDlg::OnMenuEfnAnimationDelete()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN )
		return;

	CEfnCharacter* pEfnAnim = (CEfnCharacter*)pBaseScene;

	pEfnAnim->DeleteAnimationByName( m_rtSceneContents.GetCurrNodeText().GetBuffer() );
	m_rtSceneContents.DeleteItem( m_rtSceneContents.GetSelectedItem() );

	UpdateStatusBarInfoEFN();
}

void CCookieMachineDlg::OnMenuEfnAnimationEdit()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN )
		return;

	HTREEITEM		hSelected		= m_rtSceneContents.GetSelectedItem();
	CAnimationDlg*	dlgAnimation	= new CAnimationDlg(this);
	CEfnCharacter*	pEfnChar		= (CEfnCharacter*)pBaseScene;
	EFN_ANIMATION*	pAnim			= pEfnChar->GetAnimationByName( m_rtSceneContents.GetCurrNodeText().GetBuffer() );

	dlgAnimation->m_iStart  = pAnim->frames_list[0];
	dlgAnimation->m_iEnd    = pAnim->frames_list[pAnim->frames_count - 1];
	dlgAnimation->m_iMax    = pEfnChar->GetFramesCount() - 1;
	dlgAnimation->m_strName = pAnim->name.data(); 
	dlgAnimation->m_bIsTransition = pAnim->IsTransition;

	dlgAnimation->DoModal();
	
	str_t strNewID = dlgAnimation->m_strName.GetBuffer();

	if( dlgAnimation->m_bOkPressed )
	{
		str_t _strCurrID2	= pAnim->name;
		str_t _strNewID2	= strNewID;

		_2Upper( _strCurrID2 );
		_2Upper( _strNewID2 );

		if( _strCurrID2 != _strNewID2 && pEfnChar->GetAnimationByName( strNewID ) )
		{
			MessageBox( "Анимация с таким именем уже существует!", "Внимание!", MB_OK | MB_ICONHAND );
			SAFE_DELETE( dlgAnimation );
		}
		else
		{
			pAnim->name = strNewID;
			pAnim->frames_count = dlgAnimation->m_iEnd - dlgAnimation->m_iStart + 1;
			pAnim->IsTransition = dlgAnimation->m_bIsTransition;
			
			SAFE_DELETE_ARRAY( pAnim->frames_list );
			pAnim->frames_list = new int[pAnim->frames_count];

			for( int i = dlgAnimation->m_iStart; i <= dlgAnimation->m_iEnd; i++ )
				pAnim->frames_list[i - dlgAnimation->m_iStart] = i;

			m_rtSceneContents.SetItemText( hSelected, strNewID.data() );
		}
	}

	UpdateStatusBarInfoEFN();

	//clean up
	SAFE_DELETE( dlgAnimation );
}

void CCookieMachineDlg::OnBnClickedChkMoveChar()
{
	m_bIsAnimationMoving = !m_bIsAnimationMoving;
}

void CCookieMachineDlg::OnMenuEfnAnimationRun()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN )
		return;

	CEfnCharacter*	pEfnChar		= (CEfnCharacter*)pBaseScene;
	EFN_ANIMATION*	pAnim			= pEfnChar->GetAnimationByName( m_rtSceneContents.GetCurrNodeText().GetBuffer() );

	if( !pAnim )
		return;

	int iMin	= pAnim->frames_list[0]; 
	int	iMax	= pAnim->frames_list[pAnim->frames_count - 1];

	//set slider data
	m_sliderAnim.ClearSel(true);
	m_sliderAnim.SetSelection(iMin, iMax);
	m_sliderAnim.SetPos(iMin);
	m_iStartingFrame	= iMin;
	m_iEndingFrame		= iMax;
	m_sliderAnim.Invalidate(true);

	m_btnTransitionAnim.SetCheck( pAnim->IsTransition );

	//play animation
	m_bIsPlaying = false;
	m_bIsPlayingFinished = true;
	OnBnClickedBtnAnimPlay();				
	firstSelFrame = iMin;

	UpdateStatusBarInfoEFN();
}

void CCookieMachineDlg::OnMenuAANone()
{
	g_d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;

	g_pd3dDevice->Reset(&g_d3dpp);
	//OneTimeSceneInit();
}

void CCookieMachineDlg::OnMenuAaX2()
{
  g_d3dpp.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES;

	g_pd3dDevice->Reset(&g_d3dpp);
	//OneTimeSceneInit();
}

void CCookieMachineDlg::OnMenuAaX4()
{
	g_d3dpp.MultiSampleType = D3DMULTISAMPLE_4_SAMPLES;

	g_pd3dDevice->Reset(&g_d3dpp);
	//OneTimeSceneInit();
}

void CCookieMachineDlg::OnMenuAaX6()
{
	g_d3dpp.MultiSampleType = D3DMULTISAMPLE_6_SAMPLES;

	g_pd3dDevice->Reset(&g_d3dpp);
	//OneTimeSceneInit();
}

void CCookieMachineDlg::HandleMeshSelection( str_t strObjID )
{
	HTREEITEM		hMeshes		= m_rtSceneContents.FindTreeItemByName( "Meshes" );
	HTREEITEM		hMesh		= m_rtSceneContents.FindTreeItemByName( strObjID, hMeshes );

	CEfnMeshlist*	pMeshlist	= NULL;
	CMesh*			pMesh		= NULL;

	if( pBaseScene->GetType() == FT_EFN_MESHLIST )
	{
		pMeshlist	= ((CEfnMeshlist*)pBaseScene);
	}
	else if( pBaseScene->GetType() == FT_SCN )
	{
		pMeshlist	= (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();
	}
	else
		return;

	if( !pMeshlist )
		return;

	pMesh = pMeshlist->GetMeshByName( strObjID );

	if( !pMesh )
		return;


	if( KEYDOWN(VK_SHIFT) )
	{
		if( pMeshlist->GetSelectedMeshesCount() == 0 )
		{
			m_rtSceneContents.SetItemState( hMesh, TVIS_SELECTED, TVIS_SELECTED );
			m_rtSceneContents.SelectItem( hMesh );
			pMeshlist->DeselectAllMeshes();
			pMesh->SetSelected( true );

			if( pMeshlist->IsParentMeshSet() == false )
				pMeshlist->SetParentMesh( strObjID );
		}
		else
		{
			if( pMesh->IsSelected() )
			{
				if( pMesh->IsParentMesh() && pMeshlist->GetSelectedMeshesCount() > 0 )
				{
					vector<str_t> sel;

					pMesh->SetSelected( false );
					m_rtSceneContents.SetItemState( hMesh, 0, TVIS_SELECTED );

					
					for( int i = 0; i < pMeshlist->GetMeshesCount(); i++ )
					{
						if( pMeshlist->GetMeshByIndex( i )->IsSelected() )
							sel.push_back( pMeshlist->GetMeshByIndex( i )->GetName() );
					}

					/*for( UINT i = 0; i < sel.size(); i++ )
					{
						OutputDebugString( "\n" );
						OutputDebugString( sel[i].data() );
					}

					OutputDebugString( "\n---------------------" );*/

					if( sel.size() >= 1 )
					{
						HTREEITEM	hPar	= m_rtSceneContents.FindTreeItemByName( sel[0], hMeshes );
						m_rtSceneContents.SetItemState( hPar, TVIS_SELECTED, TVIS_SELECTED );
						m_rtSceneContents.SelectItem( hPar );

						pMeshlist->GetMeshByName( sel[0] )->SetSelected( true );
						pMeshlist->GetMeshByName( sel[0] )->SetParentMesh( true );

						for( UINT i = 1; i < sel.size(); i++ )
						{
							hPar = m_rtSceneContents.FindTreeItemByName( sel[i], hMeshes );
							m_rtSceneContents.SetItemState( hPar, TVIS_SELECTED, TVIS_SELECTED );
							pMeshlist->GetMeshByName( sel[i] )->SetSelected( true );
						}
					}
				
				}
				else
				{
					pMesh->SetSelected( false );
					m_rtSceneContents.SetItemState( hMesh, 0, TVIS_SELECTED );
				}
			}
			else
			{
				pMesh->SetSelected( true );
				m_rtSceneContents.SetItemState( hMesh, TVIS_SELECTED, TVIS_SELECTED );
				m_rtSceneContents.EnsureVisible( hMesh );
			}
		}
	}
	else
	{
		m_rtSceneContents.UpdateWindow();
		m_rtSceneContents.SetFocus();

		m_rtSceneContents.DeselectAllNodes();
		m_rtSceneContents.SetItemState( hMesh, TVIS_SELECTED, TVIS_SELECTED );
		m_rtSceneContents.SelectItem( hMesh );

		pMeshlist->DeselectAllMeshes();
		pMesh->SetSelected( true );
		pMeshlist->SetParentMesh( strObjID );
	}
}

CVector CCookieMachineDlg::GetGridIntersection(int x, int y)
{
	//take basic XZ aligned plane and apply complete transformation
	CVector a, b, c, res;
  

	/////////////////
	// PLANE //
	/////////////////

	//transform plane
	D3DXMATRIX plane_transform = m_matGrid * m_matViewport;
	
	D3DXVECTOR3 _a( 0.0f, 0.0f, 1.0f );
	D3DXVECTOR3 _b( 1.0f, 0.0f, 0.0f );
	D3DXVECTOR3 _c( 0.0f, 0.0f, 0.0f );

	D3DXVec3TransformCoord(&_a, &_a, &plane_transform);
	D3DXVec3TransformCoord(&_b, &_b, &plane_transform);
	D3DXVec3TransformCoord(&_c, &_c, &plane_transform);

	a.x = _a.x;
	a.y = _a.y;
	a.z = _a.z;
	b.x = _b.x;
	b.y = _b.y;
	b.z = _b.z;
	c.x = _c.x;
	c.y = _c.y;
	c.z = _c.z;

	CPlane plane(a, b, c);
	//plane complete
 

	/////////////
	// RAY //
	/////////////
	CVector from_vec;
	CVector to_vec;
	//GetProjectedRay( x, y, from_vec, to_vec );
	

	/////////////////////////////
	// INTERSECTION //
	/////////////////////////////

	res = plane.RayIntersection(from_vec, to_vec);

	D3DXVECTOR3 res_complete(res.x, res.y, res.z);
 
	D3DXMATRIX view_inverse;
	D3DXMatrixInverse(&view_inverse, NULL, &m_matViewport);
	D3DXVec3TransformCoord(&res_complete, &res_complete, &view_inverse);

	res.x = res_complete.x;
	res.y = res_complete.y;
	res.z = res_complete.z;

	return res;
}

void CCookieMachineDlg::OnMenuTestWalk()
{
	//if( !pBaseScene )
	//	return;

	//if(pBaseScene->GetType() != FT_EFN)
	//	return;

	//if(m_bIsWalkModeOn) 
	//	EnableWalkMode(false);
	//else
	//	EnableWalkMode(true);
}

void CCookieMachineDlg::EnableWalkMode(bool bEnabled)
{
	//CMenu *menu = this->GetMenu();

	//if( !pBaseScene )
	//{
	//	menu->CheckMenuItem(ID_MENU_TEST_WALK, MF_UNCHECKED);
	//	return;
	//}

	//m_bIsWalkModeOn = bEnabled;

	//if( bEnabled )
	//{
	//	menu->CheckMenuItem(ID_MENU_TEST_WALK, MF_CHECKED);
	//	//EnableControls(false);
	//}
	//else
	//{
	//	menu->CheckMenuItem(ID_MENU_TEST_WALK, MF_UNCHECKED);
	//	//EnableControls(true);
	//}
}

void CCookieMachineDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	str_t func_decl_str = "void CCookieMachineDlg::OnLButtonDblClk(UINT nFlags, CPoint point)";

	try
	{
		m_bDoubleClicked = true;
		CDialog::OnLButtonDblClk(nFlags, point);
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}
}

void CCookieMachineDlg::RenderEFNCharacterEd( bool bIsWireframe )
{
	static bool  anim_reset			= false;
	static float timeelapsed		= 0.0f;
	static float timeelapsed_old	= 0.0f;
	static float z_off				= 0.0f;
	static float fTween				= 0.0f;
	CString      tmp;
	static bool  restarted			= false;
	
	//reset z offset var
	z_off = 0.0f;

	if( !m_bIsPlaying ) 
		fTween = 0.0f;

	//increment time counter from last procedure call
	timeelapsed += DXUtil_Timer( TIMER_GETELAPSEDTIME );

	//calculate offset
	if( m_bIsAnimationMoving )
	{
		for( int i = 0; i < m_sliderAnim.GetPos(); i++ )
			z_off += ((CEfnCharacter*)pBaseScene)->GetZOffset( i );

		z_off += ((CEfnCharacter*)pBaseScene)->GetZOffset( m_sliderAnim.GetPos() ) * fTween;
	}

	//find actual world position vec of character
	D3DXMATRIX charPathOff;
	D3DXMatrixTranslation( &charPathOff, 0.0f, 0.0f, z_off );

	int iCurrFrmOff = m_iStartingFrame + m_iCurrentFrame;
	
	//draw character
	if( m_bIsPlaying )
	{
		int iCurr = 0;
		int iNext = 0;

		if( iCurrFrmOff >= m_iEndingFrame )
		{
			if( m_btnTransitionAnim.GetCheck() == 1 )
			{
				iCurr = m_iEndingFrame;
				iNext = m_iStartingFrame;
			}
			else
			{
				if( m_iEndingFrame - m_iStartingFrame == 0 )
				{
					iCurr = iCurrFrmOff;
					iNext = iCurrFrmOff;
				}
				else
				{
					iCurr = iCurrFrmOff;
					iNext = iCurrFrmOff + 1;
				}
			}
		}
		else
		{
			iCurr = iCurrFrmOff;
			iNext = iCurrFrmOff + 1;
		}

		((CEfnCharacter*)pBaseScene)->SetCurrNextFrame( iCurr, iNext );

		if( m_bIsAnimationLooped )
		{
			CString test;
			test.Format( "%0.4f", iCurr + fTween );
			m_StatusBar->SetPaneText( 3, test, true );
		}
		else
		{
			CString test;
			test.Format( "%0.4f", iCurr + fTween );
			m_StatusBar->SetPaneText( 3, test, true );
		}

		g_pd3dDevice->SetTransform( D3DTS_WORLD, &(D3DXMATRIX(charPathOff)) );
		((CEfnCharacter*)pBaseScene)->Render( g_pd3dDevice, bIsWireframe, fTween );

		//==
		anim_reset = false;

		fTween += ((CEfnCharacter*)pBaseScene)->GetFPS() * (timeelapsed - timeelapsed_old);

		if( fTween >= 1.0f )
		{
			int iNewSldrPos;

			if( !restarted )
				iNewSldrPos = iCurrFrmOff + 1;
			else
			{
				iNewSldrPos = m_iStartingFrame;
				restarted = false;
				
				m_iCurrentFrame = -1;

				if( m_btnTransitionAnim.GetCheck() == 0 )
					m_iCurrentFrame = 0;
			}

			//update frame counter
			tmp.Format( "%d", iNewSldrPos );
			m_eFrameCounter.SetWindowText(tmp);

			//update frame z-offset
			tmp.Format( "%f", ((CEfnCharacter*)pBaseScene)->GetZOffset( iNewSldrPos ) );
			m_eFrameOffset.SetWindowText( tmp );

			if( m_iStartingFrame != m_iEndingFrame )
				m_sliderAnim.SetPos( iNewSldrPos );
			
			switch( m_ObjPropWnd )
			{
				case OWE_CUSTPARAM:
					m_pPropCustomParamDlg->Populate( m_sliderAnim.GetPos() );
					break;

				case OWE_NULL:
					m_pPropNullDlg->Populate( m_sliderAnim.GetPos(), m_iStartingFrame, m_iEndingFrame );
					break;
			}


			int skip = fTween / 1.0f;
			m_iCurrentFrame += skip;
			
			fTween = fTween - 1.0f * skip;
			
			if( m_iCurrentFrame >= m_iEndingFrame - m_iStartingFrame )
			{		
				if( m_bIsAnimationLooped && !restarted )
				{
					restarted		= true;

					if( m_btnTransitionAnim.GetCheck() == 0 )
					{
						m_iCurrentFrame = 0;
						//fTween = 0.0f;
					}
				}
				else
				{
					m_bIsPlaying	= false;
					m_iCurrentFrame = m_iEndingFrame - m_iStartingFrame;

					CButton* pBtnPlay = (CButton*)GetDlgItem( IDC_BTN_ANIM_PLAY );
					pBtnPlay->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(), MAKEINTRESOURCE( IDB_ANIM_CTRLS_PLAY ) ) );

					m_bIsPlayingFinished = true;

					CString test;
					test.Format( "%0.4f", (float)m_iEndingFrame );
					m_StatusBar->SetPaneText( 3, test, true );
				}
			}	
		}
	}
	else
	{
		((CEfnCharacter*)pBaseScene)->SetCurrNextFrame( iCurrFrmOff, iCurrFrmOff );
		g_pd3dDevice->SetTransform( D3DTS_WORLD, &(D3DXMATRIX(charPathOff)) );
		((CEfnCharacter*)pBaseScene)->Render( g_pd3dDevice, bIsWireframe, 0 );

		/*if( !anim_reset )
		{
			if( m_bIsAnimationLooped )
			{
				m_iCurrentFrame = 0;
				((CEfnCharacter*)pBaseScene)->SetCurrNextFrame( m_iStartingFrame + m_iCurrentFrame, m_iStartingFrame + m_iCurrentFrame );
			}
			anim_reset = true;
		}*/
	}
	//else
	//	((CEfnCharacter*)pBaseScene)->SetCurrNextFrame( m_iCurrentFrame, m_iCurrentFrame );

	((CEfnCharacter*)pBaseScene)->RenderNormals( g_pd3dDevice );

	timeelapsed_old = timeelapsed;
}

void CCookieMachineDlg::OnMenuConvertCoordsSystem()
{
	if( !((CEfnCharacter*)pBaseScene) )
		return;

	((CEfnCharacter*)pBaseScene)->ConvertToLHCoords();
	((CEfnCharacter*)pBaseScene)->Prepare(g_pd3dDevice);
}

void CCookieMachineDlg::OnMenuScale()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN )
		return;

	SetCursor( LoadCursor( NULL, IDC_WAIT ) );

	float height, new_height, scale_koef;

	CScalingDlg *scaling_dlg = NULL;
	scaling_dlg = new CScalingDlg(this);

	height = ((CEfnCharacter*)pBaseScene)->GetModelHeight();

	if( !scaling_dlg )
	{
		SetCursor( LoadCursor( NULL, IDC_ARROW ) );
		return;
	}

	scaling_dlg->SetHeight( height );
	scaling_dlg->DoModal();

	if( !scaling_dlg->m_bOK )
	{
		SAFE_DELETE( scaling_dlg );
	}
	else
	{
		new_height = scaling_dlg->GetNewHeight();
		scale_koef = new_height / height;
		
		D3DXMATRIX scale;
		D3DXMatrixScaling( &scale, scale_koef, scale_koef, scale_koef );

		((CEfnCharacter*)pBaseScene)->Scale(scale);

		pBaseScene->Prepare( g_pd3dDevice );
	}

	SAFE_DELETE( scaling_dlg );
	SetCursor( LoadCursor( NULL, IDC_ARROW ) );
}

void CCookieMachineDlg::OnMenuSwapyz()
{
	switch( pBaseScene->GetType() )
	{
		case FT_XSI:
			((CXSIFile*)pBaseScene)->SwapYZCoords();
			((CXSIFile*)pBaseScene)->Prepare(g_pd3dDevice);
			UpdateUI();
		break;
	}
}

void CCookieMachineDlg::OnMenuOptions()
{
	if( !m_dlgOptions )
	  m_dlgOptions = new COptionsDlg(this);

	if( !m_dlgOptions )
		return;

	AppSettingsData temp = m_appSettings;
	
	KillTimer( RENDERING_TIMER );

	m_dlgOptions->SetValues( m_appSettings );
	m_dlgOptions->DoModal();

	SetTimer(RENDERING_TIMER, 0, (TIMERPROC) NULL);
	
	m_appSettings = m_dlgOptions->GetValues();

	if( m_dlgOptions->m_bOK && m_dlgOptions->m_bSettingsChanged )
	{
		//apply new settings
		SettingsApplyToApp();

		//write new ini file
		SettingsWriteToFile();

		GridCreate();
	}
	else
	{
		//m_appSettings = temp;

		//apply new settings
		//SettingsApplyToApp();	
	}
	
	SAFE_DELETE(m_dlgOptions);
}

void CCookieMachineDlg::SettingsWriteToFile()
{
	FILE*		stream = NULL;
	//char			str[256];
	CString		tmp_str;

	stream = fopen( (m_strAppPath + "\\" + m_strIniFilename).GetBuffer(), "w");

	fprintf( stream, "%s\n", "[General]" );
	fprintf( stream, "DefaultDirectory  = \"%s\"\n", m_appSettings.FilesPath );

	if( m_appSettings.DefaultFileType == FT_EFN )
		fprintf( stream, "DefaultFiletype   = %s\n", "efn" );

	if( m_appSettings.DefaultFileType == FT_XSI )
		fprintf( stream, "DefaultFiletype   = %s\n", "xsi" );

	if( m_appSettings.DefaultFileType == FT_SCN )
		fprintf( stream, "DefaultFiletype   = %s\n", "scn" );

	if( m_appSettings.DefaultFileType == FT_ALLSUPPORTED )
		fprintf( stream, "DefaultFiletype   = %s\n", "all_supported" );

	if( m_appSettings.RepairXSIFiles )
		fprintf( stream, "%s", "RepairXSIFiles    = true\n" );
	else
		fprintf( stream, "%s", "RepairXSIFiles    = false\n" );

	if( m_appSettings.FilterObjsByPrefix )
		fprintf( stream, "%s", "FilterObjByPrefx  = true\n" );
	else
		fprintf( stream, "%s", "FilterObjByPrefx  = false\n" );

	if( m_appSettings.ScnAlternateNullIco )
		fprintf( stream, "%s", "SCNAltNullIcon    = true\n" );
	else
		fprintf( stream, "%s", "SCNAltNullIcon    = false\n" );

	fprintf( stream, "EngineDirectory   = \"%s\"\n", m_appSettings.EngineFolder );
	fprintf( stream, "StartParams       = \"%s\"\n", m_appSettings.StartParams );

	if( m_appSettings.ExpertMode )
		fprintf( stream, "%s", "ExpertMode        = true\n" );
	else
		fprintf( stream, "%s", "ExpertMode        = false\n" );

	if( m_appSettings.EngineDebugModeOn )
		fprintf( stream, "%s", "EngineDebugModeOn = true\n" );
	else
		fprintf( stream, "%s", "EngineDebugModeOn = false\n" );

	if( m_appSettings.AskForSavingOnAppQuit )
		fprintf( stream, "%s", "AskForSaving      = true\n" );
	else
		fprintf( stream, "%s", "AskForSaving      = false\n" );

	if( m_appSettings.FirstMonitorOnly )
		fprintf( stream, "%s", "1stMonitorOnly    = true\n" );
	else
		fprintf( stream, "%s", "1stMonitorOnly    = false\n" );

	fprintf( stream, "CustButtnonACapt  = \"%s\"\n", m_appSettings.Button_A_Capt );
	fprintf( stream, "CustButtnonAPath  = \"%s\"\n", m_appSettings.Button_A_Path );
	fprintf( stream, "CustButtnonBCapt  = \"%s\"\n", m_appSettings.Button_B_Capt );
	fprintf( stream, "CustButtnonBPath  = \"%s\"\n", m_appSettings.Button_B_Path );

	if( m_appSettings.SwitchBackOnDebug )
		fprintf( stream, "%s", "SwitchBackOnDebug = true\n" );
	else
		fprintf( stream, "%s", "SwitchBackOnDebug = false\n" );


	fprintf( stream, "\n\n%s\n", "[Lighting]" );

	if( m_appSettings.LightingEnabled )
		fprintf( stream, "Enabled           = %s\n", "true" );
	else
		fprintf( stream, "Enabled           = %s\n", "false" );

	fprintf( stream, "LightAmbientRGB   = %i, %i, %i\n", GetRValue( m_appSettings.LightAmbientColor ), GetGValue( m_appSettings.LightAmbientColor ), GetBValue( m_appSettings.LightAmbientColor ) );
	fprintf( stream, "LightDiffuseRGB   = %i, %i, %i\n", GetRValue( m_appSettings.LightDiffuseColor ), GetGValue( m_appSettings.LightDiffuseColor ), GetBValue( m_appSettings.LightDiffuseColor ) );
	fprintf( stream, "AmbientRGB        = %i, %i, %i\n", GetRValue( m_appSettings.AmbientColor ), GetGValue( m_appSettings.AmbientColor ), GetBValue( m_appSettings.AmbientColor) );

	if( m_appSettings.AttachToCamera )
		fprintf( stream, "AttachToCamera    = %s\n", "true" );
	else
		fprintf( stream, "AttachToCamera    = %s\n", "false" );

	fprintf( stream, "\n\n%s\n", "[Camera]" );
	fprintf( stream, "Position          = %f, %f, %f\n", m_appSettings.DefaultCamera.pos.x, m_appSettings.DefaultCamera.pos.y, m_appSettings.DefaultCamera.pos.z );
	fprintf( stream, "Direction         = %f, %f, %f\n", m_appSettings.DefaultCamera.dir.x, m_appSettings.DefaultCamera.dir.y, m_appSettings.DefaultCamera.dir.z );
	fprintf( stream, "Upvector          = %f, %f, %f\n", m_appSettings.DefaultCamera.upvector.x, m_appSettings.DefaultCamera.upvector.y, m_appSettings.DefaultCamera.upvector.z );


	D3DXVECTOR3 wRot = m_appSettings.DefaultWorldStates.rot;
	D3DXVECTOR3 wPos = m_appSettings.DefaultWorldStates.pos;

	wRot.x = D3DXToDegree( wRot.x );
	wRot.y = D3DXToDegree( wRot.y );
	wRot.z = D3DXToDegree( wRot.z );

	ClampAngle( wRot.x );
	ClampAngle( wRot.y );
	ClampAngle( wRot.z );

	fprintf( stream, "WorldRotate       = %f, %f, %f\n", wRot.x, wRot.y, wRot.z );
	fprintf( stream, "ViewportTranslate = %f, %f, %f\n", wPos.x, wPos.y, wPos.z );
	fprintf( stream, "ViewportBackcolor = %i, %i, %i\n", GetRValue( m_appSettings.ViewportBackColor ), GetGValue( m_appSettings.ViewportBackColor ), GetBValue( m_appSettings.ViewportBackColor ) );


	if( m_appSettings.ViewportBackfaceCull )
		fprintf( stream, "ViewportCulling   = %s\n", "true" );
	else
		fprintf( stream, "ViewportCulling   = %s\n", "false" );


	if( m_appSettings.ViewportPreserve43 )
		fprintf( stream, "Preserve4_3       = %s\n", "true" );
	else
		fprintf( stream, "Preserve4_3       = %s\n", "false" );


	fprintf( stream, "FieldOfView       = %f\n", m_appSettings.DefaultCamera.hfov );


	if( m_appSettings.ResetCameraOnFileOpen )
		fprintf( stream, "ResetOnFileOpen   = true" );
	else
		fprintf( stream, "ResetOnFileOpen   = false" );


	fprintf( stream, "\n\n\n%s\n", "[Grid]" );

	if( m_appSettings.GridVisible )
		fprintf( stream, "Visible           = true\n" );
	else
		fprintf( stream, "Visible           = false\n" );

	fprintf( stream, "Size              = %i\n", m_appSettings.GridSize );
	fprintf( stream, "Cellsize          = %f\n", m_appSettings.GridCellSize );

	fprintf( stream, "WiresColor        = %i, %i, %i\n", GetRValue( m_appSettings.GridWiresColor ), GetGValue( m_appSettings.GridWiresColor ), GetBValue( m_appSettings.GridWiresColor ) );
	fprintf( stream, "CrossColor        = %i, %i, %i\n", GetRValue( m_appSettings.GridCrossColor ), GetGValue( m_appSettings.GridCrossColor ), GetBValue( m_appSettings.GridCrossColor ) );


	fprintf( stream, "\n\n%s\n", "[CamLightLicons]" );
	fprintf( stream, "Scale             = %f\n", m_appSettings.IconsScaling );



	fprintf( stream, "\n\n%s\n", "[RecentFiles]" );

	if( !m_appSettings.RecentFiles.empty() )
	{
		for( int i = 1; i < 10; i++ )
		{
			if( i - 1 < (int)m_appSettings.RecentFiles.size() )
				fprintf( stream, "File%d             = \"%s\"\n", i,  m_appSettings.RecentFiles[i-1].GetBuffer() );
			else
				fprintf( stream, "File%d             = \"\"\n", i );
		}
	}



	fprintf( stream, "\n\n%s\n", "[RecentDirectories]" );

	if( !m_appSettings.RecentDirs.empty() )
	{
		for( int i = 1; i < 10; i++ )
		{
			if( i - 1 < (int)m_appSettings.RecentDirs.size() )
				fprintf( stream, "Dir%d              = \"%s\"\n", i,  m_appSettings.RecentDirs[i-1].GetBuffer() );
			else
				fprintf( stream, "Dir%d              = \"\"\n", i );
		}
	}

	fclose( stream );
	stream = NULL;
}

void CCookieMachineDlg::AddPathToRecentFilesList( str_t strPath )
{
	SettingsReadFromFile();

	CMenu* menu = this->GetMenu();
	CMenu* file_menu = menu->GetSubMenu( 0 );
	CMenu* recent_files_menu = file_menu->GetSubMenu( 4 );

	strPath = _2Lower( (char*)strPath.data() );

	bool	bAlreadyAdded = false;
	int		iExistingItemIdx;
	CString	strTemp, strTemp2;


	strTemp = strPath.c_str();
	strTemp.MakeUpper();
	strTemp.Trim();


	//check whether path is already in the list
	if( !m_appSettings.RecentFiles.empty() )
	{
		for( UINT i = 0; i < m_appSettings.RecentFiles.size(); i++ )
		{
			strTemp2 = m_appSettings.RecentFiles[i];
			strTemp2.MakeUpper();
			strTemp2.Trim();

			if( strTemp == strTemp2 )
			{
				iExistingItemIdx = i;
				bAlreadyAdded = true;
				break;
			}
		}
	}

	//if not -- add path to the recent list, otherwise -- move to the top of the recents
	if( bAlreadyAdded )
	{
		m_appSettings.RecentFiles.erase( m_appSettings.RecentFiles.begin() + iExistingItemIdx );
		m_appSettings.RecentFiles.insert( m_appSettings.RecentFiles.begin(), strPath.data() ); 
	}
	else
	{
		//move existing entries 1 down
		m_appSettings.RecentFiles.insert( m_appSettings.RecentFiles.begin(), "" );
		m_appSettings.RecentFiles[0] = strPath.data();

		if( m_appSettings.RecentFiles.size() > 9 ) 
			m_appSettings.RecentFiles.erase( m_appSettings.RecentFiles.begin() + 9 );

		SettingsWriteToFile();
	}

	UpdateRecentFilesMenu();
}

void CCookieMachineDlg::AddPathToRecentDirsList( str_t strPath )
{
	SettingsReadFromFile();

	CMenu* menu = this->GetMenu();
	CMenu* file_menu = menu->GetSubMenu( 0 );
	CMenu* recent_files_menu = file_menu->GetSubMenu( 5 );

	strPath = _2Lower( (char*)strPath.data() );

	bool	bAlreadyAdded = false;
	int		iExistingItemIdx;
	CString	strTemp, strTemp2;


	strTemp = strPath.c_str();
	strTemp.MakeUpper();
	strTemp.Trim();


	//check whether path is already in the list
	if( !m_appSettings.RecentDirs.empty() )
	{
		for( UINT i = 0; i < m_appSettings.RecentDirs.size(); i++ )
		{
			strTemp2 = m_appSettings.RecentDirs[i];
			strTemp2.MakeUpper();
			strTemp2.Trim();

			if( strTemp == strTemp2 )
			{
				iExistingItemIdx = i;
				bAlreadyAdded = true;
				break;
			}
		}
	}

	//if not -- add path to the recent list, otherwise -- move to the top of the recents
	if( bAlreadyAdded )
	{
		m_appSettings.RecentDirs.erase( m_appSettings.RecentDirs.begin() + iExistingItemIdx );
		m_appSettings.RecentDirs.insert( m_appSettings.RecentDirs.begin(), strPath.data() ); 
	}
	else
	{
		//move existing entries 1 down
		m_appSettings.RecentDirs.insert( m_appSettings.RecentDirs.begin(), "" );
		m_appSettings.RecentDirs[0] = strPath.data();

		if( m_appSettings.RecentDirs.size() > 9 ) 
			m_appSettings.RecentDirs.erase( m_appSettings.RecentDirs.begin() + 9 );

		SettingsWriteToFile();
	}

	UpdateRecentDirsMenu();
}

void CCookieMachineDlg::SettingsReadFromFile()
{
	FILE*		stream = NULL;
	char		str[256];
	CString		tmp_str;

	stream = fopen( (m_strAppPath + "\\" + m_strIniFilename).GetBuffer(), "r" );

	if( !stream )
		return;

	m_appSettings.CameraToCurrent = false;
	m_appSettings.RecentFiles.clear();
	m_appSettings.RecentDirs.clear();

	// move to the beginning of the file
	fseek( stream, 0L, SEEK_SET );

	// read file
	while( !feof( stream ) )
	{
		//read line
		fscanf( stream, "%s", str );

		// read [General] section
		if( strcmp("[General]", str) == 0 )
		{
			fscanf( stream, "%s", str );

			// read "DefaultDirectory" param
			if( strcmp("DefaultDirectory", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fgets( str, 128, stream );
				m_appSettings.FilesPath = str;
				
				m_appSettings.FilesPath = CSTRim( m_appSettings.FilesPath );
			}

			fscanf( stream, "%s", str );

			// read "DefaultFiletype" param
			if( strcmp("DefaultFiletype", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fscanf( stream, "%s", str );

				//convert to uppercase
				sprintf(str, "%s", _2Upper( str ));

				if( strcmp( "XSI", str ) == 0 )
					m_appSettings.DefaultFileType  = FT_XSI;

				if( strcmp( "EFN", str ) == 0 )
					m_appSettings.DefaultFileType  = FT_EFN;

				if( strcmp( "SCN", str ) == 0 )
					m_appSettings.DefaultFileType  = FT_SCN;

				if( strcmp( "ALL_SUPPORTED", str ) == 0 )
					m_appSettings.DefaultFileType  = FT_ALLSUPPORTED;
			}

			fscanf( stream, "%s", str );

			// read "RepairXSIFiles" param
			if( strcmp("RepairXSIFiles", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fscanf( stream, "%s", str );

				str_t temp_str = _2Upper( str );
				temp_str = TrimStr( temp_str );
				
				if( temp_str == "TRUE" )
					m_appSettings.RepairXSIFiles = true;
				else
					m_appSettings.RepairXSIFiles = false;
			}

			fscanf( stream, "%s", str );

			// read "FilterObjByPrefx" param
			if( strcmp("FilterObjByPrefx", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fscanf( stream, "%s", str );

				str_t temp_str = _2Upper( str );
				temp_str = TrimStr( temp_str );
				
				if( temp_str == "TRUE" )
					m_appSettings.FilterObjsByPrefix = true;
				else
					m_appSettings.FilterObjsByPrefix = false;
			}

			fscanf( stream, "%s", str );

			// read "SCNAltNullIcon" param
			if( strcmp("SCNAltNullIcon", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fscanf( stream, "%s", str );

				str_t temp_str = _2Upper( str );
				temp_str = TrimStr( temp_str );
				
				if( temp_str == "TRUE" )
					m_appSettings.ScnAlternateNullIco = true;
				else
					m_appSettings.ScnAlternateNullIco = false;
			}

			fscanf( stream, "%s", str );

			// read "EngineDirectory" param
			if( strcmp("EngineDirectory", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fgets( str, 128, stream );
				m_appSettings.EngineFolder = str;
				m_appSettings.EngineFolder = CSTRim( m_appSettings.EngineFolder );
			}

			fscanf( stream, "%s", str );

			// read "StartParams" param
			if( strcmp("StartParams", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fgets( str, 128, stream );
				m_appSettings.StartParams = str;
				m_appSettings.StartParams = CSTRim( m_appSettings.StartParams );
			}

			fscanf( stream, "%s", str );

			// read "ExpertMode" param
			if( strcmp( "ExpertMode", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fscanf( stream, "%s", str );

				str_t temp_str = _2Upper( str );
				temp_str = TrimStr( temp_str );
				
				if( temp_str == "TRUE" )
					m_appSettings.ExpertMode = true;
				else
					m_appSettings.ExpertMode = false;
			}

			fscanf( stream, "%s", str );

			// read "EngineDebugModeOn" param
			if( strcmp( "EngineDebugModeOn", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fscanf( stream, "%s", str );

				str_t temp_str = _2Upper( str );
				temp_str = TrimStr( temp_str );

				m_appSettings.EngineDebugModeOn = ( temp_str == "TRUE" );
			}

			fscanf( stream, "%s", str );

			// read "AskForSaving" param
			if( strcmp( "AskForSaving", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fscanf( stream, "%s", str );

				str_t temp_str = _2Upper( str );
				temp_str = TrimStr( temp_str );
				
				if( temp_str == "TRUE" )
					m_appSettings.AskForSavingOnAppQuit = true;
				else
					m_appSettings.AskForSavingOnAppQuit = false;
			}

			fscanf( stream, "%s", str );

			// read "1stMonitorOnly" param
			if( strcmp( "1stMonitorOnly", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fscanf( stream, "%s", str );

				str_t temp_str = _2Upper( str );
				temp_str = TrimStr( temp_str );
				
				if( temp_str == "TRUE" )
					m_appSettings.FirstMonitorOnly = true;
				else
					m_appSettings.FirstMonitorOnly = false;
			}

			fscanf( stream, "%s", str );

			// read "CustButtnonACapt" param
			if( strcmp("CustButtnonACapt", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fgets( str, 128, stream );
				m_appSettings.Button_A_Capt = str;
				m_appSettings.Button_A_Capt = CSTRim( m_appSettings.Button_A_Capt );
			}

			fscanf( stream, "%s", str );

			// read "CustButtnonAPath" param
			if( strcmp("CustButtnonAPath", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fgets( str, 128, stream );
				m_appSettings.Button_A_Path = str;
				m_appSettings.Button_A_Path = CSTRim( m_appSettings.Button_A_Path );
			}

			fscanf( stream, "%s", str );

			// read "CustButtnonBCapt" param
			if( strcmp("CustButtnonBCapt", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fgets( str, 128, stream );
				m_appSettings.Button_B_Capt = str;
				m_appSettings.Button_B_Capt = CSTRim( m_appSettings.Button_B_Capt );
			}

			fscanf( stream, "%s", str );

			// read "CustButtnonBPath" param
			if( strcmp("CustButtnonBPath", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fgets( str, 128, stream );
				m_appSettings.Button_B_Path = str;
				m_appSettings.Button_B_Path = CSTRim( m_appSettings.Button_B_Path );
			}

			fscanf( stream, "%s", str );

			// read "SwitchBackOnDebug" param
			if( strcmp( "SwitchBackOnDebug", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fscanf( stream, "%s", str );

				str_t temp_str = _2Upper( str );
				temp_str = TrimStr( temp_str );
				
				if( temp_str == "TRUE" )
					m_appSettings.SwitchBackOnDebug = true;
				else
					m_appSettings.SwitchBackOnDebug = false;
			}

		} // END read [General] section

		// read [Lighting] section
		if( strcmp("[Lighting]", str) == 0 )
		{
			// read "Enabled" param
			fscanf( stream, "%s", str );
			if( strcmp("Enabled", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fscanf( stream, "%s", str );

				//convert to uppercase
				sprintf(str, "%s", _2Upper( str ));

				if( strcmp("TRUE", str) == 0 )
					m_appSettings.LightingEnabled = true;
				else
					m_appSettings.LightingEnabled = false;
			}

			//read LightAmbientRGB param
			fscanf( stream, "%s", str );
			if( strcmp("LightAmbientRGB", str) == 0 )
			{
				int r, g, b;
				r = g = b = 0;

				fscanf( stream, "%s", str );
				fscanf( stream, "%s", str );

				tmp_str = str;
				tmp_str.Trim();
				tmp_str = tmp_str.Left( tmp_str.GetLength() - 1 );
                r = atoi(tmp_str.GetBuffer());

				fscanf( stream, "%s", str );
				tmp_str = str;
				tmp_str.Trim();
				tmp_str = tmp_str.Left( tmp_str.GetLength() - 1 );
                g = atoi(tmp_str.GetBuffer());

				fscanf( stream, "%s", str );
				tmp_str = str;
				tmp_str.Trim();
                b = atoi(tmp_str.GetBuffer());

				m_appSettings.LightAmbientColor = RGB(r, g, b);
			}

			//read LightDiffuseRGB param
			fscanf( stream, "%s", str );
			if( strcmp("LightDiffuseRGB", str) == 0 )
			{
				int r, g, b;
				r = g = b = 0;

				fscanf( stream, "%s", str );
				fscanf( stream, "%s", str );

				tmp_str = str;
				tmp_str.Trim();
				tmp_str = tmp_str.Left( tmp_str.GetLength() - 1 );
                r = atoi(tmp_str.GetBuffer());

				fscanf( stream, "%s", str );
				tmp_str = str;
				tmp_str.Trim();
				tmp_str = tmp_str.Left( tmp_str.GetLength() - 1 );
                g = atoi(tmp_str.GetBuffer());

				fscanf( stream, "%s", str );
				tmp_str = str;
				tmp_str.Trim();
                b = atoi(tmp_str.GetBuffer());

				m_appSettings.LightDiffuseColor = RGB(r, g, b);
			}

			//read AmbientRGB param
			fscanf( stream, "%s", str );
			if( strcmp("AmbientRGB", str) == 0 )
			{
				int r, g, b;
				r = g = b = 0;

				fscanf( stream, "%s", str );
				fscanf( stream, "%s", str );

				tmp_str = str;
				tmp_str.Trim();
				tmp_str = tmp_str.Left( tmp_str.GetLength() - 1 );
                r = atoi(tmp_str.GetBuffer());

				fscanf( stream, "%s", str );
				tmp_str = str;
				tmp_str.Trim();
				tmp_str = tmp_str.Left( tmp_str.GetLength() - 1 );
                g = atoi(tmp_str.GetBuffer());

				fscanf( stream, "%s", str );
				tmp_str = str;
				tmp_str.Trim();
                b = atoi(tmp_str.GetBuffer());

				m_appSettings.AmbientColor = RGB(r, g, b);
			}

			//read AttachToCamera param
			fscanf( stream, "%s", str );            
			if( strcmp("AttachToCamera", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fscanf( stream, "%s", str );

				//convert to uppercase
				sprintf(str, "%s", _2Upper( str ));

				if( strcmp("TRUE", str) == 0 )
					m_appSettings.AttachToCamera = true;
				else
					m_appSettings.AttachToCamera = false;
			}

		} // END read [Lighting] section


		// read [Camera] section
		if( strcmp("[Camera]", str) == 0 )
		{
			// read "Position" param
			fscanf( stream, "%s", str );

			if( strcmp("Position", str) == 0 )
			{
				fscanf( stream, "%s", str );

				//read pos.x
				fscanf( stream, "%s", str );
				tmp_str = str;
				tmp_str.Trim();
				tmp_str = tmp_str.Left( tmp_str.GetLength() - 1 );
                m_appSettings.DefaultCamera.pos.x = atof(tmp_str.GetBuffer());

				//read pos.y
				fscanf( stream, "%s", str );
				tmp_str = str;
				tmp_str.Trim();
				tmp_str = tmp_str.Left( tmp_str.GetLength() - 1 );
                m_appSettings.DefaultCamera.pos.y = atof(tmp_str.GetBuffer());

				//read pos.z
				fscanf( stream, "%s", str );
				tmp_str = str;
                m_appSettings.DefaultCamera.pos.z = atof(tmp_str.GetBuffer());
			}


			// read "Direction" param
			fscanf( stream, "%s", str );
			if( strcmp("Direction", str) == 0 )
			{
				fscanf( stream, "%s", str );

				//read dir.x
				fscanf( stream, "%s", str );
				tmp_str = str;
				tmp_str.Trim();
				tmp_str = tmp_str.Left( tmp_str.GetLength() - 1 );
                m_appSettings.DefaultCamera.dir.x = atof(tmp_str.GetBuffer());

				//read dir.y
				fscanf( stream, "%s", str );
				tmp_str = str;
				tmp_str.Trim();
				tmp_str = tmp_str.Left( tmp_str.GetLength() - 1 );
                m_appSettings.DefaultCamera.dir.y = atof(tmp_str.GetBuffer());

				//read dir.z
				fscanf( stream, "%s", str );
				tmp_str = str;
                m_appSettings.DefaultCamera.dir.z = atof(tmp_str.GetBuffer());
			}


			// read "Upvector" param
			fscanf( stream, "%s", str );
			if( strcmp("Upvector", str) == 0 )
			{
				fscanf( stream, "%s", str );

				//read upvec.x
				fscanf( stream, "%s", str );
				tmp_str = str;
				tmp_str.Trim();
				tmp_str = tmp_str.Left( tmp_str.GetLength() - 1 );
                m_appSettings.DefaultCamera.upvector.x = atof(tmp_str.GetBuffer());

				//read upvec.y
				fscanf( stream, "%s", str );
				tmp_str = str;
				tmp_str.Trim();
				tmp_str = tmp_str.Left( tmp_str.GetLength() - 1 );
                m_appSettings.DefaultCamera.upvector.y = atof(tmp_str.GetBuffer());

				//read upvec.z
				fscanf( stream, "%s", str );
				tmp_str = str;
                m_appSettings.DefaultCamera.upvector.z = atof(tmp_str.GetBuffer());
			}

			// read "WorldRotate" param
			fscanf( stream, "%s", str );
			if( strcmp("WorldRotate", str) == 0 )
			{
				fscanf( stream, "%s", str );

				//read rot.x
				fscanf( stream, "%s", str );
				tmp_str = str;
				tmp_str.Trim();
				tmp_str = tmp_str.Left( tmp_str.GetLength() - 1 );
                m_appSettings.DefaultWorldStates.rot.x = atof(tmp_str.GetBuffer());
				ClampAngle( m_appSettings.DefaultWorldStates.rot.x );
				m_appSettings.DefaultWorldStates.rot.x = D3DXToRadian( m_appSettings.DefaultWorldStates.rot.x );

				//read rot.y
				fscanf( stream, "%s", str );
				tmp_str = str;
				tmp_str.Trim();
				tmp_str = tmp_str.Left( tmp_str.GetLength() - 1 );
                m_appSettings.DefaultWorldStates.rot.y = atof(tmp_str.GetBuffer());
				ClampAngle( m_appSettings.DefaultWorldStates.rot.y );
				m_appSettings.DefaultWorldStates.rot.y = D3DXToRadian( m_appSettings.DefaultWorldStates.rot.y );

				//read rot.z
				fscanf( stream, "%s", str );
				tmp_str = str;
                m_appSettings.DefaultWorldStates.rot.z = atof(tmp_str.GetBuffer());
				ClampAngle( m_appSettings.DefaultWorldStates.rot.z );
				m_appSettings.DefaultWorldStates.rot.z = D3DXToRadian( m_appSettings.DefaultWorldStates.rot.z );
			}

			// read "ViewportTranslate" param
			fscanf( stream, "%s", str );
			if( strcmp("ViewportTranslate", str) == 0 )
			{
				fscanf( stream, "%s", str );

				//read transl.x
				fscanf( stream, "%s", str );
				tmp_str = str;
				tmp_str.Trim();
				tmp_str = tmp_str.Left( tmp_str.GetLength() - 1 );
                m_appSettings.DefaultWorldStates.pos.x = atof(tmp_str.GetBuffer());

				//read transl.y
				fscanf( stream, "%s", str );
				tmp_str = str;
				tmp_str.Trim();
				tmp_str = tmp_str.Left( tmp_str.GetLength() - 1 );
                m_appSettings.DefaultWorldStates.pos.y = atof(tmp_str.GetBuffer());

				//read transl.z
				fscanf( stream, "%s", str );
				tmp_str = str;
                m_appSettings.DefaultWorldStates.pos.z = atof(tmp_str.GetBuffer());
			}

			// read "ViewportBackcolor" param
			fscanf( stream, "%s", str );
			if( strcmp("ViewportBackcolor", str) == 0 )
			{
				int r, g, b;
				r = g = b = 0;

				fscanf( stream, "%s", str );
				fscanf( stream, "%s", str );

				tmp_str = str;
				tmp_str.Trim();
				tmp_str = tmp_str.Left( tmp_str.GetLength() - 1 );
                r = atoi(tmp_str.GetBuffer());

				fscanf( stream, "%s", str );
				tmp_str = str;
				tmp_str.Trim();
				tmp_str = tmp_str.Left( tmp_str.GetLength() - 1 );
                g = atoi(tmp_str.GetBuffer());

				fscanf( stream, "%s", str );
				tmp_str = str;
				tmp_str.Trim();
                b = atoi(tmp_str.GetBuffer());

				m_appSettings.ViewportBackColor = RGB(r, g, b);
			}

			// read "ViewportCulling" param
			fscanf( stream, "%s", str );
			if( strcmp("ViewportCulling", str) == 0 )
			{
				fscanf( stream, "%s", str );

				//read value
				fscanf( stream, "%s", str );

				//convert to uppercase
				sprintf(str, "%s", _2Upper( str ));

				if( strcmp("TRUE", str) == 0 )
					m_appSettings.ViewportBackfaceCull = true;
				else
					m_appSettings.ViewportBackfaceCull = false;
			}

			// read "Preserve4_3" param
			fscanf( stream, "%s", str );
			if( strcmp("Preserve4_3", str) == 0 )
			{
				fscanf( stream, "%s", str );

				//read value
				fscanf( stream, "%s", str );

				//convert to uppercase
				sprintf(str, "%s", _2Upper( str ));

				if( strcmp("TRUE", str) == 0 )
					m_appSettings.ViewportPreserve43 = true;
				else
					m_appSettings.ViewportPreserve43 = false;
			}

			// read "FieldOfView" param
			fscanf( stream, "%s", str );
			if( strcmp("FieldOfView", str) == 0 )
			{
				fscanf( stream, "%s", str );

				//read fov
				fscanf( stream, "%s", str );
				tmp_str = str;
                m_appSettings.DefaultCamera.hfov = atof(tmp_str.GetBuffer());
			}

			// read "ResetOnFileOpen" param
			fscanf( stream, "%s", str );
			if( strcmp("ResetOnFileOpen", str) == 0 )
			{
				fscanf( stream, "%s", str );

				//read value
				fscanf( stream, "%s", str );

				//convert to uppercase
				sprintf(str, "%s", _2Upper( str ));

				if( strcmp("TRUE", str) == 0 )
					m_appSettings.ResetCameraOnFileOpen = true;
				else
					m_appSettings.ResetCameraOnFileOpen = false;
			}

		} // END read [Camera] section

		// read [Grid] section
		if( strcmp("[Grid]", str) == 0 )
		{
			// read "Visible" param
			fscanf( stream, "%s", str );
			if( strcmp("Visible", str) == 0 )
			{
				fscanf( stream, "%s", str );

				//read value
				fscanf( stream, "%s", str );

				//convert to uppercase
				sprintf(str, "%s", _2Upper( str ));

				if( strcmp("TRUE", str) == 0 )
					m_appSettings.GridVisible = true;
				else
					m_appSettings.GridVisible = false;
			}

			// read "Size" param
			fscanf( stream, "%s", str );
			if( strcmp("Size", str) == 0 )
			{
				fscanf( stream, "%s", str );

				//read fov
				fscanf( stream, "%s", str );
				tmp_str = str;
                m_appSettings.GridSize = atoi( tmp_str.GetBuffer() );
			}

			// read "Cellsize" param
			fscanf( stream, "%s", str );
			if( strcmp("Cellsize", str) == 0 )
			{
				fscanf( stream, "%s", str );

				//read fov
				fscanf( stream, "%s", str );
				tmp_str = str;
                m_appSettings.GridCellSize = atof( tmp_str.GetBuffer() );
			}

			// read 'WiresColor" param
			fscanf( stream, "%s", str );
			if( strcmp("WiresColor", str) == 0 )
			{
				int r, g, b;
				r = g = b = 0;

				fscanf( stream, "%s", str );
				fscanf( stream, "%s", str );

				tmp_str = str;
				tmp_str.Trim();
				tmp_str = tmp_str.Left( tmp_str.GetLength() - 1 );
                r = atoi(tmp_str.GetBuffer());

				fscanf( stream, "%s", str );
				tmp_str = str;
				tmp_str.Trim();
				tmp_str = tmp_str.Left( tmp_str.GetLength() - 1 );
                g = atoi(tmp_str.GetBuffer());

				fscanf( stream, "%s", str );
				tmp_str = str;
				tmp_str.Trim();
                b = atoi(tmp_str.GetBuffer());

				m_appSettings.GridWiresColor = RGB(r, g, b);
			}

			// read 'CrossColor" param
			fscanf( stream, "%s", str );
			if( strcmp("CrossColor", str) == 0 )
			{
				int r, g, b;
				r = g = b = 0;

				fscanf( stream, "%s", str );
				fscanf( stream, "%s", str );

				tmp_str = str;
				tmp_str.Trim();
				tmp_str = tmp_str.Left( tmp_str.GetLength() - 1 );
                r = atoi(tmp_str.GetBuffer());

				fscanf( stream, "%s", str );
				tmp_str = str;
				tmp_str.Trim();
				tmp_str = tmp_str.Left( tmp_str.GetLength() - 1 );
                g = atoi(tmp_str.GetBuffer());

				fscanf( stream, "%s", str );
				tmp_str = str;
				tmp_str.Trim();
                b = atoi(tmp_str.GetBuffer());

				m_appSettings.GridCrossColor = RGB(r, g, b);
			}
		}



		// read [CamLightLicons] section
		if( strcmp("[CamLightLicons]", str) == 0 )
		{
			fscanf( stream, "%s", str );

			// read "Scale" param
			if( strcmp("Scale", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fscanf( stream, "%s", str );
				m_appSettings.IconsScaling = atof( str );
			}
		} // END read [CamLightLicons] section



		// read [RecentFiles] section
		if( strcmp("[RecentFiles]", str) == 0 )
		{
			CString tmp_strr;

			// read "File1" param
			fscanf( stream, "%s", str );
			if( strcmp("File1", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fgets( str, 128, stream );
				tmp_strr = str;

				if(	tmp_strr.GetBuffer()[0] = '"' )
					tmp_strr = tmp_strr.Left( tmp_strr.GetLength() - 1 );

				if( tmp_strr.GetBuffer()[tmp_strr.GetLength()] = '"' )
					tmp_strr = tmp_strr.Right( tmp_strr.GetLength() - 1 );

				if(	tmp_strr.GetBuffer()[0] = '"' )
					tmp_strr = tmp_strr.Left( tmp_strr.GetLength() - 1 );

				if( tmp_strr.GetBuffer()[tmp_strr.GetLength()] = '"' )
					tmp_strr = tmp_strr.Right( tmp_strr.GetLength() - 1 );

				tmp_strr.Trim();

				if( tmp_strr != "" )
					m_appSettings.RecentFiles.push_back( tmp_strr );
			}

			// read "File2" param
			fscanf( stream, "%s", str );
			if( strcmp("File2", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fgets( str, 128, stream );
				tmp_strr = str;

				if(	tmp_strr.GetBuffer()[0] = '"' )
					tmp_strr = tmp_strr.Left( tmp_strr.GetLength() - 1 );

				if( tmp_strr.GetBuffer()[tmp_strr.GetLength()] = '"' )
					tmp_strr = tmp_strr.Right( tmp_strr.GetLength() - 1 );

				if(	tmp_strr.GetBuffer()[0] = '"' )
					tmp_strr = tmp_strr.Left( tmp_strr.GetLength() - 1 );

				if( tmp_strr.GetBuffer()[tmp_strr.GetLength()] = '"' )
					tmp_strr = tmp_strr.Right( tmp_strr.GetLength() - 1 );

				tmp_strr.Trim();

				if( tmp_strr != "" )
					m_appSettings.RecentFiles.push_back( tmp_strr );
			}

			// read "File3" param
			fscanf( stream, "%s", str );
			if( strcmp("File3", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fgets( str, 128, stream );
				tmp_strr = str;

				if(	tmp_strr.GetBuffer()[0] = '"' )
					tmp_strr = tmp_strr.Left( tmp_strr.GetLength() - 1 );

				if( tmp_strr.GetBuffer()[tmp_strr.GetLength()] = '"' )
					tmp_strr = tmp_strr.Right( tmp_strr.GetLength() - 1 );

				if(	tmp_strr.GetBuffer()[0] = '"' )
					tmp_strr = tmp_strr.Left( tmp_strr.GetLength() - 1 );

				if( tmp_strr.GetBuffer()[tmp_strr.GetLength()] = '"' )
					tmp_strr = tmp_strr.Right( tmp_strr.GetLength() - 1 );

				tmp_strr.Trim();

				if( tmp_strr != "" )
					m_appSettings.RecentFiles.push_back( tmp_strr );
			}

			// read "File4" param
			fscanf( stream, "%s", str );
			if( strcmp("File4", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fgets( str, 128, stream );
				tmp_strr = str;

				if(	tmp_strr.GetBuffer()[0] = '"' )
					tmp_strr = tmp_strr.Left( tmp_strr.GetLength() - 1 );

				if( tmp_strr.GetBuffer()[tmp_strr.GetLength()] = '"' )
					tmp_strr = tmp_strr.Right( tmp_strr.GetLength() - 1 );

				if(	tmp_strr.GetBuffer()[0] = '"' )
					tmp_strr = tmp_strr.Left( tmp_strr.GetLength() - 1 );

				if( tmp_strr.GetBuffer()[tmp_strr.GetLength()] = '"' )
					tmp_strr = tmp_strr.Right( tmp_strr.GetLength() - 1 );

				tmp_strr.Trim();

				if( tmp_strr != "" )
					m_appSettings.RecentFiles.push_back( tmp_strr );
			}

			// read "File5" param
			fscanf( stream, "%s", str );
			if( strcmp("File5", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fgets( str, 128, stream );
				tmp_strr = str;

				if(	tmp_strr.GetBuffer()[0] = '"' )
					tmp_strr = tmp_strr.Left( tmp_strr.GetLength() - 1 );

				if( tmp_strr.GetBuffer()[tmp_strr.GetLength()] = '"' )
					tmp_strr = tmp_strr.Right( tmp_strr.GetLength() - 1 );

				if(	tmp_strr.GetBuffer()[0] = '"' )
					tmp_strr = tmp_strr.Left( tmp_strr.GetLength() - 1 );

				if( tmp_strr.GetBuffer()[tmp_strr.GetLength()] = '"' )
					tmp_strr = tmp_strr.Right( tmp_strr.GetLength() - 1 );

				tmp_strr.Trim();

				if( tmp_strr != "" )
					m_appSettings.RecentFiles.push_back( tmp_strr );
			}

			// read "File6" param
			fscanf( stream, "%s", str );
			if( strcmp("File6", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fgets( str, 128, stream );
				tmp_strr = str;

				if(	tmp_strr.GetBuffer()[0] = '"' )
					tmp_strr = tmp_strr.Left( tmp_strr.GetLength() - 1 );

				if( tmp_strr.GetBuffer()[tmp_strr.GetLength()] = '"' )
					tmp_strr = tmp_strr.Right( tmp_strr.GetLength() - 1 );

				if(	tmp_strr.GetBuffer()[0] = '"' )
					tmp_strr = tmp_strr.Left( tmp_strr.GetLength() - 1 );

				if( tmp_strr.GetBuffer()[tmp_strr.GetLength()] = '"' )
					tmp_strr = tmp_strr.Right( tmp_strr.GetLength() - 1 );

				tmp_strr.Trim();

				if( tmp_strr != "" )
					m_appSettings.RecentFiles.push_back( tmp_strr );
			}

			// read "File7" param
			fscanf( stream, "%s", str );
			if( strcmp("File7", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fgets( str, 128, stream );
				tmp_strr = str;

				if(	tmp_strr.GetBuffer()[0] = '"' )
					tmp_strr = tmp_strr.Left( tmp_strr.GetLength() - 1 );

				if( tmp_strr.GetBuffer()[tmp_strr.GetLength()] = '"' )
					tmp_strr = tmp_strr.Right( tmp_strr.GetLength() - 1 );

				if(	tmp_strr.GetBuffer()[0] = '"' )
					tmp_strr = tmp_strr.Left( tmp_strr.GetLength() - 1 );

				if( tmp_strr.GetBuffer()[tmp_strr.GetLength()] = '"' )
					tmp_strr = tmp_strr.Right( tmp_strr.GetLength() - 1 );

				tmp_strr.Trim();

				if( tmp_strr != "" )
					m_appSettings.RecentFiles.push_back( tmp_strr );
			}

			// read "File8" param
			fscanf( stream, "%s", str );
			if( strcmp("File8", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fgets( str, 128, stream );
				tmp_strr = str;

				if(	tmp_strr.GetBuffer()[0] = '"' )
					tmp_strr = tmp_strr.Left( tmp_strr.GetLength() - 1 );

				if( tmp_strr.GetBuffer()[tmp_strr.GetLength()] = '"' )
					tmp_strr = tmp_strr.Right( tmp_strr.GetLength() - 1 );

				if(	tmp_strr.GetBuffer()[0] = '"' )
					tmp_strr = tmp_strr.Left( tmp_strr.GetLength() - 1 );

				if( tmp_strr.GetBuffer()[tmp_strr.GetLength()] = '"' )
					tmp_strr = tmp_strr.Right( tmp_strr.GetLength() - 1 );

				tmp_strr.Trim();

				if( tmp_strr != "" )
					m_appSettings.RecentFiles.push_back( tmp_strr );
			}

			// read "File9" param
			fscanf( stream, "%s", str );
			if( strcmp("File9", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fgets( str, 128, stream );
				tmp_strr = str;

				if(	tmp_strr.GetBuffer()[0] = '"' )
					tmp_strr = tmp_strr.Left( tmp_strr.GetLength() - 1 );

				if( tmp_strr.GetBuffer()[tmp_strr.GetLength()] = '"' )
					tmp_strr = tmp_strr.Right( tmp_strr.GetLength() - 1 );

				if(	tmp_strr.GetBuffer()[0] = '"' )
					tmp_strr = tmp_strr.Left( tmp_strr.GetLength() - 1 );

				if( tmp_strr.GetBuffer()[tmp_strr.GetLength()] = '"' )
					tmp_strr = tmp_strr.Right( tmp_strr.GetLength() - 1 );

				tmp_strr.Trim();

				if( tmp_strr != "" )
					m_appSettings.RecentFiles.push_back( tmp_strr );
			}

		} // END read [RecentFiles] section


		// read [RecentDirectories] section
		if( strcmp("[RecentDirectories]", str) == 0 )
		{
			CString tmp_strr;

			// read "Dir1" param
			fscanf( stream, "%s", str );
			if( strcmp("Dir1", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fgets( str, 128, stream );
				tmp_strr = str;

				tmp_strr.Replace( '\"', ' ' );
				tmp_strr.Trim();

				if( tmp_strr != "" )
					m_appSettings.RecentDirs.push_back( tmp_strr );
			}

			// read "Dir2" param
			fscanf( stream, "%s", str );
			if( strcmp("Dir2", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fgets( str, 128, stream );
				tmp_strr = str;

				tmp_strr.Replace( '\"', ' ' );
				tmp_strr.Trim();

				if( tmp_strr != "" )
					m_appSettings.RecentDirs.push_back( tmp_strr );
			}

			// read "Dir3" param
			fscanf( stream, "%s", str );
			if( strcmp("Dir3", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fgets( str, 128, stream );
				tmp_strr = str;

				tmp_strr.Replace( '\"', ' ' );
				tmp_strr.Trim();

				if( tmp_strr != "" )
					m_appSettings.RecentDirs.push_back( tmp_strr );
			}

			// read "Dir4" param
			fscanf( stream, "%s", str );
			if( strcmp("Dir4", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fgets( str, 128, stream );
				tmp_strr = str;

				tmp_strr.Replace( '\"', ' ' );
				tmp_strr.Trim();

				if( tmp_strr != "" )
					m_appSettings.RecentDirs.push_back( tmp_strr );
			}

			// read "Dir5" param
			fscanf( stream, "%s", str );
			if( strcmp("Dir5", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fgets( str, 128, stream );
				tmp_strr = str;

				tmp_strr.Replace( '\"', ' ' );
				tmp_strr.Trim();

				if( tmp_strr != "" )
					m_appSettings.RecentDirs.push_back( tmp_strr );
			}

			// read "Dir6" param
			fscanf( stream, "%s", str );
			if( strcmp("Dir6", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fgets( str, 128, stream );
				tmp_strr = str;

				tmp_strr.Replace( '\"', ' ' );
				tmp_strr.Trim();

				if( tmp_strr != "" )
					m_appSettings.RecentDirs.push_back( tmp_strr );
			}

			// read "Dir7" param
			fscanf( stream, "%s", str );
			if( strcmp("Dir7", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fgets( str, 128, stream );
				tmp_strr = str;

				tmp_strr.Replace( '\"', ' ' );
				tmp_strr.Trim();

				if( tmp_strr != "" )
					m_appSettings.RecentDirs.push_back( tmp_strr );
			}

			// read "Dir8" param
			fscanf( stream, "%s", str );
			if( strcmp("Dir8", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fgets( str, 128, stream );
				tmp_strr = str;

				tmp_strr.Replace( '\"', ' ' );
				tmp_strr.Trim();

				if( tmp_strr != "" )
					m_appSettings.RecentDirs.push_back( tmp_strr );
			}

			// read "Dir9" param
			fscanf( stream, "%s", str );
			if( strcmp("Dir9", str) == 0 )
			{
				fscanf( stream, "%s", str );
				fgets( str, 128, stream );
				tmp_strr = str;

				tmp_strr.Replace( '\"', ' ' );
				tmp_strr.Trim();

				if( tmp_strr != "" )
					m_appSettings.RecentDirs.push_back( tmp_strr );
			}

		} // END read [RecentDirs] section

	} // while( !feof( stream ) )

	fclose( stream );
	stream = NULL;
}

void CCookieMachineDlg::SettingsApplyToApp(const AppSettingsData& appSettings)
{
	str_t func_decl_str = "void CCookieMachineDlg::SettingsApplyToApp(const AppSettingsData& appSettings)";
	
	try
	{
		m_appSettings = appSettings;
		SettingsApplyToApp();
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}
}

void CCookieMachineDlg::SettingsApplyToApp()
{
	str_t func_decl_str = "void CCookieMachineDlg::SettingsApplyToApp()";
	
	try
	{
		if( !g_pd3dDevice )
			return;

		lastFileType = m_appSettings.DefaultFileType;

		int rA = GetRValue( m_appSettings.LightAmbientColor );
		int gA = GetGValue( m_appSettings.LightAmbientColor );
		int bA = GetBValue( m_appSettings.LightAmbientColor );

		int rD = GetRValue( m_appSettings.LightDiffuseColor );
		int gD = GetGValue( m_appSettings.LightDiffuseColor );
		int bD = GetBValue( m_appSettings.LightDiffuseColor );

		light.Ambient.r = rA / 255.0f;
		light.Ambient.g = gA / 255.0f;
		light.Ambient.b = bA / 255.0f;

		light.Diffuse.r = rD / 255.0f;
		light.Diffuse.g = gD / 255.0f;
		light.Diffuse.b = bD / 255.0f;

		g_pd3dDevice->SetLight( 0, &light );

		m_ViewMgr.m_camPerspective.hfov = m_appSettings.DefaultCamera.hfov;

		if( m_appSettings.LightingEnabled )
		{
			g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
			g_pd3dDevice->LightEnable( 0, TRUE );
		}
		else
		{
			g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
			g_pd3dDevice->LightEnable( 0, FALSE );
		}

		if( pBaseScene )
		{
			if( pBaseScene->GetType() == FT_EFN_MESHLIST )
			{
				if( m_appSettings.ScnAlternateNullIco )
					((CEfnMeshlist*)pBaseScene)->SetNullIcon( pNullIco );
				else
					((CEfnMeshlist*)pBaseScene)->SetNullIcon( NULL );
			}

			if( pBaseScene->GetType() == FT_SCN )
			{
				CSceneFile*		scene		= (CSceneFile*)pBaseScene;
				CEfnMeshlist*	meshlist	= (CEfnMeshlist*)scene->GetMeshlist();

				if( m_appSettings.ScnAlternateNullIco )
					meshlist->SetNullIcon( pNullIco );
				else
					meshlist->SetNullIcon( NULL );
			}
		}


		g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_ARGB( 0,
																	GetRValue(m_appSettings.AmbientColor), 
																	GetGValue(m_appSettings.AmbientColor), 
																	GetBValue(m_appSettings.AmbientColor) ) );

		if( m_appSettings.CameraToCurrent )
		{
			m_appSettings.CameraToCurrent		= false;
			m_appSettings.DefaultCamera			= *m_ViewMgr.m_camCurrent;
			//m_appSettings.DefaultWorldStates	= worldStates;
		}
		else
		{
			if( !m_bOnInitDialogFinished )
			{
				*m_ViewMgr.m_camCurrent = m_appSettings.DefaultCamera;
			}
		}


		if( !m_bOnInitDialogFinished )
		{
			SwitchCulling( m_appSettings.ViewportBackfaceCull );
			
			if( m_appSettings.FirstMonitorOnly )
				OnMenuIldarWindow();

			//modelStates.IsCullingOn = !m_appSettings.ViewportBackfaceCull;
		}
		

		m_bResetCameraOnFileOpen = m_appSettings.ResetCameraOnFileOpen;


		if( m_appSettings.RecentFilesClear )
		{
			m_appSettings.RecentFiles.clear();
			m_appSettings.RecentDirs.clear();
		}

		m_strLastFileOpenDirectory = m_appSettings.FilesPath;

		UpdateRecentFilesMenu();
		UpdateRecentDirsMenu();
		UpdateContextMenus();
		UpdateUISizing();
		OnLostDevice();
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}
}

void CCookieMachineDlg::RenderEfn(void)
{
	g_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, TRUE );
	
	switch ( modelStates.RenderMode  )
	{
		case RM_SHADED_WIREFRAME:
			{
				int t = 0;
				if( m_appSettings.LightingEnabled )
				{
					g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
					g_pd3dDevice->LightEnable( 0, TRUE );
					SettingsApplyLight();
				}
				else
				{
					g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
					g_pd3dDevice->LightEnable( 0, FALSE );
					g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_ARGB( 0, 0, 0, 0 ) );
				}

				g_pd3dDevice->SetMaterial( &m_Gmtrl );
				g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );

				/*if( m_bIsWalkModeOn )
				{
					((CEfnCharacter*)pBaseScene)->SetWorldTransformation( m_matGrid );
					((CEfnCharacter*)pBaseScene)->DrawModel( g_pd3dDevice );
				}
				else*/
				RenderEFNCharacterEd( false );


				//g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0 );
				g_pd3dDevice->SetMaterial( &m_Wmtrl );
				g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
				g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00000000 );
				g_pd3dDevice->LightEnable( 0, FALSE );

				/*if( m_bIsWalkModeOn )
				{
					((CEfnCharacter*)pBaseScene)->SetWorldTransformation( m_matGrid );
					((CEfnCharacter*)pBaseScene)->DrawModel( g_pd3dDevice );
				}
				else*/

				D3DXMatrixPerspectiveFovLH( &matProj, D3DXToRadian( m_ViewMgr.m_camCurrent->hfov ), 1.0f / m_fAspect, 0.1002f, 500.0001f );
				g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

				RenderEFNCharacterEd( true );

				D3DXMatrixPerspectiveFovLH( &matProj, D3DXToRadian( m_ViewMgr.m_camCurrent->hfov ), 1.0f / m_fAspect, 0.1f, 500.0f );
				g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
			}
		break;

		case RM_SHADED:
			if( m_appSettings.LightingEnabled )
			{
				g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
				g_pd3dDevice->LightEnable( 0, TRUE );
				SettingsApplyLight();
			}
			else
			{
				g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
				g_pd3dDevice->LightEnable( 0, FALSE );
				g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_ARGB( 0, 0, 0, 0 ) );
			}

			g_pd3dDevice->SetMaterial( &m_Gmtrl );
			g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );



			//if( m_bIsWalkModeOn )
			//{
			//	((CEfnCharacter*)pBaseScene)->SetWorldTransformation( m_matGrid );
			//	((CEfnCharacter*)pBaseScene)->DrawModel( g_pd3dDevice );
			//}
			//else
			RenderEFNCharacterEd( false );
		break;

		case RM_WIREFRAME:
			g_pd3dDevice->SetMaterial( &m_Wmtrl );
			g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
			g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00000000 );
			g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
			g_pd3dDevice->LightEnable( 0, FALSE );

			//if( m_bIsWalkModeOn )
			//{
			//	((CEfnCharacter*)pBaseScene)->SetWorldTransformation( m_matGrid );
			//	((CEfnCharacter*)pBaseScene)->DrawModel( g_pd3dDevice );
			//}
			//else
			RenderEFNCharacterEd( true );
		break;
	}

	g_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
}

void CCookieMachineDlg::RenderXSI(void)
{
	D3DXMATRIX identity;
	D3DXMatrixIdentity( &identity );

	switch ( modelStates.RenderMode  )
	{
		case RM_SHADED_WIREFRAME:
			if( m_appSettings.LightingEnabled )
			{
				g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
				g_pd3dDevice->LightEnable( 0, TRUE );
				SettingsApplyLight();
			}
			else
			{
				g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
				g_pd3dDevice->LightEnable( 0, FALSE );
				g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_ARGB( 0, 0, 0, 0 ) );
			}

			g_pd3dDevice->SetMaterial(&m_Gmtrl);
			g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
			((CXSIFile*)pBaseScene)->Render( g_pd3dDevice, ppMesh, identity );


			g_pd3dDevice->SetMaterial( &m_Wmtrl );
			g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
			g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00000000 );
			g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
			g_pd3dDevice->LightEnable( 0, FALSE );
			((CXSIFile*)pBaseScene)->Render( g_pd3dDevice, ppMesh, identity );


			//draw selected mesh
			g_pd3dDevice->SetMaterial(&m_Wmtrl);
			g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
			g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00FF0000 );
			g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
			g_pd3dDevice->LightEnable( 0, FALSE );
			((CXSIFile*)pBaseScene)->RenderSelection( g_pd3dDevice );

			g_pd3dDevice->SetMaterial(&m_Gmtrl);
			g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
			g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00000000 );
			g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
			g_pd3dDevice->LightEnable( 0, TRUE );
			((CXSIFile*)pBaseScene)->RenderSelection( g_pd3dDevice );
		break;

		case RM_SHADED:

			//draw shaded model
			if( m_appSettings.LightingEnabled )
			{
				g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
				g_pd3dDevice->LightEnable( 0, TRUE );
				SettingsApplyLight();
			}
			else
			{
				g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
				g_pd3dDevice->LightEnable( 0, FALSE );
				g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_ARGB( 0, 0, 0, 0 ) );
			}

			g_pd3dDevice->SetMaterial(&m_Gmtrl);
			g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
			((CXSIFile*)pBaseScene)->Render( g_pd3dDevice, ppMesh, identity  );

			//draw selected mesh
			g_pd3dDevice->SetMaterial(&m_Gmtrl);
			g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
			g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00FF0000 );
			g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
			g_pd3dDevice->LightEnable( 0, TRUE );
			((CXSIFile*)pBaseScene)->RenderSelection( g_pd3dDevice );
		break;

		case RM_WIREFRAME:
			g_pd3dDevice->SetMaterial( &m_Wmtrl );
			g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
			g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00000000 );
			g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
			g_pd3dDevice->LightEnable( 0, FALSE );
			((CXSIFile*)pBaseScene)->Render( g_pd3dDevice, ppMesh, identity );

			//draw selected mesh
			g_pd3dDevice->SetMaterial(&m_Wmtrl);
			g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
			g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00000000 );
			g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
			g_pd3dDevice->LightEnable( 0, FALSE );

			((CXSIFile*)pBaseScene)->RenderSelection( g_pd3dDevice );
		break;
	}
}

void CCookieMachineDlg::SceneTreeItemChangeProcXSI()
{
	((CXSIFile*)pBaseScene)->SetNoMeshesSelected();
	((CXSIFile*)pBaseScene)->SetNoLightsSelected();
	((CXSIFile*)pBaseScene)->SetNoCameraSelected();

	CString strItem		= m_rtSceneContents.GetCurrNodeText();
	CString strParent2	= m_rtSceneContents.GetPar2NodeText();
	bool	bObjWindowUpdated = false;

	switch( m_rtSceneContents.GetCurrNodeType() )
	{
		case TNT_MESH:
			{
				((CXSIFile*)pBaseScene)->SetSelectedMesh( strItem.GetBuffer() );
				pBaseScene->Prepare( g_pd3dDevice );
			}
			break;

		case TNT_LIGHT:
			{
				((CXSIFile*)pBaseScene)->SetSelectedLight( strItem.GetBuffer() );
				pBaseScene->Prepare( g_pd3dDevice );
			}
			break;

		case TNT_CAMERA:
			{
				((CXSIFile*)pBaseScene)->SetSelectedCamera( strItem.GetBuffer() );
				pBaseScene->Prepare( g_pd3dDevice );
			}
			break;

		case TNT_NULL:
			{
				CNULL2*	null = ((CXSIFile*)pBaseScene)->GetNullByName( strItem.GetBuffer() );

				m_ObjPropWnd = OWE_NULL;
				ShowObjectPropertiesWnd( true );

				m_pPropNullDlg->SetNull( null );
				m_pPropNullDlg->Populate( m_sliderAnim.GetPos() );

				bObjWindowUpdated = true;
			}
			break;

		case TNT_CUST_PARAM:
			{
				str_t		cust_par_id	= (strParent2 + "." + strItem).GetBuffer();
				CCUSTPARAM*	cpar		= ((CXSIFile*)pBaseScene)->GetCustParamByName( cust_par_id );

				m_ObjPropWnd = OWE_CUSTPARAM;
				ShowObjectPropertiesWnd( true );
				m_pPropCustomParamDlg->SetCustomParam( cpar );
				m_pPropCustomParamDlg->Populate( m_sliderAnim.GetPos() );

				bObjWindowUpdated = true;
			}
			break;
	}

	if( !bObjWindowUpdated )
	{
		m_ObjPropWnd = OWE_UNDEFINED;
		ShowObjectPropertiesWnd( false );
	}
}

void CCookieMachineDlg::SceneTreeItemChangeProcMSH()
{
	bool	ObjWndSet	= false;
	CString strItem		= m_rtSceneContents.GetCurrNodeText();
	CString strParent2	= m_rtSceneContents.GetPar2NodeText();
	bool		bObjWindowUpdated = false;
	HTREEITEM	hItem	= m_rtSceneContents.GetSelectedItem();
	HTREEITEM	hParent	= m_rtSceneContents.GetParentItem( hItem );
	
	((CEfnMeshlist*)pBaseScene)->DeselectAllMeshes();
	((CEfnMeshlist*)pBaseScene)->DeselectAllNulls();

	switch( m_rtSceneContents.GetCurrNodeType() )
	{
		case TNT_MESH:
			{
				CMesh* pMesh = ((CEfnMeshlist*)pBaseScene)->GetMeshByName( strItem.GetBuffer() );

				if( pMesh )
				{
					((CEfnMeshlist*)pBaseScene)->DeselectAllMeshes();
					((CEfnMeshlist*)pBaseScene)->DeselectAllNulls();

					pMesh->SetSelected( true );
					((CEfnMeshlist*)pBaseScene)->SetParentMesh( strItem.GetBuffer() );
					pBaseScene->Prepare( g_pd3dDevice );
					
					ObjWndSet = true;

					HTREEITEM hTemp = m_rtSceneContents.GetChildItem( hParent );

					while( hTemp )
					{
						if( hTemp != hItem )
							m_rtSceneContents.SetItemState( hTemp, 0, TVIS_SELECTED );	

						hTemp = m_rtSceneContents.GetNextItem( hTemp, TVGN_NEXT );
					}

					m_rtSceneContents.SelectItem( hItem );
				}
			}
			break;


		case TNT_NULL:
			{
				((CEfnMeshlist*)pBaseScene)->SetSelectedNull( "Nulls", strItem.GetBuffer() );
				
				CNULL* pNull = ((CEfnMeshlist*)pBaseScene)->GetMeshByName( "Nulls" )->GetNullByName( strItem.GetBuffer() );

				pSelectedEfnAnimNull = pNull;

				m_ObjPropWnd = OWE_NULL;
				ShowObjectPropertiesWnd( true );

				m_pPropNullDlg->SetNull( pNull );
				m_pPropNullDlg->Populate( 0 );

				bObjWindowUpdated = true;
			}
			break;
	}

	if( !bObjWindowUpdated )
	{
		m_ObjPropWnd = OWE_UNDEFINED;
		ShowObjectPropertiesWnd( false );
	}

	UpdateStatusBarInfoMESHLIST();
}

void CCookieMachineDlg::SceneTreeItemChangeProcEFN()
{
	CString strItem				= m_rtSceneContents.GetCurrNodeText();
	CString strParent2			= m_rtSceneContents.GetPar2NodeText();
	bool	bObjWindowUpdated	= false;

	((CEfnCharacter*)pBaseScene)->DeselectedAllNulls();

	m_fGlobRadScale			= 1.0f;
	pSelectedEfnAnimNull	= NULL;
	m_bIsGRadEditorModeOn	= false;

	TNODE* data = (TNODE*)m_rtSceneContents.GetCurrNodeData();

	if( !data )
		return;

	switch( data->_type )
	{
		case TREENODE::N_NULL:
			{
				((CEfnCharacter*)pBaseScene)->SetSelectedNull( strItem.GetBuffer() );

				m_ObjPropWnd = OWE_NULL;
				ShowObjectPropertiesWnd( true );

				m_pPropNullDlg->SetNull( ((CEfnCharacter*)pBaseScene)->GetNullByName( strItem.GetBuffer() ) );
				m_pPropNullDlg->Populate( m_sliderAnim.GetPos(), m_iStartingFrame, m_iEndingFrame );

				bObjWindowUpdated = true;
			}
			break;

		case TREENODE::N_CUST_PARAM:
			{
				str_t		cust_par_id	= (strParent2 + "." + strItem).GetBuffer();
				CCUSTPARAM*	cpar		= ((CEfnCharacter*)pBaseScene)->GetCustParamByName( cust_par_id );

				m_ObjPropWnd = OWE_CUSTPARAM;
				ShowObjectPropertiesWnd( true );
				m_pPropCustomParamDlg->SetCustomParam( cpar );
				m_pPropCustomParamDlg->Populate( m_sliderAnim.GetPos() );

				bObjWindowUpdated = true;
			}
			break;

		case TREENODE::N_GLOB_RADIUS:
			{
				m_bIsGRadEditorModeOn = true;
			}
			break;
	}

	if( !bObjWindowUpdated )
	{
		m_ObjPropWnd = OWE_UNDEFINED;
		ShowObjectPropertiesWnd( false );
	}

	UpdateStatusBarInfoEFN();
}

void CCookieMachineDlg::SceneTreeItemChangeProcSCN()
{
	CSceneFile*		pScene		= (CSceneFile*)pBaseScene;
	CEfnMeshlist*	pMeshlist	= (CEfnMeshlist*)pScene->GetMeshlist();
	bool			ObjWndSet	= false;
	bool			bObjWindowUpdated = false;
	str_t			strItem		= m_rtSceneContents.GetCurrNodeText().GetBuffer();

	pMeshlist->DeselectAllMeshes();
	pScene->DeselectAllCurves();
	pScene->SetNoLightSelected();
	pScene->SetNoCameraSelected();

	switch( m_rtSceneContents.GetCurrNodeType() )
	{
		case TNT_MESH:
			{
				pMeshlist->SelectMesh( strItem );
			}
			break;

		case TNT_NULL:
			{
				pMeshlist->DeselectAllNulls();

				pMeshlist->SetSelectedNull( "Nulls", strItem );

				CNULL* pNull = pMeshlist->GetMeshByName( "Nulls" )->GetNullByName( strItem );

				pSelectedEfnAnimNull = pNull;

				m_ObjPropWnd = OWE_NULL;
				ShowObjectPropertiesWnd( true );

				m_pPropNullDlg->SetNull( pNull );
				m_pPropNullDlg->Populate();

				bObjWindowUpdated = true;
			}
			break;

		case TNT_SCN_CURVE:
			{
				pScene->GetCurveByName( strItem )->selected = true;
			}
			break;

		case TNT_SCN_CURVE_NULL:
			{
				pMeshlist->DeselectAllNulls();

				pMeshlist->SetSelectedNull( "Nulls", strItem );
				CNULL* pNull = pMeshlist->GetMeshByName( "Nulls" )->GetNullByName( strItem );

				pSelectedEfnAnimNull	= pNull;
				m_ObjPropWnd			= OWE_NULL;

				ShowObjectPropertiesWnd( true );

				m_pPropNullDlg->SetNull( pNull );
				m_pPropNullDlg->Populate();

				bObjWindowUpdated = true;
			}
			break;

		case TNT_LIGHT:
			{
				pScene->SetSelectedLight( strItem );

				CLIGHT* pLight = pScene->GetLightByID( strItem );

				m_ObjPropWnd = OWE_LIGHT;
				ShowObjectPropertiesWnd( true );

				m_pPropLightDlg->SetLight( pLight );
				m_pPropLightDlg->Populate();

				bObjWindowUpdated = true;
			}
			break;

		case TNT_CAMERA:
			{
				pScene->SetSelectedCamera( strItem );

				CCAMERA2* pCamera = pScene->GetCameraByID( strItem );
				
				if( pCamera )
				{
					m_ObjPropWnd = OWE_CAMERA;
					ShowObjectPropertiesWnd( true );

					m_pPropCameraDlg->SetCamera( pCamera );
					m_pPropCameraDlg->Populate();

					CString strTemp = "Quest:SetCam( \"Camera_";
					strTemp += pCamera->name.data(); 
					strTemp += "\" );\r\n";
					
					g_MemMappingsMgr->PostScriptToEngine( strTemp, true );

					bObjWindowUpdated = true;
				}
			}
			break;

		case TNT_SCN_CAMERA_BIND:
			{
				pScene->SetSelectedCamera( strItem );

				switch( m_rtSceneContents.GetCurrNodeImage() )
				{
					case 9:
						pScene->SetSelectedLight( strItem );
						break;

					case 14:
						pMeshlist->SelectMesh( strItem );

					case 17:
						pMeshlist->SelectMesh( strItem );

					case 18:
						pMeshlist->SelectMesh( strItem );

					case 19:
						pMeshlist->SelectMesh( strItem );
						break;
				}
			}
			break;
	}

	if( !bObjWindowUpdated )
	{
		m_ObjPropWnd = OWE_UNDEFINED;
		ShowObjectPropertiesWnd( false );
	}

	UpdateStatusBarInfoSCN();
}

void CCookieMachineDlg::RenameCurrTreeviewItemGlob( str_t strNewID )
{
	if( !pBaseScene )
		return;

	TNODE* data = (TNODE*)m_rtSceneContents.GetCurrNodeData();

	switch( pBaseScene->GetType() )
	{
		case FT_EFN:
			{

				if( !data || data->_type == TREENODE::N_UNKNOWN )
					return;

				switch( data->_type )
				{
					case TREENODE::N_NULL:
						OnMenuEfnNullRename( m_rtSceneContents.GetCurrNodeText(), strNewID.data() );
						break;

					case TREENODE::N_MODEL_NAME:
						OnMenuEfnEditModelName( strNewID.data() );
						break;

					case TREENODE::N_ANIMATION:
						OnMenuEfnAnimationRename( m_rtSceneContents.GetCurrNodeText(), strNewID.data() );
						break;

					case TREENODE::N_CUST_PARAM:
						OnMenuEfnEditCustParamName( m_rtSceneContents.GetCurrNodeText(), strNewID.data() );
						break;

					case TREENODE::N_FPS:
						OnMenuEfnEditFPS( m_rtSceneContents.GetCurrNodeText(), strNewID.data() );
						break;

					case TREENODE::N_GLOB_RADIUS:
						OnMenuEfnEditGlobRadius( m_rtSceneContents.GetCurrNodeText(), strNewID.data() );
						break;
				}

			}
			break;


			case FT_EFN_MESHLIST:
			{
				if( m_rtSceneContents.GetCurrNodeType() == TNT_NULL )
				{
					OnMenuMeshlistNullRename( m_rtSceneContents.GetCurrNodeText(), strNewID.data() );
				}
				else if( m_rtSceneContents.GetCurrNodeType() == TNT_MESH )
				{
					OnMenuMeshlistMeshRename( m_rtSceneContents.GetCurrNodeText(), strNewID.data() );
				}
			}
			break;

			case FT_SCN:
			{
				if( m_rtSceneContents.GetCurrNodeType() == TNT_CAMERA )
				{
					OnMenuScnCameraRename( m_rtSceneContents.GetCurrNodeText(), strNewID.data() );
				}
				else if( m_rtSceneContents.GetCurrNodeType() == TNT_LIGHT )
				{
					OnMenuScnLightRename( m_rtSceneContents.GetCurrNodeText(), strNewID.data() );
				}
				else if( m_rtSceneContents.GetCurrNodeType() == TNT_MESH )
				{
					OnMenuScnMeshRename( m_rtSceneContents.GetCurrNodeText(), strNewID.data() );
				}
				else if( m_rtSceneContents.GetCurrNodeType() == TNT_NULL )
				{
					OnMenuScnNullRename( m_rtSceneContents.GetCurrNodeText(), strNewID.data() );
				}
				else if( m_rtSceneContents.GetCurrNodeType() == TNT_TEXTURE )
				{
					OnMenuScnTextureRename( m_rtSceneContents.GetCurrNodeText(), strNewID.data() );
				}
				else if( m_rtSceneContents.GetCurrNodeType() == TNT_SCN_CURVE )
				{
					OnMenuScnCurveRename( m_rtSceneContents.GetCurrNodeText(), strNewID.data() );
				}
				else if( m_rtSceneContents.GetCurrNodeType() == TNT_SCN_CUST_OBJECT )
				{
					OnMenuScnCustObjRename( m_rtSceneContents.GetCurrNodeText(), strNewID.data() );
				}
				else if( m_rtSceneContents.GetCurrNodeType() == TNT_SCN_CUST_TYPE )
				{
					OnMenuScnCustTypeRename( m_rtSceneContents.GetCurrNodeText(), strNewID.data() );
				}
				else if( m_rtSceneContents.GetCurrNodeType() == TNT_SCN_LEVEL )
				{
					OnMenuScnEditLevelName( strNewID.data() );
				}
			}
			break;
	}

	if( m_pObjPropWnd )
		m_pObjPropWnd->Populate();
}

void CCookieMachineDlg::OnMenuScnEditLevelName( str_t strNewID )
{
	if( !pBaseScene || pBaseScene->GetType() != FT_SCN )
		return;

	((CSceneFile*)pBaseScene)->SetLevelID( strNewID );
	m_rtSceneContents.SetItemText( m_rtSceneContents.GetSelectedItem(), ("Level ID: " + strNewID).data() );
}

void CCookieMachineDlg::OnMenuEfnAnimationRename( CString strCurrID, CString strNewID )
{
	if( !pBaseScene || pBaseScene->GetType() != FT_EFN )
		return;

	str_t			_strCurrID	= strCurrID.GetBuffer();
	str_t			_strNewID	= strNewID.GetBuffer();
	CEfnCharacter*	pEfnAnim	= (CEfnCharacter*)pBaseScene;

	if( _strCurrID == _strNewID )
		return;

	str_t _strCurrID2	= _strCurrID;
	str_t _strNewID2	= _strNewID;

	_2Upper( _strCurrID2 );
	_2Upper( _strNewID2 );

	if( _strCurrID2 != _strNewID2 && pEfnAnim->GetAnimationByName( _strNewID ) )
	{
		MessageBox( "Анимация с таким ID уже существует!", "Внимание!", MB_OK | MB_ICONINFORMATION );
		return;
	}

	pEfnAnim->GetAnimationByName( _strCurrID )->name =_strNewID; 
	m_rtSceneContents.SetItemText( m_rtSceneContents.GetSelectedItem(), strNewID );

	UpdateStatusBarInfoEFN();
}

void CCookieMachineDlg::OnMenuEfnEditCustParamName( CString strCurrID, CString strNewID )
{
	str_t _strCurrID	= strCurrID.GetBuffer();
	str_t _strNewID		= strNewID.GetBuffer();

	if( _strCurrID == _strNewID )
		return;

	HTREEITEM hNull = m_rtSceneContents.GetSelectedItem();
	hNull = m_rtSceneContents.GetParentItem( hNull );
	hNull = m_rtSceneContents.GetParentItem( hNull );

	CString strNullID = m_rtSceneContents.GetItemText( hNull );

	CEfnCharacter*	efnChar	= (CEfnCharacter*)pBaseScene;

	if( efnChar->GetCustParamByName( (strNullID + "." + strNewID).GetBuffer() ) )
	{
		MessageBox( "Custom parameter с таким ID уже существует!", "Внимание!", MB_OK | MB_ICONINFORMATION );
		return;
	}

	CCUSTPARAM* par = efnChar->GetCustParamByName( (strNullID + "." + strCurrID).GetBuffer() );

	if( !par )
		return;

	par->id = (strNullID + "." + strNewID).GetBuffer();
	
    m_rtSceneContents.SetCurrNodeText( strNewID );

	UpdateStatusBarInfoEFN();
}

void CCookieMachineDlg::OnMenuEfnEditFPS( CString strCurrID, CString strNewID )
{
	str_t _strCurrID	= strCurrID.GetBuffer();
	str_t _strNewID		= strNewID.GetBuffer();

	if( atof( _strCurrID.data() ) == atof( _strNewID.data() ) || atof( _strNewID.data() ) == 0 )
		return;

	CEfnCharacter* efnChar = (CEfnCharacter*)pBaseScene;
	efnChar->SetFPS( atof( _strNewID.data() ) );

	HTREEITEM hFPS = m_rtSceneContents.GetSelectedItem();
	strNewID.Format( "FPS: %.2f", atof( _strNewID.data() ) );
	m_rtSceneContents.SetCurrNodeText( strNewID );
}

void CCookieMachineDlg::OnMenuEfnEditGlobRadius( CString strCurrID, CString strNewID )
{
	str_t _strCurrID	= strCurrID.GetBuffer();
	str_t _strNewID		= strNewID.GetBuffer();

	if( atof( _strCurrID.data() ) == atof( _strNewID.data() ) || atof( _strNewID.data() ) == 0 )
		return;

	CEfnCharacter* efnChar = (CEfnCharacter*)pBaseScene;
	efnChar->SetGlobalRadius( atof( _strNewID.data() ) );

	HTREEITEM hFPS = m_rtSceneContents.GetSelectedItem();
	strNewID.Format( "Global radius: %.4f", atof( _strNewID.data() ) );
	m_rtSceneContents.SetCurrNodeText( strNewID );
}

void CCookieMachineDlg::SceneTreeItemChangeProc()
{
	str_t func_decl_str = "void CCookieMachineDlg::SceneTreeItemChangeProc()";

	try
	{
		if( !pBaseScene )
			return;

		if( pBaseScene->GetType() == FT_XSI )
		{	
			SceneTreeItemChangeProcXSI();
		}
		else if( pBaseScene->GetType() == FT_EFN_MESHLIST )
		{
			SceneTreeItemChangeProcMSH();
		}
		else if( pBaseScene->GetType() == FT_EFN )
		{
			SceneTreeItemChangeProcEFN();
		}
		else if( pBaseScene->GetType() == FT_SCN )
		{
			SceneTreeItemChangeProcSCN();
		}
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}
}

void CCookieMachineDlg::OnTvnSelchangedSceneTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	if( !pBaseScene )
		return;

	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	//SceneTreeClickProc();

	*pResult = 0;
}

void CCookieMachineDlg::OnMenuMeshInverseNormals()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_XSI )
		return;

	CMenu menuPopup;
	HTREEITEM hSelected, hParent;
	CString strParentText;
	CString hSelItemText;

	hSelected		= m_rtSceneContents.GetSelectedItem();
	hSelItemText	= m_rtSceneContents.GetItemText(hSelected);
	
	if (hSelected >= 0)
		hParent = m_rtSceneContents.GetParentItem(hSelected);

	if (hParent >= 0)
		strParentText = m_rtSceneContents.GetItemText(hParent);

	if (strParentText == "Meshes")
	{
		((CXSIFile*)pBaseScene)->InverseMeshNormals( hSelItemText.GetBuffer() );
		pBaseScene->Prepare( g_pd3dDevice );
	}
}

void CCookieMachineDlg::OnMenuSetCameraTocurrent()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_XSI && pBaseScene->GetType() != FT_SCN )
		return;

	if( !m_bIsPerspectiveCameraOn )
	{
		//m_ViewMgr.m_camCurrent	= m_camOld;
		//worldStates		= worldStatesOldCam;
	}

	m_bIsPerspectiveCameraOn = false;

	CCAMERA* cam;
	CCAMERA _cam;

	if( pBaseScene->GetType() == FT_XSI )
		cam =((CXSIFile*)pBaseScene)->GetSelectedCamera();

	if( pBaseScene->GetType() == FT_SCN )
	{
		CCAMERA2* cam2 = ((CSceneFile*)pBaseScene)->GetSelectedCamera();
		cam2->visible = false;
		((CSceneFile*)pBaseScene)->PrepareCamsLines( g_pd3dDevice );

		_cam.name		= cam2->name;
		_cam.pos		= cam2->pos;
		_cam.dir		= cam2->targ;	
		_cam.hfov		= cam2->fov.x;
		_cam.vfov		= cam2->fov.y;		
		_cam.upvector	= cam2->upvec;		
		_cam.visible		= cam2->visible;
	}

	//m_camOld = *m_ViewMgr.m_camCurrent;

	if( pBaseScene->GetType() == FT_XSI )
	{
		*m_ViewMgr.m_camCurrent = *cam;
		
		//find cam in combobox and set it active
		int idx = m_cmbCameras.FindString( 0, cam->name.data() );
		m_cmbCameras.SetCurSel( idx );
	}

	if( pBaseScene->GetType() == FT_SCN )
	{
		*m_ViewMgr.m_camCurrent = _cam;

		//find cam in combobox and set it active
		int idx = m_cmbCameras.FindString( 0, _cam.name.data() );
		m_cmbCameras.SetCurSel( idx );
	}

	//worldStatesOldCam = worldStates;
}

void CCookieMachineDlg::OnMenuReturnToPerspectiveview()
{
	//in case user didn't switch into default camera before closing file
	if( pBaseScene->GetType() == FT_SCN )
	{
		CCAMERA2* cam = ((CSceneFile*)pBaseScene)->GetSelectedCamera();

		if( cam )
			cam->visible = true;

		((CSceneFile*)pBaseScene)->SetNoCameraSelected();
		((CSceneFile*)pBaseScene)->PrepareCamsLines( g_pd3dDevice );
	}

	if( !m_bIsPerspectiveCameraOn )
	{
		m_bIsPerspectiveCameraOn = true;

		//*m_ViewMgr.m_camCurrent	= m_camOld;
		//worldStates		= worldStatesOldCam;
		m_cmbCameras.SetCurSel( 0 );
	}
}

void CCookieMachineDlg::OnMenuConvToScn()
{
	CValidateDlg*	dlgValidate		= NULL;
	CModelNameDlg*	dlgSceneName	= NULL;
	CMenu*				menu				= this->GetMenu();
	CSceneFile*		scn_file				= NULL;

	BROWSEINFO bi;
	ZeroMemory( &bi, sizeof( BROWSEINFO ) );
	bi.hwndOwner	= this->GetSafeHwnd();
	bi.lpszTitle	= "Укажите папку для сцены:";
	bi.ulFlags		= BIF_RETURNONLYFSDIRS;

	LPITEMIDLIST pIDL = SHBrowseForFolder( &bi );

	TCHAR szPath[MAX_PATH];
	SHGetPathFromIDList( pIDL, szPath );

	CString scene_destination = szPath;

	//make sure user chose valid directory
	if( scene_destination == "" )
	{
		AfxMessageBox( "Invalid directory" );
		return;
	}

	scene_destination += "\\";

	// make sure current file type is XSI
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_XSI)
		return;

	// show validation dialog
	if( !dlgValidate )
	{
		dlgValidate = new CValidateDlg(this);
		dlgValidate->SetMessageStr("Конвертация...");
	}

	CEfnMeshlist*	meshlist	= new CEfnMeshlist;
	str_t					error;
	
	// show error message (duplicate meshes, if any)
	if( CheckForDuplicateMeshes( pBaseScene, error ) )
	{
		if( MessageBox( error.data(), "Куджо, внимание!", MB_YESNO ) == IDNO )
			return;

		int m_cnt = ((CXSIFile*)pBaseScene )->GetMeshesCount();

		for( int i = 0; i < m_cnt; i++ )
		{
			CString tmp;
			str_t		base_name  = ((CXSIFile*)pBaseScene )->GetMeshByIndex( i )->GetName();
			str_t		check_name;

			for( int j = 0; j < m_cnt; j++ )
			{
				check_name = ((CXSIFile*)pBaseScene )->GetMeshByIndex( j )->GetName();

				if( i != j && base_name == check_name )
				{
					tmp.Format( "%s__%i", base_name.data(), i );
					((CXSIFile*)pBaseScene )->GetMeshByIndex( i )->SetName( tmp.GetBuffer() );
				}
			}
		}
	}

	//ask for scene name
	//get new scene ID from user
	dlgSceneName = new CModelNameDlg( this );
	dlgSceneName->SetCaption("Укажите имя сцены: ");
	dlgSceneName->DoModal();

	CString scene_name =  dlgSceneName->GetString();

	SAFE_DELETE( dlgSceneName );

	// convert
	if( !meshlist->InitFromXSICharacter( (CXSIFile*)pBaseScene ) )
	{
		SAFE_DELETE( meshlist );
		MessageBox( "Meshlist creation error!", "Conversion error!", MB_OK | MB_ICONERROR );
		return;
	}

	CString geom_filename = scene_name + ".geom.efn";
	CString scene_filename = scene_name + ".scn";

	meshlist->SetPaths( scene_destination.GetBuffer(), geom_filename.GetBuffer() );
	meshlist->SetSceneTag( scene_name.GetBuffer() );

	//delete existing meshlist
	DeleteFile( (scene_destination + geom_filename).GetBuffer() );

	meshlist->SaveToFile( (scene_destination + geom_filename).GetBuffer() );
	SAFE_DELETE( meshlist );

	// create new scene
	scn_file = new CSceneFile;

	scn_file->SetPaths( scene_destination.GetBuffer(), scene_filename.GetBuffer() );
	//scn_file->SetSceneID( scene_name.GetBuffer() );

	// init efn from xsi character
	if( !scn_file->InitFromXSICharacter( (CXSIFile*)pBaseScene ) )
	{
		SAFE_DELETE( scn_file );
		MessageBox("Ошибка конвертации!", "Внимание!", MB_OK | MB_ICONERROR);
		SAFE_DELETE( dlgValidate );
		return;
	}

	CString app_caption;
	app_caption.Format( "Cookie Machine: %s%s", scene_destination.GetBuffer(), scene_filename.GetBuffer() );

	this->SetWindowText( app_caption );

	((CEfnMeshlist*)scn_file->GetMeshlist())->SetParent( this );

	scn_file->Prepare( g_pd3dDevice );

	scn_file->SetCamBox( pCamBox );
	scn_file->SetLightBulbCone( pLightBulb, pLightCone );
	scn_file->SetParent( this );

	SAFE_DELETE( pBaseScene );
	pBaseScene = scn_file;
	
	((CSceneFile*)pBaseScene)->SaveToFile( (scene_destination + scene_filename).GetBuffer() );

	UpdateUI();

	EnableControls( true );

	//delete validation window
	SAFE_DELETE( dlgValidate );
}

void CCookieMachineDlg::OnMenuShowAabb()
{
	if( !pBaseScene || m_bIsVertexEditingModeOn )
		return;
	
	CMenu* menu = GetMenu();
	
	switch( pBaseScene->GetType() )
	{
		case FT_EFN:
			{
				if( ((CEfnCharacter*)pBaseScene)->IsAABBVisible() )
				{
					((CEfnCharacter*)pBaseScene)->ShowAABB( false );
					menu->CheckMenuItem(ID_MENU_SHOW_AABB, MF_UNCHECKED); 
				}
				else
				{
					((CEfnCharacter*)pBaseScene)->ShowAABB( true );
					menu->CheckMenuItem(ID_MENU_SHOW_AABB, MF_CHECKED); 
				}
			}
			break;
	
		case FT_EFN_MESHLIST:
			{
				if( ((CEfnMeshlist*)pBaseScene)->GetOOBBVisible() )
				{
					((CEfnMeshlist*)pBaseScene)->ShowOOBBs( false );
					menu->CheckMenuItem( ID_MENU_SHOW_AABB, MF_UNCHECKED ); 
				}
				else
				{
					((CEfnMeshlist*)pBaseScene)->ShowOOBBs( true );
					menu->CheckMenuItem( ID_MENU_SHOW_AABB, MF_CHECKED ); 
				}
			}
			break;

		case FT_SCN:
			{
				CSceneFile*		scene		= ((CSceneFile*)pBaseScene);
				CEfnMeshlist*	meshlist	= (CEfnMeshlist*)scene->GetMeshlist();

				if( meshlist->GetOOBBVisible() )
				{
					meshlist->ShowOOBBs( false );
					menu->CheckMenuItem( ID_MENU_SHOW_AABB, MF_UNCHECKED ); 
				}
				else
				{
					meshlist->ShowOOBBs( true );
					menu->CheckMenuItem( ID_MENU_SHOW_AABB, MF_CHECKED ); 
				}
			}
			break;
	}
}

void CCookieMachineDlg::OnMenuShowBsphere()
{
	if( !pBaseScene || m_bIsVertexEditingModeOn )
		return;

	CMenu* menu = GetMenu();

	if( pBaseScene->GetType() == FT_EFN )
	{
		if( ((CEfnCharacter*)pBaseScene)->IsBoundingSphereVisible() )
		{
			((CEfnCharacter*)pBaseScene)->ShowBoundingSphere( false );
			menu->CheckMenuItem(ID_MENU_SHOW_BSPHERE, MF_UNCHECKED); 
		}
		else
		{
			((CEfnCharacter*)pBaseScene)->ShowBoundingSphere( true );
			menu->CheckMenuItem(ID_MENU_SHOW_BSPHERE, MF_CHECKED); 
		}
	}


	if( pBaseScene->GetType() == FT_EFN_MESHLIST )
	{
		if( ((CEfnMeshlist*)pBaseScene)->GetBSpheresVisible() )
		{
			((CEfnMeshlist*)pBaseScene)->ShowBSpheres( false );
			menu->CheckMenuItem( ID_MENU_SHOW_BSPHERE, MF_UNCHECKED ); 
		}
		else
		{
			((CEfnMeshlist*)pBaseScene)->ShowBSpheres( true );
			menu->CheckMenuItem( ID_MENU_SHOW_BSPHERE, MF_CHECKED ); 
		}	
	}

	if( pBaseScene->GetType() == FT_SCN )
	{
		CSceneFile*		scene		= ((CSceneFile*)pBaseScene);
		CEfnMeshlist*	meshlist	= (CEfnMeshlist*)scene->GetMeshlist();

		if( meshlist->GetBSpheresVisible() )
		{
			meshlist->ShowBSpheres( false );
			menu->CheckMenuItem( ID_MENU_SHOW_BSPHERE, MF_UNCHECKED ); 
		}
		else
		{
			meshlist->ShowBSpheres( true );
			menu->CheckMenuItem( ID_MENU_SHOW_BSPHERE, MF_CHECKED ); 
		}
	}
}

HBRUSH CCookieMachineDlg::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;

	str_t func_decl_str = "HBRUSH CCookieMachineDlg::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )";

	try
	{
		HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

		if( pWnd->GetDlgCtrlID() != IDD_MAIN )
		{
			pDC->SetTextColor( RGB(0, 0, 0) );
			pDC->SetBkMode( TRANSPARENT );

			return m_brush;
		}
		else
			return hbr;
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}
}

void CCookieMachineDlg::OnMenuRecalcGlobalRadius()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN )
		return;

	( (CEfnCharacter*)pBaseScene )->RecalcGlobalRadius();

	UpdateUI();
}

void CCookieMachineDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	str_t func_decl_str = "void CCookieMachineDlg::OnShowWindow(BOOL bShow, UINT nStatus)";

	try
	{
		CDialog::OnShowWindow(bShow, nStatus);

		//ShowWindow( SW_SHOWMAXIMIZED );
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}
}

bool CCookieMachineDlg::CheckForDuplicateMeshes( CBaseScene* scene, str_t& strErrorMessage )
{
	CStringList		dupes, dupes_res;

	//check for duplicate meshes
	//create list of strings of dupes
	for( int i = 0; i < ((CXSIFile*)scene)->GetMeshesCount(); i++ )
	{
		str_t base_name = ((CXSIFile*)scene)->GetMeshByIndex( i )->GetName();
		str_t count_name;

		for( int j = 0; j < ((CXSIFile*)scene)->GetMeshesCount(); j++ )
		{
			if( i != j )
			{
				count_name = ((CXSIFile*)scene)->GetMeshByIndex( j )->GetName();

				if( base_name == count_name )
					dupes.AddTail( base_name.data() );
			}
		}
	}

	//remove duplicate entries in list of strings
	POSITION pos			= dupes.GetHeadPosition();
	POSITION pos_res		= dupes_res.GetHeadPosition();

	CString res_dupes = "Meshes: \n\r";
	
	bool found = false;

	for( int i = 0; i < dupes.GetCount(); i++ )
	{
		CString a = dupes.GetNext( pos );

		for( int j = 0; j < dupes_res.GetCount(); j++ )
		{
			if( a == dupes_res.GetNext( pos_res )  )
				found = true;
		}

		if( !found )
			dupes_res.AddTail( a );

		pos_res	= dupes_res.GetHeadPosition();
		found		= false;
	}

	pos_res	= dupes_res.GetHeadPosition();
	
	// create error message
	for( int i = 0; i < dupes_res.GetCount(); i++ )
	{
		CString temp2;
		temp2.Format( "%i - %s%s", i + 1, dupes_res.GetNext( pos_res ), "\n\r" );
		res_dupes += temp2;
	}

	if( dupes_res.GetCount() > 0 )
	{
		strErrorMessage = "Найдены дупликаты!\n\r\n\r";
		strErrorMessage += res_dupes.GetBuffer();
		strErrorMessage += "\n\r\n\r\n\rПродолжить конвертацию? (дупликаты будут переименованы)";

		return true;
	}

	return false;
}

void CCookieMachineDlg::OnMenuConvToEfnMeshlist()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_XSI )
		return;

	CMenu*			menu		= this->GetMenu();
	CEfnMeshlist*	meshlist	= new CEfnMeshlist;
	str_t			error;
	
	// show error message (duplicate meshes, if any)
	if( CheckForDuplicateMeshes( pBaseScene, error ) )
	{
		if( MessageBox( error.data(), "Куджо, внимание!", MB_YESNO ) == IDNO )
			return;

		int m_cnt = ( (CXSIFile*)pBaseScene )->GetMeshesCount();

		for( int i = 0; i < m_cnt; i++ )
		{
			CString tmp;
			str_t	base_name  = ((CXSIFile*)pBaseScene )->GetMeshByIndex( i )->GetName();
			str_t	check_name;

			for( int j = 0; j < m_cnt; j++ )
			{
				check_name = ((CXSIFile*)pBaseScene )->GetMeshByIndex( j )->GetName();

				if( i != j && base_name == check_name )
				{
					tmp.Format( "%s__%i", base_name.data(), i );
					((CXSIFile*)pBaseScene )->GetMeshByIndex( i )->SetName( tmp.GetBuffer() );
				}
			}
		}
	}


	// convert
	if( !meshlist->InitFromXSICharacter( (CXSIFile*)pBaseScene ) )
	{
		SAFE_DELETE( meshlist );
	}
	else
	{
		SAFE_DELETE( pBaseScene );
		meshlist->SetParent( this );
		pBaseScene = meshlist;
	}

	if( m_appSettings.ScnAlternateNullIco )
		((CEfnMeshlist*)pBaseScene)->SetNullIcon( pNullIco );

	pBaseScene->Prepare( g_pd3dDevice );
	pBaseScene->SetPaths( "", "" );

	this->SetWindowText( "Cookie Machine -- не указано" );

	EnableControls( true );

	EnableMenuItem( menu, ID_MENU_SAVE, false );
	EnableToolbarButton( IDT_FILESAVE, false );

	UpdateUI();
}

int* CCookieMachineDlg::IntersectCellWithMesh( CEfnCharacter* mesh, int x, int y, float grid_min_size_x, float grid_min_size_z, int& tri_count, float fCellSize )
{
	std::vector <int> tris;
	float cell_size = fCellSize;

	tris.clear();

	//cell's corners
	D3DXVECTOR3	cell_verts1, cell_verts2, cell_verts3, cell_verts4, cell_verts5, cell_verts6, cell_verts7, cell_verts8;							
	bool					vert_ok[4];		//intersection flag

	for( int i = 0; i < 4; i++ )
		vert_ok[i] = false;

	//prepare cell vertecies
	//A
	cell_verts1.x = grid_min_size_x + cell_size * x;
	cell_verts1.y = 0.0f;
	cell_verts1.z = grid_min_size_z + cell_size * y;

	cell_verts2.x = 0.0f;
	cell_verts2.y = 1.0f;
	cell_verts2.z = 0.0f;

	//D3DXVec3Normalize( &cell_verts2, &cell_verts2 );

	
	//B
	cell_verts3.x = grid_min_size_x + (cell_size * x + cell_size);
	cell_verts3.y = 0.0f;
	cell_verts3.z = grid_min_size_z + (cell_size * y);

	cell_verts4.x = 0.0f;
	cell_verts4.y = 1.0f;
	cell_verts4.z = 0.0f;

	//D3DXVec3Normalize( &cell_verts4, &cell_verts4 );

	
	//C
	cell_verts5.x = grid_min_size_x + (cell_size * x);
	cell_verts5.y = 0.0f;
	cell_verts5.z = grid_min_size_z + (cell_size * y + cell_size);
	
	cell_verts6.x = 0.0f;
	cell_verts6.y = 1.0f;
	cell_verts6.z = 0.0f;

	//D3DXVec3Normalize( &cell_verts6, &cell_verts6 );

	//D
	cell_verts7.x = grid_min_size_x + (cell_size * x + cell_size);
	cell_verts7.y = 0.0f;
	cell_verts7.z = grid_min_size_z + (cell_size * y + cell_size);

	cell_verts8.x = 0.0f;
	cell_verts8.y = 1.0f;
	cell_verts8.z = 0.0f;

	//D3DXVec3Normalize( &cell_verts8, &cell_verts8 );

	/////////////////////////////////////
	// DO INTERSECTIONS //
	/////////////////////////////////////

	//intersect vertex A with mesh, collecting tri indecies;
	for( int i = 0; i < mesh->GetTrianglesCount(); i++ )
	{
		CVERTEX_XYZ tA = mesh->GetVerteciesList()[ mesh->GetIndicesList()[ i * 3 + 0 ] ];
		CVERTEX_XYZ tB = mesh->GetVerteciesList()[ mesh->GetIndicesList()[ i * 3 + 1 ] ];
		CVERTEX_XYZ tC = mesh->GetVerteciesList()[ mesh->GetIndicesList()[ i * 3 + 2 ] ];

		D3DXVECTOR3 A( tA.x, tA.y, tA.z );
		D3DXVECTOR3 B( tB.x, tB.y, tB.z );
		D3DXVECTOR3 C( tC.x, tC.y, tC.z );

		if( CalcRayTriangleIntersection( &A, &B, &C, &cell_verts1, &cell_verts2 ) != -1.0f )
		{
			tris.push_back( i );
			vert_ok[0] = true;
		}

		if( CalcRayTriangleIntersection( &A, &B, &C, &cell_verts3, &cell_verts4 ) != -1.0f )
		{
			tris.push_back( i );
			vert_ok[1] = true;
		}

		if( CalcRayTriangleIntersection( &A, &B, &C, &cell_verts5, &cell_verts6 ) != -1.0f )
		{
			tris.push_back( i );
			vert_ok[2] = true;
		}

		if( CalcRayTriangleIntersection( &A, &B, &C, &cell_verts7, &cell_verts8 ) != -1.0f )
		{
			tris.push_back( i );
			vert_ok[3] = true;
		}
	}


	/////////////////////////////
	// RETURN DATA //
	////////////////////////////
	int rr = (int)( vert_ok[0] + vert_ok[1] + vert_ok[2] + vert_ok[3] );
	
	if( rr > 3 )
	{
		// intersect cells edges with tri edges
		for( int i = 0; i < mesh->GetTrianglesCount(); i++ )
		{
			CVERTEX_XYZ tA = mesh->GetVerteciesList()[ mesh->GetIndicesList()[ i * 3 + 0 ] ];
			CVERTEX_XYZ tB = mesh->GetVerteciesList()[ mesh->GetIndicesList()[ i * 3 + 1 ] ];
			CVERTEX_XYZ tC = mesh->GetVerteciesList()[ mesh->GetIndicesList()[ i * 3 + 2 ] ];

			D3DXVECTOR2 isect;

			//tri edge 1
			D3DXVECTOR2 tri_edge10( tA.x, tA.z );
			D3DXVECTOR2 tri_edge11( tB.x, tB.z );

			//tri edge 2
			D3DXVECTOR2 tri_edge20( tB.x, tB.z );
			D3DXVECTOR2 tri_edge21( tC.x, tC.z );

			//tri edge 3
			D3DXVECTOR2 tri_edge30( tC.x, tC.z );
			D3DXVECTOR2 tri_edge31( tA.x, tA.z );


			//cell edge 1
			D3DXVECTOR2 cell_edge10( cell_verts1.x, cell_verts1.z);
			D3DXVECTOR2 cell_edge11( cell_verts2.x, cell_verts2.z);

			//cell edge 2
			D3DXVECTOR2 cell_edge20( cell_verts3.x, cell_verts3.z);
			D3DXVECTOR2 cell_edge21( cell_verts4.x, cell_verts4.z);

			//cell edge 3
			D3DXVECTOR2 cell_edge30( cell_verts5.x, cell_verts5.z);
			D3DXVECTOR2 cell_edge31( cell_verts6.x, cell_verts6.z);

			//cell edge 4
			D3DXVECTOR2 cell_edge40( cell_verts7.x, cell_verts7.z);
			D3DXVECTOR2 cell_edge41( cell_verts8.x, cell_verts8.z);

			bool edge1 = false, edge2 = false, edge3 = false, edge4 = false;
            
			//do the intersections
			// 1st cell edge
			if(	CalcLineLineIntersection( &tri_edge10, &tri_edge11, &cell_edge10, &cell_edge11, &isect ) ||
				CalcLineLineIntersection( &tri_edge20, &tri_edge21, &cell_edge10, &cell_edge11, &isect ) ||
				CalcLineLineIntersection( &tri_edge30, &tri_edge31, &cell_edge10, &cell_edge11, &isect ) )
			{
				edge1 = true;
			}

			// 2st cell edge
			if(	CalcLineLineIntersection( &tri_edge10, &tri_edge11, &cell_edge20, &cell_edge21, &isect ) ||
				CalcLineLineIntersection( &tri_edge20, &tri_edge21, &cell_edge20, &cell_edge21, &isect ) ||
				CalcLineLineIntersection( &tri_edge30, &tri_edge31, &cell_edge20, &cell_edge21, &isect ) )
			{
				edge2 = true;
			}

			// 3st cell edge
			if(	CalcLineLineIntersection( &tri_edge10, &tri_edge11, &cell_edge30, &cell_edge31, &isect ) ||
				CalcLineLineIntersection( &tri_edge20, &tri_edge21, &cell_edge30, &cell_edge31, &isect ) ||
				CalcLineLineIntersection( &tri_edge30, &tri_edge31, &cell_edge30, &cell_edge31, &isect ) )
			{
				edge3 = true;
			}

			// 4st cell edge
			if(	CalcLineLineIntersection( &tri_edge10, &tri_edge11, &cell_edge40, &cell_edge41, &isect ) ||
				CalcLineLineIntersection( &tri_edge20, &tri_edge21, &cell_edge40, &cell_edge41, &isect ) ||
				CalcLineLineIntersection( &tri_edge30, &tri_edge31, &cell_edge40, &cell_edge41, &isect ) )
			{
				edge4 = true;
			}

			// if tri intersected -- add it to the list
			if( edge1 ||  edge2 ||  edge3 ||  edge4 )
				tris.push_back( i );
		}

		// clean-up trilist
		//RemoveDupesFromVector( tris );

		tri_count = tris.size();

		int* triangles = new int[ tri_count ];

		for( int i = 0; i < tri_count; i++ )
			triangles[i] = tris.at( i );

		return triangles;
	}
	else
	{
		tri_count = 0;
		return NULL;
	}

	return NULL;
}

UINT CCookieMachineDlg::OnGetDlgCode()
{
	return DLGC_WANTARROWS | DLGC_WANTALLKEYS | DLGC_WANTCHARS;
}

void CCookieMachineDlg::OnMenuShowAllCameras()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_XSI )
		return;

	HTREEITEM	hChildNode;
	HTREEITEM	hCameras	= m_rtSceneContents.FindTreeItemByName( "Cameras" );
	bool		completed	= false;

	for( int i = 0; i < ((CXSIFile*)pBaseScene)->GetCamerasCount(); i++ )
		((CXSIFile*)pBaseScene)->GetCameraByIndex( i )->visible = true;

	pBaseScene->Prepare( g_pd3dDevice );

	//in case root node not found
	if( !hCameras )
		return;

	hChildNode = m_rtSceneContents.GetChildItem( hCameras );

	//in case child node not found
	if( !hChildNode )	
		return;

	while( completed == false )
	{
		m_rtSceneContents.SetItemImage( hChildNode, 5, 5 ); 
		hChildNode = m_rtSceneContents.GetNextItem( hChildNode, TVGN_NEXT );
		
		if( hChildNode == NULL )
			break;
	}

	pBaseScene->Prepare( g_pd3dDevice );
}

void CCookieMachineDlg::OnMenuHideAllCameras()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_XSI )
		return;

	HTREEITEM hChildNode, hCameras = m_rtSceneContents.FindTreeItemByName( "Cameras" );
	bool completed = false;

	
	for( int i = 0; i < ((CXSIFile*)pBaseScene)->GetCamerasCount(); i++ )
		((CXSIFile*)pBaseScene)->GetCameraByIndex( i )->visible  = false;

	pBaseScene->Prepare( g_pd3dDevice );

	//in case root node not found
	if( !hCameras )
		return;

	hChildNode = m_rtSceneContents.GetChildItem( hCameras );

	//in case child node not found
	if( !hChildNode )	
		return;

	while( completed == false )
	{
		m_rtSceneContents.SetItemImage( hChildNode, 11, 11 ); 
		hChildNode = m_rtSceneContents.GetNextItem( hChildNode, TVGN_NEXT );
		
		if( hChildNode == NULL )
			break;
	}

	pBaseScene->Prepare(g_pd3dDevice);
}

void CCookieMachineDlg::OnMenuShowAllLights()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_XSI )
		return;

	HTREEITEM	hChildNode;
	HTREEITEM	hLights		= m_rtSceneContents.FindTreeItemByName( "Lights" );
	bool		completed	= false;
	
	for( int i = 0; i < ((CXSIFile*)pBaseScene)->GetLightsCount(); i++ )
		((CXSIFile*)pBaseScene)->GetLightByIndex( i )->visible = true;

	pBaseScene->Prepare( g_pd3dDevice );

	//in case root node not found
	if( !hLights )
		return;

	hChildNode = m_rtSceneContents.GetChildItem( hLights );

	//in case child node not found
	if( !hChildNode )	
		return;

	while( completed == false )
	{
		m_rtSceneContents.SetItemImage( hChildNode, 9, 9 ); 
		hChildNode = m_rtSceneContents.GetNextItem( hChildNode, TVGN_NEXT );
		
		if( hChildNode == NULL )
			break;
	}

	pBaseScene->Prepare( g_pd3dDevice );
}

void CCookieMachineDlg::OnMenuHideAllLights()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_XSI )
		return;

	HTREEITEM	hChildNode;
	HTREEITEM	hLights		= m_rtSceneContents.FindTreeItemByName( "Lights" );
	bool		completed	= false;


	for( int i = 0; i < ((CXSIFile*)pBaseScene)->GetLightsCount(); i++ )
		((CXSIFile*)pBaseScene)->GetLightByIndex( i )->visible = false;

	pBaseScene->Prepare( g_pd3dDevice );

	//in case root node not found
	if( !hLights )
		return;

	hChildNode = m_rtSceneContents.GetChildItem( hLights );

	//in case child node not found
	if( !hChildNode )	
		return;

	while( completed == false )
	{
		m_rtSceneContents.SetItemImage( hChildNode, 12, 12 ); 
		hChildNode = m_rtSceneContents.GetNextItem( hChildNode, TVGN_NEXT );
		
		if( hChildNode == NULL )
			break;
	}

	pBaseScene->Prepare(g_pd3dDevice);
}

//int CCookieMachineDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
//{
//	if (CDialog::OnCreate(lpCreateStruct) == -1)
//		return -1;
//
//
//	return 0;
//}

void CCookieMachineDlg::OnCbnSelchangeCmbCameras()
{
	if( !pBaseScene )
		return;

	//if( pBaseScene->GetType() != FT_SCN )
	//	return;

	//get current camera name
	int curr = m_cmbCameras.GetCurSel();

	CString str, str2;
	int n = m_cmbCameras.GetLBTextLen( curr );
	m_cmbCameras.GetLBText( curr, str.GetBuffer(n) );

	//make that camera active
	if( str == DEFAULT_CAMERA_ID )
		OnMenuReturnToPerspectiveview();
	else
	{
		if( pBaseScene->GetType() == FT_XSI )
			((CXSIFile*)pBaseScene)->SetSelectedCamera( str.GetBuffer() );

		if( pBaseScene->GetType() == FT_SCN )
			((CSceneFile*)pBaseScene)->SetSelectedCamera( str.GetBuffer() );

		OnMenuSetCameraTocurrent();
	}
}

void CCookieMachineDlg::DeleteSelectedAnimFrames()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN )
		return;

	CValidateDlg*	dlgValidate	= NULL;
	int				_min, _max;
	bool			single_frame = false;

	m_sliderAnim.GetSelection( _min, _max );

	if( (_min == -1 || _min == 0)  && _max == -1 ) 
		single_frame = true;

	CString a[3];

	a[0] = "Вы действительно хотите удалить текущий кадр?";
	a[1] = "Вы действительно хотите удалить выделенные кадры?";

	if( single_frame )
		a[2] = a[0];
	else
		a[2] = a[1];

	if( MessageBox( a[2], "Внимание!", MB_YESNO | MB_ICONWARNING ) == IDYES )
	{     
		int old_frames_count = ((CEfnCharacter*)pBaseScene)->GetFramesCount();
		int old_tri_count    = ((CEfnCharacter*)pBaseScene)->GetTrianglesCount();
		int old_vert_count   = ((CEfnCharacter*)pBaseScene)->GetVerteciesCount();

  		//show validation window
		if( !dlgValidate )
		{
			dlgValidate = new CValidateDlg(this);
			dlgValidate->SetMessageStr("Удаление кадров...");
			ProcessMessages();
		}

		SetCursor( LoadCursor( NULL, IDC_WAIT ) );

		if( single_frame )
		{
			( (CEfnCharacter*)pBaseScene )->DeleteFrames( m_sliderAnim.GetPos(), m_sliderAnim.GetPos() );

			m_vecMacrosDeletedFrames.push_back( m_sliderAnim.GetPos() );
			m_vecMacrosDeletedFrames.push_back( m_sliderAnim.GetPos() );
		}
		else
		{
			( (CEfnCharacter*)pBaseScene )->DeleteFrames( m_iStartingFrame, m_iEndingFrame );

			m_vecMacrosDeletedFrames.push_back( m_iStartingFrame );
			m_vecMacrosDeletedFrames.push_back( m_iEndingFrame );
		}

		m_iCurrentFrame = 0;
		m_sliderAnim.SetPos(0);
		
		firstSelFrame	= 0;
		lastSelFrame	= 0;

		m_sliderAnim.ClearSel(true);
		m_sliderAnim.SetRange(0, ((CEfnCharacter*)pBaseScene)->GetFramesCount() - 1);
		m_sliderAnim.Invalidate(true);

		m_sliderAnim.SetPos( m_iStartingFrame );

		CString a;
		a.Format("%f", ((CEfnCharacter*)pBaseScene)->GetZOffset(0));
		m_eFrameOffset.SetWindowText(a);
		a.Format("%d", m_iStartingFrame);
		m_eFrameCounter.SetWindowText(a);

		m_iStartingFrame = m_sliderAnim.GetPos();
		m_iEndingFrame   = m_sliderAnim.GetPos();

		UpdateStatusBarInfoEFN();

		//update tree nodes
		HTREEITEM hInfo        = m_rtSceneContents.FindTreeItemByName( "Info" );
		HTREEITEM hInfoChild   = m_rtSceneContents.GetChildItem( hInfo );
		HTREEITEM hFramesCount;

		a.Format( "Frames: %d", old_frames_count );
		hFramesCount = m_rtSceneContents.FindTreeItemByName( a, hInfo );
		a.Format( "Frames: %d", ((CEfnCharacter*)pBaseScene)->GetFramesCount() );
		m_rtSceneContents.SetItemText( hFramesCount, a );

		a.Format( "Triangles in model: %d", old_tri_count );
		hFramesCount = m_rtSceneContents.FindTreeItemByName( a, hInfo );
		a.Format( "Triangles in model: %d", ((CEfnCharacter*)pBaseScene)->GetTrianglesCount() );
		m_rtSceneContents.SetItemText( hFramesCount, a );

		a.Format( "Vertecies in model: %d", old_vert_count );
		hFramesCount = m_rtSceneContents.FindTreeItemByName( a, hInfo );
		a.Format( "Vertecies in model: %d", ((CEfnCharacter*)pBaseScene)->GetVerteciesCount() );
		m_rtSceneContents.SetItemText( hFramesCount, a );

		//rebuild d3d buffers
		( (CEfnCharacter*)pBaseScene )->Prepare( g_pd3dDevice ); 
		( (CEfnCharacter*)pBaseScene )->SetCurrNextFrame( 0, 0 );

		//delete validation window
		SAFE_DELETE( dlgValidate );

		SetCursor( LoadCursor( NULL, IDC_ARROW ) );
	}
}

void CCookieMachineDlg::OnMenuObjectShowAll()
{
	if( !pBaseScene || m_bIsVertexEditingModeOn )
		return;

	OnBnClickedBtnShowall();
}

void CCookieMachineDlg::OnMenuMeshlistMeshesHide()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN_MESHLIST )
		return;

	((CEfnMeshlist*)pBaseScene)->ShowAllMeshes( false );

	HTREEITEM hMeshes	= m_rtSceneContents.FindTreeItemByName( "Meshes" );
	HTREEITEM hTemp		= m_rtSceneContents.GetChildItem( hMeshes );

	while( hTemp )
	{
		m_rtSceneContents.SetItemImage( hTemp, 4, 4 );
		hTemp = m_rtSceneContents.GetNextSiblingItem( hTemp );
	}
}

void CCookieMachineDlg::OnMenuMeshlistMeshesShow()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN_MESHLIST )
		return;

	((CEfnMeshlist*)pBaseScene)->ShowAllMeshes( true );

	HTREEITEM hMeshes	= m_rtSceneContents.FindTreeItemByName( "Meshes" );
	HTREEITEM hTemp		= m_rtSceneContents.GetChildItem( hMeshes );

	while( hTemp )
	{
		m_rtSceneContents.SetItemImage( hTemp, 0, 0 );
		hTemp = m_rtSceneContents.GetNextSiblingItem( hTemp );
	}
}

void CCookieMachineDlg::OnMenuMeshlistNullsHide()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN_MESHLIST )
		return;

	((CEfnMeshlist*)pBaseScene)->ShowAllNulls( false );

	HTREEITEM hNulls	= m_rtSceneContents.FindTreeItemByName( "Nulls" );
	HTREEITEM hTemp		= m_rtSceneContents.GetChildItem( hNulls );

	while( hTemp )
	{
		m_rtSceneContents.SetItemImage( hTemp, 24, 24 );
		hTemp = m_rtSceneContents.GetNextSiblingItem( hTemp );
	}
}

void CCookieMachineDlg::OnMenuMeshlistNullsShow()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN_MESHLIST )
		return;

	((CEfnMeshlist*)pBaseScene)->ShowAllNulls( true );

	HTREEITEM hNulls	= m_rtSceneContents.FindTreeItemByName( "Nulls" );
	HTREEITEM hTemp		= m_rtSceneContents.GetChildItem( hNulls );

	while( hTemp )
	{
		m_rtSceneContents.SetItemImage( hTemp, 10, 10 );
		hTemp = m_rtSceneContents.GetNextSiblingItem( hTemp );
	}
}

void CCookieMachineDlg::OnMenuMeshlistMeshShow()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN_MESHLIST )
		return;

	HTREEITEM hSelected = m_rtSceneContents.GetSelectedItem();
	CString			strNodeText;

	//get currently selected treenode name
	if( hSelected >= 0 )
	{
		strNodeText = m_rtSceneContents.GetItemText(hSelected);

		m_rtSceneContents.SetItemImage( hSelected, 0, 0 );
		((CEfnMeshlist*)pBaseScene)->ShowMeshes( strNodeText.GetBuffer(), true );
	}
}

void CCookieMachineDlg::ObjectRename()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN && 
		pBaseScene->GetType() != FT_EFN_MESHLIST && 
		pBaseScene->GetType() != FT_SCN )
		return;
	
	CEfnMeshlist* pMeshlist = NULL;

	if( pBaseScene->GetType() == FT_EFN_MESHLIST )
	{
		pMeshlist = (CEfnMeshlist*)pBaseScene;
	}
	else if( pBaseScene->GetType() == FT_SCN )
	{
		pMeshlist = (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();
	}
	else
	{}

	if( pBaseScene->GetType() == FT_EFN )
	{
		m_rtSceneContents.StartEditingLabel();
	}
	else if( pBaseScene->GetType() == FT_EFN_MESHLIST || pBaseScene->GetType() == FT_SCN )
	{
		if( !pMeshlist || pMeshlist->IsVertexEditingMode() )
			return;

		m_rtSceneContents.StartEditingLabel();
	}
}

void CCookieMachineDlg::OnMenuObjectMaterial()
{
	if( !pBaseScene || m_bIsVertexEditingModeOn )
		return;

	if( pBaseScene->GetType() == FT_EFN )
	{
		OnMenuEfnEditMaterial();
	}
	else if( pBaseScene->GetType() == FT_EFN_MESHLIST )
	{
		OnMenuMeshlistMeshMaterial();
	}
	else if( pBaseScene->GetType() == FT_SCN )
	{
		OnMenuScnMeshEditMaterial();
	}
	else
	{}
}

void CCookieMachineDlg::UpdateContextMenus()
{
	//CMenu* menu;
	//menu = menu->FromHandle( LoadMenu( AfxGetInstanceHandle(), MAKEINTRESOURCE( IDR_MENU_CONTEXT ) ) );

	//// disable most menu items
	//EnableMenuItem( menu, ID_MENU_SCN_MESH_DELETE, false );

	//if( m_appSettings.ExpertMode )
	//{
	//	EnableMenuItem( menu, ID_MENU_SCN_MESH_DELETE, true );
	//}

	//ID_MENU_SCN_MESH_DELETE
	//ID_MENU_SCN_MESH_RENAME
}

void CCookieMachineDlg::UpdateMeshesInfoInExplorer()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_EFN_MESHLIST )
		return;

	HTREEITEM hInfo = m_rtSceneContents.FindTreeItemByName( "Info" );

	HTREEITEM hMeshCnt, hMeshPolyCnt, hTemp;

	hTemp = m_rtSceneContents.GetChildItem( hInfo );

	while( hTemp )
	{
		CString strTemp = m_rtSceneContents.GetItemText( hTemp );

		if( strTemp.Left( 13 ) == "Meshes count:" )
			hMeshCnt = hTemp;

		if( strTemp.Left( 16 ) == "Total polycount:" )
			hMeshPolyCnt = hTemp;

		hTemp = m_rtSceneContents.GetNextSiblingItem( hTemp );
	}

	if( pBaseScene->GetType() == FT_EFN_MESHLIST )
	{
		CString strTemp;

		strTemp.Format( "Meshes count: %d", ((CEfnMeshlist*)pBaseScene)->GetMeshesCount() - 1 );
		m_rtSceneContents.SetItemText( hMeshCnt, strTemp );

		strTemp.Format( "Total polycount: %d", ((CEfnMeshlist*)pBaseScene)->GetTotalPolycount() );
		m_rtSceneContents.SetItemText( hMeshPolyCnt, strTemp );
	}
}

void CCookieMachineDlg::OnMenuObjectCreateMesh( enum GeomPrimitiveTypes type, D3DXVECTOR3* pos )
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN_MESHLIST && pBaseScene->GetType() != FT_SCN )
		return;

	UINT		uiMask		= TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	CVERTEX*	vertecies	= NULL;
	int*		indicies	= NULL;
	CMesh*		msh			= NULL;
	int			iVcount		= 0;
	int			iIcount		= 0;


	int			tri_idx[]	= { 0, 1, 2 };
	int			quad_idx[]	= { 0, 1, 2, 0, 2, 3 };
	int			cube_idx[]	= { 0, 1, 2, 0, 2, 3, 4, 0, 3, 4, 3, 7, 6, 2, 5, 2, 1, 5,
	                            5, 4, 7, 5, 7, 6, 7, 3, 2, 7, 2, 6, 5, 1, 0, 5, 0, 4 };

	CModelNameDlg* dlgModelName = new CModelNameDlg(this);

	dlgModelName->SetCaption( "Название модели:" );
	dlgModelName->SetString( "" );
	dlgModelName->DoModal();

	str_t strNewModelName = dlgModelName->GetString().GetBuffer();

	SAFE_DELETE( dlgModelName );


	if( strNewModelName == "" )
		return;

	if( pBaseScene->GetType() == FT_EFN_MESHLIST )
	{
		if( ((CEfnMeshlist*)pBaseScene)->GetMeshByName2( strNewModelName ) )
		{
			MessageBox( "Меш с таким ID уже существует!", "Внимание!", MB_OK | MB_ICONINFORMATION );
			return;
		}

		((CEfnMeshlist*)pBaseScene)->AddMesh( strNewModelName );
		msh = ((CEfnMeshlist*)pBaseScene)->GetMeshByName( strNewModelName );
	}
	else if( pBaseScene->GetType() == FT_SCN )
	{
		CEfnMeshlist* pMeshlist = (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();

		if( pMeshlist->GetMeshByName2( strNewModelName ) )
		{
			MessageBox( "Меш с таким ID уже существует!", "Внимание!", MB_OK | MB_ICONINFORMATION );
			return;
		}

		pMeshlist->AddMesh( strNewModelName );
		msh = pMeshlist->GetMeshByName( strNewModelName );
	}
	else
	{}

	if( !msh )
		return;

	switch( type )
	{
		case GEOM_TRI:
			iVcount	= 3;
			iIcount	= 3;

			//create vertecies
			vertecies	= new CVERTEX[iVcount];
			ZeroMemory( vertecies, sizeof(CVERTEX) * iVcount );

			vertecies[0].y = 1.5f;
			vertecies[1].x = 1.0f;
			vertecies[2].x = -1.0f;

			//create indices
			indicies	= new int[iIcount];
			memcpy( indicies, tri_idx, sizeof( int ) * iIcount );

			//init mesh
			msh->SetVertecies( vertecies, iVcount );
			msh->SetIndices( indicies, iIcount );

			break;


		case GEOM_QUAD:
			iVcount	= 4;
			iIcount	= 6;

			//create vertecies
			vertecies	= new CVERTEX[iVcount];
			ZeroMemory( vertecies, sizeof(CVERTEX) * iVcount );

			vertecies[0].x = -1.0f;

			vertecies[1].x = -1.0f;
			vertecies[1].y = 2.0f;

			vertecies[2].x = 1.0f;
			vertecies[2].y = 2.0f;

			vertecies[3].x = 1.0f;

			//create indices
			indicies	= new int[iIcount];
			memcpy( indicies, quad_idx, sizeof( int ) * iIcount );

			//init mesh
			msh->SetVertecies( vertecies, iVcount );
			msh->SetIndices( indicies, iIcount );

			break;
		

		case GEOM_CUBE:
			iVcount	= 8;
			iIcount	= 36;

			//create vertecies
			vertecies	= new CVERTEX[iVcount];
			ZeroMemory( vertecies, sizeof(CVERTEX) * iVcount );

			vertecies[0].x = -1.0f;
			vertecies[0].z = -1.0f;

			vertecies[1].x = -1.0f;
			vertecies[1].y = 2.0f;
			vertecies[1].z = -1.0f;

			vertecies[2].x = 1.0f;
			vertecies[2].y = 2.0f;
			vertecies[2].z = -1.0f;

			vertecies[3].x = 1.0f;
			vertecies[3].z = -1.0f;

			vertecies[4].x = -1.0f;
			vertecies[4].z = 1.0f;

			vertecies[5].x = -1.0f;
			vertecies[5].y = 2.0f;
			vertecies[5].z = 1.0f;

			vertecies[6].x = 1.0f;
			vertecies[6].y = 2.0f;
			vertecies[6].z = 1.0f;

			vertecies[7].x = 1.0f;
			vertecies[7].z = 1.0f;

			//create indices
			indicies = new int[iIcount];
			memcpy( indicies, cube_idx, sizeof( int ) * iIcount );

			//init mesh
			msh->SetVertecies( vertecies, iVcount );
			msh->SetIndices( indicies, iIcount );

			break;

		default:
			if( pBaseScene->GetType() == FT_EFN_MESHLIST )
			{
				((CEfnMeshlist*)pBaseScene)->DeleteMesh( strNewModelName );
			}
			else if( pBaseScene->GetType() == FT_SCN )
			{
				CEfnMeshlist* pMeshlist = (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();
				pMeshlist->DeleteMesh( strNewModelName );
			}
			else
			{}
			
			return;
	}

	//transform obj to pos
	if( pos )
	{
		for( int i = 0; i < msh->GetVerteciesCount(); i++ )
		{
			msh->GetVertecies()[i].x += pos->x;
			msh->GetVertecies()[i].y += pos->y;
			msh->GetVertecies()[i].z += pos->z;
		}
	}

	msh->CalcOOBB();
	msh->CalcBoundingSphere();
	msh->CalcReverseIndex();
	msh->PrecalculateNormals();
	//msh->Prepare( g_pd3dDevice );

	((CSceneFile*)pBaseScene)->Prepare( g_pd3dDevice );

	HTREEITEM hMeshes	= m_rtSceneContents.FindTreeItemByName( "Meshes" );
	HTREEITEM hMesh		= m_rtSceneContents.InsertItem( uiMask, strNewModelName.data(), 0, 0, 0, 0, 0, hMeshes, TVI_SORT ); 

	m_rtSceneContents.SelectItem( hMesh );
	m_rtSceneContents.EnsureVisible( hMesh );

	//cleanup
	SAFE_DELETE_ARRAY( vertecies );
	SAFE_DELETE_ARRAY( indicies );

	UpdateMeshesInfoInExplorer();
}

void CCookieMachineDlg::OnMenuObjectHide()
{
	if( !pBaseScene || m_bIsVertexEditingModeOn )
		return;

	if( pBaseScene->GetType() == FT_EFN_MESHLIST )
	{
		SelectedObjectsHide();
	}
	else if( pBaseScene->GetType() == FT_SCN )
	{
		HTREEITEM	hSelected		= m_rtSceneContents.GetSelectedItem();
		HTREEITEM	hParent			= m_rtSceneContents.GetParentItem( hSelected );
		HTREEITEM	hParentParent	= m_rtSceneContents.GetParentItem( hParent );

		CString	item	= m_rtSceneContents.GetItemText( hSelected );
		CString	parent	= m_rtSceneContents.GetItemText( hParent );


		//get currently selected treenode name
		if( parent == "Lights" && hParentParent == NULL )
		{
			CLIGHT* light = ((CSceneFile*)pBaseScene)->GetLightByID( item.GetBuffer() );
			
			if( light->visible )
			{
				light->visible = false;
				m_rtSceneContents.SetItemImage( hSelected, 12, 12 );
			}
			else
			{
				light->visible = true;
				m_rtSceneContents.SetItemImage( hSelected, 9, 9 );
			}

			((CSceneFile*)pBaseScene)->PrepareLightsLines( g_pd3dDevice ); 
		}
		else if( parent == "Cameras" && hParentParent == NULL )
		{
			CCAMERA2* camera = ((CSceneFile*)pBaseScene)->GetCameraByID( item.GetBuffer() );
			
			if( camera->visible )
			{
				camera->visible = false;
				m_rtSceneContents.SetItemImage( hSelected, 11, 11 );
			}
			else
			{
				camera->visible = true;
				m_rtSceneContents.SetItemImage( hSelected, 5, 5 );
			}
		}
		else if( parent == "Meshes" && hParentParent == NULL )
		{
			SelectedObjectsHide();
		}
		else if( item == "Cameras" && hParent == NULL )
		{
			OnMenuScnCamerasHide();
		}
		else if( item == "Lights" && hParent == NULL )
		{
			OnMenuScnLightsHide();
		}
		else if( item == "Nulls" && hParent == NULL )
		{
			OnMenuScnNullsHide();
		}
		else if( item == "Meshes" && hParent == NULL )
		{
			OnMenuScnMeshesHide();
		}
		else if( item == "Curves" && hParent == NULL )
		{
			OnMenuScnCurvesHide();
		}
	}
	else
	{}
}

void CCookieMachineDlg::OnMenuMeshlistMeshRename( CString strCurrID, CString strNewID )
{
	if( strCurrID == strNewID )
		return;

	CEfnMeshlist*	pMeshlist = ((CEfnMeshlist*)pBaseScene);

	CString _strCurrID	= strCurrID;
	CString _strNewID	= strNewID;

	_strCurrID.MakeUpper();
	_strNewID.MakeUpper();

	if( pMeshlist->GetMeshByName2( strNewID.GetBuffer() ) && _strCurrID != _strNewID )
	{
		MessageBox( "Меш с таким ID уже существует!", "Внимание!", MB_OK | MB_ICONINFORMATION );
		return;
	}

	CMesh* msh = pMeshlist->GetMeshByName( strCurrID.GetBuffer() );
	
	if( !msh )
		return;

    msh->SetName( strNewID.GetBuffer() );
	m_rtSceneContents.SetItemText( m_rtSceneContents.GetSelectedItem(), strNewID );
}

void CCookieMachineDlg::OnMenuMeshlistMeshDelete()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN_MESHLIST )
		return;


	HTREEITEM	hSelected = m_rtSceneContents.GetSelectedItem();
	CString		tmp;
	CString		tmp2;

	tmp = m_rtSceneContents.GetItemText(hSelected);

	tmp2 = "Вы действительно хотите удалить объект \"" + tmp + "\"?";

	if( MessageBox( tmp2, "Внимание!", MB_YESNO | MB_ICONWARNING  ) == IDYES )
	{
		((CEfnMeshlist*)pBaseScene)->DeleteMesh( tmp.GetBuffer() );
		((CEfnMeshlist*)pBaseScene)->SetNullIcon( pNullIco );
		m_rtSceneContents.DeleteItem( hSelected );
	}

	UpdateMeshesInfoInExplorer();
}

void CCookieMachineDlg::OnMenuEfnNullAttachTo( str_t strNullID, str_t strPath )
{
	CString	path	= strPath.data();
	int		idx		= path.ReverseFind( '\\' );
	str_t	strDir	= path.Left( idx + 1 );
	str_t	strFile	= path.Right( path.GetLength() - idx - 1 );

	OnMenuEfnNullAttachTo( strNullID, strDir, strFile );
}

void CCookieMachineDlg::OnMenuEfnNullAttachTo( str_t strNullID, str_t strDir, str_t strFile )
{
	CBaseScene*	pAttachment	= NULL;
	HTREEITEM	hItem2 = m_rtSceneContents.GetSelectedItem();

	TNODE* node = ( TNODE* )m_rtSceneContents.GetItemData( hItem2 );

	if( !node || node->_type != TREENODE::N_NULL || !node->_data )
		return;

	CNULL2* nl = ((CEfnCharacter*)node->_data)->GetNullByName( strNullID );

	if( !nl )
		return;

	bool exists = false;

	for( UINT i = 0; i < nl->attaches.size(); i++ )
	{
		if( nl->attaches[i]->GetFolder() == strDir && nl->attaches[i]->GetFilename() == strFile )
		{
			exists = true;
			break;
		}
	}

	if( exists )
	{
		MessageBox( ("Файл \"" + strDir + strFile + "\" уже прикреплён!").data(), "Информация.", MB_OK | MB_ICONINFORMATION ); 
		return;
	}

	if( !FileExists( strDir + strFile ) )
	{
		MessageBox( ("Файл " + strDir + strFile + " не найден!").data(), "Ошибка.", MB_OK | MB_ICONWARNING ); 
		return;
	}

	if( GetEfnFiletype( strDir + strFile ) != EF_ANIMATION )
	{
		MessageBox( "Тип файла должен быть анимационным EFN!", "Ошибка.", MB_OK | MB_ICONWARNING ); 
		return;
	}

	CString			temp;
	bool			bIsIncorrectFileVersion	= false;
	CValidateDlg*	dlgValidate				= NULL;


	CString extention = strFile.substr( strFile.length() - 3, 3 ).data();
	extention.MakeUpper();

	//show validation window
	if( !dlgValidate )
	{
		dlgValidate = new CValidateDlg( this );
		dlgValidate->SetMessageStr( "Проверка файла..." );
		ProcessMessages();
	}

	SetCursor( LoadCursor(NULL, IDC_WAIT) );


	if( extention == "EFN" )
	{
		str_t _strFile	= _2Lower( (char*)strFile.data() );
		str_t _strDir	= _2Lower( (char*)strDir.data() );

		//CBaseScene* pTemp = FindExistingEfn( pBaseScene, _strFile, _strDir );

		//if( pTemp )
		//{
		//	nl->attaches.push_back( pTemp );

		//	//add detailed attachment info to tree view
		//	InsertEfnAttachNodes( m_rtSceneContents.GetSelectedItem(), pTemp );
		//}
		//else
		//{
			CEfnCharacter* efn_character = new CEfnCharacter;

			if( efn_character->LoadFromFile( strDir + strFile ) )
			{
				efn_character->SetParentWnd( this );
				//OnMenuEfnNullDetach();

				pAttachment = efn_character;
				pAttachment->SetPaths( strDir, strFile );


				//prepare data for rendering
				pAttachment->Prepare( g_pd3dDevice );

				//SAFE_DELETE( nl->attachment );
				nl->attaches.push_back( pAttachment );

				//add detailed attachment info to tree view
				InsertEfnAttachNodes( m_rtSceneContents.GetSelectedItem(), pAttachment );
			}
			else
			{
				// load failed -- cleanup
				SAFE_DELETE( efn_character ); 
			}
		//}
	} // 	else if (extention == "EFN")

	SAFE_DELETE( dlgValidate );

	//restore cursor
	SetCursor( LoadCursor(NULL, IDC_ARROW) );
}

void CCookieMachineDlg::StopGame()
{
	g_MemMappingsMgr->PostScriptToEngine( "QEngine_Exit();" );
}

CBaseScene* CCookieMachineDlg::FindExistingEfn( CBaseScene* pRoot, str_t strFile, str_t strDir )
{
	if( pRoot->GetFilename() == strFile && pRoot->GetFolder() == strDir )
		return pRoot;

	CEfnCharacter* pEfnAnim = (CEfnCharacter*)pRoot;

	for( int i = 0; i < pEfnAnim->GetNullsCount(); i++ )
	{
		CNULL2* pNull = pEfnAnim->GetNullByIndex(i);

		for( UINT j = 0; j < pNull->attaches.size(); j++ )
		{
			CBaseScene* pRes = FindExistingEfn( pNull->attaches[j], strFile, strDir );

			if( pRes )
				return pRes;
		}
	}

	return NULL;
}

void CCookieMachineDlg::InsertEfnAttachNodes( HTREEITEM hRoot, CBaseScene* pAttach )
{
	if( !pAttach || !hRoot )
		return;

	if( pAttach->GetType() != FT_EFN )
		return;

	CEfnCharacter*	pEfnAnim	= (CEfnCharacter*)pAttach;
	TNODE*			node		= NULL; 
	HTREEITEM		hNulls;
	HTREEITEM		hAnim;
	
	hRoot	= m_rtSceneContents.InsertItem2( pAttach->GetFilename().data(), 28, new TNODE( TREENODE::N_ATTACH, pAttach ), hRoot, NULL );
	hNulls	= m_rtSceneContents.InsertItem2( "Nulls", 1, new TNODE( TREENODE::N_NULLS, pAttach ), hRoot, NULL );
	hAnim	= m_rtSceneContents.InsertItem2( "Animations", 5, new TNODE( TREENODE::N_ANIMATIONS, pAttach ), hRoot, NULL );

	m_rtSceneContents.InsertItem2( "Mesh", 0, new TNODE( TREENODE::N_MESH, pAttach ), hRoot, NULL );
	m_rtSceneContents.InsertItem2( "Material", 16, new TNODE( TREENODE::N_MATERIAL, pAttach ), hRoot, NULL );
	m_rtSceneContents.InsertItem2( "Texture", 21, new TNODE( TREENODE::N_TEXTURE, pAttach ), hRoot, NULL );

	/********************** NULLS ********************************/
	for( int i = 0; i < pEfnAnim->GetNullsCount(); i++ )
	{
		CNULL2* temp_null = pEfnAnim->GetNullByIndex( i );

		//(TNODE*)m_rtSceneContents.GetItemData( hRoot )

		//insert null (name)
		m_rtSceneContents.InsertItem2( temp_null->id.data(), 10, new TNODE( TREENODE::N_NULL, pAttach ), hNulls, TVI_SORT );
	}

	/********************** ANIMATIONS ***************************/
	for( int i = 0; i < pEfnAnim->GetAnimationsCount(); i++ )
	{
		EFN_ANIMATION* anim = pEfnAnim->GetAnimationByIndex( i );

		m_rtSceneContents.InsertItem2( anim->name.data(), 8, new TNODE( TREENODE::N_ANIMATION, pAttach ), hAnim, TVI_SORT );
	}
}

void CCookieMachineDlg::OnMenuEfnNullAttachTo()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN )
		return;

	HTREEITEM hSelected = m_rtSceneContents.GetSelectedItem();

	str_t	szFilter	= "EFN Files (*.efn)|*.efn|";
	CString	temp		= "*.efn";
	str_t	file_dir, file_name;

	//show open file dialog
	CCustomFileDialog* dlg = new CCustomFileDialog( TRUE, temp.GetBuffer(0), 0, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_ENABLETEMPLATE | OFN_ENABLESIZING, szFilter.data(), NULL );
	dlg->m_pOFN->lpstrInitialDir	= m_appSettings.FilesPath;
	//dlg->m_pOFN->lpfnHook			= OpenDlgExProc;
	dlg->m_pOFN->FlagsEx			= OFN_EX_NOPLACESBAR;
	dlg->m_pOFN->lpTemplateName		= MAKEINTRESOURCE( IDD_OPENFILE_EXT );
	dlg->m_pOFN->hInstance			= GetModuleHandle(NULL);

	m_bIsAppPaused = false;

	//user confirmed "open file dialog"
	if( dlg->DoModal() == IDOK )
	{
		file_dir	= dlg->GetPathName().Left( dlg->GetPathName().GetLength() - dlg->GetFileName().GetLength() );
		file_name	= dlg->GetFileName();

		m_strLastFileOpenDirectory = file_dir.data();

		if( !FileExists( file_dir + file_name ) )
		{
			MessageBox( "Файл не найден!", "Ошибка.", MB_OK | MB_ICONWARNING ); 
			return;
		}

		OnMenuEfnNullAttachTo( m_rtSceneContents.GetItemText(hSelected).GetBuffer(), file_dir, file_name );
	}

	SAFE_DELETE( dlg );
}

void CCookieMachineDlg::OnMenuEfnNullDetachAll()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN )
		return;

	HTREEITEM	hRoot	= m_rtSceneContents.GetSelectedItem();
	TNODE*		data	= (TNODE*)m_rtSceneContents.GetCurrNodeData();

	if( !data || data->_type != TREENODE::N_NULL )
		return;

	CNULL2* pNl = ((CEfnCharacter*)data->_data)->GetNullByName( m_rtSceneContents.GetCurrNodeText().GetBuffer() );

	for( UINT i = 0; i < pNl->attaches.size(); i++ )
		SAFE_DELETE( pNl->attaches[i] );

	pNl->attaches.clear();


	HTREEITEM hTemp = m_rtSceneContents.GetChildItem( hRoot );

	while( hTemp )
	{
		m_rtSceneContents.DeleteItem( hTemp );
		hTemp = m_rtSceneContents.GetChildItem( hRoot );
	}
}

void CCookieMachineDlg::OnMenuFpsChange()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN )
		return;

	CModelNameDlg* dlgModelName = new CModelNameDlg(this);
	HTREEITEM hSelected = m_rtSceneContents.GetSelectedItem();
	CString tmp;

	tmp = m_rtSceneContents.GetItemText(hSelected);
	tmp = tmp.Right(tmp.GetLength() - 4);
	tmp.Trim();

	dlgModelName->SetCaption( "FPS:" );
	dlgModelName->SetString(tmp);
	dlgModelName->DoModal();
	
	((CEfnCharacter*)pBaseScene)->SetFPS( atof( dlgModelName->GetString().GetBuffer() ) );

	tmp.Format("FPS: %s", dlgModelName->GetString() );

	m_rtSceneContents.SetItemText( hSelected, tmp );

	SAFE_DELETE( dlgModelName );
}

void CCookieMachineDlg::OnMenuEfnLoadTexture()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN )
		return;

	TNODE* node = ( TNODE* )m_rtSceneContents.GetCurrNodeData();

	if( !node || node->_type != TREENODE::N_TEXTURE )
		return;

	str_t	szFilter	= "Texture Files (*.jpg; *.jpeg; *.dds)|*.jpg; *.jpeg; *.dds|";
	CString	temp		= "*.jpg, *.jpeg, *.dds";


	//show open file dialog
	CCustomFileDialog* dlg = new CCustomFileDialog( TRUE, temp.GetBuffer(0), 0, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_ENABLETEMPLATE | OFN_ENABLESIZING, szFilter.data(), NULL );
	dlg->m_pOFN->lpstrInitialDir	= m_appSettings.FilesPath;
	//dlg->m_pOFN->lpfnHook			= OpenDlgExProc;
	dlg->m_pOFN->FlagsEx			= OFN_EX_NOPLACESBAR;
	dlg->m_pOFN->lpTemplateName		= MAKEINTRESOURCE( IDD_OPENFILE_EXT );
	dlg->m_pOFN->hInstance			= GetModuleHandle(NULL);

	m_bIsAppPaused = false;

	//user confirmed "open file dialog"
	if( dlg->DoModal() == IDOK )
	{
		str_t file_dir	= dlg->GetPathName().Left( dlg->GetPathName().GetLength() - dlg->GetFileName().GetLength() );
		str_t file_name	= dlg->GetFileName();

		m_strLastFileOpenDirectory = file_dir.data();

		if( !FileExists( file_dir + file_name ) )
		{
			MessageBox( "Файл не найден!", "Ошибка.", MB_OK | MB_ICONWARNING ); 
			return;
		}

		((CEfnCharacter*)node->_data)->SetTexturePath( file_dir + file_name );
		((CEfnCharacter*)node->_data)->ResetTexture( g_pd3dDevice );
	}

	SAFE_DELETE( dlg );
}

void CCookieMachineDlg::OnMenuMeshlistScenetag()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN_MESHLIST )
		return;

	CModelNameDlg *dlgModelName = new CModelNameDlg(this);
	HTREEITEM hSelected = m_rtSceneContents.GetSelectedItem();

	dlgModelName->SetCaption( "Имя сцены:" );
	dlgModelName->SetString( ((CEfnMeshlist*)pBaseScene)->GetSceneTag().data() );
	dlgModelName->DoModal();
	
	((CEfnMeshlist*)pBaseScene)->SetSceneTag( dlgModelName->GetString().GetBuffer(0) );

	CString node_new_text;

	node_new_text.Format( "Scene name: %s", dlgModelName->GetString().GetBuffer(0) );

	m_rtSceneContents.SetItemText( hSelected, node_new_text );

	SAFE_DELETE( dlgModelName );
}

void CCookieMachineDlg::ShowScriptsWindow()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() == FT_SCN )
	{
		if( !m_dlgScriptingDlg )
		{
			CSceneFile*		scene	= ((CSceneFile*)pBaseScene);
			CEfnMeshlist*	mlist	= (CEfnMeshlist*)scene->GetMeshlist();
			
			CMesh* nulls_mesh = mlist->GetMeshByName( "Nulls" );

			StrVec nulls;

			for( int i = 0; i < nulls_mesh->GetNullsCount(); i++ )
			{
				str_t nl_str = "Null." + nulls_mesh->GetNullByIndex( i )->id;
				nulls.push_back( nl_str );
			}

			m_dlgScriptingDlg = new CScriptingDlg( this );

			m_dlgScriptingDlg->m_strFilePath = "Scripts editor -- " + pBaseScene->GetFolder() + pBaseScene->GetFilename();
			m_dlgScriptingDlg->SetBinds( ((CSceneFile*)pBaseScene)->GetBinds() );
			m_dlgScriptingDlg->SetFuncs( ((CSceneFile*)pBaseScene)->GetFuncs() );
			m_dlgScriptingDlg->m_strLastSelObj			= scene->m_strLastEditedObj;
			m_dlgScriptingDlg->m_strFilterParID_full	= scene->m_strLastEditedObj_FilterParID.data();
			m_dlgScriptingDlg->m_strFilterParType		= scene->m_strLastEditedObj_FilterParType.data();

			m_dlgScriptingDlg->SetNulls( &nulls );
			m_dlgScriptingDlg->DoModal();

			scene->m_strLastEditedObj = m_dlgScriptingDlg->m_strLastSelObj;

			scene->m_strLastEditedObj_FilterParType = m_dlgScriptingDlg->m_strFilterParType.GetBuffer();
			scene->m_strLastEditedObj_FilterParID	= m_dlgScriptingDlg->m_strFilterParID_full.GetBuffer();

			SAFE_DELETE( m_dlgScriptingDlg );
		}
		//else
		//	SAFE_DELETE( m_dlgScriptingDlg );
	}
	else if( pBaseScene->GetType() == FT_EFN )
	{
		CAnimScriptingDlg* dlg = new CAnimScriptingDlg(this);
		dlg->SetEfn( (CEfnCharacter*)pBaseScene );
		dlg->DoModal();

		delete dlg;
	}
	else
	{}
}

void CCookieMachineDlg::ShowBindsWindow()
{
	StrVec vecSceneData;

	if( pBaseScene && pBaseScene->GetType() == FT_SCN )
	{
		if( !m_dlgBindingDlg )
		{
			CSceneFile*	scene = ((CSceneFile*)pBaseScene);
			CString		temp;

			//collect data from the scene
			for( int i = 0 ; i < scene->GetLightsCount(); i++ )
			{
				temp.Format( "Light.%s", scene->GetLightByIndex( i )->name.data() );
				vecSceneData.push_back( temp.GetBuffer() );
			}

			for( int i = 0 ; i < scene->GetCamerasCount(); i++ )
			{
				temp.Format( "Camera.%s", scene->GetCameraByIndex( i )->name.data() );
				vecSceneData.push_back( temp.GetBuffer() );
			}

			CEfnMeshlist* meshlist = (CEfnMeshlist*)scene->GetMeshlist();

			if( meshlist )
			{
				for( int i = 0 ; i < meshlist->GetMeshesCount(); i++ )
				{
					temp.Format( "Mesh.%s", meshlist->GetMeshByIndex( i )->GetName().data() );
					vecSceneData.push_back( temp.GetBuffer() );
				}
			}

			StrVec tex_vec = *(scene->GetTextures());
			for( unsigned int i = 0; i < tex_vec.size(); i++ )
			{
				temp.Format( "Texture.%s", tex_vec[i].data() );
				vecSceneData.push_back( temp.GetBuffer() );
			}

			temp.Format( "Scene.%s", scene->GetSceneID().data() );
			vecSceneData.push_back( temp.GetBuffer() );


			StrVec scnTypes;
			GetTypesList( scnTypes );


			StrVec* m_CustomObjects	= ((CSceneFile*)pBaseScene)->GetCustObjList();
			StrVec* m_CustomTypes	= ((CSceneFile*)pBaseScene)->GetCustTypesList();

			if( m_appSettings.ExpertMode )
			{
				for( unsigned int i = 0; i < m_CustomObjects->size(); i++ )
				{
					temp.Format( "Object.%s", (*m_CustomObjects)[i].data() );
					vecSceneData.push_back( temp.GetBuffer() );
				}

				for( unsigned int i = 0; i < m_CustomTypes->size(); i++ )
				{
					scnTypes.push_back( (*m_CustomTypes)[i] );
				}
			}


			m_dlgBindingDlg = new CBindsDlg();
			m_dlgBindingDlg->LinkBinds( ((CSceneFile*)pBaseScene)->GetBinds() );
			m_dlgBindingDlg->LinkFuncs( ((CSceneFile*)pBaseScene)->GetFuncs() );
			m_dlgBindingDlg->LinkSceneData( &vecSceneData );
			m_dlgBindingDlg->LinkSceneTypes( &scnTypes );
			m_dlgBindingDlg->SetParent( this );
			m_dlgBindingDlg->SetAccel( m_hAccelerator );

			m_dlgBindingDlg->DoModal();


			if( m_dlgBindingDlg->ChangesMade() )
				UpdateUI_SCNBinds();


			SAFE_DELETE( m_dlgBindingDlg );
		}
	}
}

void CCookieMachineDlg::UpdateUI_SCNBinds()
{
	DWORD			uiMask	= TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	CSceneFile*		pScn	= (CSceneFile*)pBaseScene;
	StrVec			supported_types;
	ListStrVec_iter	iter;

	supported_types.push_back( "Background" );
	supported_types.push_back( "Action" );
	supported_types.push_back( "Light" );
	supported_types.push_back( "LayerGeom" );
	supported_types.push_back( "Floor" );
	supported_types.push_back( "CameraSelect" );
	int	image_idx[] = { 21, 17, 9, 18, 14, 19 };

	//remove current binds from cameras
	HTREEITEM	hCameras	= m_rtSceneContents.FindTreeItemByName( "Cameras" );
	HTREEITEM	hCamera		= m_rtSceneContents.GetChildItem( hCameras );
	HTREEITEM	hBinds		= NULL;
	HTREEITEM	hBind = NULL, hObj = NULL, hTemp = NULL;
	str_t		strCamName;

	while( hCamera )
	{
		hBind		= m_rtSceneContents.GetChildItem( hCamera );
		strCamName	= m_rtSceneContents.GetItemText( hCamera );
		hBinds		= m_rtSceneContents.FindTreeItemByName( "Binds", hCamera );
		hObj		= m_rtSceneContents.GetChildItem( hBinds );

		while( hBind )
		{
			if( m_rtSceneContents.GetItemText( hBind ) != "Binds" )
			{
				m_rtSceneContents.DeleteItem( hBind );
				hBind = m_rtSceneContents.GetChildItem( hCamera );
			}
			else
				hBind = m_rtSceneContents.GetNextSiblingItem( hBind );
		}

		while( hObj )
		{
			m_rtSceneContents.DeleteItem( hObj );
			hObj = m_rtSceneContents.GetChildItem( hBinds );
		}

		// add binded object nodes 
		//loop throught binds, looking for children objects
		for( unsigned int u = 0; u < supported_types.size() ; u++ )
		{
			iter = pScn->GetBinds()->begin();

			while( iter != pScn->GetBinds()->end() )
			{
				if( (*iter)[2] == "Camera" && (*iter)[1] == strCamName )
				{
					if( (*iter)[4] == supported_types[u] )
						hTemp = m_rtSceneContents.InsertItem( uiMask, _T( (*iter)[3].data() ), image_idx[u], image_idx[u], 0, 0, 0, hCamera, NULL );	

					//layer geometry attached to backs
					if( supported_types[u] == "Background" )
					{
						ListStrVec_iter iter2 = pScn->GetBinds()->begin();
						
						while( iter2 != pScn->GetBinds()->end() )
						{
							if( (*iter2)[2] == "Background" && (*iter2)[1] == (*iter)[3] && (*iter2)[4] == "LayerGeom" )
								m_rtSceneContents.InsertItem( uiMask, _T( (*iter2)[3].data() ), 18, 18, 0, 0, 0, hTemp, TVI_SORT );	

							iter2++;
						}
					}
				}

				iter++;
			}
		} // for( unsigned int u = 0; u < supported_types.size() ; u++ )


		iter = pScn->GetBinds()->begin();

		//OutputDebugString( ("--------------- " + strCamName + " --------------\n").data() );
		while( iter != pScn->GetBinds()->end() )
		{
			if( (*iter)[2] == "Camera" && (*iter)[1] == strCamName )
			{
				bool supp = false;

				for( unsigned int u = 0; u < supported_types.size() ; u++ )
				{
					if( (*iter)[4] == supported_types[u] )
					{
						supp = true;
						break;
					}
				} // for( unsigned int u = 0; u < supported_types.size() ; u++ )				

				if( supp == false )
				{
					m_rtSceneContents.InsertItem( uiMask, _T( ((*iter)[4] + "." + (*iter)[3]).data() ), 3, 3, 0, 0, 0, hBinds, TVI_SORT );
					//OutputDebugString( ((*iter)[4] + "." + (*iter)[3] + "\n").data() );
				}
			}

			iter++;
		}

		hCamera = m_rtSceneContents.GetNextSiblingItem( hCamera );
	}
}

void CCookieMachineDlg::OnMenuScnTextureAdd()
{
    if( !pBaseScene )
        return;

    if( pBaseScene->GetType() != FT_SCN )
        return;

	HTREEITEM	hTextures = m_rtSceneContents.FindTreeItemByName( "Textures" );
    CModelNameDlg* dlgModelName = new CModelNameDlg(this);

	dlgModelName->SetCaption( "Имя текстуры:" );
	dlgModelName->SetString( "" );
	dlgModelName->DoModal();
    
    str_t tex = dlgModelName->GetString().GetBuffer(0);

    StrVec* tex_list = ((CSceneFile*)pBaseScene)->GetTextures();

	bool found = false;

	for( unsigned int i = 0; i < tex_list->size(); i++ )
	{
		if( (*tex_list)[i] == tex )
		{
			found = true;
			break;
		}
	}

	if( !found )
	{
		tex_list->push_back( tex );
		m_rtSceneContents.InsertItem( TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE, tex.data(), 21, 21, 0, 0, 0, hTextures, TVI_SORT );
	}
	else
		MessageBox( "Текстура с таким ID уже существует!", "Внимание!", MB_OK | MB_ICONINFORMATION );

	SAFE_DELETE( dlgModelName );

}

void CCookieMachineDlg::OnMenuScnTexturesDeleteAll()
{
    if( !pBaseScene || pBaseScene->GetType() != FT_SCN )
        return;

	if( MessageBox( "Удалить все текстуры?", "Внимание!", MB_YESNO | MB_ICONQUESTION ) != IDYES )
		return;

	StrVec* tex_list = ((CSceneFile*)pBaseScene)->GetTextures();

	tex_list->clear();

	// remove null nodes from treeview
	HTREEITEM hTextures = m_rtSceneContents.FindTreeItemByName( "Textures" );
	HTREEITEM hTemp = m_rtSceneContents.GetChildItem( hTextures );

	while( hTemp )
	{
		m_rtSceneContents.DeleteItem( hTemp );
		hTemp = m_rtSceneContents.GetChildItem( hTextures );
	}
}

void CCookieMachineDlg::OnMenuScnFloorRecalc()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;


	CValidateDlg* dlgValidate = NULL;

	SetCursor( LoadCursor(NULL, IDC_WAIT) );

	//ask for cell size
	CModelNameDlg* dlgModelName = new CModelNameDlg(this);

	CString strTemp; 
	strTemp.Format( "%.4f", ((CSceneFile*)pBaseScene)->GetCellSize() );

	dlgModelName->SetCaption( "Cell size:" );

	if( ((CSceneFile*)pBaseScene)->GetCellSize() == 0.0f )	
		dlgModelName->SetString( "0.0500" );
	else
		dlgModelName->SetString( strTemp );

	dlgModelName->DoModal();

	CString str_type_id = dlgModelName->GetString().Trim();

	float fCellSize = atof( str_type_id.GetBuffer() );

	//show validation window
	if( !dlgValidate )
	{
		dlgValidate = new CValidateDlg(this);
		dlgValidate->SetMessageStr("Расчёт сетки проходимостей...");
		ProcessMessages();
		((CSceneFile*)pBaseScene)->SetProgressWnd( (CWnd*)dlgValidate );
	}
	
	((CSceneFile*)pBaseScene)->SetCellSize( fCellSize );
	((CSceneFile*)pBaseScene)->RecalcGrid();
	pBaseScene->Prepare( g_pd3dDevice );

	SetCursor( LoadCursor(NULL, IDC_ARROW) );

	SAFE_DELETE( dlgValidate );
	SAFE_DELETE( dlgModelName );
}

void CCookieMachineDlg::OnMenuScnFloorShowOnly()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	((CSceneFile*)pBaseScene)->ShowGridSelectors( false );
	((CSceneFile*)pBaseScene)->ShowGrid( false );

	ShowFloorOnly();
}

void CCookieMachineDlg::ShowFloorOnly()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	//hide lights and cams
	((CSceneFile*)pBaseScene)->ShowAllCameras( false );
	((CSceneFile*)pBaseScene)->ShowAllLights( false );
	((CSceneFile*)pBaseScene)->PrepareCamsLines( g_pd3dDevice );
	((CSceneFile*)pBaseScene)->PrepareLightsLines( g_pd3dDevice );

	CEfnMeshlist* meshlist = ((CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist());

	for( int i = 0; i < meshlist->GetMeshesCount(); i++ )
	{
		meshlist->GetMeshByIndex( i )->SetVisible( false );
	}

	//get list of floor meshes
	ListStrVec*		lBinds	= ((CSceneFile*)pBaseScene)->GetBinds();
	ListStrVec_iter lst_iter	= lBinds->begin();

	while( lst_iter != lBinds->end() )
	{
		StrVec	strVec = *lst_iter;

		if( strVec[2] == "Scene" && strVec[4] == "Floor"  )
		{
			for( int i = 0; i < meshlist->GetMeshesCount(); i++ )
			{
				str_t nm = strVec[3];
				if( meshlist->GetMeshByIndex( i )->GetName() == nm )
					meshlist->GetMeshByIndex( i )->SetVisible( true );
			}
		}

		lst_iter++;
	}
}

void CCookieMachineDlg::OnMenuScnFloorShowGrid()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	ShowFloorOnly();

	if( ((CSceneFile*)pBaseScene)->IsGridVisible() )
	{
		((CSceneFile*)pBaseScene)->ShowGrid( false );
	}
	else
	{
		((CSceneFile*)pBaseScene)->ShowGrid( true );
		pBaseScene->Prepare( g_pd3dDevice );
	}
}

void CCookieMachineDlg::OnMenuScnFloorShowSelector()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	ShowFloorOnly();

	if( ((CSceneFile*)pBaseScene)->IsGridSelectorsVisible() )
	{
		((CSceneFile*)pBaseScene)->ShowGridSelectors( false );
	}
	else
	{
		((CSceneFile*)pBaseScene)->ShowGridSelectors( true );
		pBaseScene->Prepare( g_pd3dDevice );
	}
}

void CCookieMachineDlg::OnMenuScnFloorSelectorShowOnly()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	((CSceneFile*)pBaseScene)->ShowGridSelectors( false );
	((CSceneFile*)pBaseScene)->ShowGrid( false );

	CEfnMeshlist* meshlist = ((CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist());

	for( int i = 0; i < meshlist->GetMeshesCount(); i++ )
	{
		meshlist->GetMeshByIndex( i )->SetVisible( false );
	}

	//get list of floor meshes
	ListStrVec*		lBinds	= ((CSceneFile*)pBaseScene)->GetBinds();
	ListStrVec_iter lst_iter	= lBinds->begin();

	while( lst_iter != lBinds->end() )
	{
		StrVec	strVec = *lst_iter;

		if( strVec[4] == "CameraSelect"  )
		{
			for( int i = 0; i < meshlist->GetMeshesCount(); i++ )
			{
				str_t nm = strVec[3];
				if( meshlist->GetMeshByIndex( i )->GetName() == nm )
					meshlist->GetMeshByIndex( i )->SetVisible( true );
			}
		}

		lst_iter++;
	}
}

//void CCookieMachineDlg::OnMenuMeshlistNullAttach()
//{
//	if( !pBaseScene )
//		return;
//
//	if( pBaseScene->GetType() != FT_EFN_MESHLIST )
//		return;
//
//	HTREEITEM hSelected = m_rtSceneContents.GetSelectedItem();
////	HTREEITEM hTempItem;
//
//	CBaseScene* pAttachment	= NULL;
//
//	str_t		szFilter	= "EFN Files (*.efn)|*.efn|";
//	CString	temp		= "*.efn";
//
//	//show open file dialog
//	CFileDialog *dlg = new CFileDialog( TRUE,	temp.GetBuffer(0), 0,	OFN_EXPLORER, szFilter.data(), NULL, sizeof(OPENFILENAME) );
//	dlg->m_pOFN->lpstrInitialDir = m_appSettings.FilesPath;
//
//	//user confirmed "open file dialog"
//	if( dlg->DoModal() == IDOK )
//	{
//		//get file dir
//		str_t file_dir		= dlg->GetPathName().Left( dlg->GetPathName().GetLength() - dlg->GetFileName().GetLength() );
//		//get filename
//		str_t file_name	= dlg->GetFileName();
//
//
//		if( !FileExists( file_dir + file_name ) )
//		{
//			MessageBox( "Файл не найден!", "Ошибка.", MB_OK | MB_ICONWARNING ); 
//			return;
//		}
//
//		if( GetEfnFiletype( file_dir + file_name ) != EF_MESHLIST )
//		{
//			MessageBox( "Тип файла должен быть Meshlist EFN!", "Ошибка.", MB_OK | MB_ICONWARNING ); 
//			return;
//		}
//
//		CString				temp;
//		bool					bIsIncorrectFileVersion		= false;
//		CValidateDlg*	dlgValidate						= NULL;
//
//
//		CString extention = dlg->GetFileExt().GetBuffer();
//		extention.MakeUpper();
//
//		//show validation window
//		if( !dlgValidate )
//		{
//			dlgValidate = new CValidateDlg( this );
//			dlgValidate->SetMessageStr( "Проверка файла..." );
//			ProcessMessages();
//		}
//
//		SetCursor( LoadCursor(NULL, IDC_WAIT) );
//
//
//		if( extention == "EFN" )
//		{
//			CEfnCharacter	ch;
//			DWORD			file_version	= GetEfnVersion( file_dir + file_name );
//
//			if( file_version == EFN_CURR_VERSION )
//			{
//				//check file prefix
//				CEfnMeshlist* efn_meshlist = new CEfnMeshlist;
//
//				if( efn_meshlist->LoadFromFile( file_dir + file_name ) )
//				{
//					pAttachment = efn_meshlist;
//					pAttachment->SetPaths( file_dir, file_name );
//
//
//					//prepare data for rendering
//					pAttachment->Prepare( g_pd3dDevice );
//
//					CNULL* nl = ((CEfnMeshlist*)pBaseScene)->GetMeshByName( "Nulls" )->GetNullByName( m_rtSceneContents.GetItemText(hSelected).GetBuffer() );
//
//					SAFE_DELETE( nl->attachment );
//					nl->attachment = pAttachment;
//
//					//add detailed attachment info to tree view
//					//CString tmp;
//					//
//					//tmp.Format( "File: %s", pAttachment->GetFilename().data() );
//					//hTempItem = m_rtSceneContents.InsertItem( TVIF_TEXT, tmp.GetBuffer(), 0, 0, 0, 0, 0, hSelected, NULL);
//					//m_rtSceneContents.SetItemImage( hTempItem, 3, 3 );
//
//					//tmp.Format( "Texture: %s", ((CEfnCharacter*)pAttachment)->GetTexturePath().data() );
//					//hTempItem = m_rtSceneContents.InsertItem( TVIF_TEXT, tmp.GetBuffer(), 0, 0, 0, 0, 0, hSelected, NULL);
//					//m_rtSceneContents.SetItemImage( hTempItem, 3, 3 );
//
//					//tmp.Format( "Polycount: %i", ((CEfnCharacter*)pAttachment)->GetTrianglesCount() );
//					//hTempItem = m_rtSceneContents.InsertItem( TVIF_TEXT, tmp.GetBuffer(), 0, 0, 0, 0, 0, hSelected, NULL);
//					//m_rtSceneContents.SetItemImage( hTempItem, 3, 3 );
//				}
//				else
//				{
//					// load failed -- cleanup
//					SAFE_DELETE( efn_meshlist ); 
//				}
//			}
//			else
//			{
//				CString err_str;
//				str_t curr_vers_str		= GetEfnVersionAsString( EFN_CURR_VERSION );
//				str_t file_vers_str		= GetEfnVersionAsString( file_version );
//
//				err_str.Format("Incorrect EFN version: [%s], current version is: [%s] \n\rFile can not be opened.", file_vers_str.c_str(), curr_vers_str.c_str() );
//
//				MessageBox( err_str, "Warning!", MB_OK | MB_ICONEXCLAMATION );
//
//				bIsIncorrectFileVersion = true;
//			}
//		} // 	else if (extention == "EFN")
//		
//		//restore cursor
//		SetCursor(LoadCursor(NULL, IDC_ARROW));
//
//		SAFE_DELETE( dlg );
//		SAFE_DELETE( dlgValidate );
//		return;
//	}
//
//	SAFE_DELETE( dlg );
//}

//void CCookieMachineDlg::OnMenuMeshlistNullDetach()
//{
//	// TODO: Add your command handler code here
//}

void CCookieMachineDlg::OnMenuSceneCamsObjectDelete()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	HTREEITEM	hSelected, hParent, hParentParent;
	CString		strParentText, hSelItemText;
	str_t		strItem, strItemPar, strItemParPar, strType;
	bool		bLgAttachedToBack = false;
	CSceneFile* pScene = (CSceneFile*)pBaseScene;

	hSelected		= m_rtSceneContents.GetSelectedItem();
	hSelItemText	= m_rtSceneContents.GetItemText( hSelected );
	strItem			= hSelItemText.GetBuffer();

	//get image from selected tree node and determine object type
	int image_idx = 0, par_image_idx = 0;
	m_rtSceneContents.GetItemImage( hSelected, image_idx, image_idx );

	if( hSelected >= 0 )
		hParent = m_rtSceneContents.GetParentItem( hSelected );

	if( hParent >= 0 )
	{
		m_rtSceneContents.GetItemImage( hParent, par_image_idx, par_image_idx );
		strParentText = m_rtSceneContents.GetItemText( hParent );
		strItemPar = strParentText.GetBuffer();
		hParentParent = m_rtSceneContents.GetParentItem( hParent );
	}

	if( hParentParent >= 0 )
	{
		strItemParPar = m_rtSceneContents.GetItemText( hParentParent ).GetBuffer();
	}


	if( image_idx == 21 )
	{
		strType = "Background";

		if( m_rtSceneContents.GetChildItem( hSelected ) != NULL )
			bLgAttachedToBack = true;
	}

	if( image_idx == 17 )
		strType = "Action"; 

	if( image_idx == 9 )
		strType = "Light"; 

	if( image_idx == 18 )
		strType = "LayerGeom"; 

	if( image_idx == 14 )
		strType = "Floor";

	if( image_idx == 19 )
		strType = "CameraSelect";


	if( bLgAttachedToBack )
	{
		if( MessageBox( ( "Удалить объект [" + strItem + "] и зависящие от него Layer Geometry?" ).data(), "Внимание!", MB_YESNO ) != IDYES )
			return;

		ListStrVec* binds	= ((CSceneFile*)pBaseScene)->GetBinds();
		ListStrVec_iter iter = binds->begin();

		while( iter != binds->end() )
		{		
			if( (*iter)[1] == strItem && (*iter)[2] == "Background" && (*iter)[4] == "LayerGeom" )
			{
				DeleteFuncEntriesByKey( pScene->GetFuncs(), atoi((*iter)[0].data()) );
				binds->erase( iter );
				iter = binds->begin();
			}

			iter++;
		}
	}
	else
	{
		if( MessageBox( ( "Удалить объект [" + strItem + "] ?" ).data(), "Внимание!", MB_YESNO ) != IDYES )
			return;
	}

	ListStrVec* binds	= ((CSceneFile*)pBaseScene)->GetBinds();
	ListStrVec_iter iter = binds->begin();

	while( iter != binds->end() )
	{
		if( par_image_idx == 21 && image_idx == 18 )
		{
			if( (*iter)[1] == strItemPar && (*iter)[2] == "Background" && (*iter)[3] == strItem && (*iter)[4] == "LayerGeom" )
			{
				DeleteFuncEntriesByKey( pScene->GetFuncs(), atoi((*iter)[0].data()) );
				binds->erase( iter );

				hParent	= m_rtSceneContents.FindTreeItemByName( "Cameras" );
				hParent	= m_rtSceneContents.FindTreeItemByName( strItemParPar, hParent );
				hParent = m_rtSceneContents.FindTreeItemByName( strItemPar, hParent );
				hParent = m_rtSceneContents.FindTreeItemByName( strItem, hParent );

				m_rtSceneContents.DeleteItem( hParent );

				break;
			}
		}
		else
		{
			if( (*iter)[1] == strItemPar && (*iter)[2] == "Camera" && (*iter)[3] == strItem && (*iter)[4] == strType )
			{
				DeleteFuncEntriesByKey( pScene->GetFuncs(), atoi((*iter)[0].data()) );
				binds->erase( iter );

				hParent	= m_rtSceneContents.FindTreeItemByName( "Cameras" );
				hParent	= m_rtSceneContents.FindTreeItemByName( strItemPar, hParent );
				hParent = m_rtSceneContents.FindTreeItemByName( strItem, hParent );

				m_rtSceneContents.DeleteItem( hParent );

				break;
			}
		}

		iter++;
	}
}

void CCookieMachineDlg::OnMenuSceneCamsObjectScript()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	HTREEITEM hSelected, hParent, hParentParent;
	CString strParentText, hSelItemText;
	str_t strItem, strItemPar, strType;

	hSelected		= m_rtSceneContents.GetSelectedItem();
	hSelItemText	= m_rtSceneContents.GetItemText( hSelected );
	strItem			= hSelItemText.GetBuffer();

	//get image from selected tree node and determine object type
	int image_idx = 0;
	m_rtSceneContents.GetItemImage( hSelected, image_idx, image_idx );

	if( hSelected >= 0 )
		hParent = m_rtSceneContents.GetParentItem( hSelected );

	if( hParent >= 0 )
	{
		strParentText = m_rtSceneContents.GetItemText( hParent );
		strItemPar = strParentText.GetBuffer();
		hParentParent = m_rtSceneContents.GetParentItem( hParent );
	}


	if( image_idx == 21 )
		strType = "Background"; 

	if( image_idx == 17 )
		strType = "Action"; 

	if( image_idx == 9 )
		strType = "Light"; 

	if( image_idx == 18 )
		strType = "LayerGeom"; 

	if( image_idx == 14 )
		strType = "Floor"; 

	if( image_idx == 19 )
		strType = "CameraSelect";

	
	//prepare last edited item and assign it to scene
	CSceneFile*	scene = ((CSceneFile*)pBaseScene);
	scene->m_strLastEditedObj = strType + "." + strItem;

	ShowScriptsWindow();
}

void CCookieMachineDlg::OnMenuSceneCamsObjectBind()
{
	StrVec vecSceneData;

	if( pBaseScene && pBaseScene->GetType() == FT_SCN )
	{
		if( !m_dlgBindingDlg )
		{
			HTREEITEM hSelected, hParent, hParentParent;
			CString strParentText, hSelItemText;
			str_t strItem, strItemPar, strType;

			hSelected		= m_rtSceneContents.GetSelectedItem();
			hSelItemText	= m_rtSceneContents.GetItemText( hSelected );
			strItem = hSelItemText.GetBuffer();

			//get image from selected tree node and determine object type
			int image_idx = 0, par_image_idx = 0;
			m_rtSceneContents.GetItemImage( hSelected, image_idx, image_idx );

			if( hSelected >= 0 )
				hParent = m_rtSceneContents.GetParentItem( hSelected );

			if( hParent >= 0 )
			{
				m_rtSceneContents.GetItemImage( hParent, par_image_idx, par_image_idx );
				strParentText = m_rtSceneContents.GetItemText( hParent );
				strItemPar = strParentText.GetBuffer();
				hParentParent = m_rtSceneContents.GetParentItem( hParent );
			}


			if( image_idx == 21 )
				strType = "Background";

			if( image_idx == 17 )
				strType = "Action"; 

			if( image_idx == 9 )
				strType = "Light"; 

			if( image_idx == 18 )
				strType = "LayerGeom"; 

			if( image_idx == 14 )
				strType = "Floor"; 

			if( image_idx == 19 )
				strType = "CameraSelect";

			//build item
			StrVec bind_item;
			bind_item.push_back( strItemPar );

			if( par_image_idx == 21 )
				bind_item.push_back( "Background" );
			else
				bind_item.push_back( "Camera" );

			bind_item.push_back( strItem );
			bind_item.push_back( strType );

			CSceneFile*	scene = ((CSceneFile*)pBaseScene);
			CString		temp;

			//collect data from the scene
			for( int i = 0 ; i < scene->GetLightsCount(); i++ )
			{
				temp.Format( "Light.%s", scene->GetLightByIndex( i )->name.data() );
				vecSceneData.push_back( temp.GetBuffer() );
			}

			for( int i = 0 ; i < scene->GetCamerasCount(); i++ )
			{
				temp.Format( "Camera.%s", scene->GetCameraByIndex( i )->name.data() );
				vecSceneData.push_back( temp.GetBuffer() );
			}

			CEfnMeshlist* meshlist = (CEfnMeshlist*)scene->GetMeshlist();

			if( meshlist )
			{
				for( int i = 0 ; i < meshlist->GetMeshesCount(); i++ )
				{
					temp.Format( "Mesh.%s", meshlist->GetMeshByIndex( i )->GetName().data() );
					vecSceneData.push_back( temp.GetBuffer() );
				}
			}

			StrVec tex_vec = *(scene->GetTextures());
			for( unsigned int i = 0; i < tex_vec.size(); i++ )
			{
				temp.Format( "Texture.%s", tex_vec[i].data() );
				vecSceneData.push_back( temp.GetBuffer() );
			}

			temp.Format( "Scene.%s", scene->GetSceneID().data() );
			vecSceneData.push_back( temp.GetBuffer() );


			StrVec scnTypes;
			GetTypesList( scnTypes );


			StrVec* m_CustomObjects	= ((CSceneFile*)pBaseScene)->GetCustObjList();
			StrVec* m_CustomTypes	= ((CSceneFile*)pBaseScene)->GetCustTypesList();

			if( m_appSettings.ExpertMode )
			{
				for( unsigned int i = 0; i < m_CustomObjects->size(); i++ )
				{
					temp.Format( "Object.%s", (*m_CustomObjects)[i].data() );
					vecSceneData.push_back( temp.GetBuffer() );
				}

				for( unsigned int i = 0; i < m_CustomTypes->size(); i++ )
				{
					scnTypes.push_back( (*m_CustomTypes)[i] );
				}
			}


			m_dlgBindingDlg = new CBindsDlg();
			m_dlgBindingDlg->LinkBinds( ((CSceneFile*)pBaseScene)->GetBinds() );
			m_dlgBindingDlg->LinkSceneData( &vecSceneData );
			m_dlgBindingDlg->LinkSceneTypes( &scnTypes );
			m_dlgBindingDlg->SetParent( this );
			m_dlgBindingDlg->SetAccel( m_hAccelerator );
			m_dlgBindingDlg->SetSelection( &bind_item );
			m_dlgBindingDlg->DoModal();
				

			if( m_dlgBindingDlg->ChangesMade() )
				UpdateUI_SCNBinds();


			SAFE_DELETE( m_dlgBindingDlg );

			hParentParent	= m_rtSceneContents.FindTreeItemByName( "Cameras" );
			hParent			= m_rtSceneContents.FindTreeItemByName( strItemPar, hParentParent );
			hSelected		= m_rtSceneContents.FindTreeItemByName( strItem, hParent );

			m_rtSceneContents.Expand( hParentParent, TVE_EXPAND );
			m_rtSceneContents.Expand( hParent, TVE_EXPAND );
			m_rtSceneContents.SelectItem( hSelected );
		}
	}
}

void CCookieMachineDlg::OnMenuScnMeshesShow()
{
 	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	CSceneFile*		pScene		= ((CSceneFile*)pBaseScene);
	CEfnMeshlist*	pMeshlist	= (CEfnMeshlist*)pScene->GetMeshlist();

	pMeshlist->ShowAllMeshes( true );

	HTREEITEM hMeshes	= m_rtSceneContents.FindTreeItemByName( "Meshes" );
	HTREEITEM hMesh		= m_rtSceneContents.GetChildItem( hMeshes );

	m_rtSceneContents.SetItemImage( hMesh, 0, 0 );

	while( hMesh )
	{
		hMesh = m_rtSceneContents.GetNextSiblingItem( hMesh );
		m_rtSceneContents.SetItemImage( hMesh, 0, 0 );
	}
}

void CCookieMachineDlg::OnMenuScnMeshesHide()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	CSceneFile*		pScene		= ((CSceneFile*)pBaseScene);
	CEfnMeshlist*	pMeshlist	= (CEfnMeshlist*)pScene->GetMeshlist();

	pMeshlist->ShowAllMeshes( false );

	HTREEITEM hMeshes	= m_rtSceneContents.FindTreeItemByName( "Meshes" );
	HTREEITEM hMesh		= m_rtSceneContents.GetChildItem( hMeshes );

	m_rtSceneContents.SetItemImage( hMesh, 4, 4 );

	while( hMesh )
	{
		hMesh = m_rtSceneContents.GetNextSiblingItem( hMesh );
		m_rtSceneContents.SetItemImage( hMesh, 4, 4 );
	}
}

void CCookieMachineDlg::OnMenuScnMeshShow()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	HTREEITEM	hSelected;
	CString			hSelItemText;
	str_t				strItem;

	hSelected		= m_rtSceneContents.GetSelectedItem();
	hSelItemText	= m_rtSceneContents.GetItemText( hSelected );
	strItem			= hSelItemText.GetBuffer();

	CSceneFile*		pScene		= ((CSceneFile*)pBaseScene);
	CEfnMeshlist*	pMeshlist	= (CEfnMeshlist*)pScene->GetMeshlist();

	if( strItem != "Nulls" )
		m_rtSceneContents.SetItemImage( hSelected, 0, 0 );

	pMeshlist->ShowMeshes( strItem, true );
}

void CCookieMachineDlg::OnMenuScnMeshHide()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	HTREEITEM	hSelected;
	CString			hSelItemText;
	str_t				strItem;

	hSelected		= m_rtSceneContents.GetSelectedItem();
	hSelItemText	= m_rtSceneContents.GetItemText( hSelected );
	strItem			= hSelItemText.GetBuffer();

	CSceneFile*		pScene		= ((CSceneFile*)pBaseScene);
	CEfnMeshlist*	pMeshlist	= (CEfnMeshlist*)pScene->GetMeshlist();

	if( strItem != "Nulls" )
		m_rtSceneContents.SetItemImage( hSelected, 4, 4 );

	pMeshlist->ShowMeshes( strItem, false );
}

void CCookieMachineDlg::OnMenuScnCamerasShow()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	((CSceneFile*)pBaseScene)->ShowAllCameras( true );
	((CSceneFile*)pBaseScene)->PrepareCamsLines( g_pd3dDevice );


	HTREEITEM	hCameras	= m_rtSceneContents.FindTreeItemByName( "Cameras" );
	HTREEITEM	hCam		= m_rtSceneContents.GetChildItem( hCameras );

	m_rtSceneContents.SetItemImage( hCam, 5, 5 );

	while( hCam )
	{
		hCam = m_rtSceneContents.GetNextSiblingItem( hCam );
		m_rtSceneContents.SetItemImage( hCam, 5, 5 );
	}
}

void CCookieMachineDlg::OnMenuScnCamerasHide()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	((CSceneFile*)pBaseScene)->ShowAllCameras( false );
	((CSceneFile*)pBaseScene)->PrepareCamsLines( g_pd3dDevice );

	HTREEITEM	hCameras	= m_rtSceneContents.FindTreeItemByName( "Cameras" );
	HTREEITEM	hCam		= m_rtSceneContents.GetChildItem( hCameras );

	m_rtSceneContents.SetItemImage( hCam, 11, 11 );

	while( hCam )
	{
		hCam = m_rtSceneContents.GetNextSiblingItem( hCam );
		m_rtSceneContents.SetItemImage( hCam, 11, 11 );
	}
}

void CCookieMachineDlg::OnMenuScnCameraShow()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	HTREEITEM	hSelected;
	CString		hSelItemText;
	str_t		strItem;

	hSelected		= m_rtSceneContents.GetSelectedItem();
	hSelItemText	= m_rtSceneContents.GetItemText( hSelected );
	strItem			= hSelItemText.GetBuffer();

	CCAMERA2* cam = ((CSceneFile*)pBaseScene)->GetCameraByID( hSelItemText.GetBuffer() );

	if( cam )
	{
		cam->visible = true;
		m_rtSceneContents.SetItemImage( hSelected, 5, 5 );
		((CSceneFile*)pBaseScene)->PrepareCamsLines( g_pd3dDevice );
	}
}

void CCookieMachineDlg::OnMenuScnCameraHide()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	HTREEITEM	hSelected;
	CString			hSelItemText;
	str_t				strItem;

	hSelected		= m_rtSceneContents.GetSelectedItem();
	hSelItemText	= m_rtSceneContents.GetItemText( hSelected );
	strItem			= hSelItemText.GetBuffer();

	CCAMERA2* cam = ((CSceneFile*)pBaseScene)->GetCameraByID( hSelItemText.GetBuffer() );

	if( cam )
	{
		cam->visible = false;
		m_rtSceneContents.SetItemImage( hSelected, 11, 11 );
		((CSceneFile*)pBaseScene)->PrepareCamsLines( g_pd3dDevice );
	}
}

void CCookieMachineDlg::OnMenuScnCameraObjects()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	//get cam name
	HTREEITEM	hSelected, hCameras;
	CString			hSelItemText;
	str_t				strItem;

	hSelected		= m_rtSceneContents.GetSelectedItem();
	hSelItemText	= m_rtSceneContents.GetItemText( hSelected );
	strItem			= hSelItemText.GetBuffer();


	CSceneFile*		pScene		= ((CSceneFile*)pBaseScene);
	CEfnMeshlist*	pMeshlist	= (CEfnMeshlist*)pScene->GetMeshlist();
	ListStrVec*		pBinds		= pScene->GetBinds();
	ListStrVec_iter	iter			= pBinds->begin();


	//hide everything
	OnMenuScnMeshesHide();
	OnMenuScnCamerasHide();
	OnMenuScnLightsHide();

	StrVec meshes_list;
	StrVec lights_list;

	while( iter != pBinds->end() )
	{
		if( (*iter)[1] == strItem && (*iter)[2] == "Camera" )
		{
			if( (*iter)[4] == "Floor" || (*iter)[4] == "CameraSelect" || (*iter)[4] == "LayerGeom" || (*iter)[4] == "Action" )
				meshes_list.push_back( (*iter)[3] );

			if( (*iter)[4] == "Light" )
				lights_list.push_back( (*iter)[3] );
		}
		iter++;
	}


	CString cmb_cam_id;

	m_cmbCameras.GetLBText( m_cmbCameras.GetCurSel(), cmb_cam_id );

	pScene->SetSelectedCamera( strItem );

	//show camera
	if( cmb_cam_id == DEFAULT_CAMERA_ID )
		pScene->GetCameraByID( strItem )->visible = true;
	else
		pScene->GetCameraByID( strItem )->visible = false;

	//show meshes
	for( unsigned int i = 0; i < meshes_list.size(); i++ )
		pMeshlist->ShowMeshes( meshes_list[i], true );

	//show lights
	for( unsigned int i = 0; i < lights_list.size(); i++ )
		pScene->GetLightByID( lights_list[i] )->visible = true;

	pScene->PrepareCamsLines( g_pd3dDevice );
	pScene->PrepareLightsLines( g_pd3dDevice );

	UpdateUI();

	hCameras			= m_rtSceneContents.FindTreeItemByName( "Cameras" );
	hSelected			= m_rtSceneContents.FindTreeItemByName( hSelItemText, hCameras );

	m_rtSceneContents.Expand( hSelected, TVE_TOGGLE );
	m_rtSceneContents.Expand( hCameras, TVE_TOGGLE );
	m_rtSceneContents.SelectItem( hCameras );
}

void CCookieMachineDlg::OnMenuScnLightsShow()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	((CSceneFile*)pBaseScene)->ShowAllLights( true );
	((CSceneFile*)pBaseScene)->PrepareLightsLines( g_pd3dDevice );

	HTREEITEM	hLights	= m_rtSceneContents.FindTreeItemByName( "Lights" );
	HTREEITEM	hLight	= m_rtSceneContents.GetChildItem( hLights );

	m_rtSceneContents.SetItemImage( hLight, 9, 9 );

	while( hLight )
	{
		hLight = m_rtSceneContents.GetNextSiblingItem( hLight );
		m_rtSceneContents.SetItemImage( hLight, 9, 9 );
	}
}

void CCookieMachineDlg::OnMenuScnLightsHide()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	((CSceneFile*)pBaseScene)->ShowAllLights( false );
	((CSceneFile*)pBaseScene)->PrepareLightsLines( g_pd3dDevice );

	HTREEITEM	hLights	= m_rtSceneContents.FindTreeItemByName( "Lights" );
	HTREEITEM	hLight	= m_rtSceneContents.GetChildItem( hLights );

	m_rtSceneContents.SetItemImage( hLight, 12, 12 );

	while( hLight )
	{
		hLight = m_rtSceneContents.GetNextSiblingItem( hLight );
		m_rtSceneContents.SetItemImage( hLight, 12, 12 );
	}
}

void CCookieMachineDlg::OnMenuScnLightShow()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	HTREEITEM hSelected = m_rtSceneContents.GetSelectedItem();
	CString strNodeText;
	CLIGHT *light = NULL;

	//get currently selected treenode name
	if( hSelected >= 0 )
	{
		strNodeText = m_rtSceneContents.GetItemText( hSelected );

		//get mesh by name
		light = ( (CSceneFile*)pBaseScene )->GetLightByID( strNodeText.GetBuffer(0) );

		if( light != NULL )
		{
			m_rtSceneContents.SetItemImage( hSelected, 9, 9 );

			light->visible = true;

			( (CSceneFile*)pBaseScene )->PrepareLightsLines( g_pd3dDevice );
		}
	}
}

void CCookieMachineDlg::OnMenuScnLightHide()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	HTREEITEM hSelected = m_rtSceneContents.GetSelectedItem();
	CString strNodeText;
	CLIGHT *light = NULL;

	//get currently selected treenode name
	if( hSelected >= 0 )
	{
		strNodeText = m_rtSceneContents.GetItemText( hSelected );

		//get mesh by name
		light = ( (CSceneFile*)pBaseScene )->GetLightByID( strNodeText.GetBuffer(0) );

		if( light != NULL )
		{
			m_rtSceneContents.SetItemImage( hSelected, 12, 12 );

			light->visible = false;

			( (CSceneFile*)pBaseScene )->PrepareLightsLines( g_pd3dDevice );
		}
	}
}

void CCookieMachineDlg::OnBnClickedBtnShowall()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() == FT_SCN )
	{
		OnMenuScnCurvesShow();
		OnMenuScnCamerasShow();
		OnMenuScnLightsShow();
		OnMenuScnNullsShow();
		OnMenuScnMeshesShow();
	}
	else if( pBaseScene->GetType() == FT_EFN_MESHLIST )
	{
		OnMenuMeshlistMeshesShow();
		OnMenuMeshlistNullsShow();
	}
	else
	{}
}

void CCookieMachineDlg::OnBnClickedBtnHideall()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() == FT_SCN )
	{
		OnMenuScnCurvesHide();
		OnMenuScnCamerasHide();
		OnMenuScnLightsHide();
		OnMenuScnNullsHide();
		OnMenuScnMeshesHide();
	}
	else if( pBaseScene->GetType() == FT_EFN_MESHLIST )
	{
		OnMenuMeshlistMeshesHide();
		OnMenuMeshlistNullsHide();
	}
	else
	{}
}

void CCookieMachineDlg::OnMenuScnCamZoomnear()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	CModelNameDlg *dlgModelName = new CModelNameDlg(this);
	HTREEITEM hSelected = m_rtSceneContents.GetSelectedItem();
	HTREEITEM hProp, hCam;
	hProp = m_rtSceneContents.GetParentItem( hSelected );
	hCam = m_rtSceneContents.GetParentItem( hProp );

	str_t		cam_id = m_rtSceneContents.GetItemText( hCam ).GetBuffer();

	CCAMERA2* cam = ((CSceneFile*)pBaseScene)->GetCameraByID( cam_id );

	CString value;

	value.Format( "%.1f", cam->zoom_near );

	dlgModelName->SetCaption( "Zoom near:" );
	dlgModelName->SetString( value );
	dlgModelName->DoModal();

	cam->zoom_near = atof( dlgModelName->GetString().GetBuffer() );

	value.Format( "Zoom near: %.1f", cam->zoom_near );
	m_rtSceneContents.SetItemText( hSelected, value );

	SAFE_DELETE( dlgModelName );
}

void CCookieMachineDlg::OnMenuScnCamZoomfar()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	CModelNameDlg *dlgModelName = new CModelNameDlg(this);
	HTREEITEM hSelected = m_rtSceneContents.GetSelectedItem();
	HTREEITEM hProp, hCam;
	hProp = m_rtSceneContents.GetParentItem( hSelected );
	hCam = m_rtSceneContents.GetParentItem( hProp );

	str_t		cam_id = m_rtSceneContents.GetItemText( hCam ).GetBuffer();

	CCAMERA2* cam = ((CSceneFile*)pBaseScene)->GetCameraByID( cam_id );

	CString value;

	value.Format( "%.1f", cam->zoom_far );

	dlgModelName->SetCaption( "Zoom far:" );
	dlgModelName->SetString( value );
	dlgModelName->DoModal();

	cam->zoom_far = atof( dlgModelName->GetString().GetBuffer() );

	value.Format( "Zoom far: %.1f", cam->zoom_far );
	m_rtSceneContents.SetItemText( hSelected, value );

	SAFE_DELETE( dlgModelName );
}

void CCookieMachineDlg::OnMenuScnLightDiffuse()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	char szTemp[128];

	HTREEITEM hSelected = m_rtSceneContents.GetSelectedItem();
	HTREEITEM hLight;
	hLight = m_rtSceneContents.GetParentItem( hSelected );
	str_t light_id = m_rtSceneContents.GetItemText( hLight ).GetBuffer();

	CLIGHT* light = ((CSceneFile*)pBaseScene)->GetLightByID( light_id );

	CColorDlg* pDlg = new CColorDlg;
	pDlg->caption = "Diffuse color:";
	pDlg->alpha_enabled = false;
	pDlg->color = light->diffuse;
	pDlg->DoModal();

	light->diffuse = pDlg->color;

	//update tree item
	sprintf( szTemp, "Diffuse:  r:%i  g:%i  b:%i", (int)( light->diffuse.r ), (int)( light->diffuse.g ), (int)( light->diffuse.b ) );
	m_rtSceneContents.SetItemText( hSelected, szTemp );

	SAFE_DELETE( pDlg );
}

void CCookieMachineDlg::OnMenuScnLightAmb()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	char szTemp[128];

	HTREEITEM hSelected = m_rtSceneContents.GetSelectedItem();
	HTREEITEM hLight;
	hLight = m_rtSceneContents.GetParentItem( hSelected );
	str_t light_id = m_rtSceneContents.GetItemText( hLight ).GetBuffer();

	CLIGHT* light = ((CSceneFile*)pBaseScene)->GetLightByID( light_id );

	CColorDlg* pDlg = new CColorDlg;
	pDlg->caption = "Ambient color:";
	pDlg->alpha_enabled = false;
	pDlg->color = light->ambient;
	pDlg->DoModal();

	light->ambient = pDlg->color;

	//update tree item
	sprintf( szTemp, "Ambient:  r:%i  g:%i  b:%i", (int)( light->ambient.r ), (int)( light->ambient.g ), (int)( light->ambient.b ) );
	m_rtSceneContents.SetItemText( hSelected, szTemp );

	SAFE_DELETE( pDlg );
}

void CCookieMachineDlg::OnMenuScnMeshVcol()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	char szTemp[128];

	HTREEITEM hSelected = m_rtSceneContents.GetSelectedItem();
	HTREEITEM hLight;
	hLight = m_rtSceneContents.GetParentItem( hSelected );
	str_t mesh_id = m_rtSceneContents.GetItemText( hLight ).GetBuffer();

	CSceneFile*		scene		= (CSceneFile*)pBaseScene;
	CEfnMeshlist*	meshlist	= (CEfnMeshlist*)scene->GetMeshlist();

	int a, r, g, b;
	DWORD color = meshlist->GetMeshByName( mesh_id )->GetVColor();
	GetARGBFromDWORD( a, r, g, b, color );

	CCOLOR vcol;
	vcol.a = a;
	vcol.r = r;
	vcol.g = g;
	vcol.b = b;

	CColorDlg* pDlg = new CColorDlg;
	pDlg->caption = "Vertex color:";
	pDlg->alpha_enabled = true;
	pDlg->color = vcol;
	pDlg->DoModal();

	vcol = pDlg->color;

	//update tree item
	sprintf( szTemp, "VColor:  a:%i  r:%i  g:%i  b:%i", (int)( vcol.a ), (int)( vcol.r ), (int)( vcol.g ), (int)( vcol.b ) );

	meshlist->GetMeshByName( mesh_id )->SetVColor( D3DCOLOR_ARGB( (int)( vcol.a ), (int)( vcol.r ), (int)( vcol.g ), (int)( vcol.b ) ) );

	m_rtSceneContents.SetItemText( hSelected, szTemp );

	SAFE_DELETE( pDlg );
}

void CCookieMachineDlg::OnMenuScnLightMultiplier()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	CModelNameDlg *dlgModelName = new CModelNameDlg(this);
	HTREEITEM hSelected = m_rtSceneContents.GetSelectedItem();
	HTREEITEM hLight;
	hLight = m_rtSceneContents.GetParentItem( hSelected );

	str_t light_id = m_rtSceneContents.GetItemText( hLight ).GetBuffer();

	CLIGHT* light = ((CSceneFile*)pBaseScene)->GetLightByID( light_id );

	CString value;

	value.Format( "%.1f", light->multiplier );

	dlgModelName->SetCaption( "Multiplier:" );
	dlgModelName->SetString( value );
	dlgModelName->DoModal();

	light->multiplier = atof( dlgModelName->GetString().GetBuffer() );

	value.Format( "Multiplier: %.1f", light->multiplier );

	m_rtSceneContents.SetItemText( hSelected, value );

	SAFE_DELETE( dlgModelName );
}

void CCookieMachineDlg::OnMenuScnLightAttenuation()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	CModelNameDlg *dlgModelName = new CModelNameDlg(this);
	HTREEITEM hSelected = m_rtSceneContents.GetSelectedItem();
	HTREEITEM hLight;
	hLight = m_rtSceneContents.GetParentItem( hSelected );

	str_t light_id = m_rtSceneContents.GetItemText( hLight ).GetBuffer();

	CLIGHT* light = ((CSceneFile*)pBaseScene)->GetLightByID( light_id );

	CString value;

	value.Format( "%.1f", light->attenuation );

	dlgModelName->SetCaption( "Attenuation:" );
	dlgModelName->SetString( value );
	dlgModelName->DoModal();

	light->attenuation = atof( dlgModelName->GetString().GetBuffer() );

	value.Format( "Attenuation: %.1f", light->attenuation );

	m_rtSceneContents.SetItemText( hSelected, value );

	SAFE_DELETE( dlgModelName );
}

void CCookieMachineDlg::OnMenuScnLightRange()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	CModelNameDlg *dlgModelName = new CModelNameDlg(this);
	HTREEITEM hSelected = m_rtSceneContents.GetSelectedItem();
	HTREEITEM hLight;
	hLight = m_rtSceneContents.GetParentItem( hSelected );

	str_t light_id = m_rtSceneContents.GetItemText( hLight ).GetBuffer();

	CLIGHT* light = ((CSceneFile*)pBaseScene)->GetLightByID( light_id );

	CString value;

	value.Format( "%.1f", light->range );

	dlgModelName->SetCaption( "Range:" );
	dlgModelName->SetString( value );
	dlgModelName->DoModal();

	light->range = atof( dlgModelName->GetString().GetBuffer() );

	value.Format( "Range: %.1f", light->range );

	m_rtSceneContents.SetItemText( hSelected, value );

	SAFE_DELETE( dlgModelName );
}

void CCookieMachineDlg::RunGame()
{
	int offset = 0;

	SetCurrentDirectory( m_appSettings.EngineFolder );

	g_MemMappingsMgr->PostScriptToEngine( "g_Engine:Exit();", false );

	CWnd* pWnd = FindWindow( NULL, "Яйца Фаберже" );

	int iCounter = 0;

	while( pWnd )
	{
		Sleep( 100 );
		pWnd = FindWindow( NULL, "Яйца Фаберже" );

		iCounter++;

		if( iCounter > 50 )
			break;
	}

	if( !pWnd )
	{
		CString strEngineExePath;

		if( m_appSettings.EngineDebugModeOn )
		{
			strEngineExePath = m_appSettings.EngineFolder + "\\Game_debug.exe";
		}
		else
		{
			strEngineExePath = m_appSettings.EngineFolder + "\\Game.exe";
		}

		if( FileExists( strEngineExePath.GetBuffer() ) == false )
		{
			MessageBox( "Engine executable is missing!", "Error", MB_OK | MB_ICONERROR );
			return;
		}

		WinExec( strEngineExePath + " " + m_appSettings.StartParams, SW_SHOW );
		pWnd = FindWindow( NULL, "Яйца Фаберже" );

		if( m_appSettings.SwitchBackOnDebug )
		{
			SetForegroundWindow();
			SetFocus();
		}
		else
		{
			pWnd->SetForegroundWindow();
			pWnd->SetFocus();
		}

		ProcessMessages();
	}
}

void CCookieMachineDlg::OnMenuScnCustObjAddnew()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	CModelNameDlg *dlgModelName = new CModelNameDlg(this);
	HTREEITEM hSelected = m_rtSceneContents.GetSelectedItem();
	HTREEITEM hNewObj;


	dlgModelName->SetCaption( "Object name:" );
	dlgModelName->SetString( "" );
	dlgModelName->DoModal();

	CString str_object_id = dlgModelName->GetString().Trim();
	str_t object_id = str_object_id.GetBuffer();

	//check whether object already exists
	bool obj_exists = false;

	StrVec* m_CustomObjects	= ((CSceneFile*)pBaseScene)->GetCustObjList();
	StrVec* m_CustomTypes	= ((CSceneFile*)pBaseScene)->GetCustTypesList();

	for( unsigned int i = 0; i < m_CustomObjects->size(); i++ )
	{
		if( (*m_CustomObjects)[i] == object_id )
		{
			obj_exists = true;
			break;
		}
	}

	if( obj_exists )
	{
		MessageBox( "Объект с таким именем уже существует!", "Внимание!", MB_OK | MB_ICONWARNING );
	}
	else
	{
		hNewObj = m_rtSceneContents.InsertItem( TVIF_TEXT, _T( object_id.data() ), 0, 0, 0, 0, 0, hSelected, TVI_SORT );
		m_rtSceneContents.SetItemImage( hNewObj, 3, 3 );
		(*m_CustomObjects).push_back( object_id );

		m_rtSceneContents.SortChildren( m_rtSceneContents.GetParentItem( hNewObj ) );
		m_rtSceneContents.SelectItem( hNewObj );
	}

	SAFE_DELETE( dlgModelName );
}

void CCookieMachineDlg::OnMenuScnCustObjDelall()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	HTREEITEM hSelected = m_rtSceneContents.GetSelectedItem();


	StrVec* m_CustomObjects	= ((CSceneFile*)pBaseScene)->GetCustObjList();
	StrVec* m_CustomTypes	= ((CSceneFile*)pBaseScene)->GetCustTypesList();

	if( MessageBox( "Удалить все \"custom\" объекты?", "Внимание!", MB_YESNO | MB_ICONQUESTION ) == IDYES )
	{
		while( m_rtSceneContents.GetNextItem( hSelected, TVGN_CHILD ) != NULL )
			m_rtSceneContents.DeleteItem( m_rtSceneContents.GetNextItem( hSelected, TVGN_CHILD ) );

		(*m_CustomObjects).clear();
	}
}

void CCookieMachineDlg::OnMenuScnCustTypeAddnew()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	CModelNameDlg *dlgModelName = new CModelNameDlg(this);
	HTREEITEM hSelected = m_rtSceneContents.GetSelectedItem();
	HTREEITEM hNewType;


	dlgModelName->SetCaption( "Type name:" );
	dlgModelName->SetString( "" );
	dlgModelName->DoModal();

	CString str_type_id = dlgModelName->GetString().Trim();
	str_t type_id = str_type_id.GetBuffer();

	//check whether type already exists
	bool type_exists = false;

	StrVec* m_CustomObjects	= ((CSceneFile*)pBaseScene)->GetCustObjList();
	StrVec* m_CustomTypes	= ((CSceneFile*)pBaseScene)->GetCustTypesList();

	for( unsigned int i = 0; i < m_CustomTypes->size(); i++ )
	{
		if( (*m_CustomTypes)[i] == type_id )
		{
			type_exists = true;
			break;
		}
	}

	if( type_exists )
	{
		MessageBox( "Тип с таким именем уже существует!", "Внимание!", MB_OK | MB_ICONWARNING );
	}
	else
	{
		hNewType = m_rtSceneContents.InsertItem( TVIF_TEXT, _T( type_id.data() ), 0, 0, 0, 0, 0, hSelected, TVI_SORT );
		m_rtSceneContents.SetItemImage( hNewType, 3, 3 );
		m_CustomTypes->push_back( type_id );

		m_rtSceneContents.SortChildren( m_rtSceneContents.GetParentItem( hNewType ) );
		m_rtSceneContents.SelectItem( hNewType );
	}

	SAFE_DELETE( dlgModelName );
}

void CCookieMachineDlg::OnMenuScnCustTypeDelall()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	HTREEITEM hSelected = m_rtSceneContents.GetSelectedItem();

	StrVec* m_CustomObjects	= ((CSceneFile*)pBaseScene)->GetCustObjList();
	StrVec* m_CustomTypes	= ((CSceneFile*)pBaseScene)->GetCustTypesList();

	if( MessageBox( "Удалить все \"custom\" типы?", "Внимание!", MB_YESNO | MB_ICONQUESTION ) == IDYES )
	{
		while( m_rtSceneContents.GetNextItem( hSelected, TVGN_CHILD ) != NULL )
			m_rtSceneContents.DeleteItem( m_rtSceneContents.GetNextItem( hSelected, TVGN_CHILD ) );

		m_CustomTypes->clear();
	}
}

void CCookieMachineDlg::OnMenuScnCustTypeDel()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	HTREEITEM hSelected = m_rtSceneContents.GetSelectedItem();
	str_t item_text = m_rtSceneContents.GetItemText( hSelected ).GetBuffer();

	StrVec* m_CustomObjects	= ((CSceneFile*)pBaseScene)->GetCustObjList();
	StrVec* m_CustomTypes	= ((CSceneFile*)pBaseScene)->GetCustTypesList();

	if( MessageBox( "Удалить тип?", "Внимание!", MB_YESNOCANCEL | MB_ICONWARNING ) == IDYES )
	{
		//delete tree node
		m_rtSceneContents.DeleteItem( hSelected );

		//delete string from str vector
		StrVec_iter iter = m_CustomTypes->begin();
        while( iter != m_CustomTypes->end() )
		{
			if( *iter == item_text )
			{
				m_CustomTypes->erase( iter );
				break;
			}

			iter++;
		}
	} // if( MessageBox( "Удалить тип
}

void CCookieMachineDlg::OnMenuScnCustObjDel()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	HTREEITEM hSelected = m_rtSceneContents.GetSelectedItem();
	str_t item_text = m_rtSceneContents.GetItemText( hSelected ).GetBuffer();

	StrVec* m_CustomObjects	= ((CSceneFile*)pBaseScene)->GetCustObjList();
	StrVec* m_CustomTypes	= ((CSceneFile*)pBaseScene)->GetCustTypesList();

	if( MessageBox( "Удалить объект?", "Внимание!", MB_YESNOCANCEL | MB_ICONWARNING ) == IDYES )
	{
		//delete tree node
		m_rtSceneContents.DeleteItem( hSelected );

		//delete string from str vector
		StrVec_iter iter = m_CustomObjects->begin();
        while( iter != m_CustomObjects->end() )
		{
			if( *iter == item_text )
			{
				m_CustomObjects->erase( iter );
				break;
			}

			iter++;
		}
	} // if( MessageBox( "Удалить тип
}

void CCookieMachineDlg::OnMenuScnBindsShowWnd()
{
	ShowBindsWindow();
}

void CCookieMachineDlg::OnMenuScnScriptsShowWnd()
{
	ShowScriptsWindow();
}

void CCookieMachineDlg::OnMenuScnFloorUVRecalc()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	CValidateDlg* dlgValidate = NULL;

	SetCursor( LoadCursor(NULL, IDC_WAIT) );

	//show validation window
	if( !dlgValidate )
	{
		dlgValidate = new CValidateDlg(this);
		dlgValidate->SetMessageStr( " Расчёт UV map для полов..." );
		ProcessMessages();
		((CSceneFile*)pBaseScene)->SetProgressWnd( (CWnd*)dlgValidate );
	}

	((CSceneFile*)pBaseScene)->RecalcGridUV();
	pBaseScene->Prepare( g_pd3dDevice );

	SetCursor( LoadCursor(NULL, IDC_ARROW) );

	SAFE_DELETE( dlgValidate );
}

void CCookieMachineDlg::OnMenuScnTextureAddBackground()
{
    if( !pBaseScene )
        return;

    if( pBaseScene->GetType() != FT_SCN )
        return;

	StrVec BFiles, BFilesSceneOK, BFilesBackAlphaOK, BFilesRes;

	//show open file dialog
	str_t			szFilter	= "DDS, JPG Files (*.dds; *.jpg)|*.dds; *.jpg|";
	CString			temp		= "*.dds; *.jpg";
	DWORD			dwFlags		= OFN_ALLOWMULTISELECT | OFN_ENABLESIZING | OFN_FILEMUSTEXIST;
	CFileDialog*	dlg			= new CFileDialog( TRUE,	temp.GetBuffer(0), 0,	dwFlags, szFilter.data(), NULL, sizeof(OPENFILENAME) );

	DWORD MAXFILE = 2562; //2562 is the max
	dlg->m_pOFN->nMaxFile = MAXFILE;
	char* pc = new char[MAXFILE];

	dlg->m_pOFN->lpstrFile			= pc;
	dlg->m_pOFN->lpstrFile[0]		= NULL;
	dlg->m_pOFN->lpstrInitialDir	= m_appSettings.FilesPath;


	//user confirmed "open file dialog"
	if( dlg->DoModal() == IDOK )
	{
		POSITION pos = dlg->GetStartPosition();

		while( pos )
		{
			str_t file_path	= dlg->GetNextPathName( pos ).GetBuffer();
			BFiles.push_back( FilenameFromPath( file_path ) );
		}
	}

	SAFE_DELETE_ARRAY( pc );
	SAFE_DELETE( dlg );

	// in case no files selected -- abort
	if( BFiles.size() == 0 )
		return;

	// make sure that files prefixes comply with scene name
	str_t scene_id = ((CSceneFile*)pBaseScene)->GetSceneID();

	for( unsigned int i = 0; i < BFiles.size(); i++ )
	{
		str_t temp = BFiles[i];

		str_t file_scene_id = StripTypeHeader( temp );

		if(	file_scene_id != scene_id )
		{
			//str_t err_msg = "Задник \"" + BFiles[i] + "\" не принадлежит сцене [" + scene_id + "]"; 
			//MessageBox( err_msg.data(), "Внимание!", MB_OK | MB_ICONWARNING );
		}
		else
		{
			temp = temp.substr( 0, temp.length() - 4 );
			BFilesSceneOK.push_back( temp );
		}
	}


	// make sure that every background has alpha and back
 	for( unsigned int i = 0; i < BFilesSceneOK.size(); i++ )
	{
		str_t temp		= BFilesSceneOK[i];
		str_t back_id	= StripTypeHeader( temp );
	
		bool back_found = false, alpha_found = false;
	
		for( unsigned int i = 0; i < BFilesSceneOK.size(); i++ )
		{
			str_t back_type			= BFilesSceneOK[i];
			str_t back_temp_id		= StripTypeHeader( back_type );
			back_type				= _2Upper( (char*)back_type.data() );

			back_found = true;

			if( back_temp_id == back_id && back_type == "ALPHA" )
				alpha_found = true;
		}

		if( back_found )
		{
			BFilesBackAlphaOK.push_back( back_id );
		}
		else
		{
			//if( !back_found )
			//{
			//	str_t err_msg = "У задника \"" + back_id + "\" отсутствует \"back\" файл!"; 
			//	MessageBox( err_msg.data(), "Внимание!", MB_OK | MB_ICONWARNING );
			//}

			//if( !alpha_found )
			//{
			//	str_t err_msg = "У задника \"" + back_id + "\" отсутствует \"alpha\" файл!"; 
			//	MessageBox( err_msg.data(), "Внимание!", MB_OK | MB_ICONWARNING );
			//}
		}
	}

	//remove dupes
	for( unsigned int i = 0; i < BFilesBackAlphaOK.size(); i++ )
	{
		bool bExists = false;

		for( unsigned int j = 0; j < BFilesRes.size(); j++ )
		{
			if( BFilesRes[j] == BFilesBackAlphaOK[i] )
				bExists = true;
		}

		if( !bExists )
		{
			BFilesRes.push_back( BFilesBackAlphaOK[i] );
		}
	}

	
	StrVec* tex_list = ((CSceneFile*)pBaseScene)->GetTextures();


	HTREEITEM hTextures = m_rtSceneContents.FindTreeItemByName( "Textures" );

	//finally!!!
	//add correct files to the backs list
	for( unsigned int i = 0; i < BFilesRes.size(); i++ )
	{
		bool bExists = false;

		for( unsigned int j = 0; j < tex_list->size(); j++ )
		{
			if( (*tex_list)[j] == BFilesRes[i] )
				bExists = true;
		}

		if( !bExists )
		{
			tex_list->push_back( BFilesRes[i] );
			m_rtSceneContents.InsertItem( TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE, BFilesRes[i].data(), 21, 21, 0, 0, 0, hTextures, TVI_SORT );
		}
		else
		{
			//str_t err_msg = "Задник \"" + BFilesRes[i] + "\" уже существует в списке!"; 
			//MessageBox( err_msg.data(), "Внимание!", MB_OK | MB_ICONWARNING );
		}
	}

	//UpdateUI();
}

void CCookieMachineDlg::OnMenuScnTextureDel()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	HTREEITEM hSelected = m_rtSceneContents.GetSelectedItem();
	str_t item_text = m_rtSceneContents.GetItemText( hSelected ).GetBuffer();


	/*CString msg = "Удалить текстуру [";
	msg += item_text.data();
	msg += "] ?";

	if( MessageBox( msg, "Внимание!", MB_YESNOCANCEL | MB_ICONWARNING ) == IDYES )
	{*/
	StrVec*	tex_list	= ((CSceneFile*)pBaseScene)->GetTextures();

	for( StrVec_iter iter = tex_list->begin(); iter != tex_list->end(); iter++ )
	{
		if( *iter == item_text )
		{
			tex_list->erase( iter );
			break;
		}
	}

	//delete tree node
	m_rtSceneContents.DeleteItem( hSelected );

	//} // if( MessageBox( "Удалить тип	
}

void CCookieMachineDlg::OnMenuImport()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() == FT_SCN )
	{
		ImportToScn();
	}
	else if( pBaseScene->GetType() == FT_EFN_MESHLIST )
	{
		ImportToMeshlist();
	}
	else
	{}
}

void CCookieMachineDlg::OnMenuEfnEditMaterial()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN )
		return;

	TNODE* node = ( TNODE* )m_rtSceneContents.GetCurrNodeData();

	if( !node || node->_type != TREENODE::N_MATERIAL )
		return;

	CMaterialDlg* dlg = new CMaterialDlg;
	dlg->m_Material = ((CEfnCharacter*)node->_data)->GetMaterial();
	dlg->DoModal();
	
	((CEfnCharacter*)node->_data)->SetMaterial( dlg->m_Material );

	delete dlg;
}

void CCookieMachineDlg::OnMenuEfnNullsDetachAll()
{
	HTREEITEM	hNulls	= m_rtSceneContents.GetSelectedItem();
	TNODE*		data	= (TNODE*)m_rtSceneContents.GetCurrNodeData();

	if( !data || data->_type != TREENODE::N_NULLS )
		return;

	CEfnCharacter* pChar = (CEfnCharacter*)data->_data;

	for( int i = 0; i < pChar->GetNullsCount(); i++ )
	{
		CNULL2* pNull = pChar->GetNullByIndex( i );

		for( UINT j = 0; j < pNull->attaches.size(); j++ )
		{
			SAFE_DELETE( pNull->attaches[j] );
		}

		pNull->attaches.clear();
	}

	HTREEITEM hNull	= m_rtSceneContents.GetChildItem( hNulls );

	while( hNull )
	{
		HTREEITEM hTemp	= m_rtSceneContents.GetChildItem( hNull );

		while( hTemp )
		{
			m_rtSceneContents.DeleteItem( hTemp );
			hTemp = m_rtSceneContents.GetChildItem( hNull );
		}

		hNull = m_rtSceneContents.GetNextSiblingItem( hNull );
	}
}

void CCookieMachineDlg::OnBnClickedBtnFilesaveAll()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN )
		return;

	//save file itself
	OnMenuFileSave();

	////save attached files
	//CEfnCharacter* pChar = (CEfnCharacter*)pBaseScene;

	//for( int i = 0; i < pChar->GetNullsCount(); i++ )
	//{
	//	CNULL2* pNull = pChar->GetNullByIndex( i );

	//	if( pNull->attachment )
	//		((CEfnCharacter*)pNull->attachment)->SaveToFile();
	//}
}

void CCookieMachineDlg::OnMenuEfnNullScaling()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN )
		return;

	HTREEITEM	hRoot	= m_rtSceneContents.GetSelectedItem();
	TNODE*		data	= (TNODE*)m_rtSceneContents.GetCurrNodeData();

	if( !data || data->_type != TREENODE::N_NULL )
		return;

	CEfnCharacter* pEfnAnim = (CEfnCharacter*)data->_data;

	CModelNameDlg *dlgModelName = new CModelNameDlg( this );
	HTREEITEM hSelected = m_rtSceneContents.GetSelectedItem();
	CString tmp;

	tmp = m_rtSceneContents.GetItemText( hSelected );

	CNULL2* pNull = pEfnAnim->GetNullByName( tmp.GetBuffer(0) );

	tmp.Format( "%0.4f", pNull->matrices[0].scl.x );

	dlgModelName->SetCaption( "Null scale:" );
	dlgModelName->SetString( tmp );
	dlgModelName->DoModal();
	
	float fScale = atof( dlgModelName->GetString().GetBuffer(0) );

	for( int i = 0; i < pEfnAnim->GetFramesCount(); i++ )
	{
		pNull->matrices[i].scl.x = fScale;
		pNull->matrices[i].scl.y = fScale;
		pNull->matrices[i].scl.z = fScale;
	}

	SAFE_DELETE(dlgModelName);
}

void CCookieMachineDlg::OnMenuMeshlistNullRename( CString strCurrID, CString strNewID )
{
	if( strCurrID == strNewID )
		return;

	CEfnMeshlist*	pMeshlist	= (CEfnMeshlist*)pBaseScene;
	CNULL*			pNull		= pMeshlist->GetMeshByName( "Nulls" )->GetNullByName( strCurrID.GetBuffer() );

	CString _strCurrID	= strCurrID;
	CString _strNewID	= strNewID;

	_strCurrID.MakeUpper();
	_strNewID.MakeUpper();

	if( pMeshlist->GetMeshByName( "Nulls" )->GetNullByName2( strNewID.GetBuffer() ) && _strCurrID != _strNewID )
	{
		MessageBox( "Нуль с таким ID уже существует!", "Внимание!", MB_OK | MB_ICONINFORMATION );
		return;
	}

	pNull->id = strNewID.GetBuffer();
	m_rtSceneContents.SetItemText( m_rtSceneContents.GetSelectedItem(), strNewID );
}

void CCookieMachineDlg::OnMenuMeshlistNullRemove()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN_MESHLIST )
		return;

	HTREEITEM	hSelected	= m_rtSceneContents.GetSelectedItem();
	CString		tmp			= m_rtSceneContents.GetItemText( hSelected );
	
	((CEfnMeshlist*)pBaseScene)->GetMeshByName( "Nulls" )->RemoveNull( tmp.GetBuffer(0) );
	
	m_rtSceneContents.DeleteItem( hSelected );
}

void CCookieMachineDlg::OnMenuMeshlistNullAttach()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN_MESHLIST )
		return;

	HTREEITEM	hSelected	= m_rtSceneContents.GetSelectedItem();
	CBaseScene*	pAttachment	= NULL;
	str_t		szFilter	= "EFN Files (*.efn)|*.efn|";
	CString		temp		= "*.efn";
	bool		bCancelled	= false;
	str_t		file_dir;
	str_t		file_name;

	//show open file dialog
	CFileDialog* dlg = new CFileDialog( TRUE,	temp.GetBuffer(0), 0,	OFN_EXPLORER, szFilter.data(), NULL, sizeof(OPENFILENAME) );
	dlg->m_pOFN->lpstrInitialDir = m_appSettings.FilesPath;

	//user confirmed "open file dialog"
	if( dlg->DoModal() != IDOK )
		bCancelled = true;

	file_dir	= dlg->GetPathName().Left( dlg->GetPathName().GetLength() - dlg->GetFileName().GetLength() );
	file_name	= dlg->GetFileName();

	SAFE_DELETE( dlg );

	if(	bCancelled )
		return;

	if( !FileExists( file_dir + file_name ) )
	{
		MessageBox( "Файл не найден!", "Ошибка.", MB_OK | MB_ICONWARNING ); 
		return;
	}

	bool			bLoadFailed	= false;
	CValidateDlg*	dlgValidate	= NULL;

	SetCursor( LoadCursor( NULL, IDC_WAIT ) );

	//show validation window
	if( !dlgValidate )
	{
		dlgValidate = new CValidateDlg( this );
		dlgValidate->SetMessageStr( "Проверка файла..." );
		ProcessMessages();
	}

	if( GetEfnFiletype( file_dir + file_name ) == EF_ANIMATION )
	{
		//check file prefix
		CEfnCharacter* efn_anim = new CEfnCharacter;

		if( efn_anim->LoadFromFile( file_dir + file_name ) )
		{
			pAttachment = efn_anim;
			pAttachment->SetPaths( file_dir, file_name );


			//prepare data for rendering
			pAttachment->Prepare( g_pd3dDevice );

			CNULL* nl = ((CEfnMeshlist*)pBaseScene)->GetMeshByName( "Nulls" )->GetNullByName( m_rtSceneContents.GetItemText(hSelected).GetBuffer() );

			SAFE_DELETE( nl->attachment );
			nl->attachment = pAttachment;
		}
		else
		{
			// load failed -- cleanup
			SAFE_DELETE( efn_anim ); 
			bLoadFailed = true;
		}
	}
	else if( GetEfnFiletype( file_dir + file_name ) == EF_MESHLIST )
	{	
		//check file prefix
		CEfnMeshlist* efn_meshlist = new CEfnMeshlist;

		if( efn_meshlist->LoadFromFile( file_dir + file_name ) )
		{
			efn_meshlist->SetParent( this );
			efn_meshlist->SetNullIcon( pNullIco );

			pAttachment = efn_meshlist;
			pAttachment->SetPaths( file_dir, file_name );


			//prepare data for rendering
			pAttachment->Prepare( g_pd3dDevice );

			CNULL* nl = ((CEfnMeshlist*)pBaseScene)->GetMeshByName( "Nulls" )->GetNullByName( m_rtSceneContents.GetItemText(hSelected).GetBuffer() );

			SAFE_DELETE( nl->attachment );
			nl->attachment = pAttachment;
		}
		else
		{
			// load failed -- cleanup
			SAFE_DELETE( efn_meshlist ); 
			bLoadFailed = true;
		}
	}
	else
	{
		bLoadFailed = true;
	}

	SetCursor( LoadCursor( NULL, IDC_ARROW ) );

	if( bLoadFailed )
	{
		MessageBox( "Failed to load file! Undecognized file format.", "Error!", MB_OK | MB_ICONERROR );
	}
	else
	{
		HTREEITEM hItem	= m_rtSceneContents.GetSelectedItem(); 

		//delete previous attachment node
		HTREEITEM hAttachment = m_rtSceneContents.GetChildItem( hItem );

		if( hAttachment )
			m_rtSceneContents.DeleteItem( hAttachment );

		//insert attachment node in tree
		if( pAttachment && hItem )
		{	
			DWORD uiMask	= TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
			m_rtSceneContents.InsertItem( uiMask, pAttachment->GetFilename().data(), 28, 28, 0, 0, NULL, hItem, NULL );
		}
	}

	SAFE_DELETE( dlgValidate );
}

void CCookieMachineDlg::OnMenuMeshlistNullDetach()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN_MESHLIST )
		return;

	SetCursor( LoadCursor( NULL, IDC_WAIT ) );

	HTREEITEM	hSelected	= m_rtSceneContents.GetSelectedItem();
	CString		strSelectedItemText = m_rtSceneContents.GetItemText( hSelected );

	CNULL* pNull = ((CEfnMeshlist*)pBaseScene)->GetMeshByName( "Nulls" )->GetNullByName( strSelectedItemText.GetBuffer(0) );
	SAFE_DELETE( pNull->attachment );


	HTREEITEM hAttachment = m_rtSceneContents.GetChildItem( hSelected );

	if( hAttachment )
		m_rtSceneContents.DeleteItem( hAttachment );

	m_rtSceneContents.SelectItem( hSelected );

	SetCursor( LoadCursor( NULL, IDC_ARROW ) );
}

void CCookieMachineDlg::OnMenuMeshlistNullsNew()
{
	OnMenuMeshlistNullsNewByPos();
}

bool CCookieMachineDlg::OnMenuSceneNullsNewByPos( D3DXVECTOR3* pos, str_t* insertedNullID )
{
	if( !pBaseScene )
		return false;

	if( pBaseScene->GetType() != FT_SCN )
		return false;

	CModelNameDlg*	dlgNullName	= new CModelNameDlg(this);
	HTREEITEM		hNulls		= m_rtSceneContents.FindTreeItemByName( "Nulls" );
	CEfnMeshlist*	pMeshlist	= (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();
	CMesh*			pMesh		= pMeshlist->GetMeshByName( "Nulls" );

	if( !pMesh )
		return false;

	dlgNullName->SetCaption( "Имя нуля:" );
	dlgNullName->SetString( "" );

	if( dlgNullName->DoModal() == IDOK )
	{
		if( pMeshlist->GetMeshByName2( "Nulls" )->GetNullByName2( dlgNullName->GetString().GetBuffer() ) )
		{
			MessageBox( "Нуль с таким ID уже существует!", "Внимание!", MB_OK | MB_ICONINFORMATION );
			SAFE_DELETE( dlgNullName );
			return false;
		}

		pMesh->CreateNull( dlgNullName->GetString().GetBuffer(), pos );		

		CNULL*		pNull		= pMesh->GetNullByName( dlgNullName->GetString().GetBuffer(0) );
		HTREEITEM	hNullItem	= m_rtSceneContents.InsertItem2( pNull->id.data(), 10, NULL, hNulls, TVI_SORT );

		//return res id
		if( insertedNullID )
			*insertedNullID = pNull->id;

		m_rtSceneContents.SortChildren( m_rtSceneContents.GetParentItem( hNullItem ) );
		m_rtSceneContents.SelectItem( hNullItem );
	}

	SAFE_DELETE( dlgNullName );
	((CSceneFile*)pBaseScene)->Prepare( g_pd3dDevice );

	return true;
}

void CCookieMachineDlg::OnMenuMeshlistNullsNewByPos( D3DXVECTOR3* pos )
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN_MESHLIST )
		return;

	CModelNameDlg*	dlgNullName	= new CModelNameDlg(this);
	CEfnMeshlist*	pMeshlist	= (CEfnMeshlist*)pBaseScene;
	HTREEITEM	hNulls		= m_rtSceneContents.FindTreeItemByName( "Nulls" );
	UINT		uiMask		= TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	CMesh*		pMesh		= ((CEfnMeshlist*)pBaseScene)->GetMeshByName( "Nulls" );
	str_t		strNewNullName;

	if( !pMesh )
		return;

	dlgNullName->SetCaption( "Имя нуля:" );
	dlgNullName->SetString( "" );
	dlgNullName->DoModal();

	strNewNullName = dlgNullName->GetString().GetBuffer(0);

	SAFE_DELETE( dlgNullName );

	if( strNewNullName == "" )
		return;

	if( pMeshlist->GetMeshByName2( "Nulls" )->GetNullByName2( strNewNullName ) )
	{
		MessageBox( "Нуль с таким ID уже существует!", "Внимание!", MB_OK | MB_ICONINFORMATION );
		return;
	}

	pMesh->CreateNull( strNewNullName, pos );
	pBaseScene->Prepare( g_pd3dDevice );

	CNULL*		pNull = pMesh->GetNullByName( strNewNullName );
	HTREEITEM	hTemp = m_rtSceneContents.InsertItem( uiMask, pNull->id.data(), 10, 10, 0, 0, 0, hNulls, NULL );
}

void CCookieMachineDlg::OnMenuMeshlistNullsRemoveAll()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN_MESHLIST )
		return;

	HTREEITEM	hNulls				= m_rtSceneContents.FindTreeItemByName( "Nulls" );
	CString		strSelectedItemText = m_rtSceneContents.GetItemText( hNulls );

	// delete all nulls from mesh
	((CEfnMeshlist*)pBaseScene)->DeleteAllNulls();
	((CEfnMeshlist*)pBaseScene)->Prepare( g_pd3dDevice );

	// remove null nodes from treeview
	HTREEITEM hTemp = m_rtSceneContents.GetChildItem( hNulls );

	while( hTemp )
	{
		m_rtSceneContents.DeleteItem( hTemp );
		hTemp = m_rtSceneContents.GetChildItem( hNulls );
	}
}

void CCookieMachineDlg::OnMenuMeshlistNullsDetachAll()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN_MESHLIST )
		return;

	HTREEITEM	hSelected			= m_rtSceneContents.GetSelectedItem();
	CString		strSelectedItemText = m_rtSceneContents.GetItemText( hSelected );

	CMesh* pMesh = ((CEfnMeshlist*)pBaseScene)->GetMeshByName( "Nulls" );

	// detach all nulls
	for( int i = 0; i < pMesh->GetNullsCount(); i++ )
		SAFE_DELETE( pMesh->GetNullByIndex( i )->attachment );

	HTREEITEM hNull = m_rtSceneContents.GetChildItem( hSelected );

	while( hNull )
	{
		HTREEITEM att = m_rtSceneContents.GetChildItem( hNull );

		if( att )
			m_rtSceneContents.DeleteItem( att );

		hNull = m_rtSceneContents.GetNextSiblingItem( hNull );
	}
}

void CCookieMachineDlg::OnMenuMeshlistMeshEditVertices()
{
	StartEditingMeshVertices();
}

void CCookieMachineDlg::FinishEditingMeshVertices()
{
	if( !pBaseScene )
		return;

	CMesh* pMesh = NULL;

	if( pBaseScene->GetType() == FT_EFN_MESHLIST )
	{
		pMesh = ((CEfnMeshlist*)pBaseScene)->GetSelectedMesh();
	}
	else if( pBaseScene->GetType() == FT_SCN )
	{
		CEfnMeshlist* pMeshlist = (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();

		if( !pMeshlist )
			return;

		pMesh = pMeshlist->GetSelectedMesh();
	}
	else
	{}

	if( !pMesh || !m_bIsVertexEditingModeOn )
		return;


	pMesh->SetVertexEditingMode( false );
	pMesh->Prepare( g_pd3dDevice );

	m_rtSceneContents.EnableWindow( true );
	m_rtSceneContents.UpdateWindow();
	m_rtSceneContents.SetFocus();

	m_bIsVertexEditingModeOn = false;

	if( pBaseScene->GetType() == FT_EFN_MESHLIST )
	{
		UpdateStatusBarInfoMESHLIST();
		UpdateMeshesInfoInExplorer();
	}
	else if( pBaseScene->GetType() == FT_SCN )
	{
		UpdateStatusBarInfoSCN();
	}
}

void CCookieMachineDlg::StartEditingMeshVertices()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() == FT_SCN )
	{
		CEfnMeshlist*	pMeshlist	= (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();
		CMesh*			pMesh		= pMeshlist->GetSelectedMesh();

		if( pMesh && pMesh->GetVisible() )
		{
			m_rtSceneContents.EnableWindow( false );
			m_bIsVertexEditingModeOn = true;
			
			pMesh->SetVertexEditingMode( true );
		}

		UpdateStatusBarInfoSCN();
	}
	else if( pBaseScene->GetType() == FT_EFN_MESHLIST )
	{	
		CMesh* pMesh = ((CEfnMeshlist*)pBaseScene)->GetSelectedMesh();

		if( pMesh && pMesh->GetVisible() )
		{
			m_rtSceneContents.EnableWindow( false );
			m_bIsVertexEditingModeOn = true;
			
			pMesh->SetVertexEditingMode( true );
		}

		UpdateStatusBarInfoMESHLIST();
	}
	else
	{}
}

void CCookieMachineDlg::SwitchVerteciesEditingMode()
{
	if( m_bIsVertexEditingModeOn )
		FinishEditingMeshVertices();
	else
		StartEditingMeshVertices();
}

void CCookieMachineDlg::OnMenuObjectShowOnly()
{
	if( !pBaseScene || m_bIsVertexEditingModeOn )
		return;

	if( pBaseScene->GetType() == FT_EFN_MESHLIST )
	{
		HTREEITEM	hMeshes		= m_rtSceneContents.FindTreeItemByName( "Meshes" );
		HTREEITEM	hMesh		= m_rtSceneContents.GetChildItem( hMeshes );
		HTREEITEM	hSelected	= m_rtSceneContents.GetSelectedItem();
		CString		strSelectedItemText = m_rtSceneContents.GetItemText( hSelected );

		m_rtSceneContents.SetItemImage( hMesh, 4, 4 );

		while( hMesh )
		{
			hMesh = m_rtSceneContents.GetNextSiblingItem( hMesh );
			m_rtSceneContents.SetItemImage( hMesh, 4, 4 );
		}

		m_rtSceneContents.SetItemImage( hSelected, 0, 0 );

		((CEfnMeshlist*)pBaseScene)->ShowAllMeshes( false );
		((CEfnMeshlist*)pBaseScene)->ShowMeshes( strSelectedItemText.GetBuffer(0), true );
	}
	else if( pBaseScene->GetType() == FT_SCN )
	{
		((CSceneFile*)pBaseScene)->ShowEverything( false, g_pd3dDevice );
		OnBnClickedBtnHideall();

		HTREEITEM	hSelected		= m_rtSceneContents.GetSelectedItem();
		HTREEITEM	hParent			= m_rtSceneContents.GetParentItem( hSelected );
		HTREEITEM	hParentParent	= m_rtSceneContents.GetParentItem( hParent );

		CString	item	= m_rtSceneContents.GetItemText( hSelected );
		CString	parent	= m_rtSceneContents.GetItemText( hParent );


		//get currently selected treenode name
		if( parent == "Lights" && hParentParent == NULL )
		{
			CLIGHT* light = ((CSceneFile*)pBaseScene)->GetLightByID( item.GetBuffer( 0 ) );
			light->visible = true;
			((CSceneFile*)pBaseScene)->PrepareLightsLines( g_pd3dDevice ); 

			m_rtSceneContents.SetItemImage( hSelected, 9, 9 );
			//m_rtSceneContents.SelectItem( NULL );
		}
		else if( parent == "Cameras" && hParentParent == NULL )
		{
			CCAMERA2* camera = ((CSceneFile*)pBaseScene)->GetCameraByID( item.GetBuffer( 0 ) );
			camera->visible = true;
			((CSceneFile*)pBaseScene)->PrepareCamsLines( g_pd3dDevice ); 

			m_rtSceneContents.SetItemImage( hSelected, 5, 5 );
			//m_rtSceneContents.SelectItem( NULL );
		}
		else if( parent == "Meshes" && hParentParent == NULL )
		{
			CEfnMeshlist* meshlist = (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();
			
			CMesh* pMesh = meshlist->GetSelectedMesh();

			if( pMesh )
			{
				pMesh->SetVisible( true );
				//m_rtSceneContents.SelectItem( NULL );
				m_rtSceneContents.SetItemImage( hSelected, 0, 0 );
			}
		}
		else
		{}
	}
	else
	{}
}

void CCookieMachineDlg::OnMenuMeshlistNullShow()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN_MESHLIST )
		return;

	HTREEITEM	hSelected			= m_rtSceneContents.GetSelectedItem();
	CString		strSelectedItemText = m_rtSceneContents.GetItemText( hSelected );

	//((CEfnMeshlist*)pBaseScene)->GetMeshByName( "Nulls" )->s
	//((CEfnMeshlist*)pBaseScene)->ShowMeshes( strSelectedItemText.GetBuffer(0), true );
}

void CCookieMachineDlg::OnMenuMeshlistNullHide()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN_MESHLIST )
		return;

	HTREEITEM	hSelected			= m_rtSceneContents.GetSelectedItem();
	CString		strSelectedItemText = m_rtSceneContents.GetItemText( hSelected );

	//((CEfnMeshlist*)pBaseScene)->ShowAllMeshes( false );
	//((CEfnMeshlist*)pBaseScene)->ShowMeshes( strSelectedItemText.GetBuffer(0), true );
}

void CCookieMachineDlg::OnMenuMeshlistMeshMaterial()
{
	if( !pBaseScene || m_bIsVertexEditingModeOn )
		return;

	if( pBaseScene->GetType() != FT_EFN_MESHLIST )
		return;

	CMesh* pMesh = ((CEfnMeshlist*)pBaseScene)->GetSelectedMesh();

	if( !pMesh )
		return;

	CMaterialDlg* dlg = new CMaterialDlg;
	dlg->m_Material = pMesh->GetMaterial();
	dlg->DoModal();
	
	pMesh->SetMaterial( dlg->m_Material );

	delete dlg;
}

void CCookieMachineDlg::OnMenuScnMeshShowOnly()
{
	if( !pBaseScene || m_bIsVertexEditingModeOn )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	HTREEITEM	hMeshes		= m_rtSceneContents.FindTreeItemByName( "Meshes" );
	HTREEITEM	hMesh		= m_rtSceneContents.GetChildItem( hMeshes );
	HTREEITEM	hSelected	= m_rtSceneContents.GetSelectedItem();
	CString		strSelectedItemText = m_rtSceneContents.GetItemText( hSelected );

	m_rtSceneContents.SetItemImage( hMesh, 4, 4 );

	while( hMesh )
	{
		hMesh = m_rtSceneContents.GetNextSiblingItem( hMesh );
		m_rtSceneContents.SetItemImage( hMesh, 4, 4 );
	}

	m_rtSceneContents.SetItemImage( hSelected, 0, 0 );


	CEfnMeshlist* pMeshlist = (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();

	((CSceneFile*)pBaseScene)->ShowEverything( false, g_pd3dDevice );
	pMeshlist->ShowMeshes( strSelectedItemText.GetBuffer(0), true );
}

void CCookieMachineDlg::OnMenuScnCameraShowOnly()
{
	if( !pBaseScene || m_bIsVertexEditingModeOn )
		return;

	OnMenuObjectShowOnly();
}

void CCookieMachineDlg::OnMenuScnLightShowOnly()
{
	if( !pBaseScene || m_bIsVertexEditingModeOn )
		return;

	OnMenuObjectShowOnly(); 
}

void CCookieMachineDlg::OnMenuScnMeshEditMaterial()
{
	if( !pBaseScene || m_bIsVertexEditingModeOn )
		return;

	if( pBaseScene->GetType() == FT_SCN )
	{
		CEfnMeshlist* pMeshlist = (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();

		CMesh* pMesh = pMeshlist->GetSelectedMesh();

		if( !pMesh )
			return;

		CMaterialDlg* dlg = new CMaterialDlg;
		dlg->m_Material = pMesh->GetMaterial();
		dlg->DoModal();
		
		pMesh->SetMaterial( dlg->m_Material );

		delete dlg;
	}
}

void CCookieMachineDlg::OnMenuScnMeshEditVertecies()
{
	StartEditingMeshVertices();
}

void CCookieMachineDlg::OnMenuScnNullsShow()
{
	if( !pBaseScene || m_bIsVertexEditingModeOn )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	CEfnMeshlist*	pMeshlist	= (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();
	CMesh*			pMesh		= pMeshlist->GetMeshByName( "Nulls" );

	if( pMesh )
		pMesh->SetVisible( true );

	HTREEITEM	hNulls		= m_rtSceneContents.FindTreeItemByName( "Nulls" );
	HTREEITEM	hNull		= m_rtSceneContents.GetChildItem( hNulls );
	
	m_rtSceneContents.SetItemImage( hNull, 10, 10 );

	while( hNull )
	{
		hNull = m_rtSceneContents.GetNextSiblingItem( hNull );
		m_rtSceneContents.SetItemImage( hNull, 10, 10 );
	}
}

void CCookieMachineDlg::OnMenuScnNullsHide()
{
	if( !pBaseScene || m_bIsVertexEditingModeOn )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	CEfnMeshlist*	pMeshlist	= (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();
	CMesh*			pMesh		= pMeshlist->GetMeshByName( "Nulls" );

	if( pMesh )
		pMesh->SetVisible( false );

	HTREEITEM	hNulls		= m_rtSceneContents.FindTreeItemByName( "Nulls" );
	HTREEITEM	hNull		= m_rtSceneContents.GetChildItem( hNulls );
	
	m_rtSceneContents.SetItemImage( hNull, 24, 24 );

	while( hNull )
	{
		hNull = m_rtSceneContents.GetNextSiblingItem( hNull );
		m_rtSceneContents.SetItemImage( hNull, 24, 24 );
	}
}

void CCookieMachineDlg::OnMenuScnNullAttach()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;


	HTREEITEM	hSelected	= m_rtSceneContents.GetSelectedItem();
	CBaseScene*	pAttachment	= NULL;
	str_t		szFilter	= "EFN Files (*.efn)|*.efn|";
	CString		temp		= "*.efn";
	CMesh*		pMesh = ((CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist() )->GetMeshByName( "Nulls" );
	str_t		file_dir;
	str_t		file_name;
	bool		bCancelled = false;
	bool		bLoadFailed	= false;
	CValidateDlg*	dlgValidate	= NULL;

	if( !pMesh )
		return;

	CNULL* nl = pMesh->GetNullByName( m_rtSceneContents.GetItemText(hSelected).GetBuffer() );
	
	if( !nl )
		return;

	//show open file dialog
	CFileDialog* dlg = new CFileDialog( TRUE,	temp.GetBuffer(0), 0,	OFN_EXPLORER, szFilter.data(), NULL, sizeof(OPENFILENAME) );
	dlg->m_pOFN->lpstrInitialDir = m_appSettings.FilesPath;

	//user confirmed "open file dialog"
	if( dlg->DoModal() == IDCANCEL )
		bCancelled = true;
	
	file_dir	= dlg->GetPathName().Left( dlg->GetPathName().GetLength() - dlg->GetFileName().GetLength() );
	file_name	= dlg->GetFileName();

	SAFE_DELETE( dlg );

	if( bCancelled )
		return;

	if( !FileExists( file_dir + file_name ) )
	{
		MessageBox( "Файл не найден!", "Ошибка.", MB_OK | MB_ICONWARNING ); 
		return;
	}

	//show validation window
	if( !dlgValidate )
	{
		dlgValidate = new CValidateDlg( this );
		dlgValidate->SetMessageStr( "Проверка файла..." );
		ProcessMessages();
	}

	SetCursor( LoadCursor(NULL, IDC_WAIT) );


	if( GetEfnFiletype( file_dir + file_name ) == EF_ANIMATION )
	{
		//check file prefix
		CEfnCharacter* efn_anim = new CEfnCharacter;

		if( efn_anim->LoadFromFile( file_dir + file_name ) )
		{
			pAttachment = efn_anim;
			pAttachment->SetPaths( file_dir, file_name );


			//prepare data for rendering
			pAttachment->Prepare( g_pd3dDevice );

			SAFE_DELETE( nl->attachment );
			nl->attachment = pAttachment;
		}
		else
		{
			// load failed -- cleanup
			SAFE_DELETE( efn_anim ); 
			bLoadFailed = true;
		}
	}
	else if( GetEfnFiletype( file_dir + file_name ) == EF_MESHLIST )
	{
		//check file prefix
		CEfnMeshlist* efn_meshlist = new CEfnMeshlist;

		if( efn_meshlist->LoadFromFile( file_dir + file_name ) )
		{
			efn_meshlist->SetParent( this );
			efn_meshlist->SetNullIcon( pNullIco );

			pAttachment = efn_meshlist;
			pAttachment->SetPaths( file_dir, file_name );


			//prepare data for rendering
			pAttachment->Prepare( g_pd3dDevice );

			SAFE_DELETE( nl->attachment );
			nl->attachment = pAttachment;
		}
		else
		{
			// load failed -- cleanup
			SAFE_DELETE( efn_meshlist ); 
			bLoadFailed = true;
		}
	}
	else
	{
		bLoadFailed = true;
	}
	
	if( bLoadFailed )
	{
		MessageBox( "Failed to load file! Undecognized file format.", "Error!", MB_OK | MB_ICONERROR );
	}
	else
	{
		//delete previous attachment node
		HTREEITEM hAttachment = m_rtSceneContents.GetChildItem( hSelected );

		if( hAttachment )
			m_rtSceneContents.DeleteItem( hAttachment );

		//insert attachment node in tree
		if( pAttachment && hSelected )
		{	
			DWORD uiMask	= TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
			m_rtSceneContents.InsertItem( uiMask, pAttachment->GetFilename().data(), 28, 28, 0, 0, NULL, hSelected, NULL );
		}
	}

	//restore cursor
	SetCursor( LoadCursor(NULL, IDC_ARROW) );

	SAFE_DELETE( dlgValidate );
}

void CCookieMachineDlg::OnMenuScnNullDetach()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	HTREEITEM	hSelected	= m_rtSceneContents.GetSelectedItem();
	CString		strSelectedItemText = m_rtSceneContents.GetItemText( hSelected );

	CMesh* pMesh = ((CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist())->GetMeshByName( "Nulls" );

	if( !pMesh )
		return;

	CNULL* pNull = pMesh->GetNullByName( strSelectedItemText.GetBuffer(0) );

	if( !pNull )
		return;
	
	SAFE_DELETE( pNull->attachment );

	HTREEITEM hAttachment = m_rtSceneContents.GetChildItem( hSelected );

	if( hAttachment )
		m_rtSceneContents.DeleteItem( hAttachment );

	m_rtSceneContents.SelectItem( hSelected );
}

void CCookieMachineDlg::OnMenuScnNullsCreateNew()
{
	OnMenuSceneNullsNewByPos();
}

void CCookieMachineDlg::OnMenuScnNullsDetachAll()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;


	CEfnMeshlist*	pMeshlist	= (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();
	CMesh*			pMesh		= pMeshlist->GetMeshByName( "Nulls" );

	if( pMesh )
	{
		// detach all nulls
		for( int i = 0; i < pMesh->GetNullsCount(); i++ )
			SAFE_DELETE( pMesh->GetNullByIndex( i )->attachment );
	}

	HTREEITEM hNull = m_rtSceneContents.GetChildItem( m_rtSceneContents.GetSelectedItem() );

	while( hNull )
	{
		HTREEITEM att = m_rtSceneContents.GetChildItem( hNull );

		if( att )
			m_rtSceneContents.DeleteItem( att );

		hNull = m_rtSceneContents.GetNextSiblingItem( hNull );
	}
}

void CCookieMachineDlg::OnMenuEfnNullRename( CString strCurrID, CString strNewID )
{
	if( strCurrID == strNewID )
		return;

	CEfnCharacter* efnChar = (CEfnCharacter*)pBaseScene;

	CNULL2* pNull = efnChar->GetNullByName( strNewID.GetBuffer() );
	
	CString _strNewID	= strNewID;
	CString _strCurrID	= strCurrID;

	_strNewID.MakeUpper();
	_strCurrID.MakeUpper();

	if( pNull && _strNewID != _strCurrID )
	{
		MessageBox( "Нуль с таким ID уже существует!", "Внимание!", MB_OK | MB_ICONINFORMATION );
		return;		
	}

	pNull = ((CEfnCharacter*)pBaseScene)->GetNullByName( strCurrID.GetBuffer() );
	
	if( !pNull )
		return;

	pNull->id = strNewID.GetBuffer();

	//rename child cust params
	for( int i = 0; i < efnChar->GetCustParamsCount(); i++ )
	{
		CCUSTPARAM* par = efnChar->GetCustParamByIndex(i);

		str_t strParID = par->id;
		str_t strNullID;

		strNullID = StripTypeHeader( strParID );

		if( strcmp( strNullID.data(), strCurrID.GetBuffer() ) == 0 )
		{
			par->id = ( strNewID + "." ).GetBuffer();
			par->id	+= strParID;
		}
	}

	m_rtSceneContents.SetItemText( m_rtSceneContents.GetSelectedItem(), strNewID );
}

void CCookieMachineDlg::OnMenuFileAssociations()
{
	CFileAssociationsDlg* dlg = new CFileAssociationsDlg( this );
	dlg->SetAppPath( m_strAppPath );
	dlg->DoModal();

	SAFE_DELETE( dlg );
}

void CCookieMachineDlg::OnMenuMeshlistMeshDuplicate()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN_MESHLIST )
		return;

	UINT			uiMask		= TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	CMesh*			pMesh		= ((CEfnMeshlist*)pBaseScene)->GetSelectedMesh();
	int				counter		= 2;
	CEfnMeshlist*	pMeshlist	= ((CEfnMeshlist*)pBaseScene);
	CString			strTemp;


	if( !pMesh )
		return;

	str_t mesh_name = pMesh->GetName();
	strTemp.Format( "%s%s", mesh_name.data(), "_copy" );

	CMesh* pTempMesh = pMeshlist->GetMeshByName( strTemp.GetBuffer() );

	while( pTempMesh )
	{
		strTemp.Format( "%s%s_%i", mesh_name.data(), "_copy", counter );
		pTempMesh = pMeshlist->GetMeshByName( strTemp.GetBuffer() );

		counter++;
	}


	CMesh* pMeshDupe = ((CEfnMeshlist*)pBaseScene)->AddMesh( strTemp.GetBuffer() );

	pMesh = ((CEfnMeshlist*)pBaseScene)->GetMeshByName( mesh_name.data() );

	if( !pMesh )
		return;

	if( pMeshDupe )
	{
		pMeshDupe->InitFromCMesh( pMesh );
		pMeshDupe->SetName( strTemp.GetBuffer() );
		pMeshDupe->CalcOOBB();
		pMeshDupe->CalcBoundingSphere();

		if( pMeshDupe->GetVerteciesCount() == pMeshDupe->GetIndicesCount() )
			pMeshDupe->CalcReverseIndex();
		else
			pMeshDupe->CalcReverseIndex();

		pMeshDupe->Prepare( g_pd3dDevice );

		HTREEITEM hMeshes	= m_rtSceneContents.FindTreeItemByName( "Meshes" );
		HTREEITEM hMesh		= m_rtSceneContents.InsertItem( uiMask, strTemp.GetBuffer(), 0, 0, 0, 0, 0, hMeshes, TVI_SORT ); 
		m_rtSceneContents.SelectItem( hMesh );
	}

	UpdateMeshesInfoInExplorer();
}

void CCookieMachineDlg::OnMenuMeshlistMeshesCreateCube()
{
	OnMenuObjectCreateMesh( GEOM_CUBE );
}

void CCookieMachineDlg::OnMenuMeshlistMeshesCreatePoly()
{
	OnMenuObjectCreateMesh( GEOM_TRI );
}

void CCookieMachineDlg::OnMenuMeshlistMeshesCreateQuad()
{
	OnMenuObjectCreateMesh( GEOM_QUAD );
}

void CCookieMachineDlg::OnMenuScnNullRename( CString strCurrID, CString strNewID )
{
	if( strCurrID == strNewID )
		return;

	CEfnMeshlist*	pMeshlist	= (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();
	CNULL*			pNull		= pMeshlist->GetMeshByName( "Nulls" )->GetNullByName( strCurrID.GetBuffer(0) );
	CSceneFile*		scene		= (CSceneFile*)pBaseScene;
	HTREEITEM		hSelected	= m_rtSceneContents.GetSelectedItem();

	CString _strCurrID	= strCurrID;
	CString _strNewID	= strNewID;

	_strCurrID.MakeUpper();
	_strNewID.MakeUpper();

	if( pMeshlist->GetMeshByName2( "Nulls" )->GetNullByName2( strNewID.GetBuffer() ) && _strCurrID != _strNewID )
	{
		MessageBox( "Нуль с таким ID уже существует!", "Внимание!", MB_OK | MB_ICONINFORMATION );
		return;
	}

	pNull->id = strNewID.GetBuffer();

	m_rtSceneContents.SetItemText( hSelected, strNewID );
	m_rtSceneContents.SortChildren( m_rtSceneContents.GetParentItem( hSelected ) );
	m_rtSceneContents.SelectItem( hSelected );

	scene->RenameNullInCurves( strCurrID.GetBuffer(), strNewID.GetBuffer() );

	//rename null in curves in treeview
	HTREEITEM hCurves	= m_rtSceneContents.FindTreeItemByName( "Curves" );
	HTREEITEM hCurve	= m_rtSceneContents.GetChildItem( hCurves );

	while( hCurve )
	{
		HTREEITEM hNull	= m_rtSceneContents.GetChildItem( hCurve );
		
		while( hNull )
		{
			if( m_rtSceneContents.GetItemText( hNull ) == strCurrID )
				m_rtSceneContents.SetItemText( hNull, strNewID );

			hNull = m_rtSceneContents.GetNextSiblingItem( hNull );
		}

		hCurve = m_rtSceneContents.GetNextSiblingItem( hCurve );
	}

	scene->PrepareCurves();
}

void CCookieMachineDlg::OnMenuScnNullDelete()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	HTREEITEM		hSelected	= m_rtSceneContents.GetSelectedItem();
	CEfnMeshlist*	pMeshlist	= (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();
	CString			tmp, tmp2;
	CSceneFile*		scene		= (CSceneFile*)pBaseScene;
	
	tmp		= m_rtSceneContents.GetItemText( hSelected );
	tmp2	= "Вы действительно хотите удалить объект \"" + tmp + "\"?";

	if( MessageBox( tmp2, "Внимание!", MB_YESNO | MB_ICONWARNING  ) == IDYES )
	{
		pMeshlist->GetMeshByName( "Nulls" )->RemoveNull( tmp.GetBuffer(0) );
		m_rtSceneContents.DeleteItem( hSelected );
	}

	scene->RemoveNullFromCurves( tmp.GetBuffer(0) );

	//remove null from curves in treeview
	HTREEITEM hCurves	= m_rtSceneContents.FindTreeItemByName( "Curves" );
	HTREEITEM hCurve	= m_rtSceneContents.GetChildItem( hCurves );

	while( hCurve )
	{
		HTREEITEM hNull	= m_rtSceneContents.GetChildItem( hCurve );
		
		while( hNull )
		{
			if( m_rtSceneContents.GetItemText( hNull ) == tmp )
			{
				m_rtSceneContents.DeleteItem( hNull );
				hNull = m_rtSceneContents.GetChildItem( hCurve );
			}


			hNull = m_rtSceneContents.GetNextSiblingItem( hNull );
		}

		hCurve = m_rtSceneContents.GetNextSiblingItem( hCurve );
	}

	scene->PrepareCurves();
}

void CCookieMachineDlg::OnMenuScnMeshDuplicate()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	UINT			uiMask		= TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	CEfnMeshlist*	pMeshlist	= (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();
	CMesh*			pMesh		= pMeshlist->GetSelectedMesh();
	CString			strTemp;
	int				counter		= 2;
	HTREEITEM		hSelectedMesh = m_rtSceneContents.GetSelectedItem();

	if( !pMesh )
		return;

	str_t mesh_name = pMesh->GetName();

	strTemp.Format( "%s%s", mesh_name.data(), "_copy" );

	CMesh* pTempMesh = pMeshlist->GetMeshByName( strTemp.GetBuffer() );

	while( pTempMesh )
	{
		strTemp.Format( "%s%s_%i", mesh_name.data(), "_copy", counter );
		pTempMesh = pMeshlist->GetMeshByName( strTemp.GetBuffer() );

		counter++;
	}

    CMesh* pMeshDupe = pMeshlist->AddMesh( strTemp.GetBuffer() );

	pMesh = pMeshlist->GetMeshByName( mesh_name );

	if( !pMesh )
		return;

	HTREEITEM hNewMesh = NULL;

	if( pMeshDupe )
	{
		pMeshDupe->InitFromCMesh( pMesh );
		pMeshDupe->SetName( strTemp.GetBuffer() );
		pMeshDupe->CalcOOBB();
		pMeshDupe->CalcBoundingSphere();

		if( pMeshDupe->GetVerteciesCount() == pMeshDupe->GetIndicesCount() )
            pMeshDupe->CalcReverseIndex();
		else
			pMeshDupe->CalcReverseIndex();

		((CSceneFile*)pBaseScene)->Prepare( g_pd3dDevice );

		HTREEITEM hMeshes = m_rtSceneContents.FindTreeItemByName( "Meshes" );
		hNewMesh = m_rtSceneContents.InsertItem( uiMask, strTemp, 0, 0, 0, 0, 0, hMeshes, TVI_SORT ); 
	}

	m_rtSceneContents.SelectItem( hNewMesh );
	pMeshlist->DeselectAllMeshes();
	pMeshlist->SelectMesh( strTemp.GetBuffer() );
}

void CCookieMachineDlg::OnMenuScnMeshRename( CString strCurrID, CString strNewID )
{
	if( strCurrID == strNewID )
		return;

	CEfnMeshlist* pMeshlist = (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();

	CString _strCurrID	= strCurrID;
	CString _strNewID	= strNewID;

	_strCurrID.MakeUpper();
	_strNewID.MakeUpper();

	if( pMeshlist->GetMeshByName2( strNewID.GetBuffer() ) && _strNewID != _strCurrID )
	{
		MessageBox( "Меш с таким ID уже существует!", "Внимание!", MB_OK | MB_ICONINFORMATION );
		return;
	}

	CMesh* msh = pMeshlist->GetMeshByName( strCurrID.GetBuffer() );

	if( !msh )
		return;

	msh->SetName( strNewID.GetBuffer() );

	HTREEITEM hMeshItem = m_rtSceneContents.GetSelectedItem();

	m_rtSceneContents.SetItemText( hMeshItem, strNewID );
	m_rtSceneContents.SortChildren( m_rtSceneContents.GetParentItem( hMeshItem ) );
	m_rtSceneContents.SelectItem( hMeshItem );
}

void CCookieMachineDlg::OnMenuScnMeshDelete()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;


	HTREEITEM		hSelected = m_rtSceneContents.GetSelectedItem();
	CEfnMeshlist*	pMeshlist = (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();
	CString			tmp, tmp2;
	

	tmp		= m_rtSceneContents.GetItemText( hSelected );
	tmp2	= "Вы действительно хотите удалить объект \"" + tmp + "\"?";

	if( MessageBox( tmp2, "Внимание!", MB_YESNO | MB_ICONWARNING  ) == IDYES )
	{
		pMeshlist->DeleteMesh( tmp.GetBuffer() );
		pMeshlist->SetNullIcon( pNullIco );
		m_rtSceneContents.DeleteItem( hSelected );

		((CSceneFile*)pBaseScene)->Prepare( g_pd3dDevice );
	}
}

void CCookieMachineDlg::OnMenuScnMeshesCreateBox()
{
	OnMenuObjectCreateMesh( GEOM_CUBE );
}

void CCookieMachineDlg::OnMenuScnMeshesCreateTriangle()
{
	OnMenuObjectCreateMesh( GEOM_TRI );
}

void CCookieMachineDlg::OnMenuScnMeshesCreateQuad()
{
	OnMenuObjectCreateMesh( GEOM_QUAD );
}

void CCookieMachineDlg::OnMenuScnTextureBind2Cam()
{
	OnMenuScnQuickBind( "Texture" );
}

void CCookieMachineDlg::OnMenuScnMeshBind2Camera()
{
	OnMenuScnQuickBind( "Mesh2Cam" );
}

void CCookieMachineDlg::OnMenuScnMeshBind2Texture()
{
	OnMenuScnQuickBind( "Mesh2Tex" );
}

void CCookieMachineDlg::OnMenuScnLightBind2Camera()
{
	OnMenuScnQuickBind( "Light" );
}

void CCookieMachineDlg::OnMenuScnCustObjBind2Camera()
{
	OnMenuScnQuickBind( "CustObject" );
}

void CCookieMachineDlg::OnMenuMacroRecStart()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN )
		return;

	if( !m_bIsMacroRecStarted )
	{
		m_bIsMacroRecStarted = true;
		m_vecMacrosDeletedFrames.clear();
		m_mapAttaches.clear();
		m_mapAttachesTex.clear();
	
		//get path
		CString				strFilter	= "Macros Files (*.mcr)|*.mcr|";
		CCustomFileDialog*	dlg			= new CCustomFileDialog( FALSE, "*.mcr", 0, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_ENABLETEMPLATE | OFN_ENABLESIZING, strFilter, NULL );
		dlg->m_pOFN->lpstrInitialDir	= m_appSettings.FilesPath;
		//dlg->m_pOFN->lpfnHook			= OpenDlgExProc;
		dlg->m_pOFN->FlagsEx			= OFN_EX_NOPLACESBAR;
		dlg->m_pOFN->lpTemplateName		= MAKEINTRESOURCE( IDD_OPENFILE_EXT );
		dlg->m_pOFN->hInstance			= GetModuleHandle(NULL);

		//user confirmed "open file dialog"
		if( dlg->DoModal() == IDOK )
		{
			m_strMacrosPath = dlg->GetPathName().GetBuffer();

			str_t temp_path = "Cookie Machine -- " + pBaseScene->GetFolder() + pBaseScene->GetFilename() + " (Recording macro...)"; 
			this->SetWindowText( temp_path.data() );

			DeleteFile( m_strMacrosPath.data() );
		}
		else
			m_bIsMacroRecStarted = false;

		SAFE_DELETE( dlg );
	}
	else
		MessageBox( "Запись макроса уже начата!", "Внимание!", MB_OK );
}

void CCookieMachineDlg::OnMenuMacroRecEnd()
{
	if( !m_bIsMacroRecStarted )
		return;

	m_bIsMacroRecStarted = false;

	FILE* file = fopen( m_strMacrosPath.data(), "wb" );

	if( !file )
		return;

	int		iTemp, iTemp2;

	if( file )
	{
		//store deleted frames
		iTemp = m_vecMacrosDeletedFrames.size();
		fwrite( &iTemp, sizeof(int), 1, file );

		for( int i = 0; i < iTemp; i++ )
		{
			iTemp2 = m_vecMacrosDeletedFrames[i];
			fwrite( &iTemp2, sizeof(int), 1, file );
		}

		//store zoffsets
		int fr_count = ( (CEfnCharacter*)pBaseScene )->GetFramesCount();
		fwrite( &fr_count, sizeof(int), 1, file );

		for( int i = 0; i < fr_count; i++ )
		{
			float zoff = ( (CEfnCharacter*)pBaseScene )->GetZOffset( i );
			fwrite( &zoff, sizeof(float), 1, file );
		}

		//store animations
		int anim_count = ( (CEfnCharacter*)pBaseScene )->GetAnimationsCount();

		fwrite( &anim_count, sizeof(int), 1, file );

		for( int i = 0; i < anim_count; i++ )
		{
			EFN_ANIMATION* anim = ( (CEfnCharacter*)pBaseScene )->GetAnimationByIndex( i );
			
			int uu = anim->name.size();

			//name
			fwrite( &uu, sizeof(int), 1, file );
			fwrite( anim->name.data(), sizeof(char), uu, file );

			//frm count
			fwrite( &anim->frames_count, sizeof(int), 1, file );

			//frames
			fwrite( anim->frames_list, sizeof(int), anim->frames_count, file );
		}

		//int iTemp = 0;
		//
		////store model tex path
		//str_t mdl_tex_path = ((CEfnCharacter*)pBaseScene)->GetTexturePath();
		//iTemp = mdl_tex_path.size();
		//fwrite( &iTemp, sizeof(int), 1, file );

		//if( iTemp > 0 )
		//	fwrite( mdl_tex_path.data(), sizeof(char) * mdl_tex_path.size(), 1, file );


		////store attaches
		////cleanup attaches

		////attaches count
		//iTemp = m_mapAttaches.size(); 
		//fwrite( &iTemp, sizeof(int), 1, file );

		//for( int i = 0; i < ((CEfnCharacter*)pBaseScene)->GetNullsCount(); i++ )
		//{
		//	str_t		null_id = ((CEfnCharacter*)pBaseScene)->GetNullByIndex( i )->id;
		//	StrMap_iter	item = m_mapAttaches.find( null_id );

		//	//attaches map
		//	if( item != m_mapAttaches.end() )
		//	{
		//		str_t att = m_mapAttaches[null_id];

		//		iTemp = null_id.size();
		//		fwrite( &iTemp, sizeof(int), 1, file );
		//		fwrite( null_id.data(), sizeof(char) * null_id.size(), 1, file );

		//		iTemp = att.size();
		//		fwrite( &iTemp, sizeof(int), 1, file );
		//		fwrite( att.data(), sizeof(char) * att.size(), 1, file );

		//		CEfnCharacter* pChar = (CEfnCharacter*)((CEfnCharacter*)pBaseScene)->GetNullByIndex( i )->attachment;

		//		D3DMATERIAL9 mat = pChar->GetMaterial();
		//		fwrite( &mat, sizeof(D3DMATERIAL9), 1, file );
		//	}
		//}


		////store attaches TEXTURES
		//int iTempTex = m_mapAttachesTex.size();
		//fwrite( &iTempTex, sizeof(int), 1, file );

		//for( int i = 0; i < ((CEfnCharacter*)pBaseScene)->GetNullsCount(); i++ )
		//{
		//	str_t		null_id = ((CEfnCharacter*)pBaseScene)->GetNullByIndex( i )->id;
		//	StrMap_iter	item = m_mapAttachesTex.find( null_id );

		//	//attaches map
		//	if( item != m_mapAttachesTex.end() )
		//	{
		//		str_t att_tex = m_mapAttachesTex[null_id];

		//		int iTemp2;
		//		iTemp2 = null_id.size();
		//		fwrite( &iTemp2, sizeof(int), 1, file );
		//		fwrite( null_id.data(), sizeof(char), iTemp2, file );

		//		iTemp2 = att_tex.size();
		//		fwrite( &iTemp2, sizeof(int), 1, file );
		//		fwrite( att_tex.data(), sizeof(char), iTemp2, file );
		//	}
		//}

		//((CEfnCharacter*)pNull->attachment)->SetTexturePath( file_name );
		//((CEfnCharacter*)pNull->attachment)->ResetTexture( g_pd3dDevice );
			
		//m_mapAttachesTex[pNull->id] = file_name;
	}

	str_t temp_path = "Cookie Machine -- " + pBaseScene->GetFolder() + pBaseScene->GetFilename(); 
	this->SetWindowText( temp_path.data() );

	/*m_mapAttaches.clear();
	m_mapAttachesTex.clear();*/
	m_vecMacrosDeletedFrames.clear();	

	fclose( file );
}

void CCookieMachineDlg::OnMenuMacroLoadFromFile()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN )
		return;


	//if( m_bIsMacroRecStarted )
	//{
	//	MessageBox( "Запись макроса в процессе, вы должны завершить её\n\rперед тем, как закрузить макрос из файла!", "Внимание!", MB_OK );
	//	return;
	//}

	str_t			strMacrosPath;
	int				iTemp = 0, iTemp2 = 0;
	vector<int>		vecMacrosDeletedFrames;
	CEfnCharacter*	pEfnAnim = (CEfnCharacter*)pBaseScene;

	//get path
	CString			strFilter	= "Macros Files (*.mcr)|*.mcr|";
	CCustomFileDialog*	dlg			= new CCustomFileDialog( TRUE, "*.mcr", 0, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_ENABLETEMPLATE | OFN_ENABLESIZING, strFilter, NULL );
	dlg->m_pOFN->lpstrInitialDir	= m_appSettings.FilesPath;
	//dlg->m_pOFN->lpfnHook			= OpenDlgExProc;
	dlg->m_pOFN->FlagsEx			= OFN_EX_NOPLACESBAR;
	dlg->m_pOFN->lpTemplateName		= MAKEINTRESOURCE( IDD_OPENFILE_EXT );
	dlg->m_pOFN->hInstance			= GetModuleHandle(NULL);

	//user confirmed "open file dialog"
	if( dlg->DoModal() == IDOK )
	{
		strMacrosPath = dlg->GetPathName().GetBuffer();
	}
	else
	{
		SAFE_DELETE( dlg );
		return;
	}

	FILE* file = fopen( strMacrosPath.data(), "rb" );

	if( file )
	{
		//read deleted frames
		fread( &iTemp, sizeof(int), 1, file );

		if( iTemp > 0 )
		{
			for( int i = 0; i < iTemp; i++ )
			{
				fread( &iTemp2, sizeof(int), 1, file );
				vecMacrosDeletedFrames.push_back( iTemp2 );
			}

			for( int i = 0; i < iTemp / 2; i++ )
			{
				pEfnAnim->DeleteFrames( vecMacrosDeletedFrames[i * 2 + 0], vecMacrosDeletedFrames[i * 2 + 1] );
				pEfnAnim->Prepare( g_pd3dDevice ); 
				pEfnAnim->SetCurrNextFrame( 0, 0 );
			}
		}

		//read zoffsets
		int fr_count;
		fread( &fr_count, sizeof(int), 1, file );

		if( fr_count > 0 )
		{
			for( int i = 0; i < fr_count; i++ )
			{
				float zoff;
				fread( &zoff, sizeof(float), 1, file );

				pEfnAnim->SetZOffset( i, zoff );
			}
		}

		//read animations
		int anim_count;

		fread( &anim_count, sizeof(int), 1, file );
		
		HTREEITEM hAnim = m_rtSceneContents.FindTreeItemByName( "Animations" );
		
		if( anim_count > 0 )
		{
			for( int i = 0; i < anim_count; i++ )
			{
				EFN_ANIMATION anim;
			
				int name_size;

				char str_tmp[64];

				//ZeroMemory( &anim, sizeof( EFN_ANIMATION ) );
				ZeroMemory( &str_tmp, sizeof( char ) * 64 );

				//name
				fread( &name_size, sizeof(int), 1, file );
				fread( &str_tmp, sizeof(char), name_size, file );

				anim.name = str_tmp;

				//frm count
				fread( &anim.frames_count, sizeof(int), 1, file );

				anim.frames_list = new int[anim.frames_count];

				//frames
				fread( anim.frames_list, sizeof(int), anim.frames_count, file );

				if( pEfnAnim->GetAnimationByName( anim.name ) ) 
				{
					pEfnAnim->DeleteAnimationByName( anim.name );
				}

				if( anim.frames_list[0] < 0 || anim.frames_list[0] >= pEfnAnim->GetFramesCount() ||
					anim.frames_list[anim.frames_count - 1] < 0 || anim.frames_list[anim.frames_count - 1] >= pEfnAnim->GetFramesCount() )
				{
					MessageBox( ("Frames in animation \"" + anim.name + "\"are out of range!").c_str(), "", MB_ICONWARNING | MB_OK );
					SAFE_DELETE_ARRAY( anim.frames_list );
				}
				else
				{
					pEfnAnim->AddAnimation( &anim );
					m_rtSceneContents.InsertItem2( anim.name.data(), 8, new TNODE( TREENODE::N_ANIMATION, pBaseScene ), hAnim, TVI_SORT );
				}
			}
		}

		UpdateUI();

	//	int		iAttachesnum, iTemp;
	//	char	szBuff[512];
	//	str_t	strTemp;

	//	//load model's texture
	//	ZeroMemory( &szBuff, sizeof(szBuff) );
	//	fread( &iTemp, sizeof(int), 1, file );

	//	if( iTemp > 0 )
	//	{
	//		fread( (char*)&szBuff, sizeof(char), iTemp, file );
	//		strTemp = szBuff;

	//		((CEfnCharacter*)pBaseScene)->SetTexturePath( strTemp );
	//		((CEfnCharacter*)pBaseScene)->ResetTexture( g_pd3dDevice );
	//	}

	//	fread( &iAttachesnum, sizeof(int), 1, file );

	//	//load attaches
	//	for( int i = 0; i < iAttachesnum; i++ )
	//	{
	//		str_t null_id, attach_path;
	//		char	szBuffA[512];
	//		char	szBuffB[512];

	//		//read null id
	//		ZeroMemory( &szBuffA, sizeof(char) * 512 );
	//		fread( &iTemp, sizeof(int), 1, file );
	//		fread( (char*)&szBuffA, sizeof(char), iTemp, file );
	//		null_id = szBuffA;

	//		//read attach path
	//		ZeroMemory( szBuffB, sizeof(char) * 512 );
	//		fread( &iTemp, sizeof(int), 1, file );
	//		fread( (char*)&szBuffB, sizeof(char), iTemp, file );
	//		attach_path = szBuffB;

	//		OnMenuEfnNullAttachTo( null_id, attach_path );

	//		CEfnCharacter* pChar = (CEfnCharacter*)((CEfnCharacter*)pBaseScene)->GetNullByName( null_id )->attachment;
	//		
	//		if( pChar )
	//		{
	//			D3DMATERIAL9 mat;
	//			fread( &mat, sizeof(D3DMATERIAL9), 1, file );

	//			pChar->SetMaterial( mat );
	//		}
	//	}


	//	fread( &iAttachesnum, sizeof(int), 1, file );

	//	//load attaches TEXTURES
	//	for( int i = 0; i < iAttachesnum; i++ )
	//	{
	//		str_t null_id, tex_attach_path;
	//		int iTemp;
	//		char	szBuffA[512];
	//		char	szBuffB[512];

	//		//read null id
	//		ZeroMemory( &szBuffA, sizeof(char) * 512 );
	//		fread( &iTemp, sizeof(int), 1, file );
	//		fread( (char*)&szBuffA, sizeof(char), iTemp, file );
	//		null_id = szBuffA;

	//		//read attach path
	//		ZeroMemory( szBuffB, sizeof(char) * 512 );
	//		fread( &iTemp, sizeof(int), 1, file );
	//		fread( (char*)&szBuffB, sizeof(char), iTemp, file );
	//		tex_attach_path = szBuffB;

	//		OnMenuEfnSetNullsTexture( null_id, tex_attach_path );
	//	}
	}

	fclose( file );

	SAFE_DELETE( dlg );
}

void CCookieMachineDlg::OnMenuScnCameraScript()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	HTREEITEM hSelected;
	CString hSelItemText;

	hSelected		= m_rtSceneContents.GetSelectedItem();
	hSelItemText	= m_rtSceneContents.GetItemText( hSelected );

	//get image from selected tree node and determine object type
	int image_idx = 0;
	m_rtSceneContents.GetItemImage( hSelected, image_idx, image_idx );

	
	//prepare last edited item and assign it to scene
	CSceneFile*	scene = ((CSceneFile*)pBaseScene);
	scene->m_strLastEditedObj = ("Camera." + hSelItemText).GetBuffer();

	ShowScriptsWindow();
}

void CCookieMachineDlg::OnMenuScnLightScript()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	HTREEITEM hSelected;
	CString hSelItemText;

	hSelected		= m_rtSceneContents.GetSelectedItem();
	hSelItemText	= m_rtSceneContents.GetItemText( hSelected );

	//get image from selected tree node and determine object type
	int image_idx = 0;
	m_rtSceneContents.GetItemImage( hSelected, image_idx, image_idx );

	
	//prepare last edited item and assign it to scene
	CSceneFile*	scene = ((CSceneFile*)pBaseScene);
	scene->m_strLastEditedObj = ("Light." + hSelItemText).GetBuffer();

	ShowScriptsWindow();
}

void CCookieMachineDlg::OnMenuScnTextureScript()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	HTREEITEM	hSelected;
	CString		hSelItemText;

	hSelected		= m_rtSceneContents.GetSelectedItem();
	hSelItemText	= m_rtSceneContents.GetItemText( hSelected );

	//get image from selected tree node and determine object type
	int image_idx = 0;
	m_rtSceneContents.GetItemImage( hSelected, image_idx, image_idx );

	
	//prepare last edited item and assign it to scene
	CSceneFile*	scene			= ((CSceneFile*)pBaseScene);
	scene->m_strLastEditedObj	= ("Background." + hSelItemText).GetBuffer();

	ShowScriptsWindow();
}

void CCookieMachineDlg::RunCurrentGame()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;


	HTREEITEM hSelected, hParent, hParentParent;
	CString strParentText, strParentParentText, hSelItemText;

	hSelected		= m_rtSceneContents.GetSelectedItem();
	hSelItemText	= m_rtSceneContents.GetItemText( hSelected );

	if( hSelected >= 0 )
		hParent = m_rtSceneContents.GetParentItem( hSelected );

	if( hParent >= 0 )
	{
		hParentParent	= m_rtSceneContents.GetParentItem( hParent );
		strParentText	= m_rtSceneContents.GetItemText( hParent );
	}
	

	if( strParentText == "Nulls" && hParentParent == NULL )
	{
		SetCurrentDirectory( m_appSettings.EngineFolder );

		CSceneFile*		pScene		= ((CSceneFile*)pBaseScene);
		CEfnMeshlist*	pMeshlist	= (CEfnMeshlist*)pScene->GetMeshlist();

		str_t strResult = " -scn " + pScene->GetLevelID() + "\\" + pScene->GetSceneID() + " -null " + hSelItemText.GetBuffer();

		g_MemMappingsMgr->PostScriptToEngine( "g_Engine:Exit();", false );

		CWnd* pWnd = FindWindow( NULL, "Яйца Фаберже" );

		int iCounter = 0;

		while( pWnd )
		{
			Sleep( 100 );
			pWnd = FindWindow( NULL, "Яйца Фаберже" );

			iCounter++;

			if( iCounter > 50 )
				break;
		}

		if( !pWnd )
		{
			CString strRes = strResult.data();
			CString strEngineExePath;

			if( m_appSettings.EngineDebugModeOn )
			{
				strEngineExePath = m_appSettings.EngineFolder + "\\Game_debug.exe";
			}
			else
			{
				strEngineExePath = m_appSettings.EngineFolder + "\\Game.exe";
			}

			if( FileExists( strEngineExePath.GetBuffer() ) == false )
			{
				MessageBox( "Engine executable is missing!", "Error", MB_OK | MB_ICONERROR );
				return;
			}

			WinExec( strEngineExePath + " " + m_appSettings.StartParams + strRes, SW_SHOW );	
			pWnd = FindWindow( NULL, "Яйца Фаберже" );

			if( m_appSettings.SwitchBackOnDebug )
			{
				SetForegroundWindow();
				SetFocus();
			}
			else
			{
				pWnd->SetForegroundWindow();
				pWnd->SetFocus();
			}


			ProcessMessages();				
		}
	}
}

void CCookieMachineDlg::OnMenuEfnScaleNew()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN && 
		pBaseScene->GetType() != FT_EFN_MESHLIST &&
		pBaseScene->GetType() != FT_SCN )
	{
		return;
	}

	CModelNameDlg* dlgModelScale = new CModelNameDlg(this);

	dlgModelScale->SetCaption( "Scale:" );
	dlgModelScale->SetString( "16.5289" );
	dlgModelScale->DoModal();

	float fScaleCoef = atof( dlgModelScale->GetString().GetBuffer() );

	fScaleCoef = 1.0f / fScaleCoef;

	D3DXMATRIX scale;
	D3DXMatrixScaling( &scale, fScaleCoef, fScaleCoef, fScaleCoef );

	if( pBaseScene->GetType() == FT_EFN )
	{
		((CEfnCharacter*)pBaseScene)->Scale( scale );
	}
	else if( pBaseScene->GetType() == FT_EFN_MESHLIST )
	{
		((CEfnMeshlist*)pBaseScene)->Scale( scale );
	}
	else if( pBaseScene->GetType() == FT_SCN )
	{
		CSceneFile*		pScene		= (CSceneFile*)pBaseScene;
		CEfnMeshlist*	pMeshlist	= (CEfnMeshlist*)pScene->GetMeshlist();

		pMeshlist->Scale( scale );
		pScene->Scale( scale );
	}

	pBaseScene->Prepare( g_pd3dDevice );

	SAFE_DELETE( dlgModelScale );
}

void CCookieMachineDlg::OnMenuEfnNullsRescaleAll()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN )
		return;

	CModelNameDlg *dlgModelName = new CModelNameDlg( this );

	dlgModelName->SetCaption( "Nulls scale:" );
	dlgModelName->DoModal();
	
	float fScale = atof( dlgModelName->GetString().GetBuffer(0) );

	for( int i = 0; i < ((CEfnCharacter*)pBaseScene)->GetNullsCount(); i++ )
	{
		CNULL2* pNull = ((CEfnCharacter*)pBaseScene)->GetNullByIndex( i );

		for( int j = 0; j < ((CEfnCharacter*)pBaseScene)->GetFramesCount(); j++ )
		{
			pNull->matrices[j].scl.x = fScale;
			pNull->matrices[j].scl.y = fScale;
			pNull->matrices[j].scl.z = fScale;
		}
	}

	SAFE_DELETE(dlgModelName);
}

void CCookieMachineDlg::OnMenuShowObjPropertiesWnd()
{
	CMenu* menuCull = this->GetMenu();

	if( m_bIsObjPropertiesWndVisible )
	{
		ShowObjectPropertiesWnd( false );

		m_bIsObjPropertiesWndVisible = false;
		menuCull->CheckMenuItem( ID_MENU_OBJ_PROPERTIES_WND, MF_UNCHECKED );
	}
	else
	{
		m_bIsObjPropertiesWndVisible = true;
		ShowObjectPropertiesWnd( true );
		menuCull->CheckMenuItem( ID_MENU_OBJ_PROPERTIES_WND, MF_CHECKED );
	}
	
	UpdateUISizing();
	HandlePossibleSizeChange();
}

BOOL DoNotify( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	#define lpnm   ((LPNMHDR)lParam)
	#define lpnmTB ((LPNMTOOLBAR)lParam)

	if( lpnm->idFrom != ID_TOOLBAR )
		return false;

	RECT      rc;
	TPMPARAMS tpm;
	HMENU     hPopupMenu = NULL;
	HMENU     hMenuLoaded;
	BOOL      bRet = FALSE;

	switch( lpnm->code )
	{
		case TBN_DROPDOWN:
			SendMessage( lpnmTB->hdr.hwndFrom, TB_GETRECT, (WPARAM)lpnmTB->iItem, (LPARAM)&rc );

			MapWindowPoints( lpnmTB->hdr.hwndFrom, HWND_DESKTOP, (LPPOINT)&rc, 2 );                         

			tpm.cbSize		= sizeof( TPMPARAMS );
			tpm.rcExclude	= rc;

			switch( (WPARAM)lpnmTB->iItem )
			{
				case IDT_AXIS_ALIGN:
					hMenuLoaded		= LoadMenu( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MENU_AXIS) ); 
					hPopupMenu		= GetSubMenu( LoadMenu(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MENU_AXIS)),0 );

					TrackPopupMenuEx( hPopupMenu, TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL, rc.left, rc.bottom, hwnd, &tpm ); 

					break;

				case IDT_FLOORS:
					hMenuLoaded		= LoadMenu( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MENU_FLOORS) ); 
					hPopupMenu		= GetSubMenu( LoadMenu(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MENU_FLOORS)),0 );

					TrackPopupMenuEx( hPopupMenu, TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL, rc.left, rc.bottom, hwnd, &tpm ); 

					break;
			}

			DestroyMenu( hMenuLoaded );			
			return FALSE;
	}

	return FALSE;
}

CCookieMachineDlg* __clscpy;

void CCookieMachineDlg::ShowAxisAlignMenu()
{
	__clscpy = this;
	ShowAxisAlignMenuGlob();
}

void CCookieMachineDlg::ShowFloorsMenu()
{
	__clscpy = this;
	ShowFloorsMenuGlob();
}

void CCookieMachineDlg::ShowFloorsMenuGlob()
{
	/*RECT      rc;
	TPMPARAMS tpm;
	HMENU     hPopupMenu = NULL;
	HMENU     hMenuLoaded;
	BOOL      bRet = FALSE;

	HWND pToolbar = GetDlgItem( __clscpy->GetSafeHwnd(), ID_TOOLBAR );

	SendMessage( pToolbar, TB_GETRECT, (WPARAM)IDT_FLOORS, (LPARAM)&rc );

	MapWindowPoints( pToolbar, HWND_DESKTOP, (LPPOINT)&rc, 2 );                         

	tpm.cbSize		= sizeof( TPMPARAMS );
	tpm.rcExclude	= rc;
	hMenuLoaded		= LoadMenu( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MENU_FLOORS) ); 
	hPopupMenu		= GetSubMenu( LoadMenu(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MENU_FLOORS)),0 );

	TrackPopupMenuEx( hPopupMenu, TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL, rc.left, rc.bottom, __clscpy->GetSafeHwnd(), &tpm ); 

	DestroyMenu( hMenuLoaded );*/

	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	if( ((CSceneFile*)pBaseScene)->GetCellSize() == 0.0f )
		OnMenuScnFloorRecalc();

	CValidateDlg* dlgValidate = NULL;

	//show validation window
	SetCursor( LoadCursor(NULL, IDC_WAIT) );

	dlgValidate = new CValidateDlg( this );

	if( !dlgValidate )
		return;

	dlgValidate->SetMessageStr( "Расчёт сетки проходимостей..." );
	ProcessMessages();

	((CSceneFile*)pBaseScene)->SetProgressWnd( (CWnd*)dlgValidate );
	
	((CSceneFile*)pBaseScene)->RecalcGrid();
	pBaseScene->Prepare( g_pd3dDevice );

	SetCursor( LoadCursor(NULL, IDC_ARROW) );

	SAFE_DELETE( dlgValidate );
}

void ShowAxisAlignMenuGlob()
{
	RECT      rc;
	TPMPARAMS tpm;
	HMENU     hPopupMenu = NULL;
	HMENU     hMenuLoaded;
	BOOL      bRet = FALSE;

	HWND pToolbar = GetDlgItem( __clscpy->GetSafeHwnd(), ID_TOOLBAR );

	SendMessage( pToolbar, TB_GETRECT, (WPARAM)IDT_AXIS_ALIGN, (LPARAM)&rc );

	MapWindowPoints( pToolbar, HWND_DESKTOP, (LPPOINT)&rc, 2 );                         

	tpm.cbSize		= sizeof( TPMPARAMS );
	tpm.rcExclude	= rc;
	hMenuLoaded		= LoadMenu( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MENU_AXIS) ); 
	hPopupMenu		= GetSubMenu( LoadMenu(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MENU_AXIS)),0 );

	TrackPopupMenuEx( hPopupMenu, TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL, rc.left, rc.bottom, __clscpy->GetSafeHwnd(), &tpm ); 

	DestroyMenu( hMenuLoaded );			
}

BOOL CCookieMachineDlg::OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult )
{
	return CDialog::OnNotify( wParam, lParam, pResult );

	str_t func_decl_str = "BOOL CCookieMachineDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)";

	try
	{
		DoNotify( this->GetSafeHwnd(), 0, wParam, lParam );
		return CDialog::OnNotify( wParam, lParam, pResult );
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}
}

void CCookieMachineDlg::OnMenuAxisX()
{
	m_AxisAlign = AAE_X;
	SetToolbarButtonImg( IDT_AXIS_ALIGN, 10 );
}

void CCookieMachineDlg::OnMenuAxisY()
{
	m_AxisAlign = AAE_Y;
	SetToolbarButtonImg( IDT_AXIS_ALIGN, 11 );
}

void CCookieMachineDlg::OnMenuAxisZ()
{
	m_AxisAlign = AAE_Z;
	SetToolbarButtonImg( IDT_AXIS_ALIGN, 12 );
}

void CCookieMachineDlg::OnMenuAxisXY()
{
	m_AxisAlign = AAE_XY;
	SetToolbarButtonImg( IDT_AXIS_ALIGN, 13 );
}

void CCookieMachineDlg::OnMenuAxisXZ()
{
	m_AxisAlign = AAE_XZ;
	SetToolbarButtonImg( IDT_AXIS_ALIGN, 14 );
}

void CCookieMachineDlg::OnMenuAxisYZ()
{
	m_AxisAlign = AAE_YZ;
	SetToolbarButtonImg( IDT_AXIS_ALIGN, 15 );
}

void CCookieMachineDlg::PreInitDialog()
{
	CDialog::PreInitDialog();
	return;

	str_t func_decl_str = "void CCookieMachineDlg::PreInitDialog()";

	try
	{
		//create object properties dialogs
		m_pPropCustomParamDlg = new CPropCustomParamDlg( this );
		m_pPropCustomParamDlg->Create( IDD_PROP_CUSTOMPARAM, this );

		m_pPropNullDlg = new CPropNullDlg( this );
		m_pPropNullDlg->Create( IDD_PROP_NULL, this );

		m_pPropCameraDlg = new CPropCameraDlg( this );
		m_pPropCameraDlg->Create( IDD_PROP_CAMERA, this );

		m_pPropLightDlg = new CPropLightDlg( this );
		m_pPropLightDlg->Create( IDD_PROP_LIGHT, this );
		
		CDialog::PreInitDialog();

	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}
}

void CCookieMachineDlg::OnMenuXSIMeshDisconnectAllTris()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_XSI )
		return;

	SetCursor( LoadCursor( NULL, IDC_WAIT ) );

	HTREEITEM	hSelItem	= m_rtSceneContents.GetSelectedItem();
	HTREEITEM	hInfoItem	= m_rtSceneContents.FindTreeItemByName( "Info" );
	HTREEITEM	hItemVertCountInfo, hItemVertCountMesh;
	int			iVertCount	= 0;
	CString		id			= m_rtSceneContents.GetItemText( hSelItem );
	CXSIMesh*	pMesh		= ((CXSIFile*)pBaseScene)->GetMeshByName( id.GetBuffer() );
	CString		temp;

	
	for( int i = 0; i < ((CXSIFile*)pBaseScene)->GetMeshesCount(); i++ )
		iVertCount += ((CXSIFile*)pBaseScene)->GetMeshByIndex( i )->GetVerteciesCount();

	temp.Format( "Vertecies in scene: %d", iVertCount );
	hItemVertCountInfo = m_rtSceneContents.FindTreeItemByName( temp, hInfoItem );

	temp.Format( "Vertecies: %d", ((CXSIFile*)pBaseScene)->GetMeshByName( id.GetBuffer() )->GetVerteciesCount() );
	hItemVertCountMesh = m_rtSceneContents.FindTreeItemByName( temp, hSelItem );

	
	
	pMesh->DisconnectAllTris();
	((CXSIFile*)pBaseScene)->Prepare( g_pd3dDevice );



	iVertCount = 0;

	for( int i = 0; i < ((CXSIFile*)pBaseScene)->GetMeshesCount(); i++ )
		iVertCount += ((CXSIFile*)pBaseScene)->GetMeshByIndex( i )->GetVerteciesCount();

	temp.Format( "Vertecies in scene: %d", iVertCount );
	m_rtSceneContents.SetItemText( hItemVertCountInfo, temp.GetBuffer() );

	temp.Format( "Vertecies: %d", ((CXSIFile*)pBaseScene)->GetMeshByName( id.GetBuffer() )->GetVerteciesCount() );
	m_rtSceneContents.SetItemText( hItemVertCountMesh, temp.GetBuffer() );

	SetCursor( LoadCursor( NULL, IDC_ARROW ) );
}

void CCookieMachineDlg::OnMenuXSIMeshConnectAllTris()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_XSI )
		return;

	SetCursor( LoadCursor( NULL, IDC_WAIT ) );

	HTREEITEM	hSelItem	= m_rtSceneContents.GetSelectedItem();
	HTREEITEM	hInfoItem	= m_rtSceneContents.FindTreeItemByName( "Info" );
	HTREEITEM	hItemVertCountInfo, hItemVertCountMesh;
	int			iVertCount	= 0;
	CString		id			= m_rtSceneContents.GetItemText( hSelItem );
	CXSIMesh*	pMesh		= ((CXSIFile*)pBaseScene)->GetMeshByName( id.GetBuffer() );
	CString		temp;

	
	for( int i = 0; i < ((CXSIFile*)pBaseScene)->GetMeshesCount(); i++ )
		iVertCount += ((CXSIFile*)pBaseScene)->GetMeshByIndex( i )->GetVerteciesCount();

	temp.Format( "Vertecies in scene: %d", iVertCount );
	hItemVertCountInfo = m_rtSceneContents.FindTreeItemByName( temp, hInfoItem );

	temp.Format( "Vertecies: %d", ((CXSIFile*)pBaseScene)->GetMeshByName( id.GetBuffer() )->GetVerteciesCount() );
	hItemVertCountMesh = m_rtSceneContents.FindTreeItemByName( temp, hSelItem );

	
	
	pMesh->ConnectAllTris();
	((CXSIFile*)pBaseScene)->Prepare( g_pd3dDevice );



	iVertCount = 0;

	for( int i = 0; i < ((CXSIFile*)pBaseScene)->GetMeshesCount(); i++ )
		iVertCount += ((CXSIFile*)pBaseScene)->GetMeshByIndex( i )->GetVerteciesCount();

	temp.Format( "Vertecies in scene: %d", iVertCount );
	m_rtSceneContents.SetItemText( hItemVertCountInfo, temp.GetBuffer() );

	temp.Format( "Vertecies: %d", ((CXSIFile*)pBaseScene)->GetMeshByName( id.GetBuffer() )->GetVerteciesCount() );
	m_rtSceneContents.SetItemText( hItemVertCountMesh, temp.GetBuffer() );

	SetCursor( LoadCursor( NULL, IDC_ARROW ) );
}

void CCookieMachineDlg::OnMenuEfnMeshDisconnectAllTris()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_EFN )
		return;

	TNODE* node = ( TNODE* )m_rtSceneContents.GetCurrNodeData();

	if( !node || node->_type != TREENODE::N_MESH )
		return;

	SetCursor( LoadCursor( NULL, IDC_WAIT ) );

	HTREEITEM hInfo = m_rtSceneContents.FindTreeItemByName( "Info" );
	CString temp;
	temp.Format( "Vertecies in model: %i", ((CEfnCharacter*)pBaseScene)->GetVerteciesCount() );
	HTREEITEM hVCount = m_rtSceneContents.FindTreeItemByName( temp );


	((CEfnCharacter*)node->_data)->DisconnectAllTris();
	((CEfnCharacter*)node->_data)->Prepare( g_pd3dDevice );


	temp.Format( "Vertecies in model: %i", ((CEfnCharacter*)pBaseScene)->GetVerteciesCount() );
	m_rtSceneContents.SetItemText( hVCount, temp );

	SetCursor( LoadCursor( NULL, IDC_ARROW ) );
}

void CCookieMachineDlg::OnMenuEfnMeshConnectAllTris()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_EFN )
		return;

	TNODE* node = ( TNODE* )m_rtSceneContents.GetCurrNodeData();

	if( !node || node->_type != TREENODE::N_MESH )
		return;

	SetCursor( LoadCursor( NULL, IDC_WAIT ) );

	HTREEITEM hInfo = m_rtSceneContents.FindTreeItemByName( "Info" );
	CString temp;
	temp.Format( "Vertecies in model: %i", ((CEfnCharacter*)pBaseScene)->GetVerteciesCount() );
	HTREEITEM hVCount = m_rtSceneContents.FindTreeItemByName( temp );


	((CEfnCharacter*)node->_data)->ConnectAllTris();
	((CEfnCharacter*)node->_data)->Prepare( g_pd3dDevice );


	temp.Format( "Vertecies in model: %i", ((CEfnCharacter*)pBaseScene)->GetVerteciesCount() );
	m_rtSceneContents.SetItemText( hVCount, temp );

	SetCursor( LoadCursor( NULL, IDC_ARROW ) );
}

void CCookieMachineDlg::OnMenuScnFloorMaterial()
{
	if( !pBaseScene || m_bIsVertexEditingModeOn )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;


	CEfnMeshlist*	pMeshlist = (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();
	
	ListStrVec* pBinds = ((CSceneFile*)pBaseScene)->GetBinds();
	ListStrVec_iter iter;
	str_t id;

	for( iter = pBinds->begin(); iter != pBinds->end(); ++iter )
	{
		if( (*iter)[2] == "Scene" && (*iter)[4] == "Floor" )
		{
			id = (*iter)[3];
			break;
		}
	}

	CMesh* pMesh = pMeshlist->GetMeshByName( id );

	if( !pMesh )
		return;

	CMaterialDlg* dlg = new CMaterialDlg;
	dlg->m_Material = pMesh->GetMaterial();
	dlg->DoModal();
	
	for( iter = pBinds->begin(); iter != pBinds->end(); ++iter )
	{
		if( (*iter)[2] == "Scene" && (*iter)[4] == "Floor" )
		{
			pMesh = pMeshlist->GetMeshByName( (*iter)[3] );
			pMesh->SetMaterial( dlg->m_Material );	
		}
	}

	delete dlg;
}

void CCookieMachineDlg::OnMenuScnMeshInverseNormals()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_SCN )
		return;

	CEfnMeshlist*	pMeshlist	= (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();
	HTREEITEM		hItem		= m_rtSceneContents.GetSelectedItem();
	CString			strTemp		= m_rtSceneContents.GetItemText( hItem );

	
	if( !pMeshlist )
		return;

	pMeshlist->GetMeshByName( strTemp.GetBuffer() )->InverseNormals();
	((CSceneFile*)pBaseScene)->Prepare( g_pd3dDevice );
}

void CCookieMachineDlg::OnMenuScnMeshConnectTris()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_SCN )
		return;

	CEfnMeshlist*	pMeshlist	= (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();
	HTREEITEM		hItem		= m_rtSceneContents.GetSelectedItem();
	CString			strTemp		= m_rtSceneContents.GetItemText( hItem );

	if( !pMeshlist )
		return;

	pMeshlist->GetMeshByName( strTemp.GetBuffer() )->ConnectPolys();
	((CSceneFile*)pBaseScene)->Prepare( g_pd3dDevice );
}

void CCookieMachineDlg::OnMenuScnMeshDisconnectTris()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_SCN )
		return;

	CEfnMeshlist*	pMeshlist	= (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();
	HTREEITEM		hItem		= m_rtSceneContents.GetSelectedItem();
	CString			strTemp		= m_rtSceneContents.GetItemText( hItem );

	if( !pMeshlist )
		return;

	pMeshlist->GetMeshByName( strTemp.GetBuffer() )->DisconnectPolys();
	((CSceneFile*)pBaseScene)->Prepare( g_pd3dDevice );
}

void CCookieMachineDlg::OnMenuMeshlistMeshConnectTris()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_EFN_MESHLIST )
		return;

	SetCursor( LoadCursor( NULL, IDC_WAIT ) );

	HTREEITEM		hItem		= m_rtSceneContents.GetSelectedItem();
	CString			strTemp		= m_rtSceneContents.GetItemText( hItem );
	CMesh*			pMesh		= ((CEfnMeshlist*)pBaseScene)->GetMeshByName( strTemp.GetBuffer() );

	if( !pMesh )
		return;

	pMesh->ConnectPolys();
	((CEfnMeshlist*)pBaseScene)->Prepare( g_pd3dDevice );

	UpdateMeshesInfoInExplorer();
	SetCursor( LoadCursor( NULL, IDC_ARROW ) );
}

void CCookieMachineDlg::OnMenuMeshlistMeshDisconnectTris()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_EFN_MESHLIST )
		return;

	SetCursor( LoadCursor( NULL, IDC_WAIT ) );

	HTREEITEM		hItem		= m_rtSceneContents.GetSelectedItem();
	CString			strTemp		= m_rtSceneContents.GetItemText( hItem );
	CMesh*			pMesh		= ((CEfnMeshlist*)pBaseScene)->GetMeshByName( strTemp.GetBuffer() );

	if( !pMesh )
		return;

	pMesh->DisconnectPolys();
	((CEfnMeshlist*)pBaseScene)->Prepare( g_pd3dDevice );

	UpdateMeshesInfoInExplorer();

	SetCursor( LoadCursor( NULL, IDC_ARROW ) );
}

void CCookieMachineDlg::OnMenuMeshlistMeshInverseNormals()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_EFN_MESHLIST )
		return;

	HTREEITEM		hItem		= m_rtSceneContents.GetSelectedItem();
	CString			strTemp		= m_rtSceneContents.GetItemText( hItem );
	CMesh*			pMesh		= ((CEfnMeshlist*)pBaseScene)->GetMeshByName( strTemp.GetBuffer() );

	if( !pMesh )
		return;

	pMesh->InverseNormals();
	((CEfnMeshlist*)pBaseScene)->Prepare( g_pd3dDevice );
}

void CCookieMachineDlg::OnMenuMeshlistMeshesDeleteAll()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_EFN_MESHLIST )
		return;

	SetCursor( LoadCursor( NULL, IDC_WAIT ) );

	((CEfnMeshlist*)pBaseScene)->DeleteAllMeshes();
	((CEfnMeshlist*)pBaseScene)->Prepare( g_pd3dDevice );

	HTREEITEM hMeshes = m_rtSceneContents.FindTreeItemByName( "Meshes" );
	HTREEITEM hTemp = m_rtSceneContents.GetChildItem( hMeshes );

	while( hTemp )
	{
		m_rtSceneContents.DeleteItem( hTemp );
		hTemp = m_rtSceneContents.GetChildItem( hMeshes );
	}

	if( m_appSettings.ScnAlternateNullIco )
		((CEfnMeshlist*)pBaseScene)->SetNullIcon( pNullIco );

	UpdateMeshesInfoInExplorer();
	SetCursor( LoadCursor( NULL, IDC_ARROW ) );
}

void CCookieMachineDlg::OnMenuScnDeleteSelectedObjects()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_SCN )
		return;

	CEfnMeshlist* pMeshlist = (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();

	if( !pMeshlist )
		return;

	if( pMeshlist->IsVertexEditingMode() )
	{
		pMeshlist->GetSelectedMesh()->DeleteSelectedVertecies();
	}
	else
	{
		vector<str_t> sel;

		HTREEITEM hMeshes = m_rtSceneContents.FindTreeItemByName( "Meshes" );
		HTREEITEM hTemp = NULL;

		for( int i = 0; i < pMeshlist->GetMeshesCount(); i++ )
		{
			CMesh* pMesh = pMeshlist->GetMeshByIndex( i );

			if( pMesh->IsSelected() )
				sel.push_back( pMesh->GetName() );
		}
		
		pMeshlist->DeleteSelectedMeshes();
		pMeshlist->Prepare( g_pd3dDevice );

		for( UINT i = 0; i < sel.size(); i++ )
		{
			hTemp = m_rtSceneContents.FindTreeItemByName( sel[i], hMeshes );

			if( hTemp )
				m_rtSceneContents.DeleteItem( hTemp );
		}

		if( pMeshlist->IsAllNullsSelected() )
			OnMenuMeshlistNullsRemoveAll();

		if( m_appSettings.ScnAlternateNullIco )
			pMeshlist->SetNullIcon( pNullIco );
	}
}

void CCookieMachineDlg::OnMenuMeshlistDeleteSelectedObjects()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_EFN_MESHLIST )
		return;

	CEfnMeshlist* pMeshlist = (CEfnMeshlist*)pBaseScene;

	//delete selected vertecies from mesh
	if( pMeshlist->IsVertexEditingMode() )
	{
		pMeshlist->GetSelectedMesh()->DeleteSelectedVertecies();
	}
	else
	{
		vector<str_t> sel;

		HTREEITEM hMeshes	= m_rtSceneContents.FindTreeItemByName( "Meshes" );
		HTREEITEM hTemp		= NULL;

		for( int i = 0; i < pMeshlist->GetMeshesCount(); i++ )
		{
			CMesh* pMesh = pMeshlist->GetMeshByIndex( i );

			if( pMesh->IsSelected() && pMesh->GetName() != "Nulls" )
				sel.push_back( pMesh->GetName() );
		}

		for( UINT i = 0; i < sel.size(); i++ )
		{
			hTemp = m_rtSceneContents.FindTreeItemByName( sel[i], hMeshes );

			if( hTemp )
				m_rtSceneContents.DeleteItem( hTemp );

			pMeshlist->DeleteMesh( sel[i] );
		}

		pMeshlist->Prepare( g_pd3dDevice );

		if( pMeshlist->IsAllNullsSelected() )
			OnMenuMeshlistNullsRemoveAll();

		pMeshlist->DeselectAllNulls();

		if( m_appSettings.ScnAlternateNullIco )
			pMeshlist->SetNullIcon( pNullIco );

		hTemp = m_rtSceneContents.GetChildItem( hMeshes );
		
		if( hTemp )
			m_rtSceneContents.SelectItem( hTemp );
	}
}

void CCookieMachineDlg::SelectedObjectsMerge()
{
	if( !pBaseScene || m_bIsVertexEditingModeOn )
		return;

	if( pBaseScene->GetType() == FT_EFN_MESHLIST || pBaseScene->GetType() == FT_SCN )
	{
		vector<str_t>	sel;
		str_t			str_par;
		HTREEITEM		hMeshes = m_rtSceneContents.FindTreeItemByName( "Meshes" );
		HTREEITEM		hTemp	= NULL;
		UINT			uiMask	= TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		CEfnMeshlist*	pMeshlist = NULL;

		if( pBaseScene->GetType() == FT_EFN_MESHLIST )
		{
			pMeshlist = ((CEfnMeshlist*)pBaseScene);
		}
		else if( pBaseScene->GetType() == FT_SCN )
		{
			pMeshlist = (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();
		}
		else
			return;

		if( !pMeshlist )
			return;

		SetCursor( LoadCursor( NULL, IDC_WAIT ) );

		pMeshlist->MakeSureParentIsSet();

		for( int i = 0; i < pMeshlist->GetMeshesCount(); i++ )
		{
			CMesh* pMesh = pMeshlist->GetMeshByIndex( i );

			if( pMesh->IsSelected() )
			{
				if( pMesh->IsParentMesh() == false )
					sel.push_back( pMesh->GetName() );
				else
					str_par = pMesh->GetName();	
			}
			else
			{}
		}

		str_t strAlternativePar = "";

		pMeshlist->MergeSelectedMeshes( &strAlternativePar );
		pMeshlist->Prepare( g_pd3dDevice );

		for( UINT i = 0; i < sel.size(); i++ )
		{
			hTemp = m_rtSceneContents.FindTreeItemByName( sel[i], hMeshes );

			if( hTemp )
				m_rtSceneContents.DeleteItem( hTemp );
		}

		hTemp = m_rtSceneContents.FindTreeItemByName( str_par, hMeshes );
		m_rtSceneContents.SelectItem( hTemp );
		pMeshlist->SelectMesh( str_par );
	}

	UpdateMeshesInfoInExplorer();

	SetCursor( LoadCursor( NULL, IDC_ARROW ) );
}

void CCookieMachineDlg::OnNMClickSceneTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	SceneTreeItemChangeProc();
	*pResult = 0;
}

void CCookieMachineDlg::SelectedObjectsMaterial()
{
	if( !pBaseScene || m_bIsVertexEditingModeOn )
		return;

	CEfnMeshlist* pMeshlist = NULL;

	if( pBaseScene->GetType() == FT_SCN || pBaseScene->GetType() == FT_EFN_MESHLIST )
	{
		if( pBaseScene->GetType() == FT_SCN )
			pMeshlist = (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();
		else if( pBaseScene->GetType() == FT_EFN_MESHLIST )
			pMeshlist = ((CEfnMeshlist*)pBaseScene);
		else
		{}

		if( !pMeshlist || pMeshlist->GetMeshesCount() == 0 )
			return;

		CMesh* mesh = NULL;

		//dlg->m_Material = pMeshlist->Get FirstSelectedMesh()->GetMaterial();
		if( pMeshlist->GetParentMesh() != NULL )
			mesh = pMeshlist->GetParentMesh();
		else
		{
			if( pMeshlist->GetFirstSelectedMesh() != NULL )
				mesh = pMeshlist->GetFirstSelectedMesh();
		}

		if( mesh )
		{
			CMaterialDlg* dlg = new CMaterialDlg;

			dlg->m_Material = mesh->GetMaterial();

			if( dlg->DoModal() != IDCANCEL )
				pMeshlist->AssignMaterialToSelectedMeshes( dlg->m_Material );

			delete dlg;
		}
	}
}

void CCookieMachineDlg::SelectedObjectsConnectTris()
{
	if( !pBaseScene )
		return;

	CEfnMeshlist* pMeshlist = NULL;

	SetCursor( LoadCursor( NULL, IDC_WAIT ) );

	if( pBaseScene->GetType() == FT_EFN_MESHLIST ) 
	{
		pMeshlist = (CEfnMeshlist*)pBaseScene;
	}
	else if( pBaseScene->GetType() == FT_SCN )
	{
		pMeshlist = (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();
	}

	if( !pMeshlist || pMeshlist->GetMeshesCount() == 0 )
			return;

	for( int i = 0; i < pMeshlist->GetMeshesCount(); i++ )
	{
		if( pMeshlist->GetMeshByIndex( i )->IsSelected() )
			pMeshlist->GetMeshByIndex( i )->ConnectPolys();
	}

	
	
	if( pBaseScene->GetType() == FT_EFN_MESHLIST ) 
	{
		UpdateStatusBarInfoMESHLIST();
	}
	else if( pBaseScene->GetType() == FT_SCN )
	{
		UpdateStatusBarInfoSCN();
	}
	

	SetCursor( LoadCursor( NULL, IDC_ARROW ) );
}

void CCookieMachineDlg::SelectedObjectsDisconnectTris()
{
	if( !pBaseScene )
		return;

	CEfnMeshlist* pMeshlist = NULL;

	SetCursor( LoadCursor( NULL, IDC_WAIT ) );

	if( pBaseScene->GetType() == FT_EFN_MESHLIST ) 
	{
		pMeshlist = (CEfnMeshlist*)pBaseScene;
	}
	else if( pBaseScene->GetType() == FT_SCN )
	{
		pMeshlist = (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();
	}

	if( !pMeshlist || pMeshlist->GetMeshesCount() == 0 )
			return;

	for( int i = 0; i < pMeshlist->GetMeshesCount(); i++ )
	{
		if( pMeshlist->GetMeshByIndex( i )->IsSelected() )
			pMeshlist->GetMeshByIndex( i )->DisconnectPolys();
	}

	if( pBaseScene->GetType() == FT_EFN_MESHLIST ) 
	{
		UpdateStatusBarInfoMESHLIST();
	}
	else if( pBaseScene->GetType() == FT_SCN )
	{
		UpdateStatusBarInfoSCN();
	}

	SetCursor( LoadCursor( NULL, IDC_ARROW ) );
}

void CCookieMachineDlg::SelectedObjectsFlipNormals()
{
	if( !pBaseScene || m_bIsVertexEditingModeOn )
		return;

	CEfnMeshlist* pMeshlist = NULL;

	SetCursor( LoadCursor( NULL, IDC_WAIT ) );

	if( pBaseScene->GetType() == FT_SCN || pBaseScene->GetType() == FT_EFN_MESHLIST )
	{
		if( pBaseScene->GetType() == FT_SCN )
			pMeshlist = (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();
		else if( pBaseScene->GetType() == FT_EFN_MESHLIST )
			pMeshlist = ((CEfnMeshlist*)pBaseScene);
		else
		{}

		if( !pMeshlist || pMeshlist->GetMeshesCount() == 0 )
			return;

		pMeshlist->FlipNormalsInSelectedMeshes();
	}

	SetCursor( LoadCursor( NULL, IDC_ARROW ) );
}

void CCookieMachineDlg::OnMenuScnCameraMiscObjectScript()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_SCN )
		return;

	HTREEITEM hSelected, hParent, hParentParent, hParentParentParent;
	CString strParentText, strParentParentText, hSelItemText, strParentParentParentText;

	hSelected		= m_rtSceneContents.GetSelectedItem();
	hSelItemText	= m_rtSceneContents.GetItemText( hSelected );

	if( hSelected >= 0 )
		hParent = m_rtSceneContents.GetParentItem( hSelected );

	if( hParent >= 0 )
	{
		hParentParent	= m_rtSceneContents.GetParentItem( hParent );
		strParentText	= m_rtSceneContents.GetItemText( hParent );
	}

	if( hParentParent >= 0 )
	{
		strParentParentText			= m_rtSceneContents.GetItemText( hParentParent );
		hParentParentParent			= m_rtSceneContents.GetParentItem( hParentParent );
		strParentParentParentText	= m_rtSceneContents.GetItemText( hParentParentParent );
	}

	CSceneFile*		scene	= ((CSceneFile*)pBaseScene);
	ListStrVec*		binds	= scene->GetBinds();
	ListStrVec_iter	iter	= binds->begin();


	//find whether object is binded to scene
	bool ok = false;

	while( iter != binds->end() )
	{
		if( (*iter)[2] == "Scene" && ((*iter)[4] + "." +(*iter)[3]).data() == hSelItemText )
		{
			ok = true;
			break;
		}

		iter++;
	}
	
	//prepare last edited item and assign it to scene
	if( ok )
	{
		scene->m_strLastEditedObj = hSelItemText.GetBuffer();
		ShowScriptsWindow();
	}
}

void CCookieMachineDlg::OnMenuScnCameraMiscObjectBind()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_SCN )
		return;

	if( m_dlgBindingDlg )
		return;


	CSceneFile*		scene			= ((CSceneFile*)pBaseScene);
	CEfnMeshlist*	meshlist		= (CEfnMeshlist*)scene->GetMeshlist();
	StrVec*			m_CustomObjects	= scene->GetCustObjList();
	StrVec*			m_CustomTypes	= scene->GetCustTypesList();
	StrVec			vecSceneData;
	StrVec			bind_item;
	StrVec			scnTypes;
	CString			temp;


	//collect data from the scene
	for( int i = 0 ; i < scene->GetLightsCount(); i++ )
	{
		temp.Format( "Light.%s", scene->GetLightByIndex( i )->name.data() );
		vecSceneData.push_back( temp.GetBuffer() );
	}

	for( int i = 0 ; i < scene->GetCamerasCount(); i++ )
	{
		temp.Format( "Camera.%s", scene->GetCameraByIndex( i )->name.data() );
		vecSceneData.push_back( temp.GetBuffer() );
	}

	if( meshlist )
	{
		for( int i = 0 ; i < meshlist->GetMeshesCount(); i++ )
		{
			temp.Format( "Mesh.%s", meshlist->GetMeshByIndex( i )->GetName().data() );
			vecSceneData.push_back( temp.GetBuffer() );
		}
	}

	StrVec tex_vec = *(scene->GetTextures());
	for( unsigned int i = 0; i < tex_vec.size(); i++ )
	{
		temp.Format( "Texture.%s", tex_vec[i].data() );
		vecSceneData.push_back( temp.GetBuffer() );
	}

	temp.Format( "Scene.%s", scene->GetSceneID().data() );
	vecSceneData.push_back( temp.GetBuffer() );


	//build types list
	GetTypesList( scnTypes );

	if( m_appSettings.ExpertMode )
	{
		for( unsigned int i = 0; i < m_CustomObjects->size(); i++ )
		{
			temp.Format( "Object.%s", (*m_CustomObjects)[i].data() );
			vecSceneData.push_back( temp.GetBuffer() );
		}

		for( unsigned int i = 0; i < m_CustomTypes->size(); i++ )
		{
			scnTypes.push_back( (*m_CustomTypes)[i] );
		}
	}


	HTREEITEM hSelected, hParent, hParentParent, hParentParentParent;
	CString strParentText, strParentParentText, hSelItemText, strParentParentParentText;

	hSelected		= m_rtSceneContents.GetSelectedItem();
	hSelItemText	= m_rtSceneContents.GetItemText( hSelected );

	if( hSelected >= 0 )
		hParent = m_rtSceneContents.GetParentItem( hSelected );

	if( hParent >= 0 )
	{
		hParentParent	= m_rtSceneContents.GetParentItem( hParent );
		strParentText	= m_rtSceneContents.GetItemText( hParent );
	}

	if( hParentParent >= 0 )
	{
		strParentParentText			= m_rtSceneContents.GetItemText( hParentParent );
		hParentParentParent			= m_rtSceneContents.GetParentItem( hParentParent );
		strParentParentParentText	= m_rtSceneContents.GetItemText( hParentParentParent );
	}

	bind_item.push_back( strParentParentText.GetBuffer() );
	bind_item.push_back( "Camera" );

	int separator_pos = hSelItemText.Find( '.', 0 );

	if( separator_pos == -1 )
		return;

	str_t obj	= hSelItemText.Right( hSelItemText.GetLength() - separator_pos - 1 ).GetBuffer();
	str_t type	= hSelItemText.Left( separator_pos ).GetBuffer();

	bind_item.push_back( obj );
	bind_item.push_back( type );

	m_dlgBindingDlg = new CBindsDlg();
	m_dlgBindingDlg->LinkBinds( scene->GetBinds() );
	m_dlgBindingDlg->LinkSceneData( &vecSceneData );
	m_dlgBindingDlg->LinkSceneTypes( &scnTypes );
	m_dlgBindingDlg->SetParent( this );
	m_dlgBindingDlg->SetAccel( m_hAccelerator );
	m_dlgBindingDlg->SetSelection( &bind_item );
	m_dlgBindingDlg->DoModal();
		

	if( m_dlgBindingDlg->ChangesMade() )
		UpdateUI_SCNBinds();


	SAFE_DELETE( m_dlgBindingDlg );

	//hParentParent	= GetTreeItemByName( "Cameras", m_rtSceneContents.GetRootItem() );
	//hParent			= m_rtSceneContents.FindTreeItemByName( strParentParentText, hParentParent );
	//hSelected		= m_rtSceneContents.FindTreeItemByName( strItem, hParent );

	//m_rtSceneContents.Expand( hParentParent, TVE_TOGGLE );
	//m_rtSceneContents.Expand( hParent, TVE_TOGGLE );
	//m_rtSceneContents.SelectItem( hSelected );
}

void CCookieMachineDlg::OnMenuScnCurvesCreateNew()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_SCN )
		return;

	CModelNameDlg*	dlg		= new CModelNameDlg( this );
	UINT			uiMask	= TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

	dlg->SetCaption( "Curve ID:" );
	dlg->DoModal();

	CSceneFile* scene = (CSceneFile*)pBaseScene;
	CString temp = dlg->GetString();
	temp.Trim();

	SAFE_DELETE( dlg );

	if( scene->GetCurveByName( temp.GetBuffer() ) )
	{
		MessageBox( "Кривая с таким ID уже существует!", "Внимание!", MB_OK | MB_ICONINFORMATION );
		return;
	}

	if( temp.GetLength() == 0 )
		return;

	scene->AddNewCurve( temp.GetBuffer() );

	HTREEITEM hCurves	= m_rtSceneContents.FindTreeItemByName( "Curves" );
	HTREEITEM hCurve	= m_rtSceneContents.InsertItem( uiMask, temp, 26, 26, 0, 0, 0, hCurves, NULL );	

	m_rtSceneContents.SetFocus();
	m_rtSceneContents.Expand( hCurves, TVE_EXPAND );
	m_rtSceneContents.SelectItem( hCurve );
}

void CCookieMachineDlg::OnMenuScnCurveAddselectednull()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_SCN )
		return;

	CSceneFile*	scene	= ((CSceneFile*)pBaseScene);
	CNULL*		pNull	= ((CEfnMeshlist*)scene->GetMeshlist())->GetSelectedNull();
	HTREEITEM	hCurve	= m_rtSceneContents.GetSelectedItem();
	CurveItem*	crv		= scene->GetCurveByName( m_rtSceneContents.GetItemText(hCurve).GetBuffer() );

	if( !pNull || !crv )
		return;

	crv->nulls.push_back( pNull->id );
	
	m_rtSceneContents.InsertItem2( pNull->id.data(), 10, NULL, hCurve, NULL );
	m_rtSceneContents.Invalidate( false );
	m_rtSceneContents.SetFocus();

	scene->PrepareCurves();
}

void CCookieMachineDlg::OnMenuScnCurvesShow()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	((CSceneFile*)pBaseScene)->ShowAllCurves( true );
	((CSceneFile*)pBaseScene)->PrepareCamsLines( g_pd3dDevice );

	HTREEITEM	hCurves	= m_rtSceneContents.FindTreeItemByName( "Curves" );
	HTREEITEM	hCurve	= m_rtSceneContents.GetChildItem( hCurves );

	m_rtSceneContents.SetItemImage( hCurve, 26, 26 );

	while( hCurve )
	{
		hCurve = m_rtSceneContents.GetNextSiblingItem( hCurve );
		m_rtSceneContents.SetItemImage( hCurve, 26, 26 );
	}
}

void CCookieMachineDlg::OnMenuScnCurvesHide()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	((CSceneFile*)pBaseScene)->ShowAllCurves( false );
	((CSceneFile*)pBaseScene)->PrepareCamsLines( g_pd3dDevice );

	HTREEITEM	hCurves	= m_rtSceneContents.FindTreeItemByName( "Curves" );
	HTREEITEM	hCurve	= m_rtSceneContents.GetChildItem( hCurves );

	m_rtSceneContents.SetItemImage( hCurve, 29, 29 );

	while( hCurve )
	{
		hCurve = m_rtSceneContents.GetNextSiblingItem( hCurve );
		m_rtSceneContents.SetItemImage( hCurve, 29, 29 );
	}
}

void CCookieMachineDlg::OnMenuScnCurvesDeleteAll()
{
    if( !pBaseScene || pBaseScene->GetType() != FT_SCN )
        return;


	if( MessageBox( "Удалить все кривые?", "Внимание!", MB_YESNO | MB_ICONQUESTION ) != IDYES )
		return;

	// remove curve nodes from treeview
	HTREEITEM hCurves	= m_rtSceneContents.FindTreeItemByName( "Curves" );
	HTREEITEM hCurve	= m_rtSceneContents.GetChildItem( hCurves );

	while( hCurve )
	{
		m_rtSceneContents.DeleteItem( hCurve );
		hCurve = m_rtSceneContents.GetChildItem( hCurves );
	}

	((CSceneFile*)pBaseScene)->DeleteAllCurves();
	pBaseScene->Prepare( g_pd3dDevice );
}

void CCookieMachineDlg::OnMenuScnCurveRemoveAllNulls()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_SCN )
		return;

	CSceneFile*	scene	= ((CSceneFile*)pBaseScene);
	HTREEITEM	hCurve	= m_rtSceneContents.GetSelectedItem();
	CurveItem*	crv		= scene->GetCurveByName( m_rtSceneContents.GetItemText(hCurve).GetBuffer() );

	if( !crv )
		return;

	crv->nulls.clear();
	

	// remove null nodes from treeview
	HTREEITEM hNull = m_rtSceneContents.GetChildItem( hCurve );

	while( hNull )
	{
		m_rtSceneContents.DeleteItem( hNull );
		hNull = m_rtSceneContents.GetChildItem( hCurve );
	}


	m_rtSceneContents.Invalidate( false );
	m_rtSceneContents.SetFocus();

	scene->PrepareCurves();
}

void CCookieMachineDlg::OnMenuScnCurveDelete()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_SCN )
		return;

	CSceneFile*	scene	= ((CSceneFile*)pBaseScene);
	HTREEITEM	hCurve	= m_rtSceneContents.GetSelectedItem();

	scene->DeleteCurve( m_rtSceneContents.GetItemText( hCurve ).GetBuffer() );

	m_rtSceneContents.DeleteItem( hCurve );

	m_rtSceneContents.Invalidate( false );
	m_rtSceneContents.SetFocus();

	scene->PrepareCurves();
}

void CCookieMachineDlg::OnMenuScnCurveRename( CString strCurrID, CString strNewID )
{
	if( strCurrID == strNewID )
		return;

	CSceneFile*	scene	= ((CSceneFile*)pBaseScene);
	CurveItem*	crv		= scene->GetCurveByName( strCurrID.GetBuffer() );

	if( !crv )
		return;

	if( scene->GetCurveByName( strNewID.GetBuffer() ) )
	{
		AfxMessageBox( "Кривая с таким именем уже существует!" );
		return;
	}
	else
		crv->id = strNewID.GetBuffer();

	HTREEITEM hCurveItem = m_rtSceneContents.GetSelectedItem();

	m_rtSceneContents.SetItemText( hCurveItem, strNewID );
	m_rtSceneContents.SortChildren( m_rtSceneContents.GetParentItem( hCurveItem ) );
	m_rtSceneContents.SelectItem( hCurveItem );

	m_rtSceneContents.Invalidate( false );
	m_rtSceneContents.SetFocus();
}

void CCookieMachineDlg::OnMenuScnCurveSetTypeLinear()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_SCN )
		return;

	CSceneFile*	scene	= ((CSceneFile*)pBaseScene);
	HTREEITEM	hCurve	= m_rtSceneContents.GetSelectedItem();
	CurveItem*	crv		= scene->GetCurveByName( m_rtSceneContents.GetItemText(hCurve).GetBuffer() );

	crv->type = LINEAR;
	scene->PrepareCurves();
}

void CCookieMachineDlg::OnMenuScnCurveSetTypeCatmul()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_SCN )
		return;

	CSceneFile*	scene	= ((CSceneFile*)pBaseScene);
	HTREEITEM	hCurve	= m_rtSceneContents.GetSelectedItem();
	CurveItem*	crv		= scene->GetCurveByName( m_rtSceneContents.GetItemText(hCurve).GetBuffer() );

	crv->type = CATMULROM;
	scene->PrepareCurves();
}

void CCookieMachineDlg::OnMenuScnCurveNullRemove()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_SCN )
		return;

	HTREEITEM	hItem	= m_rtSceneContents.GetSelectedItem();
	HTREEITEM	hParent	= m_rtSceneContents.GetParentItem( hItem );

	HTREEITEM	hTempItem = m_rtSceneContents.GetChildItem( hParent );

	bool found = false;
	int	place = 0;

	while( hTempItem )
	{
		if( m_rtSceneContents.GetItemState( hTempItem, TVIS_SELECTED ) )
		{
			found = true;
			break;
		}

		place++;

		hTempItem = m_rtSceneContents.GetNextSiblingItem( hTempItem );
	}

	if( !found )
		return;

	CurveItem* crv = ((CSceneFile*)pBaseScene)->GetCurveByName( m_rtSceneContents.GetItemText( hParent ).GetBuffer() );

	crv->nulls.erase( crv->nulls.begin() + place );

	m_rtSceneContents.DeleteItem( hItem );

	((CSceneFile*)pBaseScene)->PrepareCurves();
}

void CCookieMachineDlg::OnMenuScnCurveNullMoveUP()
{
	HTREEITEM	hItem	= m_rtSceneContents.GetSelectedItem();
	HTREEITEM	hParent	= m_rtSceneContents.GetParentItem( hItem );
	CString		strItem = m_rtSceneContents.GetItemText( hItem );
	DWORD		uiMask	= TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	HTREEITEM	hNext	= m_rtSceneContents.GetNextSiblingItem( hItem );
	CSceneFile*	scene	= ((CSceneFile*)pBaseScene);


	HTREEITEM hSelected, hParentParent, hParentParentParent;
	CString strParentText, strParentParentText, strParentParentParentText;
	CString hSelItemText;

	hSelected		= m_rtSceneContents.GetSelectedItem();
	hSelItemText	= m_rtSceneContents.GetItemText( hSelected );
	
	if( hSelected >= 0 )
		hParent = m_rtSceneContents.GetParentItem( hSelected );

	if( hParent >= 0 )
	{
		strParentText	= m_rtSceneContents.GetItemText( hParent );
		hParentParent	= m_rtSceneContents.GetParentItem( hParent );
	}

	if( hParentParent >= 0 )
	{
		strParentParentText	= m_rtSceneContents.GetItemText( hParentParent );
		hParentParentParent	= m_rtSceneContents.GetParentItem( hParentParent );
	}

	if( strParentParentText != "Curves" || hParentParentParent != NULL )
		return;

	//move null in curve's null in data
	bool	found	= false;
	int		place	= 0;

	HTREEITEM hTempItem = m_rtSceneContents.GetChildItem( hParent );

	while( hTempItem )
	{
		if( m_rtSceneContents.GetItemState( hTempItem, TVIS_SELECTED ) )
		{
			found = true;
			break;
		}

		place++;

		hTempItem = m_rtSceneContents.GetNextSiblingItem( hTempItem );
	}

	if( !found )
		return;

	CurveItem* crv = scene->GetCurveByName( strParentText.GetBuffer() );
	
	str_t strTemp = crv->nulls[place];

	if( place - 1 < 0 )
		return;

	crv->nulls.erase( crv->nulls.begin() + place );
	crv->nulls.insert( crv->nulls.begin() + (place - 1), strTemp );

	scene->PrepareCurves();


	//move curve's null in treeview
	HTREEITEM hPrev = m_rtSceneContents.GetPrevSiblingItem( hItem );
	hPrev = m_rtSceneContents.GetPrevSiblingItem( hPrev );

	if( hPrev )
	{
		HTREEITEM hMoved = m_rtSceneContents.InsertItem( uiMask, strItem, 10, 10, 0, 0, 0, hParent, hPrev );
		m_rtSceneContents.SelectItem( hMoved );
		m_rtSceneContents.DeleteItem( hItem );
	}
	else
	{
		HTREEITEM hMoved = m_rtSceneContents.InsertItem( uiMask, strItem, 10, 10, 0, 0, 0, hParent, TVI_FIRST );
		m_rtSceneContents.SelectItem( hMoved );
		m_rtSceneContents.DeleteItem( hItem );
	}
}

void CCookieMachineDlg::OnMenuScnCurveNullMoveDOWN()
{
	HTREEITEM	hItem	= m_rtSceneContents.GetSelectedItem();
	HTREEITEM	hParent	= m_rtSceneContents.GetParentItem( hItem );
	CString		strItem = m_rtSceneContents.GetItemText( hItem );
	DWORD		uiMask	= TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	HTREEITEM	hNext	= m_rtSceneContents.GetNextSiblingItem( hItem );
	CSceneFile*	scene	= ((CSceneFile*)pBaseScene);


	
	HTREEITEM hSelected, hParentParent, hParentParentParent;
	CString strParentText, strParentParentText, strParentParentParentText;
	CString hSelItemText;

	hSelected		= m_rtSceneContents.GetSelectedItem();
	hSelItemText	= m_rtSceneContents.GetItemText( hSelected );
	
	if( hSelected >= 0 )
		hParent = m_rtSceneContents.GetParentItem( hSelected );

	if( hParent >= 0 )
	{
		strParentText	= m_rtSceneContents.GetItemText( hParent );
		hParentParent	= m_rtSceneContents.GetParentItem( hParent );
	}

	if( hParentParent >= 0 )
	{
		strParentParentText	= m_rtSceneContents.GetItemText( hParentParent );
		hParentParentParent	= m_rtSceneContents.GetParentItem( hParentParent );
	}

	if( strParentParentText != "Curves" || hParentParentParent != NULL )
		return;

	
	//move null in curve's null in data
	bool	found	= false;
	int		place	= 0;

	HTREEITEM hTempItem = m_rtSceneContents.GetChildItem( hParent );

	while( hTempItem )
	{
		if( m_rtSceneContents.GetItemState( hTempItem, TVIS_SELECTED ) )
		{
			found = true;
			break;
		}

		place++;

		hTempItem = m_rtSceneContents.GetNextSiblingItem( hTempItem );
	}

	if( !found )
		return;

	CurveItem* crv = scene->GetCurveByName( strParentText.GetBuffer() );
	
	str_t strTemp = crv->nulls[place];

	if( place + 1 >= (int)crv->nulls.size() )
		return;

	crv->nulls.erase( crv->nulls.begin() + place );
	crv->nulls.insert( crv->nulls.begin() + place + 1, strTemp );

	scene->PrepareCurves();


	//move curve's null in treeview
	if( hNext )
	{
		HTREEITEM hMoved = m_rtSceneContents.InsertItem( uiMask, strItem, 10, 10, 0, 0, 0, hParent, hNext );
		m_rtSceneContents.SelectItem( hMoved );
		m_rtSceneContents.DeleteItem( hItem );
	}
	else
	{
		HTREEITEM hMoved = m_rtSceneContents.InsertItem( uiMask, strItem, 10, 10, 0, 0, 0, hParent, TVI_LAST );
		m_rtSceneContents.SelectItem( hMoved );
		m_rtSceneContents.DeleteItem( hItem );
	}
}

void CCookieMachineDlg::OnMenuEngineMessaging()
{
	CEngineMessagingDlg dlg;
	dlg.DoModal();
}

void CCookieMachineDlg::SelectedObjectsInverseSelection()
{
	if( !pBaseScene )
		return;

	SetCursor( LoadCursor( NULL, IDC_WAIT ) );

	CEfnMeshlist* pMeshlist = NULL;

	if( pBaseScene->GetType() == FT_EFN_MESHLIST ) 
	{
		pMeshlist = (CEfnMeshlist*)pBaseScene;
	}
	else if( pBaseScene->GetType() == FT_SCN ) 
	{
		pMeshlist = (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();
	}

	if( !pMeshlist || pMeshlist->GetMeshesCount() == 0 )
			return;

	vector<str_t> sel;

	for( int i = 0; i < pMeshlist->GetMeshesCount(); i++ )
	{
		if( pMeshlist->GetMeshByIndex( i )->IsSelected() )
			sel.push_back( pMeshlist->GetMeshByIndex( i )->GetName() );
	}

	pMeshlist->SelectAllMeshes();

	for( UINT i = 0; i < sel.size(); i++ )
		pMeshlist->GetMeshByName( sel[i] )->SetSelected( false );


	sel.clear();

	for( int i = 0; i < pMeshlist->GetMeshesCount(); i++ )
	{
		CMesh* pMesh = pMeshlist->GetMeshByIndex( i );

		if( pMesh->IsSelected() )
			sel.push_back( pMesh->GetName() );
	}

	HTREEITEM	hMeshes	= m_rtSceneContents.FindTreeItemByName( "Meshes" );
	bool		first	= false;

	for( UINT i = 0; i < sel.size(); i++ )
	{
		HTREEITEM hMesh = m_rtSceneContents.FindTreeItemByName( sel[i], hMeshes );

		if( !first )
		{
			m_rtSceneContents.SelectItem( hMesh );
			first = true;
		}
		else
		{
			m_rtSceneContents.SetItemState( hMesh, TVIS_SELECTED, TVIS_SELECTED );	
		}
	}

	for( UINT i = 0; i < sel.size(); i++ )
	{
		pMeshlist->GetMeshByName( sel[i] )->SetSelected( true );

		if( i == 0 )
			pMeshlist->GetMeshByName( sel[i] )->SetParentMesh( true );
	}

	SetCursor( LoadCursor( NULL, IDC_ARROW ) );
}

void CCookieMachineDlg::SelectedObjectsHide()
{
	SetCursor( LoadCursor( NULL, IDC_WAIT ) );

	if( !pBaseScene )
		return;

	CEfnMeshlist* pMeshlist = NULL;

	if( pBaseScene->GetType() == FT_EFN_MESHLIST ) 
	{
		pMeshlist = (CEfnMeshlist*)pBaseScene;
	}
	else if( pBaseScene->GetType() == FT_SCN ) 
	{
		pMeshlist = (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();
	}

	if( !pMeshlist || pMeshlist->GetMeshesCount() == 0 )
			return;

	HTREEITEM hMeshes = m_rtSceneContents.FindTreeItemByName( "Meshes" );
	

	for( int i = 0; i < pMeshlist->GetMeshesCount(); i++ )
	{
		if( pMeshlist->GetMeshByIndex( i )->IsSelected() )
		{
			HTREEITEM hMesh	= m_rtSceneContents.FindTreeItemByName( pMeshlist->GetMeshByIndex( i )->GetName(), hMeshes );

			if( pMeshlist->GetMeshByIndex( i )->GetVisible() )
			{
				pMeshlist->GetMeshByIndex( i )->SetVisible( false );
				m_rtSceneContents.SetItemImage( hMesh, 4, 4 );
			}
			else
			{
				pMeshlist->GetMeshByIndex( i )->SetVisible( true );
				m_rtSceneContents.SetItemImage( hMesh, 0, 0 );
			}

			//pMeshlist->GetMeshByIndex( i )->SetSelected( false );
			//m_rtSceneContents.SetItemState( hMesh, 0, TVIS_SELECTED );
		}
	}

	//m_rtSceneContents.SelectItem( hMesh );

	SetCursor( LoadCursor( NULL, IDC_ARROW ) );
}

void CCookieMachineDlg::OnMenuSelectVisibleObjects()
{
	if( !pBaseScene )
		return;

	SetCursor( LoadCursor( NULL, IDC_WAIT ) );

	CEfnMeshlist* pMeshlist = NULL;

	if( pBaseScene->GetType() == FT_EFN_MESHLIST ) 
	{
		pMeshlist = (CEfnMeshlist*)pBaseScene;
	}
	else if( pBaseScene->GetType() == FT_SCN ) 
	{
		pMeshlist = (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();
	}

	if( !pMeshlist || pMeshlist->GetMeshesCount() == 0 )
			return;

	pMeshlist->DeselectAllMeshes();


	vector<str_t> sel;

	for( int i = 0; i < pMeshlist->GetMeshesCount(); i++ )
	{
		if( pMeshlist->GetMeshByIndex( i )->GetVisible() && pMeshlist->GetMeshByIndex( i )->GetName() != "Nulls" )
			sel.push_back( pMeshlist->GetMeshByIndex( i )->GetName() );
	}


	HTREEITEM	hMeshes	= m_rtSceneContents.FindTreeItemByName( "Meshes" );
	bool		first	= false;

	for( UINT i = 0; i < sel.size(); i++ )
	{
		HTREEITEM hMesh = m_rtSceneContents.FindTreeItemByName( sel[i], hMeshes );

		if( !first )
		{
			m_rtSceneContents.SelectItem( hMesh );
			first = true;
		}
		else
		{
			m_rtSceneContents.SetItemState( hMesh, TVIS_SELECTED, TVIS_SELECTED );	
		}
	}

	for( UINT i = 0; i < sel.size(); i++ )
	{
		pMeshlist->GetMeshByName( sel[i] )->SetSelected( true );

		if( i == 0 )
			pMeshlist->SetParentMesh( sel[i] );
	}

	SetCursor( LoadCursor( NULL, IDC_ARROW ) );
}

void CCookieMachineDlg::OnMenuRename()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() == FT_EFN_MESHLIST )
	{
		if( ((CEfnMeshlist*)pBaseScene)->GetSelectedMeshesCount() == 1 )
			ObjectRename();
	}
	else if( pBaseScene->GetType() == FT_SCN )
	{
		if( ((CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist())->GetSelectedMeshesCount() == 1 )
			ObjectRename();
	}
}

void CCookieMachineDlg::OnMenuDuplicate()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() == FT_EFN_MESHLIST )
	{
		if( ((CEfnMeshlist*)pBaseScene)->GetSelectedMeshesCount() == 1 )
			OnMenuMeshlistMeshDuplicate();
	}
	else if( pBaseScene->GetType() == FT_SCN )
	{
		if( ((CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist())->GetSelectedMeshesCount() == 1 )
			OnMenuScnMeshDuplicate();
	}
}

void CCookieMachineDlg::OnMenuEfnNullsReloadAttaches()
{
	
}

void CCookieMachineDlg::OnStnClickedBmp()
{
	CStatic*	render_wnd		= (CStatic*)GetDlgItem( IDC_RENDERWND );
	CStatic*	view_menu_icon	= (CStatic*)GetDlgItem( IDC_BMP );
	CRect		rc, rc2;

	render_wnd->GetWindowRect( rc );
	view_menu_icon->GetWindowRect( rc2 );

	m_menuViewPopup.GetSubMenu( 0 )->TrackPopupMenu( TPM_LEFTALIGN, rc2.left, rc.top, this );
}

void CCookieMachineDlg::OnMenuScnLightsDeleteAll()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_SCN )
		return;

	if( MessageBox( "Удалить все источники света?", "Внимание!", MB_YESNO | MB_ICONQUESTION ) != IDYES )
		return;

	HTREEITEM hLights	= m_rtSceneContents.FindTreeItemByName( "Lights" );
	HTREEITEM hLight	= m_rtSceneContents.GetChildItem( hLights );

	while( hLight )
	{
		m_rtSceneContents.DeleteItem( hLight );
		hLight = m_rtSceneContents.GetChildItem( hLights );
	}

	((CSceneFile*)pBaseScene)->DeleteAllLights();
	pBaseScene->Prepare( g_pd3dDevice );
}

void CCookieMachineDlg::OnMenuScnCamerasDeleteAll()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_SCN )
		return;

	if( MessageBox( "Удалить все камеры?", "Внимание!", MB_YESNO | MB_ICONQUESTION ) != IDYES )
		return;

	HTREEITEM hCameras	= m_rtSceneContents.FindTreeItemByName( "Cameras" );
	HTREEITEM hCamera	= m_rtSceneContents.GetChildItem( hCameras );

	while( hCamera )
	{
		m_rtSceneContents.DeleteItem( hCamera );
		hCamera = m_rtSceneContents.GetChildItem( hCameras );
	}

	((CSceneFile*)pBaseScene)->DeleteAllCameras();
	pBaseScene->Prepare( g_pd3dDevice );
}

void CCookieMachineDlg::OnMenuScnMeshesDeleteAll()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_SCN )
		return;

	if( MessageBox( "Удалить всю геометрию?", "Внимание!", MB_YESNO | MB_ICONQUESTION ) != IDYES )
		return;

	HTREEITEM hMeshes	= m_rtSceneContents.FindTreeItemByName( "Meshes" );
	HTREEITEM hMesh		= m_rtSceneContents.GetChildItem( hMeshes );

	while( hMesh )
	{
		m_rtSceneContents.DeleteItem( hMesh );
		hMesh = m_rtSceneContents.GetChildItem( hMeshes );
	}

	CEfnMeshlist* pMeshlist = (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();
	pMeshlist->DeleteAllMeshes();

	pBaseScene->Prepare( g_pd3dDevice );
}

void CCookieMachineDlg::OnMenuScnNullsDeleteAll()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_SCN )
		return;

	if( MessageBox( "Удалить все нули?", "Внимание!", MB_YESNO | MB_ICONQUESTION ) != IDYES )
		return;

	HTREEITEM hNulls	= m_rtSceneContents.FindTreeItemByName( "Nulls" );
	HTREEITEM hNull		= m_rtSceneContents.GetChildItem( hNulls );

	while( hNull )
	{
		m_rtSceneContents.DeleteItem( hNull );
		hNull = m_rtSceneContents.GetChildItem( hNulls );
	}

	CEfnMeshlist* pMeshlist = (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();
	pMeshlist->DeleteAllNulls();

	pBaseScene->Prepare( g_pd3dDevice );
}

void CCookieMachineDlg::OnMenuScnTextureRename( CString strCurrID, CString strNewID )
{
	StrVec*			tex_list	= ((CSceneFile*)pBaseScene)->GetTextures();
	bool			found		= false;
	CString			strTemp;

	if( strNewID == strCurrID )
		return;

	for( StrVec_iter iter = tex_list->begin(); iter != tex_list->end(); iter++ )
	{
		strTemp = (*iter).data();

		if( strTemp == strNewID )
		{
			found = true;
			break;
		}
	}

	if( found )
	{
		MessageBox( "Текстура с таким ID уже существует!", "Внимание!", MB_OK | MB_ICONINFORMATION );
		return;
	}


	for( StrVec_iter iter = tex_list->begin(); iter != tex_list->end(); iter++ )
	{
		strTemp = (*iter).data();

		if( strTemp == strCurrID )
		{
			(*iter) = strNewID.GetBuffer();
			break;
		}
	}

	HTREEITEM hTextureItem = m_rtSceneContents.GetSelectedItem();

	m_rtSceneContents.SetItemText( hTextureItem, strNewID );
	m_rtSceneContents.SortChildren( m_rtSceneContents.GetParentItem( hTextureItem ) );
	m_rtSceneContents.SelectItem( hTextureItem );
}

void CCookieMachineDlg::OnMenuScnCustObjRename( CString strCurrID, CString strNewID )
{
	StrVec*			cust_list	= ((CSceneFile*)pBaseScene)->GetCustObjList();
	bool			found		= false;
	HTREEITEM		hSelected	= m_rtSceneContents.GetSelectedItem();
	CString			strTemp;

	  
	if( strNewID == strCurrID )
		return;

	for( StrVec_iter iter = cust_list->begin(); iter != cust_list->end(); iter++ )
	{
		strTemp = (*iter).data();

		if( strTemp == strNewID )
		{
			found = true;
			break;
		}
	}

	if( found )
	{
		MessageBox( "Custom object с таким ID уже существует!", "Внимание!", MB_OK | MB_ICONINFORMATION );
		return;
	}


	for( StrVec_iter iter = cust_list->begin(); iter != cust_list->end(); iter++ )
	{
		strTemp = (*iter).data();

		if( strTemp == strCurrID )
		{
			(*iter) = strNewID.GetBuffer();
			break;
		}
	}

	m_rtSceneContents.SetItemText( hSelected, strNewID );

	m_rtSceneContents.SortChildren( m_rtSceneContents.GetParentItem( hSelected ) );
	m_rtSceneContents.SelectItem( hSelected );
}

void CCookieMachineDlg::OnMenuScnCustTypeRename( CString strCurrID, CString strNewID )
{
	StrVec*			cust_type	= ((CSceneFile*)pBaseScene)->GetCustTypesList();
	bool			found		= false;
	HTREEITEM		hSelected	= m_rtSceneContents.GetSelectedItem();
	CString			strTemp;
  
	if( strNewID == strCurrID )
		return;

	for( StrVec_iter iter = cust_type->begin(); iter != cust_type->end(); iter++ )
	{
		strTemp = (*iter).data();

		if( strTemp == strNewID )
		{
			found = true;
			break;
		}
	}

	if( found )
	{
		MessageBox( "Custom type с таким ID уже существует!", "Внимание!", MB_OK | MB_ICONINFORMATION );
		return;
	}


	for( StrVec_iter iter = cust_type->begin(); iter != cust_type->end(); iter++ )
	{
		strTemp = (*iter).data();

		if( strTemp == strCurrID )
		{
			(*iter) = strNewID.GetBuffer();
			break;
		}
	}

	m_rtSceneContents.SetItemText( hSelected, strNewID );

	m_rtSceneContents.SortChildren( m_rtSceneContents.GetParentItem( hSelected ) );
	m_rtSceneContents.SelectItem( hSelected );
}

void CCookieMachineDlg::OnMenuScnLightRename( CString strCurrID, CString strNewID )
{

	if( strNewID == strCurrID )
		return;

	if( ((CSceneFile*)pBaseScene)->GetLightByID( strNewID.GetBuffer() ) )
	{
		MessageBox( "Источник света с таким ID уже существует!", "Внимание!", MB_OK | MB_ICONINFORMATION );
		return;
	}
	else if( strNewID.GetLength() > 15 )
	{
		MessageBox( "ID источника света не может быть длинее 15ти знаков!", "Внимание!", MB_OK | MB_ICONINFORMATION );
		return;
	}

	CLIGHT* pLight = ((CSceneFile*)pBaseScene)->GetLightByID( strCurrID.GetBuffer() );

	if( !pLight )
		return;

	pLight->name = strNewID.GetBuffer();

	HTREEITEM hLightItem = m_rtSceneContents.GetSelectedItem();

	m_rtSceneContents.SetItemText( hLightItem, strNewID );
	m_rtSceneContents.SortChildren( m_rtSceneContents.GetParentItem( hLightItem ) );
	m_rtSceneContents.SelectItem( hLightItem );
}

void CCookieMachineDlg::OnMenuScnCameraRename( CString strCurrID, CString strNewID )
{
	if( strNewID == strCurrID )
		return;

	if( ((CSceneFile*)pBaseScene)->GetCameraByID( strNewID.GetBuffer() ) )
	{
		MessageBox( "Камера с таким ID уже существует!", "Внимание!", MB_OK | MB_ICONINFORMATION );
		return;
	}
	else if( strNewID.GetLength() > 15 )
	{
		MessageBox( "ID камеры не может быть длинее 15ти знаков!", "Внимание!", MB_OK | MB_ICONINFORMATION );
		return;
	}

	CCAMERA2* pCamera = ((CSceneFile*)pBaseScene)->GetCameraByID( strCurrID.GetBuffer() );

	if( !pCamera )
		return;

	pCamera->name = strNewID.GetBuffer();

	HTREEITEM hCamItem = m_rtSceneContents.GetSelectedItem();

	m_rtSceneContents.SetItemText( hCamItem, strNewID );
	m_rtSceneContents.SortChildren( m_rtSceneContents.GetParentItem( hCamItem ) );
	m_rtSceneContents.SelectItem( hCamItem );
}

void CCookieMachineDlg::OnMenuScnLightDelete()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	HTREEITEM hSelected = m_rtSceneContents.GetSelectedItem();
	str_t item_text = m_rtSceneContents.GetItemText( hSelected ).GetBuffer();

	//delete tree node
	m_rtSceneContents.DeleteItem( hSelected );

	((CSceneFile*)pBaseScene)->DeleteLight( item_text );
	pBaseScene->Prepare( g_pd3dDevice );
}

void CCookieMachineDlg::OnMenuScnCameraDelete()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	HTREEITEM	hSelected = m_rtSceneContents.GetSelectedItem();
	str_t		item_text = m_rtSceneContents.GetItemText( hSelected ).GetBuffer();

	//delete tree node
	m_rtSceneContents.DeleteItem( hSelected );

	((CSceneFile*)pBaseScene)->DeleteCamera( item_text );
	pBaseScene->Prepare( g_pd3dDevice );
}

void CCookieMachineDlg::OnMenuViewSpotlightcone()
{
	m_ViewMgr.m_SpotLightsConeVisible = !m_ViewMgr.m_SpotLightsConeVisible;

	if( m_ViewMgr.m_SpotLightsConeVisible )
		m_menuViewPopup.CheckMenuItem( ID_MENU_VIEW_SPOTLIGHTCONE, MF_CHECKED ); 
	else
		m_menuViewPopup.CheckMenuItem( ID_MENU_VIEW_SPOTLIGHTCONE, MF_UNCHECKED );
}

void CCookieMachineDlg::OnMenuScnRestoreEngineCams()
{
	//g_MemMappingsMgr->PostScriptToEngine( "Quest:SetGameControlsCam( true ); \r\n" );
}

BOOL CCookieMachineDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	str_t func_decl_str = "BOOL CCookieMachineDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)";

	try
	{
		if( m_ViewMgr.IsCamRotating() )
		{
			SetCursor( AfxGetApp()->LoadCursor(IDC_CURSOR_ROT) );
			return true;
		}
		else if( m_ViewMgr.IsCamZooming() )
		{
			SetCursor( AfxGetApp()->LoadCursor(IDC_CURSOR_TRN_Z) );
			return true;	
		}
		else if( m_ViewMgr.IsCamTranslating() )
		{
			SetCursor( AfxGetApp()->LoadCursor(IDC_CURSOR_TRN) );
			return true;	
		}
	}
	catch(...)
	{
		g_ExeptLog->__data.push_back( func_decl_str );
		throw;
	}
  
	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

void CCookieMachineDlg::OnMenuViewCameraInterest()
{
	m_bIsCameraInterestVisible = !m_bIsCameraInterestVisible;

	if( m_bIsCameraInterestVisible )
		m_menuViewPopup.CheckMenuItem( ID_MENU_VIEW_CAMERA_INTEREST, MF_CHECKED );
	else
		m_menuViewPopup.CheckMenuItem( ID_MENU_VIEW_CAMERA_INTEREST, MF_UNCHECKED );
}

void CCookieMachineDlg::OnMenuViewNormals()
{
	m_bNormalsRendered = !m_bNormalsRendered;

	if( m_bNormalsRendered )
	{
		m_menuViewPopup.CheckMenuItem( ID_MENU_VIEW_NORMALS, MF_CHECKED );
	}
	else
	{
		m_menuViewPopup.CheckMenuItem( ID_MENU_VIEW_NORMALS, MF_UNCHECKED );
	}

	pBaseScene->SetRenderNormals( m_bNormalsRendered );
}

void CCookieMachineDlg::OnMenuCreateMeshlist()
{
	SetCursor( LoadCursor( NULL, IDC_WAIT ) );

	str_t file_dir = (m_strAppPath + "\\data\\templates\\").GetBuffer();
	str_t file_name = "empty_meshlist.geom.efn";

	/**********************************************************/
	/******                   MSH FILE OPEN              ******/
	/**********************************************************/
	CEfnMeshlist*	meshlist = NULL;
	DWORD			file_version = GetScnVersion( file_dir + file_name );

	if( meshlist->IsSupportedVersion( file_version ) )
	{
		CloseCurrentFile();

		meshlist = new CEfnMeshlist;

		if( meshlist->LoadFromFile( file_dir + file_name ) )
		{
			meshlist->SetParent( this );
			pBaseScene = meshlist;

			if( m_appSettings.ScnAlternateNullIco )
				meshlist->SetNullIcon( pNullIco );

			//prepare data for rendering
			pBaseScene->Prepare( g_pd3dDevice );

			SetWindowText( "Cookie Machine -- не указано" );

			meshlist->SetPaths( "", "" );

			UpdateUI();
			EnableControls( true );

			return;
		}
		else
		{
			// load failed -- cleanup
			MessageBox( "Corrupted meshlist template file! Meshlist couldn't not be created.", "Warning!", MB_OK | MB_ICONEXCLAMATION );
			
			SAFE_DELETE( meshlist );
			SetCursor( LoadCursor( NULL, IDC_ARROW ) );
			return;
		}
	}


	MessageBox( "Unsupported SCN template version! Scene couldn't not be created.", "Warning!", MB_OK | MB_ICONEXCLAMATION );
	SetCursor( LoadCursor( NULL, IDC_ARROW ) );
}

void CCookieMachineDlg::OnMenuCreateScene()
{
	SetCursor( LoadCursor( NULL, IDC_WAIT ) );

	str_t file_dir = (m_strAppPath + "\\data\\templates\\").GetBuffer();
	str_t file_name = "empty_scene.scn";

	/**********************************************************/
	/******                   SCN FILE OPEN              ******/
	/**********************************************************/
	CSceneFile* scene = NULL;
	DWORD		file_version = GetScnVersion( file_dir + file_name );

	if( scene->IsSupportedVersion( file_version ) )
	{
		CloseCurrentFile();

		scene = new CSceneFile;

		scene->SetPaths( file_dir, file_name );

		if( scene->LoadFromFile( file_dir + file_name ) )
		{
			scene->SetParent( this );
			pBaseScene = scene;

			CEfnMeshlist* meshlist = (CEfnMeshlist*)scene->GetMeshlist();

			pBaseScene->SetPaths( "", "" );
			meshlist->SetPaths( "", "" );

			if( m_appSettings.ScnAlternateNullIco )
				meshlist->SetNullIcon( pNullIco );

			

			//prepare data for rendering
			pBaseScene->Prepare( g_pd3dDevice );

			((CSceneFile*)pBaseScene)->SetLightBulbCone( pLightBulb, pLightCone );
			((CSceneFile*)pBaseScene)->SetCamBox( pCamBox );

			SetWindowText( "Cookie Machine -- не указано" );

			UpdateUI();
			EnableControls( true );

			return;
		}
		else
		{
			// load failed -- cleanup
			MessageBox( "Corrupted SCN template file! Scene couldn't not be created.", "Warning!", MB_OK | MB_ICONEXCLAMATION );
			
			SAFE_DELETE( scene );
			SetCursor( LoadCursor( NULL, IDC_ARROW ) );
			return;
		}
	}


	MessageBox( "Unsupported SCN template version! Scene couldn't not be created.", "Warning!", MB_OK | MB_ICONEXCLAMATION );
	SetCursor( LoadCursor( NULL, IDC_ARROW ) );
}

void CCookieMachineDlg::OnMenuHidebytypeCameras()
{
	OnMenuScnCamerasHide();
}

void CCookieMachineDlg::OnMenuShowbytypeCameras()
{
	OnMenuScnCamerasShow();
}

void CCookieMachineDlg::OnMenuHidebytypeLights()
{
	OnMenuScnLightsHide();
}

void CCookieMachineDlg::OnMenuShowbytypeLights()
{
	OnMenuScnLightsShow();
}

void CCookieMachineDlg::OnMenuHidebytypeMeshes()
{
	OnMenuScnMeshesHide();
	OnMenuMeshlistMeshesHide();
}

void CCookieMachineDlg::OnMenuShowbytypeMeshes()
{
	OnMenuScnMeshesShow();
	OnMenuMeshlistMeshesShow();
}

void CCookieMachineDlg::OnMenuHidebytypeNulls()
{
	OnMenuScnNullsHide();
	OnMenuMeshlistNullsHide();
}

void CCookieMachineDlg::OnMenuShowbytypeNulls()
{
	OnMenuScnNullsShow();
	OnMenuMeshlistNullsShow();
}

void CCookieMachineDlg::OnMenuHidebytypeCurves()
{
	OnMenuScnCurvesHide();
}

void CCookieMachineDlg::OnMenuShowbytypeCurves()
{
	OnMenuScnCurvesShow();
}

void CCookieMachineDlg::OnMenuHidebytypeAll()
{
	OnMenuScnCamerasHide();
	OnMenuScnLightsHide();
	OnMenuScnMeshesHide();
	OnMenuScnNullsHide();
	OnMenuScnCurvesHide();

	OnMenuMeshlistMeshesHide();
	OnMenuMeshlistNullsHide();
}

void CCookieMachineDlg::OnMenuShowbytypeAll()
{
	OnMenuScnCamerasShow();
	OnMenuScnLightsShow();
	OnMenuScnMeshesShow();
	OnMenuScnNullsShow();
	OnMenuScnCurvesShow();

	OnMenuMeshlistMeshesShow();
	OnMenuMeshlistNullsShow();
}

void CCookieMachineDlg::OnMenuHidebytypeFloors()
{
	OnMenuScnShowMeshByType( false, "Floor" );
}

void CCookieMachineDlg::OnMenuShowbytypeFloors()
{
	OnMenuScnShowMeshByType( true, "Floor" );
}

void CCookieMachineDlg::OnMenuHidebytypeLayergeom()
{
	OnMenuScnShowMeshByType( false, "LayerGeom" );
}

void CCookieMachineDlg::OnMenuShowbytypeLayergeom()
{
	OnMenuScnShowMeshByType( true, "LayerGeom" );
}

void CCookieMachineDlg::OnMenuHidebytypeCameraselectors()
{
	OnMenuScnShowMeshByType( false, "CameraSelect" );
}

void CCookieMachineDlg::OnMenuShowbytypeCameraselectors()
{
	OnMenuScnShowMeshByType( true, "CameraSelect" );
}

void CCookieMachineDlg::OnMenuHidebytypeActionobjects()
{
	OnMenuScnShowMeshByType( false, "Action" );
}

void CCookieMachineDlg::OnMenuShowbytypeActionobjects()
{
	OnMenuScnShowMeshByType( true, "Action" );
}

void CCookieMachineDlg::OnMenuHidebytypeOccluders()
{
	OnMenuScnShowMeshByType( false, "OcclusionGeom" );
}

void CCookieMachineDlg::OnMenuShowbytypeOccluders()
{
	OnMenuScnShowMeshByType( true, "OcclusionGeom" );
}

void CCookieMachineDlg::OnMenuScnShowMeshByType( bool bShow, str_t strType )
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	HTREEITEM		hMeshes		= m_rtSceneContents.FindTreeItemByName( "Meshes" );
	CSceneFile*		pScene		= (CSceneFile*)pBaseScene;
	CEfnMeshlist*	pMeshlist	= (CEfnMeshlist*)pScene->GetMeshlist();

	//get list of floor meshes
	ListStrVec*		pBinds	= pScene->GetBinds();
	ListStrVec_iter	iter	= pBinds->begin();

	while( iter != pBinds->end() )
	{
		StrVec strVec = *iter;

		if( strVec[2] == "Scene" && strVec[4] == strType )
		{
			CMesh* pMesh = pMeshlist->GetMeshByName( strVec[3] );

			if( pMesh )
			{
				pMesh->SetVisible( bShow );

				HTREEITEM hMesh = m_rtSceneContents.FindTreeItemByName( strVec[3], hMeshes );

				if( bShow )
					m_rtSceneContents.SetItemImage( hMesh, 0, 0 );
				else
					m_rtSceneContents.SetItemImage( hMesh, 4, 4 );
			}
		}

		iter++;
	}
}

void CCookieMachineDlg::OnMenuBatchConvertXSI2EFNAnim()
{
	CBatchConvertDlg* dlg = new CBatchConvertDlg(this);
	dlg->SetConvertType( BCT_XSI2EFN );
	dlg->DoModal();

	SAFE_DELETE( dlg );
}

void CCookieMachineDlg::OnMenuCreateobjectNull()
{
	D3DXVECTOR3 isect;

	if( GetClosestIntersectionFromMeshlist( &isect ) )
	{
		if( pBaseScene->GetType() == FT_EFN_MESHLIST )
		{
			OnMenuMeshlistNullsNewByPos( &isect );
		}
		else if( pBaseScene->GetType() == FT_SCN )
		{
			OnMenuSceneNullsNewByPos( &isect );
		}
	}
}

void CCookieMachineDlg::OnMenuCreateobjectBox()
{
	D3DXVECTOR3 isect;

	if( GetClosestIntersectionFromMeshlist( &isect ) )
	{
		OnMenuObjectCreateMesh( GEOM_CUBE, &isect );
	}
}

void CCookieMachineDlg::OnMenuCreateobjectQuad()
{
	D3DXVECTOR3 isect;

	if( GetClosestIntersectionFromMeshlist( &isect ) )
	{
		OnMenuObjectCreateMesh( GEOM_QUAD, &isect );
	}
}

void CCookieMachineDlg::OnMenuCreateobjectTriangle()
{
	D3DXVECTOR3 isect;

	if( GetClosestIntersectionFromMeshlist( &isect ) )
	{
		OnMenuObjectCreateMesh( GEOM_TRI, &isect );
	}
}

bool CCookieMachineDlg::GetClosestIntersectionFromMeshlist( D3DXVECTOR3* isect )
{
	//selecting objects in render view
	D3DXVECTOR3		out, scr = D3DXVECTOR3( g_iMouseX, g_iMouseY, 0 );
	D3DVIEWPORT9	viewport;
	D3DXMATRIX		world;

	D3DXMatrixIdentity( &world );
	g_pd3dDevice->GetViewport( &viewport );
	D3DXVec3Unproject( &out, &scr, &viewport, &matProj, &m_matView, &world );

	out = out - m_ViewMgr.m_camCurrent->pos;
	D3DXVec3Normalize( &out, &out );

	D3DXMATRIX ident_mtx;
	D3DXMatrixIdentity( &ident_mtx );

	if( !pBaseScene || m_bIsVertexEditingModeOn )
		return false;

	CEfnMeshlist* pMeshlist = NULL;

	if( pBaseScene->GetType() == FT_EFN_MESHLIST )
	{
		pMeshlist = ((CEfnMeshlist*)pBaseScene); 
	}
	else if( pBaseScene->GetType() == FT_SCN )
	{
		CSceneFile* pScene = ((CSceneFile*)pBaseScene);
		pMeshlist = (CEfnMeshlist*)pScene->GetMeshlist(); 
	}


	if( !pMeshlist )
		return false;

	enum	OBJECT_TYPES objType;
	str_t	strObjID;
	float	distance = pMeshlist->IntersectByRay( m_ViewMgr.m_camCurrent->pos, out, ident_mtx, objType, strObjID );

	if( distance == -1.0f )
	{
		*isect = D3DXVECTOR3( 0, 0, 0 );
	}
	else
	{
		D3DXVec3Normalize( &out, &out );
		D3DXVec3Scale( &out, &out, distance );
		out += m_ViewMgr.m_camCurrent->pos;

		*isect = out;
	}

	return true;
}

void CCookieMachineDlg::OnMenuScnQuickBind( str_t BindType )
{
	//if( !pBaseScene )
	//	return;

	//if( pBaseScene->GetType() != FT_SCN )
	//	return;

	//CQuickBindDlg*	dlg			= new CQuickBindDlg( this );
	//HTREEITEM		hItem		= m_rtSceneContents.GetSelectedItem();
	//str_t			strTexID	= m_rtSceneContents.GetItemText( hItem ).GetBuffer();
	//ListStrVec*		binds		= ((CSceneFile*)pBaseScene)->GetBinds();

	//dlg->SetBinds( binds );
	//dlg->SetSceneID( ((CSceneFile*)pBaseScene)->GetSceneID() );

	//if( BindType == "CustObject" )
	//{
	//	dlg->BindCustomObj( strTexID );
	//}
	//else if( BindType == "Texture" )
	//{
	//	dlg->BindTexture( strTexID );
	//}
	//else if( BindType == "Mesh2Cam" )
	//{
	//	dlg->BindMesh2Camera( strTexID );
	//}
	//else if( BindType == "Mesh2Tex" )
	//{
	//	dlg->BindMesh2Texture( strTexID );
	//}
	//else if( BindType == "Light" )
	//{
	//	dlg->BindLight( strTexID );
	//}
	//else
	//{
	//	SAFE_DELETE( dlg );
	//	return;
	//}

	//dlg->SetQBindItem( &m_PrevQuickBind );
	//
	//if( dlg->DoModal() == IDOK )
	//	UpdateUI_SCNBinds();

	//SAFE_DELETE( dlg );
}

void CCookieMachineDlg::OnMenuScnQuickBind2( StrVec* Objs, str_t BindType )
{
	//if( !pBaseScene )
	//	return;

	//if( pBaseScene->GetType() != FT_SCN )
	//	return;

	//CQuickBindDlg*	dlg			= new CQuickBindDlg( this );
	//HTREEITEM		hItem		= m_rtSceneContents.GetSelectedItem();
	//str_t			strTexID	= m_rtSceneContents.GetItemText( hItem ).GetBuffer();
	//ListStrVec*		binds		= ((CSceneFile*)pBaseScene)->GetBinds();

	//dlg->SetBinds( binds );
	//dlg->SetSceneID( ((CSceneFile*)pBaseScene)->GetSceneID() );

	//if( BindType == "Meshes2Cam" )
	//{
	//	//dlg->BindCustomObj( strTexID );
	//}
	//else if( BindType == "Meshes2Tex" )
	//{
	//	//dlg->BindTexture( strTexID );
	//}
	//else
	//{
	//	SAFE_DELETE( dlg );
	//	return;
	//}

	////dlg->SetQBindItem( &m_PrevQuickBind );
	//
	//if( dlg->DoModal() == IDOK )
	//	UpdateUI_SCNBinds();

	//SAFE_DELETE( dlg );
}

void CCookieMachineDlg::OnMenuSelGeomAlignPoints()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN && pBaseScene->GetType() != FT_EFN_MESHLIST )
		return;

	CEfnMeshlist* pMeshlist = NULL;
	
	if( pBaseScene->GetType() == FT_SCN )
	{
		CSceneFile* pScene = (CSceneFile*)pBaseScene;
		pMeshlist = (CEfnMeshlist*)pScene->GetMeshlist();
	}
	else if( pBaseScene->GetType() == FT_EFN_MESHLIST )
	{
		pMeshlist = (CEfnMeshlist*)pBaseScene;
	}

	if( !pMeshlist )
		return;

	CMesh* pMesh = pMeshlist->GetSelectedMesh();

	if( !pMesh )
		return;

	pMesh->AlignPoints();
}

void CCookieMachineDlg::OnMenuEfnUnloadTexture()
{
	TNODE* node = ( TNODE* )m_rtSceneContents.GetCurrNodeData();

	if( !node || node->_type != TREENODE::N_TEXTURE )
		return;

	((CEfnCharacter*)node->_data)->SetTexturePath( str_t("") );
	((CEfnCharacter*)node->_data)->ResetTexture( g_pd3dDevice );
}

void CCookieMachineDlg::OnMenuCreateobjectNullOnCurve()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_SCN )
		return;

	if( m_rtSceneContents.GetCurrNodeType() != TNT_SCN_CURVE_NULL )
		return;

	CSceneFile*	pScene		= (CSceneFile*)pBaseScene;
	HTREEITEM	hNull		= m_rtSceneContents.GetSelectedItem();
	CString		strCurve	= m_rtSceneContents.GetParNodeText();
	CurveItem*	pCurve		= pScene->GetCurveByName( strCurve.GetBuffer() );
	HTREEITEM	hCurve		= m_rtSceneContents.GetParNode();

	if( !pCurve || !hCurve )
		return;


	D3DXVECTOR3 isect;

	if( GetClosestIntersectionFromMeshlist( &isect ) )
	{
		str_t		strNewNullID;
		int			iCounter	= 0;
		HTREEITEM	hTemp		= m_rtSceneContents.GetSelectedItem();

		while( hTemp )
		{
			iCounter++;
			hTemp = m_rtSceneContents.GetPrevSiblingItem( hTemp );
		}

		if( iCounter < 0 || iCounter > ((int)pCurve->nulls.size() - 1) )
			return;

		if( OnMenuSceneNullsNewByPos( &isect, &strNewNullID ) )
		{
			pCurve->nulls.insert( pCurve->nulls.begin() + iCounter, strNewNullID );
			m_rtSceneContents.InsertItem2( strNewNullID.data(), 10, NULL, hCurve, hNull );
			m_rtSceneContents.Invalidate( false );
			m_rtSceneContents.SetFocus();

			pScene->PrepareCurves();
		}
	}
}

void CCookieMachineDlg::OnMenuEfnUseUV1()
{
	CMenu menuPopupEFN;
	menuPopupEFN.LoadMenu( IDR_MENU_CONTEXT_EFN );

	if( ((CEfnCharacter*)pBaseScene)->IsUV1Used() == false )
	{
		CheckMenuItem( menuPopupEFN.GetSubMenu( 7 )->GetSafeHmenu(), ID_MENU_EFN_USE_UV1, MF_CHECKED );
		CheckMenuItem( menuPopupEFN.GetSubMenu( 7 )->GetSafeHmenu(), ID_MENU_EFN_USE_UV2, MF_UNCHECKED );

		((CEfnCharacter*)pBaseScene)->SetUV1Used( true );
	}
}

void CCookieMachineDlg::OnMenuEfnUseUV2()
{
	CMenu menuPopupEFN;
	menuPopupEFN.LoadMenu( IDR_MENU_CONTEXT_EFN );

	if( ((CEfnCharacter*)pBaseScene)->IsUV1Used() == true )
	{
		CheckMenuItem( menuPopupEFN.GetSubMenu( 7 )->GetSafeHmenu(), ID_MENU_EFN_USE_UV1, MF_UNCHECKED );
		CheckMenuItem( menuPopupEFN.GetSubMenu( 7 )->GetSafeHmenu(), ID_MENU_EFN_USE_UV2, MF_CHECKED );				

		((CEfnCharacter*)pBaseScene)->SetUV1Used( false );
	}
}

void CCookieMachineDlg::OnMenuEfnNullDelete()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN )
		return;

	TNODE*			node		= (TNODE*)m_rtSceneContents.GetCurrNodeData();
	CEfnCharacter*	pEfnAnim	= (CEfnCharacter*)node->_data;

	if( !pEfnAnim || !node || node->_type != TREENODE::N_NULL )
		return;

	pEfnAnim->DeleteNullByName( m_rtSceneContents.GetCurrNodeText().GetBuffer() );
	m_rtSceneContents.DeleteItem( m_rtSceneContents.GetSelectedItem() );
}

void CCookieMachineDlg::OnMenuEfnNullsCreateNew()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN )
		return;

	TNODE*			node		= (TNODE*)m_rtSceneContents.GetCurrNodeData();
	CEfnCharacter*	pEfnAnim	= (CEfnCharacter*)node->_data;

	if( !pEfnAnim || !node || node->_type != TREENODE::N_NULLS )
		return;

	CModelNameDlg* dlgNullName = new CModelNameDlg(this);

	dlgNullName->SetCaption( "Название нуля:" );
	dlgNullName->SetString( "" );
	dlgNullName->DoModal();

	str_t strNewNullName = dlgNullName->GetString().GetBuffer();

	SAFE_DELETE( dlgNullName );

	if( pEfnAnim->GetNullByName( strNewNullName ) != false )
	{
		MessageBox( "Нуль с таким ID уже существует!", "Внимание!", MB_OK | MB_ICONINFORMATION );
		return;
	}

	pEfnAnim->CreateNewNull( strNewNullName );
	m_rtSceneContents.InsertItem2( strNewNullName.data(), 10, new TNODE( TREENODE::N_NULL, pEfnAnim ), m_rtSceneContents.GetSelectedItem(), TVI_SORT );
}

void CCookieMachineDlg::OnNMDblclkSceneTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	if( !pBaseScene )
		return;

	enum TREE_NODE_TYPE tnt = m_rtSceneContents.GetCurrNodeType();
	TNODE* data = (TNODE*)m_rtSceneContents.GetCurrNodeData();

	if( pBaseScene->GetType() == FT_SCN )
	{
		if( tnt == TNT_TEXTURE || ( tnt == TNT_SCN_CAMERA_BIND && m_rtSceneContents.GetCurrNodeImage() == 21 ) )
		{	
			CSceneFile* pScene		= (CSceneFile*)pBaseScene;
			CString strVideoItemID	= m_rtSceneContents.GetCurrNodeText();
			CString strTemp			= strVideoItemID.Right( 4 );

			strTemp.MakeUpper();

			CString strPath = m_appSettings.EngineFolder;

			if( strTemp == ".BIK" )
			{
				strPath += "\\data\\Video\\BG\\" + (CString)pScene->GetLevelID().data() + 
							"\\" + (CString)pScene->GetSceneID().data() + "." + strVideoItemID;

				if( FileExists( strPath.GetBuffer() ) == false )
				{
					MessageBox( "File was not found!", "Warning.", MB_OK | MB_ICONEXCLAMATION );
					return;
				}

				CBinkInfoDlg dlg;
				dlg.file	= strPath;
				dlg.appPath = m_strAppPath;
				dlg.DoModal();
			}
			else
			{
				strPath += "\\data\\Textures\\BG\\" + (CString)pScene->GetLevelID().data() + 
						"\\" + (CString)pScene->GetSceneID().data() + "." + strVideoItemID + ".dds";

				if( FileExists( strPath.GetBuffer() ) == false )
				{
					MessageBox( "File was not found!", "Warning.", MB_OK | MB_ICONEXCLAMATION );
					return;
				}

				WinExec( m_strAppPath + "\\utils\\wtv.exe " + strPath, 1 );
			}
		}
	}

	if( pBaseScene->GetType() == FT_EFN && data && data->_type == TREENODE::N_ANIMATION )
		OnMenuEfnAnimationRun();

	*pResult = 0;
}

void CCookieMachineDlg::OnMenuScnCameraMiscObjectEditPos()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_SCN )
		return;

	CSceneFile* pScene = (CSceneFile*)pBaseScene;

	CString strTemp = m_rtSceneContents.GetCurrNodeText();

	strTemp.Replace( '.', '_' );

	pScene->SetEditedObject( strTemp.GetBuffer() );
}

void CCookieMachineDlg::OnMenuScnFinishObjectPosEditing()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_SCN )
		return;

	((CSceneFile*)pBaseScene)->SetEditedObject( "" );
}

void CCookieMachineDlg::OnMenuScnEditDudePos()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_SCN )
		return;

	((CSceneFile*)pBaseScene)->SetEditedObject( "dude" );
}

void CCookieMachineDlg::OnMenuSelGeomBind()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_SCN )
		return;

	CSceneFile*		pScene		= (CSceneFile*)pBaseScene;
	CEfnMeshlist*	pMeshlist	= (CEfnMeshlist*)pScene->GetMeshlist();

	if( pMeshlist->GetSelectedMeshesCount() <= 0 )
		return;

	StrVec MeshesList;

	for( int i = 0; i < pMeshlist->GetMeshesCount(); i++ )
	{
		CMesh* pMesh = pMeshlist->GetMeshByIndex( i );

		if( !pMesh )
			return;

		if( pMesh->IsSelected() )
			MeshesList.push_back( pMesh->GetName() );
	}

	OnMenuScnQuickBind2( &MeshesList, "Meshes2Cam" );
}

void CCookieMachineDlg::OnTempAddlod()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_EFN )
		return;

	str_t	szFilter = "EFN Files (*.efn)|*.efn|";
	CString	temp, extention;
	str_t file_dir, file_name; 

	CCustomFileDialog* dlg = new CCustomFileDialog( TRUE, temp.GetBuffer(0), 0, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_ENABLETEMPLATE | OFN_ENABLESIZING, szFilter.data(), NULL );
	dlg->m_pOFN->lpstrInitialDir	= "E:\\inbox\\Kamikaza\\Reduction";
	dlg->m_pOFN->FlagsEx			= OFN_EX_NOPLACESBAR;
	dlg->m_pOFN->lpTemplateName		= MAKEINTRESOURCE( IDD_OPENFILE_EXT );
	dlg->m_pOFN->hInstance			= GetModuleHandle(NULL);

	m_bIsAppPaused = false;

	//user confirmed "open file dialog"
	if( dlg->DoModal() == IDOK )
	{
		//get file dir
		file_dir	= dlg->GetPathName().Left( dlg->GetPathName().GetLength() - dlg->GetFileName().GetLength() );

		//get filename
		file_name	= dlg->GetFileName();

		extention = dlg->GetFileExt();
	}

	SAFE_DELETE( dlg );

	extention.MakeUpper();

	if( extention != "EFN" )
		return;

	CValidateDlg* dlgValidate = NULL;

	//show validation window
	if( !dlgValidate )
	{
		dlgValidate = new CValidateDlg( this );
		dlgValidate->SetMessageStr( "Проверка файла..." );
		ProcessMessages();
	}

	/*********************************************************/
	/******            EFN ANIM	FILE OPEN				******/
	/*********************************************************/
	
	bool bFileSuccessfullyOpened = false;

	//check file prefix
	if( GetEfnFiletype( file_dir + file_name ) != EF_ANIMATION )
		return;

	CEfnCharacter* efn_character = new CEfnCharacter;

	if( dlgValidate )
	{
		dlgValidate->SetMessageStr( "Загрузка файла..." );
		ProcessMessages();
	}

	efn_character->SetValidationDlg( dlgValidate );

	if( efn_character->LoadFromFile( file_dir + file_name ) )
	{
		efn_character->SetParentWnd( this );
		efn_character->SetPaths( file_dir, file_name );

		if( dlgValidate )
		{
			dlgValidate->SetProgressMessageStrA( "step 5 of 5" );
			dlgValidate->SetProgressMessageStrC( "Preparing buffers..." );
			dlgValidate->Step();
			ProcessMessages();
		}

		bFileSuccessfullyOpened = true;
	}
	else
	{
		// load failed -- cleanup
		SAFE_DELETE( efn_character ); 
	}

	efn_character->SetValidationDlg( NULL );

	if( !bFileSuccessfullyOpened )
		return;
	
	((CEfnCharacter*)pBaseScene)->AddLodFromAnimEfn( efn_character, g_pd3dDevice );

	SAFE_DELETE( efn_character );
	SAFE_DELETE( dlgValidate );
}

void CCookieMachineDlg::OnMenuBatchConvertXSI2EFNMeshlist()
{
	CBatchConvertDlg* dlg = new CBatchConvertDlg(this);
	dlg->SetConvertType( BCT_XSI2MESHLIST );
	dlg->DoModal();

	SAFE_DELETE( dlg );
}

void CCookieMachineDlg::OnMenuBatchConvertErrorCorrection()
{
	CBatchConvertDlg* dlg = new CBatchConvertDlg(this);
	dlg->SetConvertType( BCT_ERRORCHECK );
	dlg->DoModal();

	SAFE_DELETE( dlg );
}

void CCookieMachineDlg::ShowSceneExplorer( bool bValue )
{
	static int old[8];
	int width;

	CWnd* pFrame		= GetDlgItem( IDC_CONT_FRAME );
	CWnd* pTree			= GetDlgItem( IDC_SCENE_TREE );
	CWnd* pTopBevel		= GetDlgItem( IDC_FRM_TOOLBAR_TOP );
	CWnd* pCams			= GetDlgItem( IDC_CMB_CAMERAS );
	CWnd* pViewMenu		= GetDlgItem( IDC_BMP );
	CWnd* pCamsLbl		= GetDlgItem( IDC_CAM_CMB_LBL );
	CWnd* pViews		= GetDlgItem( IDC_CMB_VIEWS );
	CWnd* pViewLbl		= GetDlgItem( IDC_VIEW_CMB_LBL );

	CRect rcTemp;

	pFrame->ShowWindow( bValue );
	pTree->ShowWindow( bValue );
	m_wndSplitter1.ShowWindow( bValue );

	pFrame->UpdateWindow();
	pTree->UpdateWindow();
	m_wndSplitter1.UpdateWindow();

	if( bValue )
	{
		m_iSplitterX = old[0];
		
		pTopBevel->GetWindowRect( &rcTemp );
		ScreenToClient( &rcTemp );
		rcTemp.left = old[1];
		pTopBevel->MoveWindow( &rcTemp, true );

		pCamsLbl->GetWindowRect( &rcTemp );
		ScreenToClient( &rcTemp );
		width = rcTemp.Width();
		rcTemp.left = old[2];
		rcTemp.right = rcTemp.left + width;
		pCamsLbl->MoveWindow( &rcTemp, true );

		pCams->GetWindowRect( &rcTemp );
		ScreenToClient( &rcTemp );
		width = rcTemp.Width();
		rcTemp.left = old[3];
		rcTemp.right = rcTemp.left + width;
		pCams->MoveWindow( &rcTemp, true );

		pViewMenu->GetWindowRect( &rcTemp );
		ScreenToClient( &rcTemp );
		width = rcTemp.Width();
		rcTemp.left = old[4];
		rcTemp.right = rcTemp.left + width;
		pViewMenu->MoveWindow( &rcTemp, true );

		pViews->GetWindowRect( &rcTemp );
		ScreenToClient( &rcTemp );
		width = rcTemp.Width();
		rcTemp.left = old[5];
		rcTemp.right = rcTemp.left + width;
		pViews->MoveWindow( &rcTemp, true );

		pViewLbl->GetWindowRect( &rcTemp );
		ScreenToClient( &rcTemp );
		width = rcTemp.Width();
		rcTemp.left = old[6];
		rcTemp.right = rcTemp.left + width;
		pViewLbl->MoveWindow( &rcTemp, true );
	}
	else
	{
		pTopBevel->GetWindowRect( &rcTemp );
		ScreenToClient( &rcTemp );
		old[1] = rcTemp.left;
		rcTemp.right -= rcTemp.left;
		rcTemp.left = 0;
		pTopBevel->MoveWindow( &rcTemp, true );

		pCamsLbl->GetWindowRect( &rcTemp );
		ScreenToClient( &rcTemp );
		old[2] = rcTemp.left;
		width = rcTemp.Width();
		rcTemp.left -= m_iSplitterX;
		rcTemp.right = rcTemp.left + width;
		pCamsLbl->MoveWindow( &rcTemp, true );

		pCams->GetWindowRect( &rcTemp );
		ScreenToClient( &rcTemp );
		old[3] = rcTemp.left;
		width = rcTemp.Width();
		rcTemp.left -= m_iSplitterX;
		rcTemp.right = rcTemp.left + width;
		pCams->MoveWindow( &rcTemp, true );

		pViewMenu->GetWindowRect( &rcTemp );
		ScreenToClient( &rcTemp );
		old[4] = rcTemp.left;
		width = rcTemp.Width();
		rcTemp.left -= m_iSplitterX;
		rcTemp.right = rcTemp.left + width;
		pViewMenu->MoveWindow( &rcTemp, true );

		pViews->GetWindowRect( &rcTemp );
		ScreenToClient( &rcTemp );
		old[5] = rcTemp.left;
		width = rcTemp.Width();
		rcTemp.left -= m_iSplitterX;
		rcTemp.right = rcTemp.left + width;
		pViews->MoveWindow( &rcTemp, true );

		pViewLbl->GetWindowRect( &rcTemp );
		ScreenToClient( &rcTemp );
		old[6] = rcTemp.left;
		width = rcTemp.Width();
		rcTemp.left -= m_iSplitterX;
		rcTemp.right = rcTemp.left + width;
		pViewLbl->MoveWindow( &rcTemp, true );

		old[0] = m_iSplitterX;
		m_iSplitterX = 0;
	}

	pTopBevel->UpdateWindow();
	pCams->UpdateWindow();
	pViewMenu->UpdateWindow();
	pCamsLbl->UpdateWindow();
	pViews->UpdateWindow();
	pViewLbl->UpdateWindow();

	DoResize(0);

	UpdateUISizing();
	HandlePossibleSizeChange();
}

void CCookieMachineDlg::ToggleSceneExplorer()
{
	m_bIsSceneExplorerVisible = !m_bIsSceneExplorerVisible;
	ShowSceneExplorer( m_bIsSceneExplorerVisible );
}

void CCookieMachineDlg::OnMenuSelGeomMergePoints()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN && pBaseScene->GetType() != FT_EFN_MESHLIST )
		return;

	CEfnMeshlist* pMeshlist = NULL;
	
	if( pBaseScene->GetType() == FT_SCN )
	{
		CSceneFile* pScene = (CSceneFile*)pBaseScene;
		pMeshlist = (CEfnMeshlist*)pScene->GetMeshlist();
	}
	else if( pBaseScene->GetType() == FT_EFN_MESHLIST )
	{
		pMeshlist = (CEfnMeshlist*)pBaseScene;
	}

	if( !pMeshlist )
		return;

	CMesh* pMesh = pMeshlist->GetSelectedMesh();

	if( !pMesh )
		return;

	pMesh->MergePoints();
}

void CCookieMachineDlg::OnMenuScnCameraMiscObjectDelete()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_SCN )
		return;

	HTREEITEM hSelected, hParent, hParentParent, hParentParentParent;
	CString strParentText, strParentParentText, hSelItemText, strParentParentParentText;

	hSelected		= m_rtSceneContents.GetSelectedItem();
	hSelItemText	= m_rtSceneContents.GetItemText( hSelected );

	if( hSelected >= 0 )
		hParent = m_rtSceneContents.GetParentItem( hSelected );

	if( hParent >= 0 )
	{
		hParentParent	= m_rtSceneContents.GetParentItem( hParent );
		strParentText	= m_rtSceneContents.GetItemText( hParent );
	}

	if( hParentParent >= 0 )
	{
		strParentParentText			= m_rtSceneContents.GetItemText( hParentParent );
		hParentParentParent			= m_rtSceneContents.GetParentItem( hParentParent );
		strParentParentParentText	= m_rtSceneContents.GetItemText( hParentParentParent );
	}

	CSceneFile*		scene	= ((CSceneFile*)pBaseScene);
	ListStrVec*		binds	= scene->GetBinds();
	ListStrVec_iter	iter	= binds->begin();


	//find whether object is binded to scene
	bool ok = false;

	while( iter != binds->end() )
	{
		if( (*iter)[1] == strParentParentText.GetBuffer() && (*iter)[2] == "Camera" && ((*iter)[4] + "." +(*iter)[3]).data() == hSelItemText )
		{
			ok = true;
			break;
		}

		iter++;
	}
	
	//prepare last edited item and assign it to scene
	if( ok )
	{
		DeleteFuncEntriesByKey( scene->GetFuncs(), atoi( (*iter)[0].data() ) );
		m_rtSceneContents.DeleteItem( hSelected );
		binds->erase( iter );
	}
}

void CCookieMachineDlg::OnMenuEfnCropTool()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_EFN )
		return;

	int old_frames_count = ((CEfnCharacter*)pBaseScene)->GetFramesCount();
	int old_tri_count    = ((CEfnCharacter*)pBaseScene)->GetTrianglesCount();
	int old_vert_count   = ((CEfnCharacter*)pBaseScene)->GetVerteciesCount();

	vector<int> ranges_del;

	CFramesCropToolDlg* pCropTool = new CFramesCropToolDlg( this );
	pCropTool->SetFramesCount( ((CEfnCharacter*)pBaseScene)->GetFramesCount() );
	pCropTool->SetRangesDel( &ranges_del );

	if( pCropTool->DoModal() == IDOK )
	{
		if( ranges_del.empty() || ranges_del.size() == 0 )
		{
			pCropTool->DestroyWindow();
			SAFE_DELETE( pCropTool );

			return;
		}

		SetCursor( LoadCursor( NULL, IDC_WAIT ) );

		CValidateDlg* pDlgValidate = new CValidateDlg( this );

		pDlgValidate->SetMessageStr("Удаление кадров...");
		pDlgValidate->SetProgressRange( 0, ranges_del.size() / 2 );

		ProcessMessages();

		CString popa;

		int iStart = ranges_del.size() / 2 - 1;

		for( int i = iStart; i >= 0; i-- )
		{
			//CString popa;

			//for( int i = 0; i < (int)ranges_del->size() / 2; i++ )
			//{
			//	CString strTemp;
			//	strTemp.Format( "%d -- %d\r\n", ranges_del[i*2], ranges_del[i*2+1] );
			//	popa += strTemp;
			//}

			//

			( (CEfnCharacter*)pBaseScene )->DeleteFrames( ranges_del[i * 2], ranges_del[i * 2 + 1] );

			m_vecMacrosDeletedFrames.push_back( ranges_del[i * 2] );
			m_vecMacrosDeletedFrames.push_back( ranges_del[i * 2 + 1] );

			pDlgValidate->Step();
		}

		//AfxMessageBox( popa );

		SetCursor( LoadCursor( NULL, IDC_ARROW ) );

		SAFE_DELETE( pDlgValidate );
	}

	pCropTool->DestroyWindow();
	SAFE_DELETE( pCropTool );


	m_iCurrentFrame = 0;
	m_sliderAnim.SetPos(0);
	
	firstSelFrame	= 0;
	lastSelFrame	= 0;

	m_sliderAnim.ClearSel(true);
	m_sliderAnim.SetRange(0, ((CEfnCharacter*)pBaseScene)->GetFramesCount() - 1);
	m_sliderAnim.Invalidate(true);

	m_sliderAnim.SetPos( m_iStartingFrame );

	CString a;
	a.Format("%f", ((CEfnCharacter*)pBaseScene)->GetZOffset(0));
	m_eFrameOffset.SetWindowText(a);
	a.Format("%d", m_iStartingFrame);
	m_eFrameCounter.SetWindowText(a);

	UpdateStatusBarInfoEFN();

	//update tree nodes
	HTREEITEM hInfo        = m_rtSceneContents.FindTreeItemByName( "Info" );
	HTREEITEM hInfoChild   = m_rtSceneContents.GetChildItem( hInfo );
	HTREEITEM hFramesCount;

	a.Format( "Frames: %d", old_frames_count );
	hFramesCount = m_rtSceneContents.FindTreeItemByName( a, hInfo );
	a.Format( "Frames: %d", ((CEfnCharacter*)pBaseScene)->GetFramesCount() );
	m_rtSceneContents.SetItemText( hFramesCount, a );

	a.Format( "Triangles in model: %d", old_tri_count );
	hFramesCount = m_rtSceneContents.FindTreeItemByName( a, hInfo );
	a.Format( "Triangles in model: %d", ((CEfnCharacter*)pBaseScene)->GetTrianglesCount() );
	m_rtSceneContents.SetItemText( hFramesCount, a );

	a.Format( "Vertecies in model: %d", old_vert_count );
	hFramesCount = m_rtSceneContents.FindTreeItemByName( a, hInfo );
	a.Format( "Vertecies in model: %d", ((CEfnCharacter*)pBaseScene)->GetVerteciesCount() );
	m_rtSceneContents.SetItemText( hFramesCount, a );

	//rebuild d3d buffers
	( (CEfnCharacter*)pBaseScene )->Prepare( g_pd3dDevice ); 
	( (CEfnCharacter*)pBaseScene )->SetCurrNextFrame( 0, 0 );

}

void CCookieMachineDlg::OnMenuEfnAnimDegradeTool()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_EFN )
		return;

	CEfnCharacter* pEfnAnim = (CEfnCharacter*)pBaseScene;

	CAnimDegrageToolDlg* pDlg = new CAnimDegrageToolDlg(this);
	pDlg->DoModal();

	pEfnAnim->DegradeAnimation( 100.0f );

	SAFE_DELETE( pDlg );

	int frames_cout = pEfnAnim->GetFramesCount() - 1;

	/* cleanup */
	firstSelFrame		= -1;
    lastSelFrame		= -1;
	m_iStartingFrame	= 0;
	m_iEndingFrame		= frames_cout;

	m_eFrameCounter.SetWindowText( "0" );

	m_sliderAnim.SetRange( 0, frames_cout, 1 );
	m_sliderAnim.SetPos( 0 );
	m_sliderAnim.ClearSel( true );
	m_sliderAnim.SetSelection( 0, frames_cout );
	m_sliderAnim.Invalidate( true );

	CString temp;
	temp.Format( "%f", pEfnAnim->GetZOffset(0) );

	m_eFrameOffset.SetWindowText( temp );

	UpdateStatusBarInfoEFN();
}

void CCookieMachineDlg::OnMenuHackTranslateScene()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_SCN )
		return;

	D3DXVECTOR3 vec( -25.0f, 0.0f, 0.0f );

	((CSceneFile*)pBaseScene)->TranslateWholeSceneHack( &vec );

	((CSceneFile*)pBaseScene)->Prepare( g_pd3dDevice );
}

void CCookieMachineDlg::OnMenuScnLightDuplicate()
{
	if( !pBaseScene || pBaseScene->GetType() != FT_SCN )
		return;

	UINT			uiMask		= TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	CEfnMeshlist*	pMeshlist	= (CEfnMeshlist*)((CSceneFile*)pBaseScene)->GetMeshlist();
	int				counter		= 2;
	CSceneFile*		pScene		= (CSceneFile*)pBaseScene;
	CString			strTemp;

	CLIGHT* pLight = pScene->GetLightByID( m_rtSceneContents.GetCurrNodeText().GetBuffer() );
	
	if( !pLight )
		return;

	str_t light_name = pLight->name;

	strTemp.Format( "%s%s", light_name.data(), "_copy" );

	CLIGHT* pTempLight = pScene->GetLightByID( strTemp.GetBuffer() );

	while( pTempLight )
	{
		strTemp.Format( "%s%s_%i", light_name.data(), "_copy", counter );
		pTempLight = pScene->GetLightByID( strTemp.GetBuffer() );

		counter++;
	}

	if( strTemp.GetLength() > 15 )
		return;

    CLIGHT* pLightDupe = pScene->AddLight( strTemp.GetBuffer() );

	if( !pLight )
		return;

	HTREEITEM hNewLight = NULL;

	if( pLightDupe )
	{
		*pLightDupe			= *pLight;
		pLightDupe->visible = true;
		pLightDupe->name	= strTemp.GetBuffer();
		((CSceneFile*)pBaseScene)->Prepare( g_pd3dDevice );

		HTREEITEM hLights = m_rtSceneContents.FindTreeItemByName( "Lights" );
		hNewLight = m_rtSceneContents.InsertItem( uiMask, strTemp, 9, 9, 0, 0, 0, hLights, TVI_SORT ); 
	}

	m_rtSceneContents.SelectItem( hNewLight );
	pMeshlist->DeselectAllMeshes();
	pScene->SetSelectedLight( strTemp.GetBuffer() );
}

void CCookieMachineDlg::OnMenuScnLightsCreateOmni()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	CModelNameDlg*	dlgLightName= new CModelNameDlg(this);
	HTREEITEM		hLights		= m_rtSceneContents.FindTreeItemByName( "Lights" );
	CSceneFile*		pScene		= (CSceneFile*)pBaseScene;

	dlgLightName->SetCaption( "Имя источника света:" );
	dlgLightName->SetString( "" );

	if( dlgLightName->DoModal() == IDOK )
	{
		if( pScene->GetLightByID( dlgLightName->GetString().GetBuffer() ) )
		{
			MessageBox( "Источник света с таким ID уже существует!", "Внимание!", MB_OK | MB_ICONINFORMATION );
			SAFE_DELETE( dlgLightName );
			return;
		}
		else if( dlgLightName->GetString().GetLength() > 15 )
		{
			MessageBox( "Имя источника света не может быть длиннее 15ти знаков!", "Внимание!", MB_OK | MB_ICONINFORMATION );
			SAFE_DELETE( dlgLightName );
			return;
		}


		pScene->AddLight( dlgLightName->GetString().GetBuffer() );		

		CLIGHT*		pLight		= pScene->GetLightByID( dlgLightName->GetString().GetBuffer() );
		HTREEITEM	hLightItem	= m_rtSceneContents.InsertItem2( pLight->name.data(), 9, NULL, hLights, TVI_SORT );

		pLight->type = LT_POINT;

		m_rtSceneContents.SortChildren( hLights );
		m_rtSceneContents.SelectItem( hLightItem );
	}

	SAFE_DELETE( dlgLightName );
	pScene->Prepare( g_pd3dDevice );
}

void CCookieMachineDlg::OnMenuScnLightsCreateDirectional()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	CModelNameDlg*	dlgLightName= new CModelNameDlg(this);
	HTREEITEM		hLights		= m_rtSceneContents.FindTreeItemByName( "Lights" );
	CSceneFile*		pScene		= (CSceneFile*)pBaseScene;

	dlgLightName->SetCaption( "Имя источника света:" );
	dlgLightName->SetString( "" );

	if( dlgLightName->DoModal() == IDOK )
	{
		if( pScene->GetLightByID( dlgLightName->GetString().GetBuffer() ) )
		{
			MessageBox( "Источник света с таким ID уже существует!", "Внимание!", MB_OK | MB_ICONINFORMATION );
			SAFE_DELETE( dlgLightName );
			return;
		}
		else if( dlgLightName->GetString().GetLength() > 15 )
		{
			MessageBox( "Имя источника света не может быть длиннее 15ти знаков!", "Внимание!", MB_OK | MB_ICONINFORMATION );
			SAFE_DELETE( dlgLightName );
			return;
		}


		pScene->AddLight( dlgLightName->GetString().GetBuffer() );		

		CLIGHT*		pLight		= pScene->GetLightByID( dlgLightName->GetString().GetBuffer() );
		HTREEITEM	hLightItem	= m_rtSceneContents.InsertItem2( pLight->name.data(), 9, NULL, hLights, TVI_SORT );

		pLight->type = LT_DIRECTED;
		pLight->pos.y = 6;

		m_rtSceneContents.SortChildren( hLights );
		m_rtSceneContents.SelectItem( hLightItem );
	}

	SAFE_DELETE( dlgLightName );
	pScene->Prepare( g_pd3dDevice );
}

void CCookieMachineDlg::OnMenuEfnNullsDeleteAll()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN )
		return;

	TNODE*			node		= (TNODE*)m_rtSceneContents.GetCurrNodeData();
	CEfnCharacter*	pEfnAnim	= (CEfnCharacter*)node->_data;

	if( !pEfnAnim || !node || node->_type != TREENODE::N_NULLS )
		return;

	if( MessageBox( "Удалить все нули?", "Внимание!", MB_YESNO | MB_ICONQUESTION ) != IDYES )
		return;

	HTREEITEM hNulls	= m_rtSceneContents.FindTreeItemByName( "Nulls" );
	HTREEITEM hNull		= m_rtSceneContents.GetChildItem( hNulls );

	while( hNull )
	{
		m_rtSceneContents.DeleteItem( hNull );
		hNull = m_rtSceneContents.GetChildItem( hNulls );
	}

	pEfnAnim->DeleteAllNulls();
	pBaseScene->Prepare( g_pd3dDevice );
}

void CCookieMachineDlg::OnMenuEfnMeshDelete()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN )
		return;

	HTREEITEM hInfo = m_rtSceneContents.FindTreeItemByName( "Info" );
	CString temp;
	temp.Format( "Vertecies in model: %i", ((CEfnCharacter*)pBaseScene)->GetVerteciesCount() );
	HTREEITEM hVCount = m_rtSceneContents.FindTreeItemByName( temp );
	temp.Format( "Triangles in model: %i", ((CEfnCharacter*)pBaseScene)->GetTrianglesCount() );
	HTREEITEM hTCount = m_rtSceneContents.FindTreeItemByName( temp );

	((CEfnCharacter*)pBaseScene)->DeleteMeshData();
	pBaseScene->Prepare( g_pd3dDevice );

	m_rtSceneContents.SetItemText( hVCount, "Vertecies in model: 0" );
	m_rtSceneContents.SetItemText( hTCount, "Triangles in model: 0" );
}

void CCookieMachineDlg::OnMenuRotateModelY_by_180deg()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN )
		return;

	((CEfnCharacter*)pBaseScene)->RotateBy180DegAboutY();
	pBaseScene->Prepare( g_pd3dDevice );
}

void CCookieMachineDlg::OnMenuRotateModelX90()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN_MESHLIST )
		return;

	((CEfnMeshlist*)pBaseScene)->RotateXBy90();
	pBaseScene->Prepare( g_pd3dDevice );
}

void CCookieMachineDlg::OnMenuRotateModelY90()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN_MESHLIST )
		return;

	((CEfnMeshlist*)pBaseScene)->RotateYBy90();
	pBaseScene->Prepare( g_pd3dDevice );
}

void CCookieMachineDlg::OnMenuRotateModelZ90()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN_MESHLIST && pBaseScene->GetType() != FT_EFN )
		return;

	if( pBaseScene->GetType() == FT_EFN_MESHLIST )
		((CEfnMeshlist*)pBaseScene)->RotateZBy90();

	if( pBaseScene->GetType() == FT_EFN )
		((CEfnCharacter*)pBaseScene)->RotateZBy90();

	pBaseScene->Prepare( g_pd3dDevice );
}

void CCookieMachineDlg::OnMenuScnMeshesCreateBoxTess()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	D3DXVECTOR3 isect;

	if( GetClosestIntersectionFromMeshlist( &isect ) == false )
	{
		isect = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	}

	m_dlgTesselatedMesh->SetMode( TMDM_BOX );
	m_dlgTesselatedMesh->SetObjectPosition( &isect );
	m_dlgTesselatedMesh->SetScene( (CSceneFile*)pBaseScene );
	m_dlgTesselatedMesh->ShowWindow( SW_SHOW );
}

void CCookieMachineDlg::OnMenuScnMeshesCreateCylinderTess()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_SCN )
		return;

	D3DXVECTOR3 isect;

	if( GetClosestIntersectionFromMeshlist( &isect ) == false )
	{
		isect = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	}

	m_dlgTesselatedMesh->SetMode( TMDM_CYLINDER );
	m_dlgTesselatedMesh->SetObjectPosition( &isect );
	m_dlgTesselatedMesh->SetScene( (CSceneFile*)pBaseScene );
	m_dlgTesselatedMesh->ShowWindow( SW_SHOW );
}

void CCookieMachineDlg::OnMenuHidebytypeDynFloors()
{
	OnMenuScnShowMeshByType( false, "DynamicFloor" );
}

void CCookieMachineDlg::OnMenuHidebytypeShadowReceivers()
{
	OnMenuScnShowMeshByType( false, "ShadowReceiver" );
}

void CCookieMachineDlg::OnMenuShowbytypeDynFloors()
{
	OnMenuScnShowMeshByType( true, "DynamicFloor" );
}

void CCookieMachineDlg::OnMenuShowbytypeShadowReceivers()
{
	OnMenuScnShowMeshByType( true, "ShadowReceiver" );
}

void CCookieMachineDlg::OnMenuScaleZOffsets()
{
	if( !pBaseScene )
		return;

	if( pBaseScene->GetType() != FT_EFN )
		return;

	SetCursor( LoadCursor( NULL, IDC_WAIT ) );

	float scale_factor;

	CModelNameDlg* scaling_dlg = NULL;
	scaling_dlg = new CModelNameDlg(this);

	if( !scaling_dlg )
	{
		SetCursor( LoadCursor( NULL, IDC_ARROW ) );
		return;
	}

	scaling_dlg->SetCaption( "Z-offsets scaling" );
	scaling_dlg->SetString( "1.0" );

	if( scaling_dlg->DoModal() != IDOK )
	{
		SAFE_DELETE( scaling_dlg );
	}
	else
	{
		scale_factor = atof( scaling_dlg->GetString().GetBuffer() );
		((CEfnCharacter*)pBaseScene)->ScaleZOffsets( scale_factor );
	}

	SAFE_DELETE( scaling_dlg );
	SetCursor( LoadCursor( NULL, IDC_ARROW ) );
}
