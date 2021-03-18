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

        for (i = 0; i < _vertices.size() - 1; i++)
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
            char simbol;
            float firstValue = 0, secondValue = 0, thirdValue = 0;

            fin >> simbol >> firstValue >> secondValue >> thirdValue;

            switch (simbol)
            {
            case 'v':
            {
                _vertices.push_back(Vertex(firstValue, secondValue, thirdValue));
            }
            break;
            case 'f':
            {
                _faces.push_back(Face(firstValue, secondValue, thirdValue));
            }
            break;
            default:
                break;
            }
        }

        fin.close();
    }
};