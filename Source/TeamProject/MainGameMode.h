// MainGameMode.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MainGameMode.generated.h"

class UStatusComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameOver);

UCLASS()
class TEAMPROJECT_API AMainGameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

private:
	bool bIsGameOver = false;

	UStatusComponent* StatusComp;

public:
	UPROPERTY(BlueprintAssignable)
	FOnGameOver OnGameOver;

public:
	UFUNCTION()
	void SetbIsGameOver();
};
