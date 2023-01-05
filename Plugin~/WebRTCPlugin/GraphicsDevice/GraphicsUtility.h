#pragma once
#include "IGraphicsDevice.h"


namespace unity
{
namespace webrtc
{

class GraphicsUtility {
public:
    static rtc::scoped_refptr<::webrtc::I420Buffer> ConvertRGBToI420Buffer(const uint32_t width, const uint32_t height,
        const uint32_t rowToRowInBytes, const uint8_t* srcData);
    static void ConvertRGBToI420BufferInThread(const uint8_t* srcData, uint8_t* yuv_y, uint8_t* yuv_u, uint8_t* yuv_v,
        const uint32_t yStart, const uint32_t uvStart, const uint32_t fullWidth, const uint32_t splitedHeight, const uint32_t rowToRowInBytes);
    static IGraphicsDevice* GetGraphicsDevice();
    static UnityGfxRenderer GetGfxRenderer();
    static void* TextureHandleToNativeGraphicsPtr(
        void* textureHandle, IGraphicsDevice* device, UnityGfxRenderer renderer);

};

} // end namespace webrtc
} // end namespace unity
