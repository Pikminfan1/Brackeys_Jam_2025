// Fill out your copyright notice in the Description page of Project Settings.


#include "RewindVisualizationComponent.h"

#include "Containers/RingBuffer.h"
#include "Engine/World.h"
#include "RewindComponent.h"

URewindVisualizationComponent::URewindVisualizationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URewindVisualizationComponent::BeginPlay()
{
	Super::BeginPlay();

	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

	//Assign Random color for debug visualization
	DebugColor = FColor::MakeRandomColor();
	SetVectorParameterValueOnMaterials(TEXT("Color"), FVector(DebugColor));
}

void URewindVisualizationComponent::ClearInstances()
{
	Super::ClearInstances();
	LastUpdateTime = 0.0f;
}

void URewindVisualizationComponent::SetInstancesFromSnapshots(const TRingBuffer<FTransformAndVelocitySnapshot>& Snapshots)
{
	//Skip update if there are no snapshots
	int CurrentInstanceCount = GetInstanceCount();
	if (Snapshots.Num() == 0)
	{
		if (CurrentInstanceCount > 0) { Super::ClearInstances(); }
		return;
	}

	//Skip update if less than SecondsPerMesh time has passed
	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastUpdateTime < SecondsPerMesh) { return; }
	LastUpdateTime = CurrentTime;

	//Computes transforms for our instances, common case will be adding 0 -1 instances
	TArray<FTransform> InstanceTransforms;
	InstanceTransforms.Reserve(CurrentInstanceCount + 1);

	//Sample snapshots to compute instance transforms
	float AccruedTime = 0.0f;
	for (int i = 0; i < Snapshots.Num(); ++i)
	{
		bool bIsFirstOrLastSnapshot = (i == 0) || (i == Snapshots.Num() - 1);

		//Accrue time until SecondsPerMesh time has passed, always keep the first and last snapshot
		const FTransformAndVelocitySnapshot& Snapshot = Snapshots[i];
		AccruedTime += Snapshot.TimeSinceLastSnapshot;
		if(AccruedTime < SecondsPerMesh && !bIsFirstOrLastSnapshot) { continue; }
		AccruedTime = 0.0f;

		//Add instance if the location is meaningfully different from the last snapshot
		bool bAddSnapshot = bIsFirstOrLastSnapshot;
		if (!bAddSnapshot)
		{
			//Optimization skips expensive dist check using FVector::Dist 
			constexpr double Threshold = 30.0f * 30.0f;
			double DistSquared = FVector::DistSquared(InstanceTransforms.Last().GetLocation(), Snapshot.Transform.GetLocation());
			bAddSnapshot = DistSquared > Threshold;
		}
		if (bAddSnapshot) { InstanceTransforms.Add(Snapshot.Transform); }

		//Adjust rotation of the transforms so that each meshs forward vector will point at the next mesh
		if (InstanceTransforms.Num() >= 2)
		{
			for(i = 0; i < InstanceTransforms.Num() - 1; ++i)
			{
				FTransform& Source = InstanceTransforms[i];
				FTransform& Target = InstanceTransforms[i + 1];
				FQuat LookAtRotation = FRotationMatrix::MakeFromX((Target.GetLocation() - Source.GetLocation())).ToQuat();
				Source.SetRotation(LookAtRotation);
			}
		}

		//Skip past any instances that already have correct transforms
		int Index = 0;
		for (; Index < InstanceTransforms.Num() && Index < CurrentInstanceCount; ++Index)
		{
			FTransform CurrentInstanceTransform;
			bool bResult = GetInstanceTransform(Index, CurrentInstanceTransform, true /*bWorldSpace*/);
			check(bResult);
			if (!CurrentInstanceTransform.Equals(InstanceTransforms[Index])) { break; }
		}

		// Update existing transforms or add the new instance
		bool bUpdatedAnyInstances = false;
		for (; Index < InstanceTransforms.Num(); ++Index)
		{
			if (Index < CurrentInstanceCount)
			{
				constexpr bool bWorldSpace = true;
				constexpr bool bMarkRenderStateDirty = false;
				constexpr bool bTeleport = true;
				bool bResult = UpdateInstanceTransform(Index, InstanceTransforms[Index], bWorldSpace, bMarkRenderStateDirty, bTeleport);
				check(bResult);
				bUpdatedAnyInstances = true;
			}
			else
			{
				constexpr bool bWorldSpace = true;
				AddInstance(InstanceTransforms[Index], bWorldSpace);
			}
		}

		// Mark the render state dirty after updating all instances
		if (bUpdatedAnyInstances) { MarkRenderStateDirty(); }

		// Remove any extra existing instances
		for (; Index < CurrentInstanceCount; ++Index)
		{
			bool bResult = RemoveInstance(Index);
			check(bResult);
		}
	}
}

