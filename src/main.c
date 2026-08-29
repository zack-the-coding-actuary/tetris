#include "raylib.h"
#include <stdio.h>

typedef enum {
    NONE,
    BLUE_T,
    CYAN_Z,
    GREEN_L,
    PINK_J,
    PURPLE_I,
    RED_O,
    YELLOW_S
} Tetromino;

// Configs
const int screenWidth = 720;
const int screenHeight = 800;
const int fps = 30;

const int block_size = 32; // do not change unless changing sprites

const int left_box = 60;
const int right_box = 380;
const int top_box = 60;
const int bot_box = 700;

const char* title = "TETRIS";
char score_label[32];

// Game state
unsigned score = 0;
Texture2D blocks[7][5]; // 7 blocks with 4 rotations + single unit
int rotation = 0; // 0-3
bool get_next = true;

// For speed of block descent
int frame_count = 0;
const int move_frames = 20;

// Game area is 20 deep, 10 across. Bottom is i = 0.
Tetromino spaces[20][10] = {0};
Texture2D curr;
Tetromino next = 0;
const Vector2 spawn_loc = {(right_box + left_box) / 2 - block_size, top_box - block_size};
Vector2 curr_loc;

// Forward declarations
void MakeGUI();
void InitBlockTextures();
void PickNextBlock();
void CheckSpawnNextBlock();
void DrawCurrent();
void KeyPressCheck();
void ConsiderMove();

int main(void) {
    InitWindow(screenWidth, screenHeight, "Tetris");
    SetTargetFPS(fps);
    InitBlockTextures();
    PickNextBlock();

    while (!WindowShouldClose()) {

        CheckSpawnNextBlock();
        KeyPressCheck();

        BeginDrawing();

            ClearBackground(BLACK);
            MakeGUI();
            DrawCurrent();

        EndDrawing();

        ConsiderMove();
        frame_count++;
    }

    CloseWindow();
    return 0;
}

void PickNextBlock() {
    next = GetRandomValue(0,6);
}

void CheckSpawnNextBlock() {
    if (get_next) {
        rotation = 0;
        curr = blocks[next][rotation];
        curr_loc = spawn_loc;
        PickNextBlock();
        get_next = false;
    }
}

void DrawCurrent() {
    curr = blocks[next][rotation];
    DrawTexture(curr, curr_loc.x, curr_loc.y, WHITE);
}

void KeyPressCheck() {
    // TODO: Add collision logic
    if (IsKeyPressed(KEY_LEFT)) {
        curr_loc.x -= 32;
    }
    if (IsKeyPressed(KEY_RIGHT)) {
        curr_loc.x += 32;
    }
    if (IsKeyPressed(KEY_DOWN)) {
        curr_loc.y += 32;
        frame_count = 0;
    }
    if (IsKeyPressed(KEY_UP)) {
        rotation = (rotation + 1) % 4;
    }
}

void ConsiderMove() {

    // TODO: Add collision logic
    if (frame_count == move_frames) {
        curr_loc.y += block_size;
        frame_count = 0;
    }
}

void MakeGUI() {
    // Box for gameplay area
    DrawLine(left_box,  top_box, left_box,  bot_box, LIGHTGRAY); // left
    DrawLine(right_box, top_box, right_box, bot_box, LIGHTGRAY); // right
    DrawLine(left_box,  bot_box, right_box, bot_box, LIGHTGRAY); // bottom

    // Title and score, with some magic numbers
    DrawText(title, 420, 60, 30, RAYWHITE);
    snprintf(score_label, 32, "Lines cleared: %u", score);
    DrawText(score_label, 420, 120, 20, RAYWHITE);

    // Next block indicator
    DrawText("Next block:", 420, 320, 20, RAYWHITE);
    DrawTexture(blocks[next][0], 420, 400, WHITE);
}

void InitBlockTextures() {

    // Blue T
    blocks[0][0] = LoadTexture("blocks/blue_block/blue_T1.png");
    blocks[0][1] = LoadTexture("blocks/blue_block/blue_T2.png");
    blocks[0][2] = LoadTexture("blocks/blue_block/blue_T3.png");
    blocks[0][3] = LoadTexture("blocks/blue_block/blue_T4.png");
    blocks[0][4] = LoadTexture("blocks/blue_block/blue_block.png");

    // Cyan Z
    blocks[1][0] = LoadTexture("blocks/cyan_block/cyan_Z1.png");
    blocks[1][1] = LoadTexture("blocks/cyan_block/cyan_Z2.png");
    blocks[1][2] = LoadTexture("blocks/cyan_block/cyan_Z3.png");
    blocks[1][3] = LoadTexture("blocks/cyan_block/cyan_Z4.png");
    blocks[1][4] = LoadTexture("blocks/cyan_block/cyan_block.png");

    // Green L
    blocks[2][0] = LoadTexture("blocks/green_block/green_l1.png");
    blocks[2][1] = LoadTexture("blocks/green_block/green_l2.png");
    blocks[2][2] = LoadTexture("blocks/green_block/green_l3.png");
    blocks[2][3] = LoadTexture("blocks/green_block/green_l4.png");
    blocks[2][4] = LoadTexture("blocks/green_block/green_block.png");

    // Pink J
    blocks[3][0] = LoadTexture("blocks/pink_block/pink_J1.png");
    blocks[3][1] = LoadTexture("blocks/pink_block/pink_J2.png");
    blocks[3][2] = LoadTexture("blocks/pink_block/pink_J3.png");
    blocks[3][3] = LoadTexture("blocks/pink_block/pink_J4.png");
    blocks[3][4] = LoadTexture("blocks/pink_block/pink_block.png");

    // Purple I
    blocks[4][0] = LoadTexture("blocks/purple_block/purple_I1.png");
    blocks[4][1] = LoadTexture("blocks/purple_block/purple_I2.png");
    blocks[4][2] = LoadTexture("blocks/purple_block/purple_I3.png");
    blocks[4][3] = LoadTexture("blocks/purple_block/purple_I4.png");
    blocks[4][4] = LoadTexture("blocks/purple_block/purple_block.png");

    // Red O
    blocks[5][0] = LoadTexture("blocks/red_block/red_O1.png");
    blocks[5][1] = LoadTexture("blocks/red_block/red_O2.png");
    blocks[5][2] = LoadTexture("blocks/red_block/red_O3.png");
    blocks[5][3] = LoadTexture("blocks/red_block/red_O4.png");
    blocks[5][4] = LoadTexture("blocks/red_block/red_block.png");

    // Yellow S
    blocks[6][0] = LoadTexture("blocks/yellow_block/yellow_S1.png");
    blocks[6][1] = LoadTexture("blocks/yellow_block/yellow_S2.png");
    blocks[6][2] = LoadTexture("blocks/yellow_block/yellow_S3.png");
    blocks[6][3] = LoadTexture("blocks/yellow_block/yellow_S4.png");
    blocks[6][4] = LoadTexture("blocks/yellow_block/yellow_block.png");
}