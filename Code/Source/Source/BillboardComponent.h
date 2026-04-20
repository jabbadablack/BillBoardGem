#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/Math/Transform.h>

namespace BillboardGem
{
    enum class BillboardMode : AZ::u32
    {
        Spherical = 0,   // Looks directly at the camera (Tilts everywhere)
        Cylindrical,     // Locks the Z-axis (Tilts horizontally only)
        CameraAligned    // Copies camera rotation (Perfectly flat)
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
        AZ::Transform::Axis m_forwardAxis = AZ::Transform::Axis::YNegative;
        
        BillboardMode m_billboardMode = BillboardMode::Spherical; 
    };
}