#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/Math/Transform.h>

namespace BillboardGem
{
    enum class BillboardMode : AZ::u32
    {
        Spherical = 0,
        Cylindrical,
        CameraAligned
    };

    class BillboardComponent
        : public AZ::Component
        , protected AZ::TickBus::Handler
    {
    public:
        AZ_COMPONENT(BillboardComponent, "{86c96eac-9784-4f72-969b-2f475af44e0b}");

        static void Reflect(AZ::ReflectContext* context);

        void Init() override;
        void Activate() override;
        void Deactivate() override;

    protected:
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

    private:
        bool m_faceCamera = true;
        AZ::EntityId m_cameraEntityId;
        BillboardMode m_billboardMode = BillboardMode::Spherical;

        float m_angleOffset = 0.0f;
    };
}