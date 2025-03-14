// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/DHHoverableInterface.h"
#include "Interfaces/DHPickupInterface.h"
#include "DHCollectible.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCollectibleCollected);

//Class for collectible objects
UCLASS()
class HAMSTORM_API ADHCollectible : public AActor, public IDHPickupInterface, public IDHHoverableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADHCollectible();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Animation")
	FVector AnimationPosition;

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Component")
	UBoxComponent* BoxComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Component")
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(BlueprintAssignable, Category="Collectible")
	FOnCollectibleCollected OnCollectibleCollected;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool
						bFromSweep, const FHitResult& SweepResult);
	
	virtual void BeginPlay() override;

	virtual bool TryPickup() override;

	virtual void AddPickupAction() override;
};
