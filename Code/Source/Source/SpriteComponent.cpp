#include "SpriteComponent.h"
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AtomLyIntegration/CommonFeatures/Material/MaterialComponentBus.h>
#include <AtomLyIntegration/CommonFeatures/Material/MaterialAssignment.h>
#include <AzCore/std/string/string.h>
#include <AzCore/std/any.h>

namespace BillboardGem
{
    void SpriteComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<SpriteComponent, AZ::Component>()
                ->Version(1)
                ->Field("PlayAnimation", &SpriteComponent::m_playAnimation)
                ->Field("Columns", &SpriteComponent::m_columns)
                ->Field("Rows", &SpriteComponent::m_rows)
                ->Field("FPS", &SpriteComponent::m_fps);

            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                editContext->Class<SpriteComponent>("Sprite Animator", "Animates a spritesheet on a Material Component")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::Category, "Rendering")
                        ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                    
                    ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteComponent::m_playAnimation, "Play", "Toggle animation on/off.")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteComponent::m_columns, "Columns", "Number of columns in the spritesheet.")
                        ->Attribute(AZ::Edit::Attributes::Min, 1)
                    ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteComponent::m_rows, "Rows", "Number of rows in the spritesheet.")
                        ->Attribute(AZ::Edit::Attributes::Min, 1)
                    ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteComponent::m_fps, "Framerate (FPS)", "How fast the animation plays.")
                        ->Attribute(AZ::Edit::Attributes::Min, 0.1f);
            }
        }
    }

    void SpriteComponent::Init() {}

    void SpriteComponent::Activate()
    {
        if (m_columns > 0 && m_rows > 0)
        {
            AZ::Vector2 uvScale(1.0f / m_columns, 1.0f / m_rows);
            
            // FIXED: We now pass a default MaterialAssignmentId (which targets the primary material) 
            // and use AZStd::string for the property name.
            AZ::Render::MaterialAssignmentId defaultMaterialId;
            AZ::Render::MaterialComponentRequestBus::Event(
                GetEntityId(), &AZ::Render::MaterialComponentRequests::SetPropertyValue, 
                defaultMaterialId, AZStd::string("baseColor.uv.scale"), AZStd::make_any<AZ::Vector2>(uvScale));
        }

        AZ::TickBus::Handler::BusConnect();
    }

    void SpriteComponent::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
    }

    void SpriteComponent::OnTick(float deltaTime, AZ::ScriptTimePoint time)
    {
        if (!m_playAnimation || m_columns <= 0 || m_rows <= 0) return;

        m_timeAccumulator += deltaTime;
        float frameDuration = 1.0f / m_fps;

        if (m_timeAccumulator >= frameDuration)
        {
            m_timeAccumulator -= frameDuration; 
            m_currentFrame++;

            int totalFrames = m_columns * m_rows;
            if (m_currentFrame >= totalFrames)
            {
                m_currentFrame = 0; 
            }

            float scaleX = 1.0f / m_columns;
            float scaleY = 1.0f / m_rows;
            
            int currentColumn = m_currentFrame % m_columns;
            int currentRow = m_currentFrame / m_columns;

            float offsetX = currentColumn * scaleX;
            float offsetY = 1.0f - ((currentRow + 1) * scaleY);

            AZ::Vector2 uvOffset(offsetX, offsetY);

            // FIXED: Applying the same fix to the offset update!
            AZ::Render::MaterialAssignmentId defaultMaterialId;
            AZ::Render::MaterialComponentRequestBus::Event(
                GetEntityId(), &AZ::Render::MaterialComponentRequests::SetPropertyValue, 
                defaultMaterialId, AZStd::string("baseColor.uv.offset"), AZStd::make_any<AZ::Vector2>(uvOffset));
        }
    }
}