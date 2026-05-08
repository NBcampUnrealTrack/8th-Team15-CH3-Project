// MainGameInstance.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MainGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJECT_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	float PlayerCurrentHP;

	UPROPERTY()
	float PlayerCurrentATK;

public:
	UFUNCTION()
	void SavePlayerStatus(AActor* Player);

	UFUNCTION()
	void SetPlayerStatus(AActor* Player);
};
