
#include "BillboardGemSystemComponent.h"

#include <BillboardGem/BillboardGemTypeIds.h>

#include <AzCore/Serialization/SerializeContext.h>

#include <AzFramework/Asset/GenericAssetHandler.h>
#include "../Source/SpriteAnimationAsset.h"

namespace BillboardGem
{
    AZ_COMPONENT_IMPL(BillboardGemSystemComponent, "BillboardGemSystemComponent",
        BillboardGemSystemComponentTypeId);

    void BillboardGemSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<BillboardGemSystemComponent, AZ::Component>()
                ->Version(0)
                ;
        }
    }

    void BillboardGemSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("BillboardGemService"));
    }

    void BillboardGemSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("BillboardGemService"));
    }

    void BillboardGemSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void BillboardGemSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    BillboardGemSystemComponent::BillboardGemSystemComponent()
    {
        if (BillboardGemInterface::Get() == nullptr)
        {
            BillboardGemInterface::Register(this);
        }
    }

    BillboardGemSystemComponent::~BillboardGemSystemComponent()
    {
        if (BillboardGemInterface::Get() == this)
        {
            BillboardGemInterface::Unregister(this);
        }
    }

    void BillboardGemSystemComponent::Init()
    {
    }

    void BillboardGemSystemComponent::Activate()
    {
        BillboardGemRequestBus::Handler::BusConnect();
        AZ::TickBus::Handler::BusConnect();

        m_spriteAssetHandler = aznew AzFramework::GenericAssetHandler<SpriteAnimationAsset>("Sprite Animation", "Rendering", "spranim");
        m_spriteAssetHandler->Register();
    }

    void BillboardGemSystemComponent::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
        BillboardGemRequestBus::Handler::BusDisconnect();

        if (m_spriteAssetHandler)
        {
            m_spriteAssetHandler->Unregister();
            delete m_spriteAssetHandler;
            m_spriteAssetHandler = nullptr;
        }
    }

    void BillboardGemSystemComponent::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
    }

} // namespace BillboardGem
