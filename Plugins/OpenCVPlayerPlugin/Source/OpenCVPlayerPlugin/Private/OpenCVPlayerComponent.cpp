// Fill out your copyright notice in the Description page of Project Settings.

#include "OpenCVPlayerPlugin.h"

#include "OpenCVPlayerComponent.h"

#include <thread>
#include <chrono>
#include <iostream>
#include <cmath>
#include <limits>


// Sets default values for this component's properties
UOpenCVPlayerComponent::UOpenCVPlayerComponent() :
	m_vFrameSize(0, 0), m_pFrameTexture(nullptr)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UOpenCVPlayerComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UOpenCVPlayerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (m_bNewFrame)
	{
		OnNextVideoFrame.Broadcast();
	}
}


// Overridable function called whenever this actor is being removed from a level
void UOpenCVPlayerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	StopVideoStream();
}


// Open video file or a capturing device or a IP video stream for video capturing
bool UOpenCVPlayerComponent::OpenVideoStream(FString VideoFileName)
{
	StopVideoStream();

	VideoFileName.TrimStartAndEndInline();
	if (VideoFileName.IsEmpty())
	{
		return false;
	}
	if (VideoFileName.IsNumeric())
	{
		m_vcStream.open(FCString::Atoi(*VideoFileName));
	}
	else
	{
		m_vcStream.open(TCHAR_TO_ANSI(*VideoFileName));
	}
	m_bIsVideoStreamOpened = m_vcStream.isOpened();
	if (!m_bIsVideoStreamOpened)
	{
		return false;
	}
	if (m_fFrameRate == 0)
	{
		m_fFrameRate = m_vcStream.get(cv::CAP_PROP_FPS);
	}
	m_nFrameCount = m_vcStream.get(cv::CAP_PROP_FRAME_COUNT);
	m_vFrameSize = FVector2D(m_vcStream.get(cv::CAP_PROP_FRAME_WIDTH), m_vcStream.get(cv::CAP_PROP_FRAME_HEIGHT));
	m_pFrameTexture = UTexture2D::CreateTransient(m_vFrameSize.X, m_vFrameSize.Y);
	m_pFrameTexture->UpdateResource();
	VideoUpdateTextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, m_vFrameSize.X, m_vFrameSize.Y);
	m_aFrameData.Init(FColor(0, 0, 0, 255), m_vFrameSize.X * m_vFrameSize.Y);
	return true;
}


// Start video capturing stream
bool UOpenCVPlayerComponent::StartVideoStream()
{
	if (m_bRunStreamReader)
	{
		return true;
	}
	m_bIsVideoStreamOpened = m_vcStream.isOpened();
	if (!m_bIsVideoStreamOpened)
	{
		return false;
	}
	m_vcStream.set(cv::CAP_PROP_POS_FRAMES, m_nFrameStartIndex);
	m_nNewFrameIndex = m_nFrameStartIndex;
	m_bRunStreamReader = true;
	m_StreamReaderThread = std::thread(&UOpenCVPlayerComponent::StreamReader, this,
		m_fFrameRate, m_nFramesToPlay, m_bLoopPlayback, m_nFrameStartIndex);
	return true;
}


// Stop video capturing stream
void UOpenCVPlayerComponent::StopVideoStream()
{
	if (m_bRunStreamReader)
	{
		m_bRunStreamReader = false;
		if (m_StreamReaderThread.joinable())
		{
			m_StreamReaderThread.join();
		}
	}
}


void UOpenCVPlayerComponent::UpdateTexture()
{
	if (m_vcStream.isOpened() && !m_mFrame.empty() && m_mFrame.data && m_mFrame.isContinuous())
	{
		int j = 0;
		for (int i = 0; i < m_mFrame.total(); ++i)
		{
			m_aFrameData[i].B = m_mFrame.data[j++];
			m_aFrameData[i].G = m_mFrame.data[j++];
			m_aFrameData[i].R = m_mFrame.data[j++];
		}
	}
}


void UOpenCVPlayerComponent::UpdateTextureRegions(UTexture2D* Texture, int32 MipIndex, uint32 NumRegions, FUpdateTextureRegion2D* Regions, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData)
{
	if (Texture->Resource)
	{
		struct FUpdateTextureRegionsData
		{
			FTexture2DResource* Texture2DResource;
			int32 MipIndex;
			uint32 NumRegions;
			FUpdateTextureRegion2D* Regions;
			uint32 SrcPitch;
			uint32 SrcBpp;
			uint8* SrcData;
		};

		FUpdateTextureRegionsData* RegionData = new FUpdateTextureRegionsData;

		RegionData->Texture2DResource = (FTexture2DResource*)Texture->Resource;
		RegionData->MipIndex = MipIndex;
		RegionData->NumRegions = NumRegions;
		RegionData->Regions = Regions;
		RegionData->SrcPitch = SrcPitch;
		RegionData->SrcBpp = SrcBpp;
		RegionData->SrcData = SrcData;

		ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
			UpdateTextureRegionsData,
			FUpdateTextureRegionsData*, RegionData, RegionData,
			bool, bFreeData, bFreeData,
			{
				for (uint32 RegionIndex = 0; RegionIndex < RegionData->NumRegions; ++RegionIndex)
				{
					int32 CurrentFirstMip = RegionData->Texture2DResource->GetCurrentFirstMip();
					if (RegionData->MipIndex >= CurrentFirstMip)
					{
						RHIUpdateTexture2D(
							RegionData->Texture2DResource->GetTexture2DRHI(),
							RegionData->MipIndex - CurrentFirstMip,
							RegionData->Regions[RegionIndex],
							RegionData->SrcPitch,
							RegionData->SrcData
							+ RegionData->Regions[RegionIndex].SrcY * RegionData->SrcPitch
							+ RegionData->Regions[RegionIndex].SrcX * RegionData->SrcBpp
						);
					}
				}
				if (bFreeData)
				{
					FMemory::Free(RegionData->Regions);
					FMemory::Free(RegionData->SrcData);
				}
				delete RegionData;
			}
		);
	}
}


