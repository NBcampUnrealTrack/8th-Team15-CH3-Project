#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "MoveLevelTriggerBox.generated.h"

UCLASS()
class TEAMPROJECT_API AMoveLevelTriggerBox : public ATriggerBox
{
    GENERATED_BODY()
public:
    AMoveLevelTriggerBox();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UBoxComponent* TriggerVolume;

    // ?? 에디터에서 다음 레벨을 마음대로 바꿀 수 있도록 노출!
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
    TSoftObjectPtr<UWorld> NextLevel;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    //UPROPERTY()
    //FTimerHandle SetTimerBeforeChangingLevel;
    //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FadeTime")
    //float FadeTime = 3.f;
public:
    UFUNCTION(BlueprintCallable, Category = "Level")
    void NextLevelFunction();
    UFUNCTION(BlueprintCallable, Category = "UI")
    void TurnOffAllWidget();
protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinematic")
    class ALevelSequenceActor* TargetSequenceActor;
};
