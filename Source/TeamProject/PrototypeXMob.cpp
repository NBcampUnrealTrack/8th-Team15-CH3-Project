// Fill out your copyright notice in the Description page of Project Settings.


#include "PrototypeXMob.h"

// Sets default values
APrototypeXMob::APrototypeXMob()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APrototypeXMob::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APrototypeXMob::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APrototypeXMob::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

