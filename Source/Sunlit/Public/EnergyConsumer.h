// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "Tickable.h"
#include "EnergyConsumer.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateEnergyLevel, float, EnergyLevel);

UCLASS()
class SUNLIT_API UEnergyConsumer : public UObject, public FTickableGameObject
{
	GENERATED_BODY()

private:

	bool IsActive;

	bool IsRamping;

	float CurveTime;

	class AShip* EnergySource;

	void SetActiveDirect(bool NewActive);

protected:

	virtual void Tick(float DeltaTime) override;

	virtual bool IsTickable() const override;

	virtual TStatId GetStatId() const override;
	
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = EnergyUse)
	UCurveFloat* ActivateCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = EnergyUse)
	float ActiveUsage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = EnergyUse)
	float ActiveRampThreshold;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = EnergyUse)
	UCurveFloat* DeactivateCurve;
	
	FUpdateEnergyLevel OnUpdateEnergyLevel;

	UEnergyConsumer();

	void SetActive(bool NewActive);

	void SetShip(class AShip* Ship);
};
