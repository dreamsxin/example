// Wave.cpp: implementation of the CWave class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Wave.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(CWave, CObject, 1)

//////////////////////////////////////////////////////////////////////
CWave::CWave()
{
	ZeroMemory((void*)&m_pcmWaveFormat, sizeof(m_pcmWaveFormat));
	m_pcmWaveFormat.wFormatTag = 1;
}

//////////////////////////////////////////////////////////////////////
CWave::CWave(const CWave &copy)
{
	m_pcmWaveFormat = copy.GetFormat();
	m_buffer.SetNumSamples( copy.GetNumSamples(), copy.GetFormat().nBlockAlign ) ;
	m_buffer.CopyBuffer( copy.GetBuffer(), copy.GetNumSamples(), copy.GetFormat().nBlockAlign );
}

//////////////////////////////////////////////////////////////////////
CWave& CWave::operator =(const CWave& wave)
{
	if (&wave != this) {
		m_pcmWaveFormat = wave.GetFormat();
		m_buffer.SetNumSamples( wave.GetNumSamples(), wave.GetFormat().nBlockAlign );
		m_buffer.CopyBuffer( wave.GetBuffer(), wave.GetNumSamples(), wave.GetFormat().nBlockAlign );
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////
CWave::~CWave()
{
}

//////////////////////////////////////////////////////////////////////
void CWave::BuildFormat(WORD nChannels, DWORD nFrequency, WORD nBits)
{
	m_pcmWaveFormat.wFormatTag = WAVE_FORMAT_PCM;
	m_pcmWaveFormat.nChannels = nChannels;
	m_pcmWaveFormat.nSamplesPerSec = nFrequency;
	m_pcmWaveFormat.nAvgBytesPerSec = nFrequency * nChannels * nBits / 8;
	m_pcmWaveFormat.nBlockAlign = nChannels * nBits / 8;
	m_pcmWaveFormat.wBitsPerSample = nBits;
	m_buffer.SetNumSamples(0L, m_pcmWaveFormat.nBlockAlign);
}	

//////////////////////////////////////////////////////////////////////
inline WAVEFORMATEX CWave::GetFormat() const
{
	return m_pcmWaveFormat;
}

//////////////////////////////////////////////////////////////////////
void CWave::Serialize( CArchive& archive )
{
	CFile* f = archive.GetFile();
	if (archive.IsLoading())
		Load(f);
	else
		Save(f);
}

//////////////////////////////////////////////////////////////////////
void CWave::Load(const CString &strFile)
{
	CFile f(strFile, CFile::modeRead);
	Load(&f);
	f.Close();
}

//////////////////////////////////////////////////////////////////////
void CWave::Load(CFile *f)
{
	char szTmp[10];
	WAVEFORMATEX pcmWaveFormat;
	ZeroMemory(szTmp, 10 * sizeof(char));
	f->Read(szTmp, 4 * sizeof(char)) ;
	if (strncmp(szTmp, _T("RIFF"), 4) != 0) 
		::AfxThrowFileException(CFileException::invalidFile, -1, f->GetFileName());
	DWORD dwFileSize/* = m_buffer.GetNumSamples() * m_pcmWaveFormat.nBlockAlign + 36*/ ;
	f->Read(&dwFileSize, sizeof(dwFileSize)) ;
	ZeroMemory(szTmp, 10 * sizeof(char));
	f->Read(szTmp, 8 * sizeof(char)) ;
	if (strncmp(szTmp, _T("WAVEfmt "), 8) != 0) 
		::AfxThrowFileException(CFileException::invalidFile, -1, f->GetFileName());
	DWORD dwFmtSize /*= 16L*/;
	f->Read(&dwFmtSize, sizeof(dwFmtSize)) ;
	f->Read(&pcmWaveFormat.wFormatTag, sizeof(pcmWaveFormat.wFormatTag)) ;
	f->Read(&pcmWaveFormat.nChannels, sizeof(pcmWaveFormat.nChannels)) ;
	f->Read(&pcmWaveFormat.nSamplesPerSec, sizeof(pcmWaveFormat.nSamplesPerSec)) ;
	f->Read(&pcmWaveFormat.nAvgBytesPerSec, sizeof(pcmWaveFormat.nAvgBytesPerSec)) ;
	f->Read(&pcmWaveFormat.nBlockAlign, sizeof(pcmWaveFormat.nBlockAlign)) ;
	f->Read(&pcmWaveFormat.wBitsPerSample, sizeof(pcmWaveFormat.wBitsPerSample)) ;
	DWORD dwNum = 0;
    do
    {
        f->Seek(dwNum, CFile::current);
	    ZeroMemory(szTmp, 10 * sizeof(char));
	    f->Read(szTmp, 4 * sizeof(char)) ;
	    f->Read(&dwNum, sizeof(dwNum)) ;
    }while(strncmp(szTmp, _T("data"), 4) != 0);
	m_pcmWaveFormat = pcmWaveFormat;
	m_buffer.SetNumSamples(dwNum / pcmWaveFormat.nBlockAlign, pcmWaveFormat.nBlockAlign);
	f->Read(m_buffer.GetBuffer(), dwNum) ;
}

//////////////////////////////////////////////////////////////////////
void CWave::Save(const CString &strFile)
{
	CFile f(strFile, CFile::modeCreate | CFile::modeWrite);
	Save(&f);
	f.Close();
}

//////////////////////////////////////////////////////////////////////
void CWave::Save(CFile *f)
{
	ASSERT( m_buffer.GetNumSamples() > 0 );

	f->Write("RIFF", 4) ;
	DWORD dwFileSize = m_buffer.GetNumSamples() * m_pcmWaveFormat.nBlockAlign + 36 ;
	f->Write(&dwFileSize, sizeof(dwFileSize)) ;
	f->Write("WAVEfmt ", 8) ;
	DWORD dwFmtSize = 16L;
	f->Write(&dwFmtSize, sizeof(dwFmtSize)) ;
	f->Write(&m_pcmWaveFormat.wFormatTag, sizeof(m_pcmWaveFormat.wFormatTag)) ;
	f->Write(&m_pcmWaveFormat.nChannels, sizeof(m_pcmWaveFormat.nChannels)) ;
	f->Write(&m_pcmWaveFormat.nSamplesPerSec, sizeof(m_pcmWaveFormat.nSamplesPerSec)) ;
	f->Write(&m_pcmWaveFormat.nAvgBytesPerSec, sizeof(m_pcmWaveFormat.nAvgBytesPerSec)) ;
	f->Write(&m_pcmWaveFormat.nBlockAlign, sizeof(m_pcmWaveFormat.nBlockAlign)) ;
	f->Write(&m_pcmWaveFormat.wBitsPerSample, sizeof(m_pcmWaveFormat.wBitsPerSample)) ;
	f->Write("data", 4) ;
	DWORD dwNum = m_buffer.GetNumSamples() * m_pcmWaveFormat.nBlockAlign;
	f->Write(&dwNum, sizeof(dwNum)) ;
	f->Write(m_buffer.GetBuffer(), dwNum) ;
}

//////////////////////////////////////////////////////////////////////
void* CWave::GetBuffer() const
{
	return m_buffer.GetBuffer();
}

//////////////////////////////////////////////////////////////////////
DWORD CWave::GetNumSamples() const
{
	return m_buffer.GetNumSamples();
}

//////////////////////////////////////////////////////////////////////
DWORD CWave::GetBufferLength() const
{
	return ( m_buffer.GetNumSamples() * m_pcmWaveFormat.nBlockAlign );
}

//////////////////////////////////////////////////////////////////////
void CWave::SetBuffer(void* pBuffer, DWORD dwNumSample, bool bCopy)
{
	ASSERT(pBuffer);
	ASSERT(dwNumSample > 0);
	ASSERT(m_pcmWaveFormat.nBlockAlign > 0);

	if (bCopy) {
		m_buffer.CopyBuffer(pBuffer, dwNumSample, m_pcmWaveFormat.nBlockAlign);
	}
	else {
		m_buffer.SetBuffer(pBuffer, dwNumSample, m_pcmWaveFormat.nBlockAlign);
	}
}
