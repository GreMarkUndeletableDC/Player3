#pragma once
#include "CVeBase.h"

class CVeCover final : public CVeBase
{
private:
    Dui::CBitmap m_BitmapCover{};
public:
    LRESULT OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept override;

    void SetBitmap(const Dui::CBitmap& Bitmap) noexcept { m_BitmapCover = Bitmap; }
};