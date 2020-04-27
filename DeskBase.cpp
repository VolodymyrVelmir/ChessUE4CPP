// Fill out your copyright notice in the Description page of Project Settings.


#include "DeskBase.h"
#include "PlayerAI.h"
 
ADeskBase::ADeskBase()
{
 
	PrimaryActorTick.bCanEverTick = false;

}

 
void ADeskBase::BeginPlay()
{
	Super::BeginPlay();
	Random.GenerateNewSeed();
 
}


void ADeskBase::StartInitDesk(int32 IndexBlackPlayer) {
	PlayerMoveStep = (IndexBlackPlayer == 0) ? 1 : 0;
	PlayerBlackId = IndexBlackPlayer;
	if (PlayerOponentAi) {
		PlaierAIid = (IndexBlackPlayer == 0) ? 0 : 1;
		PlayerAI = GetWorld()->SpawnActor<APlayerAI>(PlayerAIClass, GetActorLocation(), { 0.0f,0.0f,0.0f }, FActorSpawnParameters());
		PlayerAI->InitAIStep(PlaierAIid,this);
	}
	CreateDesk();
	CreateAllChess(0, IndexBlackPlayer == 0);
	CreateAllChess(1, IndexBlackPlayer == 1);

	OnNextStep.Broadcast(PlayerMoveStep);
}

void ADeskBase::NextStep() { //end step move next player
	CheckEndGame(PlayerMoveStep);
	PlayerMoveStep = (PlayerMoveStep == 0) ? 1 : 0;
	SaveMoveFigure->ShowSlelectFigure(false);
	OnNextStep.Broadcast(PlayerMoveStep);
	
	DisableOldIlumination(SectorIluminationType::None);
	if (PlaierAIid != PlayerMoveStep) {
		DisableOldIlumination(SectorIluminationType::None);
	}
	SelectSector   = nullptr;
	SaveMoveFigure = nullptr;
	if (PlayerMoveStep == PlaierAIid && PlayerOponentAi) { //move Ai
		PlayerAI->StartAIStep();
	}
}


FPlayerInfo&  ADeskBase::GetPlayerStatistic(int32 PlayerId) {
	return PlayerStatistic[PlayerId];
}

void  ADeskBase::AddPlaterFigure(int32 PlayerId, AFigureBase*  AddFigur) {
	FPlayerInfo& player = GetPlayerStatistic(PlayerId);
	player.PlayerFigurs.Add(AddFigur);
}
void  ADeskBase::RemovePlaterFigure(int32 PlayerId, AFigureBase*  AddFigur) {
	FPlayerInfo& player = GetPlayerStatistic(PlayerId);
	player.PlayerFigurs.Remove(AddFigur);
}

void  ADeskBase::SetPlayerKing(AKingBase* KingFigure, int32 PlayerIndex) {
	FPlayerInfo& player = GetPlayerStatistic(PlayerIndex);
	player.King = KingFigure;
}
AKingBase*  ADeskBase::GetPlayerKing(int32 PlayerIndex) {
	FPlayerInfo& player = GetPlayerStatistic(PlayerIndex);
	return player.King;
}


void  ADeskBase::DisableOldIlumination(SectorIluminationType IluminationType) {
	TArray<int32> removeArray;
	int32 removeIndex = 0;
	for (FIluminationGroup& secgroup : IluminationSectors) {
		if (secgroup.luminationType == IluminationType || IluminationType == SectorIluminationType::None) {
			secgroup.IluminationSector->SetSectorIlumination(SectorIluminationType::None);
			removeArray.Add(removeIndex);
		}
		removeIndex++;
	}
	if (removeArray.Num() > 0) {
		for (int32 index = removeArray.Num() - 1; index > 0; index--) {
			IluminationSectors.RemoveAt(index);
		}
	}
}

void  ADeskBase::SetSectorIlumination(SectorIluminationType IluminationType, TArray<int32> SectorLocations, bool isAttackCheck) {
	bool flagIlum = false;
	if (IluminationType != SectorIluminationType::None) {//Enble new Ilumination
		for (int32 secLoc : SectorLocations) {
			ADeskSectorBase* sec = Sectors[secLoc - 1];
			flagIlum = false;
			if (IluminationType == SectorIluminationType::Move || isAttackCheck) { //check attack position in line move
				if (isAttackCheck) {
					bool isAttack = (!sec->IsEmptySector() && sec->GetFigureOwnerPlayer() != PlayerMoveStep);
					flagIlum = sec->IsEmptySector() || isAttack;
					IluminationType = (isAttack) ? SectorIluminationType::Attack : SectorIluminationType::Move;
				}
				else {
					flagIlum = sec->IsEmptySector();
				}
			}
			else if (IluminationType == SectorIluminationType::Attack) {
				flagIlum = !sec->IsEmptySector();
			}
			if (flagIlum) {
				sec->SetSectorIlumination(IluminationType);
				IluminationSectors.Add(FIluminationGroup());
				FIluminationGroup&   ilumination = IluminationSectors[IluminationSectors.Num() - 1];
				ilumination.luminationType = IluminationType;
				ilumination.IluminationSector = sec;
			}
		}
	}
}

