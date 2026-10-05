#include "pch.h"
#include "CWindowMain.h"
#include "CApplication.h"

const static UINT MessageTaskbarButtonCreated{ RegisterWindowMessageW(L"TaskbarButtonCreated") };

constexpr static float PageSwitchAnimationDelta = 60.f;

constexpr static std::wstring_view PageName[]
{
    L"主页"sv,
    L"列表"sv,
    L"效果"sv,
    L"设置"sv,
};

constexpr static UINT_PTR IDT_COMM_TICK = 101;
constexpr static UINT TE_COMM_TICK = 200;
constexpr static UINT TE_PROG = TE_COMM_TICK * 2;

EckInlineNdCe AppImage AutoNextModeToAppImage(AutoNextMode eMode) noexcept
{
    switch (eMode)
    {
    case AutoNextMode::ListLoop:   return AppImage::Circle;
    case AutoNextMode::List:       return AppImage::ArrowRight3;
    case AutoNextMode::Random:     return AppImage::ArrowCross;
    case AutoNextMode::SingleLoop: return AppImage::CircleOne;
    case AutoNextMode::Single:     return AppImage::ArrowRight1;
    }
    return AppImage::Circle;
}

BOOL CWindowMain::OnCreate(HWND hWnd, CREATESTRUCT* pcs) noexcept
{
    m_pImageManager->PrepareRealization(RdGetDC());
    m_pImageManager->AtlasInitialize();
    m_pImageManager->AtlasRealize();
    m_pImageManager->CoverInitialize();
    m_pImageManager->CoverRealize();
    m_pImageManager->SingleInitialize();

    m_pUxWndTheme->LoadDefaultTheme();
    const auto spUxAtlas = m_pUxWndTheme->GetAtlasImageData();
    if (!spUxAtlas.empty())
    {
        ComPtr<IWICBitmapSource> pWicBitmap;
        eck::CStreamView Stream{ spUxAtlas };
        eck::WicLoadSource(pWicBitmap.Self(), &Stream);
        RdGetDC()->CreateBitmapFromWicBitmap(
            pWicBitmap.Get(), nullptr, m_pUxWndThemeAtlas.AtClear());
    }

    CBass::Initialize();
    App->Player().GetEventChain().Connect(this, &CWindowMain::OnPlayEvent);

    KctStartTimer();
    KctRegisterTimeLine(this);
    RdSetBackColor(0);

    MARGINS m{};// 不能使用-1，否则会绘制标准标题栏
    m.cxLeftWidth = 65536 * 4;
    DwmExtendFrameIntoClientArea(hWnd, &m);
    eck::EnableWindowMica(hWnd);

    SmtcInitialize();

    m_pFilterBlur->Attach(this);

    InitializeUi();

    OnColorSchemeChanged();
    PageShow(Page::List, FALSE);
    // TODO: 选中
    //m_TabPanel.GetTabList().GetController().(1);
    return TRUE;
}

void CWindowMain::PageShow(Page ePage, BOOL bAnimate) noexcept
{
    __assume(ePage < Page::Max);
    const int idxShow = (int)ePage;
    const int idxShowLast = (int)m_eCurrPage;
    PageClearAnimation();
    if (idxShow == idxShowLast)
        return;
    m_eCurrPage = ePage;

    const auto bAlreadyVisible =
        (m_vPage[idxShow]->GetStyle() & Dui::DES_VISIBLE);

    m_LAPageTitle.SetText(PageName[idxShow]);
    m_LAPageTitle.Invalidate();
    m_vPage[idxShow]->SetVisible(TRUE);
    for (int i = 0; i < idxShow; ++i)
        m_vPage[i]->SetVisible(FALSE);
    for (int i = idxShow + 1; i < (int)Page::Max; ++i)
        m_vPage[i]->SetVisible(FALSE);

    if (bAnimate)
    {
        if (bAlreadyVisible)
            return;// 已经显示，不需要动画
        m_ecPage.Start(0, (float)PageSwitchAnimationDelta, !!m_pAnPage);
        m_bPageAnUpToDown = (idxShow < idxShowLast);
        m_pAnPage = m_vPage[idxShow];
        KctWake();
    }
}

void CWindowMain::PageClearAnimation() noexcept
{
    if (!m_pAnPage)
        return;
    m_pAnPage = nullptr;
}

