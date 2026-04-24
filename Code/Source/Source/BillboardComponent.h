#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/Math/Transform.h>
#include <AzCore/RTTI/TypeInfo.h>

namespace BillboardGem
{
    enum class BillboardMode : AZ::u32
    {
        Spherical = 0,
        Cylindrical,
        CameraAligned
    };

    class BillboardRequests : public AZ::ComponentBus
    {
    public:
        AZ_RTTI(BillboardRequests, "{6c92af17-12f6-4ba1-a354-c76773df3415}");
        virtual ~BillboardRequests() = default;

        virtual float GetViewingAngle() = 0;
    };
    using BillboardRequestBus = AZ::EBus<BillboardRequests>;

    class BillboardComponent
        : public AZ::Component
        , protected AZ::TickBus::Handler
        , public BillboardRequestBus::Handler
    {
    public:
        AZ_COMPONENT(BillboardComponent, "{86c96eac-9784-4f72-969b-2f475af44e0b}");

        static void Reflect(AZ::ReflectContext* context);

        void Init() override;
        void Activate() override;
        void Deactivate() override;

    protected:
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;
        float GetViewingAngle() override;

    private:
        void UpdateViewingAngle(const AZ::Vector3& myPosition, const AZ::Vector3& targetPosition);
        AZ::Transform CalculateRotation(const AZ::Transform& cameraTM, const AZ::Transform& myTM, const AZ::Vector3& myPos, const AZ::Vector3& targetPos);
        AZ::Transform ApplyScaleAndTranslation(const AZ::Transform& rotationTM, const AZ::Transform& myTM, const AZ::Vector3& myPos);

        bool m_faceCamera = true;
        AZ::EntityId m_cameraEntityId;
        BillboardMode m_billboardMode = BillboardMode::Spherical;

        float m_angleOffset = 0.0f;
        float m_viewingAngle = 0.0f;
    };
}