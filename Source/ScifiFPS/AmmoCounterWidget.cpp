#include "AmmoCounterWidget.h"
#include "Kismet/KismetTextLibrary.h"

void UAmmoCounterWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AScifiFPSCharacter* player = Cast<AScifiFPSCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	if(player)
	{
		PlayerCharacter = player;
	}
}

FText UAmmoCounterWidget::GetAmmoText() const
{
	return UKismetTextLibrary::Conv_IntToText(PlayerCharacter->GetWeaponComponent()->GetCurrentAmmoOfCurrentWeapon());
}

FText UAmmoCounterWidget::GetTotalAmmoText() const
{
	return UKismetTextLibrary::Conv_IntToText(PlayerCharacter->GetWeaponComponent()->GetReserveAmmoOfCurrentWeapon());
}
