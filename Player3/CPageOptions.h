#pragma once
#include "CVeBase.h"

class CPageOptions final : public CVeBase
{
public:
    LRESULT OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept override;
};