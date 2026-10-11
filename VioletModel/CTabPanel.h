#pragma once
#include "CVeBase.h"

class CTabPanel final : public CVeBase, public Dui::CListView::IAdapter
{
public:
    struct EVT_SELECT_CHANGE : Dui::ELENMHDR
    {
        int idx;
    };

    constexpr static float
        WindowLogoSize = 22.f,
        WindowLogoTabPadding = 20.f
        ;

    constexpr static std::wstring_view PageName[]
    {
        L"主页"sv,
        L"列表"sv,
        L"效果"sv,
        L"设置"sv,
    };
private:
    using LvIndex = eck::UiBasic::Lc::Index;
    using LvProperty = eck::UiBasic::Lc::Property;

    Dui::CLabel m_LAIcon{};
    Dui::CListView m_LV{};


    LvIndex LcaGetCount() const noexcept override;
    void LcaGet(const LvIndex& idx, int idxCol,
        LvProperty eProp, std::any& Data) const noexcept override;
    void LcaSet(const LvIndex& idx, int idxCol,
        LvProperty eProp, std::any& Data, BOOL bMove = FALSE) noexcept override;

    void OnColorSchemeChanged() noexcept;
public:
    LRESULT OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept override;

    EckInlineNdCe auto& GetTabList() noexcept { return m_LV; }
};