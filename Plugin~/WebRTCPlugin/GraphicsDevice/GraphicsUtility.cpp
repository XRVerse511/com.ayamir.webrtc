#include "pch.h"
#include "GraphicsUtility.h"

#if defined(SUPPORT_VULKAN)
#include "Vulkan/VulkanGraphicsDevice.h"
#endif

#include <chrono>
#include <thread>

#define GraphicsUtilityLog(...)       LogPrint("webrtc Log: GraphicsUtility::" __VA_ARGS__)

namespace unity
{
namespace webrtc
{

namespace webrtc = ::webrtc;

void GraphicsUtility::ConvertRGBToI420BufferInThread(const uint8_t* srcData, uint8_t* yuv_y, uint8_t* yuv_u, uint8_t* yuv_v,
    const uint32_t yStart, const uint32_t uvStart, const uint32_t fullWidth, const uint32_t deltaHeight, const uint32_t rowToRowInBytes)
{
    int yIndex = yStart * fullWidth;
    int uIndex = uvStart * fullWidth;
    int vIndex = uvStart * fullWidth;

    for (uint32_t i = yStart; i < yStart + deltaHeight; i++)
    {
        for (uint32_t j = 0; j < fullWidth; j++)
        {
            const uint32_t startIndex = i * rowToRowInBytes + j * 4;
            const int B = srcData[startIndex + 0];
            const int G = srcData[startIndex + 1];
            const int R = srcData[startIndex + 2];

            const int Y = ((66 * R + 129 * G + 25 * B + 128) >> 8) + 16;
            const int U = ((-38 * R - 74 * G + 112 * B + 128) >> 8) + 128;
            const int V = ((112 * R - 94 * G - 18 * B + 128) >> 8) + 128;

            yuv_y[yIndex++] = static_cast<uint8_t>((Y < 0) ? 0 : ((Y > 255) ? 255 : Y));
            if (i % 2 == 0 && j % 2 == 0)
            {
                yuv_u[uIndex] = static_cast<uint8_t>((U < 0) ? 0 : ((U > 255) ? 255 : U));
                uIndex += 1;

                yuv_v[vIndex] = static_cast<uint8_t>((V < 0) ? 0 : ((V > 255) ? 255 : V));
                vIndex += 1;
            }
        }
    }
}

rtc::scoped_refptr<webrtc::I420Buffer> GraphicsUtility::ConvertRGBToI420Buffer(const uint32_t width, const uint32_t height,
    const uint32_t rowToRowInBytes, const uint8_t* srcData)
{
    rtc::scoped_refptr<webrtc::I420Buffer> i420_buffer = webrtc::I420Buffer::Create(width, height);

    uint8_t* yuv_y = i420_buffer->MutableDataY();
    uint8_t* yuv_u = i420_buffer->MutableDataU();
    uint8_t* yuv_v = i420_buffer->MutableDataV();

    int threadCount = 8;
    auto uvDelta = height / (threadCount * 4);
    auto heightDelta = height / threadCount;
    std::thread t0(ConvertRGBToI420BufferInThread, srcData, yuv_y, yuv_u, yuv_v, 0 * heightDelta, 0 * uvDelta, width, heightDelta, rowToRowInBytes);
    std::thread t1(ConvertRGBToI420BufferInThread, srcData, yuv_y, yuv_u, yuv_v, 1 * heightDelta, 1 * uvDelta, width, heightDelta, rowToRowInBytes);
    std::thread t2(ConvertRGBToI420BufferInThread, srcData, yuv_y, yuv_u, yuv_v, 2 * heightDelta, 2 * uvDelta, width, heightDelta, rowToRowInBytes);
    std::thread t3(ConvertRGBToI420BufferInThread, srcData, yuv_y, yuv_u, yuv_v, 3 * heightDelta, 3 * uvDelta, width, heightDelta, rowToRowInBytes);
    std::thread t4(ConvertRGBToI420BufferInThread, srcData, yuv_y, yuv_u, yuv_v, 4 * heightDelta, 4 * uvDelta, width, heightDelta, rowToRowInBytes);
    std::thread t5(ConvertRGBToI420BufferInThread, srcData, yuv_y, yuv_u, yuv_v, 5 * heightDelta, 5 * uvDelta, width, heightDelta, rowToRowInBytes);
    std::thread t6(ConvertRGBToI420BufferInThread, srcData, yuv_y, yuv_u, yuv_v, 6 * heightDelta, 6 * uvDelta, width, heightDelta, rowToRowInBytes);
    std::thread t7(ConvertRGBToI420BufferInThread, srcData, yuv_y, yuv_u, yuv_v, 7 * heightDelta, 7 * uvDelta, width, heightDelta, rowToRowInBytes);
    if (t0.joinable()) {
        t0.join();
    }
    if (t1.joinable()) {
        t1.join();
    }
    if (t2.joinable()) {
        t2.join();
    }
    if (t3.joinable()) {
        t3.join();
    }
    if (t4.joinable()) {
        t4.join();
    }
    if (t5.joinable()) {
        t5.join();
    }
    if (t6.joinable()) {
        t6.join();
    }
    if (t7.joinable()) {
        t7.join();
    }

    return i420_buffer;
}

void* GraphicsUtility::TextureHandleToNativeGraphicsPtr(
    void* textureHandle, IGraphicsDevice* device, UnityGfxRenderer renderer)
{
#if defined(SUPPORT_VULKAN)
    if (renderer == kUnityGfxRendererVulkan)
    {
        VulkanGraphicsDevice* vulkanDevice =
            static_cast<VulkanGraphicsDevice*>(device);
        std::unique_ptr<UnityVulkanImage> unityVulkanImage =
            vulkanDevice->AccessTexture(textureHandle);
        return unityVulkanImage.release();
    }
#endif
    return textureHandle;
}

} // end namespace webrtc
} // end namespace unity

