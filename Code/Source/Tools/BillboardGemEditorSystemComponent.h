
#pragma once

#include <AzToolsFramework/API/ToolsApplicationAPI.h>

#include <Clients/BillboardGemSystemComponent.h>

namespace BillboardGem
{
    /// System component for BillboardGem editor
    class BillboardGemEditorSystemComponent
        : public BillboardGemSystemComponent
        , protected AzToolsFramework::EditorEvents::Bus::Handler
    {
        using BaseSystemComponent = BillboardGemSystemComponent;
    public:
        AZ_COMPONENT_DECL(BillboardGemEditorSystemComponent);

        static void Reflect(AZ::ReflectContext* context);

        BillboardGemEditorSystemComponent();
        ~BillboardGemEditorSystemComponent();

    private:
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        // AZ::Component
        void Activate() override;
        void Deactivate() override;
    };
} // namespace BillboardGem
