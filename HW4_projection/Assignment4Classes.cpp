// Allow use of M_PI constant
#define _USE_MATH_DEFINES

#include <math.h>
#include <iostream>
#include "Assignment4Classes.h"

using namespace std;

Vertex::Vertex()
{
	x = y = z = 0;
	h = 1;
}

void Vertex::Normalize()
{
	x = x/h;
	y = y/h;
	z = z/h;
	h = 1;
}

Object::Object()
{
	pBoundingBox = new Vertex[8];
	// Load the identity for the initial modeling matrix
	ModelMatrix[0] = ModelMatrix[5] = ModelMatrix[10] = ModelMatrix[15] = 1;
	ModelMatrix[1] = ModelMatrix[2] = ModelMatrix[3] = ModelMatrix[4] =
		ModelMatrix[6] = ModelMatrix[7] = ModelMatrix[8] = ModelMatrix[9] =
		ModelMatrix[11] = ModelMatrix[12] = ModelMatrix[13] = ModelMatrix[14]= 0;
}

Object::~Object()
{
	delete [] pBoundingBox;
	delete [] pVertexList;
	delete [] pFaceList;
}

// Load an object (.obj) file
void Object::Load(char* file, float s, float rx, float ry, float rz,
				  float tx, float ty, float tz)
{
	FILE* pObjectFile = fopen(file, "r");
	if(!pObjectFile)
		cout << "Failed to load " << file << "." << endl;
	else
		cout << "Successfully loaded " << file << "." << endl;

	char DataType;
	float a, b, c;

	// Scan the file and count the faces and vertices
	VertexCount = FaceCount = 0;
	while(!feof(pObjectFile))
	{
		fscanf(pObjectFile, "%c %f %f %f\n", &DataType, &a, &b, &c);
		if(DataType == 'v')
            VertexCount++;
		else if(DataType == 'f')
			FaceCount++;
	}
	pVertexList = new Vertex[VertexCount];
	pFaceList = new Face[FaceCount];

	fseek(pObjectFile, 0L, SEEK_SET);

	cout << "Number of vertices: " << VertexCount << endl;
	cout << "Number of faces: " << FaceCount << endl;

	// Load and create the faces and vertices
	float initi_ = 1000.0;
	int CurrentVertex = 0, CurrentFace = 0;
	float MinimumX = initi_, MaximumX = -1* initi_, MinimumY = initi_, MaximumY = -1* initi_, MinimumZ = initi_, MaximumZ = -1* initi_;
	while(!feof(pObjectFile))
	{
		fscanf(pObjectFile, "%c %f %f %f\n", &DataType, &a, &b, &c);
		if(DataType == 'v')
		{
			pVertexList[CurrentVertex].x = a;
			pVertexList[CurrentVertex].y = b;
			pVertexList[CurrentVertex].z = c;

			//ADD YOUR CODE HERE :Track maximum and minimum coordinates for use in bounding boxes
			if (a < MinimumX)
			{
				MinimumX = a;
			}

			if (a > MaximumX)
			{
				MaximumX = a;
			}

			if (b < MinimumY)
			{
				MinimumY = b;
			}

			if (b > MaximumY)
			{
				MaximumY = b;
			}

			if (c < MinimumZ)
			{
				MinimumZ = c;
			}

			if (c > MaximumZ)
			{
				MaximumZ = c;
			}

			CurrentVertex++;
		}
		else if(DataType == 'f')
		{
			// Convert to a zero-based index for convenience
			pFaceList[CurrentFace].v1 = (int)a - 1;
			pFaceList[CurrentFace].v2 = (int)b - 1;
			pFaceList[CurrentFace].v3 = (int)c - 1;
			CurrentFace++;
		}
	}

	//ADD YOUR CODE HERE: Initialize the bounding box vertices
	pBoundingBox[0].x = MinimumX;
	pBoundingBox[0].y = MinimumY;
	pBoundingBox[0].z = MinimumZ;
			
	pBoundingBox[1].x = MinimumX;
	pBoundingBox[1].y = MaximumY;
	pBoundingBox[1].z = MinimumZ;

	pBoundingBox[2].x = MaximumX;
	pBoundingBox[2].y = MaximumY;
	pBoundingBox[2].z = MinimumZ;

	pBoundingBox[3].x = MaximumX;
	pBoundingBox[3].y = MinimumY;
	pBoundingBox[3].z = MinimumZ;


	pBoundingBox[4].x = MinimumX;
	pBoundingBox[4].y = MinimumY;
	pBoundingBox[4].z = MaximumZ;
			
	pBoundingBox[5].x = MinimumX;
	pBoundingBox[5].y = MaximumY;
	pBoundingBox[5].z = MaximumZ;

	pBoundingBox[6].x = MaximumX;
	pBoundingBox[6].y = MaximumY;
	pBoundingBox[6].z = MaximumZ;

	pBoundingBox[7].x = MaximumX;
	pBoundingBox[7].y = MinimumY;
	pBoundingBox[7].z = MaximumZ;


	// Apply the initial transformations in order
	LocalScale(s);
	WorldRotate((float)(M_PI*rx/180.0), (float)(M_PI*ry/180.0), (float)(M_PI*rz/180.0));
	WorldTranslate(tx, ty, tz);	
}

