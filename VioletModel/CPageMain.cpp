#include "pch.h"
#include "CPageMain.h"
#include "CApp.h"

LRESULT CPageMain::OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
    switch (uMsg)
    {
    case WM_SIZE:
    {
        m_Lyt.Arrange(GetWidth(), GetHeight());
    }
    return 0;

    case WM_SETFONT:
        m_BTOpenFile.SetTextFormat(GetTextFormat().Get());
        m_BTOpenFolder.SetTextFormat(GetTextFormat().Get());
        return 0;

    case WM_CREATE:
    {
        __super::OnEvent(uMsg, wParam, lParam);

        constexpr eck::LYTMARGINS Mar{ .r = PageInnerPadding };

        m_BTOpenFile.Create(L"打开文件", Dui::DES_VISIBLE, 0,
            0, 0, 140, 40, this);
        m_BTOpenFile.SetIcon(GetAtlas()->AtlasGetD2D(AppImage::File));
        m_Lyt.LobAddObject(
            {
                .pObject = &m_BTOpenFile,
                .Margins = Mar,
                .uFlags = eck::LF_FIX
            });

        m_BTOpenFolder.Create(L"打开文件夹", Dui::DES_VISIBLE, 0,
            0, 0, 140, 40, this);
        m_BTOpenFolder.SetIcon(GetAtlas()->AtlasGetD2D(AppImage::Folder));
        m_Lyt.LobAddObject(
            {
                .pObject = &m_BTOpenFolder,
                .Margins = Mar,
                .uFlags = eck::LF_FIX
            });

        m_Lyt.LobAddObject(
            {
                .pObject = &m_Dummy,
                .Margins = Mar,
                .uFlags = eck::LF_FIX_HEIGHT,
                .uWeight = 1
            });

        m_LATest.Create({}, Dui::DES_VISIBLE, 0,
            0, 0, 400, 160, this);
        m_LATest.SetOnlyBitmap(TRUE);
        m_Lyt.LobAddObject(
            {
                .pObject = &m_LATest,
                .Margins = Mar,
                .uFlags = eck::LF_FIX_WIDTH,
            });
    }
    return 0;
    case WM_DESTROY:
        break;
    }
    return __super::OnEvent(uMsg, wParam, lParam);
}