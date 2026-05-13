#include "LJW/Controller/PrototypeXPlayerController.h"
#include "LJW/Character/PrototypeXCharacter.h"
#include "EnhancedInputSubsystems.h"
APrototypeXPlayerController::APrototypeXPlayerController()
	:IMC_Player(nullptr)
	, IA_Move(nullptr)
	, IA_Look(nullptr)
	, IA_Roll(nullptr)
	, IA_Sprint(nullptr)
	, IA_Lock(nullptr)
	, IA_Jump(nullptr)
	, IA_Attack(nullptr)
	, IA_Defence(nullptr)
	, IA_ItemUse(nullptr)
	, IA_Parry(nullptr)
{

}

void APrototypeXPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* LocalPlayerSubSys =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			LocalPlayerSubSys->AddMappingContext(IMC_Player, 0);
		}
	}

}

