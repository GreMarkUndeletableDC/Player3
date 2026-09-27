#include "pch.h"
#include "CWindowMain.h"
#include "CApplication.h"

constexpr static float LabelFontHeight = 18.f;

void CWindowMain::InitializeUi() noexcept
{
    GetUiHookEventChain().Connect(
        [this](Dui::CElement* pEle, const Dui::UIHOOK_EVENT* puhe, eck::Slot&) noexcept -> LRESULT
        {
            switch (puhe->uEvent)
            {
            case Dui::UIHE_PRECREATE:
            {
                const auto p = dynamic_cast<CVeBase*>(pEle);
                if (p)
                    p->SetImageManager(m_pImageManager);
            }
            break;
            case Dui::UIHE_CREATE:
            {
                if (eck::PtcCurrent()->bAppDarkMode)
                    pEle->SetStyle(pEle->GetStyle() | Dui::DES_DARK_MODE | Dui::DES_DBG_FRAME);
            }
            break;
            }
            return 0;
        });

    ComPtr<IDWriteTextFormat> pTfPageTitle, pTfLeft, pTfCenter;
    App->FontFactory().NewFont(pTfPageTitle.Self(), eck::Alignment::Near,
        eck::Alignment::Center, (float)PageTitleFontHeight, 600);
    pTfPageTitle->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    App->FontFactory().NewFont(pTfLeft.Self(), eck::Alignment::Near,
        eck::Alignment::Center, (float)NormalFontSize);
    pTfLeft->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    App->FontFactory().NewFont(pTfCenter.Self(), eck::Alignment::Center,
        eck::Alignment::Center, (float)NormalFontSize);
    pTfCenter->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

    m_NormalPageContainer.Create({}, Dui::DES_VISIBLE, 0,
        0, 0, 0, 0, nullptr, this);
    const auto pNormalParent = &m_NormalPageContainer;
    // 左侧选择夹
    m_TabPanel.Create({}, Dui::DES_VISIBLE | Dui::DES_NOTIFY_WND, 0,
        0, 0, 0, 0, pNormalParent, this);
    // 标题
    m_LAPageTitle.Create({}, Dui::DES_VISIBLE, 0,
        0, 0, 0, 0, pNormalParent, this);
    m_LAPageTitle.SetTextFormat(pTfPageTitle.Get());
    // 页 主页
    m_PageMain.Create({}, Dui::DES_VISIBLE, 0,
        0, 0, 0, 0, pNormalParent, this);
    m_PageMain.SetTextFormat(pTfCenter.Get());
    // 页 列表
    m_PageList.Create({}, Dui::DES_VISIBLE, 0,
        0, 0, 0, 0, pNormalParent, this);
    m_PageList.SetTextFormat(pTfLeft.Get());
    // 页 效果
    m_PageEffect.Create({}, Dui::DES_VISIBLE, 0,
        0, 0, 0, 0, pNormalParent, this);
    m_PageEffect.SetTextFormat(pTfLeft.Get());
    // 页 设置
    m_PageOptions.Create({}, Dui::DES_VISIBLE, 0,
        0, 0, 0, 0, pNormalParent, this);
    m_PageOptions.SetTextFormat(pTfLeft.Get());
    // 底部播放控制栏
    m_PlayPanel.Create({}, Dui::DES_VISIBLE/* | Dui::DES_BLUR_BACK*/, 0,
        0, 0, 0, 0, pNormalParent, this);
    m_PlayPanel.SetTextFormat(pTfLeft.Get());
    // 页 播放
    ComPtr<IDWriteTextFormat> pTfPP;
    m_PagePlaying.Create({}, 0, 0,
        0, 0, 0, 0, nullptr, this);
    m_PagePlaying.SetTextFormat(pTfLeft.Get());
    App->FontFactory().NewFont(pTfPP.SelfClear(), eck::Alignment::Near,
        eck::Alignment::Center, (float)LabelFontHeight, 600);
    pTfPP->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    m_PagePlaying.SetLabelTextFormatTitle(pTfPP.Get());
    App->FontFactory().NewFont(pTfPP.SelfClear(), eck::Alignment::Near,
        eck::Alignment::Center, (float)LabelFontHeight);
    pTfPP->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    m_PagePlaying.SetLabelTextFormat(pTfPP.Get());
    // 进度条
    m_TBProgress.Create({}, Dui::DES_VISIBLE | Dui::DES_NOTIFY_WND, 0,
        0, 0, ProgressBarWidth, ProgressBarHeight, nullptr, this);
    m_TBProgress.SetRange(0, 100);
    m_TBProgress.SetTrackPosition(50);
    m_TBProgress.SetTrackSize(ProgressBarTrackHeight);
    m_TBProgress.SetThumbSize(ProgressBarThumbSize);
    m_TBProgress.SetThinTrack(TRUE);
    // 按钮 上一曲
    m_BTPrev.Create({}, Dui::DES_VISIBLE | Dui::DES_NOTIFY_WND, 0,
        0, 0, CircleButtonSize, CircleButtonSize, nullptr, this);
    // 按钮 播放/暂停
    m_BTPlay.Create({}, Dui::DES_VISIBLE | Dui::DES_NOTIFY_WND, 0,
        0, 0, PlayCircleButtonSize, PlayCircleButtonSize, nullptr, this);
    // 按钮 下一曲
    m_BTNext.Create({}, Dui::DES_VISIBLE | Dui::DES_NOTIFY_WND, 0,
        0, 0, CircleButtonSize, CircleButtonSize, nullptr, this);
    // 按钮 播放模式
    m_BTAutoNext.Create({}, Dui::DES_VISIBLE | Dui::DES_NOTIFY_WND, 0,
        0, 0, CircleButtonSize, CircleButtonSize, nullptr, this);
    m_BTAutoNext.GetEventChain().Connect(
        [](UINT uMsg, WPARAM, LPARAM, eck::Slot&) -> LRESULT
        {
            if (uMsg == WM_RBUTTONDOWN)
            {
                const auto pList = App->Player().GetList();
                if (pList)
                    pList->FlShuffleRandom();
            }
            return 0;
        });
    // 按钮 歌词
    m_BTLrc.Create({}, Dui::DES_VISIBLE | Dui::DES_NOTIFY_WND, 0,
        0, 0, CircleButtonSize, CircleButtonSize, nullptr, this);
    // 按钮 音量
    m_BTVol.Create({}, Dui::DES_VISIBLE | Dui::DES_NOTIFY_WND, 0,
        0, 0, CircleButtonSize, CircleButtonSize, nullptr, this);
    // 标题栏
    m_TitleBar.Create({}, Dui::DES_VISIBLE, 0,
        0, 0, 0, 0, nullptr, this);
    m_TitleBar.SetUxDwmWindowTheme(m_pUxWndTheme);
    m_TitleBar.SetThemeAtlas(m_pUxWndThemeAtlas.Get());
    m_TitleBar.SynchronizeToSystemMenu();
    // 音量条
    m_VolBar.Create({}, 0, 0,
        0, 0, VolumeBarWidth, VolumeBarHeight, nullptr, this);
    m_VolBar.SetTextFormat(pTfCenter.Get());
    //
    UpdateButtonImageSize();
    m_PagePlaying.UpdateBlurredCover();
}

