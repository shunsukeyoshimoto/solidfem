#ifndef GLTOOL_H //インクルードガード
#define GLTOOL_H
#include "MathTool.h"
#include "Mesh.h"
#include "SolidFEM.h"
#include <GL/glut.h>

typedef struct
{
	int id;
	float position[4];
	float ambient[4];
	float diffuse[4];
	float specular[4];
}Lightf;
typedef struct
{
	float ambient[4];
	float diffuse[4];
	float specular[4];
}Materialf;
//擬似カラーコードの計算（HからRGBへの変換）
void calPseudoColor(double _value, Vec3d *_color);
//メッシュを描画する
void renderSTLMesh( Mesh *_mesh );
void renderFEMMesh( Mesh *_mesh, double _max_mises_stress );
//ウィンドウ座標の点のデプス値を取得する
float getDepth( int _pos_window_x, int _pos_window_y );
//ワールド座標からウィンドウ座標へ変換する
void convertWorld2Window( Vec3d *_position_world, Vec3d *_position_window );
//ウィンドウ座標からワールド座標へ変換する
void convertWindow2World( Vec3d *_position_window, Vec3d *_position_world);
//OpenGLの初期化
void glInit( void );
//カメラの設定
void setCamera( int _width, int _height );
//ライトの設定
void setLight( Lightf *_light );
//質感の設定
void setMaterial( Materialf *_material );
void setVec4f( float *_vec, float _x, float _y, float _z, float _w);
//回転変換行列の設定
void setMouseRotation( double _x, double _y, Matd *_dst );

#endif