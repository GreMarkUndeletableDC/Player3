#include "pch.h"
#include "CPlayPageAnimator.h"

void CPlayPageAnimator::PostRender(Dui::COMP_RENDER_INFO& cri) noexcept
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

void CPlayPageAnimator::PpaStart() noexcept
{
    ECKBOOLNOT(m_bAnReverse);
    const auto kBegin = m_bAnReverse ? 0.f : 1.f;
    const auto kEnd = m_bAnReverse ? 1.f : 0.f;
    m_Curve.Start(kBegin, kEnd, m_bAnActive);
    EckCounter(4, i)
    {
        m_CornerCurve[i].Start(kBegin, kEnd, m_bCornerAnActive[i]);
        m_bCornerAnActive[i] = TRUE;
    }
    m_bAnActive = TRUE;
}

void CPlayPageAnimator::PpaEnd() noexcept
{
    m_bAnActive = FALSE;
    ZeroMemory(m_bCornerAnActive, sizeof(m_bCornerAnActive));
}

BOOL CPlayPageAnimator::PpaTick(float ms) noexcept
{
    constexpr float MinimumDistance = 0.4f;
    constexpr float MaximumDuration = 700.f;
    constexpr float OverlayOpacityDuration = 150.f;

    constexpr float Duration[]
    {
        MaximumDuration,
        MaximumDuration * 5 / 6,
        MaximumDuration * 5 / 6,
        MaximumDuration * 4 / 6,
    };
    constexpr float DurationR[]
    {
        MaximumDuration * 4 / 6,
        MaximumDuration * 8 / 9,
        MaximumDuration * 5 / 6,
        MaximumDuration,
    };
    if (!(m_bAnActive = m_Curve.Tick((float)ms, MaximumDuration)))
        return FALSE;

    const auto kOverlay = std::clamp(
        m_Curve.Time / OverlayOpacityDuration, 0.f, 1.f);
    SetOpacity(m_bAnReverse ? (1.f - kOverlay) : kOverlay);

    D2D1_POINT_2F pt[4];
    const D2D1_POINT_2F ptMini[]
    {
        { m_rcMini.left,   m_rcMini.top     },
        { m_rcMini.right,  m_rcMini.top     },
        { m_rcMini.left,   m_rcMini.bottom  },
        { m_rcMini.right,  m_rcMini.bottom  },
    };
    const D2D1_POINT_2F ptLarge[]
    {
        { m_rcLarge.left,  m_rcLarge.top    },
        { m_rcLarge.right, m_rcLarge.top    },
        { m_rcLarge.left,  m_rcLarge.bottom },
        { m_rcLarge.right, m_rcLarge.bottom },
    };
    const auto pDuration = m_bAnReverse ? DurationR : Duration;
    BOOL bStillRunning{};
    EckCounter(4, i)
    {
        m_bCornerAnActive[i] = m_CornerCurve[i].Tick((float)ms, pDuration[i]);
        eck::CalculatePointFromLineScale(
            ptMini[i].x, ptMini[i].y,
            ptLarge[i].x, ptLarge[i].y,
            m_CornerCurve[i].K,
            pt[i].x, pt[i].y);
        if (m_bAnReverse)
        {
            if (fabs(ptLarge[i].x - pt[i].x) > MinimumDistance ||
                fabs(ptLarge[i].y - pt[i].y) > MinimumDistance)
                bStillRunning = TRUE;
        }
        else
        {
            if (fabs(ptMini[i].x - pt[i].x) > MinimumDistance ||
                fabs(ptMini[i].y - pt[i].y) > MinimumDistance)
                bStillRunning = TRUE;
        }
    }

    eck::CalculateDistortMatrix(m_rcLarge, pt, *AtMatrixD2D());
    eck::CalculateInverseDistortMatrix(m_rcLarge, pt, *AtMatrixD2DR());
    return bStillRunning;
}