#include "pch.h"
#include "CVeMiniCover.h"
#include "CWindowMain.h"

constexpr static float CoverAnimationEndValue = 6.f;

void CVeMiniCover::OnColorSchemeChanged(BOOL bForceUpdateCover) noexcept
{
}

LRESULT CVeMiniCover::OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
    switch (uMsg)
    {
    case WM_PAINT:
    {
        Dui::PAINTINFO ps;
        BeginPaint(ps, wParam, lParam);
        float k;

        const auto& Cover = GetImageManager()->CoverGetD2D();
        if (Cover.Get())
            if (m_bAnActive)
            {
                k = m_ec.K;
            BlurDC:
                auto rcView{ GetRectInClientD2D() };
                eck::InflateRect(rcView, k, k);
                GetDC()->DrawBitmap(Cover.Get(), rcView, 1.f,
                    D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, Cover.GetSourceRect());
                GetDC()->Flush();
                GetWindow().CcReserveBitmapLogical(GetWidth(), GetHeight());
                auto rcInTarget{ GetRectInClientD2D() };
                eck::OffsetRect(rcInTarget, ps.ox, ps.oy);
                GetWindow().BlurDrawDC(rcInTarget, {}, k);

                rcView = GetRectInClientD2D();
                rcView.left = (rcView.right - (float)PlayPageArrowSize) / 2;
                rcView.right = rcView.left + (float)PlayPageArrowSize;
                rcView.top = (rcView.bottom - (float)PlayPageArrowSize) / 2 +
                    (CoverAnimationEndValue - k) * 4.f/*箭头的行程因子*/;
                rcView.bottom = rcView.top + (float)PlayPageArrowSize;

                const auto& Icon = GetImageManager()->AtlasGetD2D(AppImage::PlayPageUp);
                GetDC()->DrawBitmap(Icon.Get(), rcView, k / CoverAnimationEndValue,
                    D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, Icon.GetSourceRect());
            }
            else
            {
                if (m_bHover)
                {
                    k = CoverAnimationEndValue;
                    goto BlurDC;
                }
                else
                    GetDC()->DrawBitmap(Cover.Get(), GetRectInClientD2D(), 1.f,
                        D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, Cover.GetSourceRect());
            }

        DbgDrawFrame();
        EndPaint(ps);
    }
    return 0;
    case WM_MOUSEMOVE:
    {
        if (!m_bHover)
        {
            m_bHover = TRUE;
            m_ec.Start(m_ec.K, CoverAnimationEndValue);
            GetWindow().KctWake();
        }
    }
    return 0;
    case WM_MOUSELEAVE:
    {
        if (m_bHover)
        {
            m_bHover = FALSE;
            m_ec.Start(CoverAnimationEndValue, 0.f, m_bAnActive);
            GetWindow().KctWake();
        }
    }
    return 0;

    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
    {
        m_bLBtnDown = TRUE;
        SetCapture();
    }
    break;
    case WM_LBUTTONUP:
    {
        if (m_bLBtnDown)
        {
            m_bLBtnDown = FALSE;
            ReleaseCapture();
            const auto& pt = EagPoint(lParam);
            if (eck::PointInRect(GetViewRect(), pt))
            {
                Dui::ELENMHDR nm{ Dui::ENC_COMMAND };
                SendNotify(&nm);
            }
        }
    }
    break;
    //case Dui::EWM_COLORSCHEMECHANGED:
    //    OnColorSchemeChanged(FALSE);
    //    break;
    case WM_CREATE:
    {
        __super::OnEvent(uMsg, wParam, lParam);
        GetWindow().KctRegisterTimeLine(this);
        OnColorSchemeChanged(TRUE);
    }
    return 0;
    case WM_DESTROY:
        GetWindow().KctUnregisterTimeLine(this);
        break;
    }
    return __super::OnEvent(uMsg, wParam, lParam);
}

void CVeMiniCover::TlTick(int ms) noexcept
{
    if (!m_bAnActive)
        return;
    m_bAnActive = m_ec.Tick((float)ms, 200.f);
    Invalidate();
}