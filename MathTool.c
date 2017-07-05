#include "MathTool.h"

void initVecN( VecNd *_A )
{
	_A->X = NULL;
	_A->dim = 0;
}

void clearVec3( Vec3d *_A )
{
	memset( _A->X, 0, sizeof( double ) * 3 );
}

void clearVecN( VecNd *_A )
{
	memset( _A->X, 0, sizeof( double ) * _A->dim );
}

void setVecNDim( VecNd *_A, unsigned int _dim )
{
	if( _A->X != NULL ){//領域が確保されている場合
		if( _A->dim == _dim){//確保するサイズに変更がない場合
			return;
		}
		free( _A->X );
	}
	if( _dim > 0 ){
		//void*からdouble*へのキャスト
		_A->X = ( double* )calloc( _dim, sizeof( double ) );
		if( _A->X != NULL ){//メモリ確保がうまくいった
			_A->dim = _dim;
		}
		else{//メモリ確保に失敗した
			_A->dim = 0;
		}
	}
}

void releaseVecN( VecNd *_A )
{
	if(_A->X != NULL){
		free( _A->X );
	}
}

void sumVec3andVec3( Vec3d *_A, Vec3d *_B, Vec3d *_dst )
{
	unsigned int i;
	for( i = 0; i < 3; i++ ){
		_dst->X[ i ] = _A->X[ i ] + _B->X[ i ];
	}
}

void sumVecNandVecN( VecNd *_A, VecNd *_B, VecNd *_dst )
{
	unsigned int i;
	if( _A->dim == _B->dim ){
		if(_dst->dim != _A->dim){
			initVecN( _dst );
			setVecNDim( _dst, _A->dim );
		}
		for( i = 0; i < _A->dim; i++ ){
			_dst->X[ i ] = _A->X[ i ] + _B->X[ i ];
		}
	}
}

void subVec3andVec3( Vec3d *_A, Vec3d *_B, Vec3d *_dst )
{
	unsigned int i;
	for( i = 0; i < 3; i++ ){
		_dst->X[ i ] = _A->X[ i ] - _B->X[ i ];
	}
}

void subVecNandVecN( VecNd *_A, VecNd *_B, VecNd *_dst )
{
	unsigned int i;
	if( _A->dim == _B->dim ){
		if(_dst->dim != _A->dim){
			initVecN( _dst );
			setVecNDim( _dst, _A->dim );
		}
		for( i = 0; i < _A->dim; i++ ){
			_dst->X[ i ] = _A->X[ i ] - _B->X[ i ];
		}
	}
}

void scalingVec3( double _s, Vec3d *_A, Vec3d *_dst )
{
	unsigned int i;
	for( i = 0; i < 3; i++ ){
		_dst->X[ i ] = _s * _A->X[i];
	}
}

void scalingVecN( double _s, VecNd *_A, VecNd *_dst )
{
	unsigned int i;
	if( _A->dim != _dst->dim ){
		initVecN( _dst );
		setVecNDim( _dst, _A->dim );
	}
	for( i = 0; i < _A->dim; i++ ){
		_dst->X[ i ] = _s * _A->X[ i ];
	}
}

double dotVec3andVec3( Vec3d *_A, Vec3d *_B )
{
	unsigned int i;
	double dst = 0;
	for( i = 0; i < 3; i++ ){
		dst += _A->X[ i ] * _B->X[ i ];
	}
	return dst;
}

double dotVecNandVecN( VecNd *_A, VecNd *_B )
{
	unsigned int i;
	double dst = 0;
	if( _A->dim == _B->dim ){
		for( i = 0; i < _A->dim; i++ ){
			dst += _A->X[ i ] * _B->X[ i ];
		}
	}
	return dst;
}

void crossVec3andVec3(Vec3d *_A, Vec3d *_B, Vec3d *_dst)
{
	_dst->x = _A->y * _B->z - _A->z * _B->y;
	_dst->y = _A->z * _B->x - _A->x * _B->z;
	_dst->z = _A->x * _B->y - _A->y * _B->x;
}

