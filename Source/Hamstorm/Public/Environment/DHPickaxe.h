// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/DHHoverableInterface.h"
#include "Interfaces/DHPickupInterface.h"
#include "DHPickaxe.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

//Class for pickaxe.
UCLASS()
class HAMSTORM_API ADHPickaxe : public AActor, public IDHPickupInterface, public IDHHoverableInterface
{
	GENERATED_BODY()

public:

	ADHPickaxe();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Component")
	USceneComponent* SceneComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Component")
	UStaticMeshComponent* StaticMeshComponent;

protected:

	virtual void BeginPlay() override;
	
public:

	UPROPERTY(BlueprintAssignable, Category="Pickup")
	FOnPickup OnPickup;

	UPROPERTY(BlueprintReadWrite, Category="Pickup")
	bool IsPickedUp = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Animation")
	FVector AnimationPosition;

	virtual bool TryPickup() override;

	virtual void AddPickupAction() override;
};
