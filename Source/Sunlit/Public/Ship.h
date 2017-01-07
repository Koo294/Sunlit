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

	void SetThrustAmount(TArray<class UThruster*>& Thrusters, float Amount, float DeltaTime);

	void CalculateRotationAxis(float &AxisRotation, float &AxisInput, float &AxisNewRotation, float &AxisAccel, float &AxisMaxRotation, float DeltaTime);

	void CalculateRotationToDeltaAxis(float &AxisRotation, float AxisDelta, float &AxisNewRotation, float &AxisAccel, float &AxisMaxRotation, float DeltaTime);

	void CalculateThrustAxis(float &AxisVelocity, float &AxisInput, float &AxisThrust, TArray<class UThruster*>& PosThrusters, TArray<class UThruster*>& NegThrusters, float DeltaTime);

	bool MainEnginesActive;

	bool EnginesActive;

	FVector RotationInput;

	FVector ThrustInput;

	float CurrentThrustSum(const TArray<class UThruster*>& Thrusters);

	//ENERGY
	
	float BatteryPercent;

	float BatteryDrainBuffer;

	bool onShutdown;

	float RestartThreshold;

	TArray<class UEnergyConsumer*> ActiveShutdown;

	void ShutdownConsumer(class UEnergyConsumer* Consumer);

	void SystemShutdown();

	void SystemRestart();

	void SetConsumerActive(class UEnergyConsumer* Consumer, bool Active);

	//MATERIALS

	TArray<UMaterialInstanceDynamic*> MaterialsMain;

	UMaterialInstanceDynamic* ShieldEMainMat;

	UMaterialInstanceDynamic* ShieldKMainMat;

	//CAMERA

	FVector CameraThrustAverage;

	//FLIGHT

	bool CombatMode;

	int32 CombatTarget;

	bool DecoupledMode;

	TArray<AShip*> TargetShips;

	class AShipController* PlayerShipController;

protected:

	UPROPERTY(VisibleAnywhere, Category = Components)
	class USkeletalMeshComponent* ShipMesh;

	UPROPERTY(VisibleAnywhere, Category = Components)
	class UStaticMeshComponent* ShieldEMainMesh;

	UPROPERTY(VisibleAnywhere, Category = Components)
	class UStaticMeshComponent* ShieldKMainMesh;

	UPROPERTY(VisibleAnywhere, Category = Components)
	class USphereComponent* TargetingRange;

	UPROPERTY(VisibleAnywhere, Category = Components)
	class UCameraComponent* Camera;

	UFUNCTION()
	void OnTargetingRangeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnTargetingRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	

	// Sets default values for this actor's properties
	AShip();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Ship)
	FString ShipName;

	//LOADOUT TYPES

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Loadout)
	TSubclassOf<class UWeapon> WeaponEMainType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Loadout)
	TSubclassOf<class UWeapon> WeaponKMainType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Loadout)
	TSubclassOf<class UShield> ShieldEMainType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Loadout)
	TSubclassOf<class UShield> ShieldKMainType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Loadout)
	FName WeaponESocket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Loadout)
	FName WeaponKSocket;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	float ForwardThrustSensitivity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	float RightThrustSensitivity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	float UpThrustSensitivity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	float DeltaAccelFalloff;

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

	void SetWeaponEActive(bool Active);

	void SetWeaponKActive(bool Active);

	void SetWeaponETriggered(bool Triggered);

	void SetWeaponKTriggered(bool Triggered);

	void GetWeaponFireParameters(UWeapon * Weapon, FTransform & Transform, FVector & InitialVelocity, UWorld *& World);

	void SetShieldEActive(bool Active);

	void SetShieldKActive(bool Active);

	void SetMainEnginesActive(bool Active);

	void SetEnginesActive(bool Active);

	bool GetMainEnginesActive();

	bool GetEnginesActive();

	void ThrustForward(float Val);

	void ThrustRight(float Val);

	void ThrustUp(float Val);

	void RotateYaw(float Val);

	void RotatePitch(float Val);

	void RotateRoll(float Val);

	bool SetCombatMode(bool ToCombat);

	bool GetCombatMode();

	void SetPlayerShipController(class AShipController* Controller);

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = debug)
	bool debugunit;
};
