// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Components/SphereComponent.h"
#include "Environment/Interfaces/DHHoverableInterface.h"
#include "GameFramework/Character.h"
#include "DHRat.generated.h"

class UDHHamsterPerceptionStimuli;
class UBehaviorTree;
class UBoxComponent;
class UStaticMeshComponent;
class UFloatingPawnMovement;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRatDied);

//Base class for the rat enemies.
UCLASS()
class HAMSTORM_API ADHRat : public ACharacter, public IDHHoverableInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

	UFUNCTION()
	void OnDyingBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnKillingBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY()
	TArray<AActor*> Rats;

	UPROPERTY()
	TArray<AActor*> Pickaxes;
	
public:
	ADHRat();

protected:
	virtual void BeginPlay() override;

public:	

	UPROPERTY(EditAnywhere)
	UBoxComponent* DyingBox;

	UPROPERTY(EditAnywhere)
	UBoxComponent* KillingBox;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Center;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Components")
	UDHHamsterPerceptionStimuli* StimuliComponent;

	UPROPERTY(BlueprintAssignable, Category="Death")
	FOnRatDied OnRatDied;

	UPROPERTY()
	FGenericTeamId TeamId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="State")
	bool IsAttacking = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="State")
	bool IsDead = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="State")
	bool IsMoving = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="State")
	bool IsFalling = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="State")
	bool IsDying = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="State")
	bool HasAlreadyDied = false;
	
	UFUNCTION(BlueprintCallable, Category="Death")
	virtual void Die(bool ShouldStopMovement = true);

	UFUNCTION(BlueprintCallable, Category="Movement")
	virtual void StopMovement(bool ShouldEndTurn = true);
	
	UFUNCTION(BlueprintCallable, Category="Action")
	void AddKillAction();

	UFUNCTION(BlueprintCallable, Category="Action")
	virtual void AddMoveAction();

	virtual FGenericTeamId GetGenericTeamId() const override;
};
