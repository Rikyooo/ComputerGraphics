#ifndef _MODEL_HPP
#define _MODEL_HPP

#include "SmartPtr.hpp"
#include "Vector.hpp"

#include <GL/glut.h>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>

class Face {
	friend class Model;
	
	public:
		Face();
		
		void addVertexIndex(unsigned int index);
		void addNormalIndex(unsigned int index);
		void addTexCoordIndex(unsigned int index);
		
	private:
		std::vector<unsigned int> _vertexIndices;
		std::vector<unsigned int> _normalIndices;
		std::vector<unsigned int> _texCoordIndices;
};
typedef SmartPtr<Face> FacePtr;

class Model {
	public:
		Model();
		
		//adds vertices, normals, and texture coordinates
		void addVertex(const Point3f& vertex);
		void addNormal(const Point3f& normal);
		void addTexCoord(const Point2f& coord);
		
		//computes the model's centroid
		void centroid();
		
		//computes the maximum distance of all points from the centroid
		//float radius();
		//float radius(Vector3f * boundingBox);
		float radius(Vector3f * maxBounding, Vector3f * minBounding);
		
		//centers the model on it's centroid
		void center();

		//adds a face
		void addFace(const FacePtr & face);

		void printFace();
		
		//renders the model
		void render();
		
	private:
		Vector3f _centroid;
		std::vector<Point3f> _vertices;
		std::vector<Point3f> _normals;
		std::vector<Point2f> _texCoords;
		std::vector<FacePtr> _faces;
};
typedef SmartPtr<Model> ModelPtr;

#endif
