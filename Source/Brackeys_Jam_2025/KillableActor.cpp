// Fill out your copyright notice in the Description page of Project Settings.


#include "KillableActor.h"

// Sets default values
AKillableActor::AKillableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AKillableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AKillableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AKillableActor::SetIsDead_Implementation(bool bNewIsDead)
{
	if (bIsDead != bNewIsDead)
	{
		bIsDead = bNewIsDead;
		OnIsDeadChangedKillableActor.Broadcast(bNewIsDead);
	}
}

