// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/DHHoverableInterface.h"
#include "DHEndPipe.generated.h"

class UStaticMeshComponent;
class UDecalComponent;
class USceneComponent;
class ADHFloorTile;

//Class for the pipe that handles level completion. Level completion is handled in blueprints.
UCLASS()
class HAMSTORM_API ADHEndPipe : public AActor, public IDHHoverableInterface
{
	GENERATED_BODY()

public:
	ADHEndPipe();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* Pipe;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* Entrance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* Sign;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UDecalComponent* Decal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	ADHFloorTile* EndTile;
};
