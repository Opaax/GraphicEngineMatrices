#include "StaticMath.h"
#include "StaticMath.h"
#include "StaticMath.h"
#include "StaticMath.h"
#include "StaticMath.h"
#include "StaticMath.h"
#include "StaticMath.h"
#include "StaticMath.h"
#include "StaticMath.h"
#include <cstdlib>

Vector3 StaticMath::ScaleVector(const Vector3& InVector, float Scale)
{
    return Vector3(InVector.x * Scale, InVector.y * Scale, InVector.z * Scale);
}

Vector3 StaticMath::CrossProduct(const Vector3& VecA, const Vector3& VecB)
{
    Vector3 lCrossVector;

    lCrossVector.x = VecA.x * VecB.z - VecA.z * VecB.z;
    lCrossVector.y = VecA.z * VecB.x - VecA.x * VecB.z;
    lCrossVector.z = VecA.x * VecB.y - VecA.y * VecB.x;

    return lCrossVector;
}

Vector3 StaticMath::VectorNormalized(const Vector3& InVector)
{
    Vector3 lUnitVector;
    float lVecLength = VectorSQRTMagnitude(InVector);

    lUnitVector.x = InVector.x * (1 / lVecLength);
    lUnitVector.y = InVector.y * (1 / lVecLength);
    lUnitVector.z = InVector.z * (1 / lVecLength);

    return lUnitVector;
}

Vector3 StaticMath::OppositeVector(const Vector3& InVector)
{
    Vector3 lOppositeVec;

    lOppositeVec.x = InVector.x * -1;
    lOppositeVec.y = InVector.y * -1;
    lOppositeVec.z = InVector.z * -1;

    return lOppositeVec;
}

Vector3 StaticMath::VectorDirection(const Vector3& VecA, const Vector3& VecB)
{
    return VectorDirection(VecA, VecB, false);
}

Vector3 StaticMath::VectorDirection(const Vector3& VecA, const Vector3& VecB, bool Normalized)
{
    Vector3 lDirection;

    lDirection.x = VecB.x - VecA.x;
    lDirection.y = VecB.y - VecA.y;
    lDirection.z = VecB.z - VecA.z;

    if (Normalized)
    {
        lDirection = VectorNormalized(lDirection);
    }

    return lDirection;
}

float StaticMath::DotProduct(const Vector3& VecA, const Vector3& VecB)
{
    return VecA.x * VecB.x + VecA.y * VecB.y + VecA.z * VecB.z;
}

float StaticMath::VectorSQRTMagnitude(const Vector3& InVector)
{
    return std::sqrt(std::pow(InVector.x,2) + std::pow(InVector.y, 2) + std::pow(InVector.z, 2));
}

float StaticMath::VectorSQRTMagnitudeHypot(const Vector3& InVector)
{
    return std::hypot(InVector.x, InVector.y, InVector.z);
}

float StaticMath::DegreeToRad(const float& Degree)
{
    return (Degree / 180.0f) * PI;
}

float StaticMath::RadToDegree(const float& Rad)
{
    return (Rad / PI) * 180.0f;
}
