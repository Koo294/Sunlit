// Fill out your copyright notice in the Description page of Project Settings.

#include "Sunlit.h"
#include "ShipTargetWidget.h"
#include "CanvasPanel.h"
#include "CanvasPanelSlot.h"
#include "Runtime/Engine/Classes/Engine/UserInterfaceSettings.h"
#include "ShipHUDWidget.h"

void UShipHUDWidget::AddShipTarget(FString TargetName)
{
	if (ShipTargetWidgetType)
	{
		UShipTargetWidget* ShipTarget = CreateWidget<UShipTargetWidget>(GetOwningPlayer(), ShipTargetWidgetType);
		if (ShipTarget)
		{
			UCanvasPanel* RootCanvas = Cast<UCanvasPanel>(GetRootWidget());
			if (RootCanvas)
			{
				UCanvasPanelSlot* ShipTargetSlot = RootCanvas->AddChildToCanvas(ShipTarget);
				ShipTarget->SetTargetName(TargetName);
				ShipTarget->ForceLayoutPrepass();
				ShipTargetSlot->SetSize(ShipTarget->GetDesiredSize());
				ShipTargetSlots.Add(ShipTargetSlot);
				ShipTargetWidgets.Add(ShipTarget);
			}
		}
	}
}

void UShipHUDWidget::MoveShipTarget(int32 Index, FVector2D TargetLoc, bool ClampToView)
{
	int32 ViewportX, ViewportY;
	GetOwningPlayer()->GetViewportSize(ViewportX, ViewportY);

	float ViewportScale = GetDefault<UUserInterfaceSettings>()->GetDPIScaleBasedOnSize(FIntPoint(ViewportX, ViewportY));
	FVector2D MoveLoc = (TargetLoc / ViewportScale);

	if (ClampToView)
	{
		MoveLoc.X = FMath::Clamp(MoveLoc.X, ShipTargetSlots[Index]->GetSize().X / 2.f, (ViewportX / ViewportScale) - (ShipTargetSlots[Index]->GetSize().X / 2.f));
		MoveLoc.Y = FMath::Clamp(MoveLoc.Y, ShipTargetSlots[Index]->GetSize().Y / 2.f, (ViewportY / ViewportScale) - (ShipTargetSlots[Index]->GetSize().Y / 2.f));
	}

	MoveLoc -= ShipTargetSlots[Index]->GetSize() / 2.f;
	
	ShipTargetSlots[Index]->SetPosition(MoveLoc);
}

UShipTargetWidget* UShipHUDWidget::GetShipTarget(int32 Index)
{
	return ShipTargetWidgets[Index];
}

void UShipHUDWidget::RemoveShipTarget(int32 Index)
{
	UCanvasPanel* RootCanvas = Cast<UCanvasPanel>(GetRootWidget());
	if (RootCanvas)
	{
		//RootCanvas->
		RootCanvas->RemoveChild(ShipTargetWidgets[Index]);
		ShipTargetSlots.RemoveAt(Index);
		ShipTargetWidgets.RemoveAt(Index);
	}
}
