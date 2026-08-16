#pragma once
#include "CVioletAtlas.h"

class CVeBase : public Dui::CElement
{
public:
    const static inline UINT IdCrVolumeBarBack = Dui::TmNextResourceId();
    const static inline UINT IdCrVolumeBarBorder = Dui::TmNextResourceId();
    const static inline UINT IdCrPalyPageMask = Dui::TmNextResourceId();
private:
    RefPtr<CVioletAtlas> m_pAtlas{};
public:
    static RcPtr<Dui::CTheme> TmMakeDefaultTheme(BOOL bDark) noexcept;
    static RcPtr<Dui::CColorCollection> TmMakeDefaultColorCollection(BOOL bDark) noexcept;
    static RcPtr<Dui::CMetricCollection> TmMakeDefaultMetricCollection() noexcept;

    static const RcPtr<Dui::CTheme>& TmDefaultTheme(BOOL bDark) noexcept;
    static const RcPtr<Dui::CColorCollection>& TmDefaultColorCollection(BOOL bDark) noexcept;
    static const RcPtr<Dui::CMetricCollection>& TmDefaultMetricCollection() noexcept;

    LRESULT OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept override;

    EckInline void SetAtlas(RefPtr<CVioletAtlas> p) noexcept { m_pAtlas = std::move(p); }
    EckInlineNdCe auto& GetAtlas() const noexcept { return m_pAtlas; }
};