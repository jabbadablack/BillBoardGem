#include "BillboardComponent.h"
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Component/TransformBus.h>
#include <AzCore/Math/Transform.h>
#include <AzCore/Math/Matrix3x3.h>
#include <AzCore/Debug/Trace.h> 
#include <AzCore/Module/Environment.h>

namespace BillboardGem
{
    void BillboardComponent::Reflect(AZ::ReflectContext* context)
    {
        if (context == nullptr)
        {
            AZ_Assert(false, "ReflectContext is null! Cannot reflect BillboardComponent.");
            return;
        }

        AZ_Assert(AZ::Environment::GetInstance() != nullptr, "O3DE Environment is not fully initialized.");

        AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context);
        if (serializeContext != nullptr)
        {
            auto&& classBuilder = serializeContext->Class<BillboardComponent, AZ::Component>();
            classBuilder.Version(1);
            classBuilder.Field("FaceCamera", &BillboardComponent::m_faceCamera);
            classBuilder.Field("CameraEntity", &BillboardComponent::m_cameraEntityId);
            classBuilder.Field("BillboardMode", &BillboardComponent::m_billboardMode);
            classBuilder.Field("AngleOffset", &BillboardComponent::m_angleOffset);

            AZ::EditContext* editContext = serializeContext->GetEditContext();
            if (editContext != nullptr)
            {
                auto&& editBuilder = editContext->Class<BillboardComponent>("Billboard", "Makes the entity face a specific target");
                editBuilder.ClassElement(AZ::Edit::ClassElements::EditorData, "");
                editBuilder.Attribute(AZ::Edit::Attributes::Category, "Rendering");
                editBuilder.Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"));

                editBuilder.DataElement(AZ::Edit::UIHandlers::Default, &BillboardComponent::m_faceCamera, "Enable Billboard", "Should the entity rotate?");
                editBuilder.DataElement(AZ::Edit::UIHandlers::Default, &BillboardComponent::m_cameraEntityId, "Target Entity", "Select the Camera to look at.");

                auto* modeBuilder = editBuilder.DataElement(AZ::Edit::UIHandlers::ComboBox, &BillboardComponent::m_billboardMode, "Billboard Mode", "How should the entity track the camera?");
                modeBuilder->EnumAttribute(BillboardMode::Spherical, "Spherical (Look-At)");
                modeBuilder->EnumAttribute(BillboardMode::Cylindrical, "Cylindrical (Lock Upright)");
                modeBuilder->EnumAttribute(BillboardMode::CameraAligned, "Window-Aligned (Perfectly Flat)");

                editBuilder.DataElement(AZ::Edit::UIHandlers::Default, &BillboardComponent::m_angleOffset, "Angle Offset", "Rotation offset in degrees (e.g., 0, 45, 90)");
            }
        }
    }

    void BillboardComponent::Init()
    {
        AZ::EntityId myEntityId = GetEntityId();
        if (!myEntityId.IsValid())
        {
            AZ_Assert(false, "Entity ID is invalid during BillboardComponent::Init!");
            return;
        }

        AZ::Entity* myEntity = GetEntity();
        if (myEntity == nullptr)
        {
            AZ_Assert(false, "Entity pointer is null during BillboardComponent::Init!");
            return;
        }
    }

    void BillboardComponent::Activate()
    {
        AZ::EntityId myEntityId = GetEntityId();
        if (!myEntityId.IsValid())
        {
            AZ_Assert(false, "Entity ID is invalid during BillboardComponent::Activate!");
            return;
        }

        AZ::Entity* myEntity = GetEntity();
        if (myEntity == nullptr)
        {
            AZ_Assert(false, "Entity pointer is null during BillboardComponent::Activate!");
            return;
        }

        AZ::TickBus::Handler::BusConnect();
    }

    void BillboardComponent::Deactivate()
    {
        AZ::EntityId myEntityId = GetEntityId();
        if (!myEntityId.IsValid())
        {
            AZ_Assert(false, "Entity ID is invalid during BillboardComponent::Deactivate!");
            return;
        }

        AZ::Entity* myEntity = GetEntity();
        if (myEntity == nullptr)
        {
            AZ_Assert(false, "Entity pointer is null during BillboardComponent::Deactivate!");
            return;
        }

        AZ::TickBus::Handler::BusDisconnect();
    }

    void BillboardComponent::OnTick(float deltaTime, AZ::ScriptTimePoint /*time*/)
    {
        if (deltaTime < 0.0f)
        {
            AZ_Assert(false, "Delta time in OnTick cannot be negative!");
            return;
        }

        AZ::EntityId myEntityId = GetEntityId();
        if (!myEntityId.IsValid())
        {
            AZ_Assert(false, "Entity ID is invalid during BillboardComponent::OnTick!");
            return;
        }

        if (!m_faceCamera || !m_cameraEntityId.IsValid())
        {
            return;
        }

        AZ::Transform cameraTransform = AZ::Transform::CreateIdentity();
        AZ::TransformBus::EventResult(cameraTransform, m_cameraEntityId, &AZ::TransformBus::Events::GetWorldTM);

        AZ::Transform myTransform = AZ::Transform::CreateIdentity();
        AZ::TransformBus::EventResult(myTransform, myEntityId, &AZ::TransformBus::Events::GetWorldTM);

        AZ::Vector3 myPosition = myTransform.GetTranslation();

        AZ::Vector3 basisX = myTransform.TransformPoint(AZ::Vector3(1.0f, 0.0f, 0.0f)) - myTransform.GetTranslation();
        AZ::Vector3 basisY = myTransform.TransformPoint(AZ::Vector3(0.0f, 1.0f, 0.0f)) - myTransform.GetTranslation();
        AZ::Vector3 basisZ = myTransform.TransformPoint(AZ::Vector3(0.0f, 0.0f, 1.0f)) - myTransform.GetTranslation();

        AZ::Vector3 myScale = AZ::Vector3(basisX.GetLength(), basisY.GetLength(), basisZ.GetLength());

        float offsetRadians = m_angleOffset * (AZ::Constants::Pi / 180.0f);
        AZ::Transform rotationTransform = AZ::Transform::CreateIdentity();

        if (m_billboardMode == BillboardMode::CameraAligned)
        {
            AZ::Quaternion camRotation = cameraTransform.GetRotation();
            rotationTransform = AZ::Transform::CreateFromQuaternionAndTranslation(camRotation, AZ::Vector3::CreateZero());
            rotationTransform = rotationTransform * AZ::Transform::CreateRotationZ(offsetRadians);
        }
        else
        {
            AZ::Vector3 targetPosition = cameraTransform.GetTranslation();
            if (m_billboardMode == BillboardMode::Cylindrical)
            {
                targetPosition.SetZ(myPosition.GetZ());
            }

            bool isClose = myPosition.IsClose(targetPosition, 0.001f);
            if (!isClose)
            {
                AZ::Vector3 lookDirection = targetPosition - myPosition;
                rotationTransform = AZ::Transform::CreateLookAt(AZ::Vector3::CreateZero(), lookDirection, AZ::Transform::Axis::YNegative);
                rotationTransform = rotationTransform * AZ::Transform::CreateRotationZ(offsetRadians);
            }
            else
            {
                AZ::Quaternion myRotation = myTransform.GetRotation();
                rotationTransform = AZ::Transform::CreateFromQuaternionAndTranslation(myRotation, AZ::Vector3::CreateZero());
            }
        }

        AZ::Vector3 rotBasisX = rotationTransform.TransformPoint(AZ::Vector3(1.0f, 0.0f, 0.0f)) - rotationTransform.GetTranslation();
        AZ::Vector3 rotBasisY = rotationTransform.TransformPoint(AZ::Vector3(0.0f, 1.0f, 0.0f)) - rotationTransform.GetTranslation();
        AZ::Vector3 rotBasisZ = rotationTransform.TransformPoint(AZ::Vector3(0.0f, 0.0f, 1.0f)) - rotationTransform.GetTranslation();

        AZ::Vector3 scaledBasisX = rotBasisX * myScale.GetX();
        AZ::Vector3 scaledBasisY = rotBasisY * myScale.GetY();
        AZ::Vector3 scaledBasisZ = rotBasisZ * myScale.GetZ();

        AZ::Matrix3x3 finalMatrix = AZ::Matrix3x3::CreateIdentity();
        finalMatrix.SetColumn(0, scaledBasisX);
        finalMatrix.SetColumn(1, scaledBasisY);
        finalMatrix.SetColumn(2, scaledBasisZ);

        AZ::Transform finalTransform = AZ::Transform::CreateFromMatrix3x3AndTranslation(finalMatrix, myPosition);

        AZ::TransformBus::Event(myEntityId, &AZ::TransformBus::Events::SetWorldTM, finalTransform);
    }
}