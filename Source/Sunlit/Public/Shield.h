// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnergyConsumer.h"
#include "Shield.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class SUNLIT_API UShield : public UEnergyConsumer
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Shield)
	UMaterialInterface* ShieldMat;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Shield)
	float BlockThreshold;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Shield)
	float BlockEnergyCost;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Shield)
	float BlockEnergyMult;
};