HWND CWindowMain::Create(PCWSTR pszText, DWORD dwStyle, DWORD dwExStyle,
    int x, int y, int cx, int cy, HWND hParent, HMENU hMenu, void* pParam) noexcept
{
    TblCreateGhostWindow(pszText);
    TblInitialize();
    const auto hWnd = __super::Create(pszText, dwStyle, dwExStyle,
        x, y, cx, cy, hParent, hMenu, pParam);
    TblOnTaskbarButtonCreated();
    return hWnd;
}

void CWindowMain::OnPlayEvent(const PLAY_EVT_PARAM& e) noexcept
{
    switch (e.eEvent)
    {
    case PlayEvent::CommonTick:
    {
        m_msProgTimer += TE_COMM_TICK;
        if (m_msProgTimer >= TE_PROG)
        {
            m_msProgTimer = 0;
            m_TBProgress.SetTrackPosition(float(
                App->Player().GetCurrentTime() * ProgressTrackBarScale));
            m_TBProgress.Invalidate();
            TblUpdateProgress();
        }
        SmtcOnCommonTick();
    }
    break;
    case PlayEvent::Play:
    {
        m_pImageManager->CoverUpdate(App->Player().GetCover().Get());
        m_PagePlaying.UpdateBlurredCover();
        m_PlayPageAnimator.SetOverlayBitmap(m_pImageManager->CoverGetD2D());

        m_msProgTimer = 0;
        SmtcUpdateTimeLineRange();
        SmtcUpdateDisplay();
        TblUpdateProgress();
        if (m_PagePlaying.GetStyle() & Dui::DES_VISIBLE)
            m_PagePlaying.Invalidate();
        m_TBProgress.SetRange(0.f, float(App->Player().GetTotalTime() * ProgressTrackBarScale));
        m_TBProgress.SetTrackPosition(0.f);
        m_TBProgress.Invalidate();
        m_PlayPanel.Invalidate();
        m_WndTbGhost.InvalidateThumbnailCache();
        m_WndTbGhost.InvalidateDwmThumbnail();
        m_WndTbGhost.SetIconicThumbnail();
    }
    [[fallthrough]];
    case PlayEvent::Resume:
    {
        SetTimer(Handle, IDT_COMM_TICK, TE_COMM_TICK, nullptr);
        m_BTPlay.SetIcon(m_pImageManager->AtlasGetD2D(AppImage::Pause));
        m_BTPlay.Invalidate();
        TblUpdateState();
        SmtcUpdateState();
    }
    break;
    case PlayEvent::Stop:
    {
        m_TBProgress.SetTrackPosition(0.f);
        m_TBProgress.Invalidate();
        TblUpdateProgress();
    }
    [[fallthrough]];
    case PlayEvent::Pause:
    {
        KillTimer(Handle, IDT_COMM_TICK);
        m_BTPlay.SetIcon(m_pImageManager->AtlasGetD2D(AppImage::Triangle));
        m_BTPlay.Invalidate();
        TblUpdateState();
        SmtcUpdateState();
    }
    break;
    }
}

LRESULT CWindowMain::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
    if (uMsg == MessageTaskbarButtonCreated)
    {
        if (m_pTaskbarList.Get())
            TblOnTaskbarButtonCreated();
        return 0;
    }
    switch (uMsg)
    {
    case WM_TIMER:
        if (wParam == IDT_COMM_TICK)
            App->Player().GetEventChain().Emit({ PlayEvent::CommonTick });
        break;
    case WM_SIZE:
    {
        RdLockUpdate();
        const auto lResult = __super::OnMessage(uMsg, wParam, lParam);
        OnSize();
        RdUnlockUpdate();
        return lResult;
    }

    case WM_NCCALCSIZE:
    {
        const auto cxFrame = eck::DaGetSystemMetrics(SM_CXFRAME, GetWindowDpi());
        const auto cyFrame = eck::DaGetSystemMetrics(SM_CYFRAME, GetWindowDpi());
        const auto cxPadded = eck::DaGetSystemMetrics(SM_CXPADDEDBORDER, GetWindowDpi());
        return eck::MsgOnNcCalculateSize(wParam, lParam,
            { cxFrame + cxPadded,cxFrame + cxPadded,0,cyFrame + cxPadded });
    }
    break;

    case WM_COMMAND:
        if (TblOnCommand(wParam))
            return 0;
        break;

    case WM_CREATE:
        __super::OnMessage(uMsg, wParam, lParam);
        return HANDLE_WM_CREATE(Handle, wParam, lParam, OnCreate);
    case WM_DESTROY:
        KillTimer(Handle, IDT_COMM_TICK);
        __super::OnMessage(uMsg, wParam, lParam);
        m_WndTbGhost.Destroy();
        SmtcUninitialize();
        PostQuitMessage(0);
        return 0;
    case WM_SYSCOLORCHANGE:
        eck::MsgOnSystemColorChangeMainWindow(Handle, wParam, lParam);
        break;
    case WM_SETTINGCHANGE:
    {
        if (eck::MsgOnSettingChangeMainWindow(Handle, wParam, lParam, TRUE))
        {
            OnColorSchemeChanged();
            TblUpdateToolBarIcon();
            m_WndTbGhost.InvalidateDwmThumbnail();
            m_WndTbGhost.InvalidateThumbnailCache();
            m_WndTbGhost.SetIconicThumbnail();
        }
    }
    break;
    case WM_DPICHANGED:
        SetUserDpi(LOWORD(wParam));
        break;
    case WM_DWMCOLORIZATIONCOLORCHANGED:
        //StUpdateColorizationColor();
        break;
    case WM_SETTEXT:
    {
        const auto lResult = __super::OnMessage(uMsg, wParam, lParam);
        if (lResult)
            m_WndTbGhost.SetText((PCWSTR)lParam);
        return lResult;
    }
    break;
    }
    return __super::OnMessage(uMsg, wParam, lParam);
}

