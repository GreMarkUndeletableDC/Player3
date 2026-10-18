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

    BOOLEAN m_bEnableBlur{ TRUE };

    void OnPaint(const Dui::PAINTINFO& ps) noexcept;
public:
    LRESULT OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept override;

    void TlTick(int ms) noexcept override;
    BOOL TlIsValid() noexcept override { return m_bAnActive; }
    int TlGetCurrentInterval() const noexcept override { return m_msLastInterval; }

    EckInlineCe void SetEnableBlur(BOOLEAN b) noexcept { m_bEnableBlur = b; }
    EckInlineNdCe BOOLEAN GetEnableBlur() const noexcept { return m_bEnableBlur; }
};