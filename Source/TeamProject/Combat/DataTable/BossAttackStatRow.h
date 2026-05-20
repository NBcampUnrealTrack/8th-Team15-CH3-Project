// BossAttackStatRow.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "BossAttackStatRow.generated.h"

USTRUCT(BlueprintType)
struct FBossAttackStatRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BossRadialAttack")
	float BossRadialAttackFirstRadius = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BossRadialAttack")
	float BossRadialAttackMaxRadius = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BossRadialAttack")
	float BossRadialAttackExpandSpeed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BossRadialAttack")
	float BossRadialAttackDamageMutiplier = 0.0f;
};