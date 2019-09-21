// Fill out your copyright notice in the Description page of Project Settings.


#include "Repeater.h"

// Sets default values
ARepeater::ARepeater()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent->SetMobility(EComponentMobility::Movable);

}

void ARepeater::SpawnCircle()
{
	FVector l = GetActorLocation();
	float inc = 360.f / Items;
	float angle = 0;
	for (int i = 0; i < Items; i++)
	{
		if (!isAngleIgnored(angle))

		{
			float  x = l.X + (Radius * UKismetMathLibrary::DegCos(angle));
			float  y = l.Y + (Radius * UKismetMathLibrary::DegSin(angle));

			FVector loc = FVector(x, y, l.Z);

			// rot
			FRotator rot = GetActorRotation();
			if (FaceInward)
			{
				rot = (l - loc).Rotation();
			}
			else if (FaceOutward)
			{
				rot = (loc - l).Rotation();
			}
			rot = UKismetMathLibrary::ComposeRotators(rot, RotationOffset);

			// render mesh instance
			FTransform transform = FTransform();
			transform.SetLocation(loc);
			transform.SetRotation(FQuat(rot));
			transform.SetScale3D(Scale);
			hism->AddInstance(transform);
		}
		angle += inc;
	}

}

void ARepeater::SpawnLine()
{
	FVector l = GetActorLocation();

	FVector loc = l;
	FVector mir = l;

	for (int i = 0; i < Items; i++)
	{
		// rot
		FRotator rot = GetActorRotation();
		rot = UKismetMathLibrary::ComposeRotators(rot, RotationOffset);

		// render mesh instance
		FTransform transform = FTransform();
		transform.SetLocation(loc);
		transform.SetRotation(FQuat(rot));
		transform.SetScale3D(Scale);
		hism->AddInstance(transform);
		if (Mirror && i > 0)
		{
			transform.SetLocation(mir);
			hism->AddInstance(transform);
		}
		loc += LocationIncrement;
		FVector k = UKismetMathLibrary::NegateVector(LocationIncrement);
		k.Z = LocationIncrement.Z;
		mir += k;
	}
}

bool ARepeater::isAngleIgnored(float angle)
{
	bool rc = SkipDegreeRanges.ContainsByPredicate([angle](FVector range)
	{
		return angle >= range.X && angle <= range.Y;
	});
	return rc;
}

// Called when the game starts or when spawned
void ARepeater::BeginPlay()
{
	Super::BeginPlay();

}

void ARepeater::OnConstruction(const FTransform & Transform)
{
	// no spawn for preview actors
	if (!this->HasAnyFlags(EObjectFlags::RF_Transient))
	{
		this->GetWorldTimerManager().SetTimer(hSpawnTimer, this, &ARepeater::OnSpawn, 1.0f, false);
	}
}

// Called every frame
void ARepeater::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARepeater::OnSpawn()
{
	if (hism)
	{
		hism->ClearInstances();
		hism->DestroyComponent();
		hism = nullptr;
	}


	hism = NewObject<UHierarchicalInstancedStaticMeshComponent>(this, UHierarchicalInstancedStaticMeshComponent::StaticClass(), NAME_None, RF_Transactional);
	hism->SetMobility(EComponentMobility::Static);
	hism->LightmapType = ELightmapType::ForceSurface;
	hism->bCastDynamicShadow = false;
	hism->bCastStaticShadow = true;
	hism->SetRelativeLocation(FVector::ZeroVector);
	hism->AttachTo(this->RootComponent);
	hism->RegisterComponent();
	hism->SetStaticMesh(mesh);
	//this->FinishAndRegisterComponent(hism);

	if (RenderCircle)
	{
		SpawnCircle();
	}
	if (RenderLine)
	{
		SpawnLine();
	}

}

