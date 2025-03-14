// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "DHResetDestinationtask.generated.h"

//This task is called at the end of the movement. It resets the destination in the blackboard.
UCLASS()
class HAMSTORM_API UDHResetDestinationtask : public UBTTaskNode
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Keys")
	FBlackboardKeySelector DestinationKey;
};
