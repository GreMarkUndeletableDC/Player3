#include "pch.h"
#include "CApplication.h"

CApplication* App{};

CApplication::CApplication() noexcept
{
    m_ptcUiThread = eck::PtcCurrent();
    EckAssert(m_ptcUiThread);
    m_ListManager.LoadList((eck::GetRunningPath() + L"\\List").ToStringView());
}

Tag::Result ReadMetadata(
    _In_z_ PCWSTR pszFile,
    _Inout_ Tag::SimpleData& mi,
    const Tag::SIMPLE_OPT& Opt) noexcept
{
    Tag::CMediaFile mf{ pszFile };
    if (!mf.IsValid())
        return Tag::Result::FileAccessDenied;
    mi.Clear();
    const auto uMask = mi.uMask;
    if (mf.GetTagType() & Tag::TAG_FLAC)
    {
        Tag::CFlac x{ mf };
        x.ReadTag();
        x.SimpleGet(mi, Opt);
        mi.uMask &= ~mi.uMaskChecked;
    }
    if (mf.GetTagType() & (Tag::TAG_ID3V2_3 | Tag::TAG_ID3V2_4))
    {
        Tag::CID3v2 x{ mf };
        x.ReadTag();
        x.SimpleGet(mi, Opt);
        mi.uMask &= ~mi.uMaskChecked;
    }
    if (mf.GetTagType() & Tag::TAG_APE)
    {
        Tag::CApe x{ mf };
        x.ReadTag();
        x.SimpleGet(mi, Opt);
        mi.uMask &= ~mi.uMaskChecked;
    }
    if ((uMask & Tag::MIM_TITLE) && mi.rsTitle.IsEmpty())
        mi.rsTitle.Assign(L"未知标题"sv);
    if ((uMask & Tag::MIM_ARTIST) && mi.slArtist.String.IsEmpty())
        mi.slArtist.PushBackString(L"未知艺术家"sv, {});
    if ((uMask & Tag::MIM_ALBUM) && mi.rsAlbum.IsEmpty())
        mi.rsAlbum.Assign(L"未知专辑"sv);
    mi.uMask = uMask;
    return Tag::Result::Ok;
}