double absVec3( Vec3d *_A )
{
	return sqrt( dotVec3andVec3( _A, _A ) );
}

double absVecN( VecNd *_A )
{
	return sqrt( dotVecNandVecN( _A, _A ) );
}

void printVec3( Vec3d *_A )
{
	unsigned int i;
	for( i = 0; i < 3; i++ ){
		printf( "%2.2f", _A->X[ i ] );
		if( i < 2){
			printf( " ");
		}
		else{
			printf( "\n");
		}
	}
}

void printVecN( VecNd *_A )
{
	unsigned int i;
	for( i = 0; i < _A->dim; i++ ){
		printf( "%2.2f", _A->X[ i ] );
		if( i < _A->dim - 1){
			printf( " ");
		}
		else{
			printf( "\n");
		}
	}
}

void initMat( Matd *_A )
{
	_A->X = NULL;
	_A->ncol = 0;
	_A->nrow = 0;
	_A->size = 0;
}

void clearMat( Matd *_A )
{
	memset( _A->X, 0, sizeof( double ) * _A->size );
}

void identityMat( Matd *_A )
{
	unsigned int i;
	if( _A->ncol == _A->nrow ){
		clearMat( _A );
		for( i = 0; i < _A->ncol; i ++ ){
			_A->X[ _A->ncol * i +i ]=1;
		}
	}
}

void setMatDim( Matd *_A, unsigned int _ncol, unsigned int _nrow )
{
	if( _A->X != NULL ){//領域が確保されている場合
		if( _A->ncol == _ncol && _A->nrow == _nrow ){//確保するサイズに変更がない場合
			return;
		}
		free( _A->X );
	}
	if( _ncol > 0 && _nrow > 0 ){
		_A->X = ( double* )calloc( _ncol * _nrow , sizeof( double ) );
		if( _A->X != NULL ){//メモリ確保がうまくいった
			_A->ncol = _ncol;
			_A->nrow = _nrow;
			_A->size = _ncol * _nrow;
		}
		else{//メモリ確保に失敗した
			_A->ncol = 0;
			_A->nrow = 0;
			_A->size = 0;
		}
	}
}

void releaseMat( Matd *_A )
{
	if(_A->X != NULL){
		free( _A->X );
	}
}

void sumMatandMat( Matd *_A, Matd *_B, Matd *_dst )
{
	unsigned int i,j;
	//AとBの次元が同じときだけ定義されている
	if( _A->ncol == _B->ncol && _A->nrow == _B->nrow ){
		if( _dst->ncol != _A->ncol || _dst->nrow != _A->nrow ){
			initMat( _dst );
			setMatDim( _dst, _A->ncol, _A->nrow );
		}
		for( i = 0; i < _A->nrow; i ++ ){
			for( j = 0; j < _A->ncol; j ++ ){
				_dst->X[ _A->ncol * i + j ] = _A->X[ _A->ncol * i + j ]
												+ _B->X[ _A->ncol * i + j ];
			}
		}
	}
}

void subMatandMat( Matd *_A, Matd *_B, Matd *_dst )
{
	unsigned int i,j;
	//AとBの次元が同じときだけ定義されている
	if( _A->ncol == _B->ncol && _A->nrow == _B->nrow ){
		if( _dst->ncol != _A->ncol || _dst->nrow != _A->nrow ){
			initMat( _dst );
			setMatDim( _dst, _A->ncol, _A->nrow );
		}
		for( i = 0; i < _A->nrow; i ++ ){
			for( j = 0; j < _A->ncol; j ++ ){
				_dst->X[ _A->ncol * i + j ] = _A->X[ _A->ncol * i + j ]
												- _B->X[ _A->ncol * i + j ];
			}
		}
	}
}

