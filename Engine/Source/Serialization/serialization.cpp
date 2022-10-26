/*!
@file    serialization.cpp
@author  p.jiankai@digipen.edu, ruoyan.go@digipen.edu, tundagh.pang@digipen.edu
@date    10/9/2022

This file includes the serialisation function definitions.

*//*__________________________________________________________________________*/

/*Stuff that needs doing:
* - Some way to iterate through components when serialising each component
* - Serialise each component as part of an entity
* - Serialisation in order
* - Deserialising
*/

#include <cstdio>
#include <string>
#include <vector>
#include <array>

#include <iostream>

#define RAPIDJSON_HAS_STDSTRING 1
#include "serialization.h"
#include "ECS.h"

using namespace rapidjson;
using namespace rttr;

//this is the function you call in order to serialise/save
bool Serialiser::Write(std::string fileName)
{
    std::vector<EntityID> entity_map = ecs.getEntities();
    std::ofstream file(fileName);

    writer.StartObject();
    writer.Key("Save");
    writer.StartArray();

    for (size_t i{ 0 }; i < entity_map.size(); ++i)
    {
        //writer.StartArray();
        //std::string entity_name = ecs.getEntityName(i);
        //writer.Key(entity_name);
        WriteComponent(*ecs.GetComponent<Render>(entity_map[i]));
        //Texture
        //writeComponent(*ecs.GetComponent<Texture>(entity_map[i]));
        //Movement
        //writeComponent(*ecs.GetComponent<Movement>(entity_map[i]));
        //Stats
        //writeComponent(*ecs.GetComponent<Stats>(entity_map[i]));
        //writer.EndArray();
    }
    writer.EndArray();
    writer.EndObject();

    try
    {
        //std::cout << sb.GetString();

        file << sb.GetString();
        file.close();
    }
    catch (const std::exception& e) {
        assert(e.what());
        return false;
    }

    return true;
}

bool Serialiser::WriteComponent(rttr::instance obj)
{
    if (!obj.is_valid())
    {
        return false;
    }

    int counter{ 1 };

    //std::string component_name = obj.get_type(); //extract the name of the component somehow
    writer.StartObject();
    //writer.Key(component_name);

    WriteRecur(obj);

    writer.EndObject();
}

bool Serialiser::WriteRecur(const rttr::instance& obj)
{
    writer.StartObject();
    //writer.Key("name");
    //writer.String(obj.get_type().get_name().to_string().c_str(), 
        //static_cast<rapidjson::SizeType>(obj.get_type().get_name().length(), false));

    instance obj2 = obj.get_type().get_raw_type().is_wrapper() ? obj.get_wrapped_instance() : obj;
    auto prop_list = obj2.get_derived_type().get_properties();
    for (auto& prop : prop_list)
    {
        //retrieving value from property
        if (prop.get_metadata("NO_SERIALIZE"))
        {
            continue;
        }

        variant prop_value = prop.get_value(obj2);
        if (!prop_value)
        {
            continue; //cannot serialise because cannot retrieve value
        }
        const auto name = prop.get_name();

        //serialise the value of the property using the name
        writer.String(name.to_string().c_str(), static_cast<rapidjson::SizeType>(name.length()), false);
        if (!write_variant(prop_value))
        {
            std::cerr << "Failed to serialize " << name << std::endl;
        }
    }

    writer.EndObject();
    return true;
}

