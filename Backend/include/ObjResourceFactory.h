#pragma once

#include <IResourceFactory.h>
#include <tuple>
#include <string>
#include "Object3DModel.h"

class ObjResourceFactory : public IResourceFactory
{
public:
    ObjResourceFactory();
    shared_ptr<Resource> get_resource() const final;

private:
    shared_ptr<Resource> _resource;
    tuple<string> get_path_parameters(const shared_ptr<Session> session);
    void get_handler(const shared_ptr<Session> session);
    void get_selected_faces_handler(const shared_ptr<Session> session);
    Object3DModel load_object(string object);
};