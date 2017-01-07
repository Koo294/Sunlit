// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "ShipHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class SUNLIT_API UShipHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
private:

	TArray<class UShipTargetWidget*> ShipTargetWidgets;
	TArray<class UCanvasPanelSlot*> ShipTargetSlots;
	
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Widgets)
	TSubclassOf<class UShipTargetWidget> ShipTargetWidgetType;

	void AddShipTarget(FString TargetName);
	void MoveShipTarget(int32 Index, FVector2D TargetLoc, bool ClampToView);
	class UShipTargetWidget* GetShipTarget(int32 Index);
	void RemoveShipTarget(int32 Index);

	UFUNCTION(BlueprintImplementableEvent, Category = Battery)
	void SetBatteryCharge(float BatteryCharge);
	
};
