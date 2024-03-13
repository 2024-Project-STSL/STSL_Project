// Fill out your copyright notice in the Description page of Project Settings.


#include "MouseInputInterface.h"

// Add default functionality here for any IMouseInputInterface functions that are not pure virtual.
void IMouseInputInterface::StartDrag()
{
	return;
}

void IMouseInputInterface::EndDrag()
{
	return;
}

void IMouseInputInterface::StartMouseHover()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Start Drag"));
}

void IMouseInputInterface::EndMouseHover()
{
	return;
}

void IMouseInputInterface::MoveToCursor()
{
	return;
}