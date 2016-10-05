// Fill out your copyright notice in the Description page of Project Settings.

#include "Sunlit.h"
#include "Weapon.h"
#include "Shield.h"
#include "Thruster.h"
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

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false), "hull");
	Camera->SetRelativeRotation(FRotator(0.f, 0.f, -90.f));

	MaxYawSpeed = 90.f;
	MaxPitchSpeed = 90.f;
	MaxRollSpeed = 90.f;
	YawAccel = 90.f;
	PitchAccel = 90.f;
	RollAccel = 90.f;
	YawSensitivity = 2.f;
	PitchSensitivity = 2.f;
	RollSensitivity = 1.f;

	MaxForwardSpeed = 500.f;
	BrakeThrust = 10000.f;
	BrakeFalloff = 100.f;
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

	//Construct thrusters

	if (ConstructThrusters(ThrustersForward, ThrustersForwardType))
		ThrustersForward[0]->OnUpdateEnergyLevel.AddDynamic(this, &AShip::OnMainEnginesUpdateEnergyLevel);

	ConstructThrusters(ThrustersBackward, ThrustersBackwardType);

	if (ConstructThrusters(ThrustersLeft, ThrustersLeftType))
		ThrustersLeft[0]->OnUpdateEnergyLevel.AddDynamic(this, &AShip::OnEnginesUpdateEnergyLevel);

	ConstructThrusters(ThrustersRight, ThrustersRightType);
	ConstructThrusters(ThrustersUp, ThrustersUpType);
	ConstructThrusters(ThrustersDown, ThrustersDownType);

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

bool AShip::ConstructThrusters(TArray<class UThruster*>& Thrusters, TArray<FThrusterInfo>& ThrustersType)
{
	for (int32 i = 0; i < ThrustersType.Num(); i++)
	{
		Thrusters.Add(NewObject<UThruster>(this, ThrustersType[i].ThrusterClass));
		Thrusters[i]->SetShip(this);

		UParticleSystemComponent* ThrusterParticles = NewObject<UParticleSystemComponent>(this);
		ThrusterParticles->RegisterComponentWithWorld(GetWorld());
		AddOwnedComponent(ThrusterParticles);
		ThrusterParticles->AttachToComponent(ShipMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false), ThrustersType[i].ThrusterSocketName);
		ThrusterParticles->Template = Thrusters[i]->ThrusterParticlesType;
		ThrusterParticles->SetActive(true, true);
		Thrusters[i]->SetParticleComponent(ThrusterParticles);
	}
	return (Thrusters.Num() > 0);
}

void AShip::SetThrustAmount(TArray<class UThruster*>& Thrusters, float Amount)
{
	for (int32 i = 0; i < Thrusters.Num(); i++)
	{
		Thrusters[i]->Thrust(Amount);
	}
}

void AShip::CalculateRotationAxis(float & AxisRotation, float & AxisInput, float & AxisNewRotation, float &AxisAccel, float & AxisMaxRotation, float DeltaTime)
{
	if (AxisRotation > AxisInput*AxisMaxRotation)
	{
		AxisNewRotation = AxisRotation - (AxisRotation < 0 ? FMath::Abs(AxisInput) : 1)*AxisAccel*DeltaTime;
		if (AxisNewRotation < AxisInput*AxisMaxRotation)
		{
			AxisNewRotation = AxisInput*AxisMaxRotation;
		}
	}
	else if (AxisRotation < AxisInput*AxisMaxRotation)
	{
		AxisNewRotation = AxisRotation + (AxisRotation > 0 ? FMath::Abs(AxisInput) : 1)*AxisAccel*DeltaTime;
		if (AxisNewRotation > AxisInput*AxisMaxRotation)
		{
			AxisNewRotation = AxisInput*AxisMaxRotation;
		}
	}
}

