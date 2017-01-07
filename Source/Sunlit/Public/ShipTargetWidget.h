// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "ShipTargetWidget.generated.h"

/**
 * 
 */
UCLASS()
class SUNLIT_API UShipTargetWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintImplementableEvent, Category = TargetInfo)
	void SetArrowDir(float Dir);

	UFUNCTION(BlueprintImplementableEvent, Category = TargetInfo)
	void SetTargetInView(bool InView);

	UFUNCTION(BlueprintImplementableEvent, Category = TargetInfo)
	void SetTargetName(const FString& Name);

};
