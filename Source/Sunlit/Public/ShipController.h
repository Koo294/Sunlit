// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "ShipController.generated.h"

/**
 * 
 */
UCLASS()
class SUNLIT_API AShipController : public APlayerController
{
	GENERATED_BODY()
	
private:

	class AShip* ControlledShip;

protected:

	virtual void SetupInputComponent() override;
	
	void OnEShieldOn();
	void OnEShieldOff();
	void OnKShieldOn();
	void OnKShieldOff();

public:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Ship)
	TSubclassOf<class AShip> ShipType;

};
