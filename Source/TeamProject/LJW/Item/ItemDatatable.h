#pragma once

#include "CoreMinimal.h"
#include "ItemDatatable.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	POTION,
	EQUIPMENT
};

class UStaticMesh;
class UNiagaraSystem;
class UTexture2D;
class USoundBase;
class UAminMontage;

USTRUCT(BlueprintType)
struct FItemDatatable: public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	FName ItemID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	float Amount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	EItemType ItemType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	bool MagneticOnOff;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	TSoftObjectPtr<UStaticMesh> ItemMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	TSoftObjectPtr<UNiagaraSystem> ItemEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	TSoftObjectPtr<UTexture2D> ItemIcon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	TSoftObjectPtr<USoundBase> ItemSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TSoftObjectPtr<UAnimMontage> UseItemAnim;
};
