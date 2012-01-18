#include"physics.h"

void integrator(float time,Maze *maze, Ball *ball)
{
	if(time != 0.0)
	{
		update(maze,ball);
		collision(maze,ball,time);

		ball->accel = ball->inv_mass * ball->forceAccum;
		a3d::vector3<float> a = ball->velocity;
		printf("\n  time = %f",time);
		printf("\n accel = %f, %f, %f",ball->accel.x,ball->accel.y,ball->accel.z);
		printf("\n accel.y * time = %f, %f, %f",ball->accel.x * time,ball->accel.y * time,ball->accel.z*time);
		printf("\n A =	%f, %f, %f",a.x,a.y,a.z);
		printf("\n F = %f, %f, %f",ball->forceAccum.x,ball->forceAccum.y,ball->forceAccum.z);

		ball->position.x = ball->position.x + (ball->velocity.x * time) + (0.5 * ball->accel.x * time * time);
		ball->position.y = ball->position.y + (ball->velocity.y * time) + (0.5 * ball->accel.y * time * time);
		ball->position.z = ball->position.z + (ball->velocity.z * time) + (0.5 * ball->accel.z * time * time);

		ball->velocity.x = ball->velocity.x + (ball->accel.x * time);
		ball->velocity.y = ball->velocity.y + (ball->accel.y * time);
		ball->velocity.z = ball->velocity.z + (ball->accel.z * time);

	//	if(ball->velocity.x <= MIN_PRECISION)// || ball->velocity.x > -MIN_PRECISION)
	//		ball->velocity.x = 0.0;
	//	if(ball->velocity.y <= MIN_PRECISION)// || ball->velocity.y > -MIN_PRECISION)
	//		ball->velocity.y = 0.0;
	//	if(ball->velocity.z <= MIN_PRECISION)// || ball->velocity.z > -MIN_PRECISION)
	//		ball->velocity.z = 0.0;

		//ball->velocity = ball->velocity + (ball->accel * time);
		printf("\n V = %f, %f, %f",ball->velocity.x,ball->velocity.y,ball->velocity.z);
		printf("\n Pos = %f, %f, %f",ball->position.x,ball->position.y,ball->position.z);

		ball->clearForceAccum();
		ball->clearAccel();
	}
}

