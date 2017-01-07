// Fill out your copyright notice in the Description page of Project Settings.

#include "Sunlit.h"
#include "Ship.h"
#include "ShipHUDWidget.h"
#include "ShipController.h"


AShipController::AShipController()
{
	EWeaponTriggers = 0;
	KWeaponTriggers = 0;
}

void AShipController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("ThrustForward", this, &AShipController::OnThrustForward);
	InputComponent->BindAxis("RotateYaw", this, &AShipController::OnRotateYaw);
	InputComponent->BindAxis("RotatePitch", this, &AShipController::OnRotatePitch);
	InputComponent->BindAxis("RotateRoll", this, &AShipController::OnRotateRoll);

	InputComponent->BindAction("EShield", IE_Pressed, this, &AShipController::OnEShieldOn);
	InputComponent->BindAction("EShield", IE_Released, this, &AShipController::OnEShieldOff);
	InputComponent->BindAction("KShield", IE_Pressed, this, &AShipController::OnKShieldOn);
	InputComponent->BindAction("KShield", IE_Released, this, &AShipController::OnKShieldOff);
	InputComponent->BindAction("EWeapon", IE_Pressed, this, &AShipController::OnEWeaponOn);
	InputComponent->BindAction("EWeapon", IE_Released, this, &AShipController::OnEWeaponOff);
	InputComponent->BindAction("KWeapon", IE_Pressed, this, &AShipController::OnKWeaponOn);
	InputComponent->BindAction("KWeapon", IE_Released, this, &AShipController::OnKWeaponOff);
	InputComponent->BindAction("EWeaponTrigger", IE_Pressed, this, &AShipController::OnEWeaponTriggerOn);
	InputComponent->BindAction("EWeaponTrigger", IE_Released, this, &AShipController::OnEWeaponTriggerOff);
	InputComponent->BindAction("KWeaponTrigger", IE_Pressed, this, &AShipController::OnKWeaponTriggerOn);
	InputComponent->BindAction("KWeaponTrigger", IE_Released, this, &AShipController::OnKWeaponTriggerOff);

	InputComponent->BindAction("MainEngineToggle", IE_Pressed, this, &AShipController::OnMainEngineToggle);
	InputComponent->BindAction("EngineToggle", IE_Pressed, this, &AShipController::OnEngineToggle);
	InputComponent->BindAction("CombatModeToggle", IE_Pressed, this, &AShipController::OnCombatModeToggle);
}

void AShipController::OnEShieldOn()
{
	if (ControlledShip) ControlledShip->SetShieldEActive(true);
}

void AShipController::OnEShieldOff()
{
	if (ControlledShip) ControlledShip->SetShieldEActive(false);
}

void AShipController::OnKShieldOn()
{
	if (ControlledShip) ControlledShip->SetShieldKActive(true);
}

void AShipController::OnKShieldOff()
{
	if (ControlledShip) ControlledShip->SetShieldKActive(false);
}

void AShipController::OnEWeaponOn()
{
	if (EWeaponTriggers == 0 && ControlledShip) ControlledShip->SetWeaponEActive(true);
	EWeaponTriggers++;
}

void AShipController::OnEWeaponOff()
{
	if (EWeaponTriggers == 1 && ControlledShip) ControlledShip->SetWeaponEActive(false);
	EWeaponTriggers--;
}

void AShipController::OnKWeaponOn()
{
	if (KWeaponTriggers == 0 && ControlledShip) ControlledShip->SetWeaponKActive(true);
	KWeaponTriggers++;
}

void AShipController::OnKWeaponOff()
{
	if (KWeaponTriggers == 1 && ControlledShip) ControlledShip->SetWeaponKActive(false);
	KWeaponTriggers--;
}

void AShipController::OnEWeaponTriggerOn()
{
	if (ControlledShip)
	{
		ControlledShip->SetWeaponETriggered(true);
		if (EWeaponTriggers == 0) ControlledShip->SetWeaponEActive(true);
	}
	EWeaponTriggers++;
}

void AShipController::OnEWeaponTriggerOff()
{
	if (ControlledShip)
	{
		ControlledShip->SetWeaponETriggered(false);
		if (EWeaponTriggers == 1) ControlledShip->SetWeaponEActive(false);
	}
	EWeaponTriggers--;
}

void AShipController::OnKWeaponTriggerOn()
{
	if (ControlledShip)
	{
		ControlledShip->SetWeaponKTriggered(true);
		if (KWeaponTriggers == 0) ControlledShip->SetWeaponKActive(true);
	}
	KWeaponTriggers++;
}

void AShipController::OnKWeaponTriggerOff()
{
	if (ControlledShip)
	{
		ControlledShip->SetWeaponKTriggered(false);
		if (KWeaponTriggers == 1) ControlledShip->SetWeaponKActive(false);
	}
	KWeaponTriggers--;
}


void AShipController::OnMainEngineToggle()
{
	if (ControlledShip) ControlledShip->SetMainEnginesActive(!ControlledShip->GetMainEnginesActive());
}

void AShipController::OnEngineToggle()
{
	if (ControlledShip) ControlledShip->SetEnginesActive(!ControlledShip->GetEnginesActive());
}

void AShipController::OnCombatModeToggle()
{
	if (!ControlledShip) return;

	SetCombatControls(ControlledShip->SetCombatMode(!ControlledShip->GetCombatMode()));
	ControlledShip->ThrustRight(0.f);
	ControlledShip->ThrustUp(0.f);
}

void AShipController::OnThrustForward(float Val)
{
	if (ControlledShip) ControlledShip->ThrustForward(Val);
}

void AShipController::OnRotateYaw(float Val)
{
	if (!ControlledShip) return;
	if (CombatControls) ControlledShip->ThrustRight(-Val);
	else ControlledShip->RotateYaw(Val);
}

void AShipController::OnRotatePitch(float Val)
{
	if (!ControlledShip) return;
	if (CombatControls) ControlledShip->ThrustUp(-Val);
	else ControlledShip->RotatePitch(Val);
}

void AShipController::OnRotateRoll(float Val)
{
	if (ControlledShip) ControlledShip->RotateRoll(Val);
}



void AShipController::BeginPlay()
{
	Super::BeginPlay();

	ControlledShip = GetWorld()->SpawnActor<AShip>(ShipType, FVector::ZeroVector, FRotator::ZeroRotator);
	SetViewTarget(ControlledShip);
	ControlledShip->SetPlayerShipController(this);

	if (ShipHUDType)
	{
		ShipHUD = CreateWidget<UShipHUDWidget>(this, ShipHUDType);
		if (ShipHUD)
		{
			ShipHUD->AddToViewport();
		}
	}
}

void AShipController::SetCombatControls(bool UseCombatControls)
{
	CombatControls = UseCombatControls;
}

UShipHUDWidget * AShipController::GetHUD()
{
	return ShipHUD;
}