void multiMatandMat( Matd *_A, Matd *_B, Matd *_dst )
{
	unsigned int i,j,k;
	//Aの列とBの行の数が同じときだけ定義されている
	if( _A->ncol == _B->nrow ){
		if( _dst->ncol != _B->ncol || _dst->nrow != _A->nrow ){
			initMat( _dst );
			setMatDim( _dst, _B->ncol, _A->nrow );
		}
		clearMat( _dst );
		for( i = 0; i < _A->nrow; i ++ ){
			for( j = 0; j < _B->ncol; j ++ ){
				for( k = 0; k < _A->ncol; k ++ ){
					_dst->X[ _B->ncol * i + j ] += _A->X[ _A->ncol * i + k ]
													* _B->X[ _B->ncol * k + j ];
				}
			}
		}
	}
}
void multiMatandVec3( Matd *_A, Vec3d *_B, VecNd *_dst )
{
	unsigned int i, j;
	//Aの列数とBの次元が同じときだけ定義されている
	if( _A->ncol == 3 ){
		if( _dst->dim != _A->nrow){
			initVecN( _dst );
			setVecNDim ( _dst, _A->nrow );
		}
		clearVecN( _dst );
		for( i = 0; i < _A->nrow; i ++ ){
			for( j = 0; j < _A->ncol; j ++ ){
				_dst->X[ i ] += _A->X[ _A->ncol * i + j ] * _B->X[ j ];
			}
		}
	}
}
void multiTransferMatandVec3( Matd *_A, Vec3d *_B, Vec3d *_dst )
{
	double src[ 4 ];
	double dst[ 4 ];
	unsigned int i, j;
	if( _A->ncol == 4 && _A->nrow == 4){
		//同次座標系に変換
		memcpy( src, _B->X, sizeof( double ) * 3 );
		src[ 3 ] = 1;
		memset( dst, 0, sizeof( double ) * 4 );
		for( i = 0; i < _A->nrow; i ++ ){
			for( j = 0; j < _A->ncol; j ++ ){
				dst[ i ] += _A->X[ _A->ncol * i + j ] * src[  j  ];
			}
		}
		//第4成分で割る
		dst[ 0 ] /= dst[ 3 ];
		dst[ 1 ] /= dst[ 3 ];
		dst[ 2 ] /= dst[ 3 ];
		memcpy( _dst->X, dst, sizeof( double ) * 3 );
	}
}
void multiMatandVecN( Matd *_A, VecNd *_B, VecNd *_dst )
{
	unsigned int i, j;
	//Aの列数とBの次元が同じときだけ定義されている
	if( _A->ncol == _B->dim ){
		if( _dst->dim != _A->nrow){
			initVecN( _dst );
			setVecNDim ( _dst, _A->nrow );
		}
		clearVecN( _dst );
		for( i = 0; i < _A->nrow; i ++ ){
			for( j = 0; j < _A->ncol; j ++ ){
				_dst->X[ i ] += _A->X[ _A->ncol * i + j ] * _B->X[ j ];
			}
		}
	}
}

void scalingMat( double _s, Matd *_A, Matd *_dst )
{
	unsigned int i;
	if( _dst->ncol != _A->ncol || _dst->nrow != _A->nrow ){
		initMat( _dst );
		setMatDim( _dst, _A->ncol, _A->nrow );
	}
	for( i = 0; i < _A->size; i ++ ){
		_dst->X[ i ] = _s * _A->X[ i ];
	}
}

double detMat( Matd *_A )
{
	int *ip;
	double dst = 0;
	ip = ( int* )calloc( _A->ncol, sizeof( int ) );
	dst = lu( _A->ncol, _A->X, ip );
	free(ip);
	return dst;
}

void trMat( Matd *_A, Matd *_dst )
{
	unsigned int i,j;
	if( _dst->ncol != _A->nrow || _dst->nrow != _A->ncol ){
		initMat( _dst );
		setMatDim( _dst, _A->nrow, _A->ncol );
	}
	for( i = 0; i < _A->nrow; i ++ ){
		for( j = 0; j < _A->ncol; j ++ ){
			_dst->X[ _dst->ncol * j + i ] = _A->X[ _A->ncol * i + j ];
		}
	}
}

