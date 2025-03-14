// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/DHAction.h"
#include "DHMoveAction.generated.h"

//Class for a move action in a turn. Used to revert the movement of the rats or the hamster.
UCLASS()
class HAMSTORM_API UDHMoveAction : public UDHAction
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, Category="Move")
	AActor* MovedActor;

	UPROPERTY(BlueprintReadWrite, Category="Move")
	FTransform RevertTransform;

	UPROPERTY(BlueprintReadWrite, Category="Move")
	int32 Direction;

	UPROPERTY(BlueprintReadWrite, Category="Move")
	bool IsAttacking = false;
	
	virtual void RevertAction() override;
};
