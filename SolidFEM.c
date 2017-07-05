#include "SolidFEM.h"

void setMaterialProperty( Mesh *_mesh, double _poisson_ratio, double _young_modulus )
{
	unsigned int i;
	for( i = 0; i < _mesh->num_tetrahedra; i ++ ){
		_mesh->tetrahedra[ i ].poisson_ratio = _poisson_ratio;
		_mesh->tetrahedra[ i ].young_modulus = _young_modulus;
	}
}

void setGeometoryMatrix( Tetrahedra *_tetrahedra, Matd *_G)
{
	unsigned int i,j;
	initMat( _G );
	setMatDim( _G, 4, 4 );
	for( i = 0; i < 4; i ++){
		for( j = 0; j < 4; j ++){
			if( j == 0 ){
				_G->X[ 4 * i + j ] = 1;
			}
			else{
				_G->X[ 4 * i + j ] = _tetrahedra->new_position[ i ].X[ j - 1 ];
			}
		}
	}
}

void calStrain( Tetrahedra *_tetrahedra )
{
	//[TODO]
	//変位-ひずみ関係式からひずみを求める処理
	//ヒント：関数multiMatandVecN( Matd *_A, VecNd *_B, VecNd *_dst )を使う
}

void calStress( Tetrahedra *_tetrahedra )
{
	calStrain( _tetrahedra );
	//[TODO]
	// 応力-ひずみ関係式から応力を求める処理
}

double calMisesStress( Tetrahedra *_tetrahedra )
{
	calStress( _tetrahedra );
	_tetrahedra->mises_stress
		=
		//[TODO]
		//ミーゼス応力の計算
		//_tetrahedra->stress.X[0]~_tetrahedra->stress.X[5]がひずみの各成分なので・・・
	return _tetrahedra->mises_stress;
}

double calVolume( Tetrahedra *_tetrahedra)
{
	Matd G;
	initMat( &G );
	setGeometoryMatrix( _tetrahedra, &G );
	_tetrahedra->volume = fabs( detMat( &G ) ) / 6.0;
	releaseMat( &G );
	return _tetrahedra->volume;
}

void setStrainDeformationMatrix( Tetrahedra *_tetrahedra )
{
	unsigned int i,j;
	Matd G;
	Matd invG;
	initMat( &G );
	initMat( &invG );
	calVolume( _tetrahedra );
	setGeometoryMatrix( _tetrahedra, &G );
	invMat( &G, &invG );
	for( i = 0; i < 4; i ++ ){
		for( j = 0; j < 3; j ++ ){
			_tetrahedra->dN.X[ 3 * i + j ] = invG.X[ 4 * ( j + 1) + i ];
		}
	}
	//[TODO]
	//ひずみ変位関係係数Bを求める処理
	//_tetrahedra->B.X[0]~_tetrahedra->B.X[71]を生成したい
	//ヒント：dNは形状関数の空間微分なので・・・
	releaseMat( &G );
	releaseMat( &invG );
}

void setStressStrainMatrix( Tetrahedra *_tetrahedra )
{
	unsigned int i;
	double Dscale;
	Dscale=_tetrahedra->young_modulus /
		( ( 1.0 + _tetrahedra->poisson_ratio ) * ( 1.0 - 2.0 * _tetrahedra->poisson_ratio ) );
	for( i = 0; i < 3 ; i ++ ){
		_tetrahedra->D.X[ 6 * i + i ] = Dscale * ( 1.0 - _tetrahedra->poisson_ratio );
		_tetrahedra->D.X[ 6 * i + ( i + 1 ) % 3 ] = Dscale * _tetrahedra->poisson_ratio;
		_tetrahedra->D.X[ 6 * i + ( i + 2 ) % 3 ] = Dscale * _tetrahedra->poisson_ratio;
		_tetrahedra->D.X[ 6 * ( i + 3 ) + i + 3 ]
			= Dscale * ( 1.0 - 2 * _tetrahedra->poisson_ratio ) / 2;
	}
}

void setStiffnessMatrix( Tetrahedra *_tetrahedra )
{
	Matd trB;
	Matd temp1;
	Matd temp2;
	initMat( &trB );
	initMat( &temp1 );
	initMat( &temp2 );
	trMat( &_tetrahedra->B, &trB );
	multiMatandMat( &_tetrahedra->D, &_tetrahedra->B, &temp1 );
	multiMatandMat( &trB, &temp1, &temp2 );
	scalingMat( _tetrahedra->volume, &temp2, &_tetrahedra->K );
	releaseMat( &trB );
	releaseMat( &temp1 );
	releaseMat( &temp2 );
}


