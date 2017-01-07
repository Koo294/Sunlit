// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class SUNLIT_API AProjectile : public AActor
{
	GENERATED_BODY()

private:

	FVector Velocity;

protected:


	UPROPERTY(VisibleAnywhere, Category = Components)
	class UStaticMeshComponent* ProjectileMesh;
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Projectile)
	float Acceleration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Projectile)
	float InitialSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Projectile)
	float Size;

	
	void Fire(FVector InitialVelocity, FRotator FireDirection, float Dispersion);
};
