// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Rewind/RewindComponent.h"
#include "Killable.h"
#include "KartPawnBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIsDeadChanged, bool, bNewIsDead);

UCLASS()
class BRACKEYS_JAM_2025_API AKartPawnBase : public APawn, public IKillable
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

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Killable")
	bool IsDead() const;
	virtual bool IsDead_Implementation() const override { return bIsDead; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Killable")
	void SetIsDead(bool bNewIsDead);
	virtual void SetIsDead_Implementation(bool bNewIsDead) override;
	
	virtual void Tick(float DeltaTime) override;



	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Delegate to notify when IsDead state changes
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnIsDeadChanged OnIsDeadChanged;

};
