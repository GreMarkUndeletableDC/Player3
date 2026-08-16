#pragma once
#include "CVeMiniCover.h"
#include "CPlayer.h"

class CPlayPanel : public CVeBase
{
public:
    constexpr static float
        PlayPanelTextPadding = 20.f,
        TitleTopPosition = 18.f,
        PlayPanelTextHeight = 20.f,
        TitleArtistPadding = 14.f,
        TitleArtistMaximumWidth = 130.f,
        TimeTopPosition = 40.f,
        TimeMaximumWidth = 110.f
        ;
private:
    CVeMiniCover m_Cover{};
    Dui::CLabel m_LATitle{};
    Dui::CLabel m_LAArtist{};
    Dui::CLabel m_LATime{};

    void OnPlayEvent(const PLAY_EVT_PARAM& e) noexcept;
public:
    LRESULT OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept override;

    EckInlineNdCe auto& GetCoverElement() noexcept { return m_Cover; }
};