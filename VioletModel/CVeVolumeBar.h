#pragma once
#include "CVeBase.h"

class CVeVolumeBar final : public CVeBase, public eck::ITimeLine
{
public:
    constexpr static float
        TrackBarTrackHeight = 6.f,
        TrackBarThumbSize = 10.f,
        LabelWidth = 30.f,
        InnerPadding = 8.f,
        AnimationDistance = 10.f
        ;
private:
    Dui::CLabel m_LAVol{};
    Dui::CTrackBar m_TrackBar{};

    Dui::CCompositor2DAffineTransform m_PageAn{};
    eck::EasingCurve<eck::Easing::FOutCubic> m_ecShowing{};

    int m_msLastInterval{};
    BOOLEAN m_bShow{};
    BOOLEAN m_bAnimating{};
public:
    LRESULT OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept override;

    void ShowAnimation() noexcept;

    void OnVolumeChanged(float fVol) noexcept;

    void TlTick(int ms) noexcept override;
    BOOL TlIsValid() noexcept override { return m_bAnimating; }
    int TlGetCurrentInterval() const noexcept override { return m_msLastInterval; }
};