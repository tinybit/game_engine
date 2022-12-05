#pragma once

#define D3D_DEBUG_INFO

#include "../EfnCharacter.h"
#include "../EfnMeshlist.h"
#include "../SceneFile.h"

#include "ValidateDlg.h"
#include "ScalingDlg.h"
#include "OptionsDlg.h"
#include "BindsDlg.h"
#include "AnimationDlg.h"
#include "ScriptingDlg.h"
#include "HelpDlg.h"
#include "ColorDlg.h"
#include "Import2ScnDlg.h"
#include "MaterialDlg.h"
#include "AnimScriptingDlg.h"
#include "PositionRotationScaleDlg.h"
#include "FileAssociationsDlg.h"
//#include "QuickBindDlg.h"
#include "PropCustomParamDlg.h"
#include "PropNullDlg.h"
#include "PropCameraDlg.h"
#include "PropLightDlg.h"
#include "AboutBoxDlg.h"
#include "ModelNameDlg.h"
#include "TextResourcesDlg.h"
#include "EngineMessagingDlg.h"
#include "BinkInfoDlg.h"
#include "CustomFileDialog.h"
#include "BatchConvertDlg.h"
#include "FramesCropToolDlg.h"
#include "AnimDegrageToolDlg.h"
#include "TesselatedMeshDlg.h"

#include "afxcmn.h"
#include "afxwin.h"

#include "../vector.h"
#include "../plane.h"
#include "../d3dtext.h"
#include "../utils/common.h"

//extra controls

#include "../controls/MyTree.h"
#include "../controls/SplitterControl.h"
#include "../ViewMgr.h"

#include "../sys/Resource.h"
#include "../MemMappingsMgr.h"
#include "../ExeptionLogger.h"

enum AXIS_ALIGN_ENUM
{
	AAE_X,
	AAE_Y,
	AAE_Z,
	AAE_XY,
	AAE_XZ,
	AAE_YZ
};

enum OBJPROP_WND_ENUM 
{
	OWE_UNDEFINED,
	OWE_CUSTPARAM,
	OWE_NULL,
	OWE_CAMERA,
	OWE_LIGHT
};

#define MIN_WND_WIDTH	660
#define MIN_WND_HEIGHT	550

class CCookieMachineDlg : public CDialog
{
	public:
		/* constructors */
		CCookieMachineDlg(CWnd* pParent = NULL);
		~CCookieMachineDlg();

		void				Render();
		
		CBaseScene*			GetAlternateNullIcon() { if( m_appSettings.ScnAlternateNullIco ) return pNullIco; return NULL; };
		float				GetIconsScale();
		void				SettingsApplyToApp( const AppSettingsData& appSettings );
		bool				ProcessTreeDrag( HTREEITEM dragItem, HTREEITEM dropItem );
		void				StartRendering()		{ SetTimer(RENDERING_TIMER, 0, (TIMERPROC) NULL); };
		void				SaveGlob()				{ OnMenuFileSave(); };
		void				RunEngineGlob()			{ RunGame(); };
		void				RunEngineCurrGlob()		{ RunCurrentGame(); };
		bool				FilterObjectsByPrefix()	{ return m_appSettings.FilterObjsByPrefix; };
		void				ProcessMessagesGlob()	{ ProcessMessages(); };
		CBaseScene*			Get3DIconGlob( enum Icon3DType IconType );
		ListStrVec*			GetBindsGlob();
		str_t				GetAppDir() { return m_strAppPath.GetBuffer(); };
		CTRANSFORM*			GetWorldTransformGlob() { return NULL; };           
		void				SetWorldTransformGlob( CTRANSFORM* pTrans ) { };     
		CBaseScene*			GetSceneGlob() { return pBaseScene;  };
		void				ShowTextResEditorGlob( str_t strInitialItem = "", int iErrLine = -1 );
		CWnd*				GetTextResEditorGlob() { return m_dlgTextResourcesDlg; };
		void				ShowScriptsEditorGlob();
		void				ShowFloorsMenuGlob();
		void				SceneTreeItemChangeProc();
		CViewMgr*			GetViewMgrGlob() { return &m_ViewMgr; };
		void				RenameCurrTreeviewItemGlob( str_t strNewID );
		void				SelectVerteciesByMouse();
		BOOL				ProcessDialogMessages();
		const AppSettingsData* GetAppSettingsGlob() { return &m_appSettings; };
		LPDIRECT3DDEVICE9	GetD3DDeviceGlob();
		CMyTree*			GetSceneExplorerGlob() { return &m_rtSceneContents; };
		
		enum { IDD = IDD_MAIN };
		CMyTree	m_rtSceneContents;
		
