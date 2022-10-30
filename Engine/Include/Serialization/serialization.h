/*!
@file    serialization.h
@author  p.jiankai@digipen.edu, ruoyan.go@digipen.edu
@date    10/9/2022

This file includes the serialisation function declarations.

*//*__________________________________________________________________________*/

#include <string>
#include <rttr/type>
#include "../mainHeader.h"


/*!
* Serialize the given instance to a json encoded string.
*/
std::string to_json(rttr::instance obj, std::string fileName, bool overwriteFile = false);
std::string toJsonECS(std::vector<EntityID> entityList, std::string fileName, bool overwiteFile = false);


bool from_json(std::string fileName);
bool fromJsonECS(std::string fileName);
//bool from_json(const std::string& json, rttr::instance obj);

void ecsWriteToFile();