// Fill out your copyright notice in the Description page of Project Settings.


#include "CardData.h"

FCardData::FCardData()
{
}

FCardData::~FCardData()
{
}

bool FCardData::IsCharactor() const
{
	if (CardType == CardType::person) return true;
	if (CardType == CardType::netural) return true;
	if (CardType == CardType::enemy) return true;
	return false;
}