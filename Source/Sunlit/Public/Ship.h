// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Ship.generated.h"

UCLASS()
class SUNLIT_API AShip : public AActor
{
	GENERATED_BODY()

private:

	//LOADOUT

	UPROPERTY()
	class UWeapon* WeaponEMain;

	UPROPERTY()
	class UWeapon* WeaponKMain;

	UPROPERTY()
	class UShield* ShieldEMain;

	UPROPERTY()
	class UShield* ShieldKMain;

	//MOVEMENT

	UPROPERTY()
	TArray<class UThruster*> ThrustersForward;

	UPROPERTY()
	TArray<class UThruster*> ThrustersBackward;

	UPROPERTY()
	TArray<class UThruster*> ThrustersLeft;

	UPROPERTY()
	TArray<class UThruster*> ThrustersRight;

	UPROPERTY()
	TArray<class UThruster*> ThrustersUp;

	UPROPERTY()
	TArray<class UThruster*> ThrustersDown;

	bool MainEnginesActive;

	FVector RotationInput;

	FVector ThrustInput;

	float CurrentThrustSum(const TArray<class UThruster*>& Thrusters);

	//ENERGY
	
	float BatteryPercent;

	//MATERIALS

	TArray<UMaterialInstanceDynamic*> MaterialsMain;

	UMaterialInstanceDynamic* ShieldEMainMat;

	UMaterialInstanceDynamic* ShieldKMainMat;

	//CAMERA

	FVector CameraThrustAverage;

protected:

	UPROPERTY(VisibleAnywhere, Category = Components)
	class USkeletalMeshComponent* ShipMesh;

	UPROPERTY(VisibleAnywhere, Category = Components)
	class UStaticMeshComponent* ShieldEMainMesh;

	UPROPERTY(VisibleAnywhere, Category = Components)
	class UStaticMeshComponent* ShieldKMainMesh;

	UPROPERTY(VisibleAnywhere, Category = Components)
	class UCameraComponent* Camera;

public:	
	// Sets default values for this actor's properties
	AShip();

	//LOADOUT TYPES

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Loadout)
	TSubclassOf<class UWeapon> WeaponEMainType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Loadout)
	TSubclassOf<class UWeapon> WeaponKMainType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Loadout)
	TSubclassOf<class UShield> ShieldEMainType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Loadout)
	TSubclassOf<class UShield> ShieldKMainType;

	//MOVEMENT TYPES

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	TArray<TSubclassOf<class UThruster>> ThrustersForwardType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	TArray<TSubclassOf<class UThruster>> ThrustersBackwardType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	TArray<TSubclassOf<class UThruster>> ThrustersLeftType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	TArray<TSubclassOf<class UThruster>> ThrustersRightType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	TArray<TSubclassOf<class UThruster>> ThrustersUpType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	TArray<TSubclassOf<class UThruster>> ThrustersDownType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	float MaxForwardSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	float BrakeThrust;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	float BrakeFalloff;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	float MaxYawSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	float MaxPitchSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	float MaxRollSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	float YawAccel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	float PitchAccel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	float RollAccel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	float YawSensitivity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	float PitchSensitivity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	float RollSensitivity;

	//ENERGY

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Battery)
	UCurveFloat* BatteryEfficiency;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Battery)
	float BatteryCapacity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Battery)
	float BatteryChargeRate;

	//MATERIALS

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Materials)
	TArray<int32> MaterialIndexMain;

	//CAMERA

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
	FVector CameraOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
	float CameraRotateMult;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
	float CameraThrustMult;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
	float CameraAverageEx;

	void ConsumeBattery(float ConsumeCharge);

	float GetBatteryCharge();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION(BlueprintCallable, Category = "TEST")
	void SetWeaponEActive(bool Active);

	UFUNCTION(BlueprintCallable, Category = "TEST")
	void SetWeaponKActive(bool Active);

	UFUNCTION(BlueprintCallable, Category = "TEST")
	void SetShieldEActive(bool Active);

	UFUNCTION(BlueprintCallable, Category = "TEST")
	void SetShieldKActive(bool Active);

	void SetMainEnginesActive(bool Active);

	bool GetMainEnginesActive();

	void ThrustForward(float Val);

	void RotateYaw(float Val);

	void RotatePitch(float Val);

	void RotateRoll(float Val);

	UFUNCTION()
	void OnWeaponEUpdateEnergyLevel(float EnergyLevel);

	UFUNCTION()
	void OnWeaponKUpdateEnergyLevel(float EnergyLevel);

	UFUNCTION()
	void OnShieldEUpdateEnergyLevel(float EnergyLevel);

	UFUNCTION()
	void OnShieldKUpdateEnergyLevel(float EnergyLevel);

	UFUNCTION()
	void OnMainEnginesUpdateEnergyLevel(float EnergyLevel);
};
