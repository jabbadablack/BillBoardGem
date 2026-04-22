#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/std/string/string.h>
#include <AzCore/std/containers/vector.h>

namespace BillboardGem
{
    // 1. The data structure for a single animation state
    struct SpriteAnimation
    {
        AZ_TYPE_INFO(SpriteAnimation, "{3b6a9f81-a9c1-4b13-9b7e-91325d72f12a}");
        
        AZStd::string m_name = "Idle";
        int m_startFrame = 0;
        int m_endFrame = 0;
        float m_fps = 12.0f;
    };

    // 2. The EBus interface so Script Canvas and Lua can talk to this component
    class SpriteAnimatorRequests : public AZ::ComponentBus
    {
    public:
        // Allows scripts to pass the name of the animation they want to play
        virtual void PlayAnimation(const AZStd::string& animationName) = 0;
        virtual void StopAnimation() = 0;
    };
    using SpriteAnimatorRequestBus = AZ::EBus<SpriteAnimatorRequests>;

    // 3. The Component (Now inherits from the new Request Bus!)
    class SpriteComponent
        : public AZ::Component
        , protected AZ::TickBus::Handler
        , public SpriteAnimatorRequestBus::Handler
    {
    public:
        AZ_COMPONENT(SpriteComponent, "{1dacaae8-c704-45ce-9928-8ecb9b9ba97c}");

        static void Reflect(AZ::ReflectContext* context);

        void Init() override;
        void Activate() override;
        void Deactivate() override;

    protected:
        // TickBus
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;
        
        // SpriteAnimatorRequestBus
        void PlayAnimation(const AZStd::string& animationName) override;
        void StopAnimation() override;

    private:
        // Material Strings (Exposed so you can fix them if a material changes!)
        AZStd::string m_uvScaleProperty = "baseColor.textureMap.Uv.Scale";
        AZStd::string m_uvOffsetProperty = "baseColor.textureMap.Uv.Offset";

        // Spritesheet Grid
        int m_columns = 1;
        int m_rows = 1;

        // The List of Animations
        AZStd::vector<SpriteAnimation> m_animations;
        
        // The default animation to play when the game starts
        AZStd::string m_defaultAnimation = "Idle";

        // Internal State
        bool m_isPlaying = false;
        SpriteAnimation m_currentAnim; // A copy of the currently running animation
        float m_timeAccumulator = 0.0f;
        int m_currentFrame = 0;
    };
}