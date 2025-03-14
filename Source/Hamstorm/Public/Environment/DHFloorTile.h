// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/DHDestructibleInterface.h"
#include "Interfaces/DHHoverableInterface.h"
#include "DHFloorTile.generated.h"


class ADHGameState;
class ADHHamsterCharacter;
class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBrokenTileDestroyed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBrokenTileReverted);

//Class for floor tiles and destructible ones.
UCLASS()
class HAMSTORM_API ADHFloorTile : public AActor, public IDHDestructibleInterface, public IDHHoverableInterface
{
	GENERATED_BODY()
	
	UPROPERTY()
	ADHHamsterCharacter* HamsterCharacter;
	
	UPROPERTY()
	ADHGameState* GameState;

public:

	ADHFloorTile();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Component")
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Component")
	UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Assets")
	int32 FloorType = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rotation")
	int32 FloorRotationZ = 0;

	UPROPERTY(BlueprintAssignable, Category="Destructible")
	FOnBrokenTileDestroyed OnBrokenTileDestroyed;

	UPROPERTY(BlueprintAssignable, Category="Destructible")
	FOnBrokenTileReverted OnBrokenTileReverted;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Destructible")
	bool Destructible = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Destructible")
	bool Destroyed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Destructible")
	bool HasBeenHit = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Destructible")
	bool ShouldBeDestroyed = false;

protected:

	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool
	                    bFromSweep, const FHitResult& SweepResult);

public:

	UFUNCTION(BlueprintCallable, Category="Destructible")
	virtual void SetShouldBeDestroyed(bool newValue) override;

	UFUNCTION(BlueprintCallable, Category="Destructible")
	virtual void Destruct() override;

	virtual ADHFloorTile* CheckValidAdjacentTile() override;

	UFUNCTION(BlueprintCallable, Category="Action")
	void AddDestroyAction();

	UFUNCTION(BlueprintCallable, Category="Action")
	void AddHitAction();
};
