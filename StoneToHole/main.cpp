#include "raylib.h"
#include "raymath.h"
#include "rotation.h"

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION            330
#else   // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION            100
#endif

int main(void)
{
	// Initialization
	//--------------------------------------------------------------------------------------
	const int screenWidth = 800;
	const int screenHeight = 450;
	SetConfigFlags(FLAG_MSAA_4X_HINT);  // Enable Multi Sampling Anti Aliasing 4x (if available)
	InitWindow(screenWidth, screenHeight, "Drop Stone To Hole");

	// Define the camera to look into our 3d world
	Camera camera = { 0 };
	camera.position = { 2.0f, 4.0f, 6.0f };    // Camera position
	camera.target = { 0.0f, 0.5f, 0.0f };      // Camera looking at point
	camera.up = { 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
	camera.fovy = 45.0f;                                // Camera field-of-view Y
	camera.projection = CAMERA_PERSPECTIVE;             // Camera mode type
	// Load plane model from a generated mesh
	Model cube = LoadModelFromMesh(GenMeshCube(1.0f, 2.0f, 1.0f));
	// Ambient light level (some basic lighting)
	SetCameraMode(camera, CAMERA_FREE);  // Set an orbital camera mode
	const int fps = 60;
	SetTargetFPS(fps);

	Vector3 center_pos = { 0.5, 1, 0.5 };
	bool managed = false; // idle
	Vector3 rotation_center;
	direction rotate_direction;
	auto current_state = rigid_placement(&center_pos, &(cube.transform));// 当前的刚体位置
	rigid_placement finished_state; // 当前旋转结束的时候的刚体位置
	constexpr float finished_rotate_s = 0.2;
	constexpr float arrive_deviation = 0.01;// 抵达的误差，判断物体是否完成旋转的误差。
	const float step_degree = 90.0 / finished_rotate_s * (1.0 / fps);
	float current_rotation_angle = 0;

	// Main game loop
	while (!WindowShouldClose())            // Detect window close button or ESC key
	{
		if (managed) {
			if (abs(current_rotation_angle - 90.0) < arrive_deviation) {
				// 一眼丁真，鉴定为抵达。
				managed = false;
				center_pos = *finished_state.center_pos;
				cube.transform = *finished_state.transform;
				finished_state.free();
			}
			else
			{
				// 没有抵达，就继续运动。 
				rigid_placement rotation_result = managed_rotate(current_state, rotate_direction, rotation_center, step_degree);
				center_pos = *rotation_result.center_pos;
				cube.transform = *rotation_result.transform;
				current_rotation_angle += step_degree;
			}
		}
		else
		{
			// 如果是idle状态，那么等待用户输入！
			rotate_direction = direction_from_key();
			if (rotate_direction.x == 0 && rotate_direction.z == 0) {
				// 没有任何输入！
			}
			else
			{
				managed = true;
				rotation_center = get_rotation_center(center_pos, rotate_direction);
				finished_state = managed_rotate(current_state, rotate_direction, rotation_center, 90.0);
				current_rotation_angle = 0;
			}
		}
		UpdateCamera(&camera);              // Update camera
		BeginDrawing();
		ClearBackground(RAYWHITE);
		BeginMode3D(camera);
		// DrawModelEx(cube, { 0,0,0 }, { 0,0,0 }, 0, { 1.0,1.0,1.0 }, WHITE);

		DrawModelWires(cube, center_pos, 1, BLACK);
		DrawGrid(20, 1.0f);
		EndMode3D();
		DrawText(TextFormat("%f %f %f", center_pos.x, center_pos.y, center_pos.z), 10, 10, 10, BLACK);
		EndDrawing();
	}
	CloseWindow();
	return 0;
}

