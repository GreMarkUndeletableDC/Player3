#include "pch.h"
#include "CWindowMain.h"
#include "CApplication.h"

EckInlineNdCe AppImage IndexToImage(int idx) noexcept
{
    switch (idx)
    {
    case 0: return AppImage::Home;
    case 1: return AppImage::List;
    case 2: return AppImage::Effect;
    case 3: return AppImage::Gear;
    }
    return AppImage::Home;
}

CTabPanel::LvIndex CTabPanel::LcaGetCount() const noexcept
{
    return { .Item = 4 };
}

void CTabPanel::LcaGet(const LvIndex& idx, int idxCol,
    LvProperty eProp, std::any& Data) const noexcept
{
    switch (eProp)
    {
    case LvProperty::Image:
        Data = GetImageManager()->AtlasGetD2D(IndexToImage(idx.Item));
        break;
    }
}

void CTabPanel::LcaSet(const LvIndex& idx, int idxCol,
    LvProperty eProp, std::any& Data, BOOL bMove) noexcept
{
    switch (eProp)
    {
    case LvProperty::SystemSelection:
    {
        const auto b = std::any_cast<BOOL>(Data);
        if (b)
        {
            EVT_SELECT_CHANGE nm{ ELEN_PAGE_CHANGE };
            nm.idx = idx.Item;
            SendNotify(&nm);
        }
    }
    break;
    }
}

void CTabPanel::OnColorSchemeChanged() noexcept
{
    m_LAIcon.SetBitmap(GetImageManager()->AtlasGetD2D(AppImage::WindowLogo));
}

LRESULT CTabPanel::OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
    switch (uMsg)
    {
    case WM_PAINT:
    {
        Dui::PAINTINFO ps;
        BeginPaint(ps, wParam, lParam);
        GetDC()->FillRectangle(ps.rcClipInEle, GetWindow().CcSetBrushColor(
            GetTheme()->GetColorD2D(IdCrTabPanelBackground)));
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
        m_LV.SetRect({
            Padding,
            rc.bottom + WindowLogoTabPadding,
            GetWidth() - Padding,
            GetHeight() - Padding });
    }
    break;

    case WM_CREATE:
        __super::OnEvent(uMsg, wParam, lParam);
        m_LAIcon.Create({}, Dui::DES_VISIBLE, 0,
            0, 0, GetWidth(), GetWidth(), this);
        m_LAIcon.SetOnlyBitmap(TRUE);
        m_LAIcon.SetBackgroundMode(eck::ImageMode::CenterUniform);
        m_LAIcon.SetBackgroundBitmap(GetImageManager()->AtlasGetD2D(AppImage::WindowLogo));

        m_LV.Create({}, Dui::DES_VISIBLE, 0,
            0, 0, 0, 0, this);
        m_LV.SetAdapter(this);
        m_LV.SetNonImageListIcon(TRUE);
        m_LV.SetFitImage(TRUE);
        m_LV.GetController().SetClearSelectionInSpace(FALSE);

        OnColorSchemeChanged();
        return 0;
    }
    return __super::OnEvent(uMsg, wParam, lParam);
}