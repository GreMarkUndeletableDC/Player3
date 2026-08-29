#include "pch.h"
#include "CBass.h"

static const std::pair<int, std::wstring_view> BassErrorMap[]
{
    { -1, L"BASS_ERROR_UNKNOWN"sv      },
    {  0, L"BASS_OK"sv                 },
    {  1, L"BASS_ERROR_MEM"sv          },
    {  2, L"BASS_ERROR_FILEOPEN"sv     },
    {  3, L"BASS_ERROR_DRIVER"sv       },
    {  4, L"BASS_ERROR_BUFLOST"sv      },
    {  5, L"BASS_ERROR_HANDLE"sv       },
    {  6, L"BASS_ERROR_FORMAT"sv       },
    {  7, L"BASS_ERROR_POSITION"sv     },
    {  8, L"BASS_ERROR_INIT"sv         },
    {  9, L"BASS_ERROR_START"sv        },
    { 10, L"BASS_ERROR_SSL"sv          },
    { 14, L"BASS_ERROR_ALREADY"sv      },
    { 17, L"BASS_ERROR_NOTAUDIO"sv     },
    { 18, L"BASS_ERROR_NOCHAN"sv       },
    { 19, L"BASS_ERROR_ILLTYPE"sv      },
    { 20, L"BASS_ERROR_ILLPARAM"sv     },
    { 21, L"BASS_ERROR_NO3D"sv         },
    { 22, L"BASS_ERROR_NOEAX"sv        },
    { 23, L"BASS_ERROR_DEVICE"sv       },
    { 24, L"BASS_ERROR_NOPLAY"sv       },
    { 25, L"BASS_ERROR_FREQ"sv         },
    { 27, L"BASS_ERROR_NOTFILE"sv      },
    { 29, L"BASS_ERROR_NOHW"sv         },
    { 31, L"BASS_ERROR_EMPTY"sv        },
    { 32, L"BASS_ERROR_NONET"sv        },
    { 33, L"BASS_ERROR_CREATE"sv       },
    { 34, L"BASS_ERROR_NOFX"sv         },
    { 37, L"BASS_ERROR_NOTAVAIL"sv     },
    { 38, L"BASS_ERROR_DECODE"sv       },
    { 39, L"BASS_ERROR_DX"sv           },
    { 40, L"BASS_ERROR_TIMEOUT"sv      },
    { 41, L"BASS_ERROR_FILEFORM"sv     },
    { 42, L"BASS_ERROR_SPEAKER"sv      },
    { 43, L"BASS_ERROR_VERSION"sv      },
    { 44, L"BASS_ERROR_CODEC"sv        },
    { 45, L"BASS_ERROR_ENDED"sv        },
    { 46, L"BASS_ERROR_BUSY"sv         },
    { 47, L"BASS_ERROR_UNSTREAMABLE"sv },
};


void CBass::VersionToString(DWORD dw, Eck_Out_buffer_ eck::CStringW& rs) noexcept
{
    const WORD wHigh = HIWORD(dw);
    const WORD wLow = LOWORD(dw);
    rs.Format(
        L"%d.%d.%d.%d",
        (int)HIBYTE(wHigh), (int)LOBYTE(wHigh),
        (int)HIBYTE(wLow), (int)LOBYTE(wLow));
}

DWORD CBass::Open(PCWSTR pszFile, DWORD dwFlagsHS, DWORD dwFlagsHM, DWORD dwFlagsHMIDI) noexcept
{
    Close();
    constexpr DWORD dwCommFlags = BASS_SAMPLE_FLOAT | BASS_UNICODE;
    DWORD h = BASS_StreamCreateFile(FALSE, pszFile, 0, 0, dwCommFlags | dwFlagsHS);

    m_eMusicType = MusicType::Invalid;
    if (!h && BASS_ErrorGetCode() == BASS_ERROR_FILEFORM)
    {
        h = BASS_MusicLoad(FALSE, pszFile, 0, 0, dwCommFlags | dwFlagsHM, 0);
        if (!h && BASS_ErrorGetCode() == BASS_ERROR_FILEFORM)
        {
            h = BASS_MIDI_StreamCreateFile(FALSE, pszFile, 0, 0, dwCommFlags | dwFlagsHMIDI, 1);
            if (h)
                m_eMusicType = MusicType::Midi;
        }
        else
            m_eMusicType = MusicType::Mod;
    }
    else
        m_eMusicType = MusicType::Normal;

    m_hStream = h;
    if (h)
    {
        m_fDefSpeed = GetAttribute(BASS_ATTRIB_FREQ);
        SetAttribute(BASS_ATTRIB_VOL, m_fVolume);
    }
    else
        m_fDefSpeed = 0.f;
    return h;
}

void CBass::Close() noexcept
{
    if (!m_hStream)
        return;
    switch (m_eMusicType)
    {
    case MusicType::Normal:
    case MusicType::Midi:
        BASS_StreamFree(m_hStream);
        break;
    case MusicType::Mod:
        BASS_MusicFree(m_hStream);
        break;
    default:
        EckDbgBreak();
        break;
    }
    m_hStream = 0;
}

std::wstring_view CBass::GetErrorMessage(int iErrCode) noexcept
{
    const auto it = std::lower_bound(
        std::begin(BassErrorMap), std::end(BassErrorMap), iErrCode,
        [](const auto& a, const auto& b) { return a.first < b; });
    if (it == std::end(BassErrorMap))
        return L"(Unknown)"sv;
    else
        return it->second;
}