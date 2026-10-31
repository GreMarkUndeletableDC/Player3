#pragma once
#include "CImageManager.h"

class CVeBase : public Dui::CElement
{
public:
    const static inline UINT IdCrVolumeBarBackground = Dui::TmNextResourceId();
    const static inline UINT IdCrVolumeBarBorder = Dui::TmNextResourceId();
    const static inline UINT IdCrPalyPageMask = Dui::TmNextResourceId();
    const static inline UINT IdCrTabPanelBackground = Dui::TmNextResourceId();
    const static inline UINT IdCrPlayPanelBackground = Dui::TmNextResourceId();
    const static inline UINT IdCrMiniCoverMask = Dui::TmNextResourceId();

    constexpr static ARGB
        CrlVolumeBarBackground = 0xA0'FFFFFF,
        CrlVolumeBarBorder = Dui::CrlBorder,
        CrlPalyPageMask = 0x20'FFFFFF,
        CrlTabPanelBackground = 0xA0'FFFFFF,
        CrlPlayPanelBackground = 0xA0'FFFFFF,
        CrlMiniCoverMask = 0x40'FFFFFF;
    constexpr static ARGB
        CrdVolumeBarBackground = 0x50'000000,
        CrdVolumeBarBorder = Dui::CrdBorder,
        CrdPalyPageMask = 0x30'000000,
        CrdTabPanelBackground = 0x50'000000,
        CrdPlayPanelBackground = 0x50'000000,
        CrdMiniCoverMask = 0x30'000000;
public:
    static RcPtr<Dui::CTheme> TmMakeDefaultTheme(BOOL bDark) noexcept;
    static RcPtr<Dui::CColorCollection> TmMakeDefaultColorCollection(BOOL bDark) noexcept;
    static RcPtr<Dui::CMetricCollection> TmMakeDefaultMetricCollection() noexcept;

    static const RcPtr<Dui::CTheme>& TmDefaultTheme(BOOL bDark) noexcept;
    static const RcPtr<Dui::CColorCollection>& TmDefaultColorCollection(BOOL bDark) noexcept;
    static const RcPtr<Dui::CMetricCollection>& TmDefaultMetricCollection() noexcept;

    LRESULT OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept override;

    CImageManager& GetImageManager() const noexcept;
    Dui::CFilterBlur& GetFilterBlur() const noexcept;
    eck::CD2DShadow& GetShadow() const noexcept;
};