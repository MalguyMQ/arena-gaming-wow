// Copyright Flagcat Studio. All Rights Reserved.

#include "Arena/ArenaBuilder.h"
#include "System/ArenaDataSubsystem.h"
#include "ArenaCore.h"
#include "Components/StaticMeshComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

AArenaBuilder::AArenaBuilder()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bAlwaysRelevant = true;
	SetReplicatingMovement(false);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	KeyLight = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("KeyLight"));
	KeyLight->SetupAttachment(Root);
	KeyLight->SetRelativeRotation(FRotator(-50.f, 45.f, 0.f));
	KeyLight->Intensity = 8.f;
	KeyLight->CastShadows = true;
	KeyLight->LightColor = FColor(255, 244, 214);

	FillLight = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("FillLight"));
	FillLight->SetupAttachment(Root);
	FillLight->SetRelativeRotation(FRotator(-25.f, -135.f, 0.f));
	FillLight->Intensity = 2.5f;
	FillLight->CastShadows = false;
	FillLight->LightColor = FColor(180, 200, 255);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderFinder(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));

	if (CubeFinder.Succeeded())
	{
		CubeMesh = CubeFinder.Object;
	}
	if (CylinderFinder.Succeeded())
	{
		CylinderMesh = CylinderFinder.Object;
	}
	if (MaterialFinder.Succeeded())
	{
		BaseMaterial = MaterialFinder.Object;
	}
}

void AArenaBuilder::BeginPlay()
{
	Super::BeginPlay();

	Rebuild();

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UArenaDataSubsystem* Data = GI->GetSubsystem<UArenaDataSubsystem>())
		{
			ReloadHandle = Data->OnDataReloaded.AddUObject(this, &AArenaBuilder::Rebuild);
		}
	}
}

void AArenaBuilder::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ReloadHandle.IsValid())
	{
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UArenaDataSubsystem* Data = GI->GetSubsystem<UArenaDataSubsystem>())
			{
				Data->OnDataReloaded.Remove(ReloadHandle);
			}
		}
	}
	Super::EndPlay(EndPlayReason);
}

void AArenaBuilder::Rebuild()
{
	ClearBuilt();

	UGameInstance* GI = GetGameInstance();
	UArenaDataSubsystem* Data = GI ? GI->GetSubsystem<UArenaDataSubsystem>() : nullptr;
	const UDataTable* Layout = Data ? Data->GetTable(UArenaDataSubsystem::TableId_ArenaLayout) : nullptr;
	if (!Layout)
	{
		UE_LOG(LogArena, Error, TEXT("ArenaBuilder : table ArenaLayout introuvable, arène non construite."));
		return;
	}

	Layout->ForeachRow<FArenaLayoutRow>(TEXT("ArenaBuilder"), [this](const FName& RowName, const FArenaLayoutRow& Row)
	{
		SpawnElement(RowName, Row);
	});

	UE_LOG(LogArena, Log, TEXT("ArenaBuilder : %d éléments construits."), BuiltComponents.Num());
}

void AArenaBuilder::SpawnElement(const FName& RowName, const FArenaLayoutRow& Row)
{
	if (Row.ElementType == FName("Spawn"))
	{
		return;
	}

	UStaticMesh* Mesh = (Row.Shape == FName("Cylinder")) ? CylinderMesh.Get() : CubeMesh.Get();
	if (!Mesh)
	{
		UE_LOG(LogArena, Error, TEXT("ArenaBuilder : mesh moteur introuvable pour la ligne '%s'."), *RowName.ToString());
		return;
	}

	UStaticMeshComponent* Component = NewObject<UStaticMeshComponent>(this);
	Component->SetupAttachment(Root);
	Component->SetStaticMesh(Mesh);
	Component->SetMobility(EComponentMobility::Movable);
	Component->SetRelativeLocation(FVector(Row.X, Row.Y, Row.Z));
	Component->SetRelativeRotation(FRotator(0.f, Row.Yaw, 0.f));
	Component->SetRelativeScale3D(FVector(Row.ScaleX, Row.ScaleY, Row.ScaleZ));
	Component->SetCollisionProfileName(TEXT("BlockAll"));
	Component->RegisterComponent();

	if (BaseMaterial)
	{
		UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(BaseMaterial, Component);
		MID->SetVectorParameterValue(TEXT("Color"), FLinearColor(Row.ColorR, Row.ColorG, Row.ColorB));
		Component->SetMaterial(0, MID);
	}

	if (Row.ElementType == FName("Gate"))
	{
		Component->ComponentTags.Add(FName("ArenaGate"));
	}

	BuiltComponents.Add(Component);
}

void AArenaBuilder::ClearBuilt()
{
	for (UStaticMeshComponent* Component : BuiltComponents)
	{
		if (Component)
		{
			Component->DestroyComponent();
		}
	}
	BuiltComponents.Empty();
}
