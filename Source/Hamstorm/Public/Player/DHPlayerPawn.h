// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DHPlayerPawn.generated.h"

class USphereComponent;
class UFloatingPawnMovement;
class UCameraComponent;
class USpringArmComponent;

//Class for the player pawn. It is basically just a camera.
UCLASS()
class HAMSTORM_API ADHPlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	ADHPlayerPawn();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Movement")
	USphereComponent* SphereCollision;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera")
	USpringArmComponent* SpringArm;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera")
	UCameraComponent* Camera;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Movement")
	UFloatingPawnMovement* PawnMovement;
};
