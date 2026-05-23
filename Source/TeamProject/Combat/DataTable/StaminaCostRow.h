// StaminaCostRow.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "StaminaCostRow.generated.h"

USTRUCT(BlueprintType)
struct FStaminaCostRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RollStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SprintStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float JumpStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ParryStamina;
};