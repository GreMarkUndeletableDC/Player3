#include "pch.h"
#include "CVeCover.h"


LRESULT CVeCover::OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
    switch (uMsg)
    {
    case WM_PAINT:
    {
        Dui::PAINTINFO ps;
        BeginPaint(ps, wParam, lParam);

        const auto rcEle{ GetRectInClientD2D() };
        auto rcSrc{ m_BitmapCover.GetActualSourceRect() };
        PixelToLogical(rcSrc);
        eck::AdjustRectToFitAnother(rcSrc, rcEle);
        m_BitmapCover.Draw(GetDC(), rcEle, m_kOpacity, m_eInterMode);

        DbgDrawFrame();
        EndPaint(ps);
    }
    return 0;

    case WM_DESTROY:
        m_BitmapCover.Clear();
        break;
    }

    return __super::OnEvent(uMsg, wParam, lParam);
}