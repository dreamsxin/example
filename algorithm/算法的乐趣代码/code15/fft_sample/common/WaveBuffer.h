// WaveBuffer.h: interface for the CWaveBuffer class.
//
//////////////////////////////////////////////////////////////////////

/*/
	written for E-MED INNOVATIONS INC. by E-MED INNOVATIONS INC.
						08/2001
				http://www.e-medsounds.com
				mailto://pcanthelou@e-medsounds.com
/*/

#if !defined(AFX_WAVEBUFFER_H__09A5D50C_B80E_43C7_B37F_D8D5D1B3026B__INCLUDED_)
#define AFX_WAVEBUFFER_H__09A5D50C_B80E_43C7_B37F_D8D5D1B3026B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWaveBuffer  
{
public:
	int GetSampleSize() const;
	void SetBuffer(void* pBuffer, DWORD dwNumSamples, int nSize);
	void SetNumSamples(DWORD dwNumSamples, int nSize = sizeof(short));
	void CopyBuffer(void* pBuffer, DWORD dwNumSamples, int nSize = sizeof(short));
	DWORD GetNumSamples() const;
	void* GetBuffer() const;
	CWaveBuffer();
	virtual ~CWaveBuffer();

private:
	int m_nSampleSize;
	void* m_pBuffer;
	DWORD m_dwNum;
};

#endif // !defined(AFX_WAVEBUFFER_H__09A5D50C_B80E_43C7_B37F_D8D5D1B3026B__INCLUDED_)
