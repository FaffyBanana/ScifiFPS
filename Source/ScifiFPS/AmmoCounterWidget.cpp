// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoCounterWidget.h"
#include "Kismet/KismetTextLibrary.h"

void UAmmoCounterWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AScifiFPSCharacter* player = Cast<AScifiFPSCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	if(player)
	{
		Player = player;
	}
}

FText UAmmoCounterWidget::GetAmmoText() const
{
	return UKismetTextLibrary::Conv_IntToText(Player->GetWeaponComponent()->GetCurrentAmmo());
}

FText UAmmoCounterWidget::GetTotalAmmoText() const
{
	return UKismetTextLibrary::Conv_IntToText(Player->GetWeaponComponent()->GetTotalCurrentAmmo());
}
