// Fill out your copyright notice in the Description page of Project Settings.

#include "Sunlit.h"
#include "Weapon.h"
#include "Shield.h"
#include "Ship.h"


// Sets default values
AShip::AShip()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ShipMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShipMesh"));
	RootComponent = ShipMesh;
	ShipMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	ShieldEMainMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldEMainMesh"));
	ShieldEMainMesh->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false), "hull");
	ShieldEMainMesh->SetRelativeRotation(FRotator(0.f, 0.f, -90.f));

	ShieldKMainMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldKMainMesh"));
	ShieldKMainMesh->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false), "hull");
	ShieldKMainMesh->SetRelativeRotation(FRotator(0.f, 0.f, -90.f));

}

void AShip::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AShip::ConsumeBattery(float ConsumeCharge)
{
	if (ConsumeCharge <= 0.f) return;

	BatteryPercent -= ConsumeCharge / BatteryCapacity;
	if (BatteryPercent < 0.f)
		BatteryPercent = 0.f;
}

float AShip::GetBatteryCharge()
{
	return BatteryPercent*BatteryCapacity;
}

// Called when the game starts or when spawned
void AShip::BeginPlay()
{
	Super::BeginPlay();

	ShipMesh->SetSimulatePhysics(true);
	ShipMesh->SetEnableGravity(false);

	//Construct loadout

	if (WeaponEMainType)
	{
		WeaponEMain = NewObject<UWeapon>(this, WeaponEMainType);
		WeaponEMain->SetShip(this);
		WeaponEMain->OnUpdateEnergyLevel.AddDynamic(this, &AShip::OnWeaponEUpdateEnergyLevel);
	}
	if (WeaponKMainType)
	{
		WeaponKMain = NewObject<UWeapon>(this, WeaponKMainType);
		WeaponKMain->SetShip(this);
		WeaponKMain->OnUpdateEnergyLevel.AddDynamic(this, &AShip::OnWeaponKUpdateEnergyLevel);
	}
	if (ShieldEMainType)
	{
		ShieldEMain = NewObject<UShield>(this, ShieldEMainType);
		ShieldEMain->SetShip(this);
		ShieldEMain->OnUpdateEnergyLevel.AddDynamic(this, &AShip::OnShieldEUpdateEnergyLevel);
	}
	if (ShieldKMainType)
	{
		ShieldKMain = NewObject<UShield>(this, ShieldKMainType);
		ShieldKMain->SetShip(this);
		ShieldKMain->OnUpdateEnergyLevel.AddDynamic(this, &AShip::OnShieldKUpdateEnergyLevel);
	}

	//Dynamic mats

	TArray<UMaterialInterface*> ShipMaterials = ShipMesh->GetMaterials();
	for (int32 i = 0; i < MaterialIndexMain.Num(); i++)
		if (MaterialIndexMain[i] < ShipMaterials.Num())
		{
			UMaterialInstanceDynamic* DynamicShipMaterial = UMaterialInstanceDynamic::Create(ShipMaterials[MaterialIndexMain[i]], this);
			ShipMesh->SetMaterial(MaterialIndexMain[i], DynamicShipMaterial);
			MaterialsMain.Add(DynamicShipMaterial);
		}

	if (ShieldEMain)
	{
		ShieldEMainMat = UMaterialInstanceDynamic::Create(ShieldEMain->ShieldMat, this);
		ShieldEMainMesh->SetMaterial(0, ShieldEMainMat);
	}
	if (ShieldKMain)
	{
		ShieldKMainMat = UMaterialInstanceDynamic::Create(ShieldKMain->ShieldMat, this);
		ShieldKMainMesh->SetMaterial(0, ShieldKMainMat);
	}
}

// Called every frame
void AShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ShipMesh->AddForce(FVector(600.f, 0.f, 0.f));

	if (BatteryPercent < 1.f && BatteryEfficiency)
	{
		BatteryPercent += DeltaTime*BatteryEfficiency->GetFloatValue(BatteryPercent)*BatteryChargeRate / BatteryCapacity;
		if (BatteryPercent > 1.f)
			BatteryPercent = 1.f;
	}
}

void AShip::SetWeaponEActive(bool Active)
{
	if (WeaponEMain)
		WeaponEMain->SetActive(Active);
}

void AShip::SetWeaponKActive(bool Active)
{
	if (WeaponKMain)
		WeaponKMain->SetActive(Active);
}

void AShip::SetShieldEActive(bool Active)
{
	if (ShieldEMain)
		ShieldEMain->SetActive(Active);
}


void AShip::SetShieldKActive(bool Active)
{
	if (ShieldKMain)
		ShieldKMain->SetActive(Active);
}

void AShip::OnWeaponEUpdateEnergyLevel(float EnergyLevel)
{
	for (int32 i = 0; i < MaterialsMain.Num(); i++)
		MaterialsMain[i]->SetScalarParameterValue("GunEGlowLevel", EnergyLevel);
}

void AShip::OnWeaponKUpdateEnergyLevel(float EnergyLevel)
{
	for (int32 i = 0; i < MaterialsMain.Num(); i++)
		MaterialsMain[i]->SetScalarParameterValue("GunKGlowLevel", EnergyLevel);
}

void AShip::OnShieldEUpdateEnergyLevel(float EnergyLevel)
{
	for (int32 i = 0; i < MaterialsMain.Num(); i++)
		MaterialsMain[i]->SetScalarParameterValue("ShieldEGlowLevel", EnergyLevel);

	if (ShieldEMainMat)
	{
		float ShieldGlowLevel = EnergyLevel;
		ShieldGlowLevel = (ShieldGlowLevel - ShieldEMain->BlockThreshold) / (1 - ShieldEMain->BlockThreshold);
		ShieldGlowLevel = FMath::Max(ShieldGlowLevel, 0.f);

		ShieldEMainMat->SetScalarParameterValue("Power", ShieldGlowLevel);
	}
}

void AShip::OnShieldKUpdateEnergyLevel(float EnergyLevel)
{
	for (int32 i = 0; i < MaterialsMain.Num(); i++)
		MaterialsMain[i]->SetScalarParameterValue("ShieldKGlowLevel", EnergyLevel);

	if (ShieldKMainMat)
	{
		float ShieldGlowLevel = EnergyLevel;
		ShieldGlowLevel = (ShieldGlowLevel - ShieldKMain->BlockThreshold) / (1 - ShieldKMain->BlockThreshold);
		ShieldGlowLevel = FMath::Max(ShieldGlowLevel, 0.f);

		ShieldKMainMat->SetScalarParameterValue("Power", ShieldGlowLevel);
	}
}
