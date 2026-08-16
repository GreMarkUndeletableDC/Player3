#pragma once
#include "CPlayer.h"
#include "CPlayListMgr.h"

constexpr PCWSTR MainWndPageName[]
{
    L"主页",
    L"列表",
    L"效果",
    L"设置",
};

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

    IDT_COMM_TICK,
    IDT_LRC_MOUSELEAVE,

    ELEID_PLAYPAGE_BACK,
    ELEID_VOLBAR_TRACK,

    TE_COMM_TICK = 200,
    TE_PROG = TE_COMM_TICK * 2,
    TE_LRC_MOUSELEAVE = 800,
    TE_LRC_MOUSELEAVE_FIRST = 1600,
};

enum
{
    ELEN_PLACEHOLDER = Dui::ENC_PRIVATE_BEGIN,
    ELEN_PAGE_CHANGE,		// [CTabPanel]边栏被单击时(NMLTITEMINDEX*)
    ELEN_MINICOVER_CLICK,	// [CMiniCover]封面被单击时
    ELEN_DTLRC_GET_TIME,	// [CVeDesktopLyric]取当前播放器时间(NM_DTL_GET_TIME*)
    ELEN_PLAYPAGE_LBTN_UP,	// [CPagePlaying]左键弹起
};

class CWindowMain;
class CApplication
{
private:
    CPlayer m_Player{};
    CPlayListManager m_ListManager{};

    eck::CDWriteFontFactory m_FontFactory{};

    eck::ThreadContext* m_ptcUiThread{};
    CWindowMain* m_pWndMain{};
public:
    CApplication();

    static void Init();

    EckInlineNdCe auto& Player() { return m_Player; }
    EckInlineNdCe auto& ListManager() { return m_ListManager; }
    EckInlineNdCe auto& GetFontFactory() { return m_FontFactory; }
    EckInlineNdCe auto& GetMainWindow() { return *m_pWndMain; }

    EckInlineNdCe auto UiThreadContext() const { return m_ptcUiThread; }

    EckInlineCe void SetMainWindow(CWindowMain* pWnd) { m_pWndMain = pWnd; }
};

extern CApplication* App;