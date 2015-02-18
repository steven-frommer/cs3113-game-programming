#pragma once
class Entity{
public:

	Entity(int texture, float posX,float posY, float rotation, float height, float width);

	void Draw();
	float x;
	float y;
	float rotation;
	int textureID;

	float width;
	float height;
	float speed;
	float direction_x;
	float direction_y; 

	~Entity();
};