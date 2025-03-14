// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Environment/DHTrap.h"
#include "DHWallTrap.generated.h"

class ADHPressurePlate;
class UArrowComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWallTrapFiredUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKillActor, AActor*, Actor);

//Class for the wall trap that is activated with a pressure plate.
UCLASS()
class HAMSTORM_API ADHWallTrap : public ADHTrap
{
	GENERATED_BODY()

public:

	ADHWallTrap();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Projectile")
	UStaticMeshComponent* Shooter;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Projectile")
	UStaticMeshComponent* Projectile;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Projectile")
	FVector InitialProjectileLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Projectile")
	float ProjectileSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Projectile")
	UArrowComponent* ShootingDirection;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Highlight")
	ADHPressurePlate* PressurePlate;

	UPROPERTY(BlueprintAssignable, EditAnywhere, Category="Projectile")
	FOnWallTrapFiredUpdated OnWallTrapFiredUpdated;

	UPROPERTY(BlueprintAssignable, EditAnywhere, Category="Projectile")
	FOnKillActor OnKillActor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Projectile")
	bool IsProjectileFired = false;

	UFUNCTION(BlueprintNativeEvent, Category="Trap")
	void OnTrapTriggered();
	
	UFUNCTION()
	virtual void ActivateTrap() override;

	virtual void RearmTrap() override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool
						bFromSweep, const FHitResult& SweepResult);
protected:
	virtual void BeginPlay() override;

	
};
