#pragma once
#include "CVeBase.h"

class CTabPanel : public CVeBase, public Dui::CListView::IAdapter
{
public:
    constexpr static float
        WindowLogoSize = 18.f,
        WindowLogoTabPadding = 20.f
        ;
private:
    using LvIndex = eck::UiBasic::Lc::Index;
    using LvProperty = eck::UiBasic::Lc::Property;

    Dui::CLabel m_LAIcon{};
    Dui::CListView m_TAB{};


    LvIndex LcaGetCount() const noexcept override;
    void LcaGet(const LvIndex& idx, int idxCol,
        LvProperty eProp, std::any& Data) const noexcept override {
    }
    void LcaSet(const LvIndex& idx, int idxCol,
        LvProperty eProp, std::any& Data, BOOL bMove = FALSE) noexcept override {
    }

    void OnColorSchemeChanged();
public:
    LRESULT OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept override;

    EckInlineNdCe auto& GetTabList() { return m_TAB; }
};