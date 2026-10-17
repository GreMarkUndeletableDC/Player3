#include "pch.h"
#include "CPlayPanel.h"
#include "CApplication.h"


void CPlayPanel::OnPlayEvent(const PLAY_EVT_PARAM& e) noexcept
{
    switch (e.eEvent)
    {
    case PlayEvent::CommonTick:
    {
        const auto& Player = App->Player();
        const auto lfCurrTime = Player.GetCurrentTime();
        const auto lfTotalTime = Player.GetTotalTime();
        m_LATime.SetText(eck::Format(L"%02d:%02d/%02d:%02d",
            int(lfCurrTime / 60.), int(lfCurrTime) % 60,
            int(lfTotalTime / 60.), int(lfTotalTime) % 60).Data());
        m_LATime.Invalidate();
    }
    break;
    case PlayEvent::Play:
    {
        const auto& mi = App->Player().GetMusicSimpleData();
        m_LATitle.SetText(mi.rsTitle.Data());
        m_LAArtist.SetText(mi.slArtist.FrontData());
    }
    break;
    }
}

LRESULT CPlayPanel::OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
    switch (uMsg)
    {
    case WM_PAINT:
    {
        Dui::PAINTINFO ps;
        BeginPaint(ps, wParam, lParam);

        if (m_bEnableBlur)
        {
            GetDC()->Flush();
            GetFilterBlur()->FilterDC(ps.rcClip, ps.ox, ps.oy);
        }
        GetDC()->FillRectangle(ps.rcClip, GetWindow().CcSetBrushColor(
            GetTheme()->GetColorD2D(IdCrPlayPanelBackground)));

        EndPaint(ps);
    }
    return 0;

    case WM_SETFONT:
    {
        m_LAArtist.SetTextFormat(GetTextFormat().Get());
        m_LATime.SetTextFormat(GetTextFormat().Get());
    }
    break;

    case WM_CREATE:
    {
        __super::OnEvent(uMsg, wParam, lParam);

        App->Player().GetEventChain().Connect(this, &CPlayPanel::OnPlayEvent);

        const auto pWnd = &GetWindow();

        ComPtr<IDWriteTextFormat> pTfTitle;
        App->FontFactory().NewFont(pTfTitle.Self(), eck::Alignment::Near,
            eck::Alignment::Center, NormalFontSize, 700);
        pTfTitle->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

        float x = MiniCoverLeftPosition;
        m_Cover.Create({}, Dui::DES_VISIBLE | Dui::DES_NOTIFY_WND, 0,
            x, MiniCoverTopPosition, MiniCoverSize, MiniCoverSize, this, pWnd);
        m_Cover.SetId(ELEID_MINICOVER);
        x += (MiniCoverSize + PlayPanelTextPadding);

        float y = TitleTopPosition;
        m_LATitle.Create(L"Player3", Dui::DES_VISIBLE, 0,
            x, y, TitleArtistMaximumWidth, PlayPanelTextHeight, this, pWnd);
        m_LATitle.SetFade(TRUE);
        m_LATitle.SetTextFormat(pTfTitle.Get());
        y += (PlayPanelTextHeight + TitleArtistPadding);

        m_LAArtist.Create(L"VC++/Win32", Dui::DES_VISIBLE, 0,
            x, y, TitleArtistMaximumWidth, PlayPanelTextHeight, this, pWnd);
        m_LAArtist.SetFade(TRUE);
        x += (TitleArtistMaximumWidth + PlayPanelTextPadding);

        m_LATime.Create(L"00:00/00:00", /*Dui::DES_VISIBLE*/0, 0,
            x, TimeTopPosition, TimeMaximumWidth, PlayPanelTextHeight, this, pWnd);
    }
    return 0;
    }
    return __super::OnEvent(uMsg, wParam, lParam);
}