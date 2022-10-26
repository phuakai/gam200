/*!
@file    serialization.h
@author  p.jiankai@digipen.edu, ruoyan.go@digipen.edu, tundagh.pang@digipen.edu
@date    10/9/2022

This file includes the serialisation function declarations.

*//*__________________________________________________________________________*/

#include <string>
#include <rttr/type>
#include "mainHeader.h"
#include <rapidjson/prettywriter.h> // for stringify JSON
#include <rapidjson/document.h>     // rapidjson's DOM-style API
#include "rapidjson/filewritestream.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/istreamwrapper.h"
#include <rapidjson/writer.h>
#include <rapidjson/reader.h>
#include <fstream>

class Serialiser
{
public:
	Serialiser() : writer(sb) {}
	~Serialiser() {}

	/*serialiser functions*/

	bool Write(std::string fileName);												//called outside of serialization.cpp to actually Do Serialisation/Saving
	bool WriteComponent(rttr::instance obj);					//Starts the serialisation inside Write, called per entity
	bool WriteRecur(const rttr::instance& obj);										//Fills in all the component data

	/*serialiser helper functions*/

	bool write_basic_type(const rttr::type& t, const rttr::variant& var);			//all the basic types for rttr
	void write_array(const rttr::variant_sequential_view& view);					//writes arrays and stuff
	void write_associative_container(const rttr::variant_associative_view& view);	//writes the associative view containers
	bool write_variant(const rttr::variant& var);									//writes other variants that might be needed/filled in

	/*deserialiser functions*/

	bool Read(std::string fileName);												//called outside of serialization.cpp to actually Do Deserialisation/Loading
	//bool ReadComponent(rapidjson::Document& d);										//Starts the deserialisation inside Read, (ideally) called per entity
	bool ReadRecur(rttr::instance& obj, rapidjson::Value& jsonObj);					//reads out all the component data

	/*deserialiser helper functions*/
	rttr::variant extract_basic_type(rapidjson::Value& jvalue);
	void write_array_recursively(rttr::variant_sequential_view& view,
		rapidjson::Value& json_array_value);
	rttr::variant extract_value(rapidjson::Value::MemberIterator& itr,
		const rttr::type& t);
	void write_associative_view_recursively(rttr::variant_associative_view& view,
		rapidjson::Value& json_array_value);

private:
	rapidjson::StringBuffer sb;														//The string buffer used by both serialisation and deserialisation
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer;						//used for serialisation, the pretty writer for json
};