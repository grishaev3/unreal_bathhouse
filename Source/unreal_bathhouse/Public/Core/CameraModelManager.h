#pragma once

#include <memory>
#include "Math/Vector.h"

#include "DirectorCameraTypes.h"
#include "CameraBase.h"
#include "UniqueRandom.h"

class FCameraModelManager
{
private:
    int32 CurrentModelIndex;

    TArray<std::shared_ptr<UCameraBase>> CameraModels;

    std::unique_ptr<class FUniqueRandom> UniqueRandomProvider; // Используем class-forward declaration

    static FUnityVector LinearLerp(float NormalizedTime, UCameraBase* Model);

public:
    FCameraModelManager(FBoundParameters& BoundParameters);

    UCameraBase* GetActiveModel() const;

    bool ActiveModelUsesBounds() const;

    void ResetModel();
};