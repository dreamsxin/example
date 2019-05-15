// WaveInterface.h: interface for the CWaveInterface class.
//
//////////////////////////////////////////////////////////////////////

/*/
	written for E-MED INNOVATIONS INC. by E-MED INNOVATIONS INC.
						08/2001
				http://www.e-medsounds.com
				mailto://pcanthelou@e-medsounds.com
/*/

#if !defined(AFX_WAVEINTERFACE_H__E681E7B3_7510_457E_BCD3_3BEF4B2777C5__INCLUDED_)
#define AFX_WAVEINTERFACE_H__E681E7B3_7510_457E_BCD3_3BEF4B2777C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWaveInterface  
{
public:
	static CString GetWaveInName(UINT nIndex);
	static UINT GetWaveInCount();
	static CString GetWaveOutName(UINT nIndex);
	static UINT GetWaveOutCount();
};

#endif // !defined(AFX_WAVEINTERFACE_H__E681E7B3_7510_457E_BCD3_3BEF4B2777C5__INCLUDED_)
