// Fill out your copyright notice in the Description page of Project Settings.

#include "Sunlit.h"
#include "ShipController.h"
#include "SunlitGameMode.h"

ASunlitGameMode::ASunlitGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerControllerClass = AShipController::StaticClass();
}


