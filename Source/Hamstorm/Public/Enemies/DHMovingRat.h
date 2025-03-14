// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DHRat.h"
#include "DHMovingRat.generated.h"

class UBehaviorTree;
class UBoxComponent;
class UStaticMeshComponent;
class UFloatingPawnMovement;

//Class for moving rat enemy
UCLASS()
class HAMSTORM_API ADHMovingRat : public ADHRat
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;

public:	

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Pathing")
	TArray<AActor*> TilesPath;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Pathing")
	int32 CurrentTile;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Pathing")
	FTransform RevertTransform;

	virtual void Die(bool ShouldStopMovement = true) override;
	
	virtual void StopMovement(bool ShouldEndTurn = true);

	virtual void AddMoveAction() override;
};
