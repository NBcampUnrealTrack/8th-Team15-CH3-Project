#include "LJW/Item/LevelSquenceTriggerBox.h"
#include "Components/ShapeComponent.h"
#include "LJW/Character/PrototypeXCharacter.h"
#include "GameFramework/Controller.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/UserWidget.h"
ALevelSquenceTriggerBox::ALevelSquenceTriggerBox()
{
    UShapeComponent* TriggerShape = GetCollisionComponent();
    if (TriggerShape)
    {
        TriggerShape->SetCollisionProfileName(TEXT("Trigger"));
        TriggerShape->OnComponentBeginOverlap.AddDynamic(this, &ALevelSquenceTriggerBox::OnOverlapBegin);
    }
}

void ALevelSquenceTriggerBox::OnOverlapBegin(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (APrototypeXCharacter* PlayerCharacter = Cast<APrototypeXCharacter>(OtherActor))
    {

        SavePlayerCharacter = PlayerCharacter;
        SavePlayerController = PlayerCharacter->GetController();
        APlayerController* PC = Cast<APlayerController>(SavePlayerCharacter);

        if (SavePlayerController)
        {
            PlayerCharacter->DisableInput(PC);

            TArray<UUserWidget*> FoundWidgets;
            UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, UUserWidget::StaticClass(), false);
            for (UUserWidget* Widget : FoundWidgets)
            {
                Widget->SetVisibility(ESlateVisibility::Hidden);
            }
		}

        if (TargetSequenceActor)
        {
            if (ULevelSequencePlayer* SequencePlayer = TargetSequenceActor->GetSequencePlayer())
            {
                SequencePlayer->Play();
                SequencePlayer->OnFinished.AddDynamic(this, &ALevelSquenceTriggerBox::OnSequenceFinished);
			}
        }
    }
}

void ALevelSquenceTriggerBox::OnSequenceFinished()
{
    if (SavePlayerController && SavePlayerCharacter)
    {
        APlayerController* PC = Cast<APlayerController>(SavePlayerCharacter);

        if (SavePlayerController)
        {
            SavePlayerCharacter->EnableInput(PC);

            TArray<UUserWidget*> FoundWidgets;
            UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, UUserWidget::StaticClass(), false);
            for (UUserWidget* Widget : FoundWidgets)
            {
                Widget->SetVisibility(ESlateVisibility::Visible);
            }
        }
	}

    UShapeComponent* TriggerShape = GetCollisionComponent();
    TriggerShape->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Destroy();
}