#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>

namespace BillboardGem
{
    class SpriteComponent
        : public AZ::Component
        , protected AZ::TickBus::Handler
    {
    public:
        // Note: Generate a fresh UUID for this new component!
        AZ_COMPONENT(SpriteComponent, "{1dacaae8-c704-45ce-9928-8ecb9b9ba97c}");

        static void Reflect(AZ::ReflectContext* context);

        void Init() override;
        void Activate() override;
        void Deactivate() override;

    protected:
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

    private:
        // Editor Variables
        bool m_playAnimation = true;
        int m_columns = 1;
        int m_rows = 1;
        float m_fps = 12.0f;

        // Internal Trackers
        float m_timeAccumulator = 0.0f;
        int m_currentFrame = 0;
    };
}