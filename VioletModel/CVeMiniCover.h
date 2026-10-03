#pragma once
#include "CVeBase.h"

class CVeMiniCover final : public CVeBase, public eck::ITimeLine
{
private:
    eck::EasingCurve<eck::Easing::FOutCubic> m_ec{};

    int m_msLastInterval{};

    BOOLEAN m_bHover{};
    BOOLEAN m_bLBtnDown{};
    BOOLEAN m_bAnActive{};

    void OnPaint(const Dui::PAINTINFO& ps) noexcept;
public:
    LRESULT OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept override;

    void TlTick(int ms) noexcept override;
    BOOL TlIsValid() noexcept override { return m_bAnActive; }
    int TlGetCurrentInterval() const noexcept override { return m_msLastInterval; }
};