// Do world-based translation
void Object::WorldTranslate(float tx, float ty, float tz)
{
	ModelMatrix[12] += tx;
	ModelMatrix[13] += ty;
	ModelMatrix[14] += tz;
	
}

// Perform world-based rotations in x,y,z order (intended for one-at-a-time use)
void Object::WorldRotate(float rx, float ry, float rz)
{
		float temp[16];

	if(rx != 0)
	{
		float cosx = cos(rx), sinx = sin(rx);
		for(int i = 0; i < 16; i++)
			temp[i] = ModelMatrix[i];
		ModelMatrix[1] = temp[1]*cosx - temp[2]*sinx;
		ModelMatrix[2] = temp[2]*cosx + temp[1]*sinx;
		ModelMatrix[5] = temp[5]*cosx - temp[6]*sinx;
		ModelMatrix[6] = temp[6]*cosx + temp[5]*sinx;
		ModelMatrix[9] = temp[9]*cosx - temp[10]*sinx;
		ModelMatrix[10] = temp[10]*cosx + temp[9]*sinx;
		ModelMatrix[13] = temp[13]*cosx - temp[14]*sinx;
		ModelMatrix[14] = temp[14]*cosx + temp[13]*sinx;
	}

	if(ry != 0)
	{
		float cosy = cos(ry), siny = sin(ry);
		for(int i = 0; i < 16; i++)
			temp[i] = ModelMatrix[i];
		ModelMatrix[0] = temp[0]*cosy + temp[2]*siny;
		ModelMatrix[2] = temp[2]*cosy - temp[0]*siny;
		ModelMatrix[4] = temp[4]*cosy + temp[6]*siny;
		ModelMatrix[6] = temp[6]*cosy - temp[4]*siny;
		ModelMatrix[8] = temp[8]*cosy + temp[10]*siny;
		ModelMatrix[10] = temp[10]*cosy - temp[8]*siny;
		ModelMatrix[12] = temp[12]*cosy + temp[14]*siny;
		ModelMatrix[14] = temp[14]*cosy - temp[12]*siny;
	}

	if(rz != 0)
	{
		float cosz = cos(rz), sinz = sin(rz);
		for(int i = 0; i < 16; i++)
			temp[i] = ModelMatrix[i];
		ModelMatrix[0] = temp[0]*cosz - temp[1]*sinz;
		ModelMatrix[1] = temp[1]*cosz + temp[0]*sinz;
		ModelMatrix[4] = temp[4]*cosz - temp[5]*sinz;
		ModelMatrix[5] = temp[5]*cosz + temp[4]*sinz;
		ModelMatrix[8] = temp[8]*cosz - temp[9]*sinz;
		ModelMatrix[9] = temp[9]*cosz + temp[8]*sinz;
		ModelMatrix[12] = temp[12]*cosz - temp[13]*sinz;
		ModelMatrix[13] = temp[13]*cosz + temp[12]*sinz;
	}

}

