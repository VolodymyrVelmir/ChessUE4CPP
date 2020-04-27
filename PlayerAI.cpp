// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAI.h"


APlayerAI::APlayerAI()
{
	PrimaryActorTick.bCanEverTick = false;
}

 
void APlayerAI::BeginPlay()
{
	Super::BeginPlay();
	Random.GenerateNewSeed();
}
 

FAIFiguresInfo APlayerAI::SerchMaxPowerLocation(int32 IndexPlayerAttacker) {
	FAIFiguresInfo result;
	TArray<FAttackFiguresInfo> AttackFigures;
	int32 indexOponent = (IndexPlayerAttacker == 0) ? 1 : 0;
	FPlayerInfo& player = DeskActor->GetPlayerStatistic(IndexPlayerAttacker);
	for (AFigureBase* figur : player.PlayerFigurs) {
		FAttackFiguresInfo atackInfo = FAttackFiguresInfo();
		atackInfo.FigureAtacker = figur;
		for (int32 locAtac : figur->GetAttackSectors()) {
			ADeskSectorBase* moveToSector = DeskActor->GetSectorFromIndex(locAtac);
			if (moveToSector && !moveToSector->IsEmptySector() && moveToSector->GetFigureOwnerPlayer()== indexOponent) {
				atackInfo.AttackSector.Add(locAtac);
			}
		}
		if (atackInfo.AttackSector.Num()>0) {
			AttackFigures.Add(atackInfo);
		}

	}
	TArray<int32> figureLocs = GetFigureSectorIndex(indexOponent);
	int32 power = 0;
	for (int32 fLoc : figureLocs) {
		for (FAttackFiguresInfo atackInfo : AttackFigures ) {
			if (atackInfo.AttackSector.Num()>0) {
				for (int32 oLoc : atackInfo.AttackSector) {
					AFigureBase* figur = DeskActor->GetFigureFromIndex(fLoc);
					if (figur && oLoc == fLoc && figur->GetAttackSectors().Num() > 0) {
						power = figur->GetFigurePover();
						if (power > result.FigurePower) {
							result.FigurePower = figur->GetFigurePover();
							result.FigureSectorIndex = fLoc;
							result.TargetSectorIndex = atackInfo.FigureAtacker->GetCurrentDeskSector()->GetSectorLocation().IndexSector;;
						}
					}
				}
			}
		}
	}
	return result;
}


void APlayerAI::InitAIStep(int32 PlayerAiIndex, ADeskBase* Desk) {
	IndexAi = PlayerAiIndex;
	DeskActor = Desk;
}

void APlayerAI::MoveTimer() {
	FAIFiguresInfo attackLoc = CheckAttackFigure();
	FAIFiguresInfo safeLoc = CheckSafe();
	bool isNoChanceSave = true;
	if (safeLoc.FigurePower > 0) {
		isNoChanceSave = DeskActor->GetFigureFromIndex(safeLoc.FigureSectorIndex)->GetAttackSectors().Num() <= 0;
	}
	if (attackLoc.FigurePower >= safeLoc.FigurePower && attackLoc.FigurePower > 0) {
		AttackFigure(attackLoc.TargetSectorIndex, attackLoc.FigureSectorIndex);
	}
	else if (safeLoc.FigurePower > attackLoc.FigurePower && safeLoc.FigurePower > 0 && !isNoChanceSave) {
		SefeMoveFigure(safeLoc.FigureSectorIndex, safeLoc.TargetSectorIndex);
	}
	else {
		int32 indexMove = SerchMoveFigure();
		if (indexMove != -1) {
			MoveFigure(indexMove);
		}
	}

}

void APlayerAI::StartAIStep() {
	GetWorldTimerManager().SetTimer(TimerMove, this, &APlayerAI::MoveTimer, 1.0f);


}

int32 APlayerAI::GetRandomLocation(TArray<int32> MoveLocs, bool isIgnoreLoc, TArray<int32> IgnoreLocs) {
	int32 indexMove = Random.RandRange(0, MoveLocs.Num() - 1);
	int32 moveLoc = MoveLocs[indexMove];

	if (isIgnoreLoc) {
		for (int32 iLoc : IgnoreLocs) {
			if (iLoc == moveLoc) {
				return GetRandomLocation(MoveLocs, false, IgnoreLocs);
			}
		}
	}
	return moveLoc;
}

