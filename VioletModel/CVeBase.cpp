#include "pch.h"
#include "CVeBase.h"

class CVioletTheme : public Dui::CTheme
{
    Dui::TmResult Draw(
        Dui::CElement* pEle,
        const Dui::SimpleStyle* pStyle,
        UINT idPart,
        const D2D1_RECT_F& rc,
        _In_opt_ const D2D1_RECT_F* prcClip = nullptr) noexcept;
};


Dui::TmResult CVioletTheme::Draw(
    Dui::CElement* pEle,
    const Dui::SimpleStyle* pStyle,
    UINT idPart,
    const D2D1_RECT_F& rc,
    _In_opt_ const D2D1_RECT_F* prcClip) noexcept
{
    return Dui::TmResult::NotSupport;
}


RcPtr<Dui::CTheme> CVeBase::TmMakeDefaultTheme(BOOL bDark) noexcept
{
    auto p = Dui::TmMakeTheme<CVioletTheme>(bDark);
    return p;
}

RcPtr<Dui::CColorCollection> CVeBase::TmMakeDefaultColorCollection(BOOL bDark) noexcept
{
    auto p = (bDark ? Dui::TmsColorCollectionDark() : Dui::TmsColorCollectionLight());
    return p;
}

RcPtr<Dui::CMetricCollection> CVeBase::TmMakeDefaultMetricCollection() noexcept
{
    auto p = Dui::TmsMetricCollection();
    return p;
}

const RcPtr<Dui::CTheme>& CVeBase::TmDefaultTheme(BOOL bDark) noexcept
{
    static auto p1{ TmMakeDefaultTheme(TRUE) };
    static auto p2{ TmMakeDefaultTheme(FALSE) };
    return bDark ? p1 : p2;
}

const RcPtr<Dui::CColorCollection>& CVeBase::TmDefaultColorCollection(BOOL bDark) noexcept
{
    static auto p1{ TmMakeDefaultColorCollection(TRUE) };
    static auto p2{ TmMakeDefaultColorCollection(FALSE) };
    return bDark ? p1 : p2;
}

const RcPtr<Dui::CMetricCollection>& CVeBase::TmDefaultMetricCollection() noexcept
{
    static auto p{ TmMakeDefaultMetricCollection() };
    return p;
}

LRESULT CVeBase::OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
    switch (uMsg)
    {
    case WM_CREATE:
        SetTheme(TmDefaultTheme(TmIsDarkMode()).Get());
        break;
    }
    return __super::OnEvent(uMsg, wParam, lParam);
}