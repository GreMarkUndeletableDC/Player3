#pragma once
#include "CVeBase.h"

class CPageEffect final : public CVeBase
{
public:
    LRESULT OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept override;
};