// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Runtime/Core/Public/Containers/RingBuffer.h"
//#include "RewindSnapshot.h"

#include "RewindComponent.generated.h"

class UCharacterMovementComponent;
class URewindVisualizationComponent;
class SkeletalMeshComponent;
class ARewindGameMode;
struct FTransformAndVelocitySnapshot;

//Declare Delegates for communication between actors using the Rewind Component

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimeManipulationStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeManipulationCompleted, float, TotalRewindTime);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRewindStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRewindCompleted, float, TotalRewindTime);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFastForwardStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFastForwardCompleted);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimeScrubStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimeScrubCompleted);
/*


*/
USTRUCT(BlueprintType)
struct FTransformAndVelocitySnapshot
{
	GENERATED_BODY();

	// Time since the last snapshot was recorded
	UPROPERTY(Transient, BlueprintReadWrite)
	float TimeSinceLastSnapshot = 0.0f;

	// Transform at time snapshot was recorded
	UPROPERTY(Transient, BlueprintReadWrite)
	FTransform Transform{ FVector::ZeroVector };

	// Linear velocity from the owner's root primitive component at time snapshot was recorded
	UPROPERTY(Transient, BlueprintReadWrite)
	FVector LinearVelocity = FVector::ZeroVector;

	// Angular velocity from the owner's root primitive component at time snapshot was recorded
	UPROPERTY(Transient, BlueprintReadWrite)
	FVector AngularVelocityInRadians = FVector::ZeroVector;
};




// State snapshots used when rewinding movement
USTRUCT(BlueprintType)
struct FMovementVelocityAndModeSnapshot
{
	GENERATED_BODY();

	// Time since the last snapshot was recorded
	UPROPERTY(Transient, BlueprintReadWrite)
	float TimeSinceLastSnapshot = 0.0f;

	// Movement velocity from the owner's movement component at time snapshot was recorded
	UPROPERTY(Transient, BlueprintReadWrite)
	FVector MovementVelocity = FVector::ZeroVector;

	// Movement mode from the owner's movement component at time snapshot was recorded
	UPROPERTY(Transient, BlueprintReadWrite)
	TEnumAsByte<enum EMovementMode> MovementMode = EMovementMode::MOVE_None;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BRACKEYS_JAM_2025_API URewindComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Rewind")
	float TotalRewindTime;
	// How often a snapshot should be recorded
	UPROPERTY(EditDefaultsOnly, Category = "Rewind")
	float SnapshotFrequencySeconds = 1.0f / 30.0f;

	// Whether actor requires movement component to be rewound
	UPROPERTY(EditDefaultsOnly, Category = "Rewind")
	bool bSnapshotMovementVelocityAndMode = false;

	// Whether actor should pause animations during time scrubbing
	UPROPERTY(EditDefaultsOnly, Category = "Rewind")
	bool bPauseAnimationDuringTimeScrubbing = false;


	// Called when the component begins time manipulation
	UPROPERTY(BlueprintAssignable, Category = "Rewind")
	FOnTimeManipulationStarted OnTimeManipulationStarted;

	// Called when the component stops time manipulation
	UPROPERTY(BlueprintAssignable, Category = "Rewind")
	FOnTimeManipulationCompleted OnTimeManipulationCompleted;

	// Called when the component begins rewinding
	UPROPERTY(BlueprintAssignable, Category = "Rewind")
	FOnRewindStarted OnRewindStarted;

	// Called when the component stops rewinding
	UPROPERTY(BlueprintAssignable, Category = "Rewind")
	FOnRewindCompleted OnRewindCompleted;

	// Called when the component begins fast forwarding
	UPROPERTY(BlueprintAssignable, Category = "Rewind")
	FOnRewindStarted OnFastForwardStarted;

	// Called when the component stops fast forwarding
	UPROPERTY(BlueprintAssignable, Category = "Rewind")
	FOnRewindCompleted OnFastForwardCompleted;

	// Called when the component begins rewinding
	UPROPERTY(BlueprintAssignable, Category = "Rewind")
	FOnTimeScrubStarted OnTimeScrubStarted;

	// Called when the component stops rewinding
	UPROPERTY(BlueprintAssignable, Category = "Rewind")
	FOnTimeScrubCompleted OnTimeScrubCompleted;
	/*
	UPROPERTY(BlueprintAssignable, Category = "Rewind")
	FOnSnapshotRecorded OnSnapshotRecorded;

	UPROPERTY(BlueprintAssignable, Category = "Rewind")
	FOnSnapshotApplied OnSnapshotApplied;*/



protected:
	//Whether the component is currently rewinding
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Rewind")
	bool bIsRewinding = false;

	// Whether the component is currently fast forwarding
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Rewind")
	bool bIsFastForwarding = false;

