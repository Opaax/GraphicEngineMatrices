#pragma once

#include "GameConsoleEngine.h"
#include "GraphicEngine3DTypes.h"

#define PI = 3.141559265359;

class StaticMath
{
public:
	//Vectors
	Vector3 static ScaleVector(const Vector3& InVector, float Scale);
	Vector3 static CrossProduct(const Vector3& VecA, const Vector3& VecB);
	Vector3 static VectorNormalized(const Vector3& InVector);
	Vector3 static OppositeVector(const Vector3& InVector);
	Vector3 static VectorDirection(const Vector3& VecA, const Vector3& VecB);
	Vector3 static VectorDirection(const Vector3& VecA, const Vector3& VecB, bool Normalized);

	float static DotProduct(const Vector3& VecA, const Vector3& VecB);
	float static VectorSQRTMagnitude(const Vector3& InVector);
	float static VectorSQRTMagnitudeHypot(const Vector3& InVector);

	//Trigo
	float static DegreeToRad(const float& Degree);
	float static RadToDegree(const float& Rad);
};

