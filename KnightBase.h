// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Figures/FigureBase.h"
#include "KnightBase.generated.h"

 
UCLASS()
class MYPROJECT_API AKnightBase : public AFigureBase
{
	GENERATED_BODY()
public:
	AKnightBase();
	virtual bool CheckMoveBlock(FDeskSectorLocation SectorLocation) override;
	virtual bool CheckAttackBlock(FDeskSectorLocation SectorLocation) override;
	virtual TArray<int32> GetMoveSectors()override;
};
