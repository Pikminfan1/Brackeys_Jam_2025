// Fill out your copyright notice in the Description page of Project Settings.


#include "KartPawnBase.h"

// Sets default values
AKartPawnBase::AKartPawnBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RewindComponent = CreateDefaultSubobject<URewindComponent>(TEXT("RewindComponent"));
}

// Called when the game starts or when spawned
void AKartPawnBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AKartPawnBase::SetIsDead(bool bNewIsDead)
{
	if (bIsDead != bNewIsDead)
	{
		bIsDead = bNewIsDead;
		OnIsDeadChanged.Broadcast(bNewIsDead);
	}
}

// Called every frame
void AKartPawnBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AKartPawnBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

