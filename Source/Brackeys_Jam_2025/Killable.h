// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Killable.generated.h"

/**
 * 
 */
UINTERFACE(MinimalAPI,Blueprintable)
class UKillable : public UInterface
{
	GENERATED_BODY()
};

class IKillable
{
	GENERATED_BODY()

public:
	//bool bIsDead;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Killable")
	bool IsDead() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Killable")
	void SetIsDead(bool bNewIsDead);
};
