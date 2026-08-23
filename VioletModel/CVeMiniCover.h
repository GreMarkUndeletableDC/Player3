#pragma once
#include "CVeBase.h"

class CVeMiniCover : public CVeBase, public eck::ITimeLine
{
public:
    constexpr static float
        PlayPageArrowSize = 30.f
        ;
private:
    eck::EasingCurve<eck::Easing::FOutCubic> m_ec{};

    int m_msLastInterval{};

    BOOLEAN m_bHover{};
    BOOLEAN m_bLBtnDown{};
    BOOLEAN m_bAnActive{};

    void OnColorSchemeChanged(BOOL bForceUpdateCover) noexcept;
public:
    LRESULT OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept override;

    void TlTick(int ms) noexcept override;
    BOOL TlIsValid() noexcept override { return m_bAnActive; }
    int TlGetCurrentInterval() noexcept override { return m_msLastInterval; }
};