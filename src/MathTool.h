#ifndef MATHTOOL_H //インクルードガード
#define MATHTOOL_H

#ifndef M_PI
#define M_PI 3.141592653589793238
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//回転軸の名前
enum{
	ROT_AXIS_Z,
	ROT_AXIS_X,
	ROT_AXIS_Y
};

typedef union
{
	//x,y,zとX[0],X[1],X[2]がそれぞれ対応
	struct{double x,y,z;};
	double X[3];
}Vec3d;

typedef struct
{
	//ベクトルの次元
	unsigned int dim;
	double *X;
}VecNd;

typedef struct
{
	//列数
	unsigned int ncol;
	//行数
	unsigned int nrow;
	//要素数
	unsigned int size;
	double *X;
}Matd;

/*ベクトル計算に関わる関数*/
//ベクトルの初期化
void initVecN( VecNd *_A );

//ベクトルのゼロクリア
void clearVec3( Vec3d *_A );
void clearVecN( VecNd *_A );

//次元の設定
void setVecNDim( VecNd *_A, unsigned int _dim );

//ベクトルの解放
void releaseVecN( VecNd *_A );

//ベクトルの足し算
void sumVec3andVec3( Vec3d *_A, Vec3d *_B, Vec3d *_dst );
void sumVecNandVecN( VecNd *_A, VecNd *_B, VecNd *_dst );

//ベクトルの引き算
void subVec3andVec3( Vec3d *_A, Vec3d *_B, Vec3d *_dst );
void subVecNandVecN( VecNd *_A, VecNd *_B, VecNd *_dst );

//ベクトルのスケーリング
void scalingVec3( double _s, Vec3d *_A, Vec3d *_dst );
void scalingVecN( double _s, VecNd *_A, VecNd *_dst );

//ベクトルの内積
double dotVec3andVec3(Vec3d *_A, Vec3d *_B);
double dotVecNandVecN(VecNd *_A, VecNd *_B);

//ベクトルの外積
void crossVec3andVec3(Vec3d *_A, Vec3d *_B, Vec3d *_dst);

//ベクトルの絶対値
double absVec3( Vec3d *_A );
double absVecN( VecNd *_A );

//ベクトルのコンソールへの出力
void printVec3(Vec3d *_A);
void printVecN(VecNd *_A);

/*行列計算に関わる関数*/
//行列の初期化
void initMat( Matd *_A );

//行列のゼロクリア
void clearMat( Matd *_A );

//単位行列をセット
void identityMat( Matd *_A );

//次元の設定
void setMatDim( Matd *_A, unsigned int _ncol, unsigned int _nrow );

//行列の解放
void releaseMat( Matd *_A );

//行列の足し算
void sumMatandMat( Matd *_A, Matd *_B, Matd *_dst );

//行列の引き算
void subMatandMat( Matd *_A, Matd *_B, Matd *_dst );

//行列の掛け算
void multiMatandMat( Matd *_A, Matd *_B, Matd *_dst );
void multiMatandVec3( Matd *_A, Vec3d *_B, VecNd *_dst );
void multiTransferMatandVec3( Matd *_A, Vec3d *_B, Vec3d *_dst );
void multiMatandVecN( Matd *_A, VecNd *_B, VecNd *_dst );

//行列のスケーリング
void scalingMat( double _s, Matd *_A, Matd *_dst );

//行列式
double detMat( Matd *_A);

//転置行列
void trMat( Matd *_A, Matd *_dst);

//逆行列
void invMat( Matd *_A, Matd *_dst);
double lu( int n, double *a, int *ip );
double invMatbyLU( double *a_inv, double *a, int n );

//回転行列の生成
void setRotationalMatrix( double _angle, int _axis,  Matd *_dst );

//行列のコンソールへの出力
void printMat(Matd *_A);


#endif