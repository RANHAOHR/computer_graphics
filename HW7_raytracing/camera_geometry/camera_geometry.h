#pragma once

class Vertex
{
public:
	Vertex();
	void Normalize();
	float x, y, z, h;
};

class Vector
{
public:
	float i, j, k;
};

class Face
{
public:
	int v1, v2, v3, t1, t2, t3, n1, n2, n3;
};

class Object
{
public:
	Object();
	~Object();
	void Load(char* file, float s, float rx, float ry, float rz,
				  float tx, float ty, float tz);
	void WorldTranslate(float tx, float ty, float tz);
	void WorldRotate(float rx, float ry, float rz);
	void LocalRotate(float rx, float ry, float rz);
	void LocalScale(float s);
	Vertex* pBoundingBox;
	Vertex* pVertexList;
	int VertexCount;
	Face* pFaceList;
	int FaceCount;
	Vertex* pVertexNormalList;
	int NormalCount;
	Vertex* pVertextTextureList;
	int TextureCount;

	float ModelMatrix[16];
};

class Scene
{
public:
	void Load(char* file);
	Object* pObjectList;
	int ObjectCount;
};

class Camera
{
public:
	Camera();
	void Move(float tx, float ty, float tz);
	void Pan(float y, float p);
	void ScaleView(float s);
	void MoveView(float d);
	// ADD CODE HERE: functionality to move far/near planes is needed
	void EnforceVectors();
	void Perspective();
	void Orthographic();
	void LookAt();
	Vertex Position;
	float Pitch, Yaw;
	Vector n;
	Vector v;
	Vector u;
	float FarPlane, NearPlane, ViewPlane;
	float ViewWidth, ViewHeight;
	float ViewingMatrix[16];
	float ProjectionMatrix[16];
};

Vertex Transform(float* matrix, Vertex& point);
int Select(int previous, Scene* pScene, Camera* pCamera, float x, float y);
Vertex* ClipPolygon(Vertex* input, int* out_count);
Vertex* checkClipX( Vertex &vert1, Vertex &vert2, int &length);
Vertex* checkClipY( Vertex &vert1, Vertex &vert2,int &length);
void checkClipZ( Vertex &vert1, Vertex &vert2,int &length);