// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Ship.generated.h"

USTRUCT()
struct FThrusterInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Thruster)
	FName ThrusterSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Thruster)
	TSubclassOf<class UThruster> ThrusterClass;
};

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

	bool ConstructThrusters(TArray<class UThruster*>& Thrusters, TArray<FThrusterInfo>& ThrustersType);

	void SetThrustAmount(TArray<class UThruster*>& Thrusters, float Amount);

	void CalculateRotationAxis(float &AxisRotation, float &AxisInput, float &AxisNewRotation, float &AxisAccel, float &AxisMaxRotation, float DeltaTime);

	void CalculateThrustAxis(float &AxisVelocity, float &AxisInput, float &AxisThrust, TArray<class UThruster*>& PosThrusters, TArray<class UThruster*>& NegThrusters);

	bool MainEnginesActive;

	bool EnginesActive;

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
	TArray<FThrusterInfo> ThrustersForwardType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	TArray<FThrusterInfo> ThrustersBackwardType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	TArray<FThrusterInfo> ThrustersLeftType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	TArray<FThrusterInfo> ThrustersRightType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	TArray<FThrusterInfo> ThrustersUpType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	TArray<FThrusterInfo> ThrustersDownType;

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

	void SetEnginesActive(bool Active);

	bool GetMainEnginesActive();

	bool GetEnginesActive();

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

	UFUNCTION()
	void OnEnginesUpdateEnergyLevel(float EnergyLevel);
};
