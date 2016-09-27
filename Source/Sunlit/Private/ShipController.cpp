// Fill out your copyright notice in the Description page of Project Settings.

#include "Sunlit.h"
#include "Ship.h"
#include "ShipController.h"


void AShipController::SetupInputComponent()
{
	Super::SetupInputComponent();

	//InputComponent->BindAxis("CameraZoom", this, &ACTBallController::OnCameraZoom);
	InputComponent->BindAction("EShield", IE_Pressed, this, &AShipController::OnEShieldOn);
	InputComponent->BindAction("EShield", IE_Released, this, &AShipController::OnEShieldOff);
	InputComponent->BindAction("KShield", IE_Pressed, this, &AShipController::OnKShieldOn);
	InputComponent->BindAction("KShield", IE_Released, this, &AShipController::OnKShieldOff);
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


void AShipController::BeginPlay()
{
	Super::BeginPlay();

	ControlledShip = GetWorld()->SpawnActor<AShip>(ShipType, FVector::ZeroVector, FRotator::ZeroRotator);
}

