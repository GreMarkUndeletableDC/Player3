#include "pch.h"
#include "CPagePlaying.h"
#include "CApp.h"

void CPagePlaying::UpdateBlurredCover() noexcept
{
    const auto cxEle = LogicalToPixel(GetWidth());
    const auto cyEle = LogicalToPixel(GetHeight());
    if (!cxEle || !cyEle)
        return;

    // -- 准备环境

    ComPtr<ID2D1Image> pOldTarget;
    GetDC()->GetTarget(&pOldTarget);
    GetDC()->SetTarget(m_pBitmapBlurredCover.Get());
    GetDC()->SetTransform(D2D1::Matrix3x2F::Identity());
    GetDC()->BeginDraw();
    GetDC()->Clear(D2D1::ColorF(D2D1::ColorF::White));// TODO:主题色
    float xDpi, yDpi;
    GetDC()->GetDpi(&xDpi, &yDpi);
    GetDC()->SetDpi(96.f, 96.f);

    const auto Cover = GetAtlas()->CoverGetD2D();
    const auto rcSrc = Cover.GetActualSourceRect();
    const auto cx0 = rcSrc.right - rcSrc.left;
    const auto cy0 = rcSrc.bottom - rcSrc.top;

    const D2D1_RECT_F rcEle{ 0.f, 0.f, cxEle, cyEle };
    D2D1_RECT_F rcScaled{ rcSrc };
    eck::AdjustRectToFillAnother(rcScaled, rcEle);

    ComPtr<ID2D1Image> pInput;
    D2D1::Matrix3x2F Mat{ D2D1::Matrix3x2F::Identity() };
    if (Cover.GetSourceRect())
    {
        ComPtr<ID2D1Effect> pFxCrop;
        GetDC()->CreateEffect(CLSID_D2D1Crop, &pFxCrop);
        pFxCrop->SetInput(0, Cover.Get());
        pFxCrop->SetValue(D2D1_CROP_PROP_RECT, rcSrc);
        pFxCrop->GetOutput(&pInput);
        Mat.dx = rcSrc.left;
        Mat.dy = rcSrc.top;
    }
    else
        pInput = Cover.Get();

    ComPtr<ID2D1Effect> pFxTransform, pFxCrop, pFxBlur;

    GetDC()->CreateEffect(CLSID_D2D12DAffineTransform, &pFxTransform);
    pFxTransform->SetInput(0, pInput.Get());
    Mat.m11 = (rcScaled.right - rcScaled.left) / cx0;
    Mat.m22 = (rcScaled.bottom - rcScaled.top) / cy0;
    Mat.dx += (rcScaled.left - Mat.m11 * rcSrc.left);
    Mat.dy += (rcScaled.top - Mat.m22 * rcSrc.top);
    pFxTransform->SetValue(
        D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX,
        Mat);
    pFxTransform->SetValue(
        D2D1_2DAFFINETRANSFORM_PROP_BORDER_MODE,
        D2D1_BORDER_MODE_HARD);
    pFxTransform->SetValue(
        D2D1_2DAFFINETRANSFORM_PROP_INTERPOLATION_MODE,
        D2D1_2DAFFINETRANSFORM_INTERPOLATION_MODE_NEAREST_NEIGHBOR);

    GetDC()->CreateEffect(CLSID_D2D1Crop, &pFxCrop);
    pFxCrop->SetInputEffect(0, pFxTransform.Get());
    pFxCrop->SetValue(D2D1_CROP_PROP_RECT, rcEle);

    GetDC()->CreateEffect(CLSID_D2D1GaussianBlur, &pFxBlur);
    pFxBlur->SetInputEffect(0, pFxCrop.Get());
    pFxBlur->SetValue(
        D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION,
        40.f);
    pFxBlur->SetValue(
        D2D1_GAUSSIANBLUR_PROP_BORDER_MODE,
        D2D1_BORDER_MODE_HARD);
    pFxBlur->SetValue(
        D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION,
        D2D1_GAUSSIANBLUR_OPTIMIZATION_SPEED);

    GetDC()->DrawImage(pFxBlur.Get(), { 0.f, 0.f });

    GetDC()->SetDpi(xDpi, yDpi);

    // -- 半透明遮罩

    GetDC()->FillRectangle(GetViewRectD2D(),
        GetWindow().CcSetBrushColor(GetTheme()->GetColorD2D(IdCrPalyPageMask)));

    GetDC()->EndDraw();
    GetDC()->SetTarget(pOldTarget.Get());

    m_Cover.SetBitmap(GetAtlas()->CoverGetD2D());
}

