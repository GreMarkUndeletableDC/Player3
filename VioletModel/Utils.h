#pragma once

Tag::Result ReadMetadata(
    _In_z_ PCWSTR pszFile,
    Tag::SimpleData& mi,
    const Tag::SIMPLE_OPT& Opt);