bool Serialiser::write_basic_type(const type& t, const variant& var)
{
    if (t.is_arithmetic())
    {
        if (t == type::get<bool>())
            writer.Bool(var.to_bool());
        else if (t == type::get<char>())
            writer.Bool(var.to_bool());
        else if (t == type::get<int8_t>())
            writer.Int(var.to_int8());
        else if (t == type::get<int16_t>())
            writer.Int(var.to_int16());
        else if (t == type::get<int32_t>())
            writer.Int(var.to_int32());
        else if (t == type::get<int64_t>())
            writer.Int64(var.to_int64());
        else if (t == type::get<uint8_t>())
            writer.Uint(var.to_uint8());
        else if (t == type::get<uint16_t>())
            writer.Uint(var.to_uint16());
        else if (t == type::get<uint32_t>())
            writer.Uint(var.to_uint32());
        else if (t == type::get<uint64_t>())
            writer.Uint64(var.to_uint64());
        else if (t == type::get<float>())
            writer.Double(var.to_double());
        else if (t == type::get<double>())
            writer.Double(var.to_double());
        return true;
    }
    else if (t.is_enumeration())
    {
        bool ok = false;
        auto result = var.to_string(&ok);
        if (ok)
        {
            writer.String(var.to_string());
        }
        else
        {
            ok = false;
            auto value = var.to_uint64(&ok);
            if (ok)
                writer.Uint64(value);
            else
                writer.Null();
        }

        return true;
    }
    else if (t == type::get<std::string>())
    {
        writer.String(var.to_string());
        return true;
    }
    return false;
}

void Serialiser::write_array(const variant_sequential_view& view)
{
    writer.StartArray();
    for (const auto& item : view)
    {
        if (item.is_sequential_container())
        {
            write_array(item.create_sequential_view());
        }
        else
        {
            variant wrapped_var = item.extract_wrapped_value();
            type value_type = wrapped_var.get_type();
            if (value_type.is_arithmetic() || value_type == type::get<std::string>() || value_type.is_enumeration())
            {
                write_basic_type(value_type, wrapped_var);
            }
            else
            {
                WriteRecur(wrapped_var);
            }
        }
    }
    writer.EndArray();
}

void Serialiser::write_associative_container(const variant_associative_view& view)
{
    static const string_view key_name("key");
    static const string_view value_name("value");

    writer.StartArray();

    if (view.is_key_only_type())
    {
        for (auto& item : view)
        {
            write_variant(item.first);
        }
    }
    else
    {
        for (auto& item : view)
        {
            writer.String(key_name.data(), static_cast<rapidjson::SizeType>(key_name.length()), false);
            write_variant(item.first);
            writer.String(value_name.data(), static_cast<rapidjson::SizeType>(value_name.length()), false);
            write_variant(item.second);
        }
    }
    writer.EndArray();
}

bool Serialiser::write_variant(const variant& var)
{
    variant theVar = var;
    type varType = theVar.get_type();
    if (varType.is_wrapper())
    {
        varType = varType.get_wrapped_type();
        theVar = theVar.extract_wrapped_value();
    }
    if (write_basic_type(varType, theVar))
    {
        //don't need to do anything because it parsed successfully lets gooooooooooo
    }
    else if (var.is_sequential_container())
    {
        write_array(var.create_sequential_view());
    }
    else if (var.is_associative_container())
    {
        write_associative_container(var.create_associative_view());
    }
    else
    {
        auto child_prop = varType.get_properties();
        if (!child_prop.empty())
        {
            //there's properties, which means it's an object
            //so serialise
            WriteRecur(var);
        }
        else
        {
            assert("Unknown RTTR serialization edge case that we haven't discovered");
            return false;
        }
    }
    return true;
}

/*----------------------------------------------------Deserialisation Functions-------------------------------------------------------*/

