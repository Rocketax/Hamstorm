// Fill out your copyright notice in the Description page of Project Settings.


#include "..\..\Public\Player\DHPlayerPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"

ADHPlayerPawn::ADHPlayerPawn()
{
	PrimaryActorTick.bCanEverTick = false;

	//Setting up components.
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));

	RootComponent = SphereCollision;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));

	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	Camera->SetupAttachment(SpringArm);

	PawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("PawnMovement"));
}

void ADHPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

