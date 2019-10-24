// Fill out your copyright notice in the Description page of Project Settings.


#include "SplineGenerator.h"

// Sets default values
ASplineGenerator::ASplineGenerator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//this->RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	//USplineComponent* splineComponent = NewObject<USplineComponent>(this, FName(TEXT("Spline")));
	splineComponent = CreateDefaultSubobject<USplineComponent>(FName(TEXT("Spline")));
	splineComponent->SetRelativeLocation(FVector::ZeroVector);
	splineComponent->SetRelativeScale3D(FVector(1));
	splineComponent->bShouldVisualizeScale = true;
	//splineComponent->SetupAttachment(this->RootComponent);
	this->SetRootComponent(splineComponent);
}

// Called when the game starts or when spawned
void ASplineGenerator::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ASplineGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASplineGenerator::OnConstruction(const FTransform & Transform)
{
	// no spawn for preview actors
	if (!this->HasAnyFlags(EObjectFlags::RF_Transient))
	{
		this->GetWorldTimerManager().SetTimer(hSpawnTimer, this, &ASplineGenerator::OnSpawn, 0.2f, false);
	}
}

// https://forums.unrealengine.com/development-discussion/c-gameplay-programming/1417486-trouble-implementing-splinemeshcomponents
void ASplineGenerator::OnSpawn()
{
	//this->UnregisterAllComponents();

	//GenerateSpline();

	GenerateComponents(splineComponent);
}

void ASplineGenerator::GenerateComponents(USplineComponent * spline)
{
	if (!spline)
	{
		return;
	}
	FVector locStart, tanStart, locEnd, tanEnd, ss, es;
	FVector2D sscale;
	FVector2D escale;
	sscale = escale = scale;

	// Clean up stale mesh components
	if (components.Num() > 0)
	{
		for (int32 i = 0; i < components.Num(); i++)
		{
			if (components[i])
			{
				components[i]->DetachFromParent();
				components[i]->DestroyComponent();
			}
		}
		components.Empty();
	}
	if (showMesh && segmentLength > 1)
	{
		//int32 segs = (int32)(spline->GetSplineLength() / segmentLength);
		float total = spline->GetSplineLength();
		TArray<FVector2D> segs;
		float current = 0;
		while (current < total)
		{
			segs.Add(FVector2D(current, UKismetMathLibrary::FMin(current + segmentLength, total)));
			current += segmentLength;
		}
		for (int32 i = 0; i < segs.Num(); i++)
		{
			USplineMeshComponent *splineMesh = NewObject<USplineMeshComponent>(this);
			splineMesh->RegisterComponent();
			UE_LOG(LogTemp, Display, TEXT("Added new USplineMeshComponent: %s"), *splineMesh->GetName());
			splineMesh->CreationMethod = EComponentCreationMethod::UserConstructionScript;
			GetLTSAtDistance(segs[i].X, locStart, tanStart, ss);
			GetLTSAtDistance(segs[i].Y, locEnd, tanEnd, es);
			tanStart = tanStart.GetClampedToMaxSize(segmentLength);
			tanEnd = tanEnd.GetClampedToMaxSize(segmentLength);
			UE_LOG(LogTemp, Display, TEXT("Start/End location and tangent: %d %s / %s"), i, *locStart.ToString(), *locEnd.ToString());

			splineMesh->SetMobility(EComponentMobility::Type::Movable);
			splineMesh->SetForwardAxis(forwardAxis);
			splineMesh->SetStaticMesh(mesh);
			splineMesh->SetMaterial(0, material);
			splineMesh->SetStartAndEnd(locStart, tanStart, locEnd, tanEnd);
			splineMesh->SetStartScale(FVector2D(ss.Y, ss.Z));
			splineMesh->SetEndScale(FVector2D(es.Y, es.Z));
			splineMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			splineMesh->AttachToComponent(spline, FAttachmentTransformRules::FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
			components.Add(splineMesh);
		}
	}

}

void ASplineGenerator::GetLTSAtDistance(float distance, FVector & loc, FVector & tan, FVector & scale)
{
	loc = splineComponent->GetLocationAtDistanceAlongSpline(distance, ESplineCoordinateSpace::Local);
	tan = splineComponent->GetTangentAtDistanceAlongSpline(distance, ESplineCoordinateSpace::Local);
	scale = splineComponent->GetScaleAtDistanceAlongSpline(distance);

}
