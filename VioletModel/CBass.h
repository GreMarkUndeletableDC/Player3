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
    EckInline static BOOL Initialize(
        int iDevice = -1,
        DWORD dwFreq = 44100,
        DWORD dwFlags = 0,
        HWND hWnd = nullptr) noexcept
    {
        return BASS_Init(iDevice, dwFreq, dwFlags, hWnd, nullptr);
    }

    EckInline static BOOL Free() noexcept { return BASS_Free(); }

    EckInline static DWORD GetVersion() noexcept { return BASS_GetVersion(); }
    EckInline static int GetError() noexcept { return BASS_ErrorGetCode(); }
    static std::wstring_view GetErrorMessage(int iErrCode) noexcept;

    static void VersionToString(
        DWORD dw,
        Eck_Out_buffer_ eck::CStringW& rs) noexcept;

    ~CBass() noexcept { Close(); }

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
        if (m_fDefSpeed == 0.f)
            return 0.f;
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

    EckInline float GetAttribute(
        DWORD dwAttr,
        _Out_opt_ BOOL* pbOk = nullptr) const noexcept
    {
        float f{};
        const auto b = BASS_ChannelGetAttribute(m_hStream, dwAttr, &f);
        if (pbOk) *pbOk = b;
        return f;
    }

    EckInline double GetPosition() const noexcept
    {
        return BASS_ChannelBytes2Seconds(
            m_hStream,
            BASS_ChannelGetPosition(m_hStream, BASS_POS_BYTE));
    }

    EckInline double GetLength() const noexcept
    {
        return BASS_ChannelBytes2Seconds(
            m_hStream,
            BASS_ChannelGetLength(m_hStream, BASS_POS_BYTE));
    }

    void Close() noexcept;

    EckInline DWORD GetStreamHandle() const noexcept { return m_hStream; }

    EckInline DWORD GetLevel() const noexcept
    {
        return BASS_ChannelGetLevel(m_hStream);
    }

    EckInline DWORD GetData(
        _Out_writes_bytes_(cbBuf) float* pBuf,
        DWORD cbBuf) const noexcept
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