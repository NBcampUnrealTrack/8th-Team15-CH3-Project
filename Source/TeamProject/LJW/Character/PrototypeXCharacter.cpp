#include "LJW/Character/PrototypeXCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "EnhancedInputComponent.h"
#include "LJW/Controller/PrototypeXPlayerController.h"

#include "Kismet/KismetMathLibrary.h"

#include "Combat/StatusComponent.h"
#include "LJW/Item/ItemDatatable.h"
#include "MainGameInstance.h"
APrototypeXCharacter::APrototypeXCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	//GetCapsuleComponent()->SetRelativeRotation(FRotator(0.f, 45.f, 0.f));
	FVector PivotLocation = FVector(0.f, 0.f, -50.f);
	FRotator PivotRotation = FRotator(0.f, -90.f, 0.f);
	UCapsuleComponent = GetCapsuleComponent();
	SkeletalMeshComponent = GetMesh();
	SkeletalMeshComponent->SetRelativeRotation(PivotRotation);
	SkeletalMeshComponent->SetRelativeLocation(PivotLocation);

	// 계층 구조 설정: SpringArm을 Root에 부착하고 Absolute Rotation 설정
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent_0"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 300.f;
	//SpringArmComponent->bUsePawnControlRotation = true; // 컨트롤러 회전 사용
	 // 캐릭터 회전에 카메라 축이 뒤틀리지 않게 고정

	// CameraComponent의 bUsePawnControlRotation은 반드시 false (SpringArm이 회전을 주도함)
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent_0"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);

	Inter_LookAmountX = 0;
	Inter_LookAmountY = 0;
	Inter_FinalX = 0;
	Inter_FinalY = 0;
}

void APrototypeXCharacter::BeginPlay()
{
	Super::BeginPlay();
	SetPlayerMode(EPlayerMode::Normal);

	StatusComponent = FindComponentByClass<UStatusComponent>();
	UE_LOG(LogTemp, Log, TEXT("%s"), *StatusComponent->GetName());
	checkf(StatusComponent, TEXT("Must have StatusComponents"));
}

//void APrototypeXCharacter::ItemUse_Start(const FInputActionValue& value)
//{
//	if (IsRollingMontagePlaying || bIsAttacking || bIsOnJumpping || IsItemUsing) return;
//
//	UAnimInstance* Animbackground = GetMesh()->GetAnimInstance();
//	if (!ensureMsgf(Animbackground, TEXT("Invalid AnimInstance"))) return;
//
//	if (!ensureMsgf(ItemUseMontage[0], TEXT("Invalid UseItemMontage"))) return;
//	// change item?
//	UStaticMeshComponent* SwordComp = nullptr;
//	TArray<UStaticMeshComponent*> MeshComp;
//	GetComponents<UStaticMeshComponent>(MeshComp);
//	for (UStaticMeshComponent* Meshs : MeshComp)
//	{
//		if (Meshs->GetName() == TEXT("SwordComponent"))
//		{
//			SwordComp = Meshs;
//			break;
//		}
//	}
//	if (ensureMsgf(SwordComp, TEXT("SwordSocket is Invalid")))
//	{
//		// =============
//		SwordComp->SetHiddenInGame(true);
//		//static ConstructorHelpers::FObjectFinder<UStaticMesh> Potion
//		//SwordComp->SetStaticMesh(UStaticMesh* StaticMesh);
//		Animbackground->Montage_Play(ItemUseMontage[0]);
//		IsItemUsing = true;
//
//		FOnMontageEnded EndDelegate;
//		EndDelegate.BindUObject(this, &APrototypeXCharacter::ItemUse_End, SwordComp);
//		Animbackground->Montage_SetEndDelegate(EndDelegate, ItemUseMontage[0]);
//	}
//	else
//	{
//		return;
//	}
//}

bool APrototypeXCharacter::ItemUse_MontagePlay(FName GetItemID)
{
	if (IsRollingMontagePlaying || bIsAttacking || bIsOnJumpping || IsItemUsing)
	{
		// no mongtage play;
		return false;
	}
	else
	{
		UAnimInstance* Animbackground = GetMesh()->GetAnimInstance();
		if (!ensureMsgf(Animbackground, TEXT("Invalid AnimInstance"))) return false;

		if (UMainGameInstance* GameInstance = Cast<UMainGameInstance>(GetGameInstance()))
		{
			FString DebugString(TEXT("Montage is Invalid When PlayingMontage On Player"));
			if (FItemDatatable* FoundRow = GameInstance->ItemDataTable->FindRow<FItemDatatable>(GetItemID, DebugString))
			{
				if (UAnimMontage* PlayMontage = FoundRow->UseItemAnim.LoadSynchronous())
				{
					Animbackground->Montage_Play(PlayMontage);
				}
			}
		}
		return true;
	}
}

