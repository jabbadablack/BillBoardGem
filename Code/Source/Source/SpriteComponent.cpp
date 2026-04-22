#include "SpriteComponent.h"
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h> 
#include <AtomLyIntegration/CommonFeatures/Material/MaterialComponentBus.h>
#include <AzCore/std/any.h>

namespace BillboardGem
{
    void SpriteComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<SpriteAnimation>()
                ->Version(2)
                ->Field("Name", &SpriteAnimation::m_name)
                ->Field("StartRow", &SpriteAnimation::m_startRow)
                ->Field("StartColumn", &SpriteAnimation::m_startColumn)
                ->Field("FrameCount", &SpriteAnimation::m_frameCount)
                ->Field("FPS", &SpriteAnimation::m_fps);

            serializeContext->Class<SpriteComponent, AZ::Component>()
                ->Version(1)
                ->Field("MaterialEntity", &SpriteComponent::m_materialEntityId)
                ->Field("StartupDelayFrames", &SpriteComponent::m_startupDelayFrames) 
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
                    ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteAnimation::m_startRow, "Start Row", "Row index (Starts at 0)")
                        ->Attribute(AZ::Edit::Attributes::Min, 0)
                    ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteAnimation::m_startColumn, "Start Column", "Column index (Starts at 0)")
                        ->Attribute(AZ::Edit::Attributes::Min, 0)
                    ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteAnimation::m_frameCount, "Frame Count", "Total frames in this animation")
                        ->Attribute(AZ::Edit::Attributes::Min, 1)
                    ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteAnimation::m_fps, "FPS", "");

                editContext->Class<SpriteComponent>("Sprite Animator", "Plays specific animations from a spritesheet.")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::Category, "Rendering")
                        ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                    
                    ->ClassElement(AZ::Edit::ClassElements::Group, "Grid Settings")
                        ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteComponent::m_columns, "Columns", "")
                            ->Attribute(AZ::Edit::Attributes::Min, 1)
                        ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteComponent::m_rows, "Rows", "")
                            ->Attribute(AZ::Edit::Attributes::Min, 1)
                    
                    ->ClassElement(AZ::Edit::ClassElements::Group, "Material Integration")
                        ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteComponent::m_materialEntityId, "Target Entity", "Leave blank to target this entity.")
                        ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteComponent::m_startupDelayFrames, "Startup Delay", "Frames to wait before sending data to Atom.")
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
                
                m_currentFrame = (anim.m_startRow * m_columns) + anim.m_startColumn; 
                
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

    AZStd::vector<AZ::Render::MaterialAssignmentId> SpriteComponent::GetActiveMaterialIds(AZ::EntityId targetEntity)
    {
        AZStd::vector<AZ::Render::MaterialAssignmentId> ids;
        AZ::Render::MaterialAssignmentMap materialMap;
        
        AZ::Render::MaterialComponentRequestBus::EventResult(
            materialMap, targetEntity, &AZ::Render::MaterialComponentRequests::GetDefaultMaterialMap);

        for (const auto& pair : materialMap)
        {
            ids.push_back(pair.first);
        }
        
        if (ids.empty())
        {
            ids.push_back(AZ::Render::MaterialAssignmentId());
        }

        return ids;
    }

    void SpriteComponent::ApplyMaterialScale(float tileU, float tileV)
    {
        AZ::EntityId targetEntity = m_materialEntityId.IsValid() ? m_materialEntityId : GetEntityId();
        auto materialIds = GetActiveMaterialIds(targetEntity);
        
        for (const auto& id : materialIds)
        {
            AZ::Render::MaterialComponentRequestBus::Event(
                targetEntity, &AZ::Render::MaterialComponentRequests::SetPropertyValue, 
                id, m_uvTileUProperty, AZStd::make_any<float>(tileU));
                
            AZ::Render::MaterialComponentRequestBus::Event(
                targetEntity, &AZ::Render::MaterialComponentRequests::SetPropertyValue, 
                id, m_uvTileVProperty, AZStd::make_any<float>(tileV));
        }
    }

    void SpriteComponent::ApplyMaterialOffset(float offsetU, float offsetV)
    {
        AZ::EntityId targetEntity = m_materialEntityId.IsValid() ? m_materialEntityId : GetEntityId();
        auto materialIds = GetActiveMaterialIds(targetEntity);
        
        for (const auto& id : materialIds)
        {
            AZ::Render::MaterialComponentRequestBus::Event(
                targetEntity, &AZ::Render::MaterialComponentRequests::SetPropertyValue, 
                id, m_uvOffsetUProperty, AZStd::make_any<float>(offsetU));
                
            AZ::Render::MaterialComponentRequestBus::Event(
                targetEntity, &AZ::Render::MaterialComponentRequests::SetPropertyValue, 
                id, m_uvOffsetVProperty, AZStd::make_any<float>(offsetV));
        }
    }

    void SpriteComponent::OnTick(float deltaTime, AZ::ScriptTimePoint time)
    {
        if (m_startupDelayFrames > 0)
        {
            m_startupDelayFrames--;
            
            if (m_startupDelayFrames == 0 && m_columns > 0 && m_rows > 0)
            {
                float tileU = 1.0f / static_cast<float>(m_columns);
                float tileV = 1.0f / static_cast<float>(m_rows);
                ApplyMaterialScale(tileU, tileV);
            }
            return; 
        }

        if (!m_isPlaying || m_columns <= 0 || m_rows <= 0 || m_currentAnim.m_frameCount <= 0 || m_currentAnim.m_fps <= 0.0f) return;

        m_timeAccumulator += deltaTime;
        float frameDuration = 1.0f / m_currentAnim.m_fps;

        if (m_timeAccumulator >= frameDuration)
        {
            m_timeAccumulator -= frameDuration; 
            m_currentFrame++;

            int globalStartFrame = (m_currentAnim.m_startRow * m_columns) + m_currentAnim.m_startColumn;
            int globalEndFrame = globalStartFrame + m_currentAnim.m_frameCount - 1;

            if (m_currentFrame > globalEndFrame)
            {
                m_currentFrame = globalStartFrame; 
            }

            float scaleX = 1.0f / static_cast<float>(m_columns);
            float scaleY = 1.0f / static_cast<float>(m_rows);
            
            int currentColumn = m_currentFrame % m_columns;
            int currentRow = m_currentFrame / m_columns;

            float offsetX = currentColumn * scaleX;
            float offsetY = currentRow * scaleY; 

            ApplyMaterialOffset(offsetX, offsetY);
        }
    }
}