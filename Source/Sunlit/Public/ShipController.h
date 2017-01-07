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

	bool CombatControls;

	class AShip* ControlledShip;
	class UShipHUDWidget* ShipHUD;

	uint8 EWeaponTriggers;
	uint8 KWeaponTriggers;

protected:

	virtual void SetupInputComponent() override;
	
	void OnEShieldOn();
	void OnEShieldOff();
	void OnKShieldOn();
	void OnKShieldOff();
	void OnEWeaponOn();
	void OnEWeaponOff();
	void OnKWeaponOn();
	void OnKWeaponOff();
	void OnEWeaponTriggerOn();
	void OnEWeaponTriggerOff();
	void OnKWeaponTriggerOn();
	void OnKWeaponTriggerOff();
	void OnMainEngineToggle();
	void OnEngineToggle();
	void OnCombatModeToggle();

	void OnThrustForward(float Val);
	void OnRotateYaw(float Val);
	void OnRotatePitch(float Val);
	void OnRotateRoll(float Val);

public:

	AShipController();

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Ship)
	TSubclassOf<class AShip> ShipType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Ship)
	TSubclassOf<class UShipHUDWidget> ShipHUDType;

	void SetCombatControls(bool UseCombatControls);

	class UShipHUDWidget* GetHUD();

};
