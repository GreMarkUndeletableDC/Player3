#pragma once
class CCompositorPlayPage final : public Dui::CCompositorCornerMapping
{
private:
    Dui::CBitmap m_BitmapOverlay{};
public:
    void PostRender(Dui::COMP_RENDER_INFO& cri) noexcept override
    {
        cri.pDC->DrawBitmap(cri.pBitmap, cri.rcDst, 1.f,
            D2D1_INTERPOLATION_MODE_LINEAR, cri.rcSrc, AtMatrixD2D());
        if (m_BitmapOverlay.Get())
            cri.pDC->DrawBitmap(m_BitmapOverlay.Get(), cri.rcDst, GetOpacity(),
                D2D1_INTERPOLATION_MODE_LINEAR, m_BitmapOverlay.GetSourceRect(), AtMatrixD2D());
    }

    void SetOverlayBitmap(const Dui::CBitmap& Bitmap) noexcept
    {
        m_BitmapOverlay = Bitmap;
    }
};