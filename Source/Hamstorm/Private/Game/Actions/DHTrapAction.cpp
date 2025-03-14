// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Actions/DHTrapAction.h"

#include "Environment/DHPressurePlate.h"
#include "Environment/DHTrap.h"

//This function restores the state of the trap to what it was before the activation.
void UDHTrapAction::RevertAction()
{
	PressurePlate->Trap->RearmTrap();

	PressurePlate->WasActivated = false;
}
