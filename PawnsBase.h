// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
 
#include "Figures/FigureBase.h"
#include "PawnsBase.generated.h"
 
UCLASS()
class MYPROJECT_API APawnsBase : public AFigureBase
{
	GENERATED_BODY()

	virtual bool CheckMoveBlock(FDeskSectorLocation SectorLocation) override;
	virtual bool CheckAttackBlock(FDeskSectorLocation SectorLocation) override;

	void CheclLevelUp(int32 IndexMoveSector);
	virtual TArray<int32> GetMoveSectors()override;
	virtual TArray<int32> GetAttackSectors()override;
	
};
