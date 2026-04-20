
#pragma once

#include <AzCore/Component/ComponentBus.h>

namespace BillBoardGem
{
    class BillboardRequests
        : public AZ::ComponentBus
    {
    public:
        AZ_RTTI(BillBoardGem::BillboardRequests, "{73884236-1592-4067-BB7E-252FEE9F83CD}");

        // Put your public request methods here.
        
        // Put notification events here. Examples:
        // void RegisterEvent(AZ::EventHandler<...> notifyHandler);
        // AZ::Event<...> m_notifyEvent1;
        
    };

    using BillboardRequestBus = AZ::EBus<BillboardRequests>;

} // namespace BillBoardGem
