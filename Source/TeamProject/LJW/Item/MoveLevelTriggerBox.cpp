#include "LJW/Item/MoveLevelTriggerBox.h"
#include "Components/ShapeComponent.h"
#include "LJW/Character/PrototypeXCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "LJW/Item/ActorBagComponent.h"
#include "MainGameInstance.h"

//#include "Camera/PlayerCameraManager.h"
#include "LJW/Controller/PrototypeXPlayerController.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "LevelSquenceTriggerBox.h"

AMoveLevelTriggerBox::AMoveLevelTriggerBox()
{
	UShapeComponent* TriggerShape = GetCollisionComponent();
    if (TriggerShape)
    {
        TriggerShape->SetCollisionProfileName(TEXT("Trigger"));
        TriggerShape->OnComponentBeginOverlap.AddDynamic(this, &AMoveLevelTriggerBox::OnOverlapBegin);
    }
}

void AMoveLevelTriggerBox::OnOverlapBegin(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (APrototypeXCharacter* PlayerCharacter = Cast<APrototypeXCharacter>(OtherActor))
    {
        if (PlayerCharacter && TargetSequenceActor && !NextLevel.IsNull())
        {
            if (ULevelSequencePlayer* SequencePlayer = TargetSequenceActor->GetSequencePlayer())
            {
                SequencePlayer->Play();
            }
        }
    }




	//UE_LOG(LogTemp, Error, TEXT("Overlap Begin with %s"), *OtherActor->GetName());
	//APrototypeXCharacter* PlayerCharacter = Cast<APrototypeXCharacter>(OtherActor);
 //   if (PlayerCharacter && !NextLevel.IsNull())
 //   {
 //       // fade start
 //       APlayerController* PC = Cast<APlayerController>(PlayerCharacter->GetController());
 //       if (PC && PC->PlayerCameraManager)
 //       {
 //           PC->PlayerCameraManager->StartCameraFade(0.0f, 1.0f, FadeTime, FLinearColor::Black, false, true);
 //       }
 //       // timer set
 //       GetWorldTimerManager().SetTimer(
 //           SetTimerBeforeChangingLevel,
 //           [this, PlayerCharacter]()
 //           {
 //               if (PlayerCharacter)
 //               {
 //                   if (UActorBagComponent* PlayerBag = PlayerCharacter->FindComponentByClass<UActorBagComponent>())
 //                   {
 //                       UMainGameInstance* GameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
 //                       if (GameInstance)
 //                       {
 //                           GameInstance->SetPlayerInventory(PlayerBag->GetActorBag());
 //                       }
 //                       UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), NextLevel);
 //                   }
 //               }
 //           },
 //           FadeTime,
 //           false
 //       );
	//}
}

void AMoveLevelTriggerBox::NextLevelFunction()
{
    APrototypeXCharacter* PlayerCharacter = Cast<APrototypeXCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    if (PlayerCharacter)
    {

        if (UActorBagComponent* PlayerBag = PlayerCharacter->FindComponentByClass<UActorBagComponent>())
        {
            UMainGameInstance* GameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
            if (GameInstance)
            {
                UE_LOG(LogTemp, Error, TEXT("Saving Player Data before opening level..."));
                GameInstance->SetPlayerInventory(PlayerBag->GetActorBag());
                GameInstance->SetPlayerWeaponBag(PlayerBag->GetActorWeaponBag());
            }
        }
    }
    UE_LOG(LogTemp, Error, TEXT("Sequence triggered OpenLevel!"));
    UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), NextLevel);
}

void AMoveLevelTriggerBox::TurnOffAllWidget()
{
    APrototypeXCharacter* PlayerCharacter = Cast<APrototypeXCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (PlayerCharacter)
    {
        APrototypeXPlayerController* PlayerController = Cast<APrototypeXPlayerController>(PlayerCharacter->GetController());

        TArray<UUserWidget*> FoundWidgets;
        UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, UUserWidget::StaticClass(), false);
        for (UUserWidget* Widget : FoundWidgets)
        {
            Widget->RemoveFromParent();
        }

    }
}