// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/DHHoverableInterface.h"
#include "DHPressurePlate.generated.h"

class ADHTrap;
class UBoxComponent;

//Class for the pressure plate of the wall trap. Activates the wall trap when stepped on.
UCLASS()
class HAMSTORM_API ADHPressurePlate : public AActor, public IDHHoverableInterface
{
	GENERATED_BODY()
	
public:	

	ADHPressurePlate();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Plate")
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Plate")
	UStaticMeshComponent* Plate;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Plate")
	UBoxComponent* BoxComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Trap")
	ADHTrap* Trap;

	UPROPERTY(BlueprintReadWrite, Category="Trap")
	bool WasActivated = false;

	UFUNCTION(BlueprintCallable, Category="Action")
	void AddTrapAction();

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool
						bFromSweep, const FHitResult& SweepResult);

	
};
