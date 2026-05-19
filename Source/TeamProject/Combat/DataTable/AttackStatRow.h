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
	float ParryRange;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ParryDamageMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HitStopDelayTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HitSlowDelayTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HitSlowPlayerTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HitSlowMobTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ParryDotThreshold;
};