	// Whether the component is currently time scrubbing
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Rewind")
	bool bIsTimeScrubbing = false;

	// Whether the component is currently visualizing the timeline
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Rewind")
	bool bIsVisualizingTimeline = false;

	// Whether rewinding is currently enabled
	UPROPERTY(VisibleAnywhere, Category = "Rewind")
	bool bIsRewindingEnabled = true;

public:

	UFUNCTION(BlueprintCallable, Category = "Rewind")
	void ResetTotalRewindTime() { TotalRewindTime = 0.0f; }
	//Returns whether the component is currently rewinding
	UFUNCTION(BlueprintCallable, Category = "Rewind")
	bool IsRewinding() const { return bIsRewinding; };

	//Returns whether the component is currently fast forwarding
	UFUNCTION(BlueprintCallable, Category = "Rewind")
	bool IsFastForwarding() const { return bIsFastForwarding; };

	// Returns whether the component is currently time scrubbing
	UFUNCTION(BlueprintCallable, Category = "Rewind")
	bool IsTimeScrubbing() const { return bIsTimeScrubbing; };

	// Returns whether the component is currently doing any form of time manipulation
	UFUNCTION(BlueprintCallable, Category = "Rewind")
	bool IsTimeBeingManipulated() const { return bIsRewinding || bIsFastForwarding || bIsTimeScrubbing; };

	// Returns  the component is currently visualizing the timeline
	UFUNCTION(BlueprintCallable, Category = "Rewind")
	bool IsVisualizingTimeline() const { return bIsVisualizingTimeline; };

	// Returns whether rewinding is currently enabled
	UFUNCTION(BlueprintCallable, Category = "Rewind")
	bool IsRewindingEnabled() const { return bIsRewindingEnabled; }

	// Sets whether rewinding is currently enabled
	UFUNCTION(BlueprintCallable, Category = "Rewind")
	void SetIsRewindingEnabled(bool bEnabled);

public:	
	// Sets default values for this component's properties
	URewindComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	TArray<FTransformAndVelocitySnapshot> GetTransformAndVelocitySnapshots() const;
	//Time since the last snapshot was added or removed within the ring buffer
	UPROPERTY(Transient, VisibleAnywhere, Category = "Rewind|Debug")
	float TimeSinceSnapshotsChanged = 0.0f;

	UPROPERTY()
	TArray<FTransformAndVelocitySnapshot> TotalRace;

	UPROPERTY()
	TArray<FMovementVelocityAndModeSnapshot> TotalRaceAndMode;

	UFUNCTION(BlueprintCallable)
	TArray<FTransformAndVelocitySnapshot> GetTotalRace() const { return TotalRace; }


	UFUNCTION(BlueprintCallable)
	TArray<FMovementVelocityAndModeSnapshot> GetTotalRaceAndMode() const { return TotalRaceAndMode; }

private:
	//Buffer storing transform and velocity snapshots used for rewinding

	TRingBuffer<FTransformAndVelocitySnapshot> TransformAndVelocitySnapshots;

	//Buffer storing movement velocity and mode snapshots used for rewinding
	TRingBuffer<FMovementVelocityAndModeSnapshot> MovementVelocityAndModeSnapshots;

	//Max snapshots to store, computed in BeginPlay
	UPROPERTY(Transient, VisibleAnywhere, Category = "Rewind|Debug")
	uint32 MaxSnapshots = 1;
		

	UPROPERTY(Transient, VisibleAnywhere, Category = "Rewind|Debug")
	int32 LatestSnapshotIndex = -1;

	//Root primitive component of the owner, if said component exists
	UPROPERTY(Transient, VisibleAnywhere, Category = "Rewind|Debug")
	UPrimitiveComponent* OwnerRootComponent;

	//Movement component of the owner, if said component exists
	UPROPERTY(Transient, VisibleAnywhere, Category = "Rewind|Debug")
	UCharacterMovementComponent* OwnerMovementComponent;

	//Skeletal mesh component of the owner, if said component exists
	UPROPERTY(Transient, VisibleAnywhere, Category = "Rewind|Debug")
	USkeletalMeshComponent* OwnerSkeletalMesh;

	//Rewind visualization component, if said component exists
	UPROPERTY(Transient, VisibleAnywhere, Category = "Rewind|Debug")
	URewindVisualizationComponent* OwnerVisualizationComponent;

	//Bool for whether time maniuplation has pasued physics
	UPROPERTY(Transient, VisibleAnywhere, Category = "Rewind|Debug")
	bool bPausedPhysics = false;

	//Bool for whether time maniuplation has pasued animation
	UPROPERTY(Transient, VisibleAnywhere, Category = "Rewind|Debug")
	bool bPausedAnimation = false;

