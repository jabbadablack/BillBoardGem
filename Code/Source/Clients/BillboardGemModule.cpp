
#include <BillboardGem/BillboardGemTypeIds.h>
#include <BillboardGemModuleInterface.h>
#include "BillboardGemSystemComponent.h"

namespace BillboardGem
{
    class BillboardGemModule
        : public BillboardGemModuleInterface
    {
    public:
        AZ_RTTI(BillboardGemModule, BillboardGemModuleTypeId, BillboardGemModuleInterface);
        AZ_CLASS_ALLOCATOR(BillboardGemModule, AZ::SystemAllocator);
    };
}// namespace BillboardGem

#if defined(O3DE_GEM_NAME)
AZ_DECLARE_MODULE_CLASS(AZ_JOIN(Gem_, O3DE_GEM_NAME), BillboardGem::BillboardGemModule)
#else
AZ_DECLARE_MODULE_CLASS(Gem_BillboardGem, BillboardGem::BillboardGemModule)
#endif
