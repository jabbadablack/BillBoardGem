#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/std/string/string.h>
#include <AzCore/std/containers/vector.h>
#include <AtomLyIntegration/CommonFeatures/Material/MaterialAssignment.h> 

namespace BillboardGem
{
    struct SpriteAnimation
    {
        AZ_TYPE_INFO(SpriteAnimation, "{3b6a9f81-a9c1-4b13-9b7e-91325d72f12a}");
        
        AZStd::string m_name = "Idle";
        int m_startRow = 0;
        int m_startColumn = 0;
        int m_frameCount = 1;
        float m_fps = 12.0f;
    };

    class SpriteAnimatorRequests : public AZ::ComponentBus
    {
    public:
        virtual void PlayAnimation(const AZStd::string& animationName) = 0;
        virtual void StopAnimation() = 0;
    };
    using SpriteAnimatorRequestBus = AZ::EBus<SpriteAnimatorRequests>;

    class SpriteComponent
        : public AZ::Component
        , protected AZ::TickBus::Handler
        , public SpriteAnimatorRequestBus::Handler
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
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;
        
        void PlayAnimation(const AZStd::string& animationName) override;
        void StopAnimation() override;

    private:
        AZStd::vector<AZ::Render::MaterialAssignmentId> GetActiveMaterialIds(AZ::EntityId targetEntity);

        void ApplyMaterialScale(float tileU, float tileV);
        void ApplyMaterialOffset(float offsetU, float offsetV);

        AZ::EntityId m_materialEntityId;

        AZStd::string m_uvTileUProperty = "uv.tileU";
        AZStd::string m_uvTileVProperty = "uv.tileV";
        AZStd::string m_uvOffsetUProperty = "uv.offsetU";
        AZStd::string m_uvOffsetVProperty = "uv.offsetV";

        int m_columns = 1;
        int m_rows = 1;

        AZStd::vector<SpriteAnimation> m_animations;
        AZStd::string m_defaultAnimation = "Idle";

        // Internal State tracking
        bool m_isPlaying = false;
        bool m_isMaterialInitialized = false; // NEW: Tracks dynamic material loading
        SpriteAnimation m_currentAnim; 
        float m_timeAccumulator = 0.0f;
        int m_currentFrame = 0; 
    };
}