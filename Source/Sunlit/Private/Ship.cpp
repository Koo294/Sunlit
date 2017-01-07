// Fill out your copyright notice in the Description page of Project Settings.

#include "Sunlit.h"
#include "EnergyConsumer.h"
#include "Weapon.h"
#include "Shield.h"
#include "Thruster.h"
#include "ShipController.h"
#include "ShipHUDWidget.h"
#include "ShipTargetWidget.h"
#include "Ship.h"



// Sets default values
AShip::AShip()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ShipMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShipMesh"));
	RootComponent = ShipMesh;
	ShipMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ShipMesh->bGenerateOverlapEvents = true;

	ShieldEMainMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldEMainMesh"));
	ShieldEMainMesh->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false), "hull");
	ShieldEMainMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ShieldEMainMesh->SetRelativeRotation(FRotator(0.f, 0.f, -90.f));

	ShieldKMainMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldKMainMesh"));
	ShieldKMainMesh->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false), "hull");
	ShieldKMainMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ShieldKMainMesh->SetRelativeRotation(FRotator(0.f, 0.f, -90.f));

	TargetingRange = CreateDefaultSubobject<USphereComponent>(TEXT("TargetingRange"));
	TargetingRange->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false), "hull");
	TargetingRange->OnComponentBeginOverlap.AddDynamic(this, &AShip::OnTargetingRangeBeginOverlap);
	TargetingRange->OnComponentEndOverlap.AddDynamic(this, &AShip::OnTargetingRangeEndOverlap);
	TargetingRange->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false), "hull");
	Camera->SetRelativeRotation(FRotator(0.f, 0.f, -90.f));

	BatteryPercent = 1.f;
	BatteryChargeRate = 40.f;
	BatteryCapacity = 100.f;
	BatteryDrainBuffer = 0.f;
	onShutdown = false;
	RestartThreshold = 0.1f;

	MaxYawSpeed = 90.f;
	MaxPitchSpeed = 90.f;
	MaxRollSpeed = 90.f;
	YawAccel = 90.f;
	PitchAccel = 90.f;
	RollAccel = 90.f;
	YawSensitivity = 2.f;
	PitchSensitivity = 2.f;
	RollSensitivity = 1.f;

	ForwardThrustSensitivity = 1.f;
	RightThrustSensitivity = 2.f;
	UpThrustSensitivity = 2.f;

	MaxForwardSpeed = 500.f;
	BrakeThrust = 10000.f;
	BrakeFalloff = 100.f;

	CombatMode = false;
	DecoupledMode = false;
}