void AShip::CalculateThrustAxis(float & AxisVelocity, float & AxisInput, float & AxisThrust, TArray<class UThruster*>& PosThrusters, TArray<class UThruster*>& NegThrusters)
{
	if (AxisVelocity > AxisInput*MaxForwardSpeed)
	{
		SetThrustAmount(PosThrusters, 0.f);
		bool UsingThrusters = false;

		AxisThrust = -CurrentThrustSum(NegThrusters);

		if (AxisVelocity > 0 && AxisThrust > -BrakeThrust)
		{
			AxisThrust = -BrakeThrust;
			SetThrustAmount(NegThrusters, 0.f);
		}
		else UsingThrusters = true;

		if (AxisVelocity < AxisInput*MaxForwardSpeed + BrakeFalloff)
		{
			float BrakePercent = (AxisVelocity - AxisInput*MaxForwardSpeed) / BrakeFalloff;
			AxisThrust *= BrakePercent;
			if (UsingThrusters) SetThrustAmount(NegThrusters, BrakePercent);
		}
		else if (UsingThrusters) SetThrustAmount(NegThrusters, 1.f);

	}
	else if (AxisVelocity < AxisInput*MaxForwardSpeed)
	{
		SetThrustAmount(NegThrusters, 0.f);
		bool UsingThrusters = false;

		AxisThrust = CurrentThrustSum(PosThrusters);

		if (AxisVelocity < 0 && AxisThrust < BrakeThrust)
		{
			AxisThrust = BrakeThrust;
			SetThrustAmount(PosThrusters, 0.f);
		}
		else UsingThrusters = true;

		if (AxisVelocity > AxisInput*MaxForwardSpeed - BrakeFalloff)
		{
			float BrakePercent = (AxisInput*MaxForwardSpeed - AxisVelocity) / BrakeFalloff;
			AxisThrust *= BrakePercent;
			if (UsingThrusters) SetThrustAmount(PosThrusters, BrakePercent);
		}
		else if (UsingThrusters) SetThrustAmount(PosThrusters, 1.f);
	}
	else
	{
		SetThrustAmount(PosThrusters, 0.f);
		SetThrustAmount(NegThrusters, 0.f);
	}
}

// Called every frame
void AShip::Tick(float DeltaTime)
{
	//ROTATION
	FVector CurrentRotation = GetActorRotation().UnrotateVector(ShipMesh->GetPhysicsAngularVelocity("hull"));
	FVector NewRotation = CurrentRotation;
	
	//YAW

	CalculateRotationAxis(CurrentRotation.Z, RotationInput.Z, NewRotation.Z, YawAccel, MaxYawSpeed, DeltaTime);
	CalculateRotationAxis(CurrentRotation.Y, RotationInput.Y, NewRotation.Y, PitchAccel, MaxPitchSpeed, DeltaTime);
	CalculateRotationAxis(CurrentRotation.X, RotationInput.X, NewRotation.X, RollAccel, MaxRollSpeed, DeltaTime);

	ShipMesh->SetPhysicsAngularVelocity(GetActorRotation().RotateVector(NewRotation), false, "hull");

	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("InputRotation: %f yaw, %f pitch, %f roll"), RotationInput.Z, RotationInput.Y, RotationInput.X));
	
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("NewRotation: %f yaw, %f pitch, %f roll"), NewRotation.Z, NewRotation.Y, NewRotation.X));


	//THRUST

	FVector ShipVelocity = GetActorRotation().UnrotateVector(ShipMesh->GetPhysicsLinearVelocity("hull"));
	FVector ThrustForce = FVector::ZeroVector;

	//X

	CalculateThrustAxis(ShipVelocity.X, ThrustInput.X, ThrustForce.X, ThrustersForward, ThrustersBackward);
	CalculateThrustAxis(ShipVelocity.Y, ThrustInput.Y, ThrustForce.Y, ThrustersLeft, ThrustersRight);
	CalculateThrustAxis(ShipVelocity.Z, ThrustInput.Z, ThrustForce.Z, ThrustersDown, ThrustersUp);


	ShipMesh->AddForce(GetActorRotation().RotateVector(ThrustForce), "hull");

	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("InputThrust: %f X, %f Y, %f Z"), ThrustInput.X, ThrustInput.Y, ThrustInput.Z));

	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("ShipVelocity: %f X, %f Y, %f Z"), ShipVelocity.X, ShipVelocity.Y, ShipVelocity.Z));

	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("ThrustForce: %f X, %f Y, %f Z"), ThrustForce.X, ThrustForce.Y, ThrustForce.Z));


	//CAMERA

	FRotator CameraRot = FRotator(NewRotation.Z * CameraRotateMult, NewRotation.Y * CameraRotateMult, NewRotation.X * CameraRotateMult);

	CameraRot = CameraRot.Add(0.f, 0.f, -90.f);

	FVector CameraOff = CameraOffset;

	CameraOff += FVector(-ThrustForce.X, ThrustForce.Y, ThrustForce.Z)*CameraThrustMult;

	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("CameraOffset: %f X, %f Y, %f Z"), CameraOff.X, CameraOff.Y, CameraOff.Z));

	CameraThrustAverage += DeltaTime*CameraAverageEx * (CameraOff - CameraThrustAverage);

	CameraOff = CameraThrustAverage;

	CameraOff = CameraRot.RotateVector(CameraOff);

	Camera->SetRelativeLocationAndRotation(CameraOff, CameraRot);

	Super::Tick(DeltaTime);

	if (BatteryPercent < 1.f && BatteryEfficiency)
	{
		BatteryPercent += DeltaTime*BatteryEfficiency->GetFloatValue(BatteryPercent)*BatteryChargeRate / BatteryCapacity;
		if (BatteryPercent > 1.f)
			BatteryPercent = 1.f;
	}
}