void  ADeskBase::IluminationFigure(AFigureBase* MoveFigure) { //check ilumination Type
	if (PlaierAIid != PlayerMoveStep) {
		DisableOldIlumination(SectorIluminationType::None);
		bool isEqualsAttack = MoveFigure->GetEqualsMoveAndAttack();
		SetSectorIlumination(SectorIluminationType::Move, MoveFigure->GetMoveSectors(), isEqualsAttack);
		if (!isEqualsAttack) {
			SetSectorIlumination(SectorIluminationType::Attack, MoveFigure->GetAttackSectors(), false);
		}
	}
}


int32  ADeskBase::GetPlayerPower(int32 PlayerId) {
	FPlayerInfo& player = GetPlayerStatistic(PlayerId);
	return player.Power;
}
void  ADeskBase::SetPlayerPower(int32 PlayerId, int32 Value) {
	FPlayerInfo& player = GetPlayerStatistic(PlayerId);
	player.Power += Value;

}

 

int32  ADeskBase::GetPlayerMoveStep() {
	return PlayerMoveStep;
}

bool  ADeskBase::isNoSaveMoveFigure() {
	return !SaveMoveFigure;
}
void  ADeskBase::SetSaveMoveFigure(AFigureBase* MoveFigure) {

	SaveMoveFigure = MoveFigure;
}
void  ADeskBase::BeckMoveFigure() {
	if (!SaveMoveFigure) {
		SaveMoveFigure->BeckMoveFigure();
		SaveMoveFigure = nullptr;
	}
}




void  ADeskBase::SelectFigure(AFigureBase* MoveFigure) {
	FDeskSectorLocation figureSectorLoc = MoveFigure->GetCurrentDeskSector()->GetSectorLocation();
	if (SaveMoveFigure != nullptr) {
		SaveMoveFigure->ShowSlelectFigure(false);
	}
	if (CheckPlayer(MoveFigure)) {
		if (!isNoSaveMoveFigure()) {
			BeckMoveFigure();
		}

		SetSaveMoveFigure(MoveFigure);
		IluminationFigure(MoveFigure);
		MoveFigure->ShowSlelectFigure(true);
	}
	else if (SaveMoveFigure && SaveMoveFigure->CheckAttackBlock(figureSectorLoc)) {
		int32 figurePower = MoveFigure->GetFigurePover();
		bool attackFlag = true;
		if (!GetDisableCheckMode()) {// is ignore attack  position
			GameStepMofificationType modificatorAfterAttack = CheckShach(PlayerMoveStep, figureSectorLoc.IndexSector);
			attackFlag = modificatorAfterAttack == GameStepMofificationType::None;
		}
		if (attackFlag && !MoveFigure->GetBlockAttack()) { //Attack
			SetPlayerPower(GetPlayerMoveStep(), figurePower);
			OnAttackFigure.Broadcast(PlayerMoveStep, SaveMoveFigure, figurePower);
			SelectSector = MoveFigure->GetCurrentDeskSector();

			RemovePlaterFigure(MoveFigure->GetOwnerPlayerIndex(), MoveFigure);
			MoveFigure->DestroyFigure();
			MoveFigureToSelectSector();

		}
		NextStep();
	}
}

bool  ADeskBase::GetPlayerCheckMode(int32 PlayerIndex) {
	FPlayerInfo& player = GetPlayerStatistic(PlayerIndex);
	return player.Modification == GameStepMofificationType::Check;
}

void  ADeskBase::CheckEndGame(int32 PlayerIndex) {
	int32 oponentId = (PlayerIndex == 0) ? 1 : 0;
	if (GetPlayerCheckMode(PlayerIndex)) {
		CheckCheckmate(oponentId);
	}else {
		CheckShach(PlayerIndex, -1); //-1 == is no attack ignore position
	}
}