void AShip::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AShip::ConsumeBattery(float ConsumeCharge)
{
	if (ConsumeCharge <= 0.f) return;

	BatteryDrainBuffer += ConsumeCharge;
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

void AShip::SetThrustAmount(TArray<class UThruster*>& Thrusters, float Amount, float DeltaTime)
{
	for (int32 i = 0; i < Thrusters.Num(); i++)
	{
		Thrusters[i]->Thrust(Amount, DeltaTime);
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
		else if (AxisNewRotation > AxisMaxRotation)
		{
			AxisNewRotation = AxisMaxRotation;
		}
	}
	else if (AxisRotation < AxisInput*AxisMaxRotation)
	{
		AxisNewRotation = AxisRotation + (AxisRotation > 0 ? FMath::Abs(AxisInput) : 1)*AxisAccel*DeltaTime;
		if (AxisNewRotation > AxisInput*AxisMaxRotation)
		{
			AxisNewRotation = AxisInput*AxisMaxRotation;
		}
		else if (AxisNewRotation < -AxisMaxRotation)
		{
			AxisNewRotation = -AxisMaxRotation;
		}
	}
}

void AShip::CalculateRotationToDeltaAxis(float & AxisRotation, float AxisDelta, float & AxisNewRotation, float & AxisAccel, float & AxisMaxRotation, float DeltaTime)
{
	float DecelDistance = FMath::Square(AxisRotation) / (2*AxisAccel);
	if (AxisDelta > 0.f)
	{
		if (AxisDelta < DecelDistance)
		{
			AxisNewRotation = AxisRotation*(1 - ((DecelDistance - AxisDelta) / DecelDistance));
		}
		else
		{
			if (AxisRotation > -AxisMaxRotation)
			{
				float AccelFalloffMult = 1.f;
				if (AxisDelta < DeltaAccelFalloff)
				{
					AccelFalloffMult = 1 - ((DeltaAccelFalloff - AxisDelta) / DeltaAccelFalloff);
				}
				AxisNewRotation = AxisRotation - DeltaTime*AxisAccel*AccelFalloffMult;
			}
			if (AxisRotation < -AxisMaxRotation || AxisNewRotation < -AxisMaxRotation) AxisNewRotation = -AxisMaxRotation;
		}
	}
	else if (AxisDelta < 0.f)
	{
		if (AxisDelta > -DecelDistance)
		{
			AxisNewRotation = AxisRotation*(1 - ((DecelDistance + AxisDelta) / DecelDistance));
		}
		else
		{
			if (AxisRotation < AxisMaxRotation)
			{
				float AccelFalloffMult = 1.f;
				if (AxisDelta > -DeltaAccelFalloff)
				{
					AccelFalloffMult = 1 - ((DeltaAccelFalloff + AxisDelta) / DeltaAccelFalloff);
				}
				AxisNewRotation = AxisRotation + DeltaTime*AxisAccel*AccelFalloffMult;
			}
			if (AxisRotation > AxisMaxRotation || AxisNewRotation > AxisMaxRotation) AxisNewRotation = AxisMaxRotation;
		}
	}
}

void AShip::CalculateThrustAxis(float & AxisVelocity, float & AxisInput, float & AxisThrust, TArray<class UThruster*>& PosThrusters, TArray<class UThruster*>& NegThrusters, float DeltaTime)
{
	if (AxisVelocity > AxisInput*MaxForwardSpeed)
	{
		SetThrustAmount(PosThrusters, 0.f, DeltaTime);
		bool UsingThrusters = false;

		float ThrustPercent = (AxisVelocity < 0 ? FMath::Abs(AxisInput) : 1);

		AxisThrust = ThrustPercent*-CurrentThrustSum(NegThrusters);

		if (AxisVelocity > 0 && AxisThrust > -BrakeThrust)
		{
			AxisThrust = -BrakeThrust;
			SetThrustAmount(NegThrusters, 0.f, DeltaTime);
		}
		else UsingThrusters = true;

		if (AxisVelocity < AxisInput*MaxForwardSpeed + BrakeFalloff)
		{
			float BrakePercent = (AxisVelocity - AxisInput*MaxForwardSpeed) / BrakeFalloff;
			AxisThrust *= BrakePercent;
			if (UsingThrusters) SetThrustAmount(NegThrusters, ThrustPercent*BrakePercent, DeltaTime);
		}
		else if (UsingThrusters) SetThrustAmount(NegThrusters, ThrustPercent, DeltaTime);

	}
	else if (AxisVelocity < AxisInput*MaxForwardSpeed)
	{
		SetThrustAmount(NegThrusters, 0.f, DeltaTime);
		bool UsingThrusters = false;

		float ThrustPercent = (AxisVelocity > 0 ? FMath::Abs(AxisInput) : 1);

		AxisThrust = ThrustPercent*CurrentThrustSum(PosThrusters);

		if (AxisVelocity < 0 && AxisThrust < BrakeThrust)
		{
			AxisThrust = BrakeThrust;
			SetThrustAmount(PosThrusters, 0.f, DeltaTime);
		}
		else UsingThrusters = true;

		if (AxisVelocity > AxisInput*MaxForwardSpeed - BrakeFalloff)
		{
			float BrakePercent = (AxisInput*MaxForwardSpeed - AxisVelocity) / BrakeFalloff;
			AxisThrust *= BrakePercent;
			if (UsingThrusters) SetThrustAmount(PosThrusters, ThrustPercent*BrakePercent, DeltaTime);
		}
		else if (UsingThrusters) SetThrustAmount(PosThrusters, ThrustPercent, DeltaTime);
	}
	else
	{
		SetThrustAmount(PosThrusters, 0.f, DeltaTime);
		SetThrustAmount(NegThrusters, 0.f, DeltaTime);
	}
}

// Called every frame
void AShip::Tick(float DeltaTime)
{

	//ROTATION
	FVector CurrentRotation = GetActorRotation().UnrotateVector(ShipMesh->GetPhysicsAngularVelocity("hull"));
	FVector NewRotation = CurrentRotation;

	if (CombatMode)
	{
		FRotator DeltaTarget = GetActorRotation().UnrotateVector(TargetShips[CombatTarget]->GetActorLocation() - GetActorLocation()).Rotation();
		DeltaTarget.Normalize();

		CalculateRotationToDeltaAxis(CurrentRotation.Z, -DeltaTarget.Yaw, NewRotation.Z, YawAccel, MaxYawSpeed, DeltaTime);
		CalculateRotationToDeltaAxis(CurrentRotation.Y, DeltaTarget.Pitch, NewRotation.Y, PitchAccel, MaxPitchSpeed, DeltaTime);

	}
	else
	{
		CalculateRotationAxis(CurrentRotation.Z, RotationInput.Z, NewRotation.Z, YawAccel, MaxYawSpeed, DeltaTime);
		CalculateRotationAxis(CurrentRotation.Y, RotationInput.Y, NewRotation.Y, PitchAccel, MaxPitchSpeed, DeltaTime);
	}
	
	CalculateRotationAxis(CurrentRotation.X, RotationInput.X, NewRotation.X, RollAccel, MaxRollSpeed, DeltaTime);

	ShipMesh->SetPhysicsAngularVelocity(GetActorRotation().RotateVector(NewRotation), false, "hull");

	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("InputRotation: %f yaw, %f pitch, %f roll"), RotationInput.Z, RotationInput.Y, RotationInput.X));
	
	//if (debugunit) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("NewRotation: %f yaw, %f pitch, %f roll"), NewRotation.Z, NewRotation.Y, NewRotation.X));


	//THRUST

	FVector ShipVelocity = GetActorRotation().UnrotateVector(ShipMesh->GetPhysicsLinearVelocity("hull"));
	FVector ThrustForce = FVector::ZeroVector;
	
	CalculateThrustAxis(ShipVelocity.X, ThrustInput.X, ThrustForce.X, ThrustersForward, ThrustersBackward, DeltaTime);
	CalculateThrustAxis(ShipVelocity.Y, ThrustInput.Y, ThrustForce.Y, ThrustersLeft, ThrustersRight, DeltaTime);
	CalculateThrustAxis(ShipVelocity.Z, ThrustInput.Z, ThrustForce.Z, ThrustersDown, ThrustersUp, DeltaTime);


	ShipMesh->AddForce(GetActorRotation().RotateVector(ThrustForce), "hull");

	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("InputThrust: %f X, %f Y, %f Z"), ThrustInput.X, ThrustInput.Y, ThrustInput.Z));

	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("ShipVelocity: %f X, %f Y, %f Z"), ShipVelocity.X, ShipVelocity.Y, ShipVelocity.Z));

	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("ThrustForce: %f X, %f Y, %f Z"), ThrustForce.X, ThrustForce.Y, ThrustForce.Z));


	//CAMERA

	if (PlayerShipController)
	{
		FRotator CameraRot = FRotator(NewRotation.Z * CameraRotateMult, NewRotation.Y * CameraRotateMult, NewRotation.X * CameraRotateMult);

		CameraRot = CameraRot.Add(0.f, 0.f, -90.f);

		FVector CameraOff = CameraOffset;

		CameraOff += FVector(-ThrustForce.X, ThrustForce.Y, ThrustForce.Z)*CameraThrustMult;

		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("CameraOffset: %f X, %f Y, %f Z"), CameraOff.X, CameraOff.Y, CameraOff.Z));

		CameraThrustAverage += DeltaTime*CameraAverageEx * (CameraOff - CameraThrustAverage);

		CameraOff = CameraThrustAverage;

		CameraOff = CameraRot.RotateVector(CameraOff);

		Camera->SetRelativeLocationAndRotation(CameraOff, CameraRot);
	}

	Super::Tick(DeltaTime);

	//BATTERY

	if (BatteryPercent < 1.f && BatteryEfficiency)
		BatteryPercent += DeltaTime*BatteryEfficiency->GetFloatValue(BatteryPercent)*BatteryChargeRate / BatteryCapacity;

	BatteryPercent -= BatteryDrainBuffer / BatteryCapacity;
	BatteryDrainBuffer = 0.f;

	if (!onShutdown && BatteryPercent < 0.f)
		SystemShutdown();
	if (onShutdown && BatteryPercent > RestartThreshold)
		SystemRestart();
	if (BatteryPercent > 1.f)
		BatteryPercent = 1.f;

	if (PlayerShipController)
	{
		PlayerShipController->GetHUD()->SetBatteryCharge(BatteryPercent);
	}

	//if (debugunit) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("Battery: %f"), BatteryPercent));

	//HUD

	if (PlayerShipController)
	{

		int32 ViewportX, ViewportY;
		PlayerShipController->GetViewportSize(ViewportX, ViewportY);


		for (int32 i = 0; i < TargetShips.Num(); i++)
		{
			FVector2D TargetOnScreen;
			bool TargetInView = false;

			if (UGameplayStatics::ProjectWorldToScreen(PlayerShipController, TargetShips[i]->GetActorLocation(), TargetOnScreen))
			{
				if (TargetOnScreen.X > 0.f && TargetOnScreen.X < ViewportX && TargetOnScreen.Y > 0.f && TargetOnScreen.Y < ViewportY)
				{
					TargetInView = true;
					PlayerShipController->GetHUD()->MoveShipTarget(i, TargetOnScreen, false);
					PlayerShipController->GetHUD()->GetShipTarget(i)->SetTargetInView(true);
					PlayerShipController->GetHUD()->GetShipTarget(i)->SetArrowDir(0.f);
				}
			}

			if (!TargetInView)
			{
				FVector ShipOffset = GetActorRotation().UnrotateVector(TargetShips[i]->GetActorLocation() - GetActorLocation());
				float ShipTargetDirection;
				if (ShipOffset.Y > 0)
				{
					if (ShipOffset.Z > 0)
					{
						ShipTargetDirection = FMath::Atan(ShipOffset.Z / ShipOffset.Y) + PI;
					}
					else if (ShipOffset.Z < 0)
					{
						ShipTargetDirection = FMath::Atan(ShipOffset.Y / -ShipOffset.Z) + PI/2;
					}
					else ShipTargetDirection = PI;
				}
				else if (ShipOffset.Y < 0)
				{
					if (ShipOffset.Z > 0)
					{
						ShipTargetDirection = FMath::Atan(-ShipOffset.Y / ShipOffset.Z) + 3*PI/2;
					}
					else if (ShipOffset.Z < 0)
					{
						ShipTargetDirection = FMath::Atan(-ShipOffset.Z / -ShipOffset.Y);
					}
					else ShipTargetDirection = 0;
				}
				else
				{
					if (ShipOffset.Z > 0)
					{
						ShipTargetDirection = 3*PI / 2;
					}
					else if (ShipOffset.Z < 0)
					{
						ShipTargetDirection = PI / 2;
					}
					else ShipTargetDirection = 0;
				}

				float ViewportRatio = (float)ViewportX / (float)ViewportY;
				float OffsetRatio = FMath::Abs(ShipOffset.Y / ShipOffset.Z);
				float ViewOffsetRatio;

				if (ViewportRatio > OffsetRatio)
				{
					ViewOffsetRatio = FMath::Abs(((float)ViewportY / 2.f) / ShipOffset.Z);
				}
				else
				{
					ViewOffsetRatio = FMath::Abs(((float)ViewportX / 2.f) / ShipOffset.Y);
				}
				TargetOnScreen = FVector2D(-ShipOffset.Y, ShipOffset.Z)*ViewOffsetRatio + FVector2D((float)ViewportX/2.f, (float)ViewportY/2.f);
				PlayerShipController->GetHUD()->MoveShipTarget(i, TargetOnScreen, true);
				PlayerShipController->GetHUD()->GetShipTarget(i)->SetArrowDir(FMath::RadiansToDegrees(ShipTargetDirection));
				PlayerShipController->GetHUD()->GetShipTarget(i)->SetTargetInView(false);
			}
		}
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

void AShip::ShutdownConsumer(UEnergyConsumer * Consumer)
{
	if (Consumer && Consumer->GetActive())
	{
		Consumer->SetActive(false);
		ActiveShutdown.Add(Consumer);
	}
}

void AShip::SystemShutdown()
{
	onShutdown = true;

	ShutdownConsumer(WeaponEMain);
	ShutdownConsumer(WeaponKMain);
	ShutdownConsumer(ShieldEMain);
	ShutdownConsumer(ShieldKMain);

	for (int32 i = 0; i < ThrustersBackward.Num(); i++)
		ShutdownConsumer(ThrustersBackward[i]);
	for (int32 i = 0; i < ThrustersForward.Num(); i++)
		ShutdownConsumer(ThrustersForward[i]);
	for (int32 i = 0; i < ThrustersLeft.Num(); i++)
		ShutdownConsumer(ThrustersLeft[i]);
	for (int32 i = 0; i < ThrustersRight.Num(); i++)
		ShutdownConsumer(ThrustersRight[i]);
	for (int32 i = 0; i < ThrustersUp.Num(); i++)
		ShutdownConsumer(ThrustersUp[i]);
	for (int32 i = 0; i < ThrustersDown.Num(); i++)
		ShutdownConsumer(ThrustersDown[i]);
}

void AShip::SystemRestart()
{
	onShutdown = false;

	for (int32 i = 0; i < ActiveShutdown.Num(); i++)
	{
		ActiveShutdown[i]->SetActive(true);
	}
	ActiveShutdown.Empty();
}

void AShip::SetConsumerActive(UEnergyConsumer* Consumer, bool Active)
{
	if (Consumer)
		if (onShutdown)
			if (Active)
				ActiveShutdown.Add(Consumer);
			else
				ActiveShutdown.Remove(Consumer);
		else
			Consumer->SetActive(Active);
}

void AShip::SetWeaponEActive(bool Active)
{
	SetConsumerActive(WeaponEMain, Active);
}

void AShip::SetWeaponKActive(bool Active)
{
	SetConsumerActive(WeaponKMain, Active);
}

void AShip::SetWeaponETriggered(bool Triggered)
{
	if (WeaponEMain)
		WeaponEMain->SetTriggered(Triggered);
}

void AShip::SetWeaponKTriggered(bool Triggered)
{
	if (WeaponKMain)
		WeaponKMain->SetTriggered(Triggered);
}

void AShip::GetWeaponFireParameters(UWeapon * Weapon, FTransform & Transform, FVector & InitialVelocity, UWorld *& World)
{
	FName WeaponSocket;
	if (Weapon == WeaponEMain) WeaponSocket = WeaponESocket;
	else if (Weapon == WeaponKMain) WeaponSocket = WeaponKSocket;
	else return;

	InitialVelocity = ShipMesh->GetPhysicsLinearVelocity("hull");

	Transform = ShipMesh->GetSocketTransform(WeaponSocket, ERelativeTransformSpace::RTS_World);
	World = GetWorld();
}


void AShip::SetShieldEActive(bool Active)
{
	SetConsumerActive(ShieldEMain, Active);
}


void AShip::SetShieldKActive(bool Active)
{
	SetConsumerActive(ShieldKMain, Active);
}

void AShip::SetMainEnginesActive(bool Active)
{
	for (int32 i = 0; i < ThrustersForward.Num(); i++)
		SetConsumerActive(ThrustersForward[i], Active);

	for (int32 i = 0; i < ThrustersBackward.Num(); i++)
		SetConsumerActive(ThrustersBackward[i], Active);

	MainEnginesActive = Active;
}

void AShip::SetEnginesActive(bool Active)
{
	for (int32 i = 0; i < ThrustersLeft.Num(); i++)
		SetConsumerActive(ThrustersLeft[i], Active);

	for (int32 i = 0; i < ThrustersRight.Num(); i++)
		SetConsumerActive(ThrustersRight[i], Active);

	for (int32 i = 0; i < ThrustersUp.Num(); i++)
		SetConsumerActive(ThrustersUp[i], Active);

	for (int32 i = 0; i < ThrustersDown.Num(); i++)
		SetConsumerActive(ThrustersDown[i], Active);

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
	ThrustInput.X = FMath::Clamp(Val / ForwardThrustSensitivity, -1.f, 1.f);
}

void AShip::ThrustRight(float Val)
{
	ThrustInput.Y = FMath::Clamp(Val / RightThrustSensitivity, -1.f, 1.f);
}

void AShip::ThrustUp(float Val)
{
	ThrustInput.Z = FMath::Clamp(Val / UpThrustSensitivity, -1.f, 1.f);
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

bool AShip::SetCombatMode(bool ToCombat)
{
	if (ToCombat)
	{
		if (TargetShips.Num() > 0)
		{
			int32 NewCombatTarget = 0;
			float DeltaSum = 360.f;
			for (int32 i = 0; i < TargetShips.Num(); i++)
			{
				FRotator DeltaTarget = GetActorRotation().UnrotateVector(TargetShips[i]->GetActorLocation() - GetActorLocation()).Rotation();
				DeltaTarget.Normalize();
				if (FMath::Abs(DeltaTarget.Yaw) + FMath::Abs(DeltaTarget.Pitch) < DeltaSum)
				{
					DeltaSum = FMath::Abs(DeltaTarget.Yaw) + FMath::Abs(DeltaTarget.Pitch);
					NewCombatTarget = i;
				}
			}
			CombatTarget = NewCombatTarget;
			CombatMode = true;
		}
		else CombatMode = false;
	}
	else CombatMode = false;
	return CombatMode;
}

bool AShip::GetCombatMode()
{
	return CombatMode;
}

void AShip::SetPlayerShipController(AShipController * Controller)
{
	PlayerShipController = Controller;
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

void AShip::OnTargetingRangeBeginOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor == this) return;
	AShip* NewTargetShip = Cast<AShip>(OtherActor);
	if (NewTargetShip && OtherComp->GetClass() == USkeletalMeshComponent::StaticClass())
	{
		if (!TargetShips.Contains(NewTargetShip))
		{
			TargetShips.Add(NewTargetShip);
			if (PlayerShipController) PlayerShipController->GetHUD()->AddShipTarget(NewTargetShip->ShipName);
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Target ship count %d"), TargetShips.Num()));
		}
	}
}

void AShip::OnTargetingRangeEndOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == this) return;
	AShip* OldTargetShip = Cast<AShip>(OtherActor);
	if (OldTargetShip && OtherComp->GetClass() == USkeletalMeshComponent::StaticClass())
	{
		int32 RemovedIndex;
		TargetShips.Find(OldTargetShip, RemovedIndex);
		if (PlayerShipController) PlayerShipController->GetHUD()->RemoveShipTarget(RemovedIndex);
		if (CombatMode)
		{
			if (RemovedIndex == CombatTarget)
			{
				SetCombatMode(false);
				if (PlayerShipController) PlayerShipController->SetCombatControls(false);
			}
			else if (RemovedIndex < CombatTarget)
				CombatTarget--;
			
		}
		TargetShips.RemoveAt(RemovedIndex);
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Target ship count %d"), TargetShips.Num()));
	}
}