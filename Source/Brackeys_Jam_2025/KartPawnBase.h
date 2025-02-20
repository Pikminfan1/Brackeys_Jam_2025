// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Rewind/RewindComponent.h"
#include "KartPawnBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIsDeadChanged, bool, bNewIsDead);

UCLASS()
class BRACKEYS_JAM_2025_API AKartPawnBase : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AKartPawnBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rewind")
	TObjectPtr<URewindComponent> RewindComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsDead = false;

	UFUNCTION(BlueprintCallable)
	bool IsDead() const { return bIsDead; }

	UFUNCTION(BlueprintCallable)
	void SetIsDead(bool bNewIsDead);

	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Delegate to notify when IsDead state changes
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnIsDeadChanged OnIsDeadChanged;

};
