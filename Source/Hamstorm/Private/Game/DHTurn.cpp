// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/DHTurn.h"

#include "Game/DHAction.h"

UDHTurn::UDHTurn()
{
}

//This function reverts all the actions in the turn.
void UDHTurn::Revert()
{
	while (Actions.Num() > 0)
	{
		auto Action = Actions.Pop();
		
		Action->RevertAction();
		
	}
}
