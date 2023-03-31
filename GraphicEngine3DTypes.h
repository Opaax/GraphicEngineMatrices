#pragma once

#include <vector>
#include <fstream>
#include <sstream>

struct Vector3
{
    float x, y, z;
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

