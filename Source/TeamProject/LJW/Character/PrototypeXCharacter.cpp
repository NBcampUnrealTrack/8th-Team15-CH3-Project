#include "LJW/Character/PrototypeXCharacter.h"
#include "TeamProject/LJW/GameUtilHeader/GameUtil.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "EnhancedInputComponent.h"
#include "LJW/Controller/PrototypeXPlayerController.h"

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
	//SetPlayerMode(EPlayerMode::Normal);
	SetPlayerMode(EPlayerMode::Attack);
}

void APrototypeXCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Mouse_Interp
	//Inter_Look(DeltaTime);
	

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

			//if (PlayerController->IA_Lock)
			//{
			//	EnhancedInput->BindAction(
			//		PlayerController->IA_Lock,
			//		ETriggerEvent::Triggered,
			//		this,

			//	)
			//}

			if (PlayerController->IA_Attack)
			{
				EnhancedInput->BindAction(
					PlayerController->IA_Attack,
					ETriggerEvent::Triggered,
					this,
					&APrototypeXCharacter::Attack_Start
				);

				EnhancedInput->BindAction(
					PlayerController->IA_Attack,
					ETriggerEvent::Completed,
					this,
					&APrototypeXCharacter::Attack_End
				);
			}

			if (PlayerController->IA_Defence)
			{
				EnhancedInput->BindAction(
					PlayerController->IA_Defence,
					ETriggerEvent::Triggered,
					this,
					&APrototypeXCharacter::Defence_Start
				);

				EnhancedInput->BindAction(
					PlayerController->IA_Defence,
					ETriggerEvent::Completed,
					this,
					&APrototypeXCharacter::Defence_End
				);
			}
		}
	}
}

void APrototypeXCharacter::Move_Start(const FInputActionValue& value)
{
	if (!Controller) return;
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

void APrototypeXCharacter::Attack_Start(const FInputActionValue& value)
{

}

void APrototypeXCharacter::Attack_End(const FInputActionValue& value)
{

}

void APrototypeXCharacter::Defence_Start(const FInputActionValue& value)
{

}

void APrototypeXCharacter::Defence_End(const FInputActionValue& value)
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
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->bInheritPitch = true;
	SpringArmComponent->bInheritYaw = true;
	SpringArmComponent->bInheritRoll = false;

	//============================================================================================
	MouseSensibiliy = 0.5f;
	Normal_Speed = 450.f;
	Sprint_Speed = 900.f;
	GetCharacterMovement()->MaxWalkSpeed = Normal_Speed;

	Normal_Jump_Speed = 500.f;
	Max_Jump_Speed = 500.f;
	Min_Jump_Speed = Normal_Jump_Speed;

	GetCharacterMovement()->JumpZVelocity = Normal_Jump_Speed;

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

	 Normal_Jump_Speed = 500.f;
	 Max_Jump_Speed = 500.f;
	 Min_Jump_Speed = Normal_Jump_Speed;

	 GetCharacterMovement()->JumpZVelocity = Normal_Jump_Speed;

	 // =========================================================================

}