// Perform locally-based rotations in x,y,z order (intended for one-at-a-time use)
void Object::LocalRotate(float rx, float ry, float rz)
{
	float temp[16];

	if(rx != 0)
	{
		float cosx = cos(rx), sinx = sin(rx);
		for(int i = 0; i < 16; i++)
			temp[i] = ModelMatrix[i];
		ModelMatrix[4] = temp[4]*cosx + temp[8]*sinx;
		ModelMatrix[5] = temp[5]*cosx + temp[9]*sinx;
		ModelMatrix[6] = temp[6]*cosx + temp[10]*sinx;
		ModelMatrix[7] = temp[7]*cosx + temp[11]*sinx;
		ModelMatrix[8] = temp[8]*cosx - temp[4]*sinx;
		ModelMatrix[9] = temp[9]*cosx - temp[5]*sinx;
		ModelMatrix[10] = temp[10]*cosx - temp[6]*sinx;
		ModelMatrix[11] = temp[11]*cosx - temp[7]*sinx;
	}

	if(ry != 0)
	{
        float cosy = cos(ry), siny = sin(ry);
		for(int i = 0; i < 16; i++)
			temp[i] = ModelMatrix[i];
		ModelMatrix[0] = temp[0]*cosy - temp[8]*siny;
		ModelMatrix[1] = temp[1]*cosy - temp[9]*siny;
		ModelMatrix[2] = temp[2]*cosy - temp[10]*siny;
		ModelMatrix[3] = temp[3]*cosy - temp[11]*siny;
		ModelMatrix[8] = temp[8]*cosy + temp[0]*siny;
		ModelMatrix[9] = temp[9]*cosy + temp[1]*siny;
		ModelMatrix[10] = temp[10]*cosy + temp[2]*siny;
		ModelMatrix[11] = temp[11]*cosy + temp[3]*siny;
	}

	if(rz != 0)
	{
		float cosz = cos(rz), sinz = sin(rz);
		for(int i = 0; i < 16; i++)
			temp[i] = ModelMatrix[i];
		ModelMatrix[0] = temp[0]*cosz + temp[4]*sinz;
		ModelMatrix[1] = temp[1]*cosz + temp[5]*sinz;
		ModelMatrix[2] = temp[2]*cosz + temp[6]*sinz;
		ModelMatrix[3] = temp[3]*cosz + temp[7]*sinz;
		ModelMatrix[4] = temp[4]*cosz - temp[0]*sinz;
		ModelMatrix[5] = temp[5]*cosz - temp[1]*sinz;
		ModelMatrix[6] = temp[6]*cosz - temp[2]*sinz;
		ModelMatrix[7] = temp[7]*cosz - temp[3]*sinz;
	}
}

// Do locally-based uniform scaling
void Object::LocalScale(float s)
{
	for(int i = 0; i <= 11; i++)
        ModelMatrix[i] = s*ModelMatrix[i];
}

// Load a Scene (.dat) file
void Scene::Load(char* file)
{
	FILE* pSceneFile = fopen(file, "r");
	if(!pSceneFile)
		cout << "Failed to load " << file << "." << endl;
	else
		cout << "Successfully loaded " << file << "." << endl;

	char MeshFile[255];
	float Scaling;
	float RotationX, RotationY, RotationZ;
	float TranslationX, TranslationY, TranslationZ;

	// Step through the file and count the objects
	ObjectCount = 0;
	while(!feof(pSceneFile))
	{
		fscanf(pSceneFile, "%s %f %f %f %f %f %f %f\n", MeshFile, &Scaling,
			&RotationX, &RotationY, &RotationZ, &TranslationX, &TranslationY, &TranslationZ);
		ObjectCount++;
	}
	pObjectList = new Object[ObjectCount];

	fseek(pSceneFile, 0L, SEEK_SET);

	// Step through the file and create/load the objects
    for(int i = 0; i < ObjectCount; i++)
	{
		fscanf(pSceneFile, "%s %f %f %f %f %f %f %f\n", MeshFile, &Scaling,
			&RotationX, &RotationY, &RotationZ, &TranslationX, &TranslationY, &TranslationZ);
		pObjectList[i].Load(MeshFile, Scaling, RotationX, RotationY, RotationZ,
			TranslationX, TranslationY, TranslationZ);
	}

	cout << "Number of Objects Loaded: " << ObjectCount << endl;
}

Camera::Camera()
{
	Position.x = 10.0;
	Position.y = 0.0;
	Position.z = 0.0;

	Pitch = ((float) M_PI)/2;
	Yaw = 0.0;
	v.i = 0; v.j = 0; v.k = 1;
	EnforceVectors();

	ViewWidth = 8.0;
	ViewHeight = 8.0;

	FarPlane = 10.0;
	NearPlane = 2.0;
	ViewPlane = 10.0;

	LookAt();
}



