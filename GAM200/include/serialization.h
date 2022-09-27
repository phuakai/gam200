#include <string>
#include <rttr/type>



	/*!
	 * Serialize the given instance to a json encoded string.
	 */
	std::string to_json(rttr::instance obj);


	bool from_json(FILE* fp, rttr::instance obj);
	//bool from_json(const std::string& json, rttr::instance obj);