#include "SpriteAnimationAsset.h"
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>

namespace BillboardGem
{
    void SpriteAnimationAsset::Reflect(AZ::ReflectContext* context)
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

            serializeContext->Class<SpriteAnimationAsset, AZ::Data::AssetData>()
                ->Version(1)
                ->Field("Columns", &SpriteAnimationAsset::m_columns)
                ->Field("Rows", &SpriteAnimationAsset::m_rows)
                ->Field("UVTileUProperty", &SpriteAnimationAsset::m_uvTileUProperty)
                ->Field("UVTileVProperty", &SpriteAnimationAsset::m_uvTileVProperty)
                ->Field("UVOffsetUProperty", &SpriteAnimationAsset::m_uvOffsetUProperty)
                ->Field("UVOffsetVProperty", &SpriteAnimationAsset::m_uvOffsetVProperty)
                ->Field("DefaultAnimation", &SpriteAnimationAsset::m_defaultAnimation)
                ->Field("Animations", &SpriteAnimationAsset::m_animations);

            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                editContext->Class<SpriteAnimation>("Animation State", "A single animation sequence")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteAnimation::m_name, "Name", "")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteAnimation::m_startRow, "Start Row", "")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteAnimation::m_startColumn, "Start Column", "")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteAnimation::m_frameCount, "Frame Count", "")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteAnimation::m_fps, "FPS", "");

                editContext->Class<SpriteAnimationAsset>("Sprite Animation Asset", "Defines a spritesheet layout and its animations")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)

                    ->ClassElement(AZ::Edit::ClassElements::Group, "Grid Settings")
                        ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteAnimationAsset::m_columns, "Columns", "")
                        ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteAnimationAsset::m_rows, "Rows", "")

                    ->ClassElement(AZ::Edit::ClassElements::Group, "Material Integration")
                        ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteAnimationAsset::m_uvTileUProperty, "Tile U String", "")
                        ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteAnimationAsset::m_uvTileVProperty, "Tile V String", "")
                        ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteAnimationAsset::m_uvOffsetUProperty, "Offset U String", "")
                        ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteAnimationAsset::m_uvOffsetVProperty, "Offset V String", "")

                    ->ClassElement(AZ::Edit::ClassElements::Group, "Animations")
                        ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteAnimationAsset::m_defaultAnimation, "Default State", "")
                        ->DataElement(AZ::Edit::UIHandlers::Default, &SpriteAnimationAsset::m_animations, "Animation List", "");
            }
        }
    }
}