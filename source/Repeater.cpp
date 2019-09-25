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

void ARepeater::SpawnPolygon()
{
	FVector l = GetActorLocation();

	FVector loc = l;
	if (Sides < 3)
	{
		Sides = 3;
	}
	// anchors
	TArray<FVector> anchors;
	for (int n = 0; n < Sides; n++)
	{
		float angle = 2 * UKismetMathLibrary::GetPI() *n / Sides;
		float x = Radius * UKismetMathLibrary::Cos(angle);
		float y = Radius * UKismetMathLibrary::Sin(angle);
		FVector p = FVector(l.X + x, l.Y + y, l.Z);
		FVector k = l + GetActorRotation().Quaternion() * (p - l);
		anchors.Add(k);
	}

	// side item count
	if (ItemPerSide <= 0)
	{
		ItemPerSide = 1;
	}
	else	if (ItemPerSide > 250)
	{
		ItemPerSide = 250;
	}
	TArray<FVector> points;
	for (int i = 1; i < anchors.Num(); i++)
	{
		AddPoints(anchors[i - 1], anchors[i], ItemPerSide, points);
	}
	AddPoints(anchors.Last(), anchors[0], ItemPerSide, points);
	if (RenderPolygonVertices)
	{
		points.Append(anchors);
	}

	FVector locoffset = FVector::ZeroVector;
	float totalRotations = 0;
	if (Rotations <= 0)
	{
		Rotations = 1;
	}
	while (totalRotations < Rotations)
	{
		for (int i = 0; i < points.Num(); i++)
		{
			// calc angle
			FVector vdir = GetActorLocation();
			FVector dir = (points[i] - GetActorLocation());
			dir.Normalize();
			float fbdot = FVector::DotProduct(GetActorForwardVector(), dir);
			float lrdot = FVector::DotProduct(GetActorRightVector(), dir);
			float angle = UKismetMathLibrary::DegAcos(fbdot);
			if (lrdot < 0)
			{
				angle = 180 + (180 - angle);
			}

			if (!isAngleIgnored(angle))
			{
				// rot
				FRotator rot = RotationOffset;
				FVector fwd = FVector::ZeroVector;
				if (FaceInward)
				{
					fwd = l - points[i];
					
				}
				else if (FaceOutward)
				{
					fwd = points[i] - l;
				}
				rot = UKismetMathLibrary::ComposeRotators(rot, fwd.Rotation());
				//rot = UKismetMathLibrary::ComposeRotators(rot, RotationOffset);
				//fwd = rot.RotateVector(fwd);


				// render mesh instance
				FTransform transform = FTransform();

				// add offset
				FVector p = points[i];
				fwd.Normalize();
				FRotator rotright = FRotator(0, 90, 0);
				FVector right = rotright.RotateVector(fwd);
				p += GetActorUpVector() * (locoffset.Z / ItemPerSide);
				p += right * (locoffset.Y / ItemPerSide);
				FVector fp = p + (fwd * (locoffset.X / ItemPerSide));
				//DrawDebugDirectionalArrow(GetWorld(), p, fp, 5, FColor::Green, false, 60, 0, 5);
				transform.SetLocation(fp);

				transform.SetRotation(FQuat(rot));
				transform.SetScale3D(Scale);
				hism->AddInstance(transform);


			}
			locoffset += LocationIncrement;
			totalRotations += 1 / (float)points.Num();
			if (totalRotations > Rotations)
			{
				break;
			}
		}
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

void ARepeater::AddPoints(FVector start, FVector end, int items, TArray<FVector> &points)
{
	FVector dir = end - start;
	float len = dir.Size();
	float offset = len / (items + 1);
	dir.Normalize();
	for (int i = 0; i < items; i++)
	{
		FVector p = start + (dir * (offset*(i + 1)));
		points.Add(p);
	}

	return;
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


	if (RenderLine)
	{
		SpawnLine();
	}
	if (RenderPolygon)
	{
		SpawnPolygon();
	}

}

