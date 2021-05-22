#pragma once

#include <vector>
#include <fstream>
#include <iostream>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

class Vertex
{
public:
    double x;
    double y;
    double z;

    Vertex(double value1, double value2, double value3)
    {
        x = value1;
        y = value2;
        z = value3;
    }
};

class Face
{
public:
    unsigned int v1;
    unsigned int v2;
    unsigned int v3;

    Face(unsigned int value1, unsigned int value2, unsigned int value3)
    {
        v1 = value1;
        v2 = value2;
        v3 = value3;
    }
};

class Plane
{
public:
    double a;
    double b;
    double c;
    double d;

    Plane(Vertex point1, Vertex point2, Vertex point3)
    {
        equation_plane(point1, point2, point3);
    }

    bool checkIfPointOnPlane(Vertex point)
    {
        return a * point.x + b * point.y + c * point.z + d < 0.0000001;
    }

private:
    void equation_plane(Vertex point1, Vertex point2, Vertex point3)
    {
        float a1 = point2.x - point1.x;
        float b1 = point2.y - point1.y;
        float c1 = point2.z - point1.z;
        float a2 = point3.x - point1.x;
        float b2 = point3.y - point1.y;
        float c2 = point3.z - point1.z;
        a = b1 * c2 - b2 * c1;
        b = a2 * c1 - a1 * c2;
        c = a1 * b2 - b1 * a2;
        d = (-a * point1.x - b * point1.y - c * point1.z);
    }
};

class Line
{
public:
    double xt;
    double x0;
    double yt;
    double y0;
    double zt;
    double z0;

    Line(Vertex startingPoint, Vertex endingPoint)
    {
        equation_line(startingPoint, endingPoint);
    }

private:
    void equation_line(Vertex startingPoint, Vertex endingPoint)
    {
        xt = endingPoint.x - startingPoint.x;
        x0 = startingPoint.x;
        yt = endingPoint.y - startingPoint.y;
        y0 = startingPoint.y;
        zt = endingPoint.z - startingPoint.z;
        z0 = startingPoint.z;
    }
};

class Object3DModel
{
public:
    Object3DModel(const string filename)
    {
        vector<Vertex> vertexVector;
        vector<Face> faceVector;
        _vertices = vertexVector;
        _faces = faceVector;
        this->readfile(filename);
    }

    Object3DModel(vector<Vertex> vertices, vector<Face> faces)
    {
        _vertices = vertices;
        _faces = faces;
    };

    vector<Vertex> get_vertices() { return _vertices; };
    vector<Face> get_faces() { return _faces; };
    json getJson()
    {
        int i = 0;
        vector<vector<double>> verticesResponse;
        vector<vector<unsigned int>> facesResponse;

        for (i = 0; i < _vertices.size(); i++)
        {
            vector<double> vertexCoordinates;
            vertexCoordinates.push_back(_vertices[i].x);
            vertexCoordinates.push_back(_vertices[i].y);
            vertexCoordinates.push_back(_vertices[i].z);

            verticesResponse.push_back(vertexCoordinates);
        }

        i = 0;
        for (i = 0; i < _faces.size(); i++)
        {
            vector<unsigned int> faceVertices;
            faceVertices.push_back(_faces[i].v1);
            faceVertices.push_back(_faces[i].v2);
            faceVertices.push_back(_faces[i].v3);

            facesResponse.push_back(faceVertices);
        }

        json response = {{"vertices", verticesResponse}, {"faces", facesResponse}};

        return response;
    }

private:
    vector<Vertex> _vertices;
    vector<Face> _faces;
    void readfile(const string filename)
    {
        ifstream fin(filename);

        if (!fin)
            return;

        while (!fin.eof())
        {
            char sign = NULL;
            float firstValue = 0, secondValue = 0, thirdValue = 0;
            string line = "";

            getline(fin, line);

            istringstream ss(line);

            ss >> sign;

            if (sign == 'v' || sign == 'f')
            {
                ss >> firstValue >> secondValue >> thirdValue;

                switch (sign)
                {
                case 'v':
                {
                    _vertices.push_back(Vertex(firstValue, secondValue, thirdValue));
                }
                break;
                case 'f':
                {
                    _faces.push_back(Face(firstValue - 1, secondValue - 1, thirdValue - 1));
                }
                break;
                default:
                    break;
                }
            }
            //fin >> sign >> firstValue >> secondValue >> thirdValue;
        }

        fin.close();
    }
};