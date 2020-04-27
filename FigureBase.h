// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Systemt/ChessGameInstance.h"
 
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "FigureBase.generated.h"

class ADeskSectorBase;
 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelUpFigure, AFigureBase* ,Figure);

UCLASS()
class MYPROJECT_API AFigureBase : public AActor
{
	GENERATED_BODY()
		
public:	
 
	AFigureBase();

protected:
	

	FDeskSectorLocation GetCurrentLocation();
	bool isEqualsSpawnSector(FDeskSectorLocation SectorLocation);
	virtual void BeginPlay() override;
	int32 IndexPlayerOwner = -1;
	UStaticMeshComponent* Model;
	
	ADeskSectorBase* CurrentDeskSector;
	ADeskSectorBase* SpawnDeskSector;
	int32 WorldSize = 0;
	bool isBlockAttack =  false;
 
public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Models")UMaterialInterface* MaterialWhite;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Models")UMaterialInterface* MaterialBlack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Balance")int32 Power = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Balance")bool isEqualsMoveAndAttack=false;


	//function
	bool GetBlockAttack() { 
		return isBlockAttack; 
	}
	virtual void PostCreate() {
	}
	UPROPERTY(BlueprintAssignable) FOnLevelUpFigure OnLevelUpFigure;

	UFUNCTION(BlueprintPure) int32 GetOwnerPlayerIndex() {
		return IndexPlayerOwner;
	}
	int32 GetFigurePover() {
		return Power;
	}
	int32 GetWorldSize() {
		return WorldSize;
	}

	int32 GetExisMove() {
		return (IndexPlayerOwner == 0) ? 1 : -1;
	}
	bool GetEqualsMoveAndAttack() {
		return isEqualsMoveAndAttack;
	}

	UFUNCTION(BlueprintCallable) void SetCurrentDeskSector(ADeskSectorBase* DeskSector) {
		CurrentDeskSector = DeskSector;
	}
	ADeskSectorBase* GetCurrentDeskSector() {
		return CurrentDeskSector;
	}
 
	UFUNCTION(BlueprintImplementableEvent) void ShowSlelectFigure(bool isShow);
	UFUNCTION(BlueprintPure) bool CheckOwnerIndex(int32 Index) {
		return Index == GetOwnerPlayerIndex();
	}
	UFUNCTION(BlueprintCallable)  void PlayerClick(); 

	virtual bool CheckMoveBlock(FDeskSectorLocation SectorLocation);
	virtual bool CheckAttackBlock(FDeskSectorLocation SectorLocation);
	virtual TArray<int32> GetMoveSectors() {    //return all move positions
		TArray<int32> result;
		return result;
	}
	virtual TArray<int32> GetAttackSectors(){ //return all attack positions
		return GetMoveSectors();
	}

 

	bool FindEqualdLocation(int32 FindLocation, TArray<int32> MoveLocations);
	void ClearSector();

	void DestroyFigure();
	void BeckMoveFigure();
	void SpawnInit(int32 PlayerIndex, bool isBlack, ADeskSectorBase* DeskSector, int32 LineSize);


};
