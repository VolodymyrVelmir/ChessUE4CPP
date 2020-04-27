// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Systemt/ChessGameInstance.h"

#include "Figures/FigureBase.h"
#include "Figures/KingBase.h"
#include "DeskSectorBase.h"
#include "GameFramework/Actor.h"
#include "DeskBase.generated.h"

class APlayerAI;
UENUM(BlueprintType)enum class GameStepMofificationType : uint8
{
	None UMETA(DisplayName = "None"),
	Check UMETA(DisplayName = "Check"),
	Checkmate UMETA(DisplayName = "Checkmate")
};

USTRUCT(BlueprintType) struct FIluminationGroup
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) ADeskSectorBase*  IluminationSector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) SectorIluminationType luminationType;
};

USTRUCT(BlueprintType) struct FFigureLevelupSettings
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<AFigureBase> UpdateClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName UpdateId;
};


USTRUCT(BlueprintType) struct FPlayerInfo
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Power=0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) AKingBase* King;
	TArray<AFigureBase*> PlayerFigurs;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) GameStepMofificationType Modification;

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAttackFigure, int32, OwnerAttack,AFigureBase*, AttackerFigure,int32, PowerDeathFigure);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNextStep, int32, PlayerStepId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFinisGame, int32, PlayerWinerId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnModifayerGame, int32, PlayerWinerId, GameStepMofificationType,Modification);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFigureLevelUp, int32, PlayerWinerId, AFigureBase*, Figure);

UCLASS()
class MYPROJECT_API ADeskBase : public AActor
{
	GENERATED_BODY()
	
public:	
 
	ADeskBase();

protected:
 
	virtual void BeginPlay() override;

	TArray<ADeskSectorBase*> Sectors;
	TArray<FIluminationGroup> IluminationSectors;
 
	int32 PlayerMoveStep = -1;
	AFigureBase* SaveMoveFigure;
	ADeskSectorBase* SelectSector;
	FRandomStream Random;
	int32 PlayerBlackId = -1;
	int32 PlaierAIid = -1;
	APlayerAI* PlayerAI;

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Desk")TSubclassOf<ADeskSectorBase>  DeskSectorClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Desk")float SectorSize = 100.0f;
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Desk")int32 LineSize = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chess")TSubclassOf<AFigureBase> pawnsChessClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chess")TArray< TSubclassOf<AFigureBase>> TopChessClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chess")TArray< FFigureLevelupSettings> LevelupFigureSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")TArray <FPlayerInfo> PlayerStatistic;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI") bool PlayerOponentAi = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI") TSubclassOf < APlayerAI> PlayerAIClass;

	//Delegats
	UPROPERTY(BlueprintAssignable) FOnAttackFigure OnAttackFigure;
	UPROPERTY(BlueprintAssignable) FOnNextStep OnNextStep;
	UPROPERTY(BlueprintAssignable) FOnFinisGame OnFinisGame;
	UPROPERTY(BlueprintAssignable) FOnModifayerGame OnModifayerGame;
	UPROPERTY(BlueprintAssignable) FOnFigureLevelUp OnFigureLevelUp;

	//Function
	FPlayerInfo& GetPlayerStatistic(int32 PlayerId);
	void AddPlaterFigure(int32 PlayerId, AFigureBase*  AddFigur);
	void RemovePlaterFigure(int32 PlayerId, AFigureBase*  AddFigur);
	void SetPlayerKing(AKingBase* KingFigure, int32 PlayerIndex);
	AKingBase* GetPlayerKing(int32 PlayerIndex);
	void DisableOldIlumination(SectorIluminationType IluminationType);
	void SetSectorIlumination(SectorIluminationType IluminationType, TArray<int32> SectorLocations, bool isAttackCheck);
	void IluminationFigure(AFigureBase* MoveFigure); //check ilumination Type
	
	UFUNCTION(BlueprintPure)int32 GetPlayerPower(int32 PlayerId);
	void SetPlayerPower(int32 PlayerId, int32 Value);
	UFUNCTION(BlueprintCallable) void StartInitDesk(int32 IndexBlackPlayer);

	UFUNCTION(BlueprintPure) int32 GetPlayerMoveStep();
	bool isNoSaveMoveFigure();
	void SetSaveMoveFigure(AFigureBase* MoveFigure);
	void BeckMoveFigure();
	void SelectFigure(AFigureBase* MoveFigure);
	bool GetPlayerCheckMode(int32 PlayerIndex);
	void CheckEndGame(int32 PlayerIndex);
	int32 FinfCoincidencesLocs(AKingBase* King, TArray<int32> AtackLocation, TArray<int32> MoveLocation);
 
	bool CheckCheckmate(int32 PlayerIndex);

	GameStepMofificationType CheckShach(int32 PlayerIndex, int32 IgnoreLocation);

	void NextStep(); //end step move next player
	void StartFiguresLeveUp(AFigureBase* UpdateFigure);

	UFUNCTION(BlueprintCallable) void EndFiguresLeveUp(AFigureBase* UpdateFigure, TSubclassOf<AFigureBase> UpFigureClass);

	bool GetDisableCheckMode();
	void SetSelectSector(ADeskSectorBase* Sector);
	void MoveFigureToSelectSector();
	bool CheckPlayer(AFigureBase* MoveFigure);
	void CreateAllChess(int32 PlayerIndex, bool isBlack);

	AFigureBase* CreateChess(int32 PlayerIndex, int32 IndexLine, int32 IndexColunm, TSubclassOf<AFigureBase> FigureClass, bool isBlack);
 
	ADeskSectorBase* GetDeskSector(int32 Line, int32 Colunm);
	void CreateDesk();
	CheckWaiType CheckFreeWay(int32 SectorIndex, int32 PlayerOwnerCheck);
	FDeskSectorLocation GetSectorLocation(int32 SectorIndex);
	bool isFreeWay(TArray<int32> WaySetctors);  // no calculet start & end index
	ADeskSectorBase* GetSectorFromIndex(int32 SectorIndex);
	AFigureBase* GetFigureFromIndex(int32 SectorIndex);
};
