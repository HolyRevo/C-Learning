#include "raylib.h"
#include "raymath.h"
#include <iostream>

using namespace std;

Color collGreen = Color{ 173, 204, 96, 255 };
Color coldGreen = Color{ 43, 51, 24, 255 };


class Player {
public:
	Vector3 pos = { 5,1,5 };
	float s = 0.1;

	float yaw = 29.0f; // x;
	float pitch = 0.0f; //y;
	float sens = 0.001f;

	Camera3D cam;

	Player() {
		cam.position = pos;
		cam.target = { 0.0f, 0.0f, 0.0f };
		cam.up = { 0.0f, 1.0f, 0.0f };
		cam.fovy = 45.0f;
		cam.projection = CAMERA_PERSPECTIVE;

		DisableCursor();	
	}

	void Update() {
		Vector2 mouseDelta = GetMouseDelta();
		yaw -= mouseDelta.x * sens;
		pitch -= mouseDelta.y * sens;

		Vector3 forward = {
			cosf(pitch) * sinf(yaw),
			sinf(pitch),
			cosf(pitch) * cosf(yaw)
		};

		Vector3 right = {
			sinf(yaw - 1.57f),
			0,
			cosf(yaw - 1.57f)
		};

		if (IsKeyDown(KEY_W)) {
			pos.x += forward.x * s;
			pos.z += forward.z * s;
		}
		if (IsKeyDown(KEY_S)) {
			pos.x -= forward.x * s;
			pos.z -= forward.z * s;
		}
		if (IsKeyDown(KEY_A)) {
			pos.x -= right.x * s;
			pos.z -= right.z * s;
		}
		if (IsKeyDown(KEY_D)) {
			pos.x += right.x * s;
			pos.z += right.z * s;
		}

		cam.position = pos;
		cam.target = Vector3Add(pos, forward);
	}
};

class Objects {
public:
	Vector3 pos;
	Model model;

	void LoadCube(float width, float height, float length) {
		Mesh mesh = GenMeshCube(width, height, length);
		model = LoadModelFromMesh(mesh);
	}
};


int main() {
	InitWindow(1800, 1000, "Revolver");
	SetTargetFPS(60);

	Player plr = Player();
	Objects test = Objects();

	test.pos = Vector3{ 0, 0, 0 };
	test.LoadCube(1.0f, 1.0f, 1.0f);

	while (WindowShouldClose() == false) {
		BeginDrawing();
		ClearBackground(RAYWHITE);

		BeginMode3D(plr.cam);

		DrawGrid(15, 0.5f);

		DrawLine3D(Vector3{ 0, 0, 0 }, Vector3{ 0, 5, 5 }, RED);
		DrawModel(test.model, test.pos, 1, RED);

		plr.Update();

		EndMode3D();
		EndDrawing();
	}

	UnloadModel(test.model);
	CloseWindow();
}
