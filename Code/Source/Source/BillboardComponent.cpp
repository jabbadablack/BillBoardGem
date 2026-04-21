#include "BillboardComponent.h"
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Component/TransformBus.h>
#include <AzCore/Math/Transform.h>

namespace BillboardGem
{
    void BillboardComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<BillboardComponent, AZ::Component>()
                ->Version(1)
                ->Field("FaceCamera", &BillboardComponent::m_faceCamera)
                ->Field("CameraEntity", &BillboardComponent::m_cameraEntityId)
                ->Field("BillboardMode", &BillboardComponent::m_billboardMode)
                ->Field("AngleOffset", &BillboardComponent::m_angleOffset);

            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                editContext->Class<BillboardComponent>("Billboard", "Makes the entity face a specific target")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::Category, "Rendering")
                        ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                    
                    ->DataElement(AZ::Edit::UIHandlers::Default, &BillboardComponent::m_faceCamera, "Enable Billboard", "Should the entity rotate?")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &BillboardComponent::m_cameraEntityId, "Target Entity", "Select the Camera to look at.")
                    
                    ->DataElement(AZ::Edit::UIHandlers::ComboBox, &BillboardComponent::m_billboardMode, "Billboard Mode", "How should the entity track the camera?")
                        ->EnumAttribute(BillboardMode::Spherical, "Spherical")
                        ->EnumAttribute(BillboardMode::Cylindrical, "Cylindrical")
                        ->EnumAttribute(BillboardMode::CameraAligned, "Window-Aligned")
                    
                    ->DataElement(AZ::Edit::UIHandlers::Default, &BillboardComponent::m_angleOffset, "Angle Offset", "Rotation offset in degrees (e.g., 0, 45, 90)");
            }
        }
    }

    void BillboardComponent::Init() {}

    void BillboardComponent::Activate()
    {
        AZ::TickBus::Handler::BusConnect();
    }

    void BillboardComponent::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
    }

    void BillboardComponent::OnTick(float deltaTime, AZ::ScriptTimePoint time)
    {
        if (!m_faceCamera) return;

        if (m_cameraEntityId.IsValid())
        {
            AZ::Transform cameraTransform = AZ::Transform::CreateIdentity();
            AZ::TransformBus::EventResult(cameraTransform, m_cameraEntityId, &AZ::TransformBus::Events::GetWorldTM);

            AZ::Vector3 myPosition = AZ::Vector3::CreateZero();
            AZ::TransformBus::EventResult(myPosition, GetEntityId(), &AZ::TransformBus::Events::GetWorldTranslation);

            AZ::Transform finalTransform = AZ::Transform::CreateIdentity();

            float offsetRadians = m_angleOffset * (AZ::Constants::Pi / 180.0f);
            AZ::Transform rotationOffset = AZ::Transform::CreateRotationZ(offsetRadians);

            if (m_billboardMode == BillboardMode::CameraAligned)
            {
                finalTransform = cameraTransform * rotationOffset;
                finalTransform.SetTranslation(myPosition);
            }
            else if (m_billboardMode == BillboardMode::Cylindrical)
            {
                AZ::Vector3 cameraForward = cameraTransform.GetBasisY();
                cameraForward.SetZ(0.0f);

                if (cameraForward.GetLengthSq() > 0.0001f)
                {
                    AZ::Vector3 targetPosition = myPosition + cameraForward;
                    finalTransform = AZ::Transform::CreateLookAt(myPosition, targetPosition, AZ::Transform::Axis::YPositive);
                }
                else
                {
                    AZ::Vector3 cameraUp = cameraTransform.GetBasisZ();
                    cameraUp.SetZ(0.0f);
                    AZ::Vector3 targetPosition = myPosition + cameraUp;
                    finalTransform = AZ::Transform::CreateLookAt(myPosition, targetPosition, AZ::Transform::Axis::YPositive);
                }

                finalTransform = finalTransform * rotationOffset;
            }
            else // Spherical
            {
                AZ::Vector3 targetPosition = cameraTransform.GetTranslation();
                
                if (!myPosition.IsClose(targetPosition, 0.001f))
                {
                    finalTransform = AZ::Transform::CreateLookAt(myPosition, targetPosition, AZ::Transform::Axis::YNegative);
                    finalTransform = finalTransform * rotationOffset;
                }
                else 
                {
                    return;
                }
            }

            AZ::TransformBus::Event(GetEntityId(), &AZ::TransformBus::Events::SetWorldTM, finalTransform);
        }
    }
}