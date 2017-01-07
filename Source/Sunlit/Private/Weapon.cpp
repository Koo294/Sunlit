// Fill out your copyright notice in the Description page of Project Settings.

#include "Sunlit.h"
#include "Weapon.h"
#include "Ship.h"
#include "Projectile.h"

UWeapon::UWeapon()
{
	OnUpdateEnergyLevel.AddDynamic(this, &UWeapon::OnWeaponUpdateEnergyLevel);
	FireUsage = 10.f;
}

void UWeapon::Fire()
{
	IsFiring = true;
	CurveTime = 0;

	FTransform FireTransform;
	FVector InitialVelocity;
	UWorld* FireWorld;

	EnergySource->GetWeaponFireParameters(this, FireTransform, InitialVelocity, FireWorld);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = EnergySource;
	SpawnParams.Instigator = EnergySource->GetInstigator();

	AProjectile* NewProjectile = FireWorld->SpawnActor<AProjectile>(ProjectileType, FireTransform, SpawnParams);

	NewProjectile->Fire(InitialVelocity, FireTransform.Rotator(), ProjectileDispersion);
}

void UWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsFiring)
	{
		CurveTime += DeltaTime;

		float MinTime, MaxTime;

		if (!FireCurve)
		{
			UE_LOG(LogTemp, Warning, TEXT("Missing curves for Weapon"));
		}
		else
		{
			FireCurve->GetTimeRange(MinTime, MaxTime);

			if (CurveTime > MaxTime - MinTime)
			{
				IsFiring = false;
				if (IsTriggered && GetEnergyLevel() >= 1.f)
					Fire();
			}
			else
				EnergySource->ConsumeBattery(FireCurve->GetFloatValue(MinTime + CurveTime)*DeltaTime*FireUsage);
		}
	}
}

bool UWeapon::IsTickable() const
{
	return Super::IsTickable() || IsFiring;
}

void UWeapon::SetTriggered(bool Triggered)
{
	IsTriggered = Triggered;
	if (IsTriggered && !IsFiring && GetEnergyLevel() >= 1.f)
		Fire();
}

void UWeapon::OnWeaponUpdateEnergyLevel(float EnergyLevel)
{
	if (IsTriggered && !IsFiring && EnergyLevel >= 1.f)
		Fire();
}
