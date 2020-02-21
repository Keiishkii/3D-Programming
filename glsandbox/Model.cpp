#include "Model.h"
#include "Triangle.h"

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

Model::Model(std::string _path, glm::vec4 _position) : position(_position)
{
	//Loads in the model
	loadModel(_path);
}

Model::Model(std::string _path)
{
	//Loads in the model
	loadModel(_path);
}

void Model::loadModel(std::string _path)
{
	//opens f-stream
	std::ifstream stream;
	stream.open(_path);

	//Contents used to make a triangle
	std::vector<glm::vec4>  vectorList;
	std::vector<glm::ivec4> faceList;
	std::vector<glm::vec4> normalList;
	std::vector<glm::vec4> textureCords;

	if (stream.is_open())
	{
		std::string currentLine;

		while (std::getline(stream, currentLine))
		{
			std::stringstream lineStream(currentLine);

			//Adds a vertex definition to the list.
			if (!currentLine.substr(0, 2).compare(0, 2, "v "))
			{
				float x = 0, y = 0, z = 0;

				std::string junk;

				lineStream >> junk >> x >> y >> z;

				vectorList.push_back(glm::vec4(x, y, z, 1));
			}

			//Adds a vertex normal to the list.
			else if (!currentLine.substr(0, 2).compare(0, 2, "vn"))
			{
				float x = 0, y = 0, z = 0;

				std::string junk;

				lineStream >> junk >> x >> y >> z;

				normalList.push_back(glm::vec4((x), (y), (z), 0));
			}
			// Adds a face definition to the list
			else if (!currentLine.substr(0, 2).compare(0, 2, "f "))
			{
				int vertex_1 = 0, vertex_2 = 0, vertex_3 = 0, normal = 0;
				std::string junk, sVertex_1, sVertex_2, sVertex_3, sNormal;

				lineStream >> junk >> sVertex_1 >> sVertex_2 >> sVertex_3;

				std::replace(sVertex_1.begin(), sVertex_1.end(), '/', ' ');
				std::replace(sVertex_2.begin(), sVertex_2.end(), '/', ' ');
				std::replace(sVertex_3.begin(), sVertex_3.end(), '/', ' ');

				std::stringstream converterStream(sVertex_1);

				converterStream >> vertex_1 >> junk >> normal;
				
				converterStream.clear();
				converterStream.str(sVertex_2);
				converterStream >> vertex_2 >> junk >> normal;

				converterStream.clear();
				converterStream.str(sVertex_3);
				converterStream >> vertex_3 >> junk >> normal;
				
				faceList.push_back(glm::ivec4(vertex_1, vertex_2, vertex_3, normal));
			}
		}
	}

	//closes f-stream
	stream.close();

	//creates triangle list from model file infomation
	for (int i = 0; i < faceList.size(); i++)
	{
		int vertex_1Pos = faceList.at(i).x - 1;
		int vertex_2Pos = faceList.at(i).y - 1;
		int vertex_3Pos = faceList.at(i).z - 1;
		int normalPos = faceList.at(i).w - 1;

		triangleList.push_back(Triangle(normalList.at(normalPos), vectorList.at(vertex_1Pos), vectorList.at(vertex_2Pos), vectorList.at(vertex_3Pos)));
	}
}

void Model::setPostion(glm::mat4x4 _newPos)
{
	//multiplies all coords within triangle by the new oreintation adn position matrix.
	for (int i = 0; i < triangleList.size(); i++)
	{
		//triangleList.at(i).setVertex1(_newPos * triangleList.at(i).getVertex1());
		//triangleList.at(i).setVertex2(_newPos * triangleList.at(i).getVertex2());
		//triangleList.at(i).setVertex3(_newPos * triangleList.at(i).getVertex3());
	}
}