	//Bool for whether animation was paused when the current time manipulation operation started
	UPROPERTY(Transient, VisibleAnywhere, Category = "Rewind|Debug")
	bool bAnimationsPausedAtStartOfTimeManipulation = false;

	//Bool for whether the last time maniplation was rewind or a fast forward, used by time scrubbing
	//to determind interpolation direction
	UPROPERTY(Transient, VisibleAnywhere, Category = "Rewind|Debug")
	bool bLastTimeManipulationWasRewind = true;

	//Game mode for global rewind state
	ARewindGameMode* GameMode;

private:
	//Event Handlers


	UFUNCTION()
	void HandleOnSnapshotRecorded();

	UFUNCTION()
	void HandleOnSnapshotApplied();
	//Called when rewinding starts
	UFUNCTION()
	void OnGlobalRewindStarted();

	//Called when fast forwarding starts
	UFUNCTION()
	void OnGlobalFastForwardStarted();

	//Called wehen time scrubbing starts
	UFUNCTION()
	void OnGlobalTimeScrubStarted();

	//called when rewinding completes
	UFUNCTION()
	void OnGlobalRewindCompleted();

	//Called when fast forwarding completes
	UFUNCTION()
	void OnGlobalFastForwardCompleted();

	//Called when time scrubbing completes
	UFUNCTION()
	void OnGlobalTimeScrubCompleted();

	//Called when time scrubbing visualization is enabled
	UFUNCTION()
	void OnGlobalTimelineVisualizationEnabled();

	//Called when time scrubbing visualization is disabled
	UFUNCTION()
	void OnGlobalTimelineVisualizationDisabled();
	
private:
	//Functions

	//Initializes ring buffers and computes required space
	void InitializeRingBuffers(float MaxRewindSeconds);

	//Stores a snapshot in the ring buffer
	void RecordSnapshot(float DeltaTime);

	//Erases all future snapshots after the most recent snapshot
	void EraseFutureSnapshots();

	//Plays back or forth thrugh time using the snapshots in the ring buffer
	//And the given rewind bool
	UFUNCTION(BlueprintCallable)
	void PlaySnapshots(float DeltaTime, bool bRewinding);

	UFUNCTION(BlueprintCallable)
	void PauseTime(float DeltaTime, bool bRewinding);

	//Helper Function to start a time manipulation operation
	UFUNCTION(BlueprintCallable)
	bool TryStartTimeManipulation(bool& bStateToSet, bool bResetTimeSinceSnapshotsChanged);

	// Helper to stop a time manipulation operation
	UFUNCTION(BlueprintCallable)
	bool TryStopTimeManipulation(bool& bStateToSet, bool bResetTimeSinceSnapshotsChanged, bool bResetMovementVelocity);


	//Disables physics
	UFUNCTION(BlueprintCallable)
	void PausePhysics();

	//Recreates physics and movement state
	//Update this to ignore velocity
	UFUNCTION(BlueprintCallable)
	void UnpausePhysics();

	//Disables animation
	UFUNCTION(BlueprintCallable)
	void PauseAnimation();

	//Resume animation
	UFUNCTION(BlueprintCallable)
	void UnpauseAnimation();

	//Helper function for PlaySnapshots and PauseTime that handles cases where there are an insufficient number of snapshots
	//For interpolation
	UFUNCTION(BlueprintCallable)
	bool HandleInsufficientSnapshots();

	//Interpolates between the two latest snapshots and applies the result to the owner
	UFUNCTION(BlueprintCallable)
	void InterpolateAndApplySnapshots(bool bRewinding);

	//Blends between two transform and velocity snapshots
	UFUNCTION(BlueprintCallable)
	FTransformAndVelocitySnapshot BlendSnapshots(
		const FTransformAndVelocitySnapshot& A,
		const FTransformAndVelocitySnapshot& B,
		float Alpha);

	//Blends between two movement velocity and movement mode snapshots
	UFUNCTION(BlueprintCallable)
	FMovementVelocityAndModeSnapshot BlendSnapshotsMode(
		const FMovementVelocityAndModeSnapshot& A,
		const FMovementVelocityAndModeSnapshot& B,
		float Alpha);

	//Applies the provided transform and velocity snapshot to the owner
	UFUNCTION(BlueprintCallable)
	void ApplySnapshot(const FTransformAndVelocitySnapshot& Snapshot, bool bApplyPhysics);

	//Applies the provided movement velocity and mode snapshot to the owner
	UFUNCTION(BlueprintCallable)
	void ApplySnapshotMode(const FMovementVelocityAndModeSnapshot& Snapshot, bool bApplyTimeDilationVelocity);

	//Debug helper to draw snapshots when Rewind.VisualizeSnapshots 1 is set
	void VisualizeTimeline();

};
