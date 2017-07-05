#include "MathTool.h"
#include "Mesh.h"
#include "SolidFEM.h"
#include "GLTool.h"
#include <GL/glut.h>

#ifndef M_PI
#define M_PI 3.141592653589793238
#endif

//ウィンドウの設定を記述
#define WINDOW_POSITION_X	100
#define WINDOW_POSITION_Y	100
#define WINDOW_SIZE_X			640
#define WINDOW_SIZE_Y			480

//マウスによる回転角のゲイン
#define ANGLE_GAIN					5

enum{
	IM_NODE_SELECTION,
	IM_OBJECT_ROTATION,
	IM_FEM_ANALYSIS
};

//パラメータ設定・単位を必ずコメントで書きましょう
#define POISSONRATIO				0.49		//[arb. unit]
#define YOUNGMODULUS			10		//[MPa]
#define MAX_MS							50.0		//[N/m^2]

//必要なグローバル変数を宣言
//光源
Lightf light;
//質感
Materialf material;
//物体
Mesh mesh;
//座標変換行列
Matd matrix_object2world;
Matd matrix_world2object;
Matd matrix4opengl;
//ウィンドウの幅
int width;
//ウィンドウの高さ
int height;
//マウスの過去の二次元座標
int pre_x;
int pre_y;
Vec3d pos_clicked;
Vec3d pos_dragged;
//入力インタフェースのモード
int interface_mode;

