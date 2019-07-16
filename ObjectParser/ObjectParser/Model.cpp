#include "Model.hpp"

/*
 * Face
 */
Face::Face() {
}
 
void Face::addVertexIndex(unsigned int index) {
	_vertexIndices.push_back(index);
}

void Face::addNormalIndex(unsigned int index) {
	_normalIndices.push_back(index);
}

void Face::addTexCoordIndex(unsigned int index) {
	_texCoordIndices.push_back(index);
}

Model::Model()
{
}

/*
 * Model
 */
void Model::addVertex(const Point3f& vertex) {
	_vertices.push_back(vertex);
}

void Model::addNormal(const Point3f& normal) {
	_normals.push_back(normal);
}

void Model::addTexCoord(const Point2f& coord) {
	_texCoords.push_back(coord);
}

void Model::addFace(const FacePtr& face) {
	_faces.push_back(face);
}

void Model::printFace() {
	for (int i = 0; i < _faces.size(); i++)
	{
		for (int j = 0; j < _faces[i]->_vertexIndices.size(); j++)
		{
			std::cout << _faces[i]->_vertexIndices[j] << " ";
		}

		std::cout << std::endl;
	}
}

void Model::centroid() {
	if (_vertices.size() == 0)
		throw std::runtime_error("Unable to computer centroid: no verticies found");
	
	_centroid = Vector3f::zero();
	
	for (unsigned int i = 0; i < _vertices.size(); i++) {
		_centroid +=  _vertices[i];
	}
	
	_centroid /= (float)_vertices.size();
}

float Model::radius(Vector3f* maxBounding, Vector3f* minBounding) {
	if (_vertices.size() == 0)
		throw std::runtime_error("Unable to compute max distance: no verticies found");
		
	float distance, max = 0;
	float x, y, z = 0;
	
	for (unsigned int i = 0; i < _vertices.size(); i++) {
		distance = _centroid.distance(_vertices[i]);	
		if (distance > max)
			max = distance;

		x = _vertices[i](0);
		y = _vertices[i](1);
		z = _vertices[i](2);
		if (x > (*maxBounding)(0))
			(*maxBounding)(0) = x;
		else if (x < (*minBounding)(0))
			(*minBounding)(0) = x;

		if (y > (*maxBounding)(1))
			(*maxBounding)(1) = y;
		else if (y < (*minBounding)(1))
			(*minBounding)(1) = y;

		if (z > (*maxBounding)(2))
			(*maxBounding)(2) = z;
		else if (z < (*minBounding)(2))
			(*minBounding)(2) = z;
	}
	
	return max;
}

void Model::center() {
	if (_vertices.size() == 0)
		throw std::runtime_error("Unable to center model: no verticies found");
	
	for (unsigned int i = 0; i < _vertices.size(); i++) {
		_vertices[i] -= _centroid;
	}
}

void Model::render() {
	for (unsigned int i = 0; i < _faces.size(); i++) {
		FacePtr face = _faces[i];
		
		glBegin(GL_POLYGON);
		for (unsigned int j = 0; j < face->_vertexIndices.size(); j++) {
			unsigned int index;

			if (face->_texCoordIndices.size() > 0) {
				index = face->_texCoordIndices[j];
				glTexCoord2fv(_texCoords[index - 1].data());
			}

			if (face->_normalIndices.size() > 0) {
				index = face->_normalIndices[j];
				glNormal3fv(_normals[index - 1].data());
			}

			index = face->_vertexIndices[j];
			glVertex3fv(_vertices[index - 1].data());
		}

		glEnd();
	}
}

