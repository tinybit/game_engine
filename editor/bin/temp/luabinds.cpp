#define STRICT

#include "QMgrScripts.h"
#include <luabind/detail/policy.hpp>
#include <luabind/copy_policy.hpp>
#include <luabind/adopt_policy.hpp>

#include "QEngine.h"

#include "QSubSysLog.h"
#include "QSubSysGraphics.h"
#include "QSubSysInput.h"

#include "QGameQuestScene.h"

bool QMgrScripts::RegisterVariables1()
{
	using namespace luabind;

	module(luaVM)
	[
		class_<iVECTOR2>("iVECTOR2") //<inf>iVECTOR2 структура, вектор 2D с int значениями</inf>
			.def(constructor<const int&, const int&>()) //<return>iVECTOR2</return>, <tmpl>iVECTOR2( int x, int y )</tmpl>, <inf>конструктор</inf>
			.def_readwrite("x",			&iVECTOR2::x) //<return>int</return>
			.def_readwrite("y",			&iVECTOR2::y), //<return>int</return>

		class_<D3DVECTOR>("D3DVECTOR") //<inf>базовый класс для D3DXVECTOR3</inf>
			.def_readwrite("x",			&D3DVECTOR::x) //<return>float</return> 
			.def_readwrite("y",			&D3DVECTOR::y) //<return>float</return>
			.def_readwrite("z",			&D3DVECTOR::z), //<return>float</return>
		
		class_<D3DXVECTOR2>("D3DXVECTOR2") //<inf>D3DXVECTOR2 класс, вектор 2D с float значениями</inf>
			.def(constructor<const float&, const float&>()) //<return><D3DXVECTOR2></return>, <tmpl>D3DXVECTOR2( float x, float y )</tmpl>, <inf>конструктор></inf>
			.def(self + D3DXVECTOR2()) //<return><D3DXVECTOR2></return>
			.def(self - D3DXVECTOR2())   //<return><D3DXVECTOR2></return>
			.def(self * float()) //<return><D3DXVECTOR2</return>
			.def(self == D3DXVECTOR2()) //<return>bool</return>
			.def_readwrite("x",			&D3DXVECTOR2::x) //<return>float</return>
			.def_readwrite("y",			&D3DXVECTOR2::y), //<return>float</return>

		class_<D3DXVECTOR3, D3DVECTOR>("D3DXVECTOR3") //<inf>D3DXVECTOR3 класс, вектор 3D с float значениями></inf>
			.def(constructor<const float&, const float&, const float&>()) //<return>D3DXVECTOR3</return>, <tmpl>D3DXVECTOR3( float x, float y, float z )</tmpl>, <inf>конструктор</inf>
			.def(self + D3DXVECTOR3()) //<return>D3DXVECTOR3</return>
			.def(self - D3DXVECTOR3())   //<return>D3DXVECTOR3</return>
			.def(self * float()) //<return>D3DXVECTOR3</return>
			.def(self == D3DXVECTOR3()), //<return>bool</return>

		class_<D3DXCOLOR>("D3DXCOLOR") //<inf>D3DXCOLOR класс, цвет с float значениями></inf>
			.def(constructor<const long&>()) //<return>D3DXCOLOR</return>, <tmpl>D3DXCOLOR( DWORD Color )</tmpl>, <inf>конструктор, Color = 0xAARRGGBB</inf>
			.def(constructor<const float&, const float&, const float&, const float&>()), //<return>D3DXCOLOR</return>, <tmpl>D3DXCOLOR( float r, float g, float b, float a )</tmpl>, <inf>конструктор , r,g,b,a = 0.0 .. 1.0, 0 полностью прозрачный, 1 непрозрачный</inf>

		class_<D3DMATERIAL9>("D3DMATERIAL9"),//<inf>D3DMATERIAL9 класс</inf>
		
		class_<SRAY>("SRAY") //<inf>SRAY структура, луч</inf>
			.def(constructor<const D3DXVECTOR3*, const D3DXVECTOR3*>()) //<return>SRAY</return>, <tmpl>SRAY( D3DXVECTOR3 Pos, D3DXVECTOR3 Dir )</tmpl>, <inf>конструктор, создает неопределенный луч</inf>
			.def(constructor<const D3DXVECTOR3*, const D3DXVECTOR3*, float&>()) //<return>SRAY</return>, <tmpl>SRAY( D3DXVECTOR3 Pos, D3DXVECTOR3 Dir, float fDist )</tmpl>, <inf>конструктор</inf>
			.def(constructor<const D3DXVECTOR3*, const D3DXVECTOR3*, const D3DXVECTOR3*>()) //<return>SRAY</return>, <tmpl>SRAY( D3DXVECTOR3 Pos, D3DXVECTOR3 Dir, D3DXVECTOR3 Tar )</tmpl>, <inf>конструктор</inf>
			.def_readwrite("vPos",		&SRAY::vPos) //<return>D3DXVECTOR3</return>
			.def_readwrite("vDir",		&SRAY::vDir) //<return>D3DXVECTOR3</return>
			.def_readwrite("vTar",		&SRAY::vTar), //<return>D3DXVECTOR3</return>

		class_<SNULL_BASE>("SNULL_BASE") //<inf>SNULL структура, определяет полное позицирование объекта></inf>
			.def_readwrite("vPos",		&SNULL::vPos) //<return>D3DXVECTOR3</return>
			.def_readwrite("vRot",		&SNULL::vRot) //<return>D3DXVECTOR3</return>
			.def_readwrite("vScale",	&SNULL::vScale), //<return>D3DXVECTOR3</return>
		
		class_<SNULL, SNULL_BASE>("SNULL") //<inf>SNULL структура, определяет полное позицирование объекта></inf>
			.def(constructor<>())  //<return>SNULL</return>, <tmpl>SNULL()</tmpl>, <inf>конструктор, создает нуль с координатами в нуле и скейлами 1</inf>
			.def(constructor<const float&, const float&, const float&>()) //<return>SNULL</return>, <tmpl>SNULL( float x, float y, float z )</tmpl>, <inf>конструктор, создает нуль с указанной позицией</inf>
			.def(constructor<const D3DXVECTOR3*>()) //<return>SNULL</return>, <tmpl>SNULL( D3DXVECTOR3 Pos )</tmpl>, <inf>конструктор, создает нуль с указнной позицией</inf>
			.def(constructor<const D3DXVECTOR3*, const D3DXVECTOR3*>()) //<return>SNULL</return>, <tmpl>SNULL( D3DXVECTOR3 Pos, D3DXVECTOR3 Rot )</tmpl>, <inf>конструктор, создает нуль с позицией и поворотм, а скейл будет 1</inf>
			.def(constructor<const D3DXVECTOR3*, const D3DXVECTOR3*, const D3DXVECTOR3*>()) //<return>SNULL</return>, <tmpl>SNULL( D3DXVECTOR3 Pos, D3DXVECTOR3 Rot, D3DXVECTOR3 Scale )</tmpl>, <inf>конструктор, со всеми параметрами</inf>
			.def(constructor<const SNULL*>()), //<return>SNULL</return>, <tmpl>SNULL( SNULL null )</tmpl>, <inf>конструктор, со всеми параметрами</inf>

		class_<vecSNULL>("vecSNULL") //<inf>vecSNULL stl вектор нулей</inf>
			.def("size",				&vecSNULL::size) //<return>int</return>, <tmpl>size()</tmpl>, <inf>размер вектора элементов</inf>
			.def("clear",				&vecSNULL::clear), //<return>void</return>, <tmpl>clear()</tmpl>, <inf>очистить вектор</inf>

		class_<SMOUSEINPUT>("SMOUSEINPUT") //<inf>SMOUSEINPUT структура, используется для считывания состояния мыши, имеет синглетон g_Mouse</inf>
			.def_readwrite("x",			&SMOUSEINPUT::x) //<return>int</return>
			.def_readwrite("y",			&SMOUSEINPUT::y) //<return>int</return>
			.def_readwrite("left_x",	&SMOUSEINPUT::left_x) //<return>int</return>
			.def_readwrite("left_y",	&SMOUSEINPUT::left_y) //<return>int</return>
			.def_readwrite("right_x",	&SMOUSEINPUT::right_x) //<return>int</return>
			.def_readwrite("right_y",	&SMOUSEINPUT::right_y), //<return>int</return>

		class_<SCURVE>("SCURVE") //<inf>SCURVE структура, кривая в пространстве для миниигры Quest</inf>
	];

	module(luaVM)
	[
		
		def("ExecScript",			&ExecScript),
		def("ExecScriptSafe",		&ExecScriptSafe),
		def("ExecFile",				&ExecFile),
		def("ExecFileSafe",			&ExecFileSafe),
		def("RegEvents",			&RegEvents), // bool RegisterEvents( string s ), зарегестрировать функции для когото, наприме QObjMove, т.е. просто выполняется QObjMove_Events.lua
		def("GetLuaError",			&GetLuaError),

		def("ss",					&ss),
		def("ssb",					&ssb),
		def("gs",					&gs),
		def("gsb",					&gsb),

		def("int2str",				&int2str), //<return>string</return>, <tmpl>int2str( int i )</tmpl>
		def("str2int",				&str2int), //<return>int</return>, <tmpl>str2int( string s )</tmpl>

		def("float2str",			&float2str), //<return>string</return>, <tmpl>float2str( float f )</tmpl>
		def("str2float",			&str2float), //return<float>, <tmpl>str2float( string s )</tmpl>

		def("dbl2str",				&dbl2str), //<return>string</return>, <tmpl>dbl2str( double d )</tmpl>
		def("str2dbl",				&str2dbl), //<return>double</return>, <tmpl>str2dbl( string s )</tmpl>

		def("float2int",			&float2int), //<return>int</return>, <tmpl>float2int( int i )</tmpl>
		def("bool2str",				&bool2str), //<return>bool</return>, <tmpl>bool2str( bool b )</tmpl>
		def("H",					&hex2uint), //<return>UINT</return>, <tmpl>H( string s )</tmpl>
		def("vec3tostr",			&vec3tostr), //<return>string</return>, <tmpl>vec3tostr( D3DXVECTOR3 v )</tmpl>, <inf>делает строку для вектора D3DXVECTOR3(1,2,3) следующего вида ( 1,2,3 )</inf>

		def("str2inl",				&str2inl), //<return>string</return>, <tmpl>str2inl( string s )</tmpl>, <inf>Экранирует опасные символы при вставке строки внутрь строки</inf>

		def("AngleDif",				&AngleDif), //<return>float</return>, <tmpl>AngleDif( float NewAngle, float OldAngle )</tmpl>, <inf>показывает разницу между двумя углами "+" если движение от старого к новому по часовой стрелке и "-" если иначе</inf>
		
		def("GetMtrlWhite",			&GetMtrlWhite), //<return>D3DMATERIAL9</return>, <tmpl>GetMtrlWhite()</tmpl>, <inf>создает белый материал</inf>
		def("GetMtrl",				&GetMtrl), //<return>D3DMATERIAL9</return>, <tmpl>GetMtrl( D3DXCOLOR diffuse )</tmpl>, <inf>создает материал с дифущией и амбиетом равным параметру</inf>
		def("GetMtrl2",				&GetMtrl2) //<return>D3DMATERIAL9</return>, <tmpl>GetMtrl2( D3DXCOLOR diffuse, D3DXCOLOR ambient, D3DXCOLOR specular, D3DXCOLOR emissive, float Power )</tmpl>
	];

	module(luaVM)
	[
		class_<QEngine>("QEngine") //<inf>QEngine класс, главный класс в проекте, имеет синглетон g_Engine</inf>
			.def("SetMinFPS",			&QEngine::SetMinFPS) // void SetMinFPS( float fps ), установить минимально возможный fps для игры, если fps опустится ниже игра будет ждать процессорного времени
			.def("SetMaxFPS",			&QEngine::SetMaxFPS) // void SetMaxFPS( float fps ), установить максимально возможный fps для игры
			.def("SetGameSpeed",		&QEngine::SetGameSpeed) // void SetGameSpeed( float speed ), установить скорость игры 1.0 это 100%, 2.0 это 200%
			.def("GetGameSpeed",		&QEngine::GetGameSpeed) // float GetGameSpeed(), получить скорость игры
			.def("IncGameSpeed",		&QEngine::IncGameSpeed) // void IncGameSpeed() увеличить скорость игры
			.def("DecGameSpeed",		&QEngine::DecGameSpeed) // void DecGameSpeed() уменьшить скорость игры
			.def("GetTMgr",				&QEngine::GetTMgr) // QMgrTimers GetTMgr(), получить системный таймер
			.def("SaveState",			&QEngine::SaveState) //	void SaveState( string s ), сохранится с именем
			.def("LoadState",			&QEngine::LoadState) //	void LoadState( string s ), загрузится с именем
			.def("IsLoading",			&QEngine::IsLoading) //	bool IsLoading(), получить флаг идет сейчас загрузка или нет
			.def("GetDebug",			&QEngine::GetDebug) //	
			.def("Exit",				&QEngine::Exit) // void Exit() выйти без вопросов
			
			.def_readwrite("bNoExitConfirm", &QEngine::bNoExitConfirm), // bool, флаг что ненужно подтверждение при выходе
													  
		class_<QMgrScripts>("QMgrScripts") //<inf>QMgrScripts класс, менеджер скриптов<inf>
			.def("ExecBool",			&QMgrScripts::ExecBool) // bool ExecBool( string s ), выполнить будевое выражение, вернет значение выражения
			.def("Exists",				&QMgrScripts::Exists) // bool Exists( string s ), проверка на существование объекта в луа, вернет true если существует, и false если нет
			.def("Exists2",				&QMgrScripts::Exists2) // bool Exists2( string class, string s ), проверка на существование класса и принадлежащего ему объекта в луа, вернет true если существует, и false если нет

			.def("LogEnabled",			&QMgrScripts::LogEnabled) // void LogEnabled( bool b ) включить выключить логи для менеджера скриптов
			
			.def_readwrite("bExpressionResult",	&QMgrScripts::bExpressionResult), //

		class_<QSubSysGraphics>("QSubSysGraphics") //
			//params for create device
			.def("SetWindowPrm",		&QSubSysGraphics::SetWindowPrm) //
			.def("CreateWnd",			&QSubSysGraphics::CreateWnd) //
			.def("SetTextureQuality",	&QSubSysGraphics::SetTextureQuality) //
			.def("SetEconomMemoryMode",	&QSubSysGraphics::SetEconomMemoryMode) //
			.def("SetShadowMapQuality",	&QSubSysGraphics::SetShadowMapQuality) //
			.def("SetShadowMapType",	&QSubSysGraphics::SetShadowMapType) //
			.def("SetStencilShadowEnabled",	&QSubSysGraphics::SetStencilShadowEnabled) //
			.def("SetMultiSample",		&QSubSysGraphics::SetMultiSample) //
			.def("SetForceBits",		&QSubSysGraphics::SetForceBits) //
			.def("SetForceDisp",		&QSubSysGraphics::SetForceDisp) //
			.def("SetForceFS",			&QSubSysGraphics::SetForceFS) //
			.def("SetForceRezMode",		&QSubSysGraphics::SetForceRezMode) //
			.def("SetForceTexBits",		&QSubSysGraphics::SetForceTexBits) //
			.def("SetForceStartScene",	&QSubSysGraphics::SetForceStartScene) //
			.def("SetForceStartNull",	&QSubSysGraphics::SetForceStartNull) //
			.def("GetForceStartGame",	&QSubSysGraphics::GetForceStartGame) //
			.def("GetForceStartLevel",	&QSubSysGraphics::GetForceStartLevel) //
			.def("GetForceStartScene",	&QSubSysGraphics::GetForceStartScene) //
			.def("GetForceStartNull",	&QSubSysGraphics::GetForceStartNull) //
			
			//loading screen
			.def("SetLoadingAnimPos",	&QSubSysGraphics::SetLoadingAnimPos) //
			.def("AddLoadingAnimFrame",	&QSubSysGraphics::AddLoadingAnimFrame) //
			.def("SetLoadingPause",		&QSubSysGraphics::SetLoadingPause) //
			.def("RenderLoad",			&QSubSysGraphics::RenderLoad) //
			
			//fade
			.def("GoFadeUp",			&QSubSysGraphics::GoFadeUp) //
			.def("GoFadeDown",			&QSubSysGraphics::GoFadeDown) //
			.def("GoFadeUp2",			&QSubSysGraphics::GoFadeUp2) //
			.def("GoFadeDown2",			&QSubSysGraphics::GoFadeDown2) //
			.def("SetFadeValue",		&QSubSysGraphics::SetFadeValue) //
			.def("StopFade",			&QSubSysGraphics::StopFade) //

			//gamma
			.def("SetGamma",			&QSubSysGraphics::SetGamma) //
			
			//help
			.def("SetKeysHelp",			&QSubSysGraphics::SetKeysHelp) //
			.def("GetKeysHelp",			&QSubSysGraphics::GetKeysHelp) //
			 
			//device
			.def_readwrite("fWidth",	&QSubSysGraphics::fWidth) //
			.def_readwrite("fHeight",	&QSubSysGraphics::fHeight) //
			.def_readwrite("fWindowSizeAspect",	&QSubSysGraphics::fWindowSizeAspect) //
			
			//utils
			.def("SetRenderOOBBForModels",	&QSubSysGraphics::SetRenderOOBBForModels) //
			.def("GetRenderOOBBForModels",	&QSubSysGraphics::GetRenderOOBBForModels) //
			.def("SetDoOcclusionFrustum",	&QSubSysGraphics::SetDoOcclusionFrustum) //
			.def("GetDoOcclusionFrustum",	&QSubSysGraphics::GetDoOcclusionFrustum) //
			.def("SetShowRenderInf",		&QSubSysGraphics::SetShowRenderInf) //
			.def("GetShowRenderInf",		&QSubSysGraphics::GetShowRenderInf) //
			.def("SetShowDevInf",			&QSubSysGraphics::SetShowDevInf) //
			.def("GetShowDevInf",			&QSubSysGraphics::GetShowDevInf) //
			.def("SetShowKeysHelp",			&QSubSysGraphics::SetShowKeysHelp) //
			.def("GetShowKeysHelp",			&QSubSysGraphics::GetShowKeysHelp) //
			.def("LogEnabled",				&QSubSysGraphics::LogEnabled) //

			//world detail
			.def("SetWorldDetail",			&QSubSysGraphics::SetWorldDetail) //
			.def("GetWorldDetail",			&QSubSysGraphics::GetWorldDetail), //
		
		class_<QSubSysInput>("QSubSysInput")
			.def("SetMouseOverTickTime",&QSubSysInput::SetMouseOverTickTime) //
			.def("KeybDown",			&QSubSysInput::KeybDown) //
			.def("SetInputEnabled",		&QSubSysInput::SetInputEnabled) //
			.def("GetInputEnabled",		&QSubSysInput::GetInputEnabled), //
																  
		class_<QSubSysLog>("QSubSysLog") //
			.def("Msg",					&QSubSysLog::Msg) //
			.def("Wrn",					&QSubSysLog::Wrn) //
			.def("Err",					&QSubSysLog::Err) //
			.def("Adv",					&QSubSysLog::Adv) //
			.def("Throw",				&QSubSysLog::Throw) //
			.def("ShowMessage",			&QSubSysLog::ShowMessage) //

			.def("DebugMode",			&QSubSysLog::DebugMode) //
	];

	return true;
}#define STRICT

