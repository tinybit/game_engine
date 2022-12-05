#include "binkinf.h"

int GetHigherPow2( int a )
{
	int i = 1;
	for(; i < a; i *= 2 ) {}
	return i;
}

bool GetBinkInfo( const std::string& sBinkID, SBINKINF* pInf )
{
	if ( !pInf )
		return false;

	HBINK pBink = NULL;

	pBink = BinkOpen( sBinkID.c_str(), BINKALPHA );

	if ( !pBink )
		return false;

	pInf->iWidth		= pBink->Width;
	pInf->iHeight		= pBink->Height;
	pInf->iWidthPow2		= GetHigherPow2( pBink->Width );
	pInf->iHeightPow2		= GetHigherPow2( pBink->Height );
	pInf->bHaveAlpha	= (pBink->OpenFlags & BINKALPHA) != 0;
	pInf->iFramesCount	= pBink->Frames;
	pInf->fFrameRate	= (float)pBink->FrameRate;
	if ( pBink->FrameRateDiv != 0 )
		pInf->fFrameRate /= pBink->FrameRateDiv;

	pInf->iPixelCount	= pInf->iWidthPow2 * pInf->iHeightPow2;

	BinkClose( pBink );

	return true;
}