		//{{AFX_VIRTUAL(CCookieMachineDlg)
		public:
		virtual BOOL PreTranslateMessage(MSG* pMsg);
		protected:
		virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
		virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
		//}}AFX_VIRTUAL

	protected:
		/* message map functions */
		//{{AFX_MSG(CCookieMachineDlg)

		//sys handlers
		virtual BOOL OnInitDialog();
		afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
		afx_msg void OnPaint();
		afx_msg void OnTimer(UINT nIDEvent);
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
		afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
		afx_msg void OnDestroy();
		afx_msg void OnTvnSelchangedSceneTree(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
		afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
		afx_msg UINT OnGetDlgCode();

		/* MOUSE EVENT HANDLERS */

		//mouse event handlers
		afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
		afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
		afx_msg void OnMouseMove(UINT nFlags, CPoint point);

		//convertion routines
		afx_msg void OnMenuConvToEfn();
		afx_msg void OnMenuConvToScn();
		afx_msg void OnMenuConvToEfnMeshlist();

		//file operations
		afx_msg void OnMenuFileOpen();
		afx_msg void OnMenuFileClose();
		afx_msg void OnMenuFileSave();
		afx_msg void OnMenuFileSaveAs();

		//antialiasing
		afx_msg void OnMenuAANone();
		afx_msg void OnMenuAaX2();
		afx_msg void OnMenuAaX4();
		afx_msg void OnMenuAaX6();

		//macro operations
		afx_msg void OnMenuMacroRecStart();
		afx_msg void OnMenuMacroRecEnd();
		afx_msg void OnMenuMacroLoadFromFile();

		//efn operations
		afx_msg void OnMenuEfnScaleNew();
		afx_msg void OnMenuEfnNullScaling();
		afx_msg void OnMenuEfnLoadTexture();
		afx_msg void OnMenuEfnNullAttachTo();
		afx_msg void OnMenuEfnNullDetachAll();
		afx_msg void OnMenuScale();
		afx_msg void OnMenuEfnAnimationAdd();
		afx_msg void OnMenuEfnAnimationsDeleteAll();
		afx_msg void OnMenuEfnAnimationDelete();
		afx_msg void OnMenuEfnAnimationEdit();
		afx_msg void OnMenuEfnEditMaterial();
		afx_msg void OnMenuEfnNullsDetachAll();

		//meshlist operations
		afx_msg void OnMenuMeshlistNullRemove();
		afx_msg void OnMenuMeshlistNullAttach();
		afx_msg void OnMenuMeshlistNullDetach();
		afx_msg void OnMenuMeshlistNullsNew();
		afx_msg void OnMenuMeshlistNullsRemoveAll();
		afx_msg void OnMenuMeshlistNullsDetachAll();
		afx_msg void OnMenuMeshlistMeshEditVertices();
		afx_msg void OnMenuMeshlistNullShow();
		afx_msg void OnMenuMeshlistNullHide();
		afx_msg void OnMenuMeshlistMeshMaterial();
		afx_msg void OnMenuMeshlistMeshDuplicate();
		afx_msg void OnMenuMeshlistMeshesCreateCube();
		afx_msg void OnMenuMeshlistMeshesCreatePoly();
		afx_msg void OnMenuMeshlistMeshesCreateQuad();
		afx_msg void OnMenuMeshlistMeshesHide();
		afx_msg void OnMenuMeshlistMeshesShow();
		afx_msg void OnMenuMeshlistNullsHide();
		afx_msg void OnMenuMeshlistNullsShow();
		afx_msg void OnMenuMeshlistMeshShow();
		afx_msg void OnMenuMeshlistMeshDelete();

		//scene file operations
		afx_msg void OnMenuScnCameraScript();
		afx_msg void OnMenuScnLightScript();
		afx_msg void OnMenuScnTextureScript();
		afx_msg void OnMenuScnNullDelete();
		afx_msg void OnMenuScnMeshDuplicate();
		afx_msg void OnMenuScnMeshDelete();
		afx_msg void OnMenuScnMeshesCreateBox();
		afx_msg void OnMenuScnMeshesCreateTriangle();
		afx_msg void OnMenuScnMeshesCreateQuad();
		afx_msg void OnMenuScnTextureBind2Cam();
		afx_msg void OnMenuScnMeshBind2Camera();
		afx_msg void OnMenuScnMeshBind2Texture();
		afx_msg void OnMenuScnLightBind2Camera();
		afx_msg void OnMenuScnMeshShowOnly();
		afx_msg void OnMenuScnCameraShowOnly();
		afx_msg void OnMenuScnLightShowOnly();
		afx_msg void OnMenuScnMeshEditMaterial();
		afx_msg void OnMenuScnMeshEditVertecies();
		afx_msg void OnMenuScnNullsShow();
		afx_msg void OnMenuScnNullsHide();
		afx_msg void OnMenuScnNullAttach();
		afx_msg void OnMenuScnNullDetach();
		afx_msg void OnMenuScnNullsCreateNew();
		afx_msg void OnMenuScnNullsDetachAll();
		afx_msg void OnMenuScnCustObjAddnew();
		afx_msg void OnMenuScnCustObjDelall();
		afx_msg void OnMenuScnCustTypeAddnew();
		afx_msg void OnMenuScnCustTypeDelall();
		afx_msg void OnMenuScnCustTypeDel();
		afx_msg void OnMenuScnCustObjDel();
		afx_msg void OnMenuScnBindsShowWnd();
		afx_msg void OnMenuScnScriptsShowWnd();
		afx_msg void OnMenuScnFloorUVRecalc();
		afx_msg void OnMenuScnTextureAddBackground();
		afx_msg void OnMenuScnTextureDel();
		afx_msg void OnMenuScnMeshesShow();
		afx_msg void OnMenuScnMeshesHide();
		afx_msg void OnMenuScnMeshShow();
		afx_msg void OnMenuScnMeshHide();
		afx_msg void OnMenuScnCamerasShow();
		afx_msg void OnMenuScnCamerasHide();
		afx_msg void OnMenuScnCameraShow();
		afx_msg void OnMenuScnCameraHide();
		afx_msg void OnMenuScnCameraObjects();
		afx_msg void OnMenuScnLightsShow();
		afx_msg void OnMenuScnLightsHide();
		afx_msg void OnMenuScnLightShow();
		afx_msg void OnMenuScnLightHide();
		afx_msg void OnMenuScnTextureAdd();
		afx_msg void OnMenuScnTexturesDeleteAll();
		afx_msg void OnMenuScnFloorRecalc();
		afx_msg void OnMenuScnFloorShowOnly();
		afx_msg void OnMenuScnFloorShowGrid();
		afx_msg void OnMenuScnFloorShowSelector();
		afx_msg void OnMenuScnFloorSelectorShowOnly();
		afx_msg void OnMenuScnCamZoomnear();
		afx_msg void OnMenuScnCamZoomfar();
		afx_msg void OnMenuScnLightDiffuse();
		afx_msg void OnMenuScnLightAmb();
		afx_msg void OnMenuScnMeshVcol();
		afx_msg void OnMenuScnLightMultiplier();
		afx_msg void OnMenuScnLightAttenuation();
		afx_msg void OnMenuScnLightRange();

		//unsorted
		afx_msg void OnMenuExit();
		afx_msg void OnMenuShowMesh();
		afx_msg void OnMenuHideMesh();
		afx_msg void OnMenuAbout();
		afx_msg void OnMenuShowAllMeshes();
		afx_msg void OnMenuHideMeshes();
		afx_msg void OnMenuBackfaceCulling();
		afx_msg void OnTWireframe();
		afx_msg void OnTShaded();
		afx_msg void OnBnClickedBtnAnimPlay();
		afx_msg void OnBnClickedBtnAnimStop();
		afx_msg void OnBnClickedChkLoopAnim();
		afx_msg void OnCreateSequence();
		afx_msg void OnMenuIldarWindow();
		afx_msg void OnBnClickedButnSetZOff();
		afx_msg void OnMenuDiscardVerteciesColor();
		afx_msg void OnBnClickedChkMoveChar();
		afx_msg void OnMenuEfnAnimationRun();
		afx_msg void OnMenuTestWalk();
		afx_msg void OnMenuConvertCoordsSystem();
		afx_msg void OnTWireframeShaded();
		afx_msg void OnMenuSwapyz();
		afx_msg void OnMenuOptions();
		afx_msg void OnMenuMeshInverseNormals();
		afx_msg void OnMenuSetCameraTocurrent();
		afx_msg void OnMenuReturnToPerspectiveview();
		afx_msg void OnMenuShowAabb();
		afx_msg void OnMenuShowBsphere();
		afx_msg void OnMenuRecalcGlobalRadius();
		afx_msg void OnMenuShowAllCameras();
		afx_msg void OnMenuHideAllCameras();
		afx_msg void OnMenuShowAllLights();
		afx_msg void OnMenuHideAllLights();
		afx_msg void OnCbnSelchangeCmbCameras();
		afx_msg void OnMenuObjectShowAll();
		afx_msg void OnMenuObjectHide();
		afx_msg void OnMenuFpsChange();
		afx_msg void OnMenuMeshlistScenetag();
		afx_msg void OnMenuSceneCamsObjectDelete();
		afx_msg void OnMenuSceneCamsObjectScript();
		afx_msg void OnMenuSceneCamsObjectBind();
		afx_msg void OnBnClickedBtnShowall();
		afx_msg void OnBnClickedBtnHideall();
		afx_msg void OnMenuImport();
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnBnClickedBtnFilesaveAll();
		afx_msg void OnMenuObjectShowOnly();
		afx_msg void OnMenuFileAssociations();
		afx_msg void RunGame();
		afx_msg void RunCurrentGame();
		afx_msg void OnMenuEfnNullsRescaleAll();
		afx_msg void OnMenuShowObjPropertiesWnd();
		afx_msg void OnMenuAxisX();
		afx_msg void OnMenuAxisY();
		afx_msg void OnMenuAxisZ();
		afx_msg void OnMenuAxisXY();
		afx_msg void OnMenuAxisXZ();
		afx_msg void OnMenuAxisYZ();
		afx_msg void OnMenuXSIMeshDisconnectAllTris();
		afx_msg void OnMenuXSIMeshConnectAllTris();
		afx_msg void OnMenuEfnMeshDisconnectAllTris();
		afx_msg void OnMenuEfnMeshConnectAllTris();
		afx_msg void OnCbnSelchangeCmbViews();
		afx_msg void OnMenuScnFloorMaterial();
		afx_msg void OnMenuScnMeshInverseNormals();
		afx_msg void OnMenuScnMeshConnectTris();
		afx_msg void OnMenuScnMeshDisconnectTris();
		afx_msg void OnMenuMeshlistMeshConnectTris();
		afx_msg void OnMenuMeshlistMeshDisconnectTris();
		afx_msg void OnMenuMeshlistMeshInverseNormals();
		afx_msg void OnMenuMeshlistMeshesDeleteAll();
		afx_msg void OnMenuMeshlistDeleteSelectedObjects();
		afx_msg void OnMenuScnDeleteSelectedObjects();
		afx_msg void SelectedObjectsMerge();
		afx_msg void SelectedObjectsMaterial();
		afx_msg void OnNMClickSceneTree(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void SelectedObjectsConnectTris();
		afx_msg void SelectedObjectsDisconnectTris();
		afx_msg void SelectedObjectsFlipNormals();
		afx_msg void OnMenuScnCameraMiscObjectScript();
		afx_msg void OnMenuScnCameraMiscObjectBind();
		afx_msg void OnMenuScnCurvesCreateNew();
		afx_msg void OnMenuScnCurveAddselectednull();
		afx_msg void OnMenuScnCurvesShow();
		afx_msg void OnMenuScnCurvesHide();
		afx_msg void OnMenuScnCurvesDeleteAll();
		afx_msg void OnMenuScnCurveRemoveAllNulls();
		afx_msg void OnMenuScnCurveDelete();
		afx_msg void OnMenuScnCurveSetTypeLinear();
		afx_msg void OnMenuScnCurveSetTypeCatmul();
		afx_msg void OnMenuScnCurveNullRemove();
		afx_msg void OnMenuScnCurveNullMoveUP();
		afx_msg void OnMenuScnCurveNullMoveDOWN();
		afx_msg void OnMenuEngineMessaging();
		afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
		afx_msg void SelectedObjectsInverseSelection();
		afx_msg void SelectedObjectsHide();
		afx_msg void OnMenuSelectVisibleObjects();
		afx_msg void OnMenuRename();
		afx_msg void OnMenuDuplicate();
		afx_msg void OnMenuEfnNullsReloadAttaches();
		afx_msg void OnStnClickedBmp();
		afx_msg void OnMenuScnLightsDeleteAll();
		afx_msg void OnMenuScnCamerasDeleteAll();
		afx_msg void OnMenuScnMeshesDeleteAll();
		afx_msg void OnMenuScnNullsDeleteAll();
		afx_msg void OnMenuScnLightDelete();
		afx_msg void OnMenuScnCameraDelete();
		afx_msg void OnMenuViewSpotlightcone();
		afx_msg void OnMenuScnRestoreEngineCams();
		afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
		afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
		afx_msg void OnMenuViewCameraInterest();
		afx_msg void OnMenuCreateScene();
		afx_msg void OnMenuHidebytypeCameras();
		afx_msg void OnMenuShowbytypeCameras();
		afx_msg void OnMenuHidebytypeLights();
		afx_msg void OnMenuShowbytypeLights();
		afx_msg void OnMenuHidebytypeMeshes();
		afx_msg void OnMenuShowbytypeMeshes();
		afx_msg void OnMenuShowbytypeNulls();
		afx_msg void OnMenuHidebytypeNulls();
		afx_msg void OnMenuHidebytypeCurves();
		afx_msg void OnMenuShowbytypeCurves();
		afx_msg void OnMenuHidebytypeAll();
		afx_msg void OnMenuShowbytypeAll();
		afx_msg void OnMenuHidebytypeFloors();
		afx_msg void OnMenuShowbytypeFloors();
		afx_msg void OnMenuHidebytypeLayergeom();
		afx_msg void OnMenuShowbytypeLayergeom();
		afx_msg void OnMenuShowbytypeCameraselectors();
		afx_msg void OnMenuHidebytypeCameraselectors();
		afx_msg void OnMenuHidebytypeActionobjects();
		afx_msg void OnMenuShowbytypeActionobjects();
		afx_msg void OnMenuBatchConvertXSI2EFNAnim();
		afx_msg void OnMenuCreateobjectNull();
		afx_msg void OnMenuCreateobjectBox();
		afx_msg void OnMenuCreateobjectQuad();
		afx_msg void OnMenuCreateobjectTriangle();
		afx_msg void OnMenuScnCustObjBind2Camera();
		afx_msg void OnMenuSelGeomAlignPoints();
		afx_msg void OnMenuEfnUnloadTexture();
		afx_msg void OnMenuCreateobjectNullOnCurve();
		afx_msg void OnMenuEfnUseUV1();
		afx_msg void OnMenuEfnUseUV2();
		afx_msg void OnMenuHidebytypeOccluders();
		afx_msg void OnMenuShowbytypeOccluders();
		afx_msg void OnMenuEfnNullDelete();
		afx_msg void OnMenuEfnNullsCreateNew();
		afx_msg void OnNMDblclkSceneTree(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void OnMenuScnCameraMiscObjectEditPos();
		afx_msg void OnMenuScnFinishObjectPosEditing();
		afx_msg void OnMenuScnEditDudePos();
		afx_msg void OnMenuCreateMeshlist();
		afx_msg void OnMenuSelGeomBind();
		afx_msg void OnMenuViewNormals();
		afx_msg void OnTempAddlod();
		afx_msg void OnMenuBatchConvertXSI2EFNMeshlist();
		afx_msg void OnMenuBatchConvertErrorCorrection();
		afx_msg void OnMenuSelGeomMergePoints();
		afx_msg void OnMenuScnCameraMiscObjectDelete();
		afx_msg void OnMenuEfnCropTool();
		afx_msg void OnMenuEfnAnimDegradeTool();
		afx_msg void OnMenuHackTranslateScene();
		afx_msg void OnMenuScnLightDuplicate();
		afx_msg void OnMenuScnLightsCreateOmni();
		afx_msg void OnMenuScnLightsCreateDirectional();
		afx_msg void OnMenuEfnNullsDeleteAll();
		afx_msg void OnMenuEfnMeshDelete();
		afx_msg void OnMenuRotateModelY_by_180deg();
		afx_msg void OnMenuRotateModelX90();
		afx_msg void OnMenuRotateModelY90();
		afx_msg void OnMenuRotateModelZ90();
		afx_msg void OnMenuScnMeshesCreateBoxTess();
		afx_msg void OnMenuScnMeshesCreateCylinderTess();
		afx_msg void OnMenuHidebytypeDynFloors();
		afx_msg void OnMenuHidebytypeShadowReceivers();
		afx_msg void OnMenuShowbytypeDynFloors();
		afx_msg void OnMenuShowbytypeShadowReceivers();
		afx_msg void OnMenuScaleZOffsets();

		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	private:
		virtual BOOL	OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult );
		virtual void	PreInitDialog();
		
		/* utility functions */
		void				D3DInit( HWND hWnd );
		void				EnableControls( bool value );
		void				CloseCurrentFile();
		void				RenderEFNCharacterEd( bool bIsWireframe );
		void				D3DCleanup();
		void				EnableWalkMode( bool bEnabled );
		bool				GridCreate();
		CVector				GetGridIntersection( int x, int y );
		void				HandlePossibleSizeChange();
		
		bool				Initialize();
		void				InitializeControls();
		void				InitializeGlobalVars();

		void				ProcessMessages(); 
		void				SettingsApplyToApp();
		void				SettingsApplyLight();
		void				SettingsReadFromFile();
		void				SettingsWriteToFile();
		void				SetupMatrices();
		void				SwitchCulling( bool value );
		void				SwitchRenderMode( RENDER_MODE mode );	
		void				SwitchLightingMode( bool lights_on );
		void				UpdateUI();
		void				UpdateUI_XSI();
		void				UpdateUI_EFN();
		void				UpdateUI_SCN();
		void				UpdateUI_SCNBinds();
		void				UpdateUI_EFN_MESHLIST();
		void				UpdateUISizing();     
		
		void				RenderCoordAxis();
		void				RenderVertexSelectionFrame();
		void				RenderEfn();
		void				RenderXSI();
		void				RenderSCN();
		void				RenderMeshlist();
		int*				IntersectCellWithMesh( CEfnCharacter* mesh, int x, int y, float grid_min_size_x, float grid_min_size_z, int& tri_count, float fCellSize );
		void				SelectAllAnimFrames();
		void				DeselectAllAnimFrames();
		void				DeleteSelectedAnimFrames();
		void				UpdateRecentFilesMenu();
		void				UpdateRecentDirsMenu();
		void				AddPathToRecentFilesList( str_t strPath );
		void				AddPathToRecentDirsList( str_t strPath );
		void				SetVColorFromMaterial( bool	bValue );
		void				LockCamOnObject();
		void				LockCamOnObjectAllViews();
		void				OnMenuRecentItem1() { OpenRecentFile( 0 ); };
		void				OnMenuRecentItem2() { OpenRecentFile( 1 ); };
		void				OnMenuRecentItem3() { OpenRecentFile( 2 ); };
		void				OnMenuRecentItem4() { OpenRecentFile( 3 ); };
		void				OnMenuRecentItem5() { OpenRecentFile( 4 ); };
		void				OnMenuRecentItem6() { OpenRecentFile( 5 ); };
		void				OnMenuRecentItem7() { OpenRecentFile( 6 ); };
		void				OnMenuRecentItem8() { OpenRecentFile( 7 ); };
		void				OnMenuRecentItem9() { OpenRecentFile( 8 ); };

		void				OnMenuRecentDir1() { OpenRecentDir( 0 ); };
		void				OnMenuRecentDir2() { OpenRecentDir( 1 ); };
		void				OnMenuRecentDir3() { OpenRecentDir( 2 ); };
		void				OnMenuRecentDir4() { OpenRecentDir( 3 ); };
		void				OnMenuRecentDir5() { OpenRecentDir( 4 ); };
		void				OnMenuRecentDir6() { OpenRecentDir( 5 ); };
		void				OnMenuRecentDir7() { OpenRecentDir( 6 ); };
		void				OnMenuRecentDir8() { OpenRecentDir( 7 ); };
		void				OnMenuRecentDir9() { OpenRecentDir( 8 ); };
		
		void				UpdateContextMenus();
		void				OnMenuShowGrid();
		void				StopGame();

		void                OpenFileFromDir( str_t strDir );

		void				OpenFile( str_t sPath, str_t sFile, str_t sExt );
		void				OpenRecentFile( int iRecentFileIndex );
		void				OpenRecentDir( int iRecentDirIndex );
		bool				CheckForDuplicateMeshes( CBaseScene* scene, str_t& strErrorMessage );
		void				DoResize(int delta);
		void				ShowScriptsWindow();
		void				ShowBindsWindow();
		void				ShowTextResourcesWindow( str_t strInitialItem = "", int iErrLine = -1 );
		void				ShowTextResourcesWindowWrp() { ShowTextResourcesWindow(); }; 
		void				ShowHelpWindow();
		void				ShowLogsWindow();
		void				Load3DIcons();
		void				CreateTreeViewImages();
		void				CreateStatusBar();
		void				OnLostDevice();
		void				OnResetDevice();
		void				StartEditingMeshVertices();
		void				FinishEditingMeshVertices();
		void				SwitchVerteciesEditingMode();
		void				ImportToScn();
		void				ImportToMeshlist();
		void				CancelAction();
		void				CancelEditingMeshVertices();
		void				OnMenuObjectMaterial();
		void				ShowFloorOnly();
		void				OnMenuEfnNullAttachTo( str_t strNullID, str_t strPath );
		void				OnMenuEfnNullAttachTo( str_t strNullID, str_t strDir, str_t strFile );
		void				OnMenuEfnSetNullsTexture( str_t strNullID, str_t strTexturePath );
		void				ResizeRenderWnd();
		void				CreateToolbar();
		void				EnableToolbarButton( UINT id, bool bValue );
		void				ShowAxisAlignMenu();
		void				ShowFloorsMenu();
		void				SetToolbarButtonImg( UINT id, int iImage );
		void				ShowObjectPropertiesWnd( int iShowMode );
		void				UpdateRenderView();
		void				SetViewPerspective();
		void				SetViewFront();
		void				SetViewTop();
		void				SetViewRight();
		void				SelectedObjectsDelete();
		void				SelectAllObjects();
		void				DeselectAllObjects();
		void				ObjectRename();
		void				HandleMeshSelection( str_t strObjID );
		void				SceneContextMenuProcXSI( CPoint point );			
		void				SceneContextMenuProcEFN( CPoint point );
		void				SceneContextMenuProcMSH( CPoint point );
		void				SceneContextMenuProcSCN( CPoint point );
		void				SceneTreeItemChangeProcXSI();
		void				SceneTreeItemChangeProcEFN();
		void				SceneTreeItemChangeProcMSH();
		void				SceneTreeItemChangeProcSCN();
		void				ScnMouseInteraction( UINT nMouseFlags, float& fXOffset, float& fYOffset );
		void				MeshlistMouseInteraction( UINT nMouseFlags, float& fXOffset, float& fYOffset );
		void				LightMouseInteraction( CLIGHT* pLight, UINT nMouseFlags, float& fXOffset, float& fYOffset );
		void				NullMouseInteraction( CNULL* pNull, UINT nMouseFlags, float& fXOffset, float& fYOffset );
		void				MeshMouseInteraction( CMesh* pMesh, UINT nMouseFlags, float& fXOffset, float& fYOffset );
		void				SelectNullsByMouse();
		void				OnMenuMeshlistNullsNewByPos( D3DXVECTOR3* pos = NULL );
		bool				OnMenuSceneNullsNewByPos( D3DXVECTOR3* pos = NULL, str_t* insertedNullID = NULL );
		void				OnMenuObjectCreateMesh( enum GeomPrimitiveTypes type, D3DXVECTOR3* pos = NULL );
		bool				GetClosestIntersectionFromMeshlist( D3DXVECTOR3* isect );
		void				OnMenuScnEditLevelName( str_t strNewID );
		void				UpdateMeshesInfoInExplorer();

		void				UpdateStatusBarInfoEFN( int iStartingFrame = -1, int iEndingFrame = -1 );
		void				UpdateStatusBarInfoMESHLIST();
		void				UpdateStatusBarInfoSCN();
		void				ResetStatusBarInfo();
		CBaseScene*			FindExistingEfn( CBaseScene* pRoot, str_t strFile, str_t strDir );
		

		//renaming efn animation
		void				OnMenuEfnNullRename( CString strCurrID, CString strNewID );
		void				OnMenuEfnEditModelName( CString	strNewID );
		void				OnMenuEfnAnimationRename( CString strCurrID, CString strNewID );
		void				OnMenuEfnEditCustParamName( CString strCurrID, CString strNewID );
		void				OnMenuEfnEditFPS( CString strCurrID, CString strNewID );
		void				OnMenuEfnEditGlobRadius( CString strCurrID, CString strNewID );
	
		//renaming efn meshlist
		void				OnMenuMeshlistNullRename( CString strCurrID, CString strNewID );
		void				OnMenuMeshlistMeshRename( CString strCurrID, CString strNewID );

		//renaming scn objects
		void				OnMenuScnCameraRename( CString strCurrID, CString strNewID );
		void				OnMenuScnLightRename( CString strCurrID, CString strNewID );
		void				OnMenuScnMeshRename( CString strCurrID, CString strNewID );
		void				OnMenuScnNullRename( CString strCurrID, CString strNewID );
		void				OnMenuScnTextureRename( CString strCurrID, CString strNewID );
		void				OnMenuScnCurveRename( CString strCurrID, CString strNewID );
		void				OnMenuScnCustObjRename( CString strCurrID, CString strNewID );
		void				OnMenuScnCustTypeRename( CString strCurrID, CString strNewID );
		void				OnMenuScnShowMeshByType( bool bShow, str_t strType );
		void				InsertEfnAttachNodes( HTREEITEM hRoot, CBaseScene* pAttach );
		void				UpdateRendererMousePos( CPoint* pt );

		str_t				GetTooltip( int ctrlID );
		void				ScnReloadScene();
		void				ScnReloadMeshes();
		BOOL				ToolbarTooltipProc( UINT id, NMHDR* pTTTStruct, LRESULT* pResult );
		void				OnMenuScnQuickBind( str_t BindType );
		void				OnMenuScnQuickBind2( StrVec* Objs, str_t BindType );
		void				ShowSceneExplorer( bool bValue );
		void				ToggleSceneExplorer();

		/* controls and dialogs */
		CMenu				m_menuViewPopup;
		HICON				m_hIcon;
		HACCEL				m_hAccelerator;
		CToolBarCtrl*		m_pToolbar;
		CImageList*			m_pToolbarImageList;
		CImageList*			m_pToolbarImageListHot;
		CImageList*			m_pToolbarImageListDis;
		CImageList*			m_pImageList;                  
		CStatusBar*			m_StatusBar;
		CSliderCtrl			m_sliderAnim;
		CButton				m_btnStopAnim;
		CButton				m_btnPlayAnim;
		CButton				m_btnLoopAnim;
		CButton				m_btnTransitionAnim;
		CEdit				m_eFrameCounter;
		CEdit				m_eFrameOffset;
		CButton				m_btnSetOffset;
		CAboutBoxDlg*		m_dlgAbout;
		COptionsDlg*		m_dlgOptions;
		CButton				m_btnMoveAnim;
		HWND				m_hRenderWnd;
		CComboBox			m_cmbCameras;
		CSplitterControl	m_wndSplitter1;
		CScriptingDlg*		m_dlgScriptingDlg;
		CTextResourcesDlg*	m_dlgTextResourcesDlg;
		CBindsDlg*			m_dlgBindingDlg;
		CBrush				m_brush;
		IStrMap				m_ToolbarTooltips;

		CBaseDialog*			m_pObjPropWnd;
		CPropCustomParamDlg*	m_pPropCustomParamDlg;
		CPropNullDlg*			m_pPropNullDlg;
		CPropCameraDlg*			m_pPropCameraDlg;
		CPropLightDlg*			m_pPropLightDlg;
		CTesselatedMeshDlg*		m_dlgTesselatedMesh;


		/* data */
		CBaseScene*			pBaseScene;
		CBaseScene*			pLightBulb;
		CBaseScene*			pLightCone;
		CBaseScene*			pCamBox;
		CBaseScene*			pNullIco;
		D3DLIGHT9			light;

		LPD3DXFONT          m_pFont;		// Font for drawing text
		LPD3DXSPRITE		m_pTextSprite;	// Sprite for batching draw text calls
		CD3Dtext*			m_txt;

		bool				m_bOnInitDialogFinished;
		int					m_iRenderingTimer;
		RECT				m_rcWindowBounds;		// Saved window bounds for mode switches
		CRect				m_rcWindowClient;		// Saved client area size for mode switches
		CPoint				m_point;
		bool				m_bIsMouseEditingOpStarted;


		bool				m_bIsPlaying;
		bool				m_bIsPlayingFinished;
		bool				m_bIsSceneExplorerVisible;


		FILE_TYPE			lastFileType;
		bool				m_bIsAnimationLooped;
		bool				m_bIsAnimationMoving;
		int					m_iCurrentFrame;
		int					m_iStartingFrame;
		int					m_iEndingFrame;	


		bool				m_bScrollSelectionStarted;
		bool				m_bDoubleClicked;
		bool				m_bIsLMouseButtonDown;
		bool				m_bIsGRadEditorModeOn;

		bool				m_bIsAppPaused;
		bool				m_bIsAppMinimized;
		bool				m_bIsVertexEditingModeOn;
		bool				m_bIsObjPropertiesWndVisible;
		bool				m_bIsCameraInterestVisible;
		CString				m_strLastFileOpenDirectory;

		CNULL*				pSelectedEfnAnimNull;

		float				m_fGlobRadScale;
		float				m_fAspect;

		bool				m_bSCNLoaded;

		bool				m_bIsPerspectiveCameraOn;
		bool				m_bResetCameraOnFileOpen;
		
		D3DXMATRIXA16		m_matGrid;
		D3DXMATRIXA16		m_matViewport;
		D3DXMATRIXA16		m_matView;
		D3DXMATRIX			matProj;

		D3DMATERIAL9		m_Gmtrl;
		D3DMATERIAL9		m_Wmtrl;
		D3DMATERIAL9		m_Bmtrl;

		D3DMATERIAL9		m_WhiteMtrl, m_BlackMtrl, m_RedMtrl, m_GreenMtrl, m_BlueMtrl;

		ModelStates			modelStates;
		AppSettingsData		m_appSettings;

		CPoint				m_ptTopLeft;
		CPoint				m_ptBottomRight;
		VERTEX2D_RHW		Vertices[8];

		//macros vars
		bool				m_bIsMacroRecStarted;
		str_t				m_strMacrosPath;
		vector<int>			m_vecMacrosDeletedFrames;
		StrMap				m_mapAttaches;
		StrMap				m_mapAttachesTex;

		HMODULE				m_hLexDll;

		// HACK! //
		CString				m_strAppPath;
		CString				m_strIniFilename;
		CString				m_strCmdLine;

		enum AXIS_ALIGN_ENUM	m_AxisAlign;
		enum OBJPROP_WND_ENUM	m_ObjPropWnd;

		CViewMgr			m_ViewMgr;	

		CMemMappingsMgr		MemMappingsMgr;

		int					m_iSplitterX;

		QuickBind			m_PrevQuickBind;
		bool				m_bNormalsRendered;
		bool				m_bClosing;
};