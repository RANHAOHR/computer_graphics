// Allow use of M_PI constant
#define _USE_MATH_DEFINES

#include <math.h>
#include <iostream>
#include "camera_geometry.h"
#include <string.h> 

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

	char DataType[128];
	float a, b, c;
	unsigned int v1, v2, v3, t1, t2, t3, n1, n2, n3;
	// Scan the file and count the faces and vertices
	VertexCount = FaceCount = 0;
	while(!feof(pObjectFile))
	{
		fscanf(pObjectFile, "%s %f %f %f\n", &DataType, &a, &b, &c);
		if(strcmp( DataType, "v" ) == 0)
            VertexCount++;
		else if(strcmp( DataType, "vt" ) == 0)
			TextureCount++;
		else if(strcmp( DataType, "vn" ) == 0)
			NormalCount++;
		else if(strcmp( DataType, "f" ) == 0)
			FaceCount++;
	}
	pVertexList = new Vertex[VertexCount];
	pFaceList = new Face[FaceCount];
	pVertextTextureList = new Vertex[TextureCount];
	pVertexNormalList = new Vertex[NormalCount];


	fseek(pObjectFile, 0L, SEEK_SET);

	cout << "Number of vertices: " << VertexCount << endl;
	cout << "Number of faces: " << FaceCount << endl;
	cout << "Number of VT: " << TextureCount << endl;
	cout << "Number of VN: " << NormalCount << endl;

	// Load and create the faces and vertices
	int CurrentVertex = 0, CurrentNormal = 0, CurrentTexture = 0, CurrentFace = 0;
	float MinimumX, MaximumX, MinimumY, MaximumY, MinimumZ, MaximumZ;
	while(!feof(pObjectFile))
	{

        int res = fscanf(pObjectFile, "%s", DataType);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

		if(strcmp( DataType, "v" ) == 0)
		{
			fscanf(pObjectFile, "%f %f %f\n", &a, &b, &c);
			pVertexList[CurrentVertex].x = a;
			pVertexList[CurrentVertex].y = b;
			pVertexList[CurrentVertex].z = c;
			// Track maximum and minimum coordinates for use in bounding boxes
			if(CurrentVertex == 0)
			{
				MinimumX = MaximumX = a;
				MinimumY = MaximumY = b;
				MinimumZ = MaximumZ = c;
			}
			else
			{
				if(a < MinimumX)
					MinimumX = a;
				else if(a > MaximumX)
					MaximumX = a;
				if(b < MinimumY)
					MinimumY = b;
				else if(b > MaximumY)
					MaximumY = b;
				if(c < MinimumZ)
					MinimumZ = c;
				else if(c > MaximumZ)
					MaximumZ = c;
			}
			CurrentVertex++;
		}
		else if(strcmp( DataType, "vt" ) == 0)
		{
			fscanf(pObjectFile, "%f %f %f\n", &a, &b, &c);
			pVertextTextureList[CurrentTexture].x = a;
			pVertextTextureList[CurrentTexture].y = b;
			pVertextTextureList[CurrentTexture].z = c;
			CurrentTexture++;
		}else if(strcmp( DataType, "vn" ) == 0){
			fscanf(pObjectFile, "%f %f %f\n", &a, &b, &c);
			pVertexNormalList[CurrentNormal].x = a;
			pVertexNormalList[CurrentNormal].y = b;
			pVertexNormalList[CurrentNormal].z = c;
			CurrentNormal++;
		}
		else if(strcmp( DataType, "f" ) == 0)
		{
			fscanf(pObjectFile, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3 );

			// Convert to a zero-based index for convenience
			pFaceList[CurrentFace].v1 = v1 - 1;
			pFaceList[CurrentFace].v2 = v2 - 1;
			pFaceList[CurrentFace].v3 = v3 - 1;

			pFaceList[CurrentFace].t1 = t1 - 1;
			pFaceList[CurrentFace].t2 = t2 - 1;
			pFaceList[CurrentFace].t3 = t3 - 1;

			pFaceList[CurrentFace].n1 = n1 - 1;
			pFaceList[CurrentFace].n2 = n2 - 1;
			pFaceList[CurrentFace].n3 = n3 - 1;			

			CurrentFace++;
		}
	}

	// Initialize the bounding box vertices
	pBoundingBox[0].x = MinimumX; pBoundingBox[0].y = MinimumY; pBoundingBox[0].z = MinimumZ;
	pBoundingBox[1].x = MaximumX; pBoundingBox[1].y = MinimumY; pBoundingBox[1].z = MinimumZ;
	pBoundingBox[2].x = MinimumX; pBoundingBox[2].y = MaximumY; pBoundingBox[2].z = MinimumZ;
	pBoundingBox[3].x = MaximumX; pBoundingBox[3].y = MaximumY; pBoundingBox[3].z = MinimumZ;
	pBoundingBox[4].x = MinimumX; pBoundingBox[4].y = MinimumY; pBoundingBox[4].z = MaximumZ;
	pBoundingBox[5].x = MaximumX; pBoundingBox[5].y = MinimumY; pBoundingBox[5].z = MaximumZ;
	pBoundingBox[6].x = MinimumX; pBoundingBox[6].y = MaximumY; pBoundingBox[6].z = MaximumZ;
	pBoundingBox[7].x = MaximumX; pBoundingBox[7].y = MaximumY; pBoundingBox[7].z = MaximumZ;

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
	ViewPlane = 8.0;

	LookAt();
}

