#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/Asset/AssetManagerBus.h>
#include <AzCore/Asset/AssetSerializer.h>
#include "SpriteAnimationAsset.h"
#include <AtomLyIntegration/CommonFeatures/Material/MaterialAssignment.h>

namespace BillboardGem
{
    class SpriteAnimatorRequests : public AZ::ComponentBus
    {
    public:
        AZ_RTTI(SpriteAnimatorRequests, "{b06447b6-b48b-4073-878f-32111b0a517d}");
        virtual ~SpriteAnimatorRequests() = default;

        virtual void PlayAnimation(const AZStd::string& animationName) = 0;
        virtual void StopAnimation() = 0;
    };
    using SpriteAnimatorRequestBus = AZ::EBus<SpriteAnimatorRequests>;

    class SpriteComponent
        : public AZ::Component
        , protected AZ::TickBus::Handler
        , public SpriteAnimatorRequestBus::Handler
        , protected AZ::Data::AssetBus::Handler // NEW: Listens for Asset loading
    {
    public:
        AZ_COMPONENT(SpriteComponent, "{1dacaae8-c704-45ce-9928-8ecb9b9ba97c}");

        static void Reflect(AZ::ReflectContext* context);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        void Init() override;
        void Activate() override;
        void Deactivate() override;

    protected:
        // AssetBus Overrides
        void OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset) override;
        void OnAssetReloaded(AZ::Data::Asset<AZ::Data::AssetData> asset) override;

        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

        void PlayAnimation(const AZStd::string& animationName) override;
        void StopAnimation() override;

    private:
        AZStd::vector<AZ::Render::MaterialAssignmentId> GetActiveMaterialIds(AZ::EntityId targetEntity);
        void ApplyMaterialScale(float tileU, float tileV);
        void ApplyMaterialOffset(float offsetU, float offsetV);

        // Editor Properties
        AZ::EntityId m_materialEntityId;
        AZ::Data::Asset<SpriteAnimationAsset> m_spriteAsset; 

        // Internal State
        bool m_isPlaying = false;
        bool m_isMaterialInitialized = false;
        SpriteAnimation m_currentAnim;
        float m_timeAccumulator = 0.0f;
        int m_currentFrame = 0;
    };
}