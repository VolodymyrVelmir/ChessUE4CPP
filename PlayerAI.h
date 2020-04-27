// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DeskBase.h"
#include "Figures/FigureBase.h"
#include "Systemt/ChessGameInstance.h"
#include "DeskSectorBase.h"
#include "GameFramework/Actor.h"
#include "PlayerAI.generated.h"


USTRUCT(BlueprintType) struct FAIFiguresInfo
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 FigurePower=0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 FigureSectorIndex=0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 TargetSectorIndex = 0;
};

USTRUCT(BlueprintType) struct FFiguresPositionPrioryryInfo
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf < AFigureBase> FigureClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<float> SectorPrioryrys;

};

USTRUCT(BlueprintType) struct FAttackFiguresInfo
{
	GENERATED_USTRUCT_BODY()
		UPROPERTY(EditAnywhere, BlueprintReadWrite) AFigureBase* FigureAtacker;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<int32> AttackSector;

};

UCLASS()
class MYPROJECT_API APlayerAI : public AActor
{
	GENERATED_BODY()
	
public:	
 
	APlayerAI();

protected:
	FTimerHandle TimerMove;
	virtual void BeginPlay() override;
	int32 IndexAi = 0;
	ADeskBase* DeskActor;
 
	FRandomStream Random;
public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sectors") TArray<FFiguresPositionPrioryryInfo> FiguresPositionPrioryry;

	int32 GetAIPlayer() {
		return IndexAi;
	}
	void InitAIStep(int32 PlayerAiIndex, ADeskBase* Desk);
	void MoveTimer();
	void StartAIStep(); //init move ai
	int32 GetRandomLocation(TArray<int32> MoveLocs, bool isIgnoreLoc, TArray<int32> IgnoreLocs);
	void SefeMoveFigure(int32 FigureSectorIndex, int32 AttackSectorIndex);
	int32 SerchMoveFigure();
	float GetSectorPrioryty(int32 SectorIndex, TSubclassOf < AFigureBase> FigureClass);//calculeit optimal position from sectors
	void MoveFigure(int32 FigureSectorIndex);
	void AttackFigure(int32 FigureSectorIndex, int32 TargetSectorIndex);
	FAIFiguresInfo CheckAttackFigure();
	FAIFiguresInfo CheckSafe();
	TArray<int32> GetFigureSectorIndex(int32 IndexPlayer);
	

	FAIFiguresInfo SerchMaxPowerLocation(int32 IndexPlayerAttacker);//calculeit optimal position from figure power



};
