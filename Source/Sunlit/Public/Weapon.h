// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnergyConsumer.h"
#include "Weapon.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class SUNLIT_API UWeapon : public UEnergyConsumer
{
	GENERATED_BODY()
	
private:

	bool IsFiring;

	bool IsTriggered;

	float CurveTime;

	void Fire();

	UFUNCTION()
	void OnWeaponUpdateEnergyLevel(float EnergyLevel);

protected:

	virtual void Tick(float DeltaTime) override;

	virtual bool IsTickable() const override;

public:

	UWeapon();

	void SetTriggered(bool Triggered);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = EnergyUse)
	float FireUsage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = EnergyUse)
	UCurveFloat* FireCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Projectile)
	TSubclassOf<class AProjectile> ProjectileType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Projectile)
	float ProjectileDispersion;
};
