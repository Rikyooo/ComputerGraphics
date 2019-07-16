#ifndef _OBJ_PARSER_HPP
#define _OBJ_PARSER_HPP

#include "Model.hpp"
#include "SmartPtr.hpp"

#include <string>
#include <vector>
#include <fstream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <iostream>

class ObjParser {
	public:
		ObjParser();
				
		ModelPtr parseObj(const std::string& fileName);
		
	private:
		/*
		 * OBJ parsing methods
		 */
		Point3f parseVertex(std::stringstream& sstream);
		Point3f parseNormal(std::stringstream& sstream);
		Point2f parseTexCoord(std::stringstream& sstream);
		FacePtr parseFace(std::stringstream& sstream);
		std::string parseName(std::stringstream& sstream);

		/*
		 * Utility methods
		 */
		std::string filePath(const std::string& fileName);
		
		/*
		 * String utilities
		 */
		std::vector<std::string> split(const std::string& str, char delim);
		unsigned int stou(const std::string &str, size_t* idx = 0, int base = 10);
};

#endif
