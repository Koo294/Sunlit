// Fill out your copyright notice in the Description page of Project Settings.

#include "Sunlit.h"
#include "Ship.h"
#include "EnergyConsumer.h"

UEnergyConsumer::UEnergyConsumer()
{
	IsActive = false;
	IsRamping = false;
	Reactive = false;
	ReactiveThreshold = 1.f;
}

void UEnergyConsumer::SetActive(bool NewActive)
{
	if (IsActive != NewActive)
	{
		IsActive = NewActive;
		if (IsRamping)
		{
			UCurveFloat* PrevCurve = IsActive ? DeactivateCurve : ActivateCurve;
			UCurveFloat* NewCurve = IsActive ? ActivateCurve : DeactivateCurve;
			if (PrevCurve && NewCurve)
			{
				float MinTimePrev, MaxTimePrev, MinTimeNew, MaxTimeNew;
				PrevCurve->GetTimeRange(MinTimePrev, MaxTimePrev);
				NewCurve->GetTimeRange(MinTimeNew, MaxTimeNew);

				float PrevRange = MaxTimePrev - MinTimePrev;
				float NewRange = MaxTimeNew - MinTimeNew;

				Reactive = IsActive && (CurveTime / PrevRange < ReactiveThreshold);
				//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("Threshold needs to be above %f"), CurveTime / PrevRange));

				CurveTime = NewRange - (NewRange * (CurveTime / PrevRange));
			}
			else SetActiveDirect(IsActive);
		}
		else
		{
			Reactive = false;
			CurveTime = 0.f;
			IsRamping = true;
		}
	}
}

void UEnergyConsumer::SetActiveDirect(bool NewActive)
{
	IsActive = NewActive;
	IsRamping = false;
	UpdateEnergyLevel(IsActive ? 1.f : 0.f);
}

void UEnergyConsumer::UpdateEnergyLevel(float NewLevel)
{
	EnergyLevel = NewLevel;
	OnUpdateEnergyLevel.Broadcast(NewLevel);
}

void UEnergyConsumer::Tick(float DeltaTime)
{
	if (IsRamping)
	{
		CurveTime += DeltaTime;

		float MinTime, MaxTime;

		UCurveFloat* UsingCurve = IsActive ? (Reactive ? ReactivateCurve : ActivateCurve) : DeactivateCurve;

		if (!UsingCurve)
		{
			UE_LOG(LogTemp, Warning, TEXT("Missing curves for EnergyConsumer"));
			SetActiveDirect(IsActive);
			if (IsActive)
				EnergySource->ConsumeBattery(ActiveUsage*DeltaTime);
		}
		else
		{
			UsingCurve->GetTimeRange(MinTime, MaxTime);

			if (CurveTime > MaxTime - MinTime)
			{
				SetActiveDirect(IsActive);
				if (IsActive)
					EnergySource->ConsumeBattery(ActiveUsage*DeltaTime);
			}
			else
			{
				EnergySource->ConsumeBattery(UsingCurve->GetFloatValue(MinTime + CurveTime)*ActiveUsage*DeltaTime);
				UpdateEnergyLevel(UsingCurve->GetFloatValue(MinTime + CurveTime));
			}
		}
	}
	else
		EnergySource->ConsumeBattery(ActiveUsage*DeltaTime);
}

bool UEnergyConsumer::IsTickable() const
{
	return IsActive || IsRamping;
}

TStatId UEnergyConsumer::GetStatId() const
{
	return Super::GetStatID();
}

void UEnergyConsumer::SetShip(AShip * Ship)
{
	EnergySource = Ship;
}

float UEnergyConsumer::GetEnergyLevel()
{
	return EnergyLevel;
}

bool UEnergyConsumer::GetActive()
{
	return IsActive;
}
