// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnergyConsumer.h"
#include "Thruster.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class SUNLIT_API UThruster : public UEnergyConsumer
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Thruster)
	float ThrustThreshold;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Thruster)
	float ThrustMax;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Thruster)
	float ThrustMaxEnergyUse;
};
