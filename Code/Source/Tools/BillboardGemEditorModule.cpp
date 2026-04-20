
#include <BillboardGem/BillboardGemTypeIds.h>
#include <BillboardGemModuleInterface.h>
#include "BillboardGemEditorSystemComponent.h"

namespace BillboardGem
{
    class BillboardGemEditorModule
        : public BillboardGemModuleInterface
    {
    public:
        AZ_RTTI(BillboardGemEditorModule, BillboardGemEditorModuleTypeId, BillboardGemModuleInterface);
        AZ_CLASS_ALLOCATOR(BillboardGemEditorModule, AZ::SystemAllocator);

        BillboardGemEditorModule()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            // Add ALL components descriptors associated with this gem to m_descriptors.
            // This will associate the AzTypeInfo information for the components with the the SerializeContext, BehaviorContext and EditContext.
            // This happens through the [MyComponent]::Reflect() function.
            m_descriptors.insert(m_descriptors.end(), {
                BillboardGemEditorSystemComponent::CreateDescriptor(),
            });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         * Non-SystemComponents should not be added here
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList {
                azrtti_typeid<BillboardGemEditorSystemComponent>(),
            };
        }
    };
}// namespace BillboardGem

#if defined(O3DE_GEM_NAME)
AZ_DECLARE_MODULE_CLASS(AZ_JOIN(Gem_, O3DE_GEM_NAME, _Editor), BillboardGem::BillboardGemEditorModule)
#else
AZ_DECLARE_MODULE_CLASS(Gem_BillboardGem_Editor, BillboardGem::BillboardGemEditorModule)
#endif