#include "QMgrScripts.h"

#include "QMgrStates.h"

#include "QMgrGC.h"
#include "QMgrResTextures.h"
#include "QMgrResEfnModels.h"
#include "QMgrResFonts.h"
#include "QMgrResMaterials.h"
#include "QMgrResSounds.h"
#include "QMgrResFiles.h"

bool QMgrScripts::RegisterVariables11()
{
	using namespace luabind;

	module(luaVM)
	[
		class_<QMgrTimers>("QMgrTimers") //
			.def("TimerSet",			&QMgrTimers::TimerSet ) //
			.def("TimerSet2",			&QMgrTimers::TimerSet2 ) //
			.def("TimerKill",			&QMgrTimers::TimerKill ) //
			.def("TimerKillAll",		&QMgrTimers::TimerKillAll ) //
			.def("TimerPause",			&QMgrTimers::TimerPause ) //
			.def("TimerContinue",		&QMgrTimers::TimerContinue ) //
			
			.def("LogEnabled",			&QMgrTimers::LogEnabled), //

		class_<QMgrResTextures>("QMgrResTextures") //
			.def("LoadTex4",			&QMgrResTextures::LoadTex4) //
			.def("LoadVideo",			&QMgrResTextures::LoadVideo) //
			.def("LoadVideo2",			&QMgrResTextures::LoadVideo2) //
			.def("FreeTex",				&QMgrResTextures::FreeTex) //
			.def("SetTexFrame",			&QMgrResTextures::SetTexFrame) //
			.def("SetBinkPause",		&QMgrResTextures::SetBinkPause) //
			.def("GetBinkPause",		&QMgrResTextures::GetBinkPause) //

			.def("LogEnabled",			&QMgrResTextures::LogEnabled), //

		class_<QMgrResFonts>("QMgrResFonts") //
			.def("LoadFont",			&QMgrResFonts::LoadFont) //
			.def("FreeFont",			&QMgrResFonts::FreeFont) //
			.def("FreeAll",				&QMgrResFonts::FreeAll) //

			.def("LogEnabled",			&QMgrResFonts::LogEnabled), //

		class_<QMgrResSounds>("QMgrResSounds") //
			.def("Init",				&QMgrResSounds::Init) //
			.def("SetSoundVolume",		&QMgrResSounds::SetSoundVolume) //
			.def("GetSoundVolume",		&QMgrResSounds::GetSoundVolume) //
			.def("SetMusicVolume",		&QMgrResSounds::SetMusicVolume) //
			.def("GetMusicVolume",		&QMgrResSounds::GetMusicVolume) //
			.def("PlaySmpl",			&QMgrResSounds::PlaySmpl) //
			.def("PlayList",			&QMgrResSounds::PlayList) //
			.def("StopList",			&QMgrResSounds::StopList) //

			.def("LogEnabled",			&QMgrResSounds::LogEnabled), //

		class_<QMgrResEfnModels>("QMgrResEfnModels")
			.def("LogEnabled",			&QMgrResEfnModels::LogEnabled),

		class_<QMgrResMaterials>("QMgrResMaterials")
			.def("NewMtrl",				&QMgrResMaterials::NewMtrl)
			.def("NewMtrl2",			&QMgrResMaterials::NewMtrl2)
			.def("LoadMtrl",			&QMgrResMaterials::LoadMtrl)
			.def("FreeMtrl",			&QMgrResMaterials::FreeMtrl)
			.def("FreeMtrls",			&QMgrResMaterials::FreeMtrls)
			
			.def("LogEnabled",			&QMgrResMaterials::LogEnabled),

		class_<QMgrResFiles>("QMgrResFiles")
			.def("GetFileText",			&QMgrResFiles::GetFileText),

		class_<QMgrStates>("QMgrStates")
			.def("SetStateF",			&QMgrStates::SetStateF)
			.def("SetStateD",			&QMgrStates::SetStateD)
			.def("SetStateV2",			&QMgrStates::SetStateV2)
			.def("SetStateV3",			&QMgrStates::SetStateV3)
			.def("GetStateF",			&QMgrStates::GetStateF)
			.def("GetStateD",			&QMgrStates::GetStateD)
			.def("GetStateV2",			&QMgrStates::GetStateV2)
			.def("GetStateV3",			&QMgrStates::GetStateV3)
			.def("IncState",			&QMgrStates::IncState)
			.def("DelStates",			&QMgrStates::DelStates)
			
			.def("SetEvent",					&QMgrStates::SetEvent)
			.def("SetEvent_OnMouseMoveDI",		&QMgrStates::SetEvent_OnMouseMoveDI)
			.def("SetEvent_OnMouseLeftClick",	&QMgrStates::SetEvent_OnMouseLeftClick)
			.def("SetEvent_OnMouseRightClick",	&QMgrStates::SetEvent_OnMouseRightClick)
			.def("SetEvent_OnMouseOver",		&QMgrStates::SetEvent_OnMouseOver)
			.def("SetEvent_OnKeybKeyCode",		&QMgrStates::SetEvent_OnKeybKeyCode)
			.def("SetEvent_OnKeybChar",			&QMgrStates::SetEvent_OnKeybChar)
			.def("SetEvent_OnAnyKey",			&QMgrStates::SetEvent_OnAnyKey)

			.def("DelEvent",			&QMgrStates::DelEvent)
			.def("DelEvents",			&QMgrStates::DelEvents)
			
			.def("SeqInit",				&QMgrStates::SeqInit)
			.def("SeqAddAct",			&QMgrStates::SeqAddAct)
			.def("SeqAddActInc",		&QMgrStates::SeqAddActInc)
			.def("SeqPause",			&QMgrStates::SeqPause)
			.def("SeqActGo",			&QMgrStates::SeqActGo)
			.def("SeqGo",				&QMgrStates::SeqGo)
			.def("SeqIncState",			&QMgrStates::SeqIncState)
			.def("SeqZeroState",		&QMgrStates::SeqZeroState)
			.def("SeqDel",				&QMgrStates::SeqDel)
			.def("SeqDels",				&QMgrStates::SeqDels)

			.def("GetEvState",			&QMgrStates::GetEvState)
			.def("EvStackKill",			&QMgrStates::EvStackKill)
			.def("EvKill",				&QMgrStates::EvKill)
			
			.def("LogEnabled",			&QMgrStates::LogEnabled),

		class_<QRandom>("QRandom")
			.def("GetI",				&QRandom::GetI)
			.def("GetI2",				&QRandom::GetI2)
			.def("GetF",				&QRandom::GetF)
			.def("GetF1",				&QRandom::GetF1)
			.def("GetF2",				&QRandom::GetF2)

			.def("Clear",				&QRandom::Clear)
			.def("Add2Buffer",			&QRandom::Add2Buffer)
			.def("GetWeighted",			&QRandom::GetWeighted)

			.def("GetPointOnXZ",		&QRandom::GetPointOnXZ)
			
	];
	return true;
}#define STRICT

