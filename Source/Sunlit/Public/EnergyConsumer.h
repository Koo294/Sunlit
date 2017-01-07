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

	bool Reactive;

	float CurveTime;

	void SetActiveDirect(bool NewActive);

	float EnergyLevel;

	void UpdateEnergyLevel(float NewLevel);

protected:

	class AShip* EnergySource;

	virtual void Tick(float DeltaTime) override;

	virtual bool IsTickable() const override;

	virtual TStatId GetStatId() const override;
	
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = EnergyUse)
	UCurveFloat* ActivateCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = EnergyUse)
	UCurveFloat* ReactivateCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = EnergyUse)
	float ActiveUsage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = EnergyUse)
	float ReactiveThreshold;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = EnergyUse)
	UCurveFloat* DeactivateCurve;
	
	FUpdateEnergyLevel OnUpdateEnergyLevel;

	UEnergyConsumer();

	void SetActive(bool NewActive);

	void SetShip(class AShip* Ship);

	float GetEnergyLevel();

	bool GetActive();
};
