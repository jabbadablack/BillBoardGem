
#include <AzCore/Serialization/SerializeContext.h>
#include "BillboardGemEditorSystemComponent.h"

#include <BillboardGem/BillboardGemTypeIds.h>

namespace BillboardGem
{
    AZ_COMPONENT_IMPL(BillboardGemEditorSystemComponent, "BillboardGemEditorSystemComponent",
        BillboardGemEditorSystemComponentTypeId, BaseSystemComponent);

    void BillboardGemEditorSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<BillboardGemEditorSystemComponent, BillboardGemSystemComponent>()
                ->Version(0);
        }
    }

    BillboardGemEditorSystemComponent::BillboardGemEditorSystemComponent() = default;

    BillboardGemEditorSystemComponent::~BillboardGemEditorSystemComponent() = default;

    void BillboardGemEditorSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        BaseSystemComponent::GetProvidedServices(provided);
        provided.push_back(AZ_CRC_CE("BillboardGemEditorService"));
    }

    void BillboardGemEditorSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        BaseSystemComponent::GetIncompatibleServices(incompatible);
        incompatible.push_back(AZ_CRC_CE("BillboardGemEditorService"));
    }

    void BillboardGemEditorSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        BaseSystemComponent::GetRequiredServices(required);
    }

    void BillboardGemEditorSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        BaseSystemComponent::GetDependentServices(dependent);
    }

    void BillboardGemEditorSystemComponent::Activate()
    {
        BillboardGemSystemComponent::Activate();
        AzToolsFramework::EditorEvents::Bus::Handler::BusConnect();
    }

    void BillboardGemEditorSystemComponent::Deactivate()
    {
        AzToolsFramework::EditorEvents::Bus::Handler::BusDisconnect();
        BillboardGemSystemComponent::Deactivate();
    }

} // namespace BillboardGem
