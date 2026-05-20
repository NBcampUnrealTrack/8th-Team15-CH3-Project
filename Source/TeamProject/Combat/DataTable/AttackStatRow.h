// AttackStatRow.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "AttackStatRow.generated.h"

USTRUCT(BlueprintType)
struct FAttackStatRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ParryRange = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ParryDamageMultiplier = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HitStopDelayTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HitSlowDelayTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HitSlowPlayerTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HitSlowMobTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ParryDotThreshold = 0.0f;
};