#include "QMgrScripts.h"

#include "QMgrObjects.h"

#include "QObjAll.h"

bool QMgrScripts::RegisterVariables2()
{
	using namespace luabind;

	module(luaVM)
	[
		class_<QMgrObjects>("QMgrObjects")
			.def("AddObjMan",			&QMgrObjects::AddObjMan)
			.def("AddObjCar",			&QMgrObjects::AddObjCar)
			.def("AddObjBird",			&QMgrObjects::AddObjBird)
			.def("AddObjFree",			&QMgrObjects::AddObjFree)
			.def("AddObjFreeE",			&QMgrObjects::AddObjFreeE)
			.def("AddObjLight",			&QMgrObjects::AddObjLight)
			.def("AddObjCamera",		&QMgrObjects::AddObjCamera)
			.def("AddObjShadowVolume",	&QMgrObjects::AddObjShadowVolume)
			.def("AddObjLayerGeom",		&QMgrObjects::AddObjLayerGeom)
			.def("AddObjAction",		&QMgrObjects::AddObjAction)
			.def("AddObjActiveZone",	&QMgrObjects::AddObjActiveZone)
			.def("AddObjEfnAnim",		&QMgrObjects::AddObjEfnAnim)
			.def("AddObjEfnAnimE",		&QMgrObjects::AddObjEfnAnimE)
			.def("AddObjVideoAnim",		&QMgrObjects::AddObjVideoAnim)
			.def("AddObjVideo3D",		&QMgrObjects::AddObjVideo3D)
			.def("AddObjMenuRukzak",	&QMgrObjects::AddObjMenuRukzak)
			.def("AddObjMenuMenu",		&QMgrObjects::AddObjMenuMenu)

			.def("AddObjParticlesMgr",	&QMgrObjects::AddObjParticlesMgr)
			.def("AddObjParticlesMgrE",	&QMgrObjects::AddObjParticlesMgrE)

			.def("AddObjCursor",		&QMgrObjects::AddObjCursor)
			
			.def("AddObjForm",			&QMgrObjects::AddObjForm)
			
			.def("DelAll",				&QMgrObjects::DelAll)
			.def("DelObjGroup",			&QMgrObjects::DelObjGroup)
			.def("DelObj",				&QMgrObjects::DelObj)
			.def("DelObj2",				&QMgrObjects::DelObj2)
			
			.def("GetIntersectedObjID",	&QMgrObjects::GetIntersectedObjID)
			.def("GetIntersectedObjIDFromGroup",&QMgrObjects::GetIntersectedObjIDFromGroup)
			.def("CheckForRayIntersect",&QMgrObjects::CheckForRayIntersect)
			.def("GetPosFromIntersected",&QMgrObjects::GetPosFromIntersected)
			.def("GetNullFromIntersected",&QMgrObjects::GetNullFromIntersected)
			.def("GetDistFromIntersected",&QMgrObjects::GetDistFromIntersected)
			.def("GetFirstFromGroup",	&QMgrObjects::GetFirstFromGroup)
			.def("GetObj",				&QMgrObjects::GetObj)
			.def("GetObjBaseMove",		&QMgrObjects::GetObjBaseMove)
			.def("GetObjMan",			&QMgrObjects::GetObjMan)
			.def("GetObjCar",			&QMgrObjects::GetObjCar)
			.def("GetObjBird",			&QMgrObjects::GetObjBird)
			.def("GetObjFree",			&QMgrObjects::GetObjFree)
			.def("GetObjLight",			&QMgrObjects::GetObjLight)
			.def("GetObjCamera",		&QMgrObjects::GetObjCamera)
			.def("GetObjShadowVolume",	&QMgrObjects::GetObjShadowVolume)
			.def("GetObjLayerGeom",		&QMgrObjects::GetObjLayerGeom)
			.def("GetObjAction",		&QMgrObjects::GetObjAction)
			.def("GetObjActiveZone",	&QMgrObjects::GetObjActiveZone)
			.def("GetObjEfnAnim",		&QMgrObjects::GetObjEfnAnim)
			.def("GetObjVideoAnim",		&QMgrObjects::GetObjVideoAnim)
			.def("GetObjVideo3D",		&QMgrObjects::GetObjVideo3D)
			.def("GetObjMenuRukzak",	&QMgrObjects::GetObjMenuRukzak)
			.def("GetObjMenuMenu",		&QMgrObjects::GetObjMenuMenu)

			.def("GetObjParticlesMgr",	&QMgrObjects::GetObjParticlesMgr)

			.def("GetObjCursor",		&QMgrObjects::GetObjCursor)

			.def("GetObjWindow",		&QMgrObjects::GetObjWindow)
			.def("GetObjForm",			&QMgrObjects::GetObjForm)
			.def("GetObjLabel",			&QMgrObjects::GetObjLabel)
			.def("GetObjButton",		&QMgrObjects::GetObjButton)
			.def("GetObjEditBox",		&QMgrObjects::GetObjEditBox)
			
			.def("NewZOrderGroup",		&QMgrObjects::NewZOrderGroup)
			.def("AddObjToZOrderGroup",	&QMgrObjects::AddObjToZOrderGroup)
			.def("ClearZOrderGroups",	&QMgrObjects::ClearZOrderGroups)

			.def("SetTransparentSortType",	&QMgrObjects::SetTransparentSortType)

			.def("AddObjToGroup",		&QMgrObjects::AddObjToGroup)
			.def("DelObjFromGroup",		&QMgrObjects::DelObjFromGroup)
			.def("DelObjFromGroups",	&QMgrObjects::DelObjFromGroups)
			.def("DelGroup",			&QMgrObjects::DelGroup)
			.def("GetFirstFromGroup",	&QMgrObjects::GetFirstFromGroup)
			.def("SetObjGroupLightsEnabled",	&QMgrObjects::SetObjGroupLightsEnabled)
			.def("SetObjGroupCullNone",			&QMgrObjects::SetObjGroupCullNone)
			.def("SetObjGroupMtrlColor",		&QMgrObjects::SetObjGroupMtrlColor)
			.def("SetObjGroupMtrlAlpha",		&QMgrObjects::SetObjGroupMtrlAlpha)
			.def("SetObjGroupMtrlSpecular",		&QMgrObjects::SetObjGroupMtrlSpecular)
			.def("SetObjGroupMtrlEmissive",		&QMgrObjects::SetObjGroupMtrlEmissive)
			.def("SetObjGroupTexID",			&QMgrObjects::SetObjGroupTexID)
			.def("SetObjGroupTexID2",			&QMgrObjects::SetObjGroupTexID2)
			.def("SetObjGroupTexSphereMapping",	&QMgrObjects::SetObjGroupTexSphereMapping)
			.def("SetObjGroupTex2SphereMapping",&QMgrObjects::SetObjGroupTex2SphereMapping)
			.def("SetObjGroupTexBlendFactor",	&QMgrObjects::SetObjGroupTexBlendFactor)
			.def("SetObjGroupSphereMappingByFactorAlpha",	&QMgrObjects::SetObjGroupSphereMappingByFactorAlpha)

			.def("GetUniqObjID",		&QMgrObjects::GetUniqObjID)

			.def("LogEnabled",			&QMgrObjects::LogEnabled),
		   
		//---------------------------------------------------------------------
		class_<QObjBase>("QObjBase")
			.def("Release",				&QObjBase::Release)
			.def("Release2",			&QObjBase::Release2)
			.def("GetPos",				&QObjBase::GetPos)
			.def("SetPos",				&QObjBase::SetPos)
			.def("SetCoords",			&QObjBase::SetCoords)
			.def("SetPosition",			&QObjBase::SetPosition)
			.def("SetRotation",			&QObjBase::SetRotation)
			.def("SetScale",			&QObjBase::SetScale)
			.def("IsInNull2D",			&QObjBase::IsInNull2D)
    		.def("GetEfnAnim",			&QObjBase::GetEfnAnim)
			.def("SetMtrlAlpha",		&QObjBase::SetMtrlAlpha)
			.def("SetMtrlColor",		&QObjBase::SetMtrlColor)
			.def("SetMtrlSpecular",		&QObjBase::SetMtrlSpecular)
			.def("SetMtrlEmissive",		&QObjBase::SetMtrlEmissive)
			.def("SetTexID",			&QObjBase::SetTexID)

			.def("IsAnimStarted",		&QObjBase::IsAnimStarted)

			.def("SetFocus",			&QObjBase::SetFocus)
			.def("InFocus",				&QObjBase::InFocus)
			.def("IsUnderCursor",		&QObjBase::IsUnderCursor)
			.def("IsObjMouseClick",		&QObjBase::IsObjMouseClick)

			.def("GetEnabled",			&QObjBase::GetEnabled)
			.def("SetEnabled",			&QObjBase::SetEnabled)
			
			.def("GetVisible",			&QObjBase::GetVisible)
			.def("SetVisible",			&QObjBase::SetVisible)

			.def("GetExcludeFromIntr",	&QObjBase::GetExcludeFromIntr)
			.def("SetExcludeFromIntr",	&QObjBase::SetExcludeFromIntr)

			.def("GetScript",			&QObjBase::GetScript)
			.def("SetScript",			&QObjBase::SetScript)

			.def("GetHint",				&QObjBase::GetHint)
			.def("SetHint",				&QObjBase::SetHint)

			.def("Show",				&QObjBase::Show)
			.def("Hide",				&QObjBase::Hide)

			.def("GetEfnID",			&QObjBase::GetEfnID)
			.def("SetEfnID",			&QObjBase::SetEfnID),

		class_<QObjLight, QObjBase >("QObjLight")
			.def("SetPosition",			&QObjLight::SetPosition)
			.def("SetDirection",		&QObjLight::SetDirection)
			.def("SetTarget",			&QObjLight::SetTarget)
			.def("SetLightType",		&QObjLight::SetLightType)
			.def("SetDiffuse",			&QObjLight::SetDiffuse)
			.def("SetAmbient",			&QObjLight::SetAmbient)
			.def("SetSpecular",			&QObjLight::SetSpecular)
			.def("SetRange",			&QObjLight::SetRange)
			.def("SetFalloff",			&QObjLight::SetFalloff)
			.def("SetAttenuation0",		&QObjLight::SetAttenuation0)
			.def("SetAttenuation1",		&QObjLight::SetAttenuation1)
			.def("SetAttenuation2",		&QObjLight::SetAttenuation2)
			.def("SetTheta",			&QObjLight::SetTheta)
			.def("SetPhi",				&QObjLight::SetPhi)
			.def("AttachToEfnAnim",		&QObjLight::AttachToEfnAnim),

		class_<QObjCamera, QObjBase >("QObjCamera")
			.def("SetPerCamPrm",		&QObjCamera::SetPerCamPrm)
			.def("SetOrthoCamPrm",		&QObjCamera::SetOrthoCamPrm)
			.def("SetPerCamPos",		&QObjCamera::SetPerCamPos)
			.def("SetPerCamTarget",		&QObjCamera::SetPerCamTarget)
			.def("SetOrthoCamPos",		&QObjCamera::SetOrthoCamPos)
			.def("SetOrthoCamTarget",	&QObjCamera::SetOrthoCamTarget)
			.def("SetZoomPrm",			&QObjCamera::SetZoomPrm)
			.def("SetFogPrm",			&QObjCamera::SetFogPrm)
			.def("SetPerCamFovX",		&QObjCamera::SetPerCamFovX)
			.def("SetPerCamFovY",		&QObjCamera::SetPerCamFovY)
			.def("AttachToEfnAnim",		&QObjCamera::AttachToEfnAnim),

		class_<QObjShadowVolume, QObjBase >("QObjShadowVolume"),

		class_<QObjLayerGeom, QObjBase >("QObjLayerGeom"),

		class_<QObjAction, QObjBase >("QObjAction"),
		class_<QObjActiveZone, QObjBase >("QObjActiveZone"),

		class_<QObjVideoAnim, QObjBase >("QObjVideoAnim")
			.def("Go",					&QObjVideoAnim::Go)			
			.def("Stop",				&QObjVideoAnim::Stop)
			.def("AnimAdd",				&QObjVideoAnim::AnimAdd),

		class_<QObjVideo3D, QObjBase >("QObjVideo3D")
			.def("Go",								&QObjVideo3D::Go)			
			.def("Stop",							&QObjVideo3D::Stop)
			.def("SetUseFadeFromVideo3D",			&QObjVideo3D::SetUseFadeFromVideo3D)
			.def("GetUseFadeFromVideo3D",			&QObjVideo3D::GetUseFadeFromVideo3D)
			.def("AttachObjEfnAnimToTimeLineNull",	&QObjVideo3D::AttachObjEfnAnimToTimeLineNull)
			.def("DeAttachAllFromTimeLineNulls",	&QObjVideo3D::DeAttachAllFromTimeLineNulls),

		class_<QObjMenuRukzak, QObjBase >("QObjMenuRukzak")
			.def("IsOpened",			&QObjMenuRukzak::IsOpened)
			.def("AddListItem",			&QObjMenuRukzak::AddListItem)
			.def("AddChildToListItem",	&QObjMenuRukzak::AddChildToListItem)
			.def("GetItemName",			&QObjMenuRukzak::GetItemName)
			.def("IsItemInRukzak",		&QObjMenuRukzak::IsItemInRukzak)
			.def("IsItemUsed",			&QObjMenuRukzak::IsItemUsed)
			.def("IsItemInPlace",		&QObjMenuRukzak::IsItemInPlace)
			.def("AddItem",				&QObjMenuRukzak::AddItem)
			.def("RemoveItem",			&QObjMenuRukzak::RemoveItem)
			.def("ClearZooms",			&QObjMenuRukzak::ClearZooms)
			.def("FillZooms",			&QObjMenuRukzak::FillZooms)
			.def("Open",				&QObjMenuRukzak::Open)
			.def("Close",				&QObjMenuRukzak::Close)
			.def("GetClickedItemID",	&QObjMenuRukzak::GetClickedItemID)
			.def("GetItemIDFromNum",	&QObjMenuRukzak::GetItemIDFromNum),

		class_<QObjMenuMenu, QObjBase >("QObjMenuMenu")
			.def("IsOpened",			&QObjMenuMenu::IsOpened)
			.def("Open",				&QObjMenuMenu::Open)
			.def("Close",				&QObjMenuMenu::Close)
			.def("SetActiveLine",		&QObjMenuMenu::SetActiveLine),

		class_<QObjParticlesMgr, QObjBase >("QObjParticlesMgr")
			.def("Go",					&QObjParticlesMgr::Go)
			.def("GoTime",				&QObjParticlesMgr::GoTime)
			.def("GoCount",				&QObjParticlesMgr::GoCount)
			.def("Stop",				&QObjParticlesMgr::Stop)
			.def("SetAlphaType",		&QObjParticlesMgr::SetAlphaType)
			.def("GetAlphaType",		&QObjParticlesMgr::GetAlphaType)
			.def("SetBornSpeed",		&QObjParticlesMgr::SetBornSpeed)
			.def("GetBornSpeed",		&QObjParticlesMgr::GetBornSpeed)
			.def("AddTexID",			&QObjParticlesMgr::AddTexID)
			.def("SetParticlesScale",	&QObjParticlesMgr::SetParticlesScale),

		class_<QObjCursor, QObjBase >("QObjCursor")
			.def("SetItem",				&QObjCursor::SetItem)
			.def("GetItem",				&QObjCursor::GetItem)
			.def("Clear",				&QObjCursor::Clear)
			.def("GetCursorType",		&QObjCursor::GetCursorType)
			.def("SetCursorAnim",		&QObjCursor::SetCursorAnim)
			.def("SetCursorAnim2",		&QObjCursor::SetCursorAnim2)
			.def("GetCursorAnim",		&QObjCursor::GetCursorAnim)
			.def("MouseClick",			&QObjCursor::MouseClick)
	];

	return true;
}#define STRICT

