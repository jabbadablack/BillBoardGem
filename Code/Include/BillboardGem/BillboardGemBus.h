
#pragma once

#include <BillboardGem/BillboardGemTypeIds.h>

#include <AzCore/EBus/EBus.h>
#include <AzCore/Interface/Interface.h>

namespace BillboardGem
{
    class BillboardGemRequests
    {
    public:
        AZ_RTTI(BillboardGemRequests, BillboardGemRequestsTypeId);
        virtual ~BillboardGemRequests() = default;
        // Put your public methods here
    };

    class BillboardGemBusTraits
        : public AZ::EBusTraits
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        // EBusTraits overrides
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        //////////////////////////////////////////////////////////////////////////
    };

    using BillboardGemRequestBus = AZ::EBus<BillboardGemRequests, BillboardGemBusTraits>;
    using BillboardGemInterface = AZ::Interface<BillboardGemRequests>;

} // namespace BillboardGem
