#include "rotation.h"

rigid_placement::rigid_placement(Vector3* center_pos, Matrix* transform) {
	this->center_pos = center_pos;
	this->transform = transform;
	need_deallocation = false;
}

rigid_placement::rigid_placement() {
	center_pos = new Vector3();
	transform = new Matrix();
	need_deallocation = true;
}

void rigid_placement::free() {
	if (need_deallocation) {
		delete center_pos;
		delete transform;
	}
}

rigid_placement transform_x(rigid_placement placement, float angle_rad, Vector3 rotation_center) {
	rigid_placement result = rigid_placement();
	*result.transform = MatrixMultiply(*(placement.transform), MatrixRotateZ(angle_rad));
	float relative_x = placement.center_pos->x - rotation_center.x;
	float relative_y = placement.center_pos->y - rotation_center.y;
	*result.center_pos = {
		(float)(relative_x * cos(angle_rad) - relative_y * sin(-angle_rad) + rotation_center.x),
		(float)(relative_x * sin(-angle_rad) + relative_y * cos(angle_rad) + rotation_center.y),
		placement.center_pos->z
	};
	return result;
}

rigid_placement transform_z(rigid_placement placement, float angle_rad, Vector3 rotation_center) {
	rigid_placement result = rigid_placement();
	*result.transform = MatrixMultiply(*(placement.transform), MatrixRotateX(-angle_rad));
	float relative_z = placement.center_pos->z - rotation_center.z;
	float relative_y = placement.center_pos->y - rotation_center.y;
	*result.center_pos = {
		placement.center_pos->x,
		(float)(relative_z * sin(-angle_rad) + relative_y * cos(angle_rad) + rotation_center.y),
		(float)(relative_z * cos(angle_rad) - relative_y * sin(-angle_rad) + rotation_center.z)
	};
	return result;
}

Vector3 get_rotation_center(Vector3& center_pos, direction dir) {
	constexpr float deviation = 0.01;
	// 先确定旋转中心
	Vector3 rotation_center_xz_positive = { 0.0,0.0,0.0 };
	Vector3 rotation_center_xz_negative = { 0.0,0.0,0.0 };

	if (abs(center_pos.x - floor(center_pos.x)) < deviation) {
		// 说明柱体一定是沿x轴横趴着
		rotation_center_xz_positive = { center_pos.x + 1, 0, center_pos.z + 0.5f };
		rotation_center_xz_negative = { center_pos.x - 1, 0, center_pos.z - 0.5f };
	}
	else if (abs(center_pos.z - floor(center_pos.z)) < deviation) {
		// 说明柱体一定是沿z轴横趴着
		rotation_center_xz_positive = { center_pos.x + 0.5f, 0, center_pos.z + 1 };
		rotation_center_xz_negative = { center_pos.x - 0.5f, 0, center_pos.z - 1 };
	}
	else {
		// 说明柱体一定立着
		rotation_center_xz_positive = { center_pos.x + 0.5f, 0, center_pos.z + 0.5f };
		rotation_center_xz_negative = { center_pos.x - 0.5f, 0, center_pos.z - 0.5f };
	}

	if (dir.x > 0 || dir.z > 0) {
		return rotation_center_xz_positive;
	}
	else
	{
		return rotation_center_xz_negative;
	}
}

rigid_placement managed_rotate(rigid_placement placement, direction dir, Vector3& rotation_center, float step_degree) {
	if (dir.x > 0) {
		return transform_x(placement, step_degree * DEG2RAD, rotation_center);
	}
	else if (dir.x < 0)
	{
		return transform_x(placement, -step_degree * DEG2RAD, rotation_center);
	}
	else if (dir.z > 0) {
		return transform_z(placement, step_degree * DEG2RAD, rotation_center);
	}
	else
	{
		return transform_z(placement, -step_degree * DEG2RAD, rotation_center);
	}
}

direction direction_from_key() {
	if (IsKeyDown(KEY_UP)) {
		return { -1, 0 };
	}
	else if (IsKeyDown(KEY_DOWN)) {
		return { 1 , 0 };
	}
	else if (IsKeyDown(KEY_LEFT)) {
		return { 0, -1 };
	}
	else if (IsKeyDown(KEY_RIGHT)) {
		return { 0, 1 };
	}
	else
	{
		return { 0, 0 };
	}
}