#include <cstdio>
#include <string>
#include <vector>
#include <array>

#include <iostream>

#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/prettywriter.h> // for stringify JSON
#include <rapidjson/document.h>     // rapidjson's DOM-style API
#include "rapidjson/filewritestream.h"
#include "rapidjson/filereadstream.h"
#include <rapidjson/writer.h>
#include <rttr/type>
#include "serialization.h"

using namespace rapidjson;
using namespace rttr;

bool write_variant(const variant& var, PrettyWriter<StringBuffer>& writer);
void ToJsonRecur(const instance& obj, PrettyWriter<StringBuffer>& writer);

bool write_basic_type(const type& t, const variant& var, PrettyWriter<StringBuffer>& writer) {
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


    static void write_array(const variant_sequential_view & view, PrettyWriter<StringBuffer>&writer) {
        writer.StartArray();
        for (const auto& item : view)
        {
            if (item.is_sequential_container())
            {
                write_array(item.create_sequential_view(), writer);
            }
            else
            {
                variant wrapped_var = item.extract_wrapped_value();
                type value_type = wrapped_var.get_type();
                if (value_type.is_arithmetic() || value_type == type::get<std::string>() || value_type.is_enumeration())
                {
                    write_basic_type(value_type, wrapped_var, writer);
                }
                else // object
                {
                    ToJsonRecur(wrapped_var, writer);
                }
            }
        }
        writer.EndArray();
    }

    static void write_associative_container(const variant_associative_view& view, PrettyWriter<StringBuffer>& writer) {
        static const string_view key_name("key");
        static const string_view value_name("value");

        writer.StartArray();
        

        if (view.is_key_only_type()) {
            for (auto& item : view) {
                write_variant(item.first, writer);
            }
        }
        else
        {
            for (auto& item : view)
            {
                writer.String(key_name.data(), static_cast<rapidjson::SizeType>(key_name.length()), false);
                write_variant(item.first, writer);
                writer.String(value_name.data(), static_cast<rapidjson::SizeType>(value_name.length()), false);
                write_variant(item.second, writer);
                
            }
        }

        writer.EndArray();
    }




    bool write_variant(const variant& var, PrettyWriter<StringBuffer>& writer) {

        variant theVar = var;
        type varType = theVar.get_type();
        if (varType.is_wrapper()) {
            varType = varType.get_wrapped_type();
            theVar = theVar.extract_wrapped_value();
        }
        if (write_basic_type(varType, theVar, writer)) {

            //cool
        }
        else if (var.is_sequential_container()) {
            write_array(var.create_sequential_view(), writer);
        }
        else if (var.is_associative_container()) {
            write_associative_container(var.create_associative_view(), writer);
        }
        else {
            auto child_prop = varType.get_properties();
            if (!child_prop.empty()) {

                //theres some properties so its an object to be seralized
                //json recursive
                ToJsonRecur(var, writer);
            }
            else {
                assert("Unknown RTTR serilization edge case that we haven't discovered");
                return false;
            }
        }
        return true;

    }

    void ToJsonRecur(const instance& obj, PrettyWriter<StringBuffer>& writer)
    {
        
        writer.StartObject();
        //maybe need a start object?
        /*
        instance localObj;
        if (obj.get_type().get_raw_type().is_wrapper())
            localObj = obj.get_wrapped_instance();
        else
            localObj = obj;
            */
        instance obj2 = obj.get_type().get_raw_type().is_wrapper() ? obj.get_wrapped_instance() : obj;
        auto prop_list = obj2.get_derived_type().get_properties();
        for (auto prop : prop_list)
        {
            // Retrieving value from property
            if (prop.get_metadata("NO_SERIALIZE"))
                continue;

            variant prop_value = prop.get_value(obj2);
            if (!prop_value)
                continue; // cannot serialize, because we cannot retrieve the value
            // Retrieve name of property
            const auto name = prop.get_name();

            // Serialize value of property using name
            writer.String(name.data(), static_cast<rapidjson::SizeType>(name.length()), false); 
            if (!write_variant(prop_value, writer))
            {
                std::cerr << "Failed to serialize" << name << std::endl;
            }
        }
        writer.EndObject();
    }

    std::string to_json(rttr::instance obj) {
        if (!obj.is_valid()) {
            return std::string();
        }
        StringBuffer sb;
        PrettyWriter<StringBuffer> writer(sb);
        //-----------------------------------------------
        
        ToJsonRecur(obj, writer);
      //  std::string temp = sb.GetString();
       

        Document d;
       // d.Parse(sb.GetString());
        d.Parse(sb.GetString());
        FILE* fp = fopen("output3.json", "wb");
       //FileWriteStream theStream(fp, sb.GetString(), sb.GetSize());
       // FileWriteStream theStream(fp, writeBuffer, sizeof(writeBuffer));
        //PrettyWriter<FileWriteStream> theWriter(theStream);

        
       // FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
        char writeBuffer[1000];
        FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
       // PrettyWriter<FileWriteStream> writer(os);
        PrettyWriter<FileWriteStream> fileWriter(os);
        d.Accept(fileWriter);

        fclose(fp);
        return sb.GetString();
    }
    


    
    //----------------------------------------------------------------------------------------- READING STARTS HERE 
    void fromJsonRecur(instance& obj, Value& jsonObj);
   



    variant extract_basic_type(Value& jvalue) {

        switch (jvalue.GetType()) {

        case kStringType: {
            return std::string(jvalue.GetString());
            break;
        }
        case kNullType: {
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
    
    static void write_array_recursively(variant_sequential_view& view, Value& json_array_value) {

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
                fromJsonRecur(instancedWrapped, json_index_value);
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

    variant extract_value(Value::MemberIterator& itr, const type& t)
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
                fromJsonRecur(instancedVal, json_value);
            }
        }
        return extracted_value;
    }

    static void write_associative_view_recursively(variant_associative_view& view, Value& json_array_value) {
        int counter = 0;
        for (SizeType i = 0; i < json_array_value.Size(); ++i) {
         
            auto& json_index_value = json_array_value[i];
            if (json_index_value.IsObject()) {
                Value::MemberIterator key_itr = json_array_value.FindMember("key");
                Value::MemberIterator value_itr = json_array_value.FindMember("value");
                //std::cout << value_itr[1].name.GetString() << std::endl;

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


    void fromJsonRecur(instance& obj, Value& jsonObj) {

        instance obj2 = obj.get_type().get_raw_type().is_wrapper() ? obj.get_wrapped_instance() : obj;
        const auto prop_list = obj2.get_derived_type().get_properties();

        for (auto prop : prop_list) {
            Value::MemberIterator theItr = jsonObj.FindMember(prop.get_name().data());
            if (theItr == jsonObj.MemberEnd()) {
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
                    fromJsonRecur(instancedVar, jsonVal);
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
    }



    //bool from_json(const std::string& json, rttr::instance obj) {
    //    Document theDoc;
    //    
    //    if (theDoc.Parse(json.c_str()).HasParseError()) {
    //        return 0;
    //    }

    //    fromJsonRecur(obj, theDoc);
    //    return true;
    //}

    bool from_json(FILE* fp, rttr::instance obj) {

        Document theDoc;
        //LOL i just realised we need to open the files with this function, and maybe have to read and write into multiple objects
        char readBuffer[10000];
        FileReadStream is(fp, readBuffer, sizeof(readBuffer));

        Document d;
        d.ParseStream(is);
        fromJsonRecur(obj, d);
        return true;
    }