void APrototypeXCharacter::ItemUse_End(UAnimMontage*, bool bInterrupted, UStaticMeshComponent* GetSwordComp)
{
	IsItemUsing = false;
	GetSwordComp->SetHiddenInGame(false);
}

void APrototypeXCharacter::ApplyRollingAtMode(EPlayerMode InMode)
{
	if (IsRollingMontagePlaying || bIsOnJumpping) return;
	UAnimInstance* Animbackground = GetMesh()->GetAnimInstance();

	SetPlayerMode(EPlayerMode::Normal);
	if (RollMontage)
	{
		if (Animbackground)
		{
			// 누르고있는 방향키의 방향대로 setrot한다음 play해야하나...
			BeforePlayerRot = GetActorRotation();

			FRotator ControlRot = GetControlRotation();
			FRotator YawRotation(0.f, ControlRot.Yaw, 0.f);// 컨트롤의 Yaw회전(Z축)만 가져옴

			FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			// 컨트롤러의 Yaw회전이 적용된Z축기준 기즈모를 다시만들고 거기서 정면 X축 +값
			FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			// 컨트롤러의 Yaw회전이 적용된Z축기준 기즈모를 다시만들고 거기서 오른쪽 Y축 +값
			FVector DesiredDir = (Forward * NowPlayerDir.X) + (Right * NowPlayerDir.Y);
			// 컨트롤러의 정면방향벡터 * (1, -1)x축인풋입력값, 컨트롤러의 정면방향벡터 * (1, -1)Y축인풋입력값
			// 벡터의 합은 두방향의 중간방향
			if (DesiredDir.IsNearlyZero())
			{
				DesiredDir = GetActorForwardVector();
			}

			SetActorRotation(DesiredDir.Rotation());
			//DesiredDir방향(Rotation) 회전하라(SetActorRotation)

			// ============================ STEMINA =================================
			StatusComponent->ConsumeStamina(13.f);
			// ============================ STEMINA =================================

			Animbackground->Montage_Play(RollMontage);
			IsRollingMontagePlaying = true;
			// 싱글 델리게이트
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &APrototypeXCharacter::RollingMontageEnd, InMode); // 매개변수추가가 가능하다니...
			// EndDelegate가 자신과 함수를 묶었다
			Animbackground->Montage_SetEndDelegate(EndDelegate, RollMontage);
			// RollMontage가 End일때 SetEndDelegate에 EndDelegate를 등록(set)
			// endmontage일때 함수바인딩 call
		}
	}
}

void APrototypeXCharacter::RollingMontageEnd(UAnimMontage* Montage, bool bInterrupted, EPlayerMode InMode)
{//DECLARE_DELEGATE_TwoParams(FOnMontageEnded, UAnimMontage*, bool /*bInterrupted*/)
	//FOnMontageEnded에서 F12를 눌러 확인할수있음
	IsRollingMontagePlaying = false;

	UE_LOG(LogTemp, Warning, TEXT("Montage Ended: %s"), *Montage->GetName());

	SetPlayerMode(InMode);
}

void APrototypeXCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (OnLagSpeed)
	{
		SpringArmComponent->CameraLagSpeed = FMath::FInterpTo(
			SpringArmComponent->CameraLagSpeed,
			TargetLagSpeed,
			DeltaTime,
			12.0f 
		);

		if (SpringArmComponent->CameraLagSpeed > 49.f)
		{
			SpringArmComponent->bEnableCameraLag = false;
			OnLagSpeed = false;
		}
	}



}

void APrototypeXCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput =
		Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (APrototypeXPlayerController* PlayerController =
			Cast<APrototypeXPlayerController>(GetController()))
		{
			if (PlayerController->IA_Move)
			{
				EnhancedInput->BindAction(
					PlayerController->IA_Move,
					ETriggerEvent::Triggered,
					this,
					&APrototypeXCharacter::Move_Start
				);

				EnhancedInput->BindAction(
					PlayerController->IA_Move,
					ETriggerEvent::Completed,
					this,
					&APrototypeXCharacter::Move_Stop
				);
			}

			if (PlayerController->IA_Look)
			{
				EnhancedInput->BindAction(
					PlayerController->IA_Look,
					ETriggerEvent::Triggered,
					this,
					&APrototypeXCharacter::Look
				);
			}

			if (PlayerController->IA_Roll)
			{
				EnhancedInput->BindAction(
					PlayerController->IA_Roll,
					ETriggerEvent::Triggered,
					this,
					&APrototypeXCharacter::Roll_Start
				);

				EnhancedInput->BindAction(
					PlayerController->IA_Roll,
					ETriggerEvent::Completed,
					this,
					&APrototypeXCharacter::Roll_Stop
				);
			}

			if (PlayerController->IA_Sprint)
			{
				EnhancedInput->BindAction(
					PlayerController->IA_Sprint,
					ETriggerEvent::Started,
					this,
					&APrototypeXCharacter::Sprint_Start
				);

				EnhancedInput->BindAction(
					PlayerController->IA_Sprint,
					ETriggerEvent::Completed,
					this,
					&APrototypeXCharacter::Sprint_Stop
				);
			}

			if (PlayerController->IA_Jump)
			{
				EnhancedInput->BindAction(
					PlayerController->IA_Jump,
					ETriggerEvent::Started,
					this,
					&APrototypeXCharacter::Jump_Start
				);

				EnhancedInput->BindAction(
					PlayerController->IA_Jump,
					ETriggerEvent::Completed,
					this,
					&APrototypeXCharacter::Jump_Stop
				);

			}

			if (PlayerController->IA_Defence)
			{
				EnhancedInput->BindAction(
					PlayerController->IA_Defence,
					ETriggerEvent::Started,
					this,
					&APrototypeXCharacter::Defence_Start
				);
			}

			//if (PlayerController->IA_ItemUse)
			//{
			//	EnhancedInput->BindAction(
			//		PlayerController->IA_ItemUse,
			//		ETriggerEvent::Triggered,
			//		this,
			//		&APrototypeXCharacter::ItemUse_Start
			//	);
			//}
		}
	}
}

void APrototypeXCharacter::Move_Start(const FInputActionValue& value)
{
	if (!Controller) return;

	NowPlayerDir = value.Get<FVector2D>();
	const FVector2D MoveAmount = value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0); // 컨트롤러의 Yaw(Z축) 추출

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	// 컨트롤러의 틀어진Yaw축에서 XYZ기즈모를 얻고 X축의 기즈모를 얻음(방향벡터라 크기1)
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	// 컨트롤러의 틀어진Yaw축에서 XYZ기즈모를 얻고 Y축의 기즈모를 얻음(방향벡터라 크기1)

	if (!FMath::IsNearlyZero(MoveAmount.X))
	{
		AddMovementInput(ForwardDirection, MoveAmount.X);
	}
	if (!FMath::IsNearlyZero(MoveAmount.Y))
	{
		AddMovementInput(RightDirection, MoveAmount.Y);
	}
}

void APrototypeXCharacter::Move_Stop(const FInputActionValue& value)
{
}

void APrototypeXCharacter::Look(const FInputActionValue& value)
{
	const FVector2D LookAmount = value.Get<FVector2D>();

	switch (CurrentMode)
	{
	case EPlayerMode::Normal:
		if (!FMath::IsNearlyZero(LookAmount.X))
		{
			AddControllerYawInput(LookAmount.X);
		}
		if (!FMath::IsNearlyZero(LookAmount.Y))
		{
			AddControllerPitchInput(LookAmount.Y);
		}
		break;
	case EPlayerMode::Attack:
		break;
	}
}

void APrototypeXCharacter::Inter_Look(float DeltaTime)
{
	// 수동 Relative Rotation 조절은 카메라 꼬임의 원인이 되므로 소울류 모드에서는 비워둡니다.
}

void APrototypeXCharacter::Roll_Start(const FInputActionValue& value)
{
	ApplyRollingAtMode(CurrentMode);
	UE_LOG(LogTemp, Warning, TEXT("Roll"));
}

void APrototypeXCharacter::Roll_Stop(const FInputActionValue& value)
{
}

void APrototypeXCharacter::Sprint_Start(const FInputActionValue& value)
{
	GetCharacterMovement()->MaxWalkSpeed = Sprint_Speed;
	SpringArmComponent->bEnableCameraLag = true;
	TargetLagSpeed = 10.f;
	OnLagSpeed = true;

	// ========================== STEMINA ==============================
	GetWorldTimerManager().SetTimer(
		RunningTimeCheck,
		[this]()
		{
			if (StatusComponent)
			{
				StatusComponent->ConsumeStamina(2.f);
			}
		},
		1.0f,
		true
	);
	// ========================== STEMINA ==============================

}

