#pragma once

#include <vector>

struct Vector3
{
    float x, y, z;
};

struct Triangle
{
    Vector3 points[3];
};

struct Matrix4x4
{
    float matrice[4][4] = { 0 };

    void MultiplyMatrixVector(Vector3& Input, Vector3& Output)
    {
        Output.x = Input.x * matrice[0][0] + Input.y * matrice[1][0] + Input.z * matrice[2][0] + matrice[3][0];
        Output.y = Input.x * matrice[0][1] + Input.y * matrice[1][1] + Input.z * matrice[2][1] + matrice[3][1];
        Output.z = Input.x * matrice[0][2] + Input.y * matrice[1][2] + Input.z * matrice[2][2] + matrice[3][2];

        float lW = Input.x * matrice[0][3] + Input.y * matrice[1][3] + Input.z * matrice[2][3] + matrice[3][3];

        if (lW != 0.0f)
        {
            Output.x /= lW;
            Output.y /= lW;
            Output.z /= lW;
        }
    }
};

struct Mesh
{
    std::vector<Triangle> triangles;
};

struct Cube : public Mesh
{

};

