#include "ObjParser.hpp"

ObjParser::ObjParser() {
}

ModelPtr ObjParser::parseObj(const std::string& fileName) {
	std::string path = filePath(fileName);	
	ModelPtr model(new Model());
	
	std::ifstream file;
	file.open(fileName, std::ifstream::in);	
	if (!file.is_open())
		throw std::runtime_error("Unable to open OBJ file " + fileName);
	
	std::string line;
	
	while(std::getline(file, line)) {
		std::stringstream sstream(line);		
		std::string prefix;
		sstream >> prefix;		
		if (prefix.size() > 0) {
			if ("v" == prefix) {
				model->addVertex(parseVertex(sstream));
			} else if ("vn" == prefix) {
				model->addNormal(parseNormal(sstream));
			} else if ("vt" == prefix) {
				model->addTexCoord(parseTexCoord(sstream));
			} else if ("f" == prefix) {
				model->addFace(parseFace(sstream));
			} else {
				continue;
			}
		}
	}
	
	file.close();
	
	return model;
}

Point3f ObjParser::parseVertex(std::stringstream& sstream) {
	float x, y, z;
	
	sstream >> x >> y >> z;
	
	Point3f p;
	p(0) = x;
	p(1) = y;
	p(2) = z;
	return p;
}

Point3f ObjParser::parseNormal(std::stringstream& sstream) {
	float x, y, z;
	
	sstream >> x >> y >> z;
	
	Point3f p;
	p(0) = x;
	p(1) = y;
	p(2) = z;
	return p;
}

Point2f ObjParser::parseTexCoord(std::stringstream& sstream) {
	float x, y;
	
	sstream >> x >> y;
	
	Point2f p;
	p(0) = x;
	p(1) = 1.0f - y;
	return p;
}

FacePtr ObjParser::parseFace(std::stringstream& sstream) {
	FacePtr face(new Face());
	
	std::string indexGroup;
	
	while (sstream >> indexGroup) {
		std::vector<std::string> indices = split(indexGroup, '/');
		
		if (1 == indices.size()) {
			face->addVertexIndex(stou(indices[0]));
		} else if (2 == indices.size()) {
			face->addVertexIndex(stou(indices[0]));
			face->addTexCoordIndex(stou(indices[1]));
		} else if (3 == indices.size()) {
			face->addVertexIndex(stou(indices[0]));
			face->addNormalIndex(stou(indices[2]));
			
			if ("" != indices[1])
				face->addTexCoordIndex(stou(indices[1]));
		} else {
			throw std::runtime_error("Invalid face definition in OBJ file");
		}
	}
	
	return face;
}

std::string ObjParser::parseName(std::stringstream& sstream) {
	std::string name, word;
	
	while (sstream >> word) {
		if (0 == name.size())
			name += word;
		else
			name += " " + word;
	}
	
	return name;
}

std::string ObjParser::filePath(const std::string& fileName) {
	size_t pos = fileName.find_last_of("/");
	
	if (std::string::npos == pos) {
		return "";
	} else
		return fileName.substr(0, pos) + "/";
}

std::vector<std::string> ObjParser::split(const std::string& str, char delim) {
	std::vector<std::string> items;
	std::stringstream sstream(str);
	std::string item;
	
	while (std::getline(sstream, item, delim))
		items.push_back(item);
		
	return items;
}

unsigned int ObjParser::stou(const std::string &str, size_t* idx, int base) {
	unsigned long result = std::stoul(str, idx, base);
	
	if (result > std::numeric_limits<unsigned>::max()) {
		throw std::out_of_range("stou");
	}
	
	return result;
}
