// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AUROpenCV.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Runtime/Engine/Classes/Engine/Texture2D.h"

#include <thread>
#include <atomic>
#include <mutex>

#include "OpenCVPlayerComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNextVideoFrameDelegate);


UCLASS(ClassGroup = (OpenCV), meta = (BlueprintSpawnableComponent))
class OPENCVPLAYERPLUGIN_API UOpenCVPlayerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UOpenCVPlayerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Overridable function called whenever this actor is being removed from a level
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UPROPERTY(BlueprintAssignable, Category = "OpenCV")
		FNextVideoFrameDelegate OnNextVideoFrame;

	// Open video file or a capturing device or a IP video stream for video capturing
	// 1.Name of video file (eg. video.avi)
	// 2.Image sequence (eg. img_%02d.jpg, which will read samples like img_00.jpg, img_01.jpg, img_02.jpg, ...)
	// 3.URL of video stream (eg. protocol://host:port/script_name?script_params|auth)
	// 4.Camera_id + domain_offset (CAP_*) id of the video capturing device to open (default Camera_id == 0)
	UFUNCTION(BlueprintCallable, Category = "OpenCV")
		bool OpenVideoStream(FString VideoFileName);

	// Start video capturing stream
	UFUNCTION(BlueprintCallable, Category = "OpenCV")
		bool StartVideoStream();

	// Stop video capturing stream
	UFUNCTION(BlueprintCallable, Category = "OpenCV")
		void StopVideoStream();

	// Get the current video frame's corresponding texture
	UFUNCTION(BlueprintCallable, Category = "OpenCV")
		FVector2D GetFrameSize() const;

	// Get the current video frame's corresponding texture
	UFUNCTION(BlueprintCallable, Category = "OpenCV")
		UTexture2D* GetFrameTexture();

	// Returns true if video capturing has been initialized already
	UFUNCTION(BlueprintCallable, Category = "OpenCV")
		bool IsVideoStreamOpened() const;

	// Get the current video frame rate
	UFUNCTION(BlueprintCallable, Category = "OpenCV")
		float GetFrameRate() const;

	// Set the current video frame rate
	UFUNCTION(BlueprintCallable, Category = "OpenCV")
		void SetFrameRate(float FrameRate);

	// Get the number of frames in the video file
	UFUNCTION(BlueprintCallable, Category = "OpenCV")
		int32 GetFrameCount() const;

	// Get the 0-based index of the frame to be decoded/captured next
	UFUNCTION(BlueprintCallable, Category = "OpenCV")
		int32 GetFrameStartIndex() const;

	// Set the 0-based index of the frame to be decoded/captured next
	UFUNCTION(BlueprintCallable, Category = "OpenCV")
		void SetFrameStartIndex(int32 FrameStartIndex);

	// Number of frames in the video file to be decoded/captured
	UFUNCTION(BlueprintCallable, Category = "OpenCV")
		int32 GetFramesToPlay() const;

	// Number of frames in the video file to be decoded/captured
	UFUNCTION(BlueprintCallable, Category = "OpenCV")
		void SetFramesToPlay(int32 FramesToPlay);

	// Video file loop playback mode
	UFUNCTION(BlueprintCallable, Category = "OpenCV")
		bool GetLoopPlayback() const;

	// Video file loop playback mode
	UFUNCTION(BlueprintCallable, Category = "OpenCV")
		void SetLoopPlayback(bool LoopPlayback);

	// Get the 0-based index of the new frame
	UFUNCTION(BlueprintCallable, Category = "OpenCV")
		int32 GetNewFrameIndex() const;

	// Get the absolute path for the project content dir
	UFUNCTION(BlueprintCallable, Category = "OpenCV")
		FString GetProjectContentDir() const;

private:
	// The videos width and height (width, height)
	UPROPERTY()
		FVector2D m_vFrameSize;

	// The current video frame's corresponding texture
	UPROPERTY()
		UTexture2D* m_pFrameTexture;

	// The current data array
	UPROPERTY()
		TArray<FColor> m_aFrameData;

private:
	bool m_bIsVideoStreamOpened{ false };
	float m_fFrameRate{ 0 };
	int32 m_nFrameCount{ 0 };
	int32 m_nFrameStartIndex{ 0 };
	int32 m_nFramesToPlay{ 0 };
	bool m_bLoopPlayback{ true };

private:
	// Use this function to update the texture rects you want to change:
	// NOTE: There is a method called UpdateTextureRegions in UTexture2D but it is compiled WITH_EDITOR and is not marked as ENGINE_API so it cannot be linked
	// from plugins.
	// FROM: https://wiki.unrealengine.com/Dynamic_Textures
	void UpdateTextureRegions(UTexture2D* Texture, int32 MipIndex, uint32 NumRegions, FUpdateTextureRegion2D* Regions, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData);

	// Pointer to update texture region 2D struct
	FUpdateTextureRegion2D* VideoUpdateTextureRegion;

private:
	cv::Mat m_mFrame;
	cv::VideoCapture m_vcStream;

	std::thread m_StreamReaderThread;
	std::mutex m_mtxTexture;
	std::atomic<bool> m_bRunStreamReader{ false };
	std::atomic<bool> m_bNewFrame{ false };
	std::atomic<int32> m_nNewFrameIndex{ 0 };

private:
	void UpdateTexture();
	void StreamReader(double frameRate, int numFrames, bool loopPlayback, int positionInFrames);
};
