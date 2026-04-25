#pragma once

#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/RTTI/TypeInfo.h>
#include <AzCore/std/string/string.h>
#include <AzCore/std/containers/vector.h>

namespace BillboardGem
{
    struct SpriteAnimation
    {
        AZ_TYPE_INFO(SpriteAnimation, "{80c60379-5b95-44b1-abba-286cbf630a03}");
        AZ_CLASS_ALLOCATOR(SpriteAnimation, AZ::SystemAllocator);

        AZStd::string m_name = "Idle";
        int m_startRow = 0;
        int m_startColumn = 0;
        int m_frameCount = 1;
        float m_fps = 12.0f;
    };

    class SpriteAnimationAsset : public AZ::Data::AssetData
    {
    public:
        AZ_RTTI(SpriteAnimationAsset, "{c21cc423-056f-40da-8c21-895761e9c2ba}", AZ::Data::AssetData);
        AZ_CLASS_ALLOCATOR(SpriteAnimationAsset, AZ::SystemAllocator);

        static void Reflect(AZ::ReflectContext* context);

        int m_columns = 1;
        int m_rows = 1;

        AZStd::string m_uvTileUProperty = "uv.tileU";
        AZStd::string m_uvTileVProperty = "uv.tileV";
        AZStd::string m_uvOffsetUProperty = "uv.offsetU";
        AZStd::string m_uvOffsetVProperty = "uv.offsetV";

        AZStd::vector<SpriteAnimation> m_animations;
        AZStd::string m_defaultAnimation = "Idle";
    };
}