LRESULT CWindowMain::OnElementNotify(Dui::CElement* pEle, Dui::ELENMHDR* pnm) noexcept
{
    switch (pnm->uNotify)
    {
    case ELEN_PAGE_CHANGE:
    {
        //const auto* const p = (Dui::NMLTITEMINDEX*)lParam;
        //PageShow((Page)p->idx, TRUE);
    }
    return 0;

    case Dui::ENC_POSCHANGED:
    {
        if (pEle == &m_TBProgress)
        {
            App->Player().SetPosition(
                m_TBProgress.GetTrackPosition() / ProgressTrackBarScale);
            return 0;
        }
        else if (pEle->GetId() == ELEID_VOLBAR_TRACK)
        {
            const auto f = ((Dui::CTrackBar*)pEle)->GetTrackPosition();
            App->Player().GetBass().SetVolume(f / 100.f);
            m_VolBar.OnVolumeChanged(f);
        }
    }
    return 0;
    case ELEN_PLAYPAGE_LBTN_UP:
    {
        if (m_PlayPageAnimator.PpaIsActive())
        {
            PpaStart();
            KctWake();
        }
    }
    return 0;

    case Dui::ENC_COMMAND:
    {
        if (pEle == &m_BTPlay)
            App->Player().PlayOrPause();
        else if (pEle == &m_BTPrev)
            App->Player().Previous();
        else if (pEle == &m_BTNext)
            App->Player().Next();
        //else if (pEle == &m_BTLrc)
        else if (pEle == &m_BTAutoNext)
        {
            const auto r = App->Player().NextAutoNextMode();
            m_BTAutoNext.SetIcon(m_pImageManager->AtlasGetD2D(AutoNextModeToAppImage(r)));
            m_BTAutoNext.Invalidate();
        }
        else if (pEle == &m_BTVol)
        {
            const auto x = GetClientWidthLogical() - VolumeBarWidth - VolumeBarRightPadding;
            const auto y = m_BTVol.GetOffsetInClient().y - VolumeBarHeight;
            m_VolBar.SetPosition(x, y);
            m_VolBar.ShowAnimation();
        }
        else if (
            pEle->GetId() == ELEID_PLAYPAGE_BACK ||
            pEle->GetId() == ELEID_MINICOVER)
        {
            PpaStart();
            KctWake();
        }
    }
    break;
    }
    return __super::OnElementNotify(pEle, pnm);
}

