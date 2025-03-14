// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DHFinalZoneTrigger.generated.h"

class UBoxComponent;

//Class for final zone trigger. Used to know when the player changes zones in the final level.
UCLASS()
class HAMSTORM_API ADHFinalZoneTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	ADHFinalZoneTrigger();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Trigger")
	UBoxComponent* BoxComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Zone")
	FString Zone;

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool
						bFromSweep, const FHitResult& SweepResult);
};