#include "QMgrScripts.h"

#include "QObjAll.h"

bool QMgrScripts::RegisterVariables21()
{
	using namespace luabind;

	module(luaVM)
	[
		class_<QObjWindow, QObjBase >("QObjWindow")
			.def("SetWindowText",		QObjWindow::SetWindowText)
			.def("GetWindowText",		QObjWindow::GetWindowText)

			.def("SetWindowPos",		QObjWindow::SetWindowPos)
			.def("GetWindowPos",		QObjWindow::GetWindowPos)

			.def("SetWindowSize",		QObjWindow::SetWindowSize)
			.def("GetWindowSize",		QObjWindow::GetWindowSize),

		class_<QObjForm, QObjWindow >("QObjForm")
			.def("AddLabel",			QObjForm::AddLabel)
			.def("AddLabel2",			QObjForm::AddLabel2)
			.def("AddLabel3",			QObjForm::AddLabel3)
			.def("AddButton",			QObjForm::AddButton)
			.def("AddButton2",			QObjForm::AddButton2)
			.def("AddButton3",			QObjForm::AddButton3)
			.def("AddButton4",			QObjForm::AddButton4)
			.def("AddEditBox",			QObjForm::AddEditBox)
			.def("AddEditBox2",			QObjForm::AddEditBox2)
			.def("AddEditBox3",			QObjForm::AddEditBox3)
			.def("AddEditBox4",			QObjForm::AddEditBox4)
			
			.def("SetLabelFont",		QObjForm::SetLabelFont)
			.def("SetButtonFont",		QObjForm::SetButtonFont)
			
			.def("ShowModal",			QObjForm::ShowModal)

			.def("SetStyleID",			QObjForm::SetStyleID)
			.def("GetStyleID",			QObjForm::GetStyleID),
        
		class_<QObjLabel, QObjWindow >("QObjLabel"),
		class_<QObjButton, QObjWindow >("QObjButton"),
		class_<QObjEditBox, QObjWindow >("QObjEditBox"),

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
			.def("AttachParticlesToNull",&QObjEfnAnim::AttachParticlesToNull)
			.def("DeAttachParticles",	&QObjEfnAnim::DeAttachParticles)
			.def("GetAnimName",			&QObjEfnAnim::GetAnimName)
			.def("GetSeqID",			&QObjEfnAnim::GetSeqID)
			.def("SetRenderPrms",		&QObjEfnAnim::SetRenderPrms)
			.def("SetZeroAnim",			&QObjEfnAnim::SetZeroAnim)
			.def("SetCullNone",			&QObjEfnAnim::SetCullNone)
			.def("SetLightsEnabled",	&QObjEfnAnim::SetLightsEnabled)
			.def("SetTexSphereMapping",	&QObjEfnAnim::SetTexSphereMapping)
			.def("SetTex2SphereMapping",&QObjEfnAnim::SetTex2SphereMapping)
			.def("SetTexBlendFactor",	&QObjEfnAnim::SetTexBlendFactor)
			.def("SetSphereMappingByFactorAlpha", &QObjEfnAnim::SetSphereMappingByFactorAlpha)
			.def("SetBillBoard",		&QObjEfnAnim::SetBillBoard)
			.def("SetUseColorFromCT",	&QObjEfnAnim::SetUseColorFromCT)
			.def("GetPathLength",		&QObjEfnAnim::GetPathLength)
			.def("GetPathLast",			&QObjEfnAnim::GetPathLast)
			.def("GetNull",				&QObjEfnAnim::GetNull)
			.def("GetShadowCastType",	&QObjEfnAnim::GetShadowCastType)
			.def("SetShadowCastType",	&QObjEfnAnim::SetShadowCastType)
			.def("GetOffsetOnPath",		&QObjEfnAnim::GetOffsetOnPath)
			.def("SetVisibleDistance",	&QObjEfnAnim::SetVisibleDistance)
			.def("SetDoFramesScripts",	&QObjEfnAnim::SetDoFramesScripts)
			.def("GetDoFramesScripts",	&QObjEfnAnim::GetDoFramesScripts)
			.def("SetGenerateGoStopEvents",	&QObjEfnAnim::SetGenerateGoStopEvents)
			.def("GetGenerateGoStopEvents",	&QObjEfnAnim::GetGenerateGoStopEvents),

		class_<QObjBaseMove, QObjBase >("QObjBaseMove")
			.def("Go",					&QObjBaseMove::Go)
			.def("Go2",					&QObjBaseMove::Go2)
			.def("Go3",					&QObjBaseMove::Go3)
			.def("GoCurveFloor",		&QObjBaseMove::GoCurveFloor)
			.def("GoCurveFloorRun",		&QObjBaseMove::GoCurveFloorRun)
			.def("GoCurveFloorBack",	&QObjBaseMove::GoCurveFloorBack)
			.def("GoCurveFloorBackRun",	&QObjBaseMove::GoCurveFloorBackRun)
			.def("GoCurveFly",			&QObjBaseMove::GoCurveFly)
			.def("Stop",				&QObjBaseMove::Stop)
			.def("GetTarget",			&QObjBaseMove::GetTarget)
			.def("IsMoving",			&QObjBaseMove::IsMoving)
			.def("IsFly",				&QObjBaseMove::IsFly)
			.def("GetMoveType",			&QObjBaseMove::GetMoveType)
			.def("GetGoAnim",			&QObjBaseMove::GetGoAnim)
			.def("GetAngle",			&QObjBaseMove::GetAngle)
			.def("Path",				&QObjBaseMove::Path)
			.def("GetOffsetPos",		&QObjBaseMove::GetOffsetPos)
			.def("GetPathLength",		&QObjBaseMove::GetPathLength)
			.def("SetCollisionObjID",	&QObjBaseMove::SetCollisionObjID)
			.def("GetCollisedObjID",	&QObjBaseMove::GetCollisedObjID)
			.def("SetPauseWhileSeqID",	&QObjBaseMove::SetPauseWhileSeqID)
			.def("GetPauseWhileSeqID",	&QObjBaseMove::GetPauseWhileSeqID)
			.def("RotateToObj",			&QObjBaseMove::RotateToObj)
			.def("GoPause",				&QObjBaseMove::GoPause)
			.def("AddObjToCollisionList",		&QObjBaseMove::AddObjToCollisionList)
			.def("RemoveObjFromCollisionList",	&QObjBaseMove::RemoveObjFromCollisionList),

		class_<QObjMan, QObjBaseMove >("QObjMan"),
		class_<QObjCar, QObjBaseMove >("QObjCar")
			.def("SetWheelsFromEfn",	&QObjCar::SetWheelsFromEfn),
		class_<QObjBird, QObjBaseMove >("QObjBird"),
		class_<QObjFree, QObjBaseMove >("QObjFree")
	];

	return true;
}#define STRICT

