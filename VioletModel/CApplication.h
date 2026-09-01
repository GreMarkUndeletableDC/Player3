#pragma once
#include "CPlayer.h"
#include "CPlayListManager.h"

constexpr static float
PlayPanelHeight = 90,
PageInnerPadding = 10.f,
MiniCoverLeftPosition = 40.f,
MiniCoverTopPosition = 10.f,
MiniCoverSize = 70.f,

NormalFontSize = 14,

CxyLrcBtn = 30,
CxyLrcPadding = 8
;

// 所有的ID，包括窗口定时器、WM_COMMAND、控件ID等
enum
{
    VIOLET_ID_BEGIN = 0x514B,

    ELEID_PLAYPAGE_BACK,
    ELEID_MINICOVER,
    ELEID_VOLBAR_TRACK,
};

enum
{
    ELEN_PLACEHOLDER = Dui::ENC_PRIVATE_BEGIN,
    ELEN_PAGE_CHANGE,       // [CTabPanel]边栏被单击时(NMLTITEMINDEX*)
    ELEN_DTLRC_GET_TIME,    // [CVeDesktopLyric]取当前播放器时间(NM_DTL_GET_TIME*)
    ELEN_PLAYPAGE_LBTN_UP,  // [CPagePlaying]左键放开
};

class CWindowMain;
class CApplication
{
private:
    CPlayer m_Player{};
    CPlayListManager m_ListManager{};

    eck::CDWriteFontFactory m_FontFactory{};

    eck::ThreadContext* m_ptcUiThread{};
public:
    CApplication() noexcept;

    EckInlineNdCe auto& Player() noexcept { return m_Player; }
    EckInlineNdCe auto& ListManager() noexcept { return m_ListManager; }
    EckInlineNdCe auto& FontFactory() noexcept { return m_FontFactory; }

    EckInlineNdCe auto UiThreadContext() const noexcept { return m_ptcUiThread; }
};

extern CApplication* App;

Tag::Result ReadMetadata(
    _In_z_ PCWSTR pszFile,
    _Inout_ Tag::SimpleData& mi,
    const Tag::SIMPLE_OPT& Opt) noexcept;