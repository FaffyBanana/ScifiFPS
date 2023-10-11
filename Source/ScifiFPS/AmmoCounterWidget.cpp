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

	if(Player)
	{
		//FText ammo = UKismetTextLibrary::Conv_IntToText(Player->GetWeaponComponent()->GetCurrentAmmo());
		//AmmoText->SetText(ammo);
		// /Script/UMGEditor.WidgetBlueprint'/Game/FirstPerson/Blueprints/Widgets/WBP_PlayerHUD.WBP_PlayerHUD'
	}
}
