#include "CameraBehaviour.h"

#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ACameraBehaviour::ACameraBehaviour()
{
	PrimaryActorTick.bCanEverTick = true;
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	RootComponent = CameraComponent;
}

void ACameraBehaviour::BeginPlay()
{
	Super::BeginPlay();

	Director = MakeUnique<FCameraDirector>();
}

void ACameraBehaviour::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UWorld* World = GetWorld();
	if (!Director || !World)
	{
		return;
	}

	const FCameraPose Pose = Director->Tick(World->GetDeltaSeconds(), UGameplayStatics::GetGlobalTimeDilation(World));

	// Единственная точка перевода координат Unity -> UE
	const FVector Location = Pose.From.ToUE(true);
	const FVector LookAt = Pose.At.ToUE(true);

	SetActorLocation(Location);
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(Location, LookAt));
}
