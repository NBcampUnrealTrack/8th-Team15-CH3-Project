#include "LJW/Character/PrototypeXCharacter.h"
#include "TeamProject/LJW/GameUtilHeader/GameUtil.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "EnhancedInputComponent.h"
#include "LJW/Controller/PrototypeXPlayerController.h"

#include "Kismet/KismetMathLibrary.h"

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
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 300.f;
	SpringArmComponent->bUsePawnControlRotation = true; // 컨트롤러 회전 사용
	SpringArmComponent->SetUsingAbsoluteRotation(true); // 캐릭터 회전에 카메라 축이 뒤틀리지 않게 고정

	// CameraComponent의 bUsePawnControlRotation은 반드시 false (SpringArm이 회전을 주도함)
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;

	Inter_LookAmountX = 0;
	Inter_LookAmountY = 0;
	Inter_FinalX = 0;
	Inter_FinalY = 0;
}

void APrototypeXCharacter::BeginPlay()
{
	Super::BeginPlay();
	SetPlayerMode(EPlayerMode::Normal);
}

void APrototypeXCharacter::ItemUse_Start(const FInputActionValue& value)
{
	if (IsRollingMontagePlaying || bIsAttacking || bIsOnJumpping) return;

	UAnimInstance* Animbackground = GetMesh()->GetAnimInstance();
	if (!ensureMsgf(Animbackground, TEXT("Invalid AnimInstance"))) return;

	if (!ensureMsgf(ItemUseMontage[0], TEXT("Invalid UseItemMontage"))) return;
	// change item?
	UStaticMeshComponent* SwordComp = nullptr;
	TArray<UStaticMeshComponent*> MeshComp;
	GetComponents<UStaticMeshComponent>(MeshComp);
	for (UStaticMeshComponent* Meshs : MeshComp)
	{
		if (Meshs->GetName() == TEXT("Sword"))
		{
			SwordComp = Meshs;
			break;
		}
	}
	if (ensureMsgf(SwordComp, TEXT("SwordSocket is Invalid")))
	{

		// item load
		//
		// 
		// 
		// SwordComp->SetHiddenInGame(true);
		// 
		// =============
		Animbackground->Montage_Play(ItemUseMontage[0]);
		IsItemUsing = true;
	}
	else
	{
		return;
	}
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
	
				EnhancedInput->BindAction(
					PlayerController->IA_Sprint,
					ETriggerEvent::Triggered,
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
					ETriggerEvent::Triggered,
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

			if (PlayerController->IA_ItemUse)
			{
				EnhancedInput->BindAction(
					PlayerController->IA_ItemUse,
					ETriggerEvent::Triggered,
					this,
					&APrototypeXCharacter::ItemUse_Start
				);
			}
		}
	}
}

void APrototypeXCharacter::Move_Start(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("Moving... IsAttacking: %s"), bIsAttacking ? TEXT("TRUE") : TEXT("FALSE"));

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
			AddControllerPitchInput(-LookAmount.Y);
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
}

void APrototypeXCharacter::Sprint_Stop(const FInputActionValue& value)
{
	GetCharacterMovement()->MaxWalkSpeed = Normal_Speed;
}

void APrototypeXCharacter::Jump_Start(const FInputActionValue& value)
{
	if (IsRollingMontagePlaying) return;
	UE_LOG(LogTemp, Warning, TEXT("Jumping"));
	bIsOnJumpping = true;
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
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // 회전 속도
	GetCharacterMovement()->MaxAcceleration = 1200.f;

	// 스프링암 설정 (카메라만 컨트롤러 회전을 따름)
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->bInheritPitch = true;
	SpringArmComponent->bInheritYaw = true;
	SpringArmComponent->bInheritRoll = false;

	SpringArmComponent->bEnableCameraRotationLag = true;
	SpringArmComponent->CameraRotationLagSpeed = 10.f;

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
	GetCharacterMovement()->RotationRate = FRotator(0.f, 740.f, 0.f); // 회전 속도
	GetCharacterMovement()->MaxAcceleration = 1200.f;

	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->bInheritPitch = true;
	SpringArmComponent->bInheritYaw = true;
	SpringArmComponent->bInheritRoll = false;

	SpringArmComponent->bEnableCameraLag = false;
	SpringArmComponent->CameraLagSpeed = 8.f;
	//============================================================================================
	MouseSensibiliy = 0.5f;
	Normal_Speed = 450.f;
	Sprint_Speed = 700.f;
	GetCharacterMovement()->MaxWalkSpeed = Normal_Speed;

	GetCharacterMovement()->JumpZVelocity = Normal_Jump_Speed;
}