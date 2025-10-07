// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Inventory.Add(Icon1);
	Inventory.Add(Icon2);
	Inventory.Add(Icon3);
	Inventory.Add(Icon4);
	Numbers.Add(Num1);
	Numbers.Add(Num2);
	Numbers.Add(Num3);
	Numbers.Add(Num4);
	Highlights.Add(Highlight1);
	Highlights.Add(Highlight2);
	Highlights.Add(Highlight3);
	Highlights.Add(Highlight4);
}

void UInventoryWidget::SetSlot(int SlotNum, UTexture2D* Icon, int Number)
{
	UImage* SelectedIcon = Inventory[SlotNum];
	if (SelectedIcon)
	{
		SelectedIcon->SetBrushFromTexture(Icon);
		SetCount(Number, SlotNum);
	}
}

void UInventoryWidget::SetSlot(int SlotNum, UTexture2D* Icon)
{
	UImage* SelectedIcon = Inventory[SlotNum];
	if (SelectedIcon)
	{
		SelectedIcon->SetBrushFromTexture(Icon);
	}
}

void UInventoryWidget::SetEquipped(int SlotNum)
{
	int OldSlot = SlotNum - 1;
	if (SlotNum == 0)
	{
		OldSlot = 3;
	}
	Highlights[OldSlot]->SetVisibility(ESlateVisibility::Hidden);
	Highlights[SlotNum]->SetVisibility(ESlateVisibility::Visible);
}

void UInventoryWidget::SetCount(int32 Count, int SlotNum)
{
	if (Count == 0)
	{
		Numbers[SlotNum]->SetText(FText::GetEmpty());

	}else
	{
		Numbers[SlotNum]->SetText(FText::AsNumber(Count));
	}
}
