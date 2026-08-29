#pragma once
class CCompositorPlayPage final : public Dui::CCompositorCornerMapping
{
private:
    Dui::CBitmap m_BitmapOverlay{};
public:
    void PostRender(Dui::COMP_RENDER_INFO& cri) noexcept override
    {
        cri.pDC->DrawBitmap(
            cri.pBitmap,
            cri.rcDst,
            1.f,
            GetInterpolationMode(),
            cri.rcSrc,
            AtMatrixD2D());
        if (m_BitmapOverlay.Get())
        {
            cri.pDC->DrawBitmap(
                m_BitmapOverlay.Get(),
                cri.rcDst,
                GetOpacity(),
                GetInterpolationMode(),
                m_BitmapOverlay.GetSourceRect(),
                AtMatrixD2D());
        }
    }

    void SetOverlayBitmap(const Dui::CBitmap& Bitmap) noexcept { m_BitmapOverlay = Bitmap; }
};