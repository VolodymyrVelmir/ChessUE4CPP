// Fill out your copyright notice in the Description page of Project Settings.


#include "KnightBase.h"
AKnightBase::AKnightBase() {
	isEqualsMoveAndAttack = true;
}


bool AKnightBase::CheckMoveBlock(FDeskSectorLocation SectorLocation) {
	return FindEqualdLocation(SectorLocation.IndexSector, GetMoveSectors());
};

bool AKnightBase::CheckAttackBlock(FDeskSectorLocation SectorLocation) {
	return CheckMoveBlock(SectorLocation);
};

TArray<int32> AKnightBase::GetMoveSectors() {
	TArray<int32> result;
	TArray<int32> Bufer;
	int32 sectorMax = GetWorldSize()*GetWorldSize();
	int32 indexSector = GetCurrentLocation().IndexSector;
	FDeskSectorLocation currentSectorLoc = GetCurrentLocation();
	int32 distance = indexSector + 2;
	if (distance > ((currentSectorLoc.Y )  *GetWorldSize()) && distance <= ((currentSectorLoc.Y + 1) *GetWorldSize())) {
		Bufer.Add(distance + GetWorldSize());
		Bufer.Add(distance - GetWorldSize());
	}
	 distance = indexSector - 2;
	if (distance > ((currentSectorLoc.Y)  *GetWorldSize()) && distance <= ((currentSectorLoc.Y + 1) *GetWorldSize())) {
		Bufer.Add(distance + GetWorldSize());
		Bufer.Add(distance - GetWorldSize());
	}
	int32 botIndex = indexSector - (GetWorldSize() * 2);
	Bufer.Add(botIndex + 1);
	Bufer.Add(botIndex - 1);
	int32 topIndex = indexSector + (GetWorldSize() * 2);
	Bufer.Add(topIndex + 1);
	Bufer.Add(topIndex - 1);
    for (int32 value : Bufer) {
		if (value > 0 && value < sectorMax) {
			result.Add(value);
		}
	} 
	return result;
}
