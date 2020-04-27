// Fill out your copyright notice in the Description page of Project Settings.


#include "FigureBase.h"
#include "Systemt/ChessPlayerControllerBase.h"
#include "DeskSectorBase.h"
 
AFigureBase::AFigureBase()
{
 
	PrimaryActorTick.bCanEverTick = false;

	Model = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Model"));
	//SelectFlag = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SelectFlag"));
}
 

FDeskSectorLocation AFigureBase::GetCurrentLocation() {
	return GetCurrentDeskSector()->GetSectorLocation();
}

bool AFigureBase::isEqualsSpawnSector(FDeskSectorLocation SectorLocation) {
	return SectorLocation.IndexSector == SpawnDeskSector->GetSectorLocation().IndexSector;
}

void AFigureBase::BeginPlay()
{
	Super::BeginPlay();
	ShowSlelectFigure(false);
 

}

bool AFigureBase::CheckMoveBlock(FDeskSectorLocation SectorLocation) {
	return true;
}
 bool AFigureBase::CheckAttackBlock(FDeskSectorLocation SectorLocation) {
	return true;
}
 
void AFigureBase::PlayerClick() {
	if (CurrentDeskSector!=nullptr) {
		CurrentDeskSector->PlayerClick();
	}
}
  
void AFigureBase::ClearSector() {
	CurrentDeskSector->ClearFigure();
	CurrentDeskSector = nullptr;
}

void AFigureBase::BeckMoveFigure() {
	CurrentDeskSector->AddFigureIsSector(this);
}

bool AFigureBase::FindEqualdLocation(int32 FindLocation, TArray<int32> MoveLocations) {
	for (int32 loc : MoveLocations) {
		if (loc == FindLocation) {
			return true;
		}
	}
	return false;
}

void AFigureBase::DestroyFigure() {
	if (CurrentDeskSector != nullptr) {
		ClearSector();
	}
	Destroy();
}

void AFigureBase::SpawnInit(int32 PlayerIndex, bool isBlack, ADeskSectorBase* DeskSector, int32 LineSize) {
	WorldSize = LineSize;
	SpawnDeskSector = DeskSector;
	SetCurrentDeskSector(SpawnDeskSector);
	IndexPlayerOwner = PlayerIndex;
	if (Model != nullptr) {
		UMaterialInterface* mat = (isBlack) ? MaterialBlack : MaterialWhite;
		Model->SetMaterial(0, mat);
		TArray< USceneComponent*> Childrens;
		Model->GetChildrenComponents(true, Childrens);
		for (USceneComponent* scenComp : Childrens) {
			UStaticMeshComponent* childMesh = Cast<UStaticMeshComponent>(scenComp);

			if (childMesh != nullptr && !childMesh->ComponentHasTag("SelectFlag")) {
				childMesh->SetMaterial(0, mat);
			}
		}
	}
}