void CWindowMain::TlTick(int ms) noexcept
{
    if (m_PlayPageAnimator.PpaIsActive())
    {
        const auto bStillRunning = m_PlayPageAnimator.PpaTick((float)ms);

        const auto kScale = 1.f - m_PlayPageAnimator.PpaCurrentValue() * 0.2f;
        const auto xRef = GetClientWidthLogical() / 2.f;
        const auto yRef = GetClientHeightLogical() / 2.f;
        m_CompNormalPageAn.SetMatrix(
            D2D1::Matrix3x2F::Scale(kScale, kScale, { xRef, yRef }));
        m_CompNormalPageAn.SetOpacity(1.f - m_PlayPageAnimator.PpaCurrentValue());

        if (!m_PagePlaying.GetCompositor())
            m_PagePlaying.SetCompositor(&m_PlayPageAnimator);
        if (!m_NormalPageContainer.GetCompositor())
        {
            m_NormalPageContainer.SetCompositor(&m_CompNormalPageAn);
            m_NormalPageContainer.SetStyle(Dui::DES_BASE_BEGIN_END_PAINT |
                m_NormalPageContainer.GetStyle());
        }
        m_PagePlaying.CompUpdateCompositedRect();
        m_NormalPageContainer.CompUpdateCompositedRect();
        if (!bStillRunning)
            PpaEnd();
        RdInvalidate(FALSE);
    }
    if (m_pAnPage)
    {
        const auto bActive = m_ecPage.Tick((float)ms, 250.f);

        const auto x = m_pAnPage->GetRect().left;
        constexpr float yNormal = PageTitleHeight + PageTitleTopPosition + PageInnerPadding;
        if (m_bPageAnUpToDown)
            m_pAnPage->SetPosition(x, yNormal +
                PageSwitchAnimationDelta - m_ecPage.K);
        else
            m_pAnPage->SetPosition(x, yNormal -
                PageSwitchAnimationDelta + m_ecPage.K);
        if (!bActive)
            m_pAnPage = nullptr;
    }
}

BOOL CWindowMain::TlIsValid() noexcept
{
    return m_PlayPageAnimator.PpaIsActive() || !!m_pAnPage;
}

//void CWindowMain::LwShow(BOOL bShow)
//{
//    if (bShow)
//    {
//        if (m_WndLrc.IsValid())
//            m_WndLrc.Show(SW_SHOWNOACTIVATE);
//        else
//        {
//            m_WndLrc.SetPresentMode(Dui::PresentMode::UpdateLayeredWindow);
//            m_WndLrc.SetTransparent(TRUE);
//            m_WndLrc.SetUserDpi(GetUserDpi());
//            m_WndLrc.Create(L"VioletModel - Lyrics", WS_POPUP | WS_VISIBLE,
//                WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST,
//                300, 400, 500, 300, Handle, nullptr);
//        }
//        m_WndLrc.RdInvalidate();
//    }
//    else
//        if (m_WndLrc.IsValid())
//            m_WndLrc.Show(SW_HIDE);
//}
//
//BOOL CWindowMain::LwIsShowing()
//{
//    return m_WndLrc.IsValid() && m_WndLrc.IsVisible();
//}

void CWindowMain::PpaStart() noexcept
{
    m_PlayPageAnimator.PpaStart();
    if (!m_PlayPageAnimator.PpaIsReverse())
    {
        m_NormalPageContainer.SetVisible(TRUE);
        m_PlayPanel.GetCoverElement().SetVisible(TRUE);
        m_PlayPanel.SetVisible(TRUE);
    }
    m_PagePlaying.SetVisible(TRUE);
    KctWake();
}

void CWindowMain::PpaEnd() noexcept
{
    m_PagePlaying.SetCompositor(nullptr);
    m_NormalPageContainer.SetCompositor(nullptr);
    m_NormalPageContainer.SetStyle(m_NormalPageContainer.GetStyle() &
        ~Dui::DES_BASE_BEGIN_END_PAINT);
    if (m_PlayPageAnimator.PpaIsReverse())
    {
        m_NormalPageContainer.SetVisible(FALSE);
        m_PlayPanel.SetVisible(FALSE);
    }
    else
        m_PagePlaying.SetVisible(FALSE);
    m_PlayPageAnimator.PpaEnd();
}

void CWindowMain::OnColorSchemeChanged() noexcept
{
    m_BTPrev.SetIcon(m_pImageManager->AtlasGetD2D(AppImage::Previous));
    m_BTPlay.SetIcon(m_pImageManager->AtlasGetD2D(AppImage::Triangle));
    m_BTNext.SetIcon(m_pImageManager->AtlasGetD2D(AppImage::Next));
    m_BTAutoNext.SetIcon(m_pImageManager->AtlasGetD2D(
        AutoNextModeToAppImage(App->Player().GetAutoNextMode())));
    m_BTLrc.SetIcon(m_pImageManager->AtlasGetD2D(AppImage::Lyric));
    m_BTVol.SetIcon(m_pImageManager->AtlasGetD2D(AppImage::Speaker));
}