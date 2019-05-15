// Wave.h: interface for the CWave class.
//
//////////////////////////////////////////////////////////////////////

/*/
	written for E-MED INNOVATIONS INC. by E-MED INNOVATIONS INC.
						08/2001
				http://www.e-medsounds.com
				mailto://pcanthelou@e-medsounds.com
/*/

#if !defined(AFX_WAVE_H__54B7993A_3F57_4AB0_9CCD_A55C2A1B2871__INCLUDED_)
#define AFX_WAVE_H__54B7993A_3F57_4AB0_9CCD_A55C2A1B2871__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <mmsystem.h>
#include <mmreg.h>
#include "WaveBuffer.h"	// Added by ClassView

class CWave : public CObject  
{
	DECLARE_SERIAL(CWave)
public:
	void SetBuffer(void* pBuffer, DWORD dwNumSamples, bool bCopy = false);
	void Load(const CString& strFile);
	void Save(const CString& strFile);
	DWORD GetBufferLength() const;
	DWORD GetNumSamples() const;
	void* GetBuffer() const;
	void Save(CFile* f);
	void Load(CFile* f);
	WAVEFORMATEX GetFormat() const;
	void BuildFormat(WORD nChannels, DWORD nFrequency, WORD nBits);
	CWave();
	CWave(const CWave& copy);
	CWave& operator=(const CWave& wave);
	virtual ~CWave();
	virtual void Serialize( CArchive& archive );
private:
	CWaveBuffer m_buffer;
	WAVEFORMATEX m_pcmWaveFormat;
};

#endif // !defined(AFX_WAVE_H__54B7993A_3F57_4AB0_9CCD_A55C2A1B2871__INCLUDED_)
