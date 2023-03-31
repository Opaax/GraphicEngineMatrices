#pragma once

#include "GameConsoleEngine.h"
#include "GraphicEngine3DTypes.h"

#include <cstdlib>

#define PI 3.141559265359;

class StaticMath
{
public:
    /////////////// VECTORS //////////////////

    static Vector3 ScaleVector(const Vector3& InVector, float Scale)
    {
        Vector3 lScaleVector;

        lScaleVector.x = InVector.x * Scale;
        lScaleVector.y = InVector.y * Scale;
        lScaleVector.z = InVector.z * Scale;

        return lScaleVector;
    }
    
    static Vector3 CrossProduct(const Vector3& VecA, const Vector3& VecB)
    {
        Vector3 lCrossVector;

        lCrossVector.x = VecA.y * VecB.z - VecA.z * VecB.y;
        lCrossVector.y = VecA.z * VecB.x - VecA.x * VecB.z;
        lCrossVector.z = VecA.x * VecB.y - VecA.y * VecB.x;

        return lCrossVector;
    }
    
    static Vector3 VectorNormalized(const Vector3& InVector)
    {
        Vector3 lUnitVector;
        float lVecLength = VectorSQRTMagnitude(InVector);

        lUnitVector.x = InVector.x / lVecLength;
        lUnitVector.y = InVector.y / lVecLength;
        lUnitVector.z = InVector.z / lVecLength;

        return lUnitVector;
    }

    static Vector3 OppositeVector(const Vector3& InVector)
    {
        Vector3 lOppositeVec;

        lOppositeVec.x = InVector.x * -1;
        lOppositeVec.y = InVector.y * -1;
        lOppositeVec.z = InVector.z * -1;

        return lOppositeVec;
    }

    static Vector3 VectorDirection(const Vector3& VecA, const Vector3& VecB)
    {
        return VectorDirection(VecA, VecB, false);
    }

    static Vector3 VectorDirection(const Vector3& VecA, const Vector3& VecB, bool Normalized)
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

    static Vector3 SubstractVectors(const Vector3& VecA, const Vector3& VecB)
    {
        Vector3 lSubVector;

        lSubVector.x = VecA.x - VecB.x;
        lSubVector.y = VecA.y - VecB.y;
        lSubVector.z = VecA.z - VecB.z;

        return lSubVector;
    }

    static Vector3 AddVectors(const Vector3& VecA, const Vector3& VecB)
    {
        Vector3 lAddVector;

        lAddVector.x = VecA.x + VecB.x;
        lAddVector.y = VecA.y + VecB.y;
        lAddVector.z = VecA.z + VecB.z;

        return lAddVector;
    }
    
    static float DotProduct(const Vector3& VecA, const Vector3& VecB)
    {
        return VecA.x * VecB.x + VecA.y * VecB.y + VecA.z * VecB.z;
    }

    /// <summary>
    /// Return the sqrt vector Length
    /// </summary>
    static float VectorSQRTMagnitude(const Vector3& InVector)
    {
        return std::sqrt(InVector.x * InVector.x + InVector.y * InVector.y + InVector.z * InVector.z);
    }

    /// <summary>
    /// Return the Hypotenuse which is the vector Length
    /// </summary>
    static float VectorSQRTMagnitudeHypot(const Vector3& InVector)
    {
        return std::hypot(InVector.x, InVector.y, InVector.z);
    }

    //////////// TRIGO //////////////

    /// <summary>
    /// Get Radient from Degree
    /// </summary>
    static float DegreeToRad(const float& Degree)
    {
        return (Degree / 180.0f) * PI;
    }

    /// <summary>
    /// Get Degree from Radient 
    /// </summary>
    static float RadToDegree(const float& Rad)
    {
        float lPI = PI;

        return (Rad / lPI) * 180.0f;
    }
};

