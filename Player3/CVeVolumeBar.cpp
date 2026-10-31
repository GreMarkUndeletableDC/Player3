#include "pch.h"
#include "CVeVolumeBar.h"
#include "CApplication.h"

LRESULT CVeVolumeBar::OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
    switch (uMsg)
    {
    case WM_PAINT:
    {
        Dui::PAINTINFO ps;
        BeginPaint(ps, wParam, lParam);

        auto rcContent{ GetRectInClientD2D() };
        eck::InflateRect(rcContent, -ShadowDistance, -ShadowDistance);

        D2D1_RECT_F rcTemp;
        eck::IntersectRect(rcTemp, rcContent, ps.rcClip);
        GetDC()->FillRectangle(rcTemp,
            GetWindow().CcSetBrushColor(
                GetTheme()->GetColorD2D(IdCrVolumeBarBackground)));

        rcTemp = rcContent;
        eck::InflateRect(rcTemp, -0.5f, -0.5f);
        GetDC()->DrawRectangle(rcTemp,
            GetWindow().CcSetBrushColor(
                GetTheme()->GetColorD2D(IdCrVolumeBarBorder)));

        GetShadow().DrawGrid(
            GetDC(),
            rcContent,
            ShadowDistance,
            D2D1_INTERPOLATION_MODE_LINEAR,
            0.8f);

        EndPaint(ps);
    }
    return 0;

    case WM_SETFONT:
        m_LAVol.SetTextFormat(GetTextFormat().Get());
        break;

    case WM_CREATE:
    {
        __super::OnEvent(uMsg, wParam, lParam);
        GetWindow().KctRegisterTimeLine(this);

        const auto mg = ShadowDistance;

        m_LAVol.Create(
            L"100"sv,
            Dui::DES_VISIBLE | Dui::DES_PARENT_COMP,
            0,
            InnerPadding + mg,
            mg,
            LabelWidth,
            GetHeight() - mg * 2.f,
            this);

        const auto x = InnerPadding * 2.f + LabelWidth + mg;
        m_TrackBar.Create(
            {},
            Dui::DES_VISIBLE | Dui::DES_PARENT_COMP | Dui::DES_NOTIFY_WND,
            0,
            x,
            mg,
            GetWidth() - x - InnerPadding - mg,
            GetHeight() - mg * 2.f,
            this, nullptr, ELEID_VOLBAR_TRACK);
        m_TrackBar.SetRange(0, 200);
        m_TrackBar.SetTrackPosition(100);
        m_TrackBar.SetTrackSize(TrackBarTrackHeight);
        m_TrackBar.SetThumbSize(TrackBarThumbSize);
        m_TrackBar.SetThumbTrack(TRUE);
    }
    return 0;
    case WM_DESTROY:
        GetWindow().KctUnregisterTimeLine(this);
        break;
    }
    return __super::OnEvent(uMsg, wParam, lParam);
}

void CVeVolumeBar::ShowAnimation() noexcept
{
    ECKBOOLNOT(m_bShow);
    SetStyle(GetStyle() | Dui::DES_VISIBLE);
    SetCompositor(&m_PageAn);
    if (m_bShow)
        m_ecShowing.Start(0.f, 1.f, m_bAnimating);
    else
        m_ecShowing.Start(1.f, 0.f, m_bAnimating);
    m_bAnimating = TRUE;
    GetWindow().KctWake();
}

void CVeVolumeBar::OnVolumeChanged(float fVol) noexcept
{
    WCHAR szVol[eck::TcvIntBufferSize<int>()];
    const auto [_, pEnd] = eck::TcvFromInt(EckArgString(szVol), (int)fVol);
    m_LAVol.SetText(std::wstring_view{ szVol, pEnd });
    m_LAVol.Invalidate();
}

void CVeVolumeBar::TlTick(int ms) noexcept
{
    m_msLastInterval = ms;
    m_bAnimating = m_ecShowing.Tick((float)ms, 300.f);
    m_PageAn.SetOpacity(m_ecShowing.K);
    m_PageAn.SetMatrix(D2D1::Matrix3x2F::Translation(
        0.f,
        (1.f - m_ecShowing.K) * AnimationDistance));

    auto rc = GetWholeRectInClient();
    CompUpdateCompositedRect();
    const auto rcNew = GetWholeRectInClient();
    eck::UnionRect(rc, rc, rcNew);
    GetWindow().RdInvalidate(rc, FALSE);

    if (!m_bAnimating)
    {
        SetCompositor(nullptr);
        if (!m_bShow)
            SetStyle(GetStyle() & ~Dui::DES_VISIBLE);
    }
}