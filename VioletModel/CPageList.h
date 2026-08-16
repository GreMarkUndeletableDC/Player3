#pragma once
#include "CVeBase.h"
#include "CPlayList.h"

class CPlayListFileAdapter : public Dui::CListView::IAdapter
{
private:
    struct Item
    {
        RefPtr<eck::CD2DImageList> pImageList;
        LvState uState{};
    };

    std::vector<Item> m_vList{};

    LvIndex LcaGetCount() const noexcept override;
    int LcaGetColumnCount() const noexcept override;
    void LcaGet(const LvIndex& idx, int idxCol,
        LvProperty eProp, std::any& Data) const noexcept override;
    void LcaSet(const LvIndex& idx, int idxCol,
        LvProperty eProp, std::any& Data, BOOL bMove = FALSE) noexcept override;
    void LcaColumnWidthChanged(int idxCol, float cxNew) noexcept override;
public:
    EckInlineNdCe auto& operator[](size_t idx) noexcept { return m_vList[idx]; }
};

class CPlayListItemAdapter : public Dui::CListView::IAdapter
{
private:
    enum class Column
    {
        Title,
        Artist,
        Album,
        Duration,

        Maximum,
    };

    struct Item
    {
        int idxImage{};// -1 = 需要更新
        LvState uState{};
        ComPtr<IDWriteTextLayout> pTextLayout[(size_t)Column::Maximum]{};
    };

    RefPtr<CPlayList> m_pList{};
    std::vector<Item> m_vItem{};

    LvIndex LcaGetCount() const noexcept override;
    int LcaGetColumnCount() const noexcept override;
    void LcaGet(const LvIndex& idx, int idxCol,
        LvProperty eProp, std::any& Data) const noexcept override;
    void LcaSet(const LvIndex& idx, int idxCol,
        LvProperty eProp, std::any& Data, BOOL bMove = FALSE) noexcept override;
    void LcaColumnWidthChanged(int idxCol, float cxNew) noexcept override;
public:
    EckInlineNdCe auto& operator[](size_t idx) noexcept { return m_vItem[idx]; }
    void InvalidateImage() noexcept;
};

class CPageList : public CVeBase
{
    friend class CPlayListItemAdapter;
public:
    constexpr static float
        ListFileListWidth = 170.f,
        EditHeight = 37.f,
        ButtonPadding = 110.f,
        CoverSize = 40.f,
        ListItemHeight = 46.f
        ;

    constexpr static int DefaultCoverIndex{};
private:
    struct TSKPARAM_LOAD_META_DATA
    {
        RefPtr<CPlayList> pList;
        RefPtr<eck::CD2DImageList> pImageList;
        eck::CTrivialBuffer<int> vItem;
    };

    Dui::CEdit m_EDSearch{};
    Dui::CListView m_TBLPlayList{};
    eck::CLinearLayoutV m_LytPlayList{};
    CPlayListFileAdapter m_FileAdapter{};

    Dui::CButton m_BTAddFile{};
    Dui::CButton m_BTLocate{};
    eck::CLayoutDummy m_TopBarDummySpace{};
    Dui::CEdit m_EDSearchItem{};
    eck::CLinearLayoutH m_LytTopBar{};
    Dui::CListView m_GLList{};
    eck::CLinearLayoutV m_LytList{};
    CPlayListItemAdapter m_ItemAdapter{};

    eck::CLinearLayoutH m_Lyt{};

    int m_cxIl{}, m_cyIl{};

    BOOL m_bSearchItemEditEmpty{};

    eck::CoroTask<void> PlLoadMetadata(TSKPARAM_LOAD_META_DATA&& Param) noexcept;
    void PlBeginLoadMetadata(int idxList = -1) noexcept;

    const RefPtr<CPlayList>& PlCurrent() const noexcept;

    // 使用搜索编辑框内容搜索列表
    // 返回项目数
    int PlSearchEditContent(CPlayList* pList) noexcept;

    // 上传默认封面到图像列表的第0个磁贴
    HRESULT IlUploadDefaultCover(eck::CD2DImageList* pImageList) noexcept;
    RefPtr<eck::CD2DImageList> IlCreate() noexcept;
    void IlUpdateTilePixelSize() noexcept;
    HRESULT IlDpiChanged() noexcept;

    HRESULT OnMenuAddFile(CPlayList* pList, int idxInsert = -1) noexcept;

    void OnListSwitch() noexcept;

    void InitializeUi() noexcept;
public:
    LRESULT OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept override;
};