// Pan the camera about its local axes
void Camera::Pan(float y, float p)
{
	Yaw += y;
	Pitch += p;
	EnforceVectors();
	LookAt();
}

// Scale the viewing plane
void Camera::ScaleView(float s)
{
	ViewWidth = s*ViewWidth;
	ViewHeight = s*ViewHeight;
}
// Move the viewing plane toward/away from the camera
void Camera::MoveView(float d)
{
	if(ViewPlane + d > 1.0)
        ViewPlane = ViewPlane + d;
}

// Set (and normalize) the camera vectors based on the viewing angles
void Camera::EnforceVectors()
{
	float magnitude;
	Vector temp;

	n.i = sin(Pitch)*cos(Yaw);
	n.j = sin(Pitch)*sin(Yaw);
	n.k = cos(Pitch);

	if(((n.i == 0) && (n.j == 0) && (n.k == 1)) || ((n.i == 0) && (n.j == 0) && (n.k == -1)))
	{
        v.i = 1; v.j = 0; v.k = 0;
	}
	else
	{
        v.i = 0; v.j = 0; v.k = 1;
	}

	temp.i = v.j*n.k - v.k*n.j;
	temp.j = v.k*n.i - v.i*n.k;
	temp.k = v.i*n.j - v.j*n.i;
	magnitude = sqrt(temp.i*temp.i + temp.j*temp.j + temp.k*temp.k);
	u.i = temp.i/magnitude; u.j = temp.j/magnitude; u.k = temp.k/magnitude;

	v.i = n.j*u.k - n.k*u.j;
	v.j = n.k*u.i - n.i*u.k;
	v.k = n.i*u.j - n.j*u.i;
}

// float* mat_multiplication(float* mat_1, float* mat_2){
//   float mat_[16];
//   mat_[0] = mat_1[0] * mat_2[0] + mat_1[4] * mat_2[1] + mat_1[8] * mat_2[2] + mat_1[12] * mat_2[3];
//   mat_[4] = mat_1[0] * mat_2[4] + mat_1[4] * mat_2[5] + mat_1[8] * mat_2[6] + mat_1[12] * mat_2[7];
//   mat_[8] = mat_1[0] * mat_2[8] + mat_1[4] * mat_2[9] + mat_1[8] * mat_2[10] + mat_1[12] * mat_2[11];
//   mat_[12] = mat_1[0] * mat_2[12] + mat_1[4] * mat_2[13] + mat_1[8] * mat_2[14] + mat_1[12] * mat_2[15];

//   mat_[1] = mat_1[1] * mat_2[0] + mat_1[5] * mat_2[1] + mat_1[9] * mat_2[2] + mat_1[13] * mat_2[3];
//   mat_[5] = mat_1[1] * mat_2[4] + mat_1[5] * mat_2[5] + mat_1[9] * mat_2[6] + mat_1[13] * mat_2[7];
//   mat_[9] = mat_1[1] * mat_2[8] + mat_1[5] * mat_2[9] + mat_1[9] * mat_2[10] + mat_1[13] * mat_2[11];
//   mat_[13] = mat_1[1] * mat_2[12] + mat_1[5] * mat_2[13] + mat_1[9] * mat_2[14] + mat_1[13] * mat_2[15];

//   mat_[2] = mat_1[2] * mat_2[0] + mat_1[6] * mat_2[1] + mat_1[10] * mat_2[2] + mat_1[14] * mat_2[3];
//   mat_[6] = mat_1[2] * mat_2[4] + mat_1[6] * mat_2[5] + mat_1[10] * mat_2[6] + mat_1[14] * mat_2[7];
//   mat_[10] = mat_1[2] * mat_2[8] + mat_1[6] * mat_2[9] + mat_1[10] * mat_2[10] + mat_1[14] * mat_2[11];
//   mat_[14] = mat_1[2] * mat_2[12] + mat_1[6] * mat_2[13] + mat_1[10] * mat_2[14] + mat_1[14] * mat_2[15];

