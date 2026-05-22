#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "LevelSquenceTriggerBox.generated.h"

class APrototypeXCharacter;

UCLASS()
class TEAMPROJECT_API ALevelSquenceTriggerBox : public ATriggerBox
{
	GENERATED_BODY()
public:
    ALevelSquenceTriggerBox();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UBoxComponent* TriggerVolume;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    UFUNCTION()
	void OnSequenceFinished();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinematic")
    class ALevelSequenceActor* TargetSequenceActor;

    UPROPERTY()
	class APrototypeXCharacter* SavePlayerCharacter;
    UPROPERTY()
	class AController* SavePlayerController;

};
