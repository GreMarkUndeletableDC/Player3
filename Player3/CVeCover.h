#pragma once
#include "CVeBase.h"

class CVeCover final : public CVeBase
{
private:
    Dui::CBitmap m_BitmapCover{};
    D2D1_INTERPOLATION_MODE m_eInterMode{ D2D1_INTERPOLATION_MODE_LINEAR };
    float m_kOpacity{ 1.f };
public:
    LRESULT OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept override;

    void SetBitmap(Dui::CBitmap Bitmap) noexcept { m_BitmapCover = std::move(Bitmap); }
    EckInlineNdCe auto& GetBitmap() const noexcept { return m_BitmapCover; }

    void SetInterpolationMode(D2D1_INTERPOLATION_MODE e) noexcept { m_eInterMode = e; }
    EckInlineNdCe auto GetInterpolationMode() const noexcept { return m_eInterMode; }

    void SetOpacity(float k) noexcept { m_kOpacity = k; }
    EckInlineNdCe auto GetOpacity() const noexcept { return m_kOpacity; }
};