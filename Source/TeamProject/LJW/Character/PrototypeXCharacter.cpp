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
	SpringArmComponent = GameUtil::CreateComponent<USpringArmComponent>(this);
	SpringArmComponent->TargetArmLength = 300.f;
	CameraComponent = GameUtil::CreateComponent<UCameraComponent>(this);
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

}

void APrototypeXCharacter::ItemUse_Start(const FInputActionValue& value)
{
	// UI�� ���� ���������� �ҷ�����
	// �������� �����̳� >
	// �������� ��ȭ�� >
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
	if (IsRollingMontagePlaying) return;
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

	//if (InMode == EPlayerMode::Attack)
	//{
	//	BeforeSetModeRInter = true;
	//	BeforeSetMode = InMode;
	//}
	//else
	//{
		SetPlayerMode(InMode);
	//}
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
			// change to Roll
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
		//AddMovementInput(GetActorForwardVector(), MoveAmount.X);
		AddMovementInput(ForwardDirection, MoveAmount.X);
	}
	if (!FMath::IsNearlyZero(MoveAmount.Y))
	{
		//AddMovementInput(GetActorRightVector(), MoveAmount.Y);
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
			//Inter_LookAmountX += LookAmount.X;
			AddControllerYawInput(LookAmount.X);
		}
		if (!FMath::IsNearlyZero(LookAmount.Y))
		{
			AddControllerPitchInput(-LookAmount.Y);
			//Inter_LookAmountY += LookAmount.Y;
			//Inter_LookAmountY = FMath::Clamp(Inter_LookAmountY, -80, 60);

		}
		break;
	case EPlayerMode::Attack:
		break;
	}
}

void APrototypeXCharacter::Inter_Look(float DeltaTime)
{
	if (Inter_FinalX != Inter_LookAmountX || Inter_FinalY != Inter_LookAmountY)
	{
		Inter_FinalX = FMath::FInterpTo(Inter_FinalX, Inter_LookAmountX, DeltaTime, 12.f);
		Inter_FinalY = FMath::FInterpTo(Inter_FinalY, Inter_LookAmountY, DeltaTime, 12.f);

		SpringArmComponent->SetRelativeRotation(FRotator(Inter_FinalY, Inter_FinalX, 0.f));
		if (FMath::IsNearlyEqual(Inter_FinalX, Inter_LookAmountX))
		{
			Inter_FinalX = Inter_LookAmountX;
		}
		if (FMath::IsNearlyEqual(Inter_FinalY, Inter_LookAmountY))
		{
			Inter_FinalY = Inter_LookAmountY;
		}
	}
}

void APrototypeXCharacter::Roll_Start(const FInputActionValue& value)
{
	//change to roll
	//bIsOnJumpping = true;
	//Jump();
	ApplyRollingAtMode(CurrentMode);
	UE_LOG(LogTemp, Warning, TEXT("Roll"));
}

//void APrototypeXCharacter::Landed(const FHitResult& Hit)
//{
//	//need to delete
//	Super::Landed(Hit);
//	//bIsOnJumpping = false;
//	GetCharacterMovement()->JumpZVelocity = Normal_Jump_Speed;
//
//}

void APrototypeXCharacter::Roll_Stop(const FInputActionValue& value)
{
	// change to roll
	//StopJumping();
}

void APrototypeXCharacter::Sprint_Start(const FInputActionValue& value)
{
	GetCharacterMovement()->MaxWalkSpeed = Sprint_Speed;
}

void APrototypeXCharacter::Sprint_Stop(const FInputActionValue& value)
{
	GetCharacterMovement()->MaxWalkSpeed = Normal_Speed;
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
	// TODO: bUsePawnControlRotation 값 배치
	// TODO: SpringArm Inherit Pitch/Yaw/Roll 값 배치
	// TODO: CharacterMovement 회전 관련 옵션 배치
			// TODO: 이동모드 설정 적용 함수 호출
	bUseControllerRotationYaw = false; // = 컨트롤러의 회전을 캐릭터가 상속을 하겠느냐
	bUseControllerRotationPitch = false; // = False > 따로따로 회전 적용
	bUseControllerRotationRoll = false;

	// 이동 방향으로 캐릭터가 자동으로 회전하도록 설정
	GetCharacterMovement()->bOrientRotationToMovement = true; // 핵심: 입력 방향으로 몸을 돌림
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // 회전 속도
	GetCharacterMovement()->MaxAcceleration = 1500.f;
	// 스프링암 설정 (카메라만 컨트롤러 회전을 따름)
	SpringArmComponent->bUsePawnControlRotation = false;
	SpringArmComponent->bInheritPitch = true;
	SpringArmComponent->bInheritYaw = true;
	SpringArmComponent->bInheritRoll = false;

	SpringArmComponent->bEnableCameraRotationLag = true;
	SpringArmComponent->CameraRotationLagSpeed = 5.f;
	//============================================================================================
	MouseSensibiliy = 0.5f;
	Normal_Speed = 450.f;
	Sprint_Speed = 900.f;
	GetCharacterMovement()->MaxWalkSpeed = Normal_Speed;

	//Normal_Jump_Speed = 500.f;
	//Max_Jump_Speed = 500.f;
	//Min_Jump_Speed = Normal_Jump_Speed;
	//GetCharacterMovement()->JumpZVelocity = Normal_Jump_Speed;

	// =========================================================================

}

void APrototypeXCharacter::ApplyAttackModeSettings()
{
	 bUseControllerRotationYaw = true;
	 bUseControllerRotationPitch = false; // = False > 따로따로 회전 적용
	 bUseControllerRotationRoll = false;

	 GetCharacterMovement()->bOrientRotationToMovement = false;
	 //GetCharacterMovement()->bUseControllerDesiredRotation = true;
	 GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // 회전 속도
	 GetCharacterMovement()->MaxAcceleration = 1500.f;

	 SpringArmComponent->bUsePawnControlRotation = true;
	 SpringArmComponent->bInheritPitch = true;
	 SpringArmComponent->bInheritYaw = true;
	 SpringArmComponent->bInheritRoll = false;

	 SpringArmComponent->bEnableCameraLag = true;
	 SpringArmComponent->CameraLagSpeed = 8.f;
	 //============================================================================================
	 MouseSensibiliy = 0.5f;
	 Normal_Speed = 450.f;
	 Sprint_Speed = 900.f;
	 GetCharacterMovement()->MaxWalkSpeed = Normal_Speed;

	 //Normal_Jump_Speed = 500.f;
	 //Max_Jump_Speed = 500.f;
	 //Min_Jump_Speed = Normal_Jump_Speed;

	 //GetCharacterMovement()->JumpZVelocity = Normal_Jump_Speed;

	 // =========================================================================

}