void setTotalStiffnessMatrix( Mesh *_mesh )
{
	unsigned int i,j,k,l,m;
	unsigned int col,row;
	clearMat(&_mesh->K);
	for( i = 0; i<_mesh->num_tetrahedra; i ++ ){
		setStrainDeformationMatrix( &_mesh->tetrahedra[ i ] );
		setStressStrainMatrix( &_mesh->tetrahedra[ i ] );
		setStiffnessMatrix( &_mesh->tetrahedra[ i ] );
		//要素合成行列の足し込み

		//[TODO]
		//ノード番号の配列に要素剛性行列を足していく
		//全体剛性行列:_mesh->Kを生成したい
		//要素剛性行列:_mesh->tetrahedra[#].K
		//要素ノードの番号：_mesh->tetrahedra[#].node_index[#]

		//ヒント：row行col列目の配列要素には，_mesh->K.[3*_mesh->num_node*row+col]でアクセスできる．
		//ヒント：行列は12×12のサイズ．一つのノードにつき三自由度あるので注意が必要．
	}
}

void setConstraintRegion( Mesh *_mesh )
{
	unsigned int i;
	_mesh->num_S = 0;
	for( i = 0; i < _mesh->num_node; i ++ ){
		//[TODO]
		//ディリクレ拘束以外の点のノード番号を分解後のノード順序に対応づける
		//_mesh->Sを生成する
		//ヒント：_mesh->node[ i ].stateはNODE_FIXED（拘束）かどうか
		//ヒント：_mesh->S[]に順番にノード番号を格納する
		//ヒント：_mesh->num_S（分解後の総ノード数をカウントアップ）
	}
	if(_mesh->num_S > 0){
		_mesh->is_boundary_on = 1;
	}
}

void calPreMatrix( Mesh *_mesh )
{
	unsigned int i,j,k;
	int row, col;
	if(	_mesh->is_boundary_on == 1 ){
		setMatDim( &_mesh->Ks,  _mesh->num_S * 3, _mesh->num_S * 3 );
		setMatDim( &_mesh->Ls,  _mesh->num_S * 3, _mesh->num_S * 3 );
		for( i = 0; i < _mesh->num_S ; i ++ ){//並び替えた後の順序
			for( j = 0; j < _mesh->num_S; j ++ ){//並び替えた後の順序
				for( k = 0; k < 3; k++ ){//自由度
					row= 3 * _mesh->S[ i ] + k ;
					col= 3 * _mesh->S[ j ];
					memcpy( &_mesh->Ks.X[ _mesh->num_S * 3 *( 3 * i + k ) + 3 * j ],
							&_mesh->K.X[ _mesh->num_node * 3 * row + col ],
							sizeof( double ) * 3 );
				}
			}
		}
		//高速化のため,予め逆行列を計算
		invMat( &_mesh->Ks, &_mesh->Ls );
	}
}

void setLoadedRegion( Mesh *_mesh )
{
	unsigned int i,j,k;
	unsigned int col,row;
	_mesh->num_Sd = 0;
	_mesh->num_Sn = 0;
	for( i = 0; i < _mesh->num_S; i ++ ){
		switch( _mesh->node[ _mesh->S[ i ] ].state ){
			case NODE_LOADED://ディリクレ荷重条件
				_mesh->Sd[ _mesh->num_Sd ] = i;//集合はSの中から取得
				_mesh->num_Sd ++;
				break;
			case NODE_FREE://ノイマン条件
				_mesh->Sn[ _mesh->num_Sn ] = i;//集合はSの中から取得
				_mesh->num_Sn ++;
				break;
		}
	}
	//行列・ベクトルの次元が決まる
	if( _mesh->num_Sd > 0 && _mesh->num_Sn > 0 ){
		setMatDim( &_mesh->Ldd, _mesh->num_Sd * 3, _mesh->num_Sd * 3 );
		setMatDim( &_mesh->Lnd, _mesh->num_Sd * 3, _mesh->num_Sn * 3 );
		setVecNDim( &_mesh->Ud, _mesh->num_Sd * 3);
		setVecNDim( &_mesh->Un, _mesh->num_Sn * 3);
		setVecNDim( &_mesh->Fd, _mesh->num_Sd * 3);
		setVecNDim( &_mesh->Fn, _mesh->num_Sn * 3);

		for( i = 0; i < _mesh->num_Sd; i ++ ){//並び替えた後の順序
			for( j = 0; j < _mesh->num_Sd; j ++ ){//並び替えた後の順序
				for( k = 0; k < 3; k++ ){//自由度
					col = 3 * _mesh->Sd[ j ];
					row = 3 * _mesh->Sd[ i ] + k;
					memcpy( &_mesh->Ldd.X[ _mesh->num_Sd * 3 *( 3 * i + k ) + 3 * j ],
							&_mesh->Ls.X[ _mesh->num_S * 3 * row + col],
							sizeof( double ) * 3 );
				}
			}
		}
		for( i = 0; i < _mesh->num_Sn; i ++ ){//並び替えた後の順序
			for( j = 0; j < _mesh->num_Sd; j ++ ){//並び替えた後の順序
				for( k = 0; k < 3; k++ ){//自由度
					col = 3 * _mesh->Sd[ j ];
					row = 3 * _mesh->Sn[ i ] + k;
					memcpy( &_mesh->Lnd.X[ _mesh->num_Sd * 3 *( 3 * i + k ) + 3 * j ],
							&_mesh->Ls.X[ _mesh->num_S * 3 * row + col ],
							sizeof( double ) * 3 );
				}
			}
		}
	}
}

