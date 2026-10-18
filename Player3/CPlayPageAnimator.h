#pragma once
class CPlayPageAnimator final : public Dui::CCompositorCornerMapping
{
private:
    Dui::CBitmap m_BitmapOverlay{};
    BOOLEAN m_bAnReverse{};// TRUE = 小到大，FALSE = 大到小
    BOOLEAN m_bAnActive{};
    BOOLEAN m_bCornerAnActive[4]{};
    eck::EasingCurve<eck::Easing::FOutExpo> m_Curve;
    eck::EasingCurve<eck::Easing::FOutExpo> m_CornerCurve[4]{};
    D2D1_RECT_F m_rcMini{};
    D2D1_RECT_F m_rcLarge{};
public:
    void PostRender(Dui::COMP_RENDER_INFO& cri) noexcept override;

    void SetOverlayBitmap(const Dui::CBitmap& Bitmap) noexcept { m_BitmapOverlay = Bitmap; }

    EckInlineNdCe BOOL PpaIsActive() const noexcept { return m_bAnActive; }
    EckInlineNdCe BOOL PpaIsReverse() const noexcept { return m_bAnReverse; }
    EckInlineNdCe float PpaCurrentValue() const noexcept { return m_Curve.K; }
    void PpaSetRect(const D2D1_RECT_F& rcMini, const D2D1_RECT_F& rcLarge) noexcept
    {
        m_rcMini = rcMini;
        m_rcLarge = rcLarge;
    }

    void PpaStart() noexcept;
    void PpaEnd() noexcept;
    // 如果动画需要继续运行，则返回TRUE
    BOOL PpaTick(float ms) noexcept;
};