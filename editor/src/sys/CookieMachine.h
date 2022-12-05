#include "resource.h"

class CCookieMachineApp : public CWinApp
{
public:
	CCookieMachineApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCookieMachineApp)
	public:
	virtual BOOL InitInstance();
//	virtual BOOL OnIdle( LONG );
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCookieMachineApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
};