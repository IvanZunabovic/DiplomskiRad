#include <ObjResourceFactory.h>
#include <Object3DModel.h>

#include <sstream>
#include <iomanip>

#include "json.hpp"

ObjResourceFactory::ObjResourceFactory()
{
    _resource = make_shared<Resource>();
    _resource->set_path("/{getObject: getObject}");
    _resource->set_method_handler("GET", [&](const shared_ptr<Session> session) {
        get_handler(session);
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

    session->close(OK, jsonResult.dump(), {{"Content-Lengt", to_string(jsonResult.size())}});
}

tuple<string> ObjResourceFactory::get_path_parameters(const shared_ptr<Session> session)
{
    const auto &request = session->get_request();
    auto object = request->get_path_parameter("getObject");

    return make_tuple(object);
}

Object3DModel ObjResourceFactory::load_object(string object)
{
    Object3DModel new3DObject = Object3DModel("C:\\Users\\ivanz\\Desktop\\oloid\\oloid64_tri.obj");
    return new3DObject;
}