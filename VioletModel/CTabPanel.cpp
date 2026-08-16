#include "pch.h"
#include "CWndMain.h"
#include "CApp.h"


CTabPanel::LvIndex CTabPanel::LcaGetCount() const noexcept
{
    return { .Item = 4 };
}

void CTabPanel::OnColorSchemeChanged()
{
    m_LAIcon.SetBitmap(GetAtlas()->AtlasGetD2D(AppImage::WindowLogo));
}

LRESULT CTabPanel::OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
    switch (uMsg)
    {
    case WM_PAINT:
    {
        Dui::PAINTINFO ps;
        BeginPaint(ps, wParam, lParam);
        // TODO: 颜色管理
        //m_pBrush->SetColor(App->GetColor(GPal::TabPanelBk));
        GetDC()->FillRectangle(ps.rcClipInEle, GetWindow().CcSetBrushColor({}));
        EndPaint(ps);
    }
    return 0;

    case WM_SIZE:
    {
        Kw::Rect rc;
        rc.left = (GetWidth() - WindowLogoSize) / 2.f;
        rc.top = rc.left + WindowLogoSize / 3.f;
        rc.right = rc.left + WindowLogoSize;
        rc.bottom = rc.top + WindowLogoSize;
        m_LAIcon.SetRect(rc);

        const auto Padding = GetTheme()->GetMetric(Dui::IdMePaddingInner);
        m_TAB.SetRect({
            Padding,
            rc.bottom + WindowLogoTabPadding,
            GetWidth() - Padding,
            GetHeight() - Padding });
    }
    break;

    //case Dui::EWM_COLORSCHEMECHANGED:
    //    OnColorSchemeChanged();
    //    break;

    case WM_CREATE:
        __super::OnEvent(uMsg, wParam, lParam);
        m_LAIcon.Create({}, Dui::DES_VISIBLE, 0,
            0, 0, GetWidth(), GetWidth(), this);
        m_LAIcon.SetOnlyBitmap(TRUE);
        m_LAIcon.SetBackgroundMode(eck::ImageMode::CenterUniform);

        m_TAB.Create({}, Dui::DES_VISIBLE, 0,
            0, 0, 0, 0, this);
        m_TAB.SetAdapter(this);

        OnColorSchemeChanged();
        return 0;
    }
    return __super::OnEvent(uMsg, wParam, lParam);
}