int32  ADeskBase::FinfCoincidencesLocs(AKingBase* King, TArray<int32> AtackLocation, TArray<int32> MoveLocation) {
	int32 coincidences = 0;
	int32 kingLoc = King->GetCurrentDeskSector()->GetSectorLocation().IndexSector;
	for (int32 move : MoveLocation) {
		if (King->FindEqualdLocation(kingLoc, AtackLocation)) {
			coincidences++;
		}
	}
	return coincidences;
}

bool  ADeskBase::CheckCheckmate(int32 PlayerIndex) {
	AKingBase* king = GetPlayerKing(PlayerIndex);
	int32 oponentId = (PlayerIndex == 0) ? 1 : 0;
	FPlayerInfo& oponentPlayer = GetPlayerStatistic(oponentId);
	TArray<int32> kingMove = king->GetMoveSectors();
	int32 kingLoc = king->GetCurrentDeskSector()->GetSectorLocation().IndexSector;
	kingMove.Add(kingLoc);
	TArray<int32> oponentAttackLoc;
	for (AFigureBase* figur : oponentPlayer.PlayerFigurs) {
		oponentAttackLoc.Append(figur->GetAttackSectors());
	}
	int32 valueNoWei = FinfCoincidencesLocs(king, oponentAttackLoc, kingMove);
	if (valueNoWei >= kingMove.Num()) {
		OnModifayerGame.Broadcast(oponentId, GameStepMofificationType::Checkmate);
		OnFinisGame.Broadcast(oponentId);
		return true;
	}
	return false;
}

GameStepMofificationType  ADeskBase::CheckShach(int32 PlayerIndex, int32 IgnoreLocation) {
	GameStepMofificationType result;
	FPlayerInfo& player = GetPlayerStatistic(PlayerIndex);
	int32 oponentId = (PlayerIndex == 0) ? 1 : 0;
	AKingBase* king = GetPlayerKing(oponentId);
	//FPlayerInfo& oponentPlayer = GetPlayerStatistic(oponentId);
	TArray<int32> oponentAttackLoc;
	for (AFigureBase* figur : player.PlayerFigurs) {
		oponentAttackLoc.Append(figur->GetAttackSectors());
	}
	int32 kingIndexSector = king->GetCurrentDeskSector()->GetSectorLocation().IndexSector;
	if (IgnoreLocation >= 0) {
		oponentAttackLoc.Remove(IgnoreLocation);
	}
	if (king->FindEqualdLocation(kingIndexSector, oponentAttackLoc)) {
		result = GameStepMofificationType::Check;
		OnModifayerGame.Broadcast(PlayerIndex, result);
	}
	else {
		result = GameStepMofificationType::None;
	}
	player.Modification = result;
	return result;
}

 


void  ADeskBase::StartFiguresLeveUp(AFigureBase* UpdateFigure) {
	if (PlayerMoveStep == PlaierAIid && PlayerOponentAi) {
		int32 index = Random.RandRange(0, LevelupFigureSettings.Num() - 1);
		EndFiguresLeveUp(UpdateFigure, LevelupFigureSettings[index].UpdateClass);
	}
	else {
		OnFigureLevelUp.Broadcast(PlayerMoveStep, UpdateFigure);
	}
}

 void  ADeskBase::EndFiguresLeveUp(AFigureBase* UpdateFigure, TSubclassOf<AFigureBase> UpFigureClass) {
	FDeskSectorLocation sectorLoc = UpdateFigure->GetCurrentDeskSector()->GetSectorLocation();
	UpdateFigure->ClearSector();
	bool isBlack = (PlayerMoveStep == PlayerBlackId);
	SaveMoveFigure = CreateChess(PlayerMoveStep, sectorLoc.Y, sectorLoc.X, UpFigureClass, isBlack);
	SaveMoveFigure->ShowSlelectFigure(true);
	RemovePlaterFigure(PlayerMoveStep, UpdateFigure);
	UpdateFigure->DestroyFigure();
}

bool  ADeskBase::GetDisableCheckMode() {
	bool isCheckMode = GetPlayerCheckMode(PlayerMoveStep);
	return !isCheckMode || CheckShach(PlayerMoveStep, -1) == GameStepMofificationType::None;
}

void  ADeskBase::SetSelectSector(ADeskSectorBase* Sector) {
	if (GetDisableCheckMode()) {
		if (SaveMoveFigure && SaveMoveFigure->GetCurrentDeskSector() != Sector && SaveMoveFigure->CheckMoveBlock(Sector->GetSectorLocation())) {
			SelectSector = Sector;
			if (SelectSector->IsEmptySector()) {
				MoveFigureToSelectSector();
				NextStep();
			}
		}
	}
}