void CWindowMain::OnSize() noexcept
{
    PageClearAnimation();
    const auto cxClient = GetClientWidthLogical();
    const auto cyClient = GetClientHeightLogical();
    m_NormalPageContainer.SetRect({ 0, 0, cxClient, cyClient });
    m_TitleBar.SetRect({ 0, 0, cxClient, TitleBarElementHeight });
    m_TabPanel.SetRect({ 0, 0, TabPanelWidth, cyClient - PlayPanelHeight });

    const auto yPlayPanel = cyClient - PlayPanelHeight;
    m_PlayPanel.SetRect({ 0, cyClient - PlayPanelHeight, cxClient, cyClient });

    m_LAPageTitle.SetRect({
        TabPanelWidth + TabToPagePadding,
        PageTitleTopPosition,
        TabPanelWidth + TabToPagePadding + PageTitleWidth,
        PageTitleTopPosition + PageTitleHeight });

    m_PagePlaying.SetRect({ 0, 0, cxClient, cyClient });

    D2D1_RECT_F rcMini;
    rcMini.left = MiniCoverLeftPosition;
    rcMini.top = float(cyClient - PlayPanelHeight + MiniCoverTopPosition);
    rcMini.right = rcMini.left + (float)MiniCoverSize;
    rcMini.bottom = rcMini.top + (float)MiniCoverSize;
    m_PlayPageAnimator.PpaSetRect(
        rcMini,
        { 0.f, 0.f, cxClient, cyClient });

    for (auto& e : m_vPage)
        e->SetRect({
            TabPanelWidth + TabToPagePadding,
            PageTitleHeight + PageTitleTopPosition + PageInnerPadding,
            cxClient,
            cyClient - TabToPagePadding });
    LayoutPlayPanel();
}

void CWindowMain::LayoutPlayPanel() noexcept
{
    const auto cxClient = GetClientWidthLogical();
    const auto cyClient = GetClientHeightLogical();
    float x, y;
    // 移动右侧按钮
    x = cxClient - FirstCircleButtonRightPadding - CircleButtonSize;
    y = cyClient - PlayPanelHeight + (PlayPanelHeight - CircleButtonSize) / 2;
    m_BTVol.SetPosition(x, y);
    x -= (CircleButtonSize + CircleButtonPadding);
    m_BTLrc.SetPosition(x, y);
    x -= (CircleButtonSize + CircleButtonPadding);
    m_BTAutoNext.SetPosition(x, y);
    // 移动中间按钮
    x = (cxClient - (CircleButtonSize * 2 + PlayCircleButtonSize +
        CircleButtonPadding * 2)) / 2;
    y = cyClient - PlayPanelHeight + ControlButtonTopPosition;

    m_BTPrev.SetPosition(x, y);
    x += (CircleButtonSize + CircleButtonPadding);
    m_BTPlay.SetPosition(x, y + (CircleButtonSize - PlayCircleButtonSize) / 2);
    x += (PlayCircleButtonSize + CircleButtonPadding);
    m_BTNext.SetPosition(x, y);
    // 移动进度条
    m_TBProgress.SetPosition(
        (cxClient - ProgressBarWidth) / 2.f,
        cyClient - ProgressBarHeight - 6.f);
}

void CWindowMain::UpdateButtonImageSize() noexcept
{
    constexpr D2D1_SIZE_F Size{ CircleButtonIconSize, CircleButtonIconSize };
    //m_BTPrev.SetImageSize(Size);
    //m_BTPlay.SetImageSize(Size);
    //m_BTNext.SetImageSize(Size);
    //m_BTLrc.SetImageSize(Size);
    //m_BTVol.SetImageSize(Size);
}