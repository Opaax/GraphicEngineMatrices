#pragma once

#include <vector>
#include <fstream>
#include <sstream>

struct Vector3
{
    float x, y, z,w; //Better to create a Vec4 class/struct

    Vector3& operator += (const Vector3& rhs)
    {
        this->x += rhs.x;
        this->y += rhs.y;
        this->z += rhs.z;
        this->w = 1;

        return *this;
    }

    Vector3& operator = (const Vector3& InVect)
    {
        this->x = InVect.x;
        this->y = InVect.y;
        this->z = InVect.z;
        this->w = 1.0f;

        return *this;
    }

    Vector3 operator - (const Vector3& InVect)
    {
        return {x - InVect.x, y - InVect.y,z - InVect.z,1 };
    }

    Vector3& operator -= (const Vector3& InVect)
    {
        this->x -= InVect.x;
        this->y -= InVect.y;
        this->z -= InVect.z;
        this->w = 1.0f;

        return *this;
    }

    Vector3& operator *= (const float Scale)
    {
        this->x *= Scale;
        this->y *= Scale;
        this->z *= Scale;
        this->w = 1.0f;

        return *this;
    }

    Vector3& operator /= (const float Scale)
    {
        this->x /= Scale;
        this->y /= Scale;
        this->z /= Scale;
        this->w = 1.0f;

        return *this;
    }
};

struct Triangle
{
    Vector3 points[3];

    wchar_t Symbol;
    short Color;
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

    static Matrix4x4 MakeIdentity()
    {
        Matrix4x4 lMatrix;

        lMatrix.matrice[0][0] = 1.0f;
        lMatrix.matrice[1][1] = 1.0f;
        lMatrix.matrice[2][2] = 1.0f;
        lMatrix.matrice[3][3] = 1.0f;

        return lMatrix;
    }

    static Matrix4x4 MakeProjectionMatrice(const float FOV, const float ScreenWidth,const float ScreenHeight, const float NearPlane, const float FarPlane, bool IsFOVRad = false)
    {
        float lRatio = (float)ScreenWidth / (float)ScreenHeight;

        return MakeProjectionMatrice(FOV, lRatio, NearPlane, FarPlane, IsFOVRad);
    }


    static Matrix4x4 MakeProjectionMatrice(float FOV, const float Ratio, const float NearPlane, const float FarPlane, bool IsFOVRad = false)
    {
        if (!IsFOVRad)
        {
            float lPI = 3.141559265359f;

            FOV = 1.0f / std::tanf(FOV * 0.5f / 180.f * lPI);
        }

        Matrix4x4 lProjectMatrice;

        lProjectMatrice.matrice[0][0] = Ratio * FOV;
        lProjectMatrice.matrice[1][1] = FOV;
        lProjectMatrice.matrice[2][2] = FarPlane / (FarPlane - NearPlane);
        lProjectMatrice.matrice[3][2] = (-FarPlane * NearPlane) / (FarPlane - NearPlane);
        lProjectMatrice.matrice[2][3] = 1.0f;
        lProjectMatrice.matrice[3][3] = 0.0f;

        return lProjectMatrice;
    }

    static Matrix4x4 MakeRotationMatrice() 
    {

    }

    static Matrix4x4 MakeRotationXMatrice(float AngleRad)
    {
        Matrix4x4 lRotMetrix;

        lRotMetrix.matrice[0][0] = 1.0f;
        lRotMetrix.matrice[1][1] = cosf(AngleRad);
        lRotMetrix.matrice[1][2] = sinf(AngleRad);
        lRotMetrix.matrice[2][1] = -sinf(AngleRad);
        lRotMetrix.matrice[2][2] = cosf(AngleRad);
        lRotMetrix.matrice[3][3] = 1.0f;

        return lRotMetrix;
    }