#include "QMgrScripts.h"

#include "QEngine.h"

#include "QSubSysLog.h"
#include "QSubSysGraphics.h"
#include "QSubSysInput.h"

#include "QMgrScripts.h"
#include "QMgrMiniGames.h"
#include "QMgrStates.h"

#include "QMgrGC.h"
#include "QMgrResTextures.h"
#include "QMgrResEfnModels.h"
#include "QMgrResFonts.h"
#include "QMgrResMaterials.h"
#include "QMgrResSounds.h"
#include "QMgrResShaders.h"
#include "QMgrResFiles.h"

#include "QGameAll.h"

bool QMgrScripts::RegisterVariables3()
{
	using namespace luabind;

	module(luaVM)
	[
		class_<QMgrMiniGames>("QMgrMiniGames")
			.def("AddSystemMiniGames",	&QMgrMiniGames::AddSystemMiniGames)
			.def("AddGameMenu",			&QMgrMiniGames::AddGameMenu)
			.def("AddGameVideoPlayer",	&QMgrMiniGames::AddGameVideoPlayer)
			.def("AddGameVideo",		&QMgrMiniGames::AddGameVideo)
			.def("AddGameRelsov",		&QMgrMiniGames::AddGameRelsov)	
			.def("AddGameTir",			&QMgrMiniGames::AddGameTir)	
			.def("AddGameNuts",			&QMgrMiniGames::AddGameNuts)	
			.def("AddGameQuest",		&QMgrMiniGames::AddGameQuest)	
			.def("AddGameTest",			&QMgrMiniGames::AddGameTest)	
			.def("AddGameMtrlEditor",	&QMgrMiniGames::AddGameMtrlEditor)	
			.def("AddGameUral",			&QMgrMiniGames::AddGameUral)	
			.def("DelGame",				&QMgrMiniGames::DelGame)
			.def("DelGames",			&QMgrMiniGames::DelGames)
			.def("GetGameConsole",		&QMgrMiniGames::GetGameConsole)
			.def("GetGameMenu",			&QMgrMiniGames::GetGameMenu)
			.def("GetGameVideoPlayer",	&QMgrMiniGames::GetGameVideoPlayer)
			.def("GetGameVideo",		&QMgrMiniGames::GetGameVideo)
			.def("GetGameRelsov",		&QMgrMiniGames::GetGameRelsov)
			.def("GetGameTir",			&QMgrMiniGames::GetGameTir)
			.def("GetGameNuts",			&QMgrMiniGames::GetGameNuts)
			.def("GetGameQuest",		&QMgrMiniGames::GetGameQuest)
			.def("GetGameTest",			&QMgrMiniGames::GetGameTest)
			.def("GetGameMtrlEditor",	&QMgrMiniGames::GetGameMtrlEditor)
			.def("GetGameUral",			&QMgrMiniGames::GetGameUral)
			.def("GetGameStop",			&QMgrMiniGames::GetGameStop)
			.def("PauseAll",			&QMgrMiniGames::PauseAll)
			.def("ContinueAll",			&QMgrMiniGames::ContinueAll)
			.def("SetMoveEnabledAll",	&QMgrMiniGames::SetMoveEnabledAll)
			.def("SetRenderEnabledAll",	&QMgrMiniGames::SetRenderEnabledAll)
			.def("SetMoveEnabled",		&QMgrMiniGames::SetMoveEnabled)
			.def("SetRenderEnabled",	&QMgrMiniGames::SetRenderEnabled)
			.def("SetSubTitlesEnabled",	&QMgrMiniGames::SetSubTitlesEnabled)
			.def("GetSubTitlesEnabled",	&QMgrMiniGames::GetSubTitlesEnabled)

			.def("LogEnabled",			&QMgrMiniGames::LogEnabled),

		class_<QGameBase>("QGameBase")
			.def("Init",				&QGameBase::Init)
			.def("GetGameTime",			&QGameBase::GetGameTime)
			.def("GetGameElapsedTime",	&QGameBase::GetGameElapsedTime)
			.def("GetObjMgr",			&QGameBase::GetObjMgr)
			.def("GetTMgr",				&QGameBase::GetTMgr)
			.def("Pause",				&QGameBase::Pause)
			.def("Continue",			&QGameBase::Continue)
			.def("ZeroCam",				&QGameBase::ZeroCam)
			.def("SetCam",				&QGameBase::SetCam)
			.def("CamStr",				&QGameBase::CamStr)
			.def("Cam",					&QGameBase::Cam)

			.def("SetGameContolsCam",	&QGameBase::SetGameContolsCam)
			.def("GetGameContolsCam",	&QGameBase::GetGameContolsCam)

			.def("SetMouseSensivity",	&QGameBase::SetMouseSensivity)
			.def("OnMouseMoveDI",		&QGameBase::OnMouseMoveDI)

			.def("AddMsg",				&QGameBase::AddMsg)
			.def("AddMsg2",				&QGameBase::AddMsg2)
			.def("DelMsg",				&QGameBase::DelMsg)
			.def("ClearMsgs",			&QGameBase::ClearMsgs)
			
			.def("GetMouseRay",			&QGameBase::GetMouseRay)
			.def("GetLeftClickRay",		&QGameBase::GetLeftClickRay)
			.def("GetRightClickRay",	&QGameBase::GetRightClickRay)
			
			.def("GetOrthoMouseRay",	&QGameBase::GetOrthoMouseRay)
			.def("GetOrthoLeftClickRay",&QGameBase::GetOrthoLeftClickRay)
			.def("GetOrthoRightClickRay",&QGameBase::GetOrthoRightClickRay)
			
			.def("PlaySnd",				&QGameBase::PlaySnd)
			.def("PlaySnd2",			&QGameBase::PlaySnd2)
			.def("PlaySnd3",			&QGameBase::PlaySnd3)
			.def("PlaySnd4",			&QGameBase::PlaySnd4)
			.def("StopSnd",				&QGameBase::StopSnd),

		class_<QGameConsole, QGameBase >("QGameConsole"),

		class_<QGameMenu, QGameBase >("QGameMenu"),

		class_<QGameVideo, QGameBase >("QGameVideo"),

		class_<QGameVideoPlayer, QGameBase >("QGameVideoPlayer")
			.def("SetBackColor",		&QGameVideoPlayer::SetBackColor)
			.def("LoadVideo",			&QGameVideoPlayer::LoadVideo)
			.def("FreeVideo",			&QGameVideoPlayer::FreeVideo)
			.def("Rew",					&QGameVideoPlayer::Rew)
			.def("Fvd",					&QGameVideoPlayer::Fvd)
			.def("Play",				&QGameVideoPlayer::Play)
			.def("Stop",				&QGameVideoPlayer::Stop)
			.def("Stop",				&QGameVideoPlayer::PauseVideo)
			.def("StepFvd",				&QGameVideoPlayer::StepFvd)
			.def("StepBack",			&QGameVideoPlayer::StepBack)
			.def("IncSpeed",			&QGameVideoPlayer::IncSpeed)
			.def("DecSpeed",			&QGameVideoPlayer::DecSpeed)
			.def("SetSpeed",			&QGameVideoPlayer::SetSpeed)
			.def("GetSpeed",			&QGameVideoPlayer::GetSpeed)
			.def("MoveLeft",			&QGameVideoPlayer::MoveLeft)
			.def("MoveRight",			&QGameVideoPlayer::MoveRight)
			.def("MoveUp",				&QGameVideoPlayer::MoveUp)
			.def("MoveDown",			&QGameVideoPlayer::MoveDown)
			.def("HorizSizeInc",		&QGameVideoPlayer::HorizSizeInc)
			.def("HorizSizeDec",		&QGameVideoPlayer::HorizSizeDec)
			.def("VertSizeInc",			&QGameVideoPlayer::VertSizeInc)
			.def("VertSizeDec",			&QGameVideoPlayer::VertSizeDec)
			.def("SetPos",				&QGameVideoPlayer::SetPos)
			.def("GetPos",				&QGameVideoPlayer::GetPos)
			.def("GetLength",			&QGameVideoPlayer::GetLength),

		class_<QGameRelsov, QGameBase>("QGameRelsov")
			.def("SetClickedObj",		&QGameRelsov::SetClickedObj)
			.def("SetSens",				&QGameRelsov::SetSens),

		class_<QGameTir, QGameBase>("QGameTir")
			.def("Shoot",				&QGameTir::Shoot),

		class_<QGameNuts, QGameBase>("QGameNuts")
			.def("SetClickedObj",		&QGameNuts::SetClickedObj)
			.def("SetSens",				&QGameNuts::SetSens),

		class_<QGameQuest, QGameBase>("QGameQuest")
			.def("LoadScene",			&QGameQuest::LoadScene)
			.def("GetScene",			&QGameQuest::GetScene)
			.def("GetSceneID",			&QGameQuest::GetSceneID)

			.def("SetCam2",				&QGameQuest::SetCam2)
			.def("SetCamTarget",		&QGameQuest::SetCamTarget)
			.def("GetCamTarget",		&QGameQuest::GetCamTarget)
			.def("SetCamTargetPP",		&QGameQuest::SetCamTargetPP)
			.def("SetIntervalForSetCam",&QGameQuest::SetIntervalForSetCam)

			.def("SetCamAttachedToVideo3D",	&QGameQuest::SetCamAttachedToVideo3D)
			.def("GetBackTexPos",		&QGameQuest::GetBackTexPos)
			
			.def("Cam3DStr",			&QGameQuest::Cam3DStr)
			.def("CamFloorsStr",		&QGameQuest::CamFloorsStr)
			.def("CamActionsStr",		&QGameQuest::CamActionsStr)
			
			.def("SetForceCam",			&QGameQuest::SetForceCam),

		class_<QGameTest, QGameBase >("QGameTest"),
		class_<QGameMtrlEditor, QGameBase >("QGameMtrlEditor"),
		class_<QGameUral, QGameBase >("QGameUral"),

		class_<QGameStop, QGameBase >("QGameStop"),

		//-------------------------------------------------------------------------
		class_<QGameSceneBase>("QGameSceneBase")
			.def("LoadScene",			&QGameSceneBase::LoadScene)
			.def("FreeScene",			&QGameSceneBase::FreeScene)
			
			.def("GetSceneLoaded",		&QGameSceneBase::GetSceneLoaded)
			.def("GetLevelID",			&QGameSceneBase::GetLevelID)
			.def("GetSceneID",			&QGameSceneBase::GetSceneID)
			.def("GetPrevLevelID",		&QGameSceneBase::GetPrevLevelID)
			.def("GetPrevSceneID",		&QGameSceneBase::GetPrevSceneID)
			
			.def("LogEnabled",			&QGameSceneBase::LogEnabled),

		class_<QGameQuestScene, QGameSceneBase>("QGameQuestScene")
			.def("GetNull",				&QGameQuestScene::GetNull)
			.def("GetText",				&QGameQuestScene::GetText)
			.def("GetCurve",			&QGameQuestScene::GetCurve)
			.def("GetCellSize",			&QGameQuestScene::GetCellSize)
			.def("GetCamForPos",		&QGameQuestScene::GetCamForPos)
	];

	return true;
}#define STRICT