void  ADeskBase::MoveFigureToSelectSector() {
	SaveMoveFigure->ClearSector();
	SelectSector->AddFigureIsSector(SaveMoveFigure);
}


bool  ADeskBase::CheckPlayer(AFigureBase* MoveFigure) {
	int32 playerFraction = MoveFigure->GetOwnerPlayerIndex();
	if (PlayerMoveStep == playerFraction) {// && playerFraction!= PlaierAIid
		return true;
	}
	return false;
}






void   ADeskBase::CreateAllChess(int32 PlayerIndex, bool isBlack) {
	FVector loc;
	int32 pawnLiniIndex = (PlayerIndex == 0) ? 1 : 6;
	int32 topnLiniIndex = (PlayerIndex == 0) ? 0 : 7;
	if (pawnsChessClass) {
		for (int32 iLine = 0; iLine < LineSize; iLine++) { // create linepawns chess
			CreateChess(PlayerIndex, pawnLiniIndex, iLine, pawnsChessClass, isBlack);
		}
	}
	for (int32 iLine = 0; iLine < LineSize; iLine++) { // create line top chess
		if (TopChessClass[iLine]) {
			CreateChess(PlayerIndex, topnLiniIndex, iLine, TopChessClass[iLine], isBlack);
		}
	}
}

AFigureBase*  ADeskBase::CreateChess(int32 PlayerIndex, int32 IndexLine, int32 IndexColunm, TSubclassOf<AFigureBase> FigureClass, bool isBlack) {
	ADeskSectorBase* sector = GetDeskSector(IndexLine, IndexColunm);
	FVector loc = sector->GetSpawnLocation();
	FRotator rot = { 0.0f,0.0f,0.0f };
	rot.Yaw = (PlayerIndex == 0) ? -90.0f : 90.0f;
	AFigureBase* chess = GetWorld()->SpawnActor<AFigureBase>(FigureClass, loc, rot, FActorSpawnParameters());
	chess->SpawnInit(PlayerIndex, isBlack, sector, LineSize);
	sector->AddFigureIsSector(chess);
	chess->PostCreate();
	AddPlaterFigure(PlayerIndex, chess);
	return chess;
}

ADeskSectorBase*  ADeskBase::GetDeskSector(int32 Line, int32 Colunm) {
	return Sectors[Line*LineSize + Colunm];
}

void  ADeskBase::CreateDesk() { 
	FVector loc;
	loc = GetActorLocation();
	float startY = loc.Y;
	bool colorBlack = false;
	for (int32 y = 0; y < LineSize; y++) {
		loc.Y = startY;
		colorBlack = !(y % 2 == 0);
		for (int32 x = 0; x < LineSize; x++) {
			ADeskSectorBase* sector = GetWorld()->SpawnActor<ADeskSectorBase>(DeskSectorClass, loc, { 0.0f,0.0f,0.0f }, FActorSpawnParameters());
			Sectors.Add(sector);

			FDeskSectorLocation DeskSectorLocation;
			DeskSectorLocation.X = x;
			DeskSectorLocation.Y = y;
			DeskSectorLocation.IndexSector = y * LineSize + x + 1;
			colorBlack = !colorBlack;
			sector->SpawnInit(colorBlack, DeskSectorLocation, this);
			loc.Y += SectorSize;
		}
		loc.X += SectorSize;
	}
}



CheckWaiType  ADeskBase::CheckFreeWay(int32 SectorIndex, int32 PlayerOwnerCheck) {
	ADeskSectorBase* sector = Sectors[SectorIndex - 1];
	if (!sector->IsEmptySector()) {
		return  (PlayerOwnerCheck == sector->GetFigureOwnerPlayer()) ? CheckWaiType::NoEmptyMy : CheckWaiType::NoEmptyEnemy;
	}
	return CheckWaiType::Empty;
}

FDeskSectorLocation  ADeskBase::GetSectorLocation(int32 SectorIndex) {
	ADeskSectorBase* sector = Sectors[SectorIndex - 1];
	return sector->GetSectorLocation();
}

bool  ADeskBase::isFreeWay(TArray<int32> WaySetctors) {  // no calculet start & end index
	for (int32 index : WaySetctors) {
		if (!Sectors[index - 1]->IsEmptySector()) {
			return false;
		}
	}
	return true;
}

ADeskSectorBase*  ADeskBase::GetSectorFromIndex(int32 SectorIndex) {
	return Sectors[SectorIndex - 1];
}

AFigureBase*  ADeskBase::GetFigureFromIndex(int32 SectorIndex) {
	return  Sectors[SectorIndex - 1]->GetFigureIsSector();
}
