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

        auto rc{ GetRectInClientD2D() };
        const auto rcSrc = m_BitmapCover.GetActualSourceRect();
        eck::AdjustRectToFitAnother(rc, rcSrc);
        GetDC()->DrawBitmap(
            m_BitmapCover.Get(),
            &rc,
            1.0f,
            D2D1_INTERPOLATION_MODE_LINEAR,
            m_BitmapCover.GetSourceRect());

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