//this is the function you call in order to deserialise/load
bool Serialiser::Read(std::string fileName)
{
    std::ifstream file(fileName);
    assert(file.is_open());

    IStreamWrapper isw{ file };
    Document document;
    document.ParseStream(isw);

    for (size_t i{ 0 }; i < document["Save"].Size(); ++i)
    {
        //for (size_t j{ 0 }; j < document["Save"][i].Size(); ++j)
        //{   //rewrite this, there's a new layer of arrays to work through now
        std::string entity_name = document["Save"][i]["ComponentID"]["name"].GetString();
        //std::string entity_name = document["Save"][i][/*something here*/]["name"].GetString();

        //std::cout << "Entity Name: " << entity_name << std::endl;

        EntityID newEntity = ecs.GetNewID();
        ecs.RegisterEntity(newEntity, entity_name);
        ecs.AddComponent<Render>(newEntity);
        //ecs.AddComponent<Texture>(newEntity);
        //ecs.AddComponent<Movement>(newEntity);
        //ecs.AddComponent<Stats>(newEntity);


        rttr::instance instanceEntity = *ecs.GetComponent<Render>(newEntity);
        ReadRecur(instanceEntity, document["Save"][i]);

        /*std::vector<std::string> components = ecs.getAllRegisteredComponents();
        for (size_t j{ 0 }; j < ecs.getAllRegisteredComponents().size(); ++j)
        {
            if (components[j] == "Render")
            {
                rttr::instance instanceEntity = *ecs.GetComponent<Render>(newEntity);
                ReadRecur(instanceEntity, document["Save"][i]);
            }
            if (components[j] == "Texture")
            {
                rttr::instance instanceEntity = *ecs.GetComponent<Texture>(newEntity);
                ReadRecur(instanceEntity, document["Save"][i]);
            }
            if (components[j] == "Movement")
            {
                rttr::instance instanceEntity = *ecs.GetComponent<Movement>(newEntity);
                ReadRecur(instanceEntity, document["Save"][i]);
            }
            if (components[j] == "Stats")
            {
                rttr::instance instanceEntity = *ecs.GetComponent<Stats>(newEntity);
                ReadRecur(instanceEntity, document["Save"][i]);
            }

        }*/
    }

    //GLApp::GLObject::gimmeObject(ecs.GetComponent<Render>(newEntity)->type, 
    //  ecs.GetComponent<Render>(newEntity)->name, ecs.GetComponent<Render>(newEntity)->dimension, 
    //  ecs.GetComponent<Render>(newEntity)->position, vector3D::vec3D(randr, randg, randb));

    //syntax for the rgb values: document["Save"][i]["ComponentID"]["color"]["r"]/["g"]/["b"]

    //	quadObj entity;
    //	entity.key = newEntity;
    //	entity.position = ecs.GetComponent<Render>(newEntity)->position;

    //	mainTree.insertSuccessfully(entity);
//}

    return true;
}

//bool Serialiser::ReadComponent(rapidjson::Document& d)
//{
//    EntityID newEntity = ecs.GetNewID();
//    rapidjson::Value ValueObj = d.GetObject();
//    ecs.RegisterEntity(newEntity, );
//    ecs.AddComponent<Render>(newEntity);
//    return true;
//}

bool Serialiser::ReadRecur(rttr::instance& obj, rapidjson::Value& jsonObj)
{
    instance obj2 = obj.get_type().get_raw_type().is_wrapper() ? obj.get_wrapped_instance() : obj;
    const auto prop_list = obj2.get_derived_type().get_properties();

    for (auto prop : prop_list)
    {
        Value::MemberIterator theItr = jsonObj.FindMember(prop.get_name().data());
        if (theItr == jsonObj.MemberEnd())
        {
            continue;
        }

        const type valT = prop.get_type();
        auto& jsonVal = theItr->value;

        switch (jsonVal.GetType())
        {
        case kArrayType:
        {

            variant var = prop.get_value(obj2);
            if (valT.is_associative_container()) {
                auto theView = var.create_associative_view();
                write_associative_view_recursively(theView, jsonVal);
            }
            else if (valT.is_sequential_container()) {
                auto theView = var.create_sequential_view();
                write_array_recursively(theView, jsonVal);
            }

            prop.set_value(obj2, var);
            break;
        }
        case kObjectType:
        {
            variant var = prop.get_value(obj2);
            instance instancedVar = var;
            ReadRecur(instancedVar, jsonVal);
            prop.set_value(obj2, var);
            break;
        }
        default: {
            variant extracted_value = extract_basic_type(jsonVal);
            if (extracted_value.convert(valT)) // CONVERTING WORKS ONLY WITH const type
                prop.set_value(obj, extracted_value);
        }
        }
    }

    return true;
}

