#pragma once
#include "raylib.h"	//
#include "raymath.h"
#include<memory>

struct direction {
	int z;
	int x;
};

struct rigid_placement {
	Vector3* center_pos;
	Matrix* transform;
	bool need_deallocation;
	rigid_placement(Vector3* center_pos, Matrix* transform);
	rigid_placement();
	void free();
};

rigid_placement transform_x(rigid_placement placement, float angle_rad, Vector3 rotation_center);

rigid_placement transform_z(rigid_placement placement, float angle_rad, Vector3 rotation_center);

Vector3 get_rotation_center(Vector3& center_pos, direction dir);

rigid_placement managed_rotate(rigid_placement placement, direction dir, Vector3& rotation_center, float step_degree);

direction direction_from_key();