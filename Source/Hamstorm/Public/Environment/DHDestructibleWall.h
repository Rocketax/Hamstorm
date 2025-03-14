// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/DHDestructibleInterface.h"
#include "Interfaces/DHHoverableInterface.h"
#include "DHDestructibleWall.generated.h"

class ADHGameState;
class ADHHamsterCharacter;
class ADHPlayerController;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWallDestroyedReverted);

//Class for the destructible walls
UCLASS()
class HAMSTORM_API ADHDestructibleWall : public AActor, public IDHDestructibleInterface, public IDHHoverableInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	ADHPlayerController* PlayerController;
	
	UPROPERTY()
	ADHHamsterCharacter* HamsterCharacter;
	
	UPROPERTY()
	ADHGameState* GameState;
	
public:	

	ADHDestructibleWall();

protected:

	virtual void BeginPlay() override;

public:	

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Components")
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(BlueprintAssignable, Category="Destructible")
	FOnWallDestroyedReverted OnWallDestroyedReverted;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Destructible")
	bool ShouldBeDestroyed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Destructible")
	bool Destroyed = false;

	UFUNCTION(BlueprintCallable, Category="Destructible")
	virtual void SetShouldBeDestroyed(bool newValue) override;
	
	UFUNCTION(BlueprintCallable)
	virtual void Destruct() override;

	UFUNCTION(BlueprintCallable)
	virtual ADHFloorTile* CheckValidAdjacentTile() override;

	UFUNCTION(BlueprintCallable, Category="Action")
	void AddDestroyAction();
};
