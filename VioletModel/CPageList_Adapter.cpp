#include "pch.h"
#include "CPageList.h"
#include "CApp.h"


LvIndex CPlayListFileAdapter::LcaGetCount() const noexcept
{
    return { App->ListManager().GetCount(), 0 };
}

int CPlayListFileAdapter::LcaGetColumnCount() const noexcept
{
    return 1;
}

void CPlayListFileAdapter::LcaGet(const LvIndex& idx, int idxCol,
    LvProperty eProp, std::any& Data) const noexcept
{
    switch (eProp)
    {
    case LvProperty::SystemState:
        Data = m_vList[idx.Item].uState;
        break;
    case LvProperty::Text:
    {
        auto& rs = *std::any_cast<eck::CStringW*>(Data);
        rs = App->ListManager().AtList(idx.Item)->LtmGetName();
    }
    break;
    case LvProperty::Image:
        Data = CPageList::DefaultCoverIndex;
        break;
    }
}

void CPlayListFileAdapter::LcaSet(const LvIndex& idx, int idxCol,
    LvProperty eProp, std::any& Data, BOOL bMove) noexcept
{
    switch (eProp)
    {
    case LvProperty::SystemState:
        m_vList[idx.Item].uState = std::any_cast<LvState>(Data);
        break;
    }
}

void CPlayListFileAdapter::LcaColumnWidthChanged(int idxCol, float cxNew) noexcept
{
}

void CPlayListFileAdapter::OnDataChanged() noexcept
{
    m_vList.clear();
    m_vList.resize(App->ListManager().GetCount());
}

// =======================================================

LvIndex CPlayListItemAdapter::LcaGetCount() const noexcept
{
    if (!m_pList)
        return { 0, 0 };
    LvIndex idx{};
    idx.Item = m_pList->FlIsSearching() ?
        m_pList->FlGetSearchResultCount() :
        m_pList->FlGetCount();
    return idx;
}

int CPlayListItemAdapter::LcaGetColumnCount() const noexcept
{
    return (int)Column::Maximum;
}

void CPlayListItemAdapter::LcaGet(const LvIndex& idx, int idxCol,
    LvProperty eProp, std::any& Data) const noexcept
{
    if (!m_pList)
        return;
    switch (eProp)
    {
    case LvProperty::SystemState:
        Data = (LvState)m_vItem[idx.Item].uState;
        break;
    case LvProperty::UiTextLayout:
        Data = m_vItem[idx.Item].pTextLayout[idxCol];
        break;
    case LvProperty::Image:
    {
        if (idxCol != 0)
            break;
        const auto idxImage = m_vItem[idx.Item].idxImage;
        Data = idxImage < 0 ? CPageList::DefaultCoverIndex : idxImage;
    }
    break;
    case LvProperty::Text:
    {
        auto& rs = *std::any_cast<eck::CStringW*>(Data);
        const auto& e = m_pList->FlAt(idx.Item);
        switch (idxCol)
        {
        case 0: rs = e.rsName;   break;
        case 1: rs = e.rsArtist; break;
        case 2: rs = e.rsAlbum;  break;
        case 3:
        {
            const auto uSecTime = e.s.uSecTime;
            if (uSecTime)
                rs.Format(L"%d:%02d", uSecTime / 60, uSecTime % 60);
            else
                rs.Clear();
        }
        break;
        }
    }
    break;
    }
}

void CPlayListItemAdapter::LcaSet(const LvIndex& idx, int idxCol,
    LvProperty eProp, std::any& Data, BOOL bMove) noexcept
{
    switch (eProp)
    {
    case LvProperty::SystemState:
        m_vItem[idx.Item].uState = std::any_cast<LvState>(Data);
        break;
    case LvProperty::UiTextLayout:
        m_vItem[idx.Item].pTextLayout[idxCol] = std::any_cast<ComPtr<IDWriteTextLayout>>(Data);
        break;
    }
}

void CPlayListItemAdapter::LcaColumnWidthChanged(int idxCol, float cxNew) noexcept
{
    // TODO: 考虑cxNew
    if (idxCol < 0)
        for (auto& e : m_vItem)
            for (auto& pTl : e.pTextLayout)
                pTl.Clear();
    else
        for (auto& e : m_vItem)
            e.pTextLayout[idxCol].Clear();
}

void CPlayListItemAdapter::InvalidateImage() noexcept
{
    for (auto& e : m_vItem)
        e.idxImage = -1;
}

void CPlayListItemAdapter::SetList(RefPtr<CPlayList> pList) noexcept
{
    m_pList = std::move(pList);
    m_vItem.clear();
    if (m_pList)
        m_vItem.resize(m_pList->FlGetCount());
}