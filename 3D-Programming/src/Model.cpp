#include "Model.h"
#include "Triangle.h"

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

struct face
{
	glm::ivec3 positions;
	glm::ivec3 texturecoords;
	int normal;
};

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
	std::vector<glm::vec4> normalList;
	std::vector<glm::vec2> textureCords;

	std::vector<face> faceList;

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

			//Adds a vertex normal to the list.
			else if (!currentLine.substr(0, 2).compare(0, 2, "vt"))
			{
				float x = 0, y = 0;

				std::string junk;

				lineStream >> junk >> x >> y;

				textureCords.push_back(glm::vec2((x), (y)));
			}
			// Adds a face definition to the list
			else if (!currentLine.substr(0, 2).compare(0, 2, "f "))
			{
				int vertex_1 = 0, vertex_2 = 0, vertex_3 = 0, texturePos_1 = 0, texturePos_2 = 0, texturePos_3 = 0, normal = 0;
				std::string junk, group_1, group_2, group_3;

				lineStream >> junk >> group_1 >> group_2 >> group_3;

				std::replace(group_1.begin(), group_1.end(), '/', ' ');
				std::replace(group_2.begin(), group_2.end(), '/', ' ');
				std::replace(group_3.begin(), group_3.end(), '/', ' ');

				std::stringstream converterStream(group_1);

				converterStream >> vertex_1 >> texturePos_1 >> normal;
				
				converterStream.clear();
				converterStream.str(group_2);
				converterStream >> vertex_2 >> texturePos_2 >> normal;

				converterStream.clear();
				converterStream.str(group_3);
				converterStream >> vertex_3 >> texturePos_3 >> normal;
				
				face newFaceIDs;

				newFaceIDs.positions = glm::vec3(vertex_1, vertex_2, vertex_3);
				newFaceIDs.texturecoords = glm::vec3(texturePos_1, texturePos_2, texturePos_3);
				newFaceIDs.normal = normal;

				faceList.push_back(newFaceIDs);
			}
		}
	}

	//closes f-stream
	stream.close();

	//creates triangle list from model file infomation
	for (int i = 0; i < faceList.size(); i++)
	{
		int vertex_1Pos = faceList.at(i).positions.x - 1;
		int vertex_2Pos = faceList.at(i).positions.y - 1;
		int vertex_3Pos = faceList.at(i).positions.z - 1;

		int texture_1Pos = faceList.at(i).texturecoords.x - 1;
		int texture_2Pos = faceList.at(i).texturecoords.y - 1;
		int texture_3Pos = faceList.at(i).texturecoords.z - 1;

		int normalPos = faceList.at(i).normal - 1;

		triangleList.push_back(Triangle(normalList.at(normalPos), vectorList.at(vertex_1Pos), vectorList.at(vertex_2Pos), vectorList.at(vertex_3Pos), textureCords.at(texture_1Pos), textureCords.at(texture_2Pos), textureCords.at(texture_3Pos)));
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