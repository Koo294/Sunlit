// Fill out your copyright notice in the Description page of Project Settings.

#include "Sunlit.h"
#include "Projectile.h"


// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	RootComponent = ProjectileMesh;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectile::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	Velocity += Velocity.GetSafeNormal()*Acceleration*DeltaTime;

	UWorld* World = GetWorld();
	if (World)
	{
		FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
		TraceParams.bTraceComplex = false;
		TraceParams.bReturnPhysicalMaterial = false;
		TraceParams.AddIgnoredActor(this);
		TraceParams.AddIgnoredActor(GetOwner());

		FCollisionObjectQueryParams ObjectTraceParams = FCollisionObjectQueryParams();
		ObjectTraceParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
		ObjectTraceParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
		ObjectTraceParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldDynamic);

		TArray<FHitResult> ProjectileHits;

		bool TraceSuccess = false;
		
		if (Size > 0.f) TraceSuccess = World->SweepMultiByObjectType(ProjectileHits, GetActorLocation(), GetActorLocation() + Velocity*DeltaTime, FQuat(), ObjectTraceParams, FCollisionShape::MakeSphere(Size), TraceParams);
		else TraceSuccess = World->LineTraceMultiByObjectType(ProjectileHits, GetActorLocation(), GetActorLocation() + Velocity*DeltaTime, ObjectTraceParams, TraceParams);

		if (TraceSuccess)
		{
			for (int32 i = 0; i < ProjectileHits.Num(); i++)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Projectile hit %s"), *ProjectileHits[i].GetActor()->GetName()));
			}
		}
	}

	SetActorLocation(GetActorLocation() + Velocity*DeltaTime);
	//SetActorRotation(Velocity.Rotation());
}

void AProjectile::Fire(FVector InitialVelocity, FRotator FireDirection, float Dispersion)
{
	float DispersionAmount = FMath::FRandRange(-Dispersion, Dispersion);
	FRotator DispersionRot = FRotator(DispersionAmount, 0.f, 0.f);
	DispersionRot = DispersionRot.Vector().RotateAngleAxis(FMath::FRandRange(0.f, 360.f), FVector::ForwardVector).Rotation();
	FireDirection += DispersionRot;
	Velocity = InitialVelocity + FireDirection.Vector().GetSafeNormal() * InitialSpeed;
}