//ディスプレイ・コールバック関数
void glDisplay( void )
{
	//描画バッファをリセットする
	glClearColor( 1.0, 1.0, 1.0, 1.0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	//カメラの設定を行う
	setCamera( width, height );

	//光源の設定を行う
	setLight( &light );

	//光源を有効に
	//glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 + light.id );
	//デプスバッファを有効に
	glEnable( GL_DEPTH );
	//質感の設定を行う
	setMaterial( &material );
	//物体を描画する
	glPushMatrix();
	glMultMatrixd( matrix4opengl.X );
	renderFEMMesh( &mesh, MAX_MS );
	glPopMatrix();
	glDisable( GL_DEPTH );
	glDisable( GL_LIGHTING );
	glDisable( GL_LIGHT0 + light.id );

	//フレームバッファを入れ替える
	glutSwapBuffers();
}

//アイドル・コールバック関数
void glIdle( void )
{
	glutPostRedisplay();
}

//マウス・コールバック関数
void glMouse( int _button, int _state, int _x, int _y )
{
	Vec3d pos_window;
	Vec3d pos_world;
	Vec3d pos_object;
	if( _state == GLUT_DOWN ){//クリックした際には下記を実行
		pos_window.x = _x;
		pos_window.y = _y;
		//クリックした点のZバッファを再取得
		pos_window.z = getDepth( _x, _y );
		convertWindow2World( &pos_window, &pos_world );
		multiTransferMatandVec3( &matrix_world2object, &pos_world, &pos_object );
		if( interface_mode == IM_NODE_SELECTION || interface_mode == IM_FEM_ANALYSIS ){
			switch ( _button ) {
				case GLUT_LEFT_BUTTON://左ボタンの場合
					selectMeshNodeNear( &mesh, &pos_object, NODE_FIXED, &pos_object );
					break;
				case GLUT_MIDDLE_BUTTON://中央ボタンの場合
					if( selectMeshNodeNear( &mesh, &pos_object, NODE_LOADED, &pos_object ) != -1 ){
						//荷重領域を設定
						setLoadedRegion( &mesh );
						memcpy( pos_clicked.X, pos_object.X, sizeof( double ) * 3 );
						memcpy( pos_dragged.X, pos_object.X, sizeof( double ) * 3 );
					}
					break;
				case GLUT_RIGHT_BUTTON://右ボタンの場合
					//クリックされた点の拘束・強制変位条件を開放
					selectMeshNodeNear( &mesh, &pos_object, NODE_FREE, &pos_clicked );
					//荷重領域を設定
					setLoadedRegion( &mesh );
					break;
				default:
					break;
			}
		}

		pre_x = _x;
		pre_y = _y;
	}
}

//マウス動作・コールバック関数
void glMotion( int _x, int _y)
{
	double output;
	Vec3d input;
	Vec3d pos_window;
	Vec3d pos_world;
	double input_x = _x - pre_x;
	double input_y = _y - pre_y;

	//ドラッグ開始点のZバッファを再取得
	convertWorld2Window( &pos_clicked, &pos_window );
	pos_window.x = _x;
	pos_window.y = _y;
	convertWindow2World( &pos_window, &pos_world );
	//入力はオブジェクト座標系で与える
	multiTransferMatandVec3( &matrix_world2object, &pos_world, &pos_dragged );
	//ドラッグ開始点と現在のマウスの座標の相対ベクトルを入力とする
	subVec3andVec3( &pos_dragged, &pos_clicked, &input );

	switch( interface_mode ){
		case IM_OBJECT_ROTATION:
			//マウス動作による座標変換行列の設定
			setMouseRotation( input_x / ANGLE_GAIN, input_y / ANGLE_GAIN, &matrix_object2world );
			//逆行列の計算（逆変換）
			invMat( &matrix_object2world, &matrix_world2object );
			//OpenGL用に転置行列を用意
			trMat( &matrix_object2world, &matrix4opengl );
			break;
		case IM_FEM_ANALYSIS:
			//境界条件が設定されていればマウスを動かすたびに解析を実行
			if( mesh.is_boundary_on == 1){
				//境界条件を設定
				setLoadCondition( &mesh, &input );
				//剛性方程式を解く
				calStiffnessEquation( &mesh );
				//最大ミーゼス応力を計算
				output = calTotalMisessStress( &mesh );
				//結果をコンソールに表示
				printf( "Max Misess Stress = %3.3f\r", output );
			}
			break;
		default:
			break;
	}

	pre_x = _x;
	pre_y = _y;
}

//キーボード・コールバック関数
void glKeyboard( unsigned char _key, int _x, int _y )
{
	switch (_key) {
		case 'q':
		case 'Q':
		case '\033'://プログラムを終了
			exit(0);
			break;
		case 'n'://ノード選択モード
			interface_mode = IM_NODE_SELECTION;
			break;
		case 'r'://物体回転モード
			interface_mode = IM_OBJECT_ROTATION;
			break;
		case 'f'://有限要素解析モード
			interface_mode = IM_FEM_ANALYSIS;
			break;
		case 's':
			//拘束領域を設定
			setConstraintRegion( &mesh );
			//逆行列を計算する
			calPreMatrix( &mesh );
			//荷重領域を設定
			setLoadedRegion( &mesh );
			break;
		case 'i':
			//変位をクリア
			clearDeformation( &mesh );
			break;
		case 'c':
			//ノード選択をクリア
			clearMeshNodeSelected( &mesh );
			break;
		default:
			break;
	}
}

//リサイズ・コールバック関数
void glResize( int _width, int _height )
{
	width = _width;
	height = _height;
	setCamera( width, height );
}


int main( int _argc, char *_argv[] )
{
	//メッシュの初期化
	initMesh( &mesh );
	//メッシュデータのロード
	if(_argc>1)
		loadFEMMesh( &mesh, _argv[1] );
	else
		loadFEMMesh( &mesh, "sphere.fem" );
	//物性パラメータの設定
	setMaterialProperty( &mesh, POISSONRATIO, YOUNGMODULUS );
	//剛性行列の生成
	setTotalStiffnessMatrix( &mesh );

	//初期ウィンドウサイズの代入
	width = WINDOW_SIZE_X;
	height = WINDOW_SIZE_Y;

	//ライティングの設定
	light.id = 0;
	setVec4f( light.position, 0, 100, 100, 1.0 );
	setVec4f( light.ambient, 0.0, 0.0, 0.0, 1.0 );
	setVec4f( light.diffuse, 1.0, 1.0, 1.0, 1.0 );
	setVec4f( light.specular, 0.0, 0.0, 0.0, 1.0 );

	//質感の設定
	setVec4f( material.ambient, 0.0, 0.0, 0.0, 1.0 );
	setVec4f( material.diffuse, 0.0, 1.0, 1.0, 1.0 );
	setVec4f( material.specular, 0.0, 0.0, 0.0, 1.0 );

	//座標変換行列の初期化
	initMat( &matrix4opengl );
	initMat( &matrix_object2world );
	initMat( &matrix_world2object );
	setMatDim( &matrix4opengl, 4, 4 );
	setMatDim( &matrix_object2world, 4, 4 );
	setMatDim( &matrix_world2object, 4, 4 );
	identityMat( &matrix4opengl );
	identityMat( &matrix_object2world );
	identityMat( &matrix_world2object );

	//インタフェースモードの初期設定
	interface_mode = IM_OBJECT_ROTATION;

	//キーボードの使用方法表示
	printf("[q] プログラムを終了\n");
	printf("[n] ノード選択モード\n");
	printf("[r] 物体回転モード\n");
	printf("[f] 有限要素解析モード\n");
	printf("[s] 解析前処理\n");
	printf("[i] 変位をクリア\n");
	printf("[c] ノード選択をクリア\n");

	//Window設定
	glutInit( &_argc, _argv );
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowPosition( WINDOW_POSITION_X, WINDOW_POSITION_Y );
	glutInitWindowSize( WINDOW_SIZE_X, WINDOW_SIZE_Y );
	glutCreateWindow( _argv[ 0 ] );
	glutDisplayFunc( glDisplay );
	glutReshapeFunc( glResize );
	glutMouseFunc( glMouse );
	glutMotionFunc( glMotion );
	glutKeyboardFunc( glKeyboard );
	glutIdleFunc( glIdle );
	glInit();
	glutMainLoop();

	//メッシュの開放
	releaseMesh( &mesh );

	//座標変換行列の開放
	releaseMat( &matrix4opengl );
	releaseMat( &matrix_object2world );
	releaseMat( &matrix_world2object );

	return 0;
}