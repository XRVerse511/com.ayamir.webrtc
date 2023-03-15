#include "pch.h"
#include "SoftwareEncoder.h"
#include "GraphicsDevice/IGraphicsDevice.h"
#include "GraphicsDevice/ITexture2D.h"

#include <chrono>

#if defined(_WIN32)
#else
#include <dlfcn.h>
#endif

#define LG(...)       LogPrint("webrtc Log: SoftwareEncoder.cpp::" __VA_ARGS__)

namespace unity
{
    namespace webrtc
    {

        SoftwareEncoder::SoftwareEncoder(int width, int height, IGraphicsDevice* device, UnityRenderingExtTextureFormat textureFormat) :
            m_device(device),
            m_width(width),
            m_height(height),
            m_encodeTex(nullptr),
            m_textureFormat(textureFormat)
        {
        }

        SoftwareEncoder::~SoftwareEncoder()
        {
            delete m_encodeTex;
            m_encodeTex = nullptr;
        }

        void SoftwareEncoder::InitV()
        {
            m_encodeTex = m_device->CreateCPUReadTextureV(m_width, m_height, m_textureFormat);
            m_initializationResult = CodecInitializationResult::Success;
        }

        bool SoftwareEncoder::CopyBuffer(void* frame)
        {
            m_device->CopyResourceFromNativeV(m_encodeTex, frame);
            return true;
        }

        bool SoftwareEncoder::EncodeFrame(int64_t timestamp_us)
        {
            const rtc::scoped_refptr<webrtc::I420Buffer> i420Buffer = m_device->ConvertRGBToI420(m_encodeTex);
            if (nullptr == i420Buffer)
                return false;

            // std::string filename = "D:\\MM\\record.yuv";
            // static FILE *fp = fopen(filename.c_str(), "wb+");
            // if (fp != NULL) {
            //    fwrite(i420Buffer->MutableDataY(), 1, i420Buffer->width() * i420Buffer->height(), fp);
            //    fwrite(i420Buffer->MutableDataU(), 1, i420Buffer->width() * i420Buffer->height() / 4, fp);
            //    fwrite(i420Buffer->MutableDataV(), 1, i420Buffer->width() * i420Buffer->height() / 4, fp);
            //    fflush(fp);
            // }

            webrtc::VideoFrame frame =
                webrtc::VideoFrame::Builder()
                .set_video_frame_buffer(i420Buffer)
                .set_rotation(webrtc::kVideoRotation_0)
                .set_timestamp_us(timestamp_us)
                .build();

            CaptureFrame(frame);
            m_frameCount++;
            return true;
        }

        bool SoftwareEncoder::EncodeFrame(int64_t timestamp_us, float* priorityArray)
        {
            // LG("EncodeFrame: before frame build: priorityArray: %p", priorityArray);

            const rtc::scoped_refptr<webrtc::I420Buffer> i420Buffer = m_device->ConvertRGBToI420(m_encodeTex);
            if (nullptr == i420Buffer)
                return false;

            webrtc::VideoFrame frame =
                webrtc::VideoFrame::Builder()
                .set_video_frame_buffer(i420Buffer)
                .set_rotation(webrtc::kVideoRotation_0)
                .set_timestamp_us(timestamp_us)
                .set_priority_array(priorityArray)
                .build();

            CaptureFrame(frame);
            m_frameCount++;
            return true;
        }
    } // end namespace webrtc
} // end namespace unity
