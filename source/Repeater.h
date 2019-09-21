// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"
#include "Runtime/Engine/Classes/Engine/StaticMeshActor.h"
#include "Runtime/Engine/Classes/Engine/StaticMeshSocket.h"
#include "Engine/Public/TimerManager.h"
#include "Public/DrawDebugHelpers.h"
#include "Runtime/Engine/Classes/Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Repeater.generated.h"

UCLASS()
class EMBER_API ARepeater : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARepeater();
private:
	FRandomStream rng;
	FTimerHandle hSpawnTimer;
	void SpawnCircle();
	void SpawnLine();
	bool isAngleIgnored(float angle);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void OnSpawn();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "General")
		UStaticMesh * mesh = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "General")
		int Items = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Room")
		UHierarchicalInstancedStaticMeshComponent *hism;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "General")
		FVector LocationOffset = FVector::ZeroVector;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "General")
		FRotator RotationOffset = FRotator::ZeroRotator;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "General")
		bool randomRotation = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "General")
		FVector Scale = FVector(1);
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "General|Line")
		bool RenderLine = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "General|Line")
		bool Mirror = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "General|Line")
		FVector LocationIncrement = FVector::ZeroVector;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "General|Circle")
		bool RenderCircle = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "General|Circle")
		bool FaceInward = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "General|Circle")
		bool FaceOutward = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "General|Circle")
		float Radius = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "General|Circle")
		TArray<FVector> SkipDegreeRanges;

};
