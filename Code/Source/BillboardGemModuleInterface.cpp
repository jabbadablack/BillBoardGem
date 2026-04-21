
#include "BillboardGemModuleInterface.h"
#include <AzCore/Memory/Memory.h>

#include <BillboardGem/BillboardGemTypeIds.h>

#include <Clients/BillboardGemSystemComponent.h>
#include "Source/BillboardComponent.h"
#include "Source/SpriteComponent.h"

namespace BillboardGem
{
    AZ_TYPE_INFO_WITH_NAME_IMPL(BillboardGemModuleInterface,
        "BillboardGemModuleInterface", BillboardGemModuleInterfaceTypeId);
    AZ_RTTI_NO_TYPE_INFO_IMPL(BillboardGemModuleInterface, AZ::Module);
    AZ_CLASS_ALLOCATOR_IMPL(BillboardGemModuleInterface, AZ::SystemAllocator);

    BillboardGemModuleInterface::BillboardGemModuleInterface()
    {
        // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
        // Add ALL components descriptors associated with this gem to m_descriptors.
        // This will associate the AzTypeInfo information for the components with the the SerializeContext, BehaviorContext and EditContext.
        // This happens through the [MyComponent]::Reflect() function.
        m_descriptors.insert(m_descriptors.end(), {
            BillboardGemSystemComponent::CreateDescriptor(),
            BillboardComponent::CreateDescriptor(),
            SpriteComponent::CreateDescriptor(),
            });
    }

    AZ::ComponentTypeList BillboardGemModuleInterface::GetRequiredSystemComponents() const
    {
        return AZ::ComponentTypeList{
            azrtti_typeid<BillboardGemSystemComponent>(),
        };
    }
} // namespace BillboardGem