variant Serialiser::extract_basic_type(Value& jvalue)
{
    switch (jvalue.GetType())
    {
    case kStringType:
    {
        return std::string(jvalue.GetString());
        break;
    }
    case kNullType:
    {
        break;
    }
    case kFalseType:
    case kTrueType:
    {
        return jvalue.GetBool();
        break;
    }
    case kNumberType:
    {
        if (jvalue.IsInt())
            return jvalue.GetInt();
        else if (jvalue.IsDouble())
            return jvalue.GetDouble();
        else if (jvalue.IsUint())
            return jvalue.GetUint();
        else if (jvalue.IsInt64())
            return jvalue.GetInt64();
        else if (jvalue.IsUint64())
            return jvalue.GetUint64();
        break;
    }
    // not a basic type
    case kObjectType:
    case kArrayType: return variant();

    }
    return variant();
}

void Serialiser::write_array_recursively(variant_sequential_view& view, Value& json_array_value)
{
    view.set_size(json_array_value.Size());

    const type array_value_type = view.get_rank_type(1);


    for (SizeType i = 0; i < json_array_value.Size(); ++i)
    {
        auto& json_index_value = json_array_value[i];
        if (json_index_value.IsArray()) {

            auto sub_array_view = view.get_value(i).create_sequential_view();
            write_array_recursively(sub_array_view, json_index_value);
        }

        else if (json_index_value.IsObject())
        {
            variant var_tmp = view.get_value(i);
            variant wrapped_var = var_tmp.extract_wrapped_value();
            //---------------------------------------------------------some ghetto shit happening
            // for reference
            /*
            Instance
                Holds a reference to the given object.
                Think of it as a std::any<T&>
                Variant
                Return value for properties and methods
                Content is copied over to new instance of content
                Think of it as a std::any
                */
            instance instancedWrapped = wrapped_var;
            ReadRecur(instancedWrapped, json_index_value);
            view.set_value(i, wrapped_var);
        }

        else
        {
            variant extracted_value = extract_basic_type(json_index_value);
            if (extracted_value.convert(array_value_type))
                view.set_value(i, extracted_value);
        }
    }
}

variant Serialiser::extract_value(Value::MemberIterator& itr, const type& t)
{
    auto& json_value = itr->value;
    variant extracted_value = extract_basic_type(json_value);
    const bool can_convert = extracted_value.convert(t);
    if (!can_convert) {
        //not a basic type 
        if (json_value.IsObject()) {
            constructor ctor = t.get_constructor();
            for (auto& item : t.get_constructors()) {
                if (item.get_instantiated_type() == t) {
                    ctor = item;
                }
            }
            extracted_value = ctor.invoke();
            instance instancedVal = extracted_value;
            ReadRecur(instancedVal, json_value);
        }
    }
    return extracted_value;
}

void Serialiser::write_associative_view_recursively(variant_associative_view& view, Value& json_array_value)
{
    int counter = 0;
    for (SizeType i = 0; i < json_array_value.Size(); ++i) {

        auto& json_index_value = json_array_value[i];
        if (json_index_value.IsObject()) {
            Value::MemberIterator key_itr = json_array_value.FindMember("key");
            Value::MemberIterator value_itr = json_array_value.FindMember("value");
            std::cout << "string data " << value_itr[1].name.GetString() << std::endl;

            //just a check
            if (key_itr != json_array_value.MemberEnd() && value_itr != json_array_value.MemberEnd()) {
                key_itr[0] = key_itr[counter * 2];
                value_itr[0] = value_itr[counter * 2];
                auto key_var = extract_value(key_itr, view.get_key_type());
                auto value_var = extract_value(value_itr, view.get_value_type());
                //just another check
                if (key_var && value_var) {
                    std::cout << "test" << std::endl;
                    view.insert(key_var, value_var);
                    //view.insert(key_var, value_var);
                }
                ++counter;
            }
        }
        else {   //only key 


            variant extracted_value = extract_basic_type(json_index_value);
            if (extracted_value && extracted_value.convert(view.get_key_type()))
            {
                view.insert(extracted_value);
            }
        }
    }
}