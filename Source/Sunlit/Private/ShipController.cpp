// Fill out your copyright notice in the Description page of Project Settings.

#include "Sunlit.h"
#include "Ship.h"
#include "ShipController.h"


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

	InputComponent->BindAction("MainEngineToggle", IE_Pressed, this, &AShipController::OnMainEngineToggle);
}

void AShipController::OnEShieldOn()
{
	ControlledShip->SetShieldEActive(true);
}

void AShipController::OnEShieldOff()
{
	ControlledShip->SetShieldEActive(false);
}

void AShipController::OnKShieldOn()
{
	ControlledShip->SetShieldKActive(true);
}

void AShipController::OnKShieldOff()
{
	ControlledShip->SetShieldKActive(false);
}

void AShipController::OnMainEngineToggle()
{
	ControlledShip->SetMainEnginesActive(!ControlledShip->GetMainEnginesActive());
}

void AShipController::OnThrustForward(float Val)
{
	ControlledShip->ThrustForward(Val);
}

void AShipController::OnRotateYaw(float Val)
{
	ControlledShip->RotateYaw(Val);
}

void AShipController::OnRotatePitch(float Val)
{
	ControlledShip->RotatePitch(Val);
}


void AShipController::OnRotateRoll(float Val)
{
	ControlledShip->RotateRoll(Val);
}



void AShipController::BeginPlay()
{
	Super::BeginPlay();

	ControlledShip = GetWorld()->SpawnActor<AShip>(ShipType, FVector::ZeroVector, FRotator::ZeroRotator);
	SetViewTarget(ControlledShip);
}

