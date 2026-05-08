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

void APrototypeXCharacter::ApplyRollingAtMode(EPlayerMode InMode)
{
	if (IsRollingMontagePlaying) return;
	UAnimInstance* Animbackground = GetMesh()->GetAnimInstance();

	SetPlayerMode(EPlayerMode::Normal);
		if (RollMontage)
		{
			if (Animbackground)
			{
				// �������ִ� ����Ű�� ������ setrot�Ѵ��� play�ؾ��ϳ�...
				BeforePlayerRot = GetActorRotation();

				FRotator ControlRot = GetControlRotation();
				FRotator YawRotation(0.f, ControlRot.Yaw, 0.f);// ��Ʈ���� Yawȸ��(Z��)�� ������

				FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
				// ��Ʈ�ѷ��� Yawȸ���� �����Z����� ����� �ٽø���� �ű⼭ ���� X�� +��
				FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
				// ��Ʈ�ѷ��� Yawȸ���� �����Z����� ����� �ٽø���� �ű⼭ ������ Y�� +��
				FVector DesiredDir = (Forward * NowPlayerDir.X) + (Right * NowPlayerDir.Y);
				// ��Ʈ�ѷ��� ������⺤�� * (1, -1)x����ǲ�Է°�, ��Ʈ�ѷ��� ������⺤�� * (1, -1)Y����ǲ�Է°�
				// ������ ���� �ι����� �߰�����
				if (DesiredDir.IsNearlyZero())
				{
					DesiredDir = GetActorForwardVector();
				}

				SetActorRotation(DesiredDir.Rotation());
				//DesiredDir����(Rotation) ȸ���϶�(SetActorRotation)

				Animbackground->Montage_Play(RollMontage);
				IsRollingMontagePlaying = true;
				// �̱� ��������Ʈ
				FOnMontageEnded EndDelegate;
				EndDelegate.BindUObject(this, &APrototypeXCharacter::RollingMontageEnd, InMode); // �Ű������߰��� �����ϴٴ�...
				// EndDelegate�� �ڽŰ� �Լ��� ������
				Animbackground->Montage_SetEndDelegate(EndDelegate, RollMontage);
				// RollMontage�� End�϶� SetEndDelegate�� EndDelegate�� ���(set)
				// endmontage�϶� �Լ����ε� call
			}
		}

}

void APrototypeXCharacter::RollingMontageEnd(UAnimMontage* Montage, bool bInterrupted, EPlayerMode InMode)
{//DECLARE_DELEGATE_TwoParams(FOnMontageEnded, UAnimMontage*, bool /*bInterrupted*/)
	//FOnMontageEnded���� F12�� ���� Ȯ���Ҽ�����
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
	const FRotator YawRotation(0, Rotation.Yaw, 0); // ��Ʈ�ѷ��� Yaw(Z��) ����

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	// ��Ʈ�ѷ��� Ʋ����Yaw�࿡�� XYZ����� ��� X���� ����� ����(���⺤�Ͷ� ũ��1)
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	// ��Ʈ�ѷ��� Ʋ����Yaw�࿡�� XYZ����� ��� Y���� ����� ����(���⺤�Ͷ� ũ��1)

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
	// TODO: bUsePawnControlRotation �� ��ġ
	// TODO: SpringArm Inherit Pitch/Yaw/Roll �� ��ġ
	// TODO: CharacterMovement ȸ�� ���� �ɼ� ��ġ
			// TODO: �̵���� ���� ���� �Լ� ȣ��
	bUseControllerRotationYaw = false; // = ��Ʈ�ѷ��� ȸ���� ĳ���Ͱ� ����� �ϰڴ���
	bUseControllerRotationPitch = false; // = False > ���ε��� ȸ�� ����
	bUseControllerRotationRoll = false;

	// �̵� �������� ĳ���Ͱ� �ڵ����� ȸ���ϵ��� ����
	GetCharacterMovement()->bOrientRotationToMovement = true; // �ٽ�: �Է� �������� ���� ����
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // ȸ�� �ӵ�
	GetCharacterMovement()->MaxAcceleration = 1500.f;
	// �������� ���� (ī�޶� ��Ʈ�ѷ� ȸ���� ����)
	SpringArmComponent->bUsePawnControlRotation = true;
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
	 bUseControllerRotationPitch = false; // = False > ���ε��� ȸ�� ����
	 bUseControllerRotationRoll = false;

	 GetCharacterMovement()->bOrientRotationToMovement = false;
	 //GetCharacterMovement()->bUseControllerDesiredRotation = true;
	 GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // ȸ�� �ӵ�
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
