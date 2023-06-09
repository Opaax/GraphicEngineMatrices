// GraphicEngine3D.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <iostream>
#include <vector>
#include <algorithm>

#include "GameConsoleEngine.h"
#include "GraphicEngine3DTypes.h"
#include "StaticMath.h"

class Engine3D : public ConsoleGameEngine
{
protected:
    float m_rotationCube = 0;
    Cube m_cube;

    Matrix4x4 m_projectionMatrice;
    Matrix4x4 m_rotationZMatrice;
    Matrix4x4 m_rotationXMatrice;

    Vector3 m_fakeCameraLocation;
    Vector3 m_fakeLight;

public:
    Engine3D()
    {
        m_appName = L"3D Engine";
    }

    //Override
public:
    bool OnUserCreate() override
    {
        //OBJ get on internet some normal is inverted which cause weird things while drawing it
        if (!m_cube.LoadFromOBJFile("Obj/Javidx9Ship.obj"))
        {
            //Hard code cube
            m_cube.triangles =
            {
                // SOUTH
                { 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
                { 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

                // EAST                                                      
                { 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
                { 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

                // NORTH                                                     
                { 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
                { 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

                // WEST                                                      
                { 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
                { 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

                // TOP                                                       
                { 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
                { 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

                // BOTTOM                                                    
                { 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
                { 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },
            };
        }

        //Projection Matrix
        m_projectionMatrice = Matrix4x4::MakeProjectionMatrice(90.0f, GetScreenAspectRatio(), 0.1f, 1000.f, false);

        m_fakeLight = {0.0f,0.0f,-1.0f};

        return true;
    }

    bool OnUserUpdate(float ElapsedTime) override
    {
        Fill(0,0,GetScreenWidth(), GetScreenHeight(),PIXEL_SOLID,FG_BLACK);

        m_rotationCube += 1.0f * ElapsedTime;

        m_rotationZMatrice.matrice[0][0] = std::cosf(m_rotationCube);
        m_rotationZMatrice.matrice[0][1] = std::sinf(m_rotationCube);
        m_rotationZMatrice.matrice[1][0] = -std::sinf(m_rotationCube);
        m_rotationZMatrice.matrice[1][1] = std::cosf(m_rotationCube);
        m_rotationZMatrice.matrice[2][2] = 1;
        m_rotationZMatrice.matrice[3][3] = 1;

        m_rotationXMatrice.matrice[0][0] = 1;
        m_rotationXMatrice.matrice[1][1] = std::cosf(m_rotationCube * 0.5f);
        m_rotationXMatrice.matrice[1][2] = std::sinf(m_rotationCube * 0.5f);
        m_rotationXMatrice.matrice[2][1] = -std::sinf(m_rotationCube * 0.5f);
        m_rotationXMatrice.matrice[2][2] = std::cosf(m_rotationCube * 0.5f);;
        m_rotationXMatrice.matrice[3][3] = 1;

        std::vector<Triangle> lTrianglesToRaster;

        //Draw Triangles
        for (auto& tri : m_cube.triangles)
        {
            Triangle lProjectTriangle;
            Triangle lTranslatedTriangle;
            Triangle lRotateZTriangle;
            Triangle lRotateXZTriangle;

            //Rot Z
            m_rotationZMatrice.MultiplyMatrixVector(tri.points[0], lRotateZTriangle.points[0]);
            m_rotationZMatrice.MultiplyMatrixVector(tri.points[1], lRotateZTriangle.points[1]);
            m_rotationZMatrice.MultiplyMatrixVector(tri.points[2], lRotateZTriangle.points[2]);

            //Rot X
            m_rotationXMatrice.MultiplyMatrixVector(lRotateZTriangle.points[0], lRotateXZTriangle.points[0]);
            m_rotationXMatrice.MultiplyMatrixVector(lRotateZTriangle.points[1], lRotateXZTriangle.points[1]);
            m_rotationXMatrice.MultiplyMatrixVector(lRotateZTriangle.points[2], lRotateXZTriangle.points[2]);

            //OffSet Screen
            lTranslatedTriangle = lRotateXZTriangle;
            lTranslatedTriangle.points[0].z = lRotateXZTriangle.points[0].z + 8.0f;
            lTranslatedTriangle.points[1].z = lRotateXZTriangle.points[1].z + 8.0f;
            lTranslatedTriangle.points[2].z = lRotateXZTriangle.points[2].z + 8.0f;

            //Get triangle edges
            Vector3 lEdge1 = StaticMath::VectorDirection(lTranslatedTriangle.points[1], lTranslatedTriangle.points[0]);
            Vector3 lEdge2 = StaticMath::VectorDirection(lTranslatedTriangle.points[2], lTranslatedTriangle.points[0]);

            //Get triangle normal
            Vector3 lNormal = StaticMath::VectorNormalized((StaticMath::CrossProduct(lEdge1, lEdge2)));

            //Get Vector to from triangle to camera
            Vector3 lToCamera = StaticMath::VectorNormalized(StaticMath::VectorDirection(lTranslatedTriangle.points[0],m_fakeCameraLocation,true));

            float lDot = StaticMath::DotProduct(lNormal, lToCamera);

            if(lDot > 0.0f)
            {
                float lDotLight = StaticMath::DotProduct(lNormal, m_fakeLight);

                CHAR_INFO lColor = GetGreyScaleColor(lDotLight);

                //Set those values only for the projected triangles
                lProjectTriangle.Color = lColor.Attributes;
                lProjectTriangle.Symbol = lColor.Char.UnicodeChar;

                //Project triangles 3D to 2D
                m_projectionMatrice.MultiplyMatrixVector(lTranslatedTriangle.points[0], lProjectTriangle.points[0]);
                m_projectionMatrice.MultiplyMatrixVector(lTranslatedTriangle.points[1], lProjectTriangle.points[1]);
                m_projectionMatrice.MultiplyMatrixVector(lTranslatedTriangle.points[2], lProjectTriangle.points[2]);

                //Scale into view
                lProjectTriangle.points[0].x += 1;
                lProjectTriangle.points[0].y += 1;
                lProjectTriangle.points[1].x += 1;
                lProjectTriangle.points[1].y += 1;
                lProjectTriangle.points[2].x += 1;
                lProjectTriangle.points[2].y += 1;

                lProjectTriangle.points[0].x *= 0.5f * (float)GetScreenWidth();
                lProjectTriangle.points[0].y *= 0.5f * (float)GetScreenHeight();
                lProjectTriangle.points[1].x *= 0.5f * (float)GetScreenWidth();
                lProjectTriangle.points[1].y *= 0.5f * (float)GetScreenHeight();
                lProjectTriangle.points[2].x *= 0.5f * (float)GetScreenWidth();
                lProjectTriangle.points[2].y *= 0.5f * (float)GetScreenHeight();

                lTrianglesToRaster.push_back(lProjectTriangle);
            }
        }

        //sort triangles from back to front
        //just a console engine can have some artefact anyway
        std::sort(lTrianglesToRaster.begin(), lTrianglesToRaster.end(), [](Triangle& t1, Triangle& t2) 
            {
                float lZ1 = (t1.points[0].z + t1.points[1].z + t1.points[2].z) / 3.0f;
                float lZ2 = (t2.points[0].z + t2.points[1].z + t2.points[2].z) / 3.0f;
                return lZ1 > lZ2;
            });

        //Keep 'lProjectTriangle' name for loop only for convenient purpose
        for (auto& lProjectTriangle : lTrianglesToRaster)
        {
            FillTriangle(lProjectTriangle.points[0].x, lProjectTriangle.points[0].y,
                lProjectTriangle.points[1].x, lProjectTriangle.points[1].y,
                lProjectTriangle.points[2].x, lProjectTriangle.points[2].y,
                lProjectTriangle.Symbol, lProjectTriangle.Color);

            DrawTriangle
            (
                lProjectTriangle.points[0].x, lProjectTriangle.points[0].y,
                lProjectTriangle.points[1].x, lProjectTriangle.points[1].y,
                lProjectTriangle.points[2].x, lProjectTriangle.points[2].y,
                PIXEL_SOLID, FG_BLACK
            );
        }

        return true;
    }
};

int main()
{
    Engine3D lEngine;

    if (lEngine.ConstructConsole(256, 240, 4, 4))
    {
        lEngine.Start();
    }

    return 0;
}

// Exécuter le programme : Ctrl+F5 ou menu Déboguer > Exécuter sans débogage
// Déboguer le programme : F5 ou menu Déboguer > Démarrer le débogage

// Astuces pour bien démarrer : 
//   1. Utilisez la fenêtre Explorateur de solutions pour ajouter des fichiers et les gérer.
//   2. Utilisez la fenêtre Team Explorer pour vous connecter au contrôle de code source.
//   3. Utilisez la fenêtre Sortie pour voir la sortie de la génération et d'autres messages.
//   4. Utilisez la fenêtre Liste d'erreurs pour voir les erreurs.
//   5. Accédez à Projet > Ajouter un nouvel élément pour créer des fichiers de code, ou à Projet > Ajouter un élément existant pour ajouter des fichiers de code existants au projet.
//   6. Pour rouvrir ce projet plus tard, accédez à Fichier > Ouvrir > Projet et sélectionnez le fichier .sln.
