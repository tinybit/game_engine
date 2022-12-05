#pragma once
#pragma warning(disable: 4238 4800)

#include "dxstdafx.h"
#include "stl_inc.h"

#include "UtilsDefines.h"
#include "UtilsConverts.h"

#include "QSubSysGraphics.h"
#include "QSubSysInput.h"


#include "IMgrGC.h"
#include "IMgrResFiles.h"
#include "IMgrResTextures.h"
#include "IMgrResBinks.h"
#include "IMgrResEfnModels.h"
#include "IMgrResFonts.h"

#include "QMgrScripts.h"
#include "QMgrTimers.h"
#include "QMgrMiniGames.h"
#include "QMgrResStates.h"
#include "QMgrResSounds.h"
#include "QMgrResShaders.h"

#define g_Engine QEngine::GetSingleton()

class QEngine : public Singleton <QEngine>	
{
public:
	QEngine();
	~QEngine();

	void					Init() { Init( L"QEngine" ); };
	void					Init( const std::wstring& sName );
	void					Kill();
	void					Exit();

	void					ZeroTime() { m_fTime = 0.0; };
	double					GetTime() { return m_fTime; };
	float					GetElapsedTime() { return m_fElapsedTime; };
	void					IncTime( float fElapsedTime ) { m_fElapsedTime = fElapsedTime * GetGameSpeed(); m_fTime += m_fElapsedTime; };

	void					SetGameSpeed( float fGameSpeed ) { m_fGameSpeed = ( fGameSpeed > 0 ? fGameSpeed * 0.01f : 1.0f ); };
	float					GetGameSpeed() { return m_fGameSpeed; };

	static HRESULT CALLBACK	OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc );
	static HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc );
	static void    CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime );
	static void    CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime );
	static void    CALLBACK OnLostDevice();
	static void    CALLBACK OnDestroyDevice();
	static LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing );
	static void    CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown  );
	static void	   CALLBACK MouseProc(	bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, bool bSideButton1Down,
								bool bSideButton2Down, INT nMouseWheelDelta, INT xPos, INT yPos);

	bool					bExitFlag;

private:

	double					m_fTime;
	float					m_fElapsedTime;

	float					m_fGameSpeed;

	std::wstring			m_sName;


	LPMGRGC					m_pGC;
	LPMGRRESTEXTURES		m_pTex;
	LPMGRRESEFNMODELS		m_pEfn;
	LPMGRRESBINKS			m_pBinks;
	LPMGRRESFONTS			m_pFonts;
	LPMGRRESFILES			m_pFileMan;
	QMgrResStates*			m_pStates;
	QMgrResSounds*			m_pSnd;
	QMgrResShaders*			m_pShad;
	QMgrScripts*			m_pScripts;
	QMgrTimers*				m_pTimers;
	QMgrMiniGames*			m_pGames;

	QSubSysGraphics*		m_pGraph;
	QSubSysInput*			m_pInput;

};