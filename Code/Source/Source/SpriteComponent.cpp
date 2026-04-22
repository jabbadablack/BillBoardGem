#include "SpriteComponent.h"
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h> // NEW: Needed for Script Canvas!
#include <AtomLyIntegration/CommonFeatures/Material/MaterialComponentBus.h>
#include <AtomLyIntegration/CommonFeatures/Material/MaterialAssignment.h>
#include <AzCore/std/any.h>

namespace BillboardGem
{
    void SpriteComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            // Reflect the custom struct so the Editor understands it
            serializeContext->Class<SpriteAnimation>()
                ->Version(1)
                ->Field("Name", &SpriteAnimation::m_name)
                ->Field("StartFrame", &SpriteAnimation::m_startFrame)
                ->Field("EndFrame", &SpriteAnimation::m_endFrame)
                ->Field("FPS", &SpriteAnimation::m_fps);

            serializeContext->Class<SpriteComponent, AZ::Component>()
                ->Version(1)
                ->Field("UVScaleProperty", &SpriteComponent::m_uvScaleProperty)
                ->Field("UVOffsetProperty", &SpriteComponent::m_uvOffsetProperty)
                ->Field("Columns", &SpriteComponent::m_columns)
                ->Field("Rows", &SpriteComponent::m_rows)
                ->Field("DefaultAnimation", &SpriteComponent::m_defaultAnimation)
                ->Field("Animations", &SpriteComponent::m_animations);

            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                // UI for the Struct
                editContext->Class<SpriteAnimation>("Animation State", "A single animation sequence")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteAnimation::m_name, "Name", "e.g., Idle, Walk, Attack")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteAnimation::m_startFrame, "Start Frame", "")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteAnimation::m_endFrame, "End Frame", "")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteAnimation::m_fps, "FPS", "");

                // UI for the Component
                editContext->Class<SpriteComponent>("Sprite Animator", "Plays specific animations from a spritesheet.")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::Category, "Rendering")
                        ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                    
                    ->ClassElement(AZ::Edit::ClassElements::Group, "Grid Settings")
                        ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteComponent::m_columns, "Columns", "")
                        ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteComponent::m_rows, "Rows", "")
                    
                    ->ClassElement(AZ::Edit::ClassElements::Group, "Material Integration")
                        ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteComponent::m_uvScaleProperty, "Scale Property", "The exact material property name.")
                        ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteComponent::m_uvOffsetProperty, "Offset Property", "The exact material property name.")
                    
                    ->ClassElement(AZ::Edit::ClassElements::Group, "Animations")
                        ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteComponent::m_defaultAnimation, "Default State", "Animation to play on startup")
                        ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteComponent::m_animations, "Animation List", "Add your animation states here");
            }
        }

        // NEW: Expose the EBus to Script Canvas and Lua!
        if (auto behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->EBus<SpriteAnimatorRequestBus>("SpriteAnimatorRequestBus")
                ->Event("PlayAnimation", &SpriteAnimatorRequestBus::Events::PlayAnimation)
                ->Event("StopAnimation", &SpriteAnimatorRequestBus::Events::StopAnimation);
        }
    }

    void SpriteComponent::Init() {}

    void SpriteComponent::Activate()
    {
        SpriteAnimatorRequestBus::Handler::BusConnect(GetEntityId());

        if (m_columns > 0 && m_rows > 0)
        {
            AZ::Vector2 uvScale(1.0f / m_columns, 1.0f / m_rows);
            AZ::Render::MaterialAssignmentId defaultMaterialId;
            
            // Apply scale using the exposed property string
            AZ::Render::MaterialComponentRequestBus::Event(
                GetEntityId(), &AZ::Render::MaterialComponentRequests::SetPropertyValue, 
                defaultMaterialId, m_uvScaleProperty, AZStd::make_any<AZ::Vector2>(uvScale));
        }

        // Start the default animation automatically
        PlayAnimation(m_defaultAnimation);
        
        AZ::TickBus::Handler::BusConnect();
    }

    void SpriteComponent::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
        SpriteAnimatorRequestBus::Handler::BusDisconnect();
    }

    void SpriteComponent::PlayAnimation(const AZStd::string& animationName)
    {
        // Search our list for an animation with the matching name
        for (const auto& anim : m_animations)
        {
            if (anim.m_name == animationName)
            {
                m_currentAnim = anim;
                m_currentFrame = anim.m_startFrame; // Snap to the first frame
                m_timeAccumulator = 0.0f;
                m_isPlaying = true;
                return;
            }
        }
    }

    void SpriteComponent::StopAnimation()
    {
        m_isPlaying = false;
    }

    void SpriteComponent::OnTick(float deltaTime, AZ::ScriptTimePoint time)
    {
        if (!m_isPlaying || m_columns <= 0 || m_rows <= 0 || m_currentAnim.m_fps <= 0.0f) return;

        m_timeAccumulator += deltaTime;
        float frameDuration = 1.0f / m_currentAnim.m_fps;

        if (m_timeAccumulator >= frameDuration)
        {
            m_timeAccumulator -= frameDuration; 
            m_currentFrame++;

            // Ensure we loop safely within the start and end frame limits
            if (m_currentFrame > m_currentAnim.m_endFrame)
            {
                m_currentFrame = m_currentAnim.m_startFrame; 
            }

            // Grid Math for multi-row, multi-column spritesheets
            float scaleX = 1.0f / m_columns;
            float scaleY = 1.0f / m_rows;
            
            int currentColumn = m_currentFrame % m_columns;
            int currentRow = m_currentFrame / m_columns;

            // Standard Top-Left origin math
            float offsetX = currentColumn * scaleX;
            float offsetY = currentRow * scaleY; 

            AZ::Vector2 uvOffset(offsetX, offsetY);

            // Apply offset using the exposed property string
            AZ::Render::MaterialAssignmentId defaultMaterialId;
            AZ::Render::MaterialComponentRequestBus::Event(
                GetEntityId(), &AZ::Render::MaterialComponentRequests::SetPropertyValue, 
                defaultMaterialId, m_uvOffsetProperty, AZStd::make_any<AZ::Vector2>(uvOffset));
        }
    }
}