    static Matrix4x4 MakeRotationYMatrice(float AngleRad)
    {
        Matrix4x4 lRotMetrix;

        lRotMetrix.matrice[0][0] = cosf(AngleRad);
        lRotMetrix.matrice[0][2] = sinf(AngleRad);
        lRotMetrix.matrice[2][0] = -sinf(AngleRad);
        lRotMetrix.matrice[1][1] = 1.0f;
        lRotMetrix.matrice[2][2] = cosf(AngleRad);
        lRotMetrix.matrice[3][3] = 1.0f;

        return lRotMetrix;
    }

    static Matrix4x4 MakeRotationZMatrice(float AngleRad)
    {
        Matrix4x4 lRotMetrix;

        lRotMetrix.matrice[0][0] = cosf(AngleRad);
        lRotMetrix.matrice[0][1] = sinf(AngleRad);
        lRotMetrix.matrice[1][0] = -sinf(AngleRad);
        lRotMetrix.matrice[1][1] = cosf(AngleRad);
        lRotMetrix.matrice[2][2] = 1.0f;
        lRotMetrix.matrice[3][3] = 1.0f;

        return lRotMetrix;
    }

    Vector3 MultiplyVector(const Vector3& InVector)
    {
        Vector3 lNewVector;

        lNewVector.x = InVector.x * this->matrice[0][0] + InVector.y * this->matrice[1][0] + InVector.z * this->matrice[2][0] + InVector.w * this->matrice[3][0];
        lNewVector.y = InVector.x * this->matrice[0][1] + InVector.y * this->matrice[1][1] + InVector.z * this->matrice[2][1] + InVector.w * this->matrice[3][1];
        lNewVector.z = InVector.x * this->matrice[0][2] + InVector.y * this->matrice[1][2] + InVector.z * this->matrice[2][3] + InVector.w * this->matrice[3][2];
        lNewVector.w = InVector.x * this->matrice[0][3] + InVector.y * this->matrice[1][3] + InVector.z * this->matrice[2][3] + InVector.w * this->matrice[3][3];

        return lNewVector;
    }

    static Matrix4x4 MakeTranslation(const float x, const float y, const float z)
    {
        Matrix4x4 lTransMetrix;

        lTransMetrix.matrice[0][0] = 1;
        lTransMetrix.matrice[1][1] = 1;
        lTransMetrix.matrice[2][2] = 1;
        lTransMetrix.matrice[3][3] = 1;
        lTransMetrix.matrice[3][0] = x;
        lTransMetrix.matrice[3][1] = y;
        lTransMetrix.matrice[3][2] = z;

        return lTransMetrix;
    }
};

struct Mesh
{
    std::vector<Triangle> triangles;

    bool LoadFromOBJFile(std::string Path)
    {
        std::ifstream lStream(Path);

        if (!lStream.is_open())
            return false;

        //Cache
        std::vector<Vector3> lVertices;

        while (!lStream.eof())//end of the file
        {
            char lLine[128];
            char lJunk;

            lStream.getline(lLine, 128);

            std::stringstream  lStringStream;

            lStringStream << lLine;

            //OBJ common value
            if (lLine[0] == 'v')
            {
                Vector3 lVertex;

                lStringStream >> lJunk >> lVertex.x >> lVertex.y >> lVertex.z;

                lVertices.push_back(lVertex);
            }

            //get only index
            //Obj file can come with many values use for normal, texture... separate by '/'
            if (lLine[0] == 'f')
            {
                int lIndex[3];

                std::vector<std::string> lParts;
                std::istringstream iss(lLine);
                std::string part;

                //remove all space
                while (std::getline(iss, part, ' ')) {
                    lParts.push_back(part);
                }

                for (int i = 0; i < 3; i++) {
                    std::string lVertexStr = lParts[i + 1];
                    size_t pos = lVertexStr.find('/');
                    if (pos != std::string::npos) {
                        lVertexStr = lVertexStr.substr(0, pos);
                    }
                    lIndex[i] = std::stoi(lVertexStr);
                }

                //minus 1 because in OB file index start at one not a zero
                triangles.push_back({ lVertices[lIndex[0] - 1],lVertices[lIndex[1] - 1],lVertices[lIndex[2] - 1]});
            }
        }

        return true;
    }
};

struct Cube : public Mesh
{

};

