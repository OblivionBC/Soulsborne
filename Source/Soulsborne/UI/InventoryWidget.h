// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "InventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULSBORNE_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void SetSlot(int SlotNum, UTexture2D* Icon, int Number);
	void SetSlot(int SlotNum, UTexture2D* Icon);
	void SetEquipped(int SlotNum);
	void SetCount(int32 Count, int SlotNum);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<UImage*> Inventory;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<UTextBlock*> Numbers;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<UImage*> Highlights;
	
	UPROPERTY(meta = (BindWidget))
	UBorder* Slot1;
	UPROPERTY(meta = (BindWidget))
	UImage* Icon1;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Num1;
	UPROPERTY(meta = (BindWidget))
	UImage* Highlight1;

	UPROPERTY(meta = (BindWidget))
	UBorder* Slot2;
	UPROPERTY(meta = (BindWidget))
	UImage* Icon2;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Num2;
	UPROPERTY(meta = (BindWidget))
	UImage* Highlight2;

	UPROPERTY(meta = (BindWidget))
	UBorder* Slot3;
	UPROPERTY(meta = (BindWidget))
	UImage* Icon3;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Num3;
	UPROPERTY(meta = (BindWidget))
	UImage* Highlight3;

	UPROPERTY(meta = (BindWidget))
	UBorder* Slot4;
	UPROPERTY(meta = (BindWidget))
	UImage* Icon4;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Num4;
	UPROPERTY(meta = (BindWidget))
	UImage* Highlight4;
};
