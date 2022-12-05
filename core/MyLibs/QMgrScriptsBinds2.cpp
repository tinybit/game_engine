#define STRICT

#include "QMgrScripts.h"

#include "QEngine.h"

#include "QSubSysLog.h"

#include "QObjBase.h"
#include "QObjMan.h"
#include "QObjCar.h"
#include "QObjBird.h"
#include "QObjFree.h"
#include "QObjCamera.h"
#include "QObjLight.h"
#include "QObjShadowVolume.h"
#include "QObjLayerGeom.h"
#include "QObjAction.h"
#include "QObjEfnAnim.h"
#include "QObjVideoAnim.h"
#include "QObj3DVideo.h"
#include "QObjMenuRukzak.h"
#include "QObjMenuCursor.h"
#include "QObjMenuMenu.h"


bool QMgrScripts::RegisterVariables2()
{
	using namespace luabind;

	module(luaVM)
	[
		class_<QMgrResObjects>("QMgrResObjects")
			.def("AddObjMan",			&QMgrResObjects::AddObjMan)
			.def("AddObjCar",			&QMgrResObjects::AddObjCar)
			.def("AddObjBird",			&QMgrResObjects::AddObjBird)
			.def("AddObjFree",			&QMgrResObjects::AddObjFree)
			.def("AddObjFreeE",			&QMgrResObjects::AddObjFreeE)
			.def("AddObjLight",			&QMgrResObjects::AddObjLight)
			.def("AddObjCam",			&QMgrResObjects::AddObjCam)
			.def("AddObjSV",			&QMgrResObjects::AddObjSV)
			.def("AddObjLG",			&QMgrResObjects::AddObjLG)
			.def("AddObjAction",		&QMgrResObjects::AddObjAction)
			.def("AddObjEfnAnim",		&QMgrResObjects::AddObjEfnAnim)
			.def("AddObjEfnAnimE",		&QMgrResObjects::AddObjEfnAnimE)
			.def("AddObjVideoAnim",		&QMgrResObjects::AddObjVideoAnim)
			.def("AddObj3DVideo",		&QMgrResObjects::AddObj3DVideo)
			.def("AddObjMenuRukzak",	&QMgrResObjects::AddObjMenuRukzak)
			.def("AddObjMenuCursor",	&QMgrResObjects::AddObjMenuCursor)
			.def("AddObjMenuMenu",		&QMgrResObjects::AddObjMenuMenu)
			.def("DelObj",				&QMgrResObjects::DelObj)
			.def("DelAll",				&QMgrResObjects::DelAll)
			.def("GetIntersectedObjID",	&QMgrResObjects::GetIntersectedObjID)
			.def("CheckForRayIntersect",&QMgrResObjects::CheckForRayIntersect)
			.def("GetPosFromIntersected",&QMgrResObjects::GetPosFromIntersected)
			.def("GetNullFromIntersected",&QMgrResObjects::GetNullFromIntersected)
			.def("GetDistFromIntersected",&QMgrResObjects::GetDistFromIntersected)
			.def("GetFirstFromGroup",	&QMgrResObjects::GetFirstFromGroup)
			.def("GetObj",				&QMgrResObjects::GetObj)
			.def("GetObjMan",			&QMgrResObjects::GetObjMan)
			.def("GetObjCar",			&QMgrResObjects::GetObjCar)
			.def("GetObjBird",			&QMgrResObjects::GetObjBird)
			.def("GetObjFree",			&QMgrResObjects::GetObjFree)
			.def("GetObjLight",			&QMgrResObjects::GetObjLight)
			.def("GetObjCam",			&QMgrResObjects::GetObjCam)
			.def("GetObjSV",			&QMgrResObjects::GetObjSV)
			.def("GetObjLG",			&QMgrResObjects::GetObjLG)
			.def("GetObjAction",		&QMgrResObjects::GetObjAction)
			.def("GetObjEfnAnim",		&QMgrResObjects::GetObjEfnAnim)
			.def("GetObjVideoAnim",		&QMgrResObjects::GetObjVideoAnim)
			.def("GetObj3DVideo",		&QMgrResObjects::GetObj3DVideo)
			.def("GetObjMenuRukzak",	&QMgrResObjects::GetObjMenuRukzak)
			.def("GetObjMenuCursor",	&QMgrResObjects::GetObjMenuCursor)
			.def("GetObjMenuMenu",		&QMgrResObjects::GetObjMenuMenu)
			.def("GetScript",			&QMgrResObjects::GetScript)
			.def("GetEnabled",			&QMgrResObjects::GetEnabled)
			.def("SetEnabled",			&QMgrResObjects::SetEnabled)

			.def("LogEnabled",			&QMgrResObjects::LogEnabled),

		//---------------------------------------------------------------------

		class_<QGameBase>("QGameBase")
			.def("Init",				&QGameBase::Init)
			.def("GetGameTime",			&QGameBase::GetGameTime)
			.def("GetGameElapsedTime",	&QGameBase::GetGameElapsedTime)
			.def("GetObjMgr",			&QGameBase::GetObjMgr)
			.def("SetCam",				&QGameBase::SetCam)
			.def("CamStr",				&QGameBase::CamStr)
			
			.def("GetMouseRay",			&QGameBase::GetMouseRay)
			.def("GetLeftClickRay",		&QGameBase::GetLeftClickRay)
			.def("GetRightClickRay",	&QGameBase::GetRightClickRay)
			
			.def("GetOrthoMouseRay",	&QGameBase::GetOrthoMouseRay)
			.def("GetOrthoLeftClickRay",&QGameBase::GetOrthoLeftClickRay)
			.def("GetOrthoRightClickRay",&QGameBase::GetOrthoRightClickRay),

		class_<QGameQuest, QGameBase >("QGameQuest")
			.def("LoadScene",			&QGameQuest::LoadScene)
			.def("GetScene",			&QGameQuest::GetScene)
			.def("GetClosestNull",		&QGameQuest::GetClosestNull)

			.def("SetCamTarget",		&QGameQuest::SetCamTarget)
			.def("GetCamTarget",		&QGameQuest::GetCamTarget)
			.def("SetIntervalForSetCam",&QGameQuest::SetIntervalForSetCam)
			
			.def("Cam3DStr",			&QGameQuest::Cam3DStr)
			.def("CamFloorsStr",		&QGameQuest::CamFloorsStr)
			.def("CamObjActionsStr",	&QGameQuest::CamObjActionsStr),

		class_<QGameVideo, QGameBase >("QGameVideo")
			.def("SetPrm",				&QGameVideo::SetPrm),

		class_<QGameConsole, QGameBase >("QGameConsole"),

		class_<QGameMenu, QGameBase >("QGameMenu"),

		class_<QGameStop, QGameBase >("QGameStop"),

		//-------------------------------------------------------------------------
		class_<QGameQuestSceneData>("QGameQuestSceneData")
			.def("ManFindPath",			&QGameQuestSceneData::ManFindPath)
			.def("CarFindPath",			&QGameQuestSceneData::CarFindPath)
			.def("BirdFindPath",		&QGameQuestSceneData::BirdFindPath)
			.def("LoadScene",			&QGameQuestSceneData::LoadScene)
			.def("FreeScene",			&QGameQuestSceneData::FreeScene)
			.def("GetNull",				&QGameQuestSceneData::GetNull)
			.def("GetCellSize",			&QGameQuestSceneData::GetCellSize)
			.def("GetCamForPos",		&QGameQuestSceneData::GetCamForPos)
			.def("isSceneLoaded",		&QGameQuestSceneData::isSceneLoaded),

		//-------------------------------------------------------------------------

		class_<QObjBase>("QObjBase")
			.def("GetNullPos",			&QObjBase::GetNullPos)
			.def("SetNullPos",			&QObjBase::SetNullPos)
			.def("isinNull",			&QObjBase::isinNull)
    		.def("GetEfnAnim",			&QObjBase::GetEfnAnim)
			.def("SetMtrlAlpha",		&QObjBase::SetMtrlAlpha)
			.def("SetMtrlColor",		&QObjBase::SetMtrlColor)
			.def("SetMtrlSpecular",		&QObjBase::SetMtrlSpecular)
			.def("SetMtrlEmissive",		&QObjBase::SetMtrlEmissive)

			.def("IsAnimStarted",		&QObjBase::IsAnimStarted)

			.def("GetEnabled",			&QObjBase::GetEnabled)
			.def("SetEnabled",			&QObjBase::SetEnabled)
			
			.def("GetVisible",			&QObjBase::GetVisible)
			.def("SetVisible",			&QObjBase::SetVisible)

			.def("GetEfnID",			&QObjBase::GetEfnID)
			.def("SetEfnID",			&QObjBase::SetEfnID)
			
			.def("GoEfnAnim",			&QObjBase::GoEfnAnim)
			.def("StopEfnAnim",			&QObjBase::StopEfnAnim)
			
			.def("AddObjToCollisionList",&QObjBase::AddObjToCollisionList)
			.def("RemoveObjFromCollisionList",&QObjBase::RemoveObjFromCollisionList),

		class_<QObjMan, QObjBase >("QObjMan")
			.def("Go",					&QObjMan::Go)
			.def("Stop",				&QObjMan::Stop)
			.def("GetTarget",			&QObjMan::GetTarget)
			.def("GetManRun",			&QObjMan::GetManRun)
			.def("SetPos",				&QObjMan::SetPos)
			.def("Teleport",			&QObjMan::Teleport)
			.def("GetNewCoords",		&QObjMan::GetNewCoords)
			.def("GetAngleDif",			&QObjMan::GetAngleDif)
			.def("GetNewAngle",			&QObjMan::GetNewAngle)
			.def("GetAngle",			&QObjMan::GetAngle)
			.def("Path",				&QObjMan::Path)
			.def("GetPathLength",		&QObjMan::GetPathLength),

		class_<QObjCar, QObjBase >("QObjCar")
			.def("Go",					&QObjCar::Go)
			.def("Stop",				&QObjCar::Stop)
			.def("SetPos",				&QObjCar::SetPos)
			.def("Teleport",			&QObjCar::Teleport)
			.def("GetTarget",			&QObjCar::GetTarget)
			.def("GetNewCoords",		&QObjCar::GetNewCoords)
			.def("GetAngleDif",			&QObjCar::GetAngleDif)
			.def("GetNewAngle",			&QObjCar::GetNewAngle)
			.def("GetAngle",			&QObjCar::GetAngle)
			.def("Path",				&QObjCar::Path)
			.def("GetPathLength",		&QObjCar::GetPathLength),

		class_<QObjBird, QObjBase >("QObjBird")
			.def("Go",					&QObjBird::Go)
			.def("Fly",					&QObjBird::Fly)
			.def("Stop",				&QObjBird::Stop)
			.def("AddCP",				&QObjBird::AddCP)
			.def("SetPos",				&QObjBird::SetPos)
			.def("Teleport",			&QObjBird::Teleport)
			.def("GetTarget",			&QObjBird::GetTarget)
			.def("GetNewCoords",		&QObjBird::GetNewCoords)
			.def("GetAngleDif",			&QObjBird::GetAngleDif)
			.def("GetNewAngle",			&QObjBird::GetNewAngle)
			.def("GetAngle",			&QObjBird::GetAngle)
			.def("Path",				&QObjBird::Path)
			.def("GetPathLength",		&QObjBird::GetPathLength),

		class_<QObjFree, QObjBase >("QObjFree")
			.def("Go",					&QObjFree::Go)
			.def("SetPos",				&QObjFree::SetPos)
			.def("Teleport",			&QObjFree::Teleport)
			.def("Stop",				&QObjFree::Stop),

		class_<QObjLight, QObjBase >("QObjLight"),

		class_<QObjCamera, QObjBase >("QObjCamera")
			.def("SetPerCamPrm",		&QObjCamera::SetPerCamPrm)
			.def("SetOrthoCamPrm",		&QObjCamera::SetOrthoCamPrm)
			.def("SetPerCamPos",		&QObjCamera::SetPerCamPos)
			.def("SetPerCamTarget",		&QObjCamera::SetPerCamTarget)
			.def("SetOrthoCamPos",		&QObjCamera::SetOrthoCamPos)
			.def("SetOrthoCamTarget",	&QObjCamera::SetOrthoCamTarget),

		class_<QObjShadowVolume, QObjBase >("QObjShadowVolume"),

		class_<QObjLayerGeom, QObjBase >("QObjLayerGeom"),

		class_<QObjAction, QObjBase >("QObjAction"),

		class_<QObjEfnAnim, QObjBase >("QObjEfnAnim")
			.def("Go",					&QObjEfnAnim::Go)			
			.def("Stop",				&QObjEfnAnim::Stop)
			.def("AnimAdd",				&QObjEfnAnim::AnimAdd)
			.def("AnimAdd2",			&QObjEfnAnim::AnimAdd2)
			.def("AnimAdd3",			&QObjEfnAnim::AnimAdd3)
			.def("MorfAnimAdd",			&QObjEfnAnim::MorfAnimAdd)
			.def("MorfAnimAdd2",		&QObjEfnAnim::MorfAnimAdd2)
			.def("MorfAnimAdd3",		&QObjEfnAnim::MorfAnimAdd3)
			.def("AttachObjToNull",		&QObjEfnAnim::AttachObjToNull)
			.def("AttachObjToNullC",	&QObjEfnAnim::AttachObjToNullC)
			.def("AttachObjToNullE",	&QObjEfnAnim::AttachObjToNullE)
			.def("AttachObjToNullEC",	&QObjEfnAnim::AttachObjToNullEC)
			.def("DeAttachObjFromNull",	&QObjEfnAnim::DeAttachObjFromNull)
			.def("DeAttachObjsFromNull",&QObjEfnAnim::DeAttachObjsFromNull)
			.def("DeAttachAll",			&QObjEfnAnim::DeAttachAll)
			.def("GetAnimName",			&QObjEfnAnim::GetAnimName)
			.def("IsStarted",			&QObjEfnAnim::IsStarted)
			.def("GetSeqID",			&QObjEfnAnim::GetSeqID)
			.def("SetRenderPrms",		&QObjEfnAnim::SetRenderPrms)
			.def("SetSendAnimsToChilds",&QObjEfnAnim::SetSendAnimsToChilds)
			.def("SetFillZBuffer",		&QObjEfnAnim::SetFillZBuffer)
			.def("SetZeroAnim",			&QObjEfnAnim::SetZeroAnim)
			.def("SetCullNone",			&QObjEfnAnim::SetCullNone)
			.def("SetTexSphereMapping",	&QObjEfnAnim::SetTexSphereMapping)
			.def("SetTex2SphereMapping",&QObjEfnAnim::SetTex2SphereMapping)
			.def("SetCoords",			&QObjEfnAnim::SetCoords)
			.def("SetPosition",			&QObjEfnAnim::SetPosition)
			.def("SetRotation",			&QObjEfnAnim::SetRotation)
			.def("SetScale",			&QObjEfnAnim::SetScale)
			.def("GetPathLength",		&QObjEfnAnim::GetPathLength)
			.def("GetPathLast",			&QObjEfnAnim::GetPathLast)
			.def("GetOffsetOnPath",		&QObjEfnAnim::GetOffsetOnPath)
			.def("AddPathLastEvent",	&QObjEfnAnim::AddPathLastEvent)
			.def("ClearPathLastEvents",	&QObjEfnAnim::ClearPathLastEvents),

		class_<QObjVideoAnim, QObjBase >("QObjVideoAnim")
			.def("Go",					&QObjVideoAnim::Go)			
			.def("Stop",				&QObjVideoAnim::Stop)
			.def("AnimAdd",				&QObjVideoAnim::AnimAdd),

		class_<QObj3DVideo, QObjBase >("QObj3DVideo")
			.def("Go",					&QObj3DVideo::Go)			
			.def("Stop",				&QObj3DVideo::Stop)
			.def("SetPos",				&QObj3DVideo::SetPos)
			.def("AttachObjEfnAnimToTimeLineNull",&QObj3DVideo::AttachObjEfnAnimToTimeLineNull),

		class_<QObjMenuRukzak, QObjBase >("QObjMenuRukzak")
			.def("IsOpened",			&QObjMenuRukzak::IsOpened)
			.def("AddListItem",			&QObjMenuRukzak::AddListItem)
			.def("AddChildToListItem",	&QObjMenuRukzak::AddChildToListItem)
			.def("GetItemName",			&QObjMenuRukzak::GetItemName)
			.def("AddItem",				&QObjMenuRukzak::AddItem)
			.def("RemoveItem",			&QObjMenuRukzak::RemoveItem)
			.def("ClearZooms",			&QObjMenuRukzak::ClearZooms)
			.def("SetZoomToItem",		&QObjMenuRukzak::SetZoomToItem)
			.def("Open",				&QObjMenuRukzak::Open)
			.def("Close",				&QObjMenuRukzak::Close)
			.def("GetClickedItemID",	&QObjMenuRukzak::GetClickedItemID)
			.def("GetItemIDFromNum",	&QObjMenuRukzak::GetItemIDFromNum),

		class_<QObjMenuMenu, QObjBase >("QObjMenuMenu")
			.def("IsOpened",			&QObjMenuMenu::IsOpened)
			.def("Open",				&QObjMenuMenu::Open)
			.def("Close",				&QObjMenuMenu::Close)
			.def("SetActiveLine",		&QObjMenuMenu::SetActiveLine),

		class_<QObjMenuCursor, QObjBase >("QObjMenuCursor")
			.def("SetItem",				&QObjMenuCursor::SetItem)
			.def("GetItem",				&QObjMenuCursor::GetItem)
			.def("Clear",				&QObjMenuCursor::Clear)
			.def("GetCursorType",		&QObjMenuCursor::GetCursorType)
			.def("SetCursorAnim",		&QObjMenuCursor::SetCursorAnim)
			.def("SetCursorAnim2",		&QObjMenuCursor::SetCursorAnim2)
			.def("GetCursorAnim",		&QObjMenuCursor::GetCursorAnim)
			.def("SetCursorHint",		&QObjMenuCursor::SetCursorHint)
			.def("MouseClick",			&QObjMenuCursor::MouseClick)
	];

	luabind::get_globals(luaVM)["g_Engine"]		= boost::ref(g_Engine);
	luabind::get_globals(luaVM)["g_Graph"]		= boost::ref(g_Graph);
	luabind::get_globals(luaVM)["g_Input"]		= boost::ref(g_Input);
	luabind::get_globals(luaVM)["g_Mouse"]		= boost::ref(g_Mouse);
	luabind::get_globals(luaVM)["g_Log"]		= boost::ref(g_Log);
	luabind::get_globals(luaVM)["g_StatesMgr"]	= boost::ref(g_StatesMgr);	
	luabind::get_globals(luaVM)["g_TexMgr"]		= boost::ref(g_TexMgr);
	luabind::get_globals(luaVM)["g_FontMgr"]	= boost::ref(g_FontMgr);
	luabind::get_globals(luaVM)["g_EfnMgr"]		= boost::ref(g_EfnMgr);
	luabind::get_globals(luaVM)["g_ShadMgr"]	= boost::ref(g_ShadMgr);	
	luabind::get_globals(luaVM)["g_SndMgr"]		= boost::ref(g_SndMgr);	
	luabind::get_globals(luaVM)["g_Script"]		= boost::ref(g_Script);	
	luabind::get_globals(luaVM)["g_Timers"]		= boost::ref(g_Timers);	
	luabind::get_globals(luaVM)["g_GamesMgr"]	= boost::ref(g_GamesMgr);	
	luabind::get_globals(luaVM)["g_GC"]			= boost::ref(g_GC);

	return ExecFile(L"QEngine_RegisterWraper.lua");
}