#include "Mesh.h"

void initTetrahedra( Tetrahedra *_tetrahedra)
{
	initMat( &_tetrahedra->B );
	initMat( &_tetrahedra->K );
	initMat( &_tetrahedra->D );
	initMat( &_tetrahedra->dN );
	setMatDim( &_tetrahedra->B, 12, 6 );
	setMatDim( &_tetrahedra->K, 12, 12 );
	setMatDim( &_tetrahedra->D, 6, 6 );
	setMatDim( &_tetrahedra->dN, 3, 4 );

	initVecN( &_tetrahedra->strain );
	initVecN( &_tetrahedra->stress );
	initVecN( &_tetrahedra->deformation );
	setVecNDim( &_tetrahedra->strain, 6 );
	setVecNDim( &_tetrahedra->stress, 6 );
	setVecNDim( &_tetrahedra->deformation, 12 );

	_tetrahedra->poisson_ratio = 0;
	_tetrahedra->young_modulus = 0;
	_tetrahedra->volume = 0;
	_tetrahedra->mises_stress = 0;
}

void releaseTetrahedra( Tetrahedra *_tetrahedra )
{
	releaseMat( &_tetrahedra->B );
	releaseMat( &_tetrahedra->K );
	releaseMat( &_tetrahedra->D );
	releaseMat( &_tetrahedra->dN );
	releaseVecN( &_tetrahedra->strain );
	releaseVecN( &_tetrahedra->stress );
	releaseVecN( &_tetrahedra->deformation );
}


void initMesh( Mesh *_mesh )
{
	_mesh->num_S = 0;
	_mesh->num_Sd = 0;
	_mesh->num_Sn = 0;
	_mesh->S = NULL;
	_mesh->Sd = NULL;
	_mesh->Sn = NULL;
	_mesh->num_node = 0;
	_mesh->num_facet = 0;
	_mesh->num_tetrahedra = 0;
	_mesh->node = NULL;
	_mesh->facet = NULL;
	_mesh->tetrahedra = NULL;
	_mesh->is_boundary_on = -1;

	initVecN( &_mesh->deformation );
	initVecN( &_mesh->Ud );
	initVecN( &_mesh->Un );
	initVecN( &_mesh->force );
	initVecN( &_mesh->Fd );
	initVecN( &_mesh->Fn );
	initMat( &_mesh->K );
	initMat( &_mesh->Ks );
	initMat( &_mesh->Ls );
	initMat( &_mesh->Ldd );
	initMat( &_mesh->Lnd );
}

void setMeshDim( Mesh *_mesh )
{
	unsigned int i;
	_mesh->node = ( Node* )calloc( _mesh->num_node, sizeof( Node ) );
	_mesh->facet = ( Facet* )calloc( _mesh->num_facet, sizeof( Facet ) );
	_mesh->tetrahedra
		= ( Tetrahedra* )calloc( _mesh->num_tetrahedra, sizeof( Tetrahedra ) );

	for( i = 0; i < _mesh->num_node; i ++ ){
		_mesh->node[ i ].state = NODE_FREE;
	}

	for( i = 0; i < _mesh->num_tetrahedra; i ++ ){
		initTetrahedra( &_mesh->tetrahedra[ i ] );
	}

	_mesh->S = ( unsigned int* )calloc( _mesh->num_node, sizeof( unsigned int ) );
	_mesh->Sd = ( unsigned int* )calloc( _mesh->num_node, sizeof( unsigned int ) );
	_mesh->Sn = ( unsigned int* )calloc( _mesh->num_node, sizeof( unsigned int ) );
	setMatDim( &_mesh->K, 3*_mesh->num_node, 3*_mesh->num_node );
	setVecNDim( &_mesh->deformation, 3*_mesh->num_node );
	setVecNDim( &_mesh->force, 3*_mesh->num_node );
}