void APlayerAI::SefeMoveFigure(int32 FigureSectorIndex, int32 AttackSectorIndex) {
	AFigureBase* figur = DeskActor->GetFigureFromIndex(FigureSectorIndex);
	AFigureBase* attackFigur = DeskActor->GetFigureFromIndex(AttackSectorIndex);
	TArray<int32> moveLocs = figur->GetMoveSectors();
	TArray<int32> AttackLocs = attackFigur->GetAttackSectors();
	int32 moveLoc = GetRandomLocation(moveLocs, true, AttackLocs);
	figur->PlayerClick();

	ADeskSectorBase* moveToSector = DeskActor->GetSectorFromIndex(moveLoc);
	moveToSector->PlayerClick();
}


int32 APlayerAI::SerchMoveFigure() {
	int32 IndexFigure = -1;
	TArray<AFigureBase*> moveFigurs;
	FPlayerInfo& player = DeskActor->GetPlayerStatistic(GetAIPlayer());
	for (AFigureBase* figur : player.PlayerFigurs) {
		if (figur->GetMoveSectors().Num() > 0) {
			moveFigurs.Add(figur);
		}
	}
	IndexFigure = Random.RandRange(0, moveFigurs.Num() - 1);
	IndexFigure = moveFigurs[IndexFigure]->GetCurrentDeskSector()->GetSectorLocation().IndexSector;
	return IndexFigure;
}

float APlayerAI::GetSectorPrioryty(int32 SectorIndex, TSubclassOf < AFigureBase> FigureClass) {
	for (FFiguresPositionPrioryryInfo& slot : FiguresPositionPrioryry) {
		if (slot.FigureClass == FigureClass) {
			return slot.SectorPrioryrys[SectorIndex];
		}
	}
	return 0.0f;
}
void APlayerAI::MoveFigure(int32 FigureSectorIndex) {

	AFigureBase* figur = DeskActor->GetFigureFromIndex(FigureSectorIndex);
	TArray<int32> moveLocs = figur->GetMoveSectors();
	TSubclassOf < AFigureBase> figureClass = figur->GetClass();
	float prioryty = 0;
	float priorytyMax = -99999;
	int32 PriorytyLoc = 0;
	for (int32 mLoc : moveLocs) {
		prioryty = GetSectorPrioryty(mLoc, figureClass);
		if (prioryty > priorytyMax) {
			priorytyMax = prioryty;
			PriorytyLoc = mLoc;
		}
	}
	figur->PlayerClick();
	if (PriorytyLoc > 0) {
		ADeskSectorBase* moveToSector = DeskActor->GetSectorFromIndex(PriorytyLoc);
		moveToSector->PlayerClick();
	}
}

void APlayerAI::AttackFigure(int32 FigureSectorIndex, int32 TargetSectorIndex) {
	AFigureBase* figur = DeskActor->GetFigureFromIndex(FigureSectorIndex);
	figur->PlayerClick();
	AFigureBase* attackFigur = DeskActor->GetFigureFromIndex(TargetSectorIndex);
	attackFigur->PlayerClick();
}


FAIFiguresInfo APlayerAI::CheckAttackFigure() {
	return SerchMaxPowerLocation(GetAIPlayer());
}

FAIFiguresInfo APlayerAI::CheckSafe() {
	int32 indexOponent = (GetAIPlayer() == 0) ? 1 : 0;
	return SerchMaxPowerLocation(indexOponent);
}

TArray<int32> APlayerAI::GetFigureSectorIndex(int32 IndexPlayer) {
	TArray<int32> result;
	FPlayerInfo& player = DeskActor->GetPlayerStatistic(IndexPlayer);
	for (AFigureBase* figur : player.PlayerFigurs) {
		int32 index = figur->GetCurrentDeskSector()->GetSectorLocation().IndexSector;
		result.Add(index);
	}
	return result;
}
