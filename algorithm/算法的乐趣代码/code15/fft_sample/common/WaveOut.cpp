// WaveOut.cpp: implementation of the CWaveOut class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WaveOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	
	switch(uMsg) {
	case MM_WOM_DONE:
		WAVEHDR* pWaveHdr = ( (WAVEHDR*)dwParam1 );
		CWaveOut* pWaveOut = (CWaveOut*)(pWaveHdr->dwUser);

		if (pWaveHdr && hwo && pWaveOut) {
			if ((pWaveHdr->dwFlags & WHDR_DONE) == WHDR_DONE) {
                PLAY_CALLBACK_FUNC FuncPtr = pWaveOut->GetPlayCallback();
                if(FuncPtr != NULL) {
                    WAVEFORMATEX fmt = pWaveOut->GetWaveObject().GetFormat();
                    FuncPtr(pWaveHdr->lpData, pWaveHdr->dwBufferLength, fmt.nBlockAlign, fmt.nChannels);
                }
				pWaveHdr->dwFlags = 0;
				if ( pWaveOut->IsError(waveOutUnprepareHeader(hwo, pWaveHdr, sizeof(WAVEHDR))) ) {
					break;
				}
				pWaveHdr->lpData = NULL;
			}
			if ( ! pWaveOut->ResetRequired(pWaveOut) ) {
				if ( !pWaveOut->AddNewHeader(hwo) ) {
					break;
				}
			}
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
CWaveOut::CWaveOut(const CWave& aWave, const CWaveDevice& aDevice) : m_wave(aWave), m_waveDevice(aDevice), \
	m_hWaveOut(0), m_nIndexWaveHdr(NUMWAVEOUTHDR - 1), m_dwStartPos(0L), m_dwWaveOutBufferLength(WAVEOUT_BUFFER_SIZE), \
    m_playFunPtr(NULL), m_BufReadyFuncPtr(NULL)
{
}

//////////////////////////////////////////////////////////////////////
CWaveOut::CWaveOut() : m_hWaveOut(0), \
	m_dwStartPos(0L), m_dwWaveOutBufferLength(WAVEOUT_BUFFER_SIZE), m_playFunPtr(NULL), m_BufReadyFuncPtr(NULL)
{
}

//////////////////////////////////////////////////////////////////////
CWaveOut::~CWaveOut()
{
	Close();
}

//////////////////////////////////////////////////////////////////////
// Initialisation
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void CWaveOut::ModifyWaveOutBufferLength(DWORD dwLength)
{
	ASSERT(dwLength > 1024);
	m_dwWaveOutBufferLength = dwLength;
}

//////////////////////////////////////////////////////////////////////
void CWaveOut::SetDevice(const CWaveDevice &aDevice)
{
	m_waveDevice = aDevice;
}

//////////////////////////////////////////////////////////////////////
void CWaveOut::SetWave(const CWave &aWave)
{
	m_wave = aWave;
}

//////////////////////////////////////////////////////////////////////
// Son
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
bool CWaveOut::Close()
{
	if (m_hWaveOut != NULL) {
		if ( !Stop() ) {
			return false;
		}
		if ( IsError( waveOutClose(m_hWaveOut)) ) {
			return false;
		}
		m_hWaveOut = 0;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CWaveOut::Continue()
{
	if (m_hWaveOut) {
		return !IsError( waveOutRestart(m_hWaveOut) );
	}
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CWaveOut::FullPlay(int nLoop, DWORD dwStart/*=-1*/, DWORD dwEnd/*=-1*/)
{
	DWORD oldBufferLength = m_dwWaveOutBufferLength;
	m_dwWaveOutBufferLength = m_wave.GetBufferLength();

	if ( IsError(waveOutReset(m_hWaveOut)) ) {
		return false;
	}
	m_dwStartPos = (dwStart == -1) ? 0L : dwStart * m_wave.GetFormat().nBlockAlign;
	m_dwEndPos = (dwEnd == -1) ? m_wave.GetBufferLength() : __min( m_wave.GetBufferLength(), dwEnd ) * m_wave.GetFormat().nBlockAlign;
	nLoop = (nLoop == -1) ? 0 : nLoop;
	m_nIndexWaveHdr = NUMWAVEOUTHDR - 1;

	if ( !AddFullHeader(m_hWaveOut, nLoop) ) {
		m_dwWaveOutBufferLength = oldBufferLength;
		return false;
	}

	m_dwWaveOutBufferLength = oldBufferLength;

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CWaveOut::Open()
{
	return !IsError( waveOutOpen(&m_hWaveOut, m_waveDevice.GetDevice(), &m_wave.GetFormat(), (DWORD)waveOutProc, NULL, CALLBACK_FUNCTION) );
}

//////////////////////////////////////////////////////////////////////
bool CWaveOut::Pause()
{
	if (m_hWaveOut) {
		return !IsError( waveOutPause(m_hWaveOut) );
	}
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CWaveOut::Play(DWORD dwStart/*=-1*/, DWORD dwEnd/*=-1*/)
{
	if ( !Stop() ) {
		return false;
	}
	m_dwStartPos = (dwStart == -1) ? 0L : dwStart * m_wave.GetFormat().nBlockAlign;
	m_dwEndPos = (dwEnd == -1) ? m_wave.GetBufferLength() : __min( m_wave.GetBufferLength(), dwEnd ) * m_wave.GetFormat().nBlockAlign;
	m_nIndexWaveHdr = NUMWAVEOUTHDR - 1;
	for (int i = 0; i < NUMWAVEOUTHDR; i++) {
		if ( !AddNewHeader(m_hWaveOut) ) {
			return false;
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CWaveOut::Stop()
{
	if (m_hWaveOut != NULL) {
		m_dwStartPos = m_dwEndPos;
		if ( IsError(waveOutReset(m_hWaveOut)) ) {
			return false;
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CWaveOut::AddFullHeader(HWAVEOUT hwo, int nLoop)
{
	if ( GetBufferLength() == 0) {
		return false;
	}
	m_nIndexWaveHdr = (m_nIndexWaveHdr == NUMWAVEOUTHDR - 1) ? 0 : m_nIndexWaveHdr + 1;
	m_tagWaveHdr[m_nIndexWaveHdr].lpData = (char*)m_wave.GetBuffer() + m_dwStartPos;
	m_tagWaveHdr[m_nIndexWaveHdr].dwBufferLength = m_dwEndPos - m_dwStartPos;
	m_tagWaveHdr[m_nIndexWaveHdr].dwFlags = WHDR_BEGINLOOP | WHDR_ENDLOOP;
	m_tagWaveHdr[m_nIndexWaveHdr].dwLoops = nLoop;
	m_tagWaveHdr[m_nIndexWaveHdr].dwUser = (DWORD)(void*)this;
	if ( IsError(waveOutPrepareHeader(hwo, &m_tagWaveHdr[m_nIndexWaveHdr], sizeof(WAVEHDR))) ) {
		return false;
	}
	if ( IsError(waveOutWrite(hwo, &m_tagWaveHdr[m_nIndexWaveHdr], sizeof(WAVEHDR))) ) {
		waveOutUnprepareHeader( hwo, &m_tagWaveHdr[m_nIndexWaveHdr], sizeof(WAVEHDR) );
		m_tagWaveHdr[m_nIndexWaveHdr].lpData = NULL;
		m_tagWaveHdr[m_nIndexWaveHdr].dwBufferLength = 0;
		m_tagWaveHdr[m_nIndexWaveHdr].dwFlags = 0;
		m_tagWaveHdr[m_nIndexWaveHdr].dwUser = NULL;
		m_nIndexWaveHdr--;
		return false;
	}
	m_dwStartPos = m_dwEndPos - m_dwStartPos;
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CWaveOut::AddNewHeader(HWAVEOUT hwo)
{
	if ( GetBufferLength() == 0) {
		return false;
	}
	m_nIndexWaveHdr = (m_nIndexWaveHdr == NUMWAVEOUTHDR - 1) ? 0 : m_nIndexWaveHdr + 1;
	m_tagWaveHdr[m_nIndexWaveHdr].lpData = (char*)m_wave.GetBuffer() + m_dwStartPos;
	m_tagWaveHdr[m_nIndexWaveHdr].dwBufferLength = GetBufferLength();
	m_tagWaveHdr[m_nIndexWaveHdr].dwFlags = 0;
	m_tagWaveHdr[m_nIndexWaveHdr].dwUser = (DWORD)(void*)this;
    if(m_BufReadyFuncPtr != NULL)
    {
        WAVEFORMATEX fmt = m_wave.GetFormat();
        m_BufReadyFuncPtr(m_tagWaveHdr[m_nIndexWaveHdr].lpData, m_tagWaveHdr[m_nIndexWaveHdr].dwBufferLength,
            fmt.nSamplesPerSec, fmt.nBlockAlign, fmt.nChannels);
    }
	if ( IsError(waveOutPrepareHeader(hwo, &m_tagWaveHdr[m_nIndexWaveHdr], sizeof(WAVEHDR))) ) {
		return false;
	}
	if ( IsError(waveOutWrite(hwo, &m_tagWaveHdr[m_nIndexWaveHdr], sizeof(WAVEHDR))) ) { 
		waveOutUnprepareHeader( hwo, &m_tagWaveHdr[m_nIndexWaveHdr], sizeof(WAVEHDR) );
		m_tagWaveHdr[m_nIndexWaveHdr].lpData = NULL;
		m_tagWaveHdr[m_nIndexWaveHdr].dwBufferLength = 0;
		m_tagWaveHdr[m_nIndexWaveHdr].dwFlags = 0;
		m_tagWaveHdr[m_nIndexWaveHdr].dwUser = NULL;
		m_nIndexWaveHdr--;
		return false;
	}
	m_dwStartPos += GetBufferLength();
	return true;
}

//////////////////////////////////////////////////////////////////////
// GET
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
DWORD CWaveOut::GetBufferLength()
{
	return __min( m_dwWaveOutBufferLength, m_dwEndPos - m_dwStartPos );
}

//////////////////////////////////////////////////////////////////////
CString CWaveOut::GetError() const
{
	if (m_nError != MMSYSERR_NOERROR) {
		TCHAR szText[MAXERRORLENGTH + 1];
		if ( waveOutGetErrorText(m_nError, szText, MAXERRORLENGTH) == MMSYSERR_NOERROR ) {
			return szText;
		}
	}
	return "";
}

//////////////////////////////////////////////////////////////////////
DWORD CWaveOut::GetPosition()
{
	if (m_hWaveOut) {
		MMTIME mmt;
		mmt.wType = TIME_SAMPLES;
		if ( IsError(waveOutGetPosition(m_hWaveOut, &mmt, sizeof(MMTIME))) ) {
			return -1;
		}
		else {
			return mmt.u.sample;
		}
	}
	return -1;
}

//////////////////////////////////////////////////////////////////////
bool CWaveOut::IsError(MMRESULT nResult)
{
	m_nError = nResult;
	return (m_nError != MMSYSERR_NOERROR);
}

//////////////////////////////////////////////////////////////////////
bool CWaveOut::IsPlaying()
{
	bool bResult = false;
	if (m_nIndexWaveHdr > -1 && m_tagWaveHdr[m_nIndexWaveHdr].dwFlags != 0) {
		bResult |= !((m_tagWaveHdr[m_nIndexWaveHdr].dwFlags & WHDR_DONE) == WHDR_DONE);
	}
	return bResult;
}

//////////////////////////////////////////////////////////////////////
bool CWaveOut::ResetRequired(CWaveOut* pWaveOut)
{
	return (pWaveOut->m_dwStartPos >= pWaveOut->m_dwEndPos);
}
