#ifndef MESH_H
#define MESH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "MathTool.h"

enum{
	NODE_FREE,
	NODE_FIXED,
	NODE_LOADED
};

typedef struct{
	Vec3d position;
	Vec3d new_position;
	int state;
}Node;

typedef struct{
	Vec3d position[3];
	Vec3d normal;
}Facet;

typedef struct{
	unsigned int node_index[4];
	Vec3d node[4];
	Vec3d new_position[4];
	VecNd deformation;
	int material;
	Matd K;
	Matd D;
	Matd B;
	Matd dN;

	double poisson_ratio;
	double young_modulus;
	double volume;
	double mises_stress;
	VecNd strain;
	VecNd stress;
}Tetrahedra;

typedef struct{
	unsigned int num_node;
	unsigned int num_facet;
	unsigned int num_tetrahedra;
	Node *node;
	Facet *facet;
	Tetrahedra *tetrahedra;

	unsigned int num_S;
	unsigned int *S;//ディリクレ拘束条件以外のノード集合
	unsigned int num_Sd;
	unsigned int *Sd;//Sのうち、ディリクレ荷重条件のノード集合
	unsigned int num_Sn;
	unsigned int *Sn;//Sのうち、ノイマン条件のノード集合
	//境界条件が設定されているかどうか
	int is_boundary_on;

	VecNd force;
	VecNd Fd;
	VecNd Fn;

	VecNd deformation;
	VecNd Ud;
	VecNd Un;

	Matd K;
	Matd Ks;
	Matd Ls;
	Matd Ldd;
	Matd Lnd;
}Mesh;

//要素の初期化
void initTetrahedra( Tetrahedra *_tetrahedra );
//要素の開放
void releaseTetrahedra( Tetrahedra *_tetrahedra );

//メッシュの初期化
void initMesh( Mesh *_mesh );
//メッシュのノード，要素のメモリ確保を行う
void setMeshDim( Mesh *_mesh );
//メッシュのノード，要素のメモリを開放する
void releaseMesh( Mesh *_mesh );
//メッシュの頂点，面，法線データを読み込む
int loadSTLMesh( Mesh *_mesh, const char *_filename );
//メッシュのノード，要素データを読み込む
int loadFEMMesh( Mesh *_mesh, const char *_filename );
//メッシュのノードを全解除
void clearMeshNodeSelected( Mesh *_mesh );
//メッシュのノードを選択
int selectMeshNodeNear( Mesh *_mesh, Vec3d *_pos, int _state, Vec3d *_selected );


#endif