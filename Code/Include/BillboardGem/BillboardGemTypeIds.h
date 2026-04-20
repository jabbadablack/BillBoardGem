
#pragma once

namespace BillboardGem
{
    // System Component TypeIds
    inline constexpr const char* BillboardGemSystemComponentTypeId = "{607A70BB-4166-49DE-9358-1159F2F88698}";
    inline constexpr const char* BillboardGemEditorSystemComponentTypeId = "{D90F553B-209F-4208-AD62-4323948C9424}";

    // Module derived classes TypeIds
    inline constexpr const char* BillboardGemModuleInterfaceTypeId = "{DB4FB8E9-6EBF-4ECA-88E2-26AE476B620B}";
    inline constexpr const char* BillboardGemModuleTypeId = "{9973C16F-275C-48BA-95EB-91C4C7FF0F01}";
    // The Editor Module by default is mutually exclusive with the Client Module
    // so they use the Same TypeId
    inline constexpr const char* BillboardGemEditorModuleTypeId = BillboardGemModuleTypeId;

    // Interface TypeIds
    inline constexpr const char* BillboardGemRequestsTypeId = "{80E1C2B3-A303-4719-8D49-90B5D0467B0E}";
} // namespace BillboardGem
