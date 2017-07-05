#include "GLTool.h"

void calPseudoColor(double _value, Vec3d *_color)
{
	int Hi;
	double H, f;
	if( _value > 0.7 ){
		_value = 0.7;
	}
	H = 6 * ( 0.7 - _value );
	Hi = ( int )floor( H ) % 6;
	f = H - ( double ) Hi;
	switch( Hi ){
		case 0:
			_color->x = 1; _color->y = f; _color->z = 0;
			break;
		case 1:
			_color->x = 1 - f; _color->y = 1; _color->z = 0;
			break;
		case 2:
			_color->x = 0; _color->y = 1; _color->z = f;
			break;
		case 3:
			_color->x = 0; _color->y = 1 - f; _color->z = 1;
			break;
		case 4:
			_color->x = f; _color->y = 0; _color->z = 1;
			break;
		case 5:
			_color->x = 1; _color->y = 0; _color->z = 1 - f;
			break;
	}
}

void renderSTLMesh( Mesh *_mesh )
{
	unsigned int i,j;
	//??????
	glPointSize(5);
	glBegin( GL_POINTS );
	for( i = 0; i < _mesh->num_node; i ++ ){
		//??????????????
		switch( _mesh->node[i].state ){
			case NODE_FREE:
				glColor3d( 0, 1, 1 );
				break;
			case NODE_FIXED:
				glColor3d( 1, 0, 1 );
				break;
			case NODE_LOADED:
				glColor3d( 1, 1, 0 );
				break;
		}
		//????????
		glVertex3dv( _mesh->node[ i ].position.X );
	}
	glEnd();

	//???????
	glColor3d( 0, 0, 1 );
	for( i = 0; i < _mesh->num_facet; i ++ ){
		glNormal3dv( _mesh->facet[ i ].normal.X );
		glBegin( GL_POLYGON );
		for( j = 0; j < 3; j ++ ){
			glVertex3dv( _mesh->facet[ i ].position[ j ].X );
		}
		glEnd();
	}
}

void renderFEMMesh( Mesh *_mesh, double _max_mises_stress )
{
	Vec3d color;
	unsigned int i,j,k;
	//??????
	glPointSize(10);
	glBegin( GL_POINTS );
	for( i = 0; i < _mesh->num_node; i ++ ){
		//??????????????
		switch( _mesh->node[i].state ){
			case NODE_FREE:
				glColor3d( 0, 1, 1 );
				break;
			case NODE_FIXED:
				glColor3d( 1, 0, 1 );
				break;
			case NODE_LOADED:
				glColor3d( 1, 1, 0 );
				break;
		}
		//????????
		glVertex3dv( _mesh->node[ i ].new_position.X );
	}
	glEnd();

	//??????????
	glLineWidth( 1 );
	glColor3d( 0, 0, 0 );
	for( i = 0; i < _mesh->num_tetrahedra; i ++ ){
		for( j = 0; j < 4; j ++ ){
			for( k = 0; k < j; k ++ ){
				glBegin( GL_LINE_STRIP );
				glVertex3dv( _mesh->tetrahedra[ i ].new_position[ j ].X );
				glVertex3dv( _mesh->tetrahedra[ i ].new_position[ k ].X );
				glEnd();
			}
		}
	}

	//?????????
	glColor3d( 0, 0, 1 );
	for( i = 0; i < _mesh->num_tetrahedra; i ++ ){
		calPseudoColor( _mesh->tetrahedra[ i ].mises_stress / _max_mises_stress, &color );
		for( j = 0; j < 4; j ++ ){
			glColor3dv( color.X );
			glBegin( GL_POLYGON );
			glVertex3dv( _mesh->tetrahedra[ i ].new_position[ ( j + 0 ) % 4 ].X);
			glVertex3dv( _mesh->tetrahedra[ i ].new_position[ ( j + 1 ) % 4 ].X);
			glVertex3dv( _mesh->tetrahedra[ i ].new_position[ ( j + 2 ) % 4 ].X);
			glEnd();
		}
	}
}

