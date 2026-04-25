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
        SpriteAnimationAsset::Reflect(context); // Reflect the asset here too!

        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<SpriteComponent, AZ::Component>()
                ->Version(2)
                ->Field("MaterialEntity", &SpriteComponent::m_materialEntityId)
                ->Field("SpriteAsset", &SpriteComponent::m_spriteAsset);

            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                editContext->Class<SpriteComponent>("Sprite Animator", "Plays sprite animations from a .spranim asset.")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "Rendering")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))

                    ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteComponent::m_materialEntityId, "Target Entity", "Leave blank to target this entity.")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteComponent::m_spriteAsset, "Sprite Asset", "The .spranim file containing the animation data.");
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
        m_isMaterialInitialized = false;
        SpriteAnimatorRequestBus::Handler::BusConnect(GetEntityId());

        // Queue the asset to load async when the component starts
        if (m_spriteAsset.GetId().IsValid())
        {
            AZ::Data::AssetBus::Handler::BusConnect(m_spriteAsset.GetId());
            m_spriteAsset.QueueLoad();
        }
    }

    void SpriteComponent::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
        SpriteAnimatorRequestBus::Handler::BusDisconnect();
        AZ::Data::AssetBus::Handler::BusDisconnect();
        
        if (m_spriteAsset.GetId().IsValid())
        {
            m_spriteAsset.Release();
        }
    }

    void SpriteComponent::OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset)
    {
        m_spriteAsset = asset;
        
        // Once the asset is ready, start the animation and connect to the TickBus
        if (m_spriteAsset.IsReady())
        {
            PlayAnimation(m_spriteAsset.Get()->m_defaultAnimation);
            AZ::TickBus::Handler::BusConnect();
        }
    }

    void SpriteComponent::OnAssetReloaded(AZ::Data::Asset<AZ::Data::AssetData> asset)
    {
        OnAssetReady(asset); // Re-initialize if the user edits the asset while the game is running
    }

    void SpriteComponent::PlayAnimation(const AZStd::string& animationName)
    {
        if (!m_spriteAsset.IsReady()) return;

        auto* assetData = m_spriteAsset.Get();
        for (const auto& anim : assetData->m_animations)
        {
            if (anim.m_name == animationName)
            {
                m_currentAnim = anim;
                m_currentFrame = (anim.m_startRow * assetData->m_columns) + anim.m_startColumn;
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

        if (!AZ::Render::MaterialComponentRequestBus::HasHandlers(targetEntity)) return ids;

        AZ::Render::MaterialComponentRequestBus::EventResult(materialMap, targetEntity, &AZ::Render::MaterialComponentRequests::GetMaterialMap);

        if (materialMap.empty())
        {
            AZ::Render::MaterialComponentRequestBus::EventResult(materialMap, targetEntity, &AZ::Render::MaterialComponentRequests::GetDefaultMaterialMap);
        }

        for (const auto& pair : materialMap)
        {
            ids.push_back(pair.first);
        }

        if (ids.empty()) ids.push_back(AZ::Render::MaterialAssignmentId());

        return ids;
    }

    void SpriteComponent::ApplyMaterialScale(float tileU, float tileV)
    {
        AZ::EntityId targetEntity = m_materialEntityId.IsValid() ? m_materialEntityId : GetEntityId();
        auto materialIds = GetActiveMaterialIds(targetEntity);
        if (materialIds.empty() || !m_spriteAsset.IsReady()) return;

        for (const auto& id : materialIds)
        {
            AZ::Render::MaterialComponentRequestBus::Event(targetEntity, &AZ::Render::MaterialComponentRequests::SetPropertyValue,
                id, m_spriteAsset.Get()->m_uvTileUProperty, AZStd::make_any<float>(tileU));

            AZ::Render::MaterialComponentRequestBus::Event(targetEntity, &AZ::Render::MaterialComponentRequests::SetPropertyValue,
                id, m_spriteAsset.Get()->m_uvTileVProperty, AZStd::make_any<float>(tileV));
        }
    }

    void SpriteComponent::ApplyMaterialOffset(float offsetU, float offsetV)
    {
        AZ::EntityId targetEntity = m_materialEntityId.IsValid() ? m_materialEntityId : GetEntityId();
        auto materialIds = GetActiveMaterialIds(targetEntity);
        if (materialIds.empty() || !m_spriteAsset.IsReady()) return;

        for (const auto& id : materialIds)
        {
            AZ::Render::MaterialComponentRequestBus::Event(targetEntity, &AZ::Render::MaterialComponentRequests::SetPropertyValue,
                id, m_spriteAsset.Get()->m_uvOffsetUProperty, AZStd::make_any<float>(offsetU));

            AZ::Render::MaterialComponentRequestBus::Event(targetEntity, &AZ::Render::MaterialComponentRequests::SetPropertyValue,
                id, m_spriteAsset.Get()->m_uvOffsetVProperty, AZStd::make_any<float>(offsetV));
        }
    }

    void SpriteComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("SpriteAnimatorService"));
    }

    void SpriteComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("SpriteAnimatorService"));
    }

    void SpriteComponent::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        dependent.push_back(AZ_CRC_CE("MaterialService"));
    }

    void SpriteComponent::OnTick(float deltaTime, AZ::ScriptTimePoint /*time*/)
    {
        if (!m_spriteAsset.IsReady()) return;

        auto* assetData = m_spriteAsset.Get();
        AZ::EntityId targetEntity = m_materialEntityId.IsValid() ? m_materialEntityId : GetEntityId();

        if (!AZ::Render::MaterialComponentRequestBus::HasHandlers(targetEntity)) return;

        if (!m_isMaterialInitialized)
        {
            if (assetData->m_columns > 0 && assetData->m_rows > 0)
            {
                ApplyMaterialScale(1.0f / assetData->m_columns, 1.0f / assetData->m_rows);
            }
            m_isMaterialInitialized = true;
        }

        if (!m_isPlaying || assetData->m_columns <= 0 || assetData->m_rows <= 0 || m_currentAnim.m_frameCount <= 0 || m_currentAnim.m_fps <= 0.0f) return;

        m_timeAccumulator += deltaTime;
        float frameDuration = 1.0f / m_currentAnim.m_fps;

        if (m_timeAccumulator >= frameDuration)
        {
            m_timeAccumulator -= frameDuration;
            m_currentFrame++;

            int globalStartFrame = (m_currentAnim.m_startRow * assetData->m_columns) + m_currentAnim.m_startColumn;
            int globalEndFrame = globalStartFrame + m_currentAnim.m_frameCount - 1;

            if (m_currentFrame > globalEndFrame)
            {
                m_currentFrame = globalStartFrame;
            }

            float scaleX = 1.0f / static_cast<float>(assetData->m_columns);
            float scaleY = 1.0f / static_cast<float>(assetData->m_rows);

            int currentColumn = m_currentFrame % assetData->m_columns;
            int currentRow = m_currentFrame / assetData->m_columns;

            ApplyMaterialOffset(currentColumn * scaleX, currentRow * scaleY);
        }
    }
}