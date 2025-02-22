// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Killable.h"
#include "KillableActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIsDeadChangedKillableActor, bool, bNewIsDead);

UCLASS()
class BRACKEYS_JAM_2025_API AKillableActor : public AActor, public IKillable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKillableActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsDead = false;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Killable")
	bool IsDead() const;
	virtual bool IsDead_Implementation() const override { return bIsDead; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Killable")
	void SetIsDead(bool bNewIsDead);
	virtual void SetIsDead_Implementation(bool bNewIsDead) override;

	// Delegate to notify when IsDead state changes
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnIsDeadChangedKillableActor OnIsDeadChangedKillableActor;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
