#include "SpriteComponent.h"
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h> 
#include <AtomLyIntegration/CommonFeatures/Material/MaterialComponentBus.h>
#include <AtomLyIntegration/CommonFeatures/Material/MaterialAssignment.h>
#include <AzCore/std/any.h>

namespace BillboardGem
{
    void SpriteComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<SpriteAnimation>()
                ->Version(1)
                ->Field("Name", &SpriteAnimation::m_name)
                ->Field("StartFrame", &SpriteAnimation::m_startFrame)
                ->Field("EndFrame", &SpriteAnimation::m_endFrame)
                ->Field("FPS", &SpriteAnimation::m_fps);

            serializeContext->Class<SpriteComponent, AZ::Component>()
                ->Version(1)
                ->Field("UVTileUProperty", &SpriteComponent::m_uvTileUProperty)
                ->Field("UVTileVProperty", &SpriteComponent::m_uvTileVProperty)
                ->Field("UVOffsetUProperty", &SpriteComponent::m_uvOffsetUProperty)
                ->Field("UVOffsetVProperty", &SpriteComponent::m_uvOffsetVProperty)
                ->Field("Columns", &SpriteComponent::m_columns)
                ->Field("Rows", &SpriteComponent::m_rows)
                ->Field("DefaultAnimation", &SpriteComponent::m_defaultAnimation)
                ->Field("Animations", &SpriteComponent::m_animations);

            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                editContext->Class<SpriteAnimation>("Animation State", "A single animation sequence")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteAnimation::m_name, "Name", "")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteAnimation::m_startFrame, "Start Frame", "")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteAnimation::m_endFrame, "End Frame", "")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteAnimation::m_fps, "FPS", "");

                editContext->Class<SpriteComponent>("Sprite Animator", "Plays specific animations from a spritesheet.")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::Category, "Rendering")
                        ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                    
                    ->ClassElement(AZ::Edit::ClassElements::Group, "Grid Settings")
                        ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteComponent::m_columns, "Columns", "")
                        ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteComponent::m_rows, "Rows", "")
                    
                    ->ClassElement(AZ::Edit::ClassElements::Group, "Material Integration")
                        ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteComponent::m_uvTileUProperty, "Tile U String", "")
                        ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteComponent::m_uvTileVProperty, "Tile V String", "")
                        ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteComponent::m_uvOffsetUProperty, "Offset U String", "")
                        ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteComponent::m_uvOffsetVProperty, "Offset V String", "")
                    
                    ->ClassElement(AZ::Edit::ClassElements::Group, "Animations")
                        ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteComponent::m_defaultAnimation, "Default State", "")
                        ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteComponent::m_animations, "Animation List", "");
            }
        }

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
            float tileU = 1.0f / m_columns;
            float tileV = 1.0f / m_rows;
            AZ::Render::MaterialAssignmentId defaultMaterialId;
            
            AZ::Render::MaterialComponentRequestBus::Event(
                GetEntityId(), &AZ::Render::MaterialComponentRequests::SetPropertyValue, 
                defaultMaterialId, m_uvTileUProperty, AZStd::make_any<float>(tileU));
                
            AZ::Render::MaterialComponentRequestBus::Event(
                GetEntityId(), &AZ::Render::MaterialComponentRequests::SetPropertyValue, 
                defaultMaterialId, m_uvTileVProperty, AZStd::make_any<float>(tileV));
        }

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
        for (const auto& anim : m_animations)
        {
            if (anim.m_name == animationName)
            {
                m_currentAnim = anim;
                m_currentFrame = anim.m_startFrame; 
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

            if (m_currentFrame > m_currentAnim.m_endFrame)
            {
                m_currentFrame = m_currentAnim.m_startFrame; 
            }

            float scaleX = 1.0f / m_columns;
            float scaleY = 1.0f / m_rows;
            
            int currentColumn = m_currentFrame % m_columns;
            int currentRow = m_currentFrame / m_columns;

            float offsetX = currentColumn * scaleX;
            float offsetY = currentRow * scaleY; 

            AZ::Render::MaterialAssignmentId defaultMaterialId;
            
            AZ::Render::MaterialComponentRequestBus::Event(
                GetEntityId(), &AZ::Render::MaterialComponentRequests::SetPropertyValue, 
                defaultMaterialId, m_uvOffsetUProperty, AZStd::make_any<float>(offsetX));
                
            AZ::Render::MaterialComponentRequestBus::Event(
                GetEntityId(), &AZ::Render::MaterialComponentRequests::SetPropertyValue, 
                defaultMaterialId, m_uvOffsetVProperty, AZStd::make_any<float>(offsetY));
        }
    }
}