void releaseMesh( Mesh *_mesh )
{
	unsigned int i;
	if( _mesh->num_node > 0 ){
		free( _mesh->node );
		free( _mesh->S );
		free( _mesh->Sd );
		free( _mesh->Sn );
	}
	if( _mesh->num_facet > 0 ){
		free( _mesh->facet );
	}
	if( _mesh->num_tetrahedra > 0 ){
		free( _mesh->tetrahedra );
		for( i = 0; i < _mesh->num_tetrahedra; i ++ ){
			releaseTetrahedra( &_mesh->tetrahedra[ i ] );
		}
	}

	releaseMat( &_mesh->K );
	releaseMat( &_mesh->Ks );
	releaseMat( &_mesh->Ls );
	releaseMat( &_mesh->Ldd );
	releaseMat( &_mesh->Lnd );
	releaseVecN( &_mesh->deformation );
	releaseVecN( &_mesh->Ud );
	releaseVecN( &_mesh->Un );
	releaseVecN( &_mesh->force );
	releaseVecN( &_mesh->Fn );
	releaseVecN( &_mesh->Fd );

	initMesh( _mesh );
}

int loadSTLMesh( Mesh *_mesh, const char *_filename )
{
	FILE *file;
	char buf[256];
	char dummy1[256];
	char dummy2[256];
	unsigned int i;
	unsigned int count;

	if( (file=fopen( _filename, "r" ) ) != NULL ){
		releaseMesh( _mesh );
		while( ! feof( file ) ){
			fgets( buf, 256, file );
			if( strstr( buf, "vertex" )){
				_mesh->num_node ++;
			}
			else if( strstr( buf, "normal" ) ){
				_mesh->num_facet ++;
			}
		}
		setMeshDim( _mesh );
		rewind( file );
		count = 0;
		while( ! feof( file ) ){
			fgets( buf, 256, file );
			if(strstr(buf, "normal")){
				sscanf(buf, "%s %s %lf %lf %lf", dummy1 , dummy2
																, &_mesh->facet[ count ].normal.x
																, &_mesh->facet[ count ].normal.y
																, &_mesh->facet[ count ].normal.z);
			}
			else if(strstr(buf, "vertex")){
				sscanf(buf, "%s %lf %lf %lf", dummy1, &_mesh->facet[ count ].position[0].x
																			, &_mesh->facet[ count ].position[0].y
																			, &_mesh->facet[ count ].position[0].z);
				fgets( buf, 256, file );
				sscanf(buf, "%s %lf %lf %lf", dummy1, &_mesh->facet[ count ].position[1].x
																			, &_mesh->facet[ count ].position[1].y
																			, &_mesh->facet[ count ].position[1].z);
				fgets( buf, 256, file );
				sscanf(buf, "%s %lf %lf %lf", dummy1, &_mesh->facet[ count ].position[2].x
																			, &_mesh->facet[ count ].position[2].y
																			, &_mesh->facet[ count ].position[2].z);
				for( i =0; i < 3; i ++ ){
					memcpy( _mesh->node[ 3 * count + i ].position.X
						, _mesh->facet[ count ].position[ i ].X, sizeof( double ) * 3 );
					memcpy( _mesh->node[ 3 * count + i ].new_position.X
						, _mesh->facet[ count ].position[ i ].X, sizeof( double ) * 3 );
				}
				count ++;
			}
		}
		fclose( file );
	}
	else{//読み込みに失敗
		return -1;
	}
	return 1;
}