#include "QMgrScripts.h"

#include "QEngine.h"

#include "QSubSysLog.h"
#include "QSubSysGraphics.h"
#include "QSubSysInput.h"

#include "QMgrScripts.h"
#include "QMgrMiniGames.h"
#include "QMgrStates.h"

#include "QMgrGC.h"
#include "QMgrResTextures.h"
#include "QMgrResEfnModels.h"
#include "QMgrResFonts.h"
#include "QMgrResMaterials.h"
#include "QMgrResSounds.h"
#include "QMgrResShaders.h"
#include "QMgrResFiles.h"

#include "QObjBaseMove.h"
#include "QObjParticlesMgr.h"

bool QMgrScripts::RegisterVariablesEnd()
{
	SetGlobal( "g_Engine",		g_Engine );//<return>QEngine</return>
	SetGlobal( "g_Graph",		g_Graph );//<return>QSubSysGraphics</return>
	SetGlobal( "g_Input",		g_Input );//<return>QSubsysInput</return>
	SetGlobal( "g_Mouse",		g_Mouse );//<return>SMOUSEINPUT</return>
	SetGlobal( "g_MouseDI",		g_MouseDI );//<return>QSubsysInput</return>
	SetGlobal( "g_Log",			g_Log );//<return>QSubSysLog</return>
	SetGlobal( "g_StatesMgr",	g_StatesMgr );//<return>QMgrStates</return>
	SetGlobal( "g_EfnMgr",		g_EfnMgr );//<return>QMgrResTextures</return>
	SetGlobal( "g_TexMgr",		g_TexMgr );//<return>QMgrResTextures</return>
	SetGlobal( "g_MtrlMgr",		g_MtrlMgr );//<return>QMgrResMaterials</return>
	SetGlobal( "g_FileMgr",		g_FileMgr );//<return>QMgrResFiles</return>
	SetGlobal( "g_FontMgr",		g_FontMgr );//<return>QMgrResFonts</return>
	SetGlobal( "g_EfnMgr",		g_EfnMgr );//<return>QMgrResEfnModels</return>
	SetGlobal( "g_SndMgr",		g_SndMgr );//<return>QMgrResSounds</return>
	SetGlobal( "g_Script",		g_Script );//<return>QMgrResScripts</return>
	SetGlobal( "g_GamesMgr",	g_GamesMgr );//<return>QMgrMiniGames</return>
	SetGlobal( "g_Rnd",			g_Rnd );//<return>QMgrMiniGames</return>

	ExecScript( "ALIGN_LEFT			= " + int2str( ALIGN_LEFT ) + ";" );
	ExecScript( "ALIGN_CENTER		= " + int2str( ALIGN_CENTER ) + ";" );
	ExecScript( "ALIGN_RIGHT		= " + int2str( ALIGN_RIGHT ) + ";" );

	ExecScript( "VALIGN_TOP			= " + int2str( VALIGN_TOP ) + ";" );
	ExecScript( "VALIGN_MIDDLE		= " + int2str( VALIGN_MIDDLE ) + ";" );
	ExecScript( "VALIGN_BOTTOM		= " + int2str( VALIGN_BOTTOM ) + ";" );

	ExecScript( "OVT_NONE			= " + int2str( OVT_NONE ) + ";" );
	ExecScript( "OVT_CYLINDER		= " + int2str( OVT_CYLINDER ) + ";" );
	ExecScript( "OVT_OOBB			= " + int2str( OVT_OOBB ) + ";" );
	ExecScript( "OVT_SELF			= " + int2str( OVT_SELF ) + ";" );

	ExecScript( "MT_STOP 			= " + int2str( MT_STOP ) + ";" );
	ExecScript( "MT_GO				= " + int2str( MT_GO ) + ";" );
	ExecScript( "MT_GO_PAUSE		= " + int2str( MT_GO_PAUSE ) + ";" );
	ExecScript( "MT_GOCURVE  		= " + int2str( MT_GOCURVE ) + ";" );
	ExecScript( "MT_GOCURVE_PAUSE	= " + int2str( MT_GOCURVE_PAUSE ) + ";" );

	ExecScript( "AT_CONST			= " + int2str( AT_CONST ) + ";" );
	ExecScript( "AT_GROW			= " + int2str( AT_GROW ) + ";" );
	ExecScript( "AT_FADE  			= " + int2str( AT_FADE ) + ";" );
	ExecScript( "AT_GROW_FADE		= " + int2str( AT_GROW_FADE ) + ";" );

	ExecScript( "D3DFOG_NONE		= " + int2str( D3DFOG_NONE ) + ";" );
	ExecScript( "D3DFOG_EXP			= " + int2str( D3DFOG_EXP ) + ";" );
	ExecScript( "D3DFOG_EXP2  		= " + int2str( D3DFOG_EXP2 ) + ";" );
	ExecScript( "D3DFOG_LINEAR		= " + int2str( D3DFOG_LINEAR ) + ";" );

	ExecScript( "TS_OBJ_POS 				= " + int2str( TS_OBJ_POS ) + ";" );
	ExecScript( "TS_T0SPHERE_CENTER			= " + int2str( TS_T0SPHERE_CENTER ) + ";" );
	ExecScript( "TS_T0SPHERE_SURFACE		= " + int2str( TS_T0SPHERE_SURFACE ) + ";" );
	ExecScript( "TS_T0OOBB_SPHERE_CENTER	= " + int2str( TS_T0OOBB_SPHERE_CENTER ) + ";" );
	ExecScript( "TS_T0OOBB_SPHERE_SURFACE	= " + int2str( TS_T0OOBB_SPHERE_SURFACE ) + ";" );
	ExecScript( "TS_TSPHERE_CENTER			= " + int2str( TS_TSPHERE_CENTER ) + ";" );
	ExecScript( "TS_TSPHERE_SURFACE			= " + int2str( TS_TSPHERE_SURFACE ) + ";" );
	ExecScript( "TS_TOOBB_SPHERE_CENTER		= " + int2str( TS_TOOBB_SPHERE_CENTER ) + ";" );
	ExecScript( "TS_TOOBB_SPHERE_SURFACE	= " + int2str( TS_TOOBB_SPHERE_SURFACE ) + ";" );

	ExecFile( "QEngine_RegisterWraper.lua" );

	return true;
}