float getDepth( int _pos_window_x, int _pos_window_y )
{
	float depth;
	GLint viewport[ 4 ];
	glGetIntegerv( GL_VIEWPORT, viewport );
	//??????????
	glReadPixels( _pos_window_x, viewport[3]-_pos_window_y, 1, 1,
				GL_DEPTH_COMPONENT, GL_FLOAT, &depth );
	return depth;
}

void convertWorld2Window( Vec3d *_position_world, Vec3d *_position_window )
{
    GLdouble matrix_modelview[ 16 ];
	GLdouble matrix_projection[ 16 ];
	GLint viewport[ 4 ];
	glGetIntegerv( GL_VIEWPORT,viewport );
    glGetDoublev( GL_MODELVIEW_MATRIX, matrix_modelview );
    glGetDoublev( GL_PROJECTION_MATRIX, matrix_projection );
	//????????????????????
    gluProject( _position_world->x, _position_world->y, _position_world->z,
				matrix_modelview, matrix_projection, viewport,
				&_position_window->x, &_position_window->y, &_position_window->z );
}

void convertWindow2World( Vec3d *_position_window, Vec3d *_position_world)
{
    GLdouble matrix_modelview[ 16 ];
	GLdouble matrix_projection[ 16 ];
	GLint viewport[ 4 ];
    glGetIntegerv( GL_VIEWPORT, viewport );
    glGetDoublev( GL_MODELVIEW_MATRIX, matrix_modelview );
    glGetDoublev( GL_PROJECTION_MATRIX, matrix_projection );
	//????????????????????
	gluUnProject( _position_window->x, ( double )viewport[ 3 ]-_position_window->y, _position_window->z,
				matrix_modelview, matrix_projection, viewport,
				&_position_world->x, &_position_world->y, &_position_world->z );
}

void glInit( void )
{
	glClearColor( 0.0, 0.0, 0.0, 1.0 );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_LINE_SMOOTH );
	glShadeModel( GL_SMOOTH );
}

void setCamera( int _width, int _height )
{
	glViewport( 0, 0, _width, _height );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 30.0, (double)_width / (double)_height, 0.1, 1000 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 80.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void setLight( Lightf *_light )
{
	glLightfv( GL_LIGHT0+_light->id, GL_POSITION, _light->position );
	glLightfv( GL_LIGHT0+_light->id, GL_AMBIENT, _light->ambient );
	glLightfv( GL_LIGHT0+_light->id, GL_DIFFUSE, _light->diffuse );
	glLightfv( GL_LIGHT0+_light->id, GL_SPECULAR, _light->specular );
}

void setMaterial( Materialf *_material )
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, _material->ambient );
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, _material->diffuse );
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, _material->specular );
}

void setVec4f( float *_vec, float _x, float _y, float _z, float _w)
{
	_vec[ 0 ] = _x;
	_vec[ 1 ] = _y;
	_vec[ 2 ] = _z;
	_vec[ 3 ] = _w;
}

void setMouseRotation( double _x, double _y, Matd *_dst )
{
	Matd matrix_rot_x;
	Matd matrix_rot_y;
	Matd matrix_temp;
	initMat( &matrix_rot_x );
	initMat( &matrix_rot_y );
	initMat( &matrix_temp );
	setRotationalMatrix( _x, ROT_AXIS_Y,  &matrix_rot_y);
	setRotationalMatrix( _y, ROT_AXIS_X,  &matrix_rot_x );
	multiMatandMat( &matrix_rot_y, _dst, &matrix_temp );
	multiMatandMat( &matrix_rot_x, &matrix_temp, _dst );
	releaseMat( &matrix_rot_x );
	releaseMat( &matrix_rot_y );
	releaseMat( &matrix_temp );
}

