// CharacterStatRow.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CharacterStatRow.generated.h"

USTRUCT(BlueprintType)
struct FCharacterStatRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RegenStaminaValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StaminaRegenDelayTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ATK;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DEF;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Absorption;

};