#pragma once
#include "CVeCover.h"
#include "CVeLrc.h"
#include "CVeBase.h"
#include "CPlayer.h"

class CPagePlaying : public CVeBase
{
public:
    constexpr static float
        LyricTopPosition = 50.f,
        LyricBottomPosition = 135.f,
        BackButtonSize = 40.f,
        LabelCoverPadding = 24.f,
        LabelPadding = 4.f,
        LabelHeight = 26.f
        ;
private:
    CVeCover m_Cover{};
    CVeLyric m_Lyric{};
    Dui::CButton m_BTBack{};
    Dui::CLabel m_LATitle{};
    Dui::CLabel m_LAAlbum{};
    Dui::CLabel m_LAArtist{};

    ComPtr<ID2D1Bitmap1> m_pBitmapBlurredCover{};

    void OnPlayEvent(const PLAY_EVT_PARAM& e) noexcept;

    void SetEmptyText() noexcept;

    void OnColorSchemeChanged() noexcept;
public:
    LRESULT OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept override;

    void SetLabelTextFormatTitle(IDWriteTextFormat* pTf) noexcept
    {
        m_LATitle.SetTextFormat(pTf);
    }

    void SetLabelTextFormat(IDWriteTextFormat* pTf) noexcept
    {
        m_LAAlbum.SetTextFormat(pTf);
        m_LAArtist.SetTextFormat(pTf);
    }

    void UpdateBlurredCover() noexcept;
};