// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "RewindableStaticMeshActor.generated.h"

class URewindComponent;
class URewindVisualizationComponent;

/**
 * Static Mesh Actor that supports time rewinding
 */
UCLASS()
class BRACKEYS_JAM_2025_API ARewindableStaticMeshActor : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	//Component that manages rewinding
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rewind")
	URewindComponent* RewindComponent;

	//Component that visualizes the rewind
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rewind")
	URewindVisualizationComponent* RewindVisualizationComponent;

	ARewindableStaticMeshActor();
	
};
