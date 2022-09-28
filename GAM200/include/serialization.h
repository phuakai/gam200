#include <string>
#include <rttr/type>
#include "mainHeader.h"


	/*!
	 * Serialize the given instance to a json encoded string.
	 */
	std::string to_json(rttr::instance obj, std::string fileName);


	bool from_json(FILE* fp);
	//bool from_json(const std::string& json, rttr::instance obj);

	void ecsWriteToFile();