#include "LJW/Controller/PrototypeXPlayerController.h"
#include "LJW/Character/PrototypeXCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/PlayerCameraManager.h"

APrototypeXPlayerController::APrototypeXPlayerController()
	:IMC_Player(nullptr)
	,IA_Move(nullptr)
	,IA_Look(nullptr)
	,IA_Roll(nullptr)
	,IA_Sprint(nullptr)
	,IA_Lock(nullptr)
	,IA_Jump(nullptr)
	,IA_Attack(nullptr)
	,IA_Defence(nullptr)
	,IA_ItemQuickUse(nullptr)
{

}

void APrototypeXPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (PlayerCameraManager)
	{
		PlayerCameraManager->StartCameraFade(2.0f, 0.0f, 2.0f, FLinearColor::Black, false, false);
	}

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* LocalPlayerSubSys =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			LocalPlayerSubSys->AddMappingContext(IMC_Player, 0);
		}
	}

}

