#define STRICT

#include "QMgrScripts.h"

#include "QEngine.h"

#include "QSubSysLog.h"

bool QMgrScripts::RegisterVariables()
{
	SYSTEMTIME time;
    GetSystemTime(&time);
	
	srand( (unsigned)time.wMilliseconds );
	rand();

	using namespace luabind;

	module(luaVM)
	[
		def("float2int",				&float2int),
		
		def("bool2wstr",				&bool2wstr),
		
		def("int2wstr",					(std::wstring(*)(int))&int2wstr),
		def("int2wstr",					(std::wstring(*)(int, int))&int2wstr),
		def("wstr2int",					&wstr2int),

		def("float2wstr",				&float2wstr),
		def("wstr2float",				&wstr2float),

		def("dbl2wstr",					&dbl2wstr),
		def("wstr2dbl",					&wstr2dbl),

		def("L",						(std::wstring(*)(const std::string&))&str2wstr),

		def("AngleDif",					&AngleDif),
		
		def("D3DXVec3Length",			&D3DXVec3Length),

		def("ttime",					&ttime),

		def("GetMtrlDef",				&GetMtrlDef),
		def("GetMtrl",					&GetMtrl),
		def("GetMtrl2",					&GetMtrl2),

		def("GetNull",					&GetNull),
		def("GetNull2",					&GetNull2),

		def("ShowMessage", &ShowMessage)
	];

	module(luaVM)
	[
		class_<iVECTOR2>("iVECTOR2")
			.def_readwrite("x", &iVECTOR2::x)
			.def_readwrite("y", &iVECTOR2::y),
		
		class_<std::string>("string")
			.def(constructor<const std::string&>()),
		
		class_<std::wstring>("wstring")
			.def(constructor<const std::wstring&>())
			.def(self + std::wstring())
			.def(self == std::wstring()),

		class_<D3DVECTOR>("D3DVECTOR")
			.def_readwrite("x",		&D3DVECTOR::x)
			.def_readwrite("y",		&D3DVECTOR::y)
			.def_readwrite("z",		&D3DVECTOR::z),
		
		class_<D3DXVECTOR2>("D3DXVECTOR2")
			.def(constructor<const float&, const float&>())
			.def(self + D3DXVECTOR2())
			.def(self - D3DXVECTOR2())
			.def(self * float())
			.def(self == D3DXVECTOR2()),

		class_<D3DXVECTOR3, D3DVECTOR>("D3DXVECTOR3")
			.def(constructor<const float&, const float&, const float&>())
			.def(self + D3DXVECTOR3())
			.def(self - D3DXVECTOR3())
			.def(self * float())
			.def(self == D3DXVECTOR3()),

		class_<D3DXCOLOR>("D3DXCOLOR")
			.def(constructor<const long&>())
			.def(constructor<const float&, const float&, const float&, const float&>()),

		class_<D3DMATERIAL9>("D3DMATERIAL9"),

		class_<SRay>("SRay")
			.def_readwrite("vPos",		&SRay::vPos)
			.def_readwrite("vDir",		&SRay::vDir)
			.def_readwrite("vTar",		&SRay::vTar),
		
		class_<SNULL>("SNULL")
			.def_readwrite("vPos",		&SNULL::vPos)
			.def_readwrite("vRot",		&SNULL::vRot)
			.def_readwrite("vScale",	&SNULL::vScale),

		class_<MouseInput>("MouseInput")
			.def_readwrite("x",			&MouseInput::x)
			.def_readwrite("y",			&MouseInput::y)
			.def_readwrite("left_x",	&MouseInput::left_x)
			.def_readwrite("left_y",	&MouseInput::left_y)
			.def_readwrite("right_x",	&MouseInput::right_x)
			.def_readwrite("right_y",	&MouseInput::right_y)
	];

	module(luaVM)
	[
		class_<QEngine>("QEngine")
			.def("SetGameSpeed",		&QEngine::SetGameSpeed)
			.def("Exit",				&QEngine::Exit),

		class_<QSubSysGraphics>("QSubSysGraphics")
			.def("SetWindowPrm",		&QSubSysGraphics::SetWindowPrm)
			.def("CreateWnd",			&QSubSysGraphics::CreateWnd)
			.def("SetTextureQuality",	&QSubSysGraphics::SetTextureQuality)
			.def("SetUseDXT",			&QSubSysGraphics::SetUseDXT)
			.def("SetUseMip1ForBacks",	&QSubSysGraphics::SetUseMip1ForBacks)
			.def("SetShadowQuality",	&QSubSysGraphics::SetShadowQuality)
			.def("SetUseRimTextureLib",	&QSubSysGraphics::SetUseRimTextureLib)
			.def("RenderLoad",			&QSubSysGraphics::RenderLoad)
			.def("SetLoadAnimLength",	&QSubSysGraphics::SetLoadAnimLength)
			.def("SetLoadAnimPos",		&QSubSysGraphics::SetLoadAnimPos)
			.def("SetMultiSample",		&QSubSysGraphics::SetMultiSample)
			.def("SetGamma",			&QSubSysGraphics::SetGamma)
			.def("SetRamp",				&QSubSysGraphics::SetRamp)
			.def("GoFadeUp",			&QSubSysGraphics::GoFadeUp)
			.def("GoFadeDown",			&QSubSysGraphics::GoFadeDown)
			.def("GoFadeUp2",			&QSubSysGraphics::GoFadeUp2)
			.def("GoFadeDown2",			&QSubSysGraphics::GoFadeDown2)
			.def("SetKeysHelp",			&QSubSysGraphics::SetKeysHelp)
			.def("GetKeysHelp",			&QSubSysGraphics::GetKeysHelp)
			.def("SetForceBits",		&QSubSysGraphics::SetForceBits)
			.def("SetForceDebug",		&QSubSysGraphics::SetForceDebug)
			.def("SetForceDisp",		&QSubSysGraphics::SetForceDisp)
			.def("SetForceFS",			&QSubSysGraphics::SetForceFS)
			.def("SetForceRezMode",		&QSubSysGraphics::SetForceRezMode)
			.def("SetForceSW",			&QSubSysGraphics::SetForceSW)
			.def("SetForceTexBits",		&QSubSysGraphics::SetForceTexBits)
			.def("SetForceStartScene",	&QSubSysGraphics::SetForceStartScene)
			.def("SetForceStartNull",	&QSubSysGraphics::SetForceStartNull)
			.def("GetDebug",			&QSubSysGraphics::GetDebug)
			.def("GetForceStartScene",	&QSubSysGraphics::GetForceStartScene)
			.def("GetForceStartNull",	&QSubSysGraphics::GetForceStartNull)
			.def_readwrite("fWidth",	&QSubSysGraphics::fWidth)
			.def_readwrite("fHeight",	&QSubSysGraphics::fHeight)
			
			.def("LogEnabled",			&QSubSysGraphics::LogEnabled),
		
		class_<QSubSysInput>("QSubSysInput")
			.def("SetMouseOverTickTime",&QSubSysInput::SetMouseOverTickTime)

			.def("SetEnabled",			&QSubSysInput::SetEnabled)
			.def("GetEnabled",			&QSubSysInput::GetEnabled),

		class_<QSubSysLog>("QSubSysLog")
			.def("Write",				&QSubSysLog::Write)
			.def("WriteMessage",		&QSubSysLog::WriteMessage)
			.def("WriteWarning",		&QSubSysLog::WriteWarning)
			.def("WriteError",			&QSubSysLog::WriteError)
			.def("DebugMode",			&QSubSysLog::DebugMode),

		class_<QMgrTimers>("QMgrTimers")
			.def("TimerSet",			&QMgrTimers::TimerSet )
			.def("TimerSet2",			&QMgrTimers::TimerSet2 )
			.def("TimerKill",			&QMgrTimers::TimerKill )
			.def("TimerKillAll",		&QMgrTimers::TimerKillAll )
			.def("TimerPause",			&QMgrTimers::TimerPause )
			.def("TimerContinue",		&QMgrTimers::TimerContinue )
			
			.def("LogEnabled",			&QMgrTimers::LogEnabled),

		class_<IMgrGC>("IMgrGC")
			.def("Collect",				&IMgrGC::Collect)
			.def("GetCollectionInterval",	&IMgrGC::GetCollectionInterval)
			.def("SetCollectionInterval",	&IMgrGC::SetCollectionInterval)
			.def("GetTimeFromLastCollect",	&IMgrGC::GetTimeFromLastCollect)

			.def("LogEnabled",			&IMgrGC::LogEnabled),

		class_<QMgrScripts>("QMgrScripts")
			.def("ExecScript",			&QMgrScripts::ExecScript)
			.def("ExecBool",			&QMgrScripts::ExecBool)
			.def("ExecFile",			&QMgrScripts::ExecFile)
			.def("Exists",				&QMgrScripts::Exists)
			.def("Exists2",				&QMgrScripts::Exists2)
			.def("RegEvents",			&QMgrScripts::RegisterEvents)

			.def("LogEnabled",			&QMgrScripts::LogEnabled)
			
			.def_readwrite("bExpressionResult",	&QMgrScripts::bExpressionResult),

		class_<QMgrMiniGames>("QMgrMiniGames")
			.def("AddGameQuest",		&QMgrMiniGames::AddGameQuest)	
			.def("AddGameVideo",		&QMgrMiniGames::AddGameVideo)
			.def("AddGameConsole",		&QMgrMiniGames::AddGameConsole)
			.def("AddGameMenu",			&QMgrMiniGames::AddGameMenu)
			.def("AddGameStop",			&QMgrMiniGames::AddGameStop)
			.def("DelGame",				&QMgrMiniGames::DelGame)
			.def("DelGames",			&QMgrMiniGames::DelGames)
			.def("GetGameQuest",		&QMgrMiniGames::GetGameQuest)
			.def("GetGameVideo",		&QMgrMiniGames::GetGameVideo)
			.def("GetGameConsole",		&QMgrMiniGames::GetGameConsole)
			.def("GetGameMenu",			&QMgrMiniGames::GetGameMenu)
			.def("GetGameStop",			&QMgrMiniGames::GetGameStop)
			.def("GetObjMgr",			&QMgrMiniGames::GetObjMgr)
			.def("GetObjMgrZ",			&QMgrMiniGames::GetObjMgrZ)
			.def("InitGame",			&QMgrMiniGames::InitGame)
			.def("PauseAll",			&QMgrMiniGames::PauseAll)
			.def("Pause",				&QMgrMiniGames::Pause)
			.def("PauseMove",			&QMgrMiniGames::PauseMove)
			.def("PauseRender",			&QMgrMiniGames::PauseRender)
			.def("ContinueAll",			&QMgrMiniGames::ContinueAll)
			.def("Continue",			&QMgrMiniGames::Continue)
			.def("ContinueMove",		&QMgrMiniGames::ContinueMove)
			.def("ContinueRender",		&QMgrMiniGames::ContinueRender)
			.def("FreezeAll",			&QMgrMiniGames::FreezeAll)
			.def("UnFreezeAll",			&QMgrMiniGames::UnFreezeAll)

			.def("LogEnabled",			&QMgrMiniGames::LogEnabled),

		class_<IMgrResTextures>("IMgrResTextures")
			.def("LoadTex",				&IMgrResTextures::LoadTex)
			.def("LoadTex2",			&IMgrResTextures::LoadTex2)
			.def("LoadTex3",			&IMgrResTextures::LoadTex3)
			.def("LoadTex4",			&IMgrResTextures::LoadTex4)
			.def("LoadVideo",			&IMgrResTextures::LoadVideo)
			.def("LoadVideo2",			&IMgrResTextures::LoadVideo2)
			.def("FreeTex",				&IMgrResTextures::FreeTex)
			.def("FreeTexs",			&IMgrResTextures::FreeTexs)
			.def("SetTexFrame",			&IMgrResTextures::SetTexFrame)

			.def("LogEnabled",			&IMgrResTextures::LogEnabled),

		class_<IMgrResFonts>("IMgrResFonts")
			.def("LoadFont",			&IMgrResFonts::LoadFont)
			.def("LoadFont2",			&IMgrResFonts::LoadFont2)
			.def("FreeFont",			&IMgrResFonts::FreeFont)
			.def("FreeAll",				&IMgrResFonts::FreeAll)

			.def("LogEnabled",			&IMgrResFonts::LogEnabled),

		class_<QMgrResSounds>("QMgrResSounds")
			.def("Init",				&QMgrResSounds::Init)
			.def("LoadSmpl",			&QMgrResSounds::LoadSmpl)
			.def("LoadStrm",			&QMgrResSounds::LoadStrm)
			.def("FreeSnd",				&QMgrResSounds::FreeSnd)
			.def("PlaySnd",				&QMgrResSounds::PlaySnd)
			.def("PlaySnd2",			&QMgrResSounds::PlaySnd2)
			.def("StopSnd",				&QMgrResSounds::StopSnd)

			.def("LogEnabled",			&QMgrResSounds::LogEnabled),

		class_<IMgrResEfnModels>("IMgrResEfnModels")
			.def("LoadEfn",				&IMgrResEfnModels::LoadEfn)
			.def("FreeEfn",				&IMgrResEfnModels::FreeEfnByID)
			.def("GetAnimTimeLength",	&IMgrResEfnModels::GetAnimTimeLength)
			.def("GetAnimZLength",		&IMgrResEfnModels::GetAnimZLength)

			.def("LogEnabled",			&IMgrResEfnModels::LogEnabled),

		class_<QMgrResStates>("QMgrResStates")
			.def("SetState",			&QMgrResStates::SetState)
			.def("SetStateB",			&QMgrResStates::SetStateB)
			.def("SetStateF",			&QMgrResStates::SetStateF)
			.def("SetStateD",			&QMgrResStates::SetStateD)
			.def("SetStateV2",			&QMgrResStates::SetStateV2)
			.def("SetStateV3",			&QMgrResStates::SetStateV3)
			.def("GetState",			&QMgrResStates::GetState)
			.def("GetStateB",			&QMgrResStates::GetStateB)
			.def("GetStateF",			&QMgrResStates::GetStateF)
			.def("GetStateD",			&QMgrResStates::GetStateD)
			.def("GetStateV2",			&QMgrResStates::GetStateV2)
			.def("GetStateV3",			&QMgrResStates::GetStateV3)
			.def("IncState",			&QMgrResStates::IncState)
			.def("DelStates",			&QMgrResStates::DelStates)
			
			.def("SetEvent",			&QMgrResStates::SetEvent)
			.def("DelEvent",			&QMgrResStates::DelEvent)
			.def("DelEvents",			&QMgrResStates::DelEvents)
			
			.def("SeqInit",				&QMgrResStates::SeqInit)
			.def("SeqAddAct",			&QMgrResStates::SeqAddAct)
			.def("SeqAddActInc",		&QMgrResStates::SeqAddActInc)
			.def("SeqPause",			&QMgrResStates::SeqPause)
			.def("SeqActGo",			&QMgrResStates::SeqActGo)
			.def("SeqGo",				&QMgrResStates::SeqGo)
			.def("SeqIncState",			&QMgrResStates::SeqIncState)
			.def("SeqZeroState",		&QMgrResStates::SeqZeroState)
			.def("SeqDel",				&QMgrResStates::SeqDel)
			.def("SeqDels",				&QMgrResStates::SeqDels)

			.def("GetEvState",			&QMgrResStates::GetEvState)
			.def("EvStackKill",			&QMgrResStates::EvStackKill)
			.def("EvKill",				&QMgrResStates::EvKill)
			
			.def("LogEnabled",			&QMgrResStates::LogEnabled),

		class_<QMgrResShaders>("QMgrResShaders")
			.def("LoadShader",			&QMgrResShaders::LoadShader)
			.def("FreeShader",			&QMgrResShaders::FreeShader)
			
			.def("LogEnabled",			&QMgrResShaders::LogEnabled)
	];

	return true;
}