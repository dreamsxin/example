// WaveInterface.cpp: implementation of the CWaveInterface class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WaveInterface.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include <mmsystem.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
UINT CWaveInterface::GetWaveInCount()
{
	return waveInGetNumDevs();	
}

//////////////////////////////////////////////////////////////////////
CString CWaveInterface::GetWaveInName(UINT nIndex)
{
	ASSERT(nIndex < GetWaveInCount());
	WAVEINCAPS tagCaps;
	switch (waveInGetDevCaps(nIndex, &tagCaps, sizeof(tagCaps))) {
	case MMSYSERR_NOERROR:
		return tagCaps.szPname;
		break;
	default:
		return "";
	}
}

//////////////////////////////////////////////////////////////////////
UINT CWaveInterface::GetWaveOutCount()
{
	return waveOutGetNumDevs();	
}

//////////////////////////////////////////////////////////////////////
CString CWaveInterface::GetWaveOutName(UINT nIndex)
{
	ASSERT(nIndex < GetWaveOutCount());
	WAVEOUTCAPS tagCaps;
	switch (waveOutGetDevCaps(nIndex, &tagCaps, sizeof(tagCaps))) {
	case MMSYSERR_NOERROR:
		return tagCaps.szPname;
		break;
	default:
		return "";
	}
}