float AShip::CurrentThrustSum(const TArray<class UThruster*>& Thrusters)
{
	float ThrustSum = 0;
	for (int32 i = 0; i < Thrusters.Num(); i++)
	{
		float ThrusterLevel = Thrusters[i]->GetEnergyLevel();
		ThrusterLevel = (ThrusterLevel - Thrusters[i]->ThrustThreshold) / (1 - Thrusters[i]->ThrustThreshold);
		ThrusterLevel = FMath::Max(ThrusterLevel, 0.f);
		ThrustSum += ThrusterLevel*Thrusters[i]->ThrustMax;
	}
	return ThrustSum;
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

void AShip::SetMainEnginesActive(bool Active)
{
	for (int32 i = 0; i < ThrustersForward.Num(); i++)
		ThrustersForward[i]->SetActive(Active);

	for (int32 i = 0; i < ThrustersBackward.Num(); i++)
		ThrustersBackward[i]->SetActive(Active);

	MainEnginesActive = Active;
}

void AShip::SetEnginesActive(bool Active)
{
	for (int32 i = 0; i < ThrustersLeft.Num(); i++)
		ThrustersLeft[i]->SetActive(Active);

	for (int32 i = 0; i < ThrustersRight.Num(); i++)
		ThrustersRight[i]->SetActive(Active);

	for (int32 i = 0; i < ThrustersUp.Num(); i++)
		ThrustersUp[i]->SetActive(Active);

	for (int32 i = 0; i < ThrustersDown.Num(); i++)
		ThrustersDown[i]->SetActive(Active);

	EnginesActive = Active;
}

bool AShip::GetMainEnginesActive()
{
	return MainEnginesActive;
}

bool AShip::GetEnginesActive()
{
	return EnginesActive;
}

void AShip::ThrustForward(float Val)
{
	ThrustInput.X = Val;
}

void AShip::RotateYaw(float Val)
{
	RotationInput.Z = FMath::Clamp(-Val / YawSensitivity, -1.f, 1.f);
}

void AShip::RotatePitch(float Val)
{
	RotationInput.Y = FMath::Clamp(Val / PitchSensitivity, -1.f, 1.f);
}

void AShip::RotateRoll(float Val)
{
	RotationInput.X = FMath::Clamp(-Val / RollSensitivity, -1.f, 1.f);
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

void AShip::OnMainEnginesUpdateEnergyLevel(float EnergyLevel)
{
	for (int32 i = 0; i < MaterialsMain.Num(); i++)
		MaterialsMain[i]->SetScalarParameterValue("MainEngineGlowLevel", EnergyLevel);
}

void AShip::OnEnginesUpdateEnergyLevel(float EnergyLevel)
{
	for (int32 i = 0; i < MaterialsMain.Num(); i++)
		MaterialsMain[i]->SetScalarParameterValue("EngineGlowLevel", EnergyLevel);
}