void invMat( Matd *_A, Matd *_dst )
{
	double *X;
	if( _A->ncol == _A->nrow ){
		if( _dst->ncol != _A->nrow || _dst->nrow != _A->ncol ){
			initMat( _dst );
			setMatDim( _dst, _A->nrow, _A->ncol );
		}
		X = ( double* )malloc( sizeof( double ) * _A->size );
		memcpy( X, _A->X, sizeof( double ) * _A->size );
		//LU分解による逆行列計算
		invMatbyLU( _dst->X, X, _A->ncol );
		free(X);
	}
}

double lu( int n, double *a, int *ip )
{
	int i, j, k, ii, ik;
	double t, u, det;
	double *weight;
	weight = ( double* )malloc( sizeof ( double ) * n );

	det = 0;
	for( k = 0; k < n; k ++){
		ip[ k ] = k;
		u = 0;
		for( j = 0; j < n; j ++ ) {
			t = fabs( a[ n * k + j ] );
			if(t > u) u = t;
		}
		if(u == 0){
			return 0;
		}
		weight[ k ] = 1 / u;
	}
	det = 1;
	for( k = 0; k < n; k ++ ){
		u = -1;
		for( i = k; i < n; i++ ){
			ii = ip[ i ];
			t = fabs( a[ n * ii + k ] ) * weight[ ii ];
			if( t > u ){
				u = t;
				j = i;
			}
		}
		ik = ip[ j ];
		if ( j != k ) {
			ip[ j ] = ip[ k ];
			ip[ k ] = ik;
			det = - det;
		}
		u = a[ n * ik + k ];
		det *= u;
		if(u == 0){
			return 0;
		}
		for( i = k + 1; i < n; i ++ ) {
			ii = ip[ i ];
			t = ( a[ n * ii + k ] /= u );
			for ( j = k + 1; j < n; j ++ ){
				a[ n * ii + j ] -= t * a[ n * ik + j ];
			}
		}
	}
	free( weight );
	return det;
}

double invMatbyLU( double *a_inv, double *a, int n )
{
	int i, j, k, ii;
	double t, det;
	int *ip;
	ip = ( int* )calloc( n, sizeof( int ) );
	det = lu( n, a, ip );
	if( det != 0 ){
		for( k = 0; k < n; k ++ ){
			for( i = 0; i < n; i ++){
				ii = ip[ i ];
				t = ( ii == k );
				for( j = 0; j < i; j ++ ){
					t -= a[ n * ii + j ] * a_inv[ n * j + k ];
				}
				a_inv[ n * i + k ] = t;
			}
			for( i = n - 1; i >= 0; i -- ){
				t = a_inv[ n * i + k ];
				ii = ip[i];
				for ( j = i + 1; j < n; j ++ ){
					t -= a[ n * ii + j ] * a_inv[ n * j + k ];
				}
				a_inv[ n * i + k ] = t / a[ n * ii + i ];
			}
		}
	}
	free( ip );
	return det;
}

void setRotationalMatrix( double _angle, int _axis,  Matd *_dst )
{
	if(_dst->ncol!=4||_dst->nrow!=4){
		setMatDim( _dst, 4, 4 );
	}
	identityMat( _dst );
	_dst->X[ 4*((0+_axis) % 3)+((0+_axis) % 3) ]=cos( M_PI * _angle / 180.0 );
	_dst->X[ 4*((0+_axis) % 3)+((1+_axis) % 3) ]=-sin( M_PI * _angle / 180.0 );
	_dst->X[ 4*((1+_axis) % 3)+((0+_axis) % 3) ]=sin( M_PI * _angle / 180.0 );
	_dst->X[ 4*((1+_axis) % 3)+((1+_axis) % 3) ]=cos( M_PI * _angle / 180.0 );
}

void printMat( Matd *_A )
{
	unsigned int i,j;
	for( i = 0; i < _A->nrow; i++ ){
		for( j = 0; j < _A->ncol; j++ ){
			printf( "%2.2f", _A->X[ _A->ncol * i + j ] );
			if( j < _A->ncol - 1){
				printf( " ");
			}
			else{
				printf( "\n");
			}
		}
	}
}