void CPagePlaying::OnPlayEvent(const PLAY_EVT_PARAM& e) noexcept
{
    switch (e.eEvent)
    {
    case PlayEvent::CommonTick:
    {
        m_Lyric.LrcSetCurrentLine(App->Player().GetCurrentLyricLine());
    }
    break;
    case PlayEvent::Play:
    {
        // 无需重画，UpdateBlurredCover调用后重画
        const auto& mi = App->Player().GetMusicSimpleData();
        m_LATitle.SetText(mi.rsTitle.Data());
        m_LAAlbum.SetText(mi.rsAlbum.Data());
        m_LAArtist.SetText(mi.slArtist.FrontData());

        m_Lyric.LrcInitialize(App->Player().GetLyric());
    }
    break;
    case PlayEvent::Stop:
    {
        m_Lyric.LrcClear();
        SetEmptyText();
    }
    break;
    }
}

void CPagePlaying::SetEmptyText() noexcept
{
    m_LATitle.SetText(L"Violet Model"sv);
    m_LAArtist.SetText(L"AuroraStudio"sv);
    m_LAAlbum.SetText(L"VC++/Win32"sv);
}

void CPagePlaying::OnColorSchemeChanged() noexcept
{
}

LRESULT CPagePlaying::OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
    switch (uMsg)
    {
    case WM_PAINT:
    {
        Dui::PAINTINFO ps;
        BeginPaint(ps, wParam, lParam);
        GetDC()->DrawBitmap(
            m_pBitmapBlurredCover.Get(),
            ps.rcClipInEle,
            1.f,
            D2D1_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
            ps.rcClipInEle);
        EndPaint(ps);
    }
    return 0;

    case WM_SIZE:
    {
        const auto cx = GetWidth();
        const auto cy = GetHeight();
        if (m_pBitmapBlurredCover)
        {
            const auto Size = m_pBitmapBlurredCover->GetSize();
            if (!(Size.width < cx || Size.width / 2.f > cx ||
                Size.height < cy || Size.height / 2.f > cy))
                goto SkipReCreate;
        }
        GetWindow().RdCreateBitmapLogical(
            cx, cy,
            m_pBitmapBlurredCover.Self());
    SkipReCreate:
        UpdateBlurredCover();

        const auto cxMinGap = cx * 1.f / 20.f;
        const Kw::Rect rcLyric
        {
            cx * 10.f / 20.f,
            LyricTopPosition,
            cx * 18.f / 20.f,
            cy - LyricBottomPosition
        };
        m_Lyric.SetRect(rcLyric);

        Kw::Rect rcCover;
        rcCover.left = cx * 1.f / 10.f;
        rcCover.top = cy * 15.f / 100.f;
        const auto cxyCover = std::min(
            rcLyric.left - cxMinGap - rcCover.left,
            cy * 4.f / 10.f);
        rcCover.right = rcCover.left + cxyCover;
        rcCover.bottom = rcCover.top + cxyCover;
        m_Cover.SetRect(rcCover);

        Kw::Rect rcLabel{ rcCover };
        rcLabel.top = rcCover.bottom + LabelCoverPadding;
        rcLabel.bottom = rcLabel.top + LabelHeight;
        m_LATitle.SetRect(rcLabel);
        rcLabel.top = rcLabel.bottom + LabelPadding;
        rcLabel.bottom = rcLabel.top + LabelHeight;
        m_LAAlbum.SetRect(rcLabel);
        rcLabel.top = rcLabel.bottom + LabelPadding;
        rcLabel.bottom = rcLabel.top + LabelHeight;
        m_LAArtist.SetRect(rcLabel);

        const auto dBackBtnMar = (MiniCoverSize - BackButtonSize) / 2;
        Kw::Rect rcBackBtn;
        rcBackBtn.left = MiniCoverLeftPosition + dBackBtnMar;
        rcBackBtn.top = cy - PlayPanelHeight + MiniCoverTopPosition + dBackBtnMar;
        rcBackBtn.right = rcBackBtn.left + BackButtonSize;
        rcBackBtn.bottom = rcBackBtn.top + BackButtonSize;
        m_BTBack.SetRect(rcBackBtn);
    }
    break;
    case WM_DPICHANGED:
    {
        const auto cx = GetWidth();
        const auto cy = GetHeight();
        GetWindow().RdCreateBitmapLogical(cx, cy, m_pBitmapBlurredCover.SelfClear());
        UpdateBlurredCover();
    }
    break;
    case WM_SETFONT:
    {
        const auto pTf = GetTextFormat().Get();
        m_LATitle.SetTextFormat(pTf);
        m_LAAlbum.SetTextFormat(pTf);
        m_LAArtist.SetTextFormat(pTf);
    }
    break;
    //case Dui::EWM_COLORSCHEMECHANGED:
    //{
    //    OnColorSchemeChanged();
    //    UpdateBlurredCover();
    //    Invalidate();
    //}
    //break;
    // FIXME
    //case WM_LBUTTONUP:
    //{
    //    if (((CWindowMain*)GetWnd())->TlIsValid())
    //    {
    //        Dui::DUINMHDR nm{ ELEN_PLAYPAGE_LBTN_UP };
    //        GenElemNotify(&nm);
    //    }
    //}
    //break;
    case WM_CREATE:
    {
        __super::OnEvent(uMsg, wParam, lParam);

        App->Player().GetEventChain().Connect(this, &CPagePlaying::OnPlayEvent);

        m_Cover.Create({}, Dui::DES_VISIBLE | Dui::DES_NO_CLIP, 0,
            50, 50, 200, 200, this);

        m_Lyric.Create({}, Dui::DES_VISIBLE, 0,
            50, 260, 200, 100, this);

        m_LATitle.Create({}, Dui::DES_VISIBLE, 0,
            0, 0, 0, 0, this);
        m_LATitle.SetFade(TRUE);

        m_LAAlbum.Create({}, Dui::DES_VISIBLE, 0,
            0, 0, 0, 0, this);
        m_LAAlbum.SetFade(TRUE);

        m_LAArtist.Create({}, Dui::DES_VISIBLE, 0,
            0, 0, 0, 0, this);
        m_LAArtist.SetFade(TRUE);

        m_BTBack.Create({}, Dui::DES_VISIBLE | Dui::DES_NOTIFY_WND, 0,
            100, 100, 70, 20, this);
        m_BTBack.SetId(ELEID_PLAYPAGE_BACK);

        OnColorSchemeChanged();

        ComPtr<IDWriteTextFormat> pTfLrc;
        auto& FontFactory = App->GetFontFactory();;
        FontFactory.NewFont(pTfLrc.SelfClear(),
            eck::Alignment::Near, eck::Alignment::Near, 25, 700);
        m_Lyric.SetTextFormat(pTfLrc.Get());
        FontFactory.NewFont(pTfLrc.SelfClear(),
            eck::Alignment::Near, eck::Alignment::Near, 21, 500);
        m_Lyric.SetTextFormatTranslation(pTfLrc.Get());

        SetEmptyText();
    }
    return 0;
    case WM_DESTROY:
    {
        m_pBitmapBlurredCover.Clear();
    }
    break;
    }
    return __super::OnEvent(uMsg, wParam, lParam);
}