// Move the camera with respect to its viewing vectors
void Camera::Move(float tx, float ty, float tz)
{
	Position.x += tz*n.i + ty*v.i + tx*u.i;
	Position.y += tz*n.j + ty*v.j + tx*u.j;
	Position.z += tz*n.k + ty*v.k + tx*u.k;
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

// Calculate the new perspective projection matrix; maps into (-1,1)x(-1,1)x(0,1)
void Camera::Perspective()
{
    ProjectionMatrix[0] = ProjectionMatrix[5] = ProjectionMatrix[10] = ProjectionMatrix[15] = 1;
	ProjectionMatrix[1] = ProjectionMatrix[2] = ProjectionMatrix[3] = ProjectionMatrix[4] =
		ProjectionMatrix[6] = ProjectionMatrix[7] = ProjectionMatrix[8] = ProjectionMatrix[9] =
		ProjectionMatrix[11] = ProjectionMatrix[12] = ProjectionMatrix[13] = ProjectionMatrix[14]= 0;

	// Set the non-identity elements
	ProjectionMatrix[0] = 2*ViewPlane/ViewWidth;
	ProjectionMatrix[5] = 2*ViewPlane/ViewHeight;
	ProjectionMatrix[10] = FarPlane/(NearPlane - FarPlane);
	ProjectionMatrix[11] = -1.0;
	ProjectionMatrix[14] = FarPlane*NearPlane/(NearPlane - FarPlane);
	ProjectionMatrix[15] = 0.0;
}

// Calculate the new orthographic projection matrix; maps into (-1,1)x(-1,1)x(0,1)
void Camera::Orthographic()
{
	ProjectionMatrix[0] = ProjectionMatrix[5] = ProjectionMatrix[10] = ProjectionMatrix[15] = 1;
	ProjectionMatrix[1] = ProjectionMatrix[2] = ProjectionMatrix[3] = ProjectionMatrix[4] =
		ProjectionMatrix[6] = ProjectionMatrix[7] = ProjectionMatrix[8] = ProjectionMatrix[9] =
		ProjectionMatrix[11] = ProjectionMatrix[12] = ProjectionMatrix[13] = ProjectionMatrix[14]= 0;

	// Set the non-identity elements
	ProjectionMatrix[0] = 2/ViewWidth;
	ProjectionMatrix[5] = 2/ViewHeight;
	ProjectionMatrix[10] = NearPlane/(NearPlane - FarPlane);
}

// Calculate the new viewing transform matrix
void Camera::LookAt()
{
	ViewingMatrix[0] = u.i; ViewingMatrix[4] = u.j; ViewingMatrix[8] = u.k;
		ViewingMatrix[12] = -(u.i*Position.x + u.j*Position.y + u.k*Position.z);
	ViewingMatrix[1] = v.i; ViewingMatrix[5] = v.j; ViewingMatrix[9] = v.k;
		ViewingMatrix[13] = -(v.i*Position.x + v.j*Position.y + v.k*Position.z);
	ViewingMatrix[2] = n.i; ViewingMatrix[6] = n.j; ViewingMatrix[10] = n.k;
		ViewingMatrix[14] = -(n.i*Position.x + n.j*Position.y + n.k*Position.z);
	ViewingMatrix[3] = ViewingMatrix[7] = ViewingMatrix[11] = 0; ViewingMatrix[15] = 1;
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

// Select a new object by intersecting the selection ray with all object faces
int Select(int previous, Scene* pScene, Camera* pCamera, float x, float y)
{
	int select = previous;
	float zp = 10000, z;
	float s, t;
	Vertex temp1, temp2, temp3;

	for(int i = 0; i < pScene->ObjectCount; i++)
	{
		for(int j = 0; j < pScene->pObjectList[i].FaceCount; j++)
		{
			// Project each vertex into the standard view volume
			temp1 = pScene->pObjectList[i].pVertexList[pScene->pObjectList[i].pFaceList[j].v1];
			temp1 = Transform(pScene->pObjectList[i].ModelMatrix, temp1);
			temp1 = Transform(pCamera->ViewingMatrix, temp1);
			temp1 = Transform(pCamera->ProjectionMatrix, temp1);
			temp1.Normalize();

			temp2 = pScene->pObjectList[i].pVertexList[pScene->pObjectList[i].pFaceList[j].v2];
			temp2 = Transform(pScene->pObjectList[i].ModelMatrix, temp2);
			temp2 = Transform(pCamera->ViewingMatrix, temp2);
			temp2 = Transform(pCamera->ProjectionMatrix, temp2);
			temp2.Normalize();

			temp3 = pScene->pObjectList[i].pVertexList[pScene->pObjectList[i].pFaceList[j].v3];
			temp3 = Transform(pScene->pObjectList[i].ModelMatrix, temp3);
			temp3 = Transform(pCamera->ViewingMatrix, temp3);
			temp3 = Transform(pCamera->ProjectionMatrix, temp3);
			temp3.Normalize();

			// Use parametric equations to find the intersection between the selection ray
			// and the current (transformed) face
			t = ((temp1.x - temp3.x)*(y - temp3.y) - (temp1.y - temp3.y)*(x - temp3.x))/
				((temp1.x - temp3.x)*(temp2.y - temp3.y) + (temp1.y - temp3.y)*(temp3.x - temp2.x));
			if((temp1.x - temp3.x) != 0)
                s = (x - temp3.x + (temp3.x - temp2.x)*t)/(temp1.x - temp3.x);
			else
				s = (y - temp3.y+(temp3.y - temp2.y)*t)/(temp1.y - temp3.y);
			// Use the intersection if its valid (within the triangle) and closer
			if((s >= 0) && (t >= 0) && ((s + t) <= 1))
			{
				z = temp1.z*s + temp2.z*t + (1 - s - t)*temp3.z;
				if(z < zp)
				{
					zp = z;
					select = i;
				}
			}
		}
	}

	return select;
}

// Clip a polygon against view volume borders
// ADD CODE HERE: dummy function only copies polygons
Vertex* ClipPolygon(Vertex* input, int* out_count)
{
    //length should be either 0 or 2
    int length1 = 0;
    Vertex* output1;
    checkClipZ( input[0], input[1], length1);
    if (length1 == 2 )
    {
		Vertex* outputX1 = checkClipX( input[0], input[1], length1);
	    if(length1 == 2 ){
	        output1 = checkClipY( outputX1[0], outputX1[1], length1);
	    }
    }

    int length2 = 0;
    Vertex* output2;
    checkClipZ( input[1], input[2], length2);
    if (length2 == 2 )
    {
	    Vertex* outputX2 = checkClipX( input[1], input[2], length2);
	    if(length2 == 2 ){

	        output2  = checkClipY( outputX2[0], outputX2[1], length2);
	    }
	}

    int length3 = 0;
    Vertex* output3;
    checkClipZ(  input[2], input[0], length3 );
    if (length3 == 2 )
    {
	    Vertex* outputX3 = checkClipX( input[2], input[0], length3);
	    if(length3 == 2 ){

	        output3 = checkClipY( outputX3[0], outputX3[1], length3);
	    }
	}

    *out_count = length1 + length2 + length3;

    Vertex* output = new Vertex[*out_count];
    int i = 0;
    if(length1 == 2){
        output[i] = output1[0];
        output[i+1] = output1[1];
        i +=2;
    }

    if(length2 == 2){
        output[i] = output2[0];
        output[i+1] = output2[1];
        i +=2;
    }

    if(length3 == 2){
        output[i] = output3[0];
        output[i+1] = output3[1];
    }

    return output;
}

Vertex* checkClipX( Vertex &vert1, Vertex &vert2, int &length ){

    Vertex* newVertex;
    length = 0;
    if(vert1.x < -vert1.h){
        if( ( -vert2.h < vert2.x ) && (vert2.x <= vert2.h) ){ //point2 inside, point 1 outside
//            Vertex* newVertex = new Vertex[2];
            newVertex= new Vertex[2];
            newVertex[0] = vert2;
            float t = (vert2.h + vert2.x ) / ( (vert2.h + vert2.x) - (vert1.h + vert1.x) ); // start from v2, x+w = 0
            newVertex[1].x = (1-t) * vert2.x + t * vert1.x;
            newVertex[1].y = (1-t) * vert2.y + t * vert1.y;
            newVertex[1].z = (1-t) * vert2.z + t * vert1.z;
            newVertex[1].h = (1-t) * vert2.h + t * vert1.h;
            length = 2;
        }else if( vert2.x > vert2.h ){ //point1 outside, point 2 outside, different boundaries
            newVertex= new Vertex[2];
            float t1 = (vert2.h + vert2.x ) / ( (vert2.h + vert2.x) - (vert1.h + vert1.x) ); // start from v2, x+w = 0
            float t2 = (vert2.h - vert2.x ) / ( (vert2.h - vert2.x) - (vert1.h - vert1.x) ); // start from v2, w-x = 0
            newVertex[0].x = (1-t1) * vert2.x + t1 * vert1.x;
            newVertex[0].y = (1-t1) * vert2.y + t1 * vert1.y;
            newVertex[0].z = (1-t1) * vert2.z + t1 * vert1.z;
            newVertex[0].h = (1-t1) * vert2.h + t1 * vert1.h;

            newVertex[1].x = (1-t2) * vert2.x + t2 * vert1.x;
            newVertex[1].y = (1-t2) * vert2.y + t2 * vert1.y;
            newVertex[1].z = (1-t2) * vert2.z + t2 * vert1.z;
            newVertex[1].h = (1-t2) * vert2.h + t2 * vert1.h;
            length = 2;
        }
    }else if( ( vert1.x >= -vert1.h ) && ( vert1.x <= vert1.h )){
        if( ( -vert2.h < vert2.x ) && (vert2.x <= vert2.h) ){
            newVertex= new Vertex[2];
            newVertex[0] = vert2;
            newVertex[1] = vert1;
            length = 2;
        }else if(  vert2.x > vert2.h ){
            newVertex= new Vertex[2];
            newVertex[0] = vert1;
            float t = (vert1.h - vert1.x ) / ( (vert1.h - vert1.x) - (vert2.h - vert2.x) ); // start from v1, w-x = 0
            newVertex[1].x = (1-t) * vert1.x + t * vert2.x;
            newVertex[1].y = (1-t) * vert1.y + t * vert2.y;
            newVertex[1].z = (1-t) * vert1.z + t * vert2.z;
            newVertex[1].h = (1-t) * vert1.h + t * vert2.h;
            length = 2;
        }else if( vert2.x < -vert2.h ){
            newVertex= new Vertex[2];
            newVertex[0] = vert1;
            float t = (vert1.h + vert1.x ) / ( (vert1.h + vert1.x) - (vert2.h + vert2.x) ); // start from v1, w+x = 0
            newVertex[1].x = (1-t) * vert1.x + t * vert2.x;
            newVertex[1].y = (1-t) * vert1.y + t * vert2.y;
            newVertex[1].z = (1-t) * vert1.z + t * vert2.z;
            newVertex[1].h = (1-t) * vert1.h + t * vert2.h;
            length = 2;
        }

    }else if (vert1.x > vert1.h){
        if( ( -vert2.h < vert2.x ) && (vert2.x <= vert2.h) ){
            newVertex= new Vertex[2];
            newVertex[0] = vert2;
            float t = (vert2.h - vert2.x ) / ( (vert2.h - vert2.x) - (vert1.h - vert1.x) ); // start from v2, w-x = 0
            newVertex[1].x = (1-t) * vert2.x + t * vert1.x;
            newVertex[1].y = (1-t) * vert2.y + t * vert1.y;
            newVertex[1].z = (1-t) * vert2.z + t * vert1.z;
            newVertex[1].h = (1-t) * vert2.h + t * vert1.h;
            length = 2;
        }else if( vert2.x < -vert2.h ){
            newVertex= new Vertex[2];
            float t1 = (vert2.h + vert2.x ) / ( (vert2.h + vert2.x) - (vert1.h + vert1.x) ); // start from v2, x+w = 0
            float t2 = (vert2.h - vert2.x ) / ( (vert2.h - vert2.x) - (vert1.h - vert1.x) ); // start from v2, w-x = 0
            newVertex[0].x = (1-t1) * vert2.x + t1 * vert1.x;
            newVertex[0].y = (1-t1) * vert2.y + t1 * vert1.y;
            newVertex[0].z = (1-t1) * vert2.z + t1 * vert1.z;
            newVertex[0].h = (1-t1) * vert2.h + t1 * vert1.h;

            newVertex[1].x = (1-t2) * vert2.x + t2 * vert1.x;
            newVertex[1].y = (1-t2) * vert2.y + t2 * vert1.y;
            newVertex[1].z = (1-t2) * vert2.z + t2 * vert1.z;
            newVertex[1].h = (1-t2) * vert2.h + t2 * vert1.h;
            length = 2;
        }
    }

    return newVertex;

}


Vertex* checkClipY( Vertex &vert1, Vertex &vert2, int &length){

    Vertex* newVertex;
    length = 0;
    if(vert1.y < -vert1.h){
        if( ( -vert2.h < vert2.y ) && (vert2.y <= vert2.h) ){ //point2 inside, point 1 outside
//            Vertex* newVertex = new Vertex[2];
            newVertex= new Vertex[2];
            newVertex[0] = vert2;
            float t = (vert2.h + vert2.y ) / ( (vert2.h + vert2.y) - (vert1.h + vert1.y) ); // start from v2, y+w = 0
            newVertex[1].x = (1-t) * vert2.x + t * vert1.x;
            newVertex[1].y = (1-t) * vert2.y + t * vert1.y;
            newVertex[1].z = (1-t) * vert2.z + t * vert1.z;
            newVertex[1].h = (1-t) * vert2.h + t * vert1.h;
            length = 2;
        }else if( vert2.y > vert2.h ){ //point1 outside, point 2 outside, different boundaries
            newVertex= new Vertex[2];
            float t1 = (vert2.h + vert2.y ) / ( (vert2.h + vert2.y) - (vert1.h + vert1.y) ); // start from v2, y+w = 0
            float t2 = (vert2.h - vert2.y ) / ( (vert2.h - vert2.y) - (vert1.h - vert1.y) ); // start from v2, w-y = 0
            newVertex[0].x = (1-t1) * vert2.x + t1 * vert1.x;
            newVertex[0].y = (1-t1) * vert2.y + t1 * vert1.y;
            newVertex[0].z = (1-t1) * vert2.z + t1 * vert1.z;
            newVertex[0].h = (1-t1) * vert2.h + t1 * vert1.h;

            newVertex[1].x = (1-t2) * vert2.x + t2 * vert1.x;
            newVertex[1].y = (1-t2) * vert2.y + t2 * vert1.y;
            newVertex[1].z = (1-t2) * vert2.z + t2 * vert1.z;
            newVertex[1].h = (1-t2) * vert2.h + t2 * vert1.h;
            length = 2;
        }
    }else if( ( vert1.y >= -vert1.h ) && ( vert1.y <= vert1.h )){
        if( ( -vert2.h < vert2.y ) && (vert2.y <= vert2.h) ){
            newVertex= new Vertex[2];
            newVertex[0] = vert2;
            newVertex[1] = vert1;
            length = 2;
        }else if(  vert2.y > vert2.h ){
            newVertex= new Vertex[2];
            newVertex[0] = vert1;
            float t = (vert1.h - vert1.y ) / ( (vert1.h - vert1.y) - (vert2.h - vert2.y) ); // start from v1, w-x = 0
            newVertex[1].x = (1-t) * vert1.x + t * vert2.x;
            newVertex[1].y = (1-t) * vert1.y + t * vert2.y;
            newVertex[1].z = (1-t) * vert1.z + t * vert2.z;
            newVertex[1].h = (1-t) * vert1.h + t * vert2.h;
            length = 2;
        }else if( vert2.y < -vert2.h ){
            newVertex= new Vertex[2];
            newVertex[0] = vert1;
            float t = (vert1.h + vert1.y ) / ( (vert1.h + vert1.y) - (vert2.h + vert2.y) ); // start from v1, w+y = 0
            newVertex[1].x = (1-t) * vert1.x + t * vert2.x;
            newVertex[1].y = (1-t) * vert1.y + t * vert2.y;
            newVertex[1].z = (1-t) * vert1.z + t * vert2.z;
            newVertex[1].h = (1-t) * vert1.h + t * vert2.h;
            length = 2;
        }

    }else if (vert1.y > vert1.h){
        if( ( -vert2.h < vert2.y ) && (vert2.y <= vert2.h) ){
            newVertex= new Vertex[2];
            newVertex[0] = vert2;
            float t = (vert2.h - vert2.y ) / ( (vert2.h - vert2.y) - (vert1.h - vert1.y) ); // start from v2, w-y = 0
            newVertex[1].x = (1-t) * vert2.x + t * vert1.x;
            newVertex[1].y = (1-t) * vert2.y + t * vert1.y;
            newVertex[1].z = (1-t) * vert2.z + t * vert1.z;
            newVertex[1].h = (1-t) * vert2.h + t * vert1.h;
            length = 2;
        }else if( vert2.y < -vert2.h ){
            newVertex= new Vertex[2];
            float t1 = (vert2.h + vert2.y ) / ( (vert2.h + vert2.y) - (vert1.h + vert1.y) ); // start from v2, y+w = 0
            float t2 = (vert2.h - vert2.y ) / ( (vert2.h - vert2.y) - (vert1.h - vert1.y) ); // start from v2, w-y = 0
            newVertex[0].x = (1-t1) * vert2.x + t1 * vert1.x;
            newVertex[0].y = (1-t1) * vert2.y + t1 * vert1.y;
            newVertex[0].z = (1-t1) * vert2.z + t1 * vert1.z;
            newVertex[0].h = (1-t1) * vert2.h + t1 * vert1.h;

            newVertex[1].x = (1-t2) * vert2.x + t2 * vert1.x;
            newVertex[1].y = (1-t2) * vert2.y + t2 * vert1.y;
            newVertex[1].z = (1-t2) * vert2.z + t2 * vert1.z;
            newVertex[1].h = (1-t2) * vert2.h + t2 * vert1.h;
            length = 2;
        }
    }

    return newVertex;

}

void checkClipZ( Vertex &vert1, Vertex &vert2,int &length){
    Vertex* newVertex;


    if (vert1.z <= 0.0 || vert2.z <= 0.0)
    {
	    length = 0;
    }else{
    	length = 2;
    }

}