int loadFEMMesh( Mesh *_mesh, const char *_filename )
{
	FILE *file;
	char buf[256];
	unsigned int i,j;

	if( (file=fopen( _filename, "r" ) ) != NULL){
		releaseMesh( _mesh );
		//メッシュのノード,要素のサイズを取得
		while( fgets( buf, 256, file ) ){
			if( strstr( buf, "nNodes " ) ){
				sscanf( buf, "nNodes %d", &_mesh->num_node );
				continue;
			}
			if( strstr( buf, "nTetrahedra " ) ){
				sscanf( buf, "nTetrahedra %d", &_mesh->num_tetrahedra );
				break;
			}
		}
		setMeshDim( _mesh );
		//ノード,要素のデータをセット

		//データの行まで読み飛ばす
		while( fgets( buf, 256, file ) ){
			if( strstr( buf, "# Data " ) ){
				break;
			}
		}
		//@1以降はノードのデータ
		while( !strstr(buf, "@1") ){
			fgets( buf, 256, file );
		}
		//ノードが_mesh->num_node個続く
		for( i = 0; i < _mesh->num_node; i ++ ){
			fgets( buf, 256, file );
			sscanf( buf, "%lf %lf %lf",//注意：読み込みはlf
					&_mesh->node[ i ].position.x,
					&_mesh->node[ i ].position.y,
					&_mesh->node[ i ].position.z );
			_mesh->node[ i ].state = NODE_FREE;
			memcpy( &_mesh->node[ i ].new_position
						, &_mesh->node[ i ].position, sizeof( Vec3d ) );
		}
		//@2以降は要素のノード番号データ
		while( !strstr(buf, "@2") ){
			fgets( buf, 256, file );
		}
		//要素のノード番号が_mesh->num_tetrahedra個続く
		for( i = 0; i < _mesh->num_tetrahedra; i ++ ){
			fgets( buf, 256, file );
			sscanf( buf, "%d %d %d %d",
				&_mesh->tetrahedra[ i ].node_index[ 0 ],
				&_mesh->tetrahedra[ i ].node_index[ 1 ],
				&_mesh->tetrahedra[ i ].node_index[ 2 ],
				&_mesh->tetrahedra[ i ].node_index[ 3 ]);
			for( j = 0; j < 4; j ++ ){
				//ノード番号は１から始まるので-1する
				_mesh->tetrahedra[ i ].node_index[ j ] --;
				//要素のノード変数に座標を格納
				memcpy( &_mesh->tetrahedra[ i ].node[ j ],
						&_mesh->node[ _mesh->tetrahedra[ i ].node_index[ j ] ].position,
						sizeof( Vec3d ) );
				memcpy( &_mesh->tetrahedra[ i ].new_position[ j ],
						&_mesh->node[ _mesh->tetrahedra[ i ].node_index[ j ] ].position,
						sizeof( Vec3d ) );

			}
		}
		//@3以降は要素のマテリアル番号
		while( !strstr(buf, "@3") ){
			fgets( buf, 256, file );
		}
		//マテリアル番号が_mesh->num_tetrahedra個続く
		for( i = 0; i < _mesh->num_tetrahedra; i ++ ){
			fgets( buf, 256, file );
			sscanf( buf, "%d", &_mesh->tetrahedra[ i ].material );
		}
		fclose( file );
	}
	else{//読み込みに失敗
		return -1;
	}
	return 1;
}

void clearMeshNodeSelected( Mesh *_mesh )
{
	unsigned int i;
	_mesh->is_boundary_on = -1;
	for( i = 0; i < _mesh->num_node; i ++ ){
		_mesh->node[ i ].state = NODE_FREE;
	}
}

int selectMeshNodeNear( Mesh *_mesh, Vec3d *_pos, int _state, Vec3d *_selected )
{
	double distance = 1000;
	Vec3d tPos;
	unsigned int index;
	unsigned int i;
	for( i = 0; i < _mesh->num_node; i ++ ){
		subVec3andVec3( &_mesh->node[ i ].new_position, _pos, &tPos);
		if( distance > absVec3( &tPos ) ){
			distance = absVec3( &tPos );
			index = i;
		}
	}
	if( distance < 1 ){//1より近ければ選択
		_mesh->node[ index ].state = _state;
		*_selected = _mesh->node[ index ].position;
		return 1;
	}
	return -1;
}