void APrototypeXCharacter::Sprint_Stop(const FInputActionValue& value)
{
	GetCharacterMovement()->MaxWalkSpeed = Normal_Speed;
	TargetLagSpeed = 50.f;

	// ========================== STEMINA ==============================
	GetWorldTimerManager().ClearTimer(RunningTimeCheck);
	// ========================== STEMINA ==============================
}

void APrototypeXCharacter::Jump_Start(const FInputActionValue& value)
{
	if (IsRollingMontagePlaying || bIsAttacking || bIsOnDefencing) return;
	UE_LOG(LogTemp, Warning, TEXT("Jumping"));
	bIsOnJumpping = true;
	// ======================== Stemina =============================
	StatusComponent->ConsumeStamina(10.f);
	// ======================== Stemina =============================
	Jump();
}

void APrototypeXCharacter::Landed(const FHitResult& Hit)
{
	bIsOnJumpping = false;
	StopJumping();
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->SetRootMotionMode(ERootMotionMode::RootMotionFromMontagesOnly);
	}
}

void APrototypeXCharacter::Jump_Stop(const FInputActionValue& value)
{
}

void APrototypeXCharacter::Defence_Start(const FInputActionValue& value)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!ensureMsgf(AnimInstance, TEXT("None AnimInstance"))) return;
	if (bIsOnDefencing || IsRollingMontagePlaying || bIsOnJumpping) return;
	// =========================== STEMINA ==============================
	StatusComponent->ConsumeStamina(15.f);
	// =========================== STEMINA ==============================
	bIsOnDefencing = true;
	AnimInstance->Montage_Play(DefenceMontage);
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &APrototypeXCharacter::Defence_Ended);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, DefenceMontage);
}

void APrototypeXCharacter::Defence_Ended(UAnimMontage* Montage, bool bInterrupted)
{
	bIsOnDefencing = false;
}

void APrototypeXCharacter::SetPlayerMode(EPlayerMode NewMode)
{
	CurrentMode = NewMode;

	switch (NewMode)
	{
	case EPlayerMode::Normal:
		ApplyNormalModeSettings();
		break;
	case EPlayerMode::Attack:
		ApplyAttackModeSettings();
		break;
	}
}

void APrototypeXCharacter::ApplyNormalModeSettings()
{
	bUseControllerRotationYaw = false; // = 컨트롤러의 회전을 캐릭터가 상속을 하겠느냐
	bUseControllerRotationPitch = false; // = False > 따로따로 회전 적용
	bUseControllerRotationRoll = false;

	// 이동 방향으로 캐릭터가 자동으로 회전하도록 설정
	GetCharacterMovement()->bOrientRotationToMovement = true; // 핵심: 입력 방향으로 몸을 돌림
	//GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // 회전 속도
	GetCharacterMovement()->MaxAcceleration = 1200.f;

	// 스프링암 설정 (카메라만 컨트롤러 회전을 따름)
	//SpringArmComponent->SetUsingAbsoluteRotation(true);

	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->bInheritPitch = true;
	SpringArmComponent->bInheritYaw = true;
	SpringArmComponent->bInheritRoll = false;

	//SpringArmComponent->bEnableCameraRotationLag = true;
	//SpringArmComponent->CameraRotationLagSpeed = 10.f;

	CameraComponent->bUsePawnControlRotation = false;
	//============================================================================================
	MouseSensibiliy = 0.5f;
	Normal_Speed = 450.f;
	Sprint_Speed = 700.f;
	GetCharacterMovement()->MaxWalkSpeed = Normal_Speed;

	GetCharacterMovement()->JumpZVelocity = Normal_Jump_Speed;
}

void APrototypeXCharacter::ApplyAttackModeSettings()
{
	// 컨트롤러의 회전을 pawn에 즉시 반영 ( 바로바로 set )
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false; // = False > 따로따로 회전 적용
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	// 컨트롤러의 회전을 finterp로 적용 ( 메쉬가 부드럽게 회전하며 컨트롤러의 정면방향으로 set )
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 900.f, 0.f); // 회전 속도
	GetCharacterMovement()->MaxAcceleration = 1200.f;

	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->bInheritPitch = true;
	SpringArmComponent->bInheritYaw = true;
	SpringArmComponent->bInheritRoll = false;

	SpringArmComponent->bEnableCameraLag = false;
	//SpringArmComponent->CameraLagSpeed = 8.f;
	//============================================================================================
	MouseSensibiliy = 0.5f;
	Normal_Speed = 450.f;
	Sprint_Speed = 700.f;
	GetCharacterMovement()->MaxWalkSpeed = Normal_Speed;

	GetCharacterMovement()->JumpZVelocity = Normal_Jump_Speed;
}