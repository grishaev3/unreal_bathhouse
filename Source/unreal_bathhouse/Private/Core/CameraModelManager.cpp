#include "CameraModelManager.h"

#include "CameraBase.h"
#include "LinearRandom.h"
#include "StaticCamera.h"

// --- REALIZATION: FCameraModelManager ---
FVector FCameraModelManager::LinearLerp(float NormalizedTime, UCameraBase* Model)
{
	if (!Model) return FVector::ZeroVector;

	// Прямой полиморфный Lerp без искажения осей
	return FMath::Lerp(Model->GetA(), Model->GetB(), NormalizedTime);
}

FCameraModelManager::FCameraModelManager(FBoundParameters& BoundParameters)
{
	float DurationInSeconds = 10.0f;
	float OftenFreq = 0.5f;
	float RarelyFreq = 0.3f;
	float StaticFreq = (1.0f - (OftenFreq + RarelyFreq)) / 6.0f;

	FVector Static0_From = FVector(6.00f, -0.63f, 1.97f) * 100.0f;
	FVector Static0_A = FVector(0.00f, -5.00f, 1.97f) * 100.0f;
	FVector Static0_B = FVector(0.00f, 5.00f, 1.97f) * 100.0f;

	CameraModels.Add(std::make_shared<FLinearBase>(OftenFreq, DurationInSeconds, LinearLerp, TEXT("Linear"), BoundParameters, ECameraDirectionType::Center));
	CameraModels.Add(std::make_shared<FLinearRandom>(RarelyFreq, DurationInSeconds, LinearLerp, TEXT("Random"), BoundParameters, ECameraDirectionType::Center));
	CameraModels.Add(std::make_shared<FStaticCamera>(StaticFreq, DurationInSeconds, [Static0_From](float, UCameraBase*) { return Static0_From; }, LinearLerp, Static0_A, Static0_B, TEXT("Static0")));

	for (int32 i = 0; i < CameraModels.Num(); ++i)
	{
		CameraModels[i]->Index = i;
	}

	std::vector<double> Probabilities;
	for (const auto& Model : CameraModels)
	{
		Probabilities.push_back((double)Model->Freq);
	}

	UniqueRandomProvider = std::make_unique<FUniqueRandom>(TEXT("CameraModels"), 0, CameraModels.Num(), Probabilities);

	CurrentModelIndex = UniqueRandomProvider->Next();
}

UCameraBase* FCameraModelManager::GetActiveModel() const
{
	for (const auto& Model : CameraModels)
	{
		if (Model->Index == CurrentModelIndex)
		{
			return Model.get();
		}
	}

	return nullptr;
}

void FCameraModelManager::ResetModel()
{
	CurrentModelIndex = UniqueRandomProvider->Next(CurrentModelIndex);
}

bool FCameraModelManager::ActiveModelUsesBounds() const
{
	UCameraBase* Active = GetActiveModel();
	if (!Active)
	{
		return false;
	}

	return Active->UsesBounds();
}