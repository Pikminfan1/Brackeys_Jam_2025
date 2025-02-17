// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Runtime/Core/Public/Containers/RingBuffer.h"

#include "RewindVisualizationComponent.generated.h"

struct FTransformAndVelocitySnapshot;

/**
 * Draws a static mesh instance for each snapshot on rewind timeline.
 */
UCLASS()
class BRACKEYS_JAM_2025_API URewindVisualizationComponent : public UInstancedStaticMeshComponent
{
	GENERATED_BODY()

public:
	//Sets default values for this component's properties
	URewindVisualizationComponent();
public:
	//clear all instances
	virtual void ClearInstances() override;

	//Assigns a static mesh to each transform in snapshots
	void SetInstancesFromSnapshots(const TRingBuffer<FTransformAndVelocitySnapshot>& Snapshots);

protected:
	//Called when the game starts
	virtual void BeginPlay() override;

public:
	//Time in seconds between each visualized mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rewind|Visualization")
	float SecondsPerMesh = 1.0f / 30.0f;

private:
	// Color used for this actor in debug visualization when Rewind.VisualizeSnapshots 1 is set
	UPROPERTY(Transient, VisibleAnywhere, Category = "Rewind|Debug")
	FColor DebugColor = FColor::Red;

	UPROPERTY(Transient, VisibleAnywhere, Category = "Rewind|Debug")
	float LastUpdateTime = 0.0f;
};
