// WaveDevice.h: interface for the CWaveDevice class.
//
//////////////////////////////////////////////////////////////////////

/*/
	written for E-MED INNOVATIONS INC. by E-MED INNOVATIONS INC.
						08/2001
				http://www.e-medsounds.com
				mailto://pcanthelou@e-medsounds.com
/*/

#if !defined(AFX_WAVEDEVICE_H__FD02E292_FE4D_4B69_B268_428956261170__INCLUDED_)
#define AFX_WAVEDEVICE_H__FD02E292_FE4D_4B69_B268_428956261170__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Wave.h"

class CWaveDevice  
{
public:
	CWaveDevice(const CWaveDevice& copy);
	CWaveDevice(UINT nDevice = WAVE_MAPPER);
	bool IsOutputFormat(const CWave& wave);
	bool IsInputFormat(const CWave& wave);
	UINT GetDevice() const;
	virtual ~CWaveDevice();

private:
	UINT m_nDevice;
};

#endif // !defined(AFX_WAVEDEVICE_H__FD02E292_FE4D_4B69_B268_428956261170__INCLUDED_)
