// WaveOut.h: interface for the CWaveOut class.
//
//////////////////////////////////////////////////////////////////////

/*/
	written for E-MED INNOVATIONS INC. by E-MED INNOVATIONS INC.
						08/2001
				http://www.e-medsounds.com
				mailto://pcanthelou@e-medsounds.com
/*/

#if !defined(AFX_WAVEOUT_H__FA0341BE_5A5D_40C5_80CD_ADCEDC734DEB__INCLUDED_)
#define AFX_WAVEOUT_H__FA0341BE_5A5D_40C5_80CD_ADCEDC734DEB__INCLUDED_

#include "Wave.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
#include "Wave.h"
#include "WaveDevice.h"

typedef void (*PLAY_CALLBACK_FUNC)(PVOID pData, int dataLen, int bytePerSample, int channel);
typedef void (*PLAY_BUFFER_READY_FUNC)(PVOID pData, int dataLen, int sampleRate, int bytePerSample, int channel);

//////////////////////////////////////////////////////////////////////
void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);

//////////////////////////////////////////////////////////////////////
#ifdef WAVE_OUT_BUFFER_SIZE
#undef WAVE_OUT_BUFFER_SIZE
#endif
#define WAVEOUT_BUFFER_SIZE 4096

#define NUMWAVEOUTHDR 4
#define INFINITE_LOOP INT_MAX

//////////////////////////////////////////////////////////////////////
class CWaveOut
{
	friend void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
public:
	CString GetError() const;
	DWORD GetPosition();
	bool IsPlaying();
	
	bool Close();
	bool Continue();
	bool FullPlay(int nLoop = -1, DWORD dwStart = -1, DWORD dwEnd = -1);
	bool Open();
	bool Pause();
	bool Play(DWORD dwStart = -1, DWORD dwEnd = -1);
	bool Stop();

	void ModifyWaveOutBufferLength(DWORD dwLength);
	void SetDevice(const CWaveDevice& aDevice);
	void SetWave(const CWave& aWave);
    CWave& GetWaveObject() { return m_wave; }
    void SetPlayCallback(PLAY_CALLBACK_FUNC funPtr) { m_playFunPtr = funPtr; }
    void SetBufferReadyFunc(PLAY_BUFFER_READY_FUNC funPtr) { m_BufReadyFuncPtr = funPtr; }
    PLAY_CALLBACK_FUNC GetPlayCallback() { return m_playFunPtr; };

	CWaveOut();
	CWaveOut(const CWave& aWave, const CWaveDevice& aDevice);
	virtual ~CWaveOut();
private:
	bool AddFullHeader(HWAVEOUT hwo, int nLoop);
	bool AddNewHeader(HWAVEOUT hwo);
	DWORD GetBufferLength();
	bool IsError(MMRESULT nResult);
	bool ResetRequired(CWaveOut* pWaveOut);
private:
    PLAY_CALLBACK_FUNC m_playFunPtr;
    PLAY_BUFFER_READY_FUNC m_BufReadyFuncPtr;
	DWORD m_dwEndPos;
	DWORD m_dwStartPos;
	DWORD m_dwWaveOutBufferLength;
	HWAVEOUT m_hWaveOut;
	UINT m_nError;
	int m_nIndexWaveHdr;
	WAVEHDR m_tagWaveHdr[NUMWAVEOUTHDR];
	CWave m_wave;
	CWaveDevice m_waveDevice;
};

#endif // !defined(AFX_WAVEOUT_H__FA0341BE_5A5D_40C5_80CD_ADCEDC734DEB__INCLUDED_)
