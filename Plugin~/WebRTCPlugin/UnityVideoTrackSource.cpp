#include "pch.h"
#include "UnityVideoTrackSource.h"
#include <chrono>
#include <ctime>

#include <mutex>

#include "Codec/IEncoder.h"

#define UnityVideoTrackSourceLog(...)       LogPrint("webrtc Log: UnityVideoTrackSource.cpp::" __VA_ARGS__)

namespace unity
{
namespace webrtc
{

UnityVideoTrackSource::UnityVideoTrackSource(
    bool is_screencast,
    absl::optional<bool> needs_denoising) :
    AdaptedVideoTrackSource(/*required_alignment=*/1),
    is_screencast_(is_screencast),
    needs_denoising_(needs_denoising),
    encoder_(nullptr)
{
//  DETACH_FROM_THREAD(thread_checker_);
}

UnityVideoTrackSource::UnityVideoTrackSource(
    bool is_screencast,
    absl::optional<bool> needs_denoising,
    const absl::optional<webrtc::VideoFrame::ObjectRange>& object_range) :
    AdaptedVideoTrackSource(/*required_alignment=*/1),
    is_screencast_(is_screencast),
    needs_denoising_(needs_denoising),
    object_range_(object_range),
    encoder_(nullptr)
{
//  DETACH_FROM_THREAD(thread_checker_);
}

UnityVideoTrackSource::~UnityVideoTrackSource()
{
    {
        std::unique_lock<std::mutex> lock(m_mutex);
    }
}

void UnityVideoTrackSource::Init(void* frame)
{
    frame_ = frame;
}

UnityVideoTrackSource::SourceState UnityVideoTrackSource::state() const
{
  // TODO(nisse): What's supposed to change this state?
  return MediaSourceInterface::SourceState::kLive;
}

bool UnityVideoTrackSource::remote() const {
  return false;
}

bool UnityVideoTrackSource::is_screencast() const {
  return is_screencast_;
}

absl::optional<bool> UnityVideoTrackSource::needs_denoising() const
{
    return needs_denoising_;
}

absl::optional<webrtc::VideoFrame::ObjectRange> UnityVideoTrackSource::object_range() const
{
    return object_range_;
}

uint32_t* UnityVideoTrackSource::priority_array() const
{
    return priority_array_;
}

void UnityVideoTrackSource::SetObjectRange(short iXStart, short iXEnd, short iYStart, short iYEnd, int iQpOffset)
{
    object_range_ = webrtc::VideoFrame::ObjectRange(iXStart, iXEnd, iYStart, iYEnd, iQpOffset);
}

void UnityVideoTrackSource::SetPriorityArray(uint32_t* priorityArray)
{
    priority_array_ = priorityArray;
}

CodecInitializationResult UnityVideoTrackSource::GetCodecInitializationResult() const
{
    if (encoder_ == nullptr)
    {
        return CodecInitializationResult::NotInitialized;
    }
    return encoder_->GetCodecInitializationResult();
}

void UnityVideoTrackSource::SetEncoder(IEncoder* encoder)
{
    encoder_ = encoder;
    encoder_->CaptureFrame.connect(
        this,
        &UnityVideoTrackSource::DelegateOnFrame);
}


void UnityVideoTrackSource::OnFrameCaptured(int64_t timestamp_us)
{
    // todo::(kazuki)
    // OnFrame(frame);
    std::unique_lock<std::mutex> lock(m_mutex, std::try_to_lock);
    if (!lock.owns_lock()) {
        return;
    }
    if (encoder_ == nullptr)
    {
        LogPrint("encoder is null");
        return;
    }
    // auto copy_start = std::chrono::system_clock::now();
    if (!encoder_->CopyBuffer(frame_))
    {
        LogPrint("Copy texture buffer is failed");
        return;
    }
    // auto copy_end = std::chrono::system_clock::now();
    // std::chrono::duration<double> copy_cost = copy_end - copy_start;
    // UnityVideoTrackSourceLog("CopyBuffer cost %lf", copy_cost);
    if (priority_array() == nullptr) {
        if (!encoder_->EncodeFrame(timestamp_us))
        {
            LogPrint("Encode frame is failed");
            return;
        }
    } else {
        if (!encoder_->EncodeFrame(timestamp_us, priority_array()))
        {
            LogPrint("Encode frame is failed");
            return;
        }
    }
    // auto encode_end = std::chrono::system_clock::now();
    // std::chrono::duration<double> encode_cost = encode_end - copy_end;
    // UnityVideoTrackSourceLog("EncodeFrame cost %lf", encode_cost);
}

} // end namespace webrtc
} // end namespace unity
