// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Environment/DHDestructibleWall.h"
#include "DHDestructiblePillar.generated.h"

class ADHHamsterCharacter;
class ADHPlayerController;

//Class for pillar objective in the final level.
UCLASS()
class HAMSTORM_API ADHDestructiblePillar : public ADHDestructibleWall
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

	//Zone of the final level that is associated with that pillar.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Pillar")
	FText ZoneName;

	virtual void Destruct() override;

};
