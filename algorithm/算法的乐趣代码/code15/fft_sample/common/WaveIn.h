// WaveIn.h: interface for the CWaveIn class.
//
//////////////////////////////////////////////////////////////////////

/*/
	written for E-MED INNOVATIONS INC. by E-MED INNOVATIONS INC.
						08/2001
				http://www.e-medsounds.com
				mailto://pcanthelou@e-medsounds.com
/*/

#if !defined(AFX_WAVEIN_H__2473839B_76B0_45EB_9F9A_386D27903BB1__INCLUDED_)
#define AFX_WAVEIN_H__2473839B_76B0_45EB_9F9A_386D27903BB1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
#include "Wave.h"
#include "WaveDevice.h"

//////////////////////////////////////////////////////////////////////
void CALLBACK waveInProc(HWAVEIN hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);

//////////////////////////////////////////////////////////////////////
#ifdef WAVE_IN_BUFFER_SIZE
#undef WAVE_IN_BUFFER_SIZE
#endif
#define WAVEIN_BUFFER_SIZE 4096

#define NUMWAVEINHDR 2

//////////////////////////////////////////////////////////////////////
class CWaveIn  
{
	friend void CALLBACK waveInProc(HWAVEIN hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
public:
	CString GetError() const;
	DWORD GetPosition();
	bool IsRecording();
	CWave MakeWave();

	bool Close();
	bool Continue();
	bool Open();
	bool Pause();
	bool Record(UINT nTaille = 4096);
	bool Stop();

	void SetDevice(const CWaveDevice& aDevice);
	void SetWaveFormat(WAVEFORMATEX tagFormat);

	CWaveIn();
	CWaveIn(WAVEFORMATEX tagFormat, const CWaveDevice& aDevice);
	virtual	~CWaveIn();
private:
	bool AddNewBuffer(WAVEHDR* pWaveHdr);
	bool AddNewHeader(HWAVEIN hwi);
	void FreeListOfBuffer();
	DWORD GetNumSamples();
	void FreeListOfHeader();
	void InitListOfHeader();
	bool IsError(MMRESULT nResult);
	bool ResetRequired(CWaveIn* pWaveIn);
private:
	bool m_bResetRequired;
	HWAVEIN	m_hWaveIn;
	CPtrList m_listOfBuffer;
	UINT m_nError;
	int	m_nIndexWaveHdr;
	UINT m_nBufferSize;
	WAVEHDR	m_tagWaveHdr[NUMWAVEINHDR];
	CWave m_wave;
	CWaveDevice m_waveDevice;
};

#endif // !defined(AFX_WAVEIN_H__2473839B_76B0_45EB_9F9A_386D27903BB1__INCLUDED_)
