// Fill out your copyright notice in the Description page of Project Settings.


#include "PawnsBase.h"
#include "DeskSectorBase.h"
#include "DeskBase.h"

bool APawnsBase::CheckMoveBlock(FDeskSectorLocation SectorLocation){
 
	if (FindEqualdLocation(SectorLocation.IndexSector, GetMoveSectors())) {
		CheclLevelUp(SectorLocation.Y);
		return true;
	}
	return false;
 
};

bool APawnsBase::CheckAttackBlock(FDeskSectorLocation SectorLocation) {
	if (FindEqualdLocation(SectorLocation.IndexSector, GetAttackSectors())) {
		CheclLevelUp(SectorLocation.Y);
		return true;
	}
 
	return false;
};

void APawnsBase::CheclLevelUp(int32 IndexMoveSector) {
	if ((GetExisMove()<0 && IndexMoveSector==0) || (GetExisMove() > 0 && IndexMoveSector == GetWorldSize()-1)) {
		OnLevelUpFigure.Broadcast(this);
		GetCurrentDeskSector()->GetDesk()->StartFiguresLeveUp(this);
	}
}

TArray<int32> APawnsBase::GetMoveSectors() {
	TArray<int32> WaySetctors;
	FDeskSectorLocation currentLocation = GetCurrentLocation();
	int32 indexMoveSector = currentLocation.IndexSector + (GetWorldSize()*GetExisMove());
	WaySetctors.Add(indexMoveSector);
	if (isEqualsSpawnSector(currentLocation) && GetCurrentDeskSector()->GetDesk()->isFreeWay(WaySetctors)) {
		WaySetctors.Add(indexMoveSector + (GetWorldSize()*GetExisMove()));
	}
	return WaySetctors;
}

 TArray<int32> APawnsBase::GetAttackSectors() {
	TArray<int32> result;
	FDeskSectorLocation currentLocation = GetCurrentLocation();
	int32 indexSector = currentLocation.IndexSector + (GetWorldSize()*GetExisMove());
	if (currentLocation.X!=0) {
		result.Add(indexSector - 1);
	}
	if (currentLocation.X != GetWorldSize()-1) {
		result.Add(indexSector + 1);
	}
	
	return result;
}
