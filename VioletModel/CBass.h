#pragma once
constexpr inline DWORD BassNull{};

class CBass
{
private:
	enum class MusicType
	{
		Invalid,
		Normal,
		Mod,
		Midi
	};

	MusicType m_eMusicType = MusicType::Invalid;
	DWORD m_hStream = BassNull;
	float m_fDefSpeed = 0.f;
    float m_fVolume = 1.f;
public:
	static int GetError(PCWSTR* ppszErr = nullptr) noexcept;

	static PCWSTR GetErrorMessage(int iErrCode) noexcept;

	EckInline static BOOL Initialize(
		int iDevice = -1,
		DWORD dwFreq = 44100,
		DWORD dwFlags = 0,
		HWND hWnd = nullptr) noexcept
	{
		return BASS_Init(iDevice, dwFreq, dwFlags, hWnd, nullptr);
	}

	EckInline static BOOL Free() noexcept
	{
		return BASS_Free();
	}

	EckInline static DWORD GetVersion() noexcept
	{
		return BASS_GetVersion();
	}

	EckInline static void VersionToString(DWORD dw, Eck_Out_buffer_ eck::CStringW& rs) noexcept
	{
		const WORD wHigh = HIWORD(dw);
		const WORD wLow = LOWORD(dw);
		rs.Format(L"%d.%d.%d.%d", (int)HIBYTE(wHigh), (int)LOBYTE(wHigh),
			(int)HIBYTE(wLow), (int)LOBYTE(wLow));
	}

	~CBass();

	DWORD Open(
		PCWSTR pszFile,
		DWORD dwFlagsHS = BASS_SAMPLE_FX | BASS_STREAM_DECODE,
		DWORD dwFlagsHM = BASS_SAMPLE_FX | BASS_STREAM_DECODE | BASS_MUSIC_PRESCAN,
		DWORD dwFlagsHMIDI = BASS_SAMPLE_FX | BASS_STREAM_DECODE) noexcept;

	EckInline BOOL Play(BOOL bReset = FALSE) const noexcept
	{
		return BASS_ChannelPlay(m_hStream, bReset);
	}

	EckInline BOOL Pause() const noexcept
	{
		return BASS_ChannelPause(m_hStream);
	}

	EckInline BOOL Stop() const noexcept
	{
		return BASS_ChannelStop(m_hStream);
	}

	EckInline BOOL SetVolume(float fVolume) noexcept
	{
        m_fVolume = fVolume;
		return SetAttribute(BASS_ATTRIB_VOL, fVolume);
	}

	EckInline float GetVolume() const noexcept
	{
		return GetAttribute(BASS_ATTRIB_VOL);
	}

	EckInline BOOL SetSpeed(float fScale) const noexcept
	{
		return SetAttribute(BASS_ATTRIB_FREQ, fScale * m_fDefSpeed);
	}

	EckInline float GetSpeed() const noexcept
	{
		if (eck::FloatEqual(m_fDefSpeed, 0.f))
			return 0.f;
		else [[likely]]
			return GetAttribute(BASS_ATTRIB_FREQ) / m_fDefSpeed;
	}

	EckInline BOOL SetPosition(double fTime) const noexcept
	{
		return BASS_ChannelSetPosition(m_hStream, BASS_ChannelSeconds2Bytes(m_hStream, fTime), BASS_POS_BYTE);
	}

	EckInline BOOL SetAttribute(DWORD dwAttr, float f) const noexcept
	{
		return BASS_ChannelSetAttribute(m_hStream, dwAttr, f);
	}

	EckInline float GetAttribute(DWORD dwAttr, _Out_ BOOL* pb = nullptr) const noexcept
	{
		float f{};
		const auto b = BASS_ChannelGetAttribute(m_hStream, dwAttr, &f);
		if (pb)
			*pb = b;
		return f;
	}

	EckInline double GetPosition() const noexcept
	{
		return BASS_ChannelBytes2Seconds(m_hStream, BASS_ChannelGetPosition(m_hStream, BASS_POS_BYTE));
	}

	EckInline double GetLength() const noexcept
	{
		return BASS_ChannelBytes2Seconds(m_hStream, BASS_ChannelGetLength(m_hStream, BASS_POS_BYTE));
	}

	void Close() noexcept;

	EckInline DWORD GetStreamHandle() const noexcept { return m_hStream; }

	EckInline DWORD GetLevel() const noexcept
	{
		return BASS_ChannelGetLevel(m_hStream);
	}

	EckInline DWORD GetData(float* pBuf, DWORD cbBuf) const noexcept
	{
		return BASS_ChannelGetData(m_hStream, pBuf, cbBuf);
	}

	EckInline DWORD TempoCreate(DWORD dwFlags = BASS_SAMPLE_FX | BASS_FX_FREESOURCE) noexcept
	{
		m_hStream = BASS_FX_TempoCreate(m_hStream, dwFlags);
		return m_hStream;
	}

	EckInline DWORD IsActive() const noexcept
	{
		return BASS_ChannelIsActive(m_hStream);
	}

	EckInline HSYNC SetSynchronousProcedure(
		DWORD dwType,
		QWORD ullParam,
		SYNCPROC pfn,
		void* pUser = nullptr) const noexcept
	{
		return BASS_ChannelSetSync(m_hStream, dwType, ullParam, pfn, pUser);
	}

	EckInline HFX SetEffect(DWORD dwType, int iPriority) const noexcept
	{
		return BASS_ChannelSetFX(m_hStream, dwType, iPriority);
	}

	EckInline BOOL RemoveEffect(HFX hFx) const noexcept
	{
		return BASS_ChannelRemoveFX(m_hStream, hFx);
	}
};