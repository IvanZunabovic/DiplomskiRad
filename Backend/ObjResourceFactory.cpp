#include <ObjResourceFactory.h>
#include <Object3DModel.h>

#include <sstream>
#include <iomanip>

#include "json.hpp"

ObjResourceFactory::ObjResourceFactory()
{
    _resource = make_shared<Resource>();
    _resource->set_paths({"/{getObject: getObject}", "/getSelectedFaces"});
    _resource->set_method_handler("GET", [&](const shared_ptr<Session> session) {
        get_handler(session);
    });
    _resource->set_method_handler("POST", [&](const shared_ptr<Session> session) {
        get_selected_faces_handler(session);
    });
}

shared_ptr<Resource> ObjResourceFactory::get_resource() const
{
    return _resource;
}

void ObjResourceFactory::get_handler(const shared_ptr<Session> session)
{
    const auto [object] = get_path_parameters(session);
    Object3DModel result = load_object(object);

    nlohmann::json jsonResult = result.getJson();

    session->close(OK, jsonResult.dump(), {{"Content-Lengt", to_string(jsonResult.dump().size())}});
}

tuple<string> ObjResourceFactory::get_path_parameters(const shared_ptr<Session> session)
{
    const auto &request = session->get_request();
    auto object = request->get_path_parameter("getObject");

    return make_tuple(object);
}

Object3DModel ObjResourceFactory::load_object(string object)
{
    Object3DModel new3DObject = Object3DModel("C:\\Users\\ivanz\\Desktop\\sibenik2opti.obj");
    return new3DObject;
}

static const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

static inline bool is_base64(unsigned char c)
{
    return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_decode(std::string const &encoded_string)
{
    int in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;

    while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_]))
    {
        char_array_4[i++] = encoded_string[in_];
        in_++;
        if (i == 4)
        {
            for (i = 0; i < 4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++)
                ret += char_array_3[i];
            i = 0;
        }
    }

    if (i)
    {
        for (j = i; j < 4; j++)
            char_array_4[j] = 0;

        for (j = 0; j < 4; j++)
            char_array_4[j] = base64_chars.find(char_array_4[j]);

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++)
            ret += char_array_3[j];
    }

    return ret;
}

Vertex getInterseptionPoint(Plane plane, Line line)
{
    double tFactor = -(plane.a * line.x0 + plane.b * line.y0 + plane.c * line.z0 + plane.d) / (plane.a * line.xt + plane.b * line.yt + plane.c * line.zt);
    Vertex intersectionPoint = Vertex((line.xt * tFactor + line.x0), (line.yt * tFactor + line.y0), (line.zt * tFactor + line.z0));

    bool isPointOnPlane = plane.checkIfPointOnPlane(intersectionPoint);

    if (isPointOnPlane)
        return intersectionPoint;

    else
        return Vertex(100000, 100000, 100000);
}

double calculateAreaOfTriangle(Vertex point1, Vertex point2, Vertex point3)
{
    double a = NULL, b = NULL, c = NULL, perimeter = NULL, s = NULL;

    a = sqrt(pow(point2.x - point3.x, 2) + pow(point2.y - point3.y, 2) + pow(point2.z - point3.z, 2));
    b = sqrt(pow(point1.x - point3.x, 2) + pow(point1.y - point3.y, 2) + pow(point1.z - point3.z, 2));
    c = sqrt(pow(point1.x - point2.x, 2) + pow(point1.y - point2.y, 2) + pow(point1.z - point2.z, 2));

    perimeter = a + b + c;
    s = perimeter / 2;

    return sqrt(s * (s - a) * (s - b) * (s - c));
}

bool isPointInsideTriangle(Vertex point1, Vertex point2, Vertex point3, Vertex point)
{
    double p = NULL, p1 = NULL, p2 = NULL, p3 = NULL;

    p = calculateAreaOfTriangle(point1, point2, point3);
    p1 = calculateAreaOfTriangle(point, point1, point2);
    p2 = calculateAreaOfTriangle(point, point2, point3);
    p3 = calculateAreaOfTriangle(point, point1, point3);

    return abs(p - (p1 + p2 + p3)) < 0.0000001;
}

vector<int> getSelectedFaces(Object3DModel object, Line line)
{
    vector<Vertex> allVertices;
    vector<Face> allFaces;
    vector<int> selectedFaces = vector<int>();
    int i = 0;

    allVertices = object.get_vertices();
    allFaces = object.get_faces();

    for (i = 0; i < allFaces.size() - 1; i++)
    {
        Face currentFace = allFaces[i];

        Vertex v1 = allVertices[currentFace.v1];
        Vertex v2 = allVertices[currentFace.v2];
        Vertex v3 = allVertices[currentFace.v3];

        Plane currentFacePlane = Plane(v1, v2, v3);

        Vertex intersectionPoint = getInterseptionPoint(currentFacePlane, line);

        if (intersectionPoint.x != 100000 && intersectionPoint.y != 100000 && intersectionPoint.z != 100000 && isPointInsideTriangle(v1, v2, v3, intersectionPoint))
        {
            selectedFaces.push_back(i);
        }
    }

    return selectedFaces;
}

void ObjResourceFactory::get_selected_faces_handler(const shared_ptr<Session> session)
{
    const auto request = session->get_request();

    // size_t content_length = 0;
    // request->get_header("Content-Length", content_length);

    int content_length = request->get_header("Content-Length", 0);

    session->fetch(content_length, [request](const shared_ptr<Session> session, const Bytes &body) {
        string data = string((char *)body.data(), body.size());
        string size = to_string(data.size());
        string newBody = String::to_string(body);
        std::vector<int> vect;

        nlohmann::json json;
        nlohmann::json j = json.parse(base64_decode(newBody));

        string firstVertexString = j["point1"];
        string secondVertexString = j["point2"];

        Object3DModel new3DObject = Object3DModel("C:\\Users\\ivanz\\Desktop\\sibenik2opti.obj");

        stringstream fs(firstVertexString);
        string str = "";
        vector<string> result;

        while (getline(fs, str, ','))
        {
            result.push_back(str);
        }

        Vertex firstPoint = Vertex(stod(result[0]), stod(result[1]), stod(result[2]));

        stringstream ss(secondVertexString);
        str = "";
        result.clear();

        while (getline(ss, str, ','))
        {
            result.push_back(str);
        }

        Vertex secondPoint = Vertex(stod(result[0]), stod(result[1]), stod(result[2]));

        Line newLine = Line(firstPoint, secondPoint);
        vector<int> indexes = getSelectedFaces(new3DObject, newLine);

        nlohmann::json response = {{"indexes", indexes}};
        session->close(OK, response.dump(), {{"Content-Length", to_string(response.dump().size())}, {"Connection", "close"}});
    });
}