//   mat_[3] = mat_1[3] * mat_2[0] + mat_1[7] * mat_2[1] + mat_1[11] * mat_2[2] + mat_1[15] * mat_2[3];
//   mat_[7] = mat_1[3] * mat_2[4] + mat_1[7] * mat_2[5] + mat_1[11] * mat_2[6] + mat_1[15] * mat_2[7];
//   mat_[11] = mat_1[3] * mat_2[8] + mat_1[7] * mat_2[9] + mat_1[11] * mat_2[10] + mat_1[15] * mat_2[11];
//   mat_[15] = mat_1[3] * mat_2[12] + mat_1[7] * mat_2[13] + mat_1[11] * mat_2[14] + mat_1[15] * mat_2[15];

//   return mat_;
// }

// Calculate the new perspective projection matrix
void Camera::Perspective()
{
	//ADD YOUR CODE HERE!!
	ProjectionMatrix[0] = 2 * ViewPlane / ViewWidth;
	ProjectionMatrix[5] = 2 * ViewPlane / ViewHeight;
	ProjectionMatrix[8] = 0;
	ProjectionMatrix[9] = 0;
	ProjectionMatrix[10] = -(NearPlane + FarPlane) / (FarPlane - NearPlane);
	ProjectionMatrix[11] = -1;	
	ProjectionMatrix[14] = -2*FarPlane * NearPlane / (FarPlane - NearPlane);	

	ProjectionMatrix[1] = ProjectionMatrix[2] = ProjectionMatrix[3] = ProjectionMatrix[4] = 0;
	ProjectionMatrix[6] = ProjectionMatrix[7] = ProjectionMatrix[12] = ProjectionMatrix[13] = ProjectionMatrix[15] = 0;
}

// Calculate the new orthographic projection matrix
void Camera::Orthographic()
{
	//ADD YOUR CODE HERE!!
	ProjectionMatrix[0] = 2 / ViewWidth;
	ProjectionMatrix[5] = 2 / ViewHeight;
	ProjectionMatrix[8] = 0;
	ProjectionMatrix[9] = 0;	
	ProjectionMatrix[10] = -2 / (FarPlane - NearPlane);
	ProjectionMatrix[14] = -(NearPlane + FarPlane) / (FarPlane - NearPlane);
	ProjectionMatrix[15] = 1;	

	ProjectionMatrix[1] = ProjectionMatrix[2] = ProjectionMatrix[3] = ProjectionMatrix[4] = 0;
	ProjectionMatrix[6] = ProjectionMatrix[7] = ProjectionMatrix[11] = ProjectionMatrix[12] = ProjectionMatrix[13] = 0;	
}

// Calculate the new viewing transform matrix
void Camera::LookAt()
{
	//ADD YOUR CODE HERE!!
	ViewingMatrix[0] = u.i;
	ViewingMatrix[4] = u.j;
	ViewingMatrix[8] = u.k;	
	ViewingMatrix[12] = -u.i * Position.x - u.j * Position.y - u.k * Position.z;

	ViewingMatrix[1] = v.i;
	ViewingMatrix[5] = v.j;
	ViewingMatrix[9] = v.k;	
	ViewingMatrix[13] = -v.i * Position.x - v.j * Position.y - v.k * Position.z;

	ViewingMatrix[2] = n.i;
	ViewingMatrix[6] = n.j;
	ViewingMatrix[10] = n.k;	
	ViewingMatrix[14] = -n.i * Position.x - n.j * Position.y - n.k * Position.z;

	ViewingMatrix[3] = 0;
	ViewingMatrix[7] = 0;
	ViewingMatrix[11] = 0;	
	ViewingMatrix[15] = 1;		
}

// Transform a point with an arbitrary matrix
Vertex Transform(float* matrix, Vertex& point)
{
	Vertex temp;
	temp.x = matrix[0]*point.x + matrix[4]*point.y + matrix[8]*point.z + matrix[12]*point.h;
	temp.y = matrix[1]*point.x + matrix[5]*point.y + matrix[9]*point.z + matrix[13]*point.h;
	temp.z = matrix[2]*point.x + matrix[6]*point.y + matrix[10]*point.z + matrix[14]*point.h;
	temp.h = matrix[3]*point.x + matrix[7]*point.y + matrix[11]*point.z + matrix[15]*point.h;
	return temp;

}





