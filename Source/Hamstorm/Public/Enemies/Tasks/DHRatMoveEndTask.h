// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "DHRatMoveEndTask.generated.h"

//This task is executed at the end of the rat's movement. It cleans up everything and resets variables to end the rat's turn.

UCLASS()
class HAMSTORM_API UDHRatMoveEndTask : public UBTTaskNode
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Keys")
	FBlackboardKeySelector DestinationKey;
};
