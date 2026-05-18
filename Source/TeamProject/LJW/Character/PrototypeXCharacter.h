#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PrototypeXCharacter.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;

struct FInputActionValue;

class UAnimMontage;

class UStatusComponent;

UENUM(BlueprintType)
enum class EPlayerMode : uint8
{
	Normal, Attack
};

UCLASS()
class TEAMPROJECT_API APrototypeXCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APrototypeXCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UCapsuleComponent> UCapsuleComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UCameraComponent> CameraComponent;
	// Mouse ==========================
	float Inter_LookAmountX;
	float Inter_LookAmountY;
	float Inter_FinalX;
	float Inter_FinalY;

	float MouseSensibiliy;
	// ================================
	UPROPERTY()
	FTimerHandle RunningTimeCheck;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Velocitys|Speed")
	float Normal_Speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Velocitys|Speed")
	float Sprint_Speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Velocitys|Jump")
	float Normal_Jump_Speed = 400.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Velocitys|Jump")
	float Max_Jump_Speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Velocitys|Jump")
	float Min_Jump_Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "States|Jump")
	bool bIsOnJumpping = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "States|Attack")
	bool bIsAttacking = false;

	float TargetLagSpeed = 10.f;
	bool OnLagSpeed = false;

	// ==================================

	UPROPERTY()
	UStatusComponent* StatusComponent;
protected:
	virtual void BeginPlay() override;
	void Move_Start(const FInputActionValue& value);
	void Move_Stop(const FInputActionValue& value);
	void Look(const FInputActionValue& value);
	void Inter_Look(float DeltaTime);
	void Roll_Start(const FInputActionValue& value);
	void Roll_Stop(const FInputActionValue& value);
	void Sprint_Start(const FInputActionValue& value);
	void Sprint_Stop(const FInputActionValue& value);

	void Jump_Start(const FInputActionValue& value);
	virtual void Landed(const FHitResult& Hit) override;
	void Jump_Stop(const FInputActionValue& value);

	void Defence_Start(const FInputActionValue& value);
	void Defence_Ended(UAnimMontage* Montage, bool bInterrupted);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "States|Defence")
	UAnimMontage* DefenceMontage;
	bool bIsOnDefencing = false;
	//void Defence_Stop(const FInputActionValue& value);

	// enum =======================
	UFUNCTION(BlueprintCallable, Category = "Mode")
	void SetPlayerMode(EPlayerMode NewMode);
	void ApplyNormalModeSettings();
	void ApplyAttackModeSettings();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mode")
	EPlayerMode CurrentMode;
	// ============================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "States|Roll")
	UAnimMontage* RollMontage;
	void ApplyRollingAtMode(EPlayerMode InMode);


	UFUNCTION()
	void RollingMontageEnd(UAnimMontage* Montage, bool bInterrupted, EPlayerMode InMode);
	UPROPERTY()
	FVector2D NowPlayerDir = FVector2D::ZeroVector;
	FRotator BeforePlayerRot;
	//bool BeforeSetModeRInter = false;
	//EPlayerMode BeforeSetMode;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "States|Roll")
	AActor* TargetLockActor;
	//===============================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "States|Use")
	TArray<UAnimMontage*> ItemUseMontage;

	//void ItemUse_Start(const FInputActionValue& value);
	void ItemUse_End(UAnimMontage*, bool bInterrupted, UStaticMeshComponent* GetSwordComp);
	bool IsItemUsing = false;

	//================================
public:	
	bool ItemUse_MontagePlay(FName GetItemID);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "States|Roll")
	bool IsRollingMontagePlaying = false;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
