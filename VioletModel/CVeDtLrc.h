#pragma once
#include "CVeBase.h"

struct NM_DTL_GET_TIME : Dui::ELENMHDR
{
    float fTime;
};

class CVeDesktopLyric : public CVeBase, public eck::ITimeLine
{
public:
    constexpr static int c_InvalidCacheIdx = std::numeric_limits<int>::min();
private:
    enum : size_t
    {
        BriMain,
        BriTrans,
        BriMainHiLight,
        BriTransHiLight,
        BriBorder,
        BriShadow,

        BriMax
    };

    RefPtr<Lyric::CLyric> m_pLyric{};
    int m_idxCurr{ -1 };
    int m_msLastInterval{};
public:
    LRESULT OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept override;

    void TlTick(int iMs) noexcept override;
    BOOL TlIsValid() noexcept override { return FALSE; }
    int TlGetCurrentInterval() noexcept override { return m_msLastInterval; }

    HRESULT LrcSetCurrentLine(int idx) noexcept;
    void LrcSetEmptyText(std::wstring_view svEmptyText) noexcept;

    void SetTextFormatTranslation(IDWriteTextFormat* pTf) noexcept;
    EckInlineNdCe auto GetTextFormatTrans() const noexcept { return (IDWriteTextFormat*)0; }

    void SetLyric(RefPtr<Lyric::CLyric> pLyric) noexcept { m_pLyric = std::move(pLyric); }
    EckInlineNdCe auto& GetLyric() const noexcept { return m_pLyric; }
};