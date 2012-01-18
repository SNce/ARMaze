#include "collision.h"
#include "vector.h"
#include "object.h"

void update(Maze *maze, Ball *ball)
{
	for(int i = 0;i < maze->nWalls;i++)
	{
		maze->wall[i]->prev_k = maze->wall[i]->k;
		maze->wall[i]->k = dot(maze->wall[i]->normal1, ball->position) - maze->wall[i]->offset;
	}	
}

int collision(Maze *maze,Ball *ball,float time)
{
	float k;		// decision parameter
	float dv;		// differential change in velocity
	a3d::vector3<float> coll_vel;
	a3d::vector3<float> f_normal;	// normal force
	a3d::vector3<float> f_impulse;	// impulse 
	a3d::vector3<float> f_friction;	// friction
	float impulse_val;

	float vel_component;
	float force_component;

	for(int i = 0;i < maze->nWalls;i++)
	{
		k = maze->wall[i]->k - ball->radius;
		if(k <= 0)// || (maze->wall[i]->k * maze->wall[i]->prev_k) > 0)		// collision detected.
		{
			printf("\n\n\n\n\n Collision Detected					[ %d  ]!",maze->wall[i]->id);
			printf("\n Pos = %f %f %f",ball->position.x,ball->position.y,ball->position.z);
		
		
			vel_component = dot(ball->velocity, maze->wall[i]->normal1);
			printf("\nvelocity component [ %d ] = %f",maze->wall[i]->id,vel_component);
			
			if(vel_component < 0)
			{
				vel_component = - vel_component;

				//if(velocity_component < 0.001)
				//	vel_component = 0.0;

				printf("\nvelocity_component = %f",vel_component);

				dv = (vel_component * DAMPING) + vel_component;

				//impulse_val = (dv * ball->mass)/time;
				//f_impulse.x = maze->wall[i]->normal1.x * impulse_val;
				//f_impulse.y = maze->wall[i]->normal1.y * impulse_val;
				//f_impulse.z = maze->wall[i]->normal1.z * impulse_val;

				//ball->addForce(f_impulse);
		
				//printf("\n\n Impulse = %f %f %f",f_impulse.x,f_impulse.y,f_impulse.z);
				//printf("\n Force Accum = %f %f %f",ball->forceAccum.x,ball->forceAccum.y,ball->forceAccum.z);
	
				coll_vel.x = dv * maze->wall[i]->normal1.x;
				coll_vel.y = dv * maze->wall[i]->normal1.y;
				coll_vel.z = dv * maze->wall[i]->normal1.z;

				printf("\ncollision_velocity = %f %f %f",coll_vel.x,coll_vel.y,coll_vel.z);
				ball->velocity.x = ball->velocity.x + coll_vel.x;
				ball->velocity.y = ball->velocity.y + coll_vel.y;
				ball->velocity.z = ball->velocity.z + coll_vel.z;
			}

		force_component = dot(maze->wall[i]->normal1, ball->forceAccum);
		printf("\nForce Accum = %f %f %f",ball->forceAccum.x,ball->forceAccum.y,ball->forceAccum.z);
		
			if(force_component < 0 && vel_component == 0)
			{
				force_component = -force_component;
				printf("\n force component = %f",force_component);
			
				f_normal.x = maze->wall[i]->normal1.x * force_component;
				f_normal.y = maze->wall[i]->normal1.y * force_component;
				f_normal.z = maze->wall[i]->normal1.z * force_component;
	
				ball->addForce(f_normal);
				
				getOppParallelVect(ball->velocity,maze->wall[i]->normal1,f_friction);

				f_friction.x = f_friction.x * force_component * NEU;
				f_friction.y = f_friction.y * force_component * NEU;
				f_friction.z = f_friction.z * force_component * NEU;

				ball->addForce(f_friction);
					

				printf("\nBalls velocity = %f %f %f",ball->velocity.x,ball->velocity.y,ball->velocity.z);
				printf("\nNormal Force = %f %f %f\n\n\n",f_normal.x,f_normal.y,f_normal.z);
			}
		}
	}
	return 1;
}

void getOppParallelVect(a3d::vector3<float> vect, a3d::vector3<float> normal, a3d::vector3<float> para)
{
		if(normal.x == 0.0)
			para.x = 1.0;
		else 
			para.x = 0.0;

		if(normal.y == 0.0)
			para.y = 1.0;
		else 
			para.y = 0.0;

		if(normal.z == 0.0)
			para.z = 1.0;
		else 
			para.z = 0.0;

		para.x = para.x * (-vect.x);
		para.y = para.y * (-vect.y);
		para.z = para.z * (-vect.z);

		para.normalize();
}	