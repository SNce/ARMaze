#ifndef AR_VRML97_H
#define AR_VRML97_H

int arVrml97LoadFile( char *file );
int arVrml97Free( int id );
int arVrml97Reset( int id );
int arVrml97Draw( int id );
int arVrml97TimerUpdate( void );

int arVrml97ChangeVR( int id );

int arVrml97MoveUserPosMat( double mat[16] );
int arVrml97MoveUserPos( float pos[3] );
int arVrml97RotateUserPos( float rot[4] );
int arVrml97ResetUserPos( void );
int arVrml97SetUserPos( float rot[4], float pos[3] );
int arVrml97GetUserPosMat( int id, double mat[16] );
int arVrml97GetAvatarPosMat( int id, double mat[16] );

int arVrml97InsertAvatar( int id, int avatar_id, double scale, double trans_mat[16] );

int arVrml97ClickButton( int x, int y );

int arVrml97SetFog( int flag );
int arVrml97SetBG( int flag );

#endif