void setLoadCondition( Mesh *_mesh, Vec3d *_deformation )
{
	unsigned int i;
	clearVecN( &_mesh->Ud );
	for( i = 0; i < _mesh->num_Sd; i ++ ){
		if( _mesh->node[ _mesh->S[ _mesh->Sd[ i ] ] ].state == NODE_LOADED){
			memcpy( &_mesh->Ud.X[ 3 * i ] ,_deformation->X, sizeof(double) * 3);
		}
	}
}

void calStiffnessEquation( Mesh *_mesh )
{
	unsigned int i,j;
	Matd invLdd;
	initMat( &invLdd );
	invMat( &_mesh->Ldd, &invLdd );
	multiMatandVecN( &invLdd, &_mesh->Ud, &_mesh->Fd);
	multiMatandVecN( &_mesh->Lnd, &_mesh->Fd, &_mesh->Un );
	//境界領域集合に基づいて全体ベクトルに戻す
	for( i = 0; i < _mesh->num_Sd; i ++ ){
		memcpy( &_mesh->deformation.X[ 3 * _mesh->S[ _mesh->Sd[ i ] ] ],
				&_mesh->Ud.X[ 3 * i ], sizeof(double) * 3 );
	}
	for( i = 0; i < _mesh->num_Sn; i ++ ){
		memcpy( &_mesh->deformation.X[ 3 * _mesh->S[ _mesh->Sn[ i ] ] ],
				&_mesh->Un.X[ 3 * i ], sizeof(double) * 3 );
	}
	for( i = 0; i < _mesh->num_node; i ++ ){
		for( j = 0; j < 3; j ++ ){
			_mesh->node[ i ].new_position.X[j] = _mesh->node[ i ].position.X[j]
												+ _mesh->deformation.X[ 3 * i + j ];
		}
	}
	releaseMat( &invLdd );
}

double calTotalMisessStress( Mesh *_mesh ){
	unsigned int i,j;
	double max_mises_stress = 0;
	for( i = 0; i < _mesh->num_tetrahedra; i ++ ){
		for( j = 0; j < 4; j ++ ){
			memcpy( &_mesh->tetrahedra[ i ].deformation.X[ 3 * j ],
				&_mesh->deformation.X[ 3 * _mesh->tetrahedra[ i ].node_index[ j ] ],
				sizeof( double ) * 3 );
			memcpy( _mesh->tetrahedra[ i ].new_position[ j ].X,
				_mesh->node[ _mesh->tetrahedra[ i ].node_index[ j ] ].new_position.X,
				sizeof( double ) * 3 );
		}
		//ミーゼス応力の計算
		calMisesStress( &_mesh->tetrahedra[ i ] );
		//値の大小判定
		if( max_mises_stress < _mesh->tetrahedra[ i ].mises_stress ){
			max_mises_stress = _mesh->tetrahedra[ i ].mises_stress;
		}
	}
	return max_mises_stress;
}

void clearDeformation( Mesh *_mesh )
{
	unsigned int i;
	clearVecN( &_mesh->deformation );
	clearVecN( &_mesh->force );
	for( i = 0; i <_mesh->num_node; i ++ ){
		memcpy(	&_mesh->node[ i ].new_position,
			&_mesh->node[ i ].position,
			sizeof( Vec3d ) );
	}
	for( i = 0; i <_mesh->num_tetrahedra; i ++ ){
		clearVecN( &_mesh->tetrahedra[i].deformation );
		memcpy( _mesh->tetrahedra[ i ].new_position,
				_mesh->tetrahedra[ i ].node,
				sizeof( Vec3d ) * 4 );
		_mesh->tetrahedra[ i ].mises_stress=0;
	}
}