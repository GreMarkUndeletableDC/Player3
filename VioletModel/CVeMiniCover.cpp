#include "pch.h"
#include "CVeMiniCover.h"

constexpr static float AnCoverEndValue = 6.f;
constexpr static float AnCoverDuration = 200.f;
constexpr static float PlayPageArrowSize = 30.f;

void CVeMiniCover::OnPaint(const Dui::PAINTINFO& ps) noexcept
{
    const auto Cover = GetImageManager()->CoverGetD2D();
    if (!Cover)
        return;
    if (m_bAnActive || m_bHover)
    {
        const auto k = (!m_bAnActive && m_bHover) ? AnCoverEndValue : m_ec.K;
        const auto cx = GetWidth();
        const auto cy = GetHeight();

        // -- 封面

        auto rcView{ GetRectInClientD2D() };
        eck::InflateRect(rcView, k, k);
        Cover.Draw(GetDC(), rcView);

        // -- 模糊
        if (m_bEnableBlur)
        {
            GetDC()->Flush();
            GetWindow().CcReserveBitmapLogical(cx, cy);
            Dui::CFilterBlur::Extra Extra{ sizeof(Extra) };
            Extra.fDeviation = k / 1.5f;
            GetFilterBlur()->FilterDC(
                GetRectInClientD2D(), ps.ox, ps.oy, &Extra);
        }

        // TODO: 遮罩

        // -- 箭头

        rcView = GetRectInClientD2D();
        rcView.left += (cx - PlayPageArrowSize) / 2;
        rcView.right = rcView.left + PlayPageArrowSize;
        rcView.top += (cy - PlayPageArrowSize) / 2 +
            (AnCoverEndValue - k) * 4.f/*箭头的行程因子*/;
        rcView.bottom = rcView.top + PlayPageArrowSize;

        const auto Icon = GetImageManager()->AtlasGetD2D(AppImage::PlayPageUp);
        Icon.Draw(GetDC(), rcView, k / AnCoverEndValue);
    }
    else
        Cover.Draw(GetDC(), GetRectInClientD2D());
}

LRESULT CVeMiniCover::OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
    switch (uMsg)
    {
    case WM_PAINT:
    {
        Dui::PAINTINFO ps;
        BeginPaint(ps, wParam, lParam);
        OnPaint(ps);
        DbgDrawFrame();
        EndPaint(ps);
    }
    return 0;
    case WM_MOUSEMOVE:
    {
        if (!m_bHover)
        {
            m_bHover = TRUE;
            m_ec.Start(m_ec.K, AnCoverEndValue, m_bAnActive);
            m_bAnActive = TRUE;
            GetWindow().KctWake();
        }
    }
    return 0;
    case WM_MOUSELEAVE:
    {
        if (m_bHover)
        {
            m_bHover = FALSE;
            m_ec.Start(AnCoverEndValue, 0.f, m_bAnActive);
            m_bAnActive = TRUE;
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
            const auto& pt = LpPoint(lParam);
            if (eck::PointInRect(GetViewRect(), pt))
            {
                Dui::ELENMHDR nm{ Dui::ENC_COMMAND };
                SendNotify(&nm);
            }
        }
    }
    break;
    case WM_CREATE:
        GetWindow().KctRegisterTimeLine(this);
        break;
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
    m_bAnActive = m_ec.Tick((float)ms, AnCoverDuration);
    Invalidate();
}