void UOpenCVPlayerComponent::StreamReader(double frameRate, int numFrames, bool loopPlayback, int positionInFrames)
{
	int nf = numFrames;
	double msSleep = (frameRate != 0) ? std::round(1000.0 / frameRate) : 0;
	msSleep = (msSleep > std::numeric_limits<int>::max()) ? std::numeric_limits<int>::max() : msSleep;
	auto step = std::chrono::milliseconds(int(msSleep));
	auto start = std::chrono::high_resolution_clock::now();
	while (m_bRunStreamReader)
	{
		if (m_vcStream.read(m_mFrame))
		{
			do
			{
				std::lock_guard<std::mutex> guard(m_mtxTexture);
				UpdateTexture();
			} while (false);
			m_bNewFrame = true;
			if (frameRate != 0 && nf != 1)
			{
				nf = (nf == 0) ? 0 : nf - 1;
				++m_nNewFrameIndex;
				if (loopPlayback && nf == 1)
				{
					nf = numFrames;
					m_vcStream.set(cv::CAP_PROP_POS_FRAMES, positionInFrames);
					m_nNewFrameIndex = positionInFrames;
				}
				start += step;
				std::this_thread::sleep_until(start);
			}
			else
			{
				m_bRunStreamReader = false;
			}
		}
		else if (loopPlayback && (numFrames == 0 || nf != numFrames))
		{
			nf = numFrames;
			m_vcStream.set(cv::CAP_PROP_POS_FRAMES, positionInFrames);
			m_nNewFrameIndex = positionInFrames;
		}
		else
		{
			m_bRunStreamReader = false;
		}
	}
}


FVector2D UOpenCVPlayerComponent::GetFrameSize() const
{
	return m_vFrameSize;
}


// Get the current video frame's corresponding texture
UTexture2D* UOpenCVPlayerComponent::GetFrameTexture()
{
	if (!m_bNewFrame)
	{
		return m_pFrameTexture;
	}
	do
	{
		std::lock_guard<std::mutex> guard(m_mtxTexture);
		UpdateTextureRegions(m_pFrameTexture, (int32)0, (uint32)1, VideoUpdateTextureRegion, (uint32)(4 * m_vFrameSize.X), (uint32)4, (uint8*)m_aFrameData.GetData(), false);
	} while (false);
	m_bNewFrame = false;
	return m_pFrameTexture;
}


// Get the video stream status
bool UOpenCVPlayerComponent::IsVideoStreamOpened() const
{
	return m_bIsVideoStreamOpened;
}


float UOpenCVPlayerComponent::GetFrameRate() const
{
	return m_fFrameRate;
}


void UOpenCVPlayerComponent::SetFrameRate(float FrameRate)
{
	m_fFrameRate = std::max<float>(0, FrameRate);
}


int32 UOpenCVPlayerComponent::GetFrameCount() const
{
	return m_nFrameCount;
}


int32 UOpenCVPlayerComponent::GetFrameStartIndex() const
{
	return m_nFrameStartIndex;
}


void UOpenCVPlayerComponent::SetFrameStartIndex(int32 FrameStartIndex)
{
	m_nFrameStartIndex = std::max<int32>(0, FrameStartIndex);
}


int32 UOpenCVPlayerComponent::GetFramesToPlay() const
{
	return m_nFramesToPlay;
}


void UOpenCVPlayerComponent::SetFramesToPlay(int32 FramesToPlay)
{
	m_nFramesToPlay = std::max<int32>(0, FramesToPlay);
}


bool UOpenCVPlayerComponent::GetLoopPlayback() const
{
	return m_bLoopPlayback;
}


void UOpenCVPlayerComponent::SetLoopPlayback(bool LoopPlayback)
{
	m_bLoopPlayback = LoopPlayback;
}


int32 UOpenCVPlayerComponent::GetNewFrameIndex() const
{
	return m_nNewFrameIndex;
}


FString UOpenCVPlayerComponent::GetProjectContentDir() const
{
	return FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir());
}
