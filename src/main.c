#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <string.h>

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

typedef enum {
    ROTATE,
    LEFT,
    RIGHT,
    DOWN
} Move;

typedef struct {
    Tetromino  type; // need to add one to get back to proper enum
    Texture2D *texture;
} Block_Data;

// Where 0, 0 is top-left corner, 4 units per block type, 4 configs per block
typedef Vector2 Orientation[4][4];

// Configs
const int screenWidth = 720;
const int screenHeight = 800;
const int fps = 30;

const int block_size = 32; // do not change unless changing sprites

const Vector2 vecL = (Vector2){-block_size, 0};
const Vector2 vecR = (Vector2){block_size, 0};
const Vector2 vecD = (Vector2){0, block_size};

const int left_box = 60;
const int right_box = 380;
const int top_box = 60;
const int bot_box = 700;

const char* title = "TETRIS";
char score_label[32];
// char debug_label[48];

// Game state
unsigned score = 0;
Texture2D blocks[7][5]; // 7 blocks with 4 rotations + single unit
Orientation block_locs[7]; // 7 blocks, 4 orientations, 2 x/y coords
int rotation = 0; // 0-3
bool get_next = true;
bool game_over = false;

// For speed of block descent
int frame_count = 0;
const int move_frames = 20;

// Game area is 20 deep, 10 across. Bottom is i = 0.
Tetromino spaces[25][10] = {0};
Block_Data curr;
Vector2 curr_loc;
int coords[4][2];
Tetromino next = 0;
const Vector2 spawn_loc = {(right_box + left_box) / 2 - block_size, top_box - block_size};

// Sounds and music
Music music;
Sound land_piece;
Sound clear_line;
Sound game_over_sfx;

// Forward declarations
void MakeGUI();
void InitBlockTextures();
void UnloadBlockTextures();
void LoadSounds();
void UnloadSounds();
void InitBlockLocs();
void PickNextBlock();
void CheckSpawnNextBlock();
void DrawCurrent();
void DrawExisting();
void CheckDestroyRows();
void KeyPressCheck();
void ConsiderMove();
bool CheckCollision(Vector2 proposed, int new_rotation);
void GameOver();
void PixelsToCoords(Vector2 current, Vector2 *unit_offsets);

int main(void) {
    InitWindow(screenWidth, screenHeight, "Tetris");
    SetTargetFPS(fps);
    InitAudioDevice();
    InitBlockTextures();
    LoadSounds();
    InitBlockLocs();
    PickNextBlock();
    PlayMusicStream(music);

    while (!WindowShouldClose()) {

        UpdateMusicStream(music);
        CheckSpawnNextBlock();
        KeyPressCheck();

        BeginDrawing();

            ClearBackground(BLACK);
            MakeGUI();
            DrawCurrent();
            DrawExisting();

        EndDrawing();

        // Frame counter mechanics / auto move down
        if (frame_count == move_frames) ConsiderMove(DOWN); // Change this to be specific to moving down
        frame_count++;

        CheckDestroyRows();

        GameOver();
    }

    UnloadSounds();
    UnloadBlockTextures();
    CloseAudioDevice();

    CloseWindow();
    return 0;
}

void PickNextBlock() {
    next = GetRandomValue(0,6);
}

void CheckSpawnNextBlock() {
    if (get_next) {
        rotation = 0;
        curr.texture = &blocks[next][rotation];
        curr.type = next;
        curr_loc = spawn_loc;
        PickNextBlock();
        get_next = false;
    }
}

void DrawCurrent() {
    curr.texture = &blocks[curr.type][rotation];
    DrawTexture(*(curr.texture), curr_loc.x, curr_loc.y, WHITE);
}

void DrawExisting() {
    for (int i = 0; i < 20; ++i) {
        for (int j = 0; j < 10; ++j) {
            switch (spaces[i][j]) {
                case BLUE_T:
                    DrawTexture(blocks[0][4], left_box + block_size * j, bot_box - (i + 1) * block_size, WHITE);
                    break;
                case CYAN_Z:
                    DrawTexture(blocks[1][4], left_box + block_size * j, bot_box - (i + 1) * block_size, WHITE);
                    break;
                case GREEN_L:
                    DrawTexture(blocks[2][4], left_box + block_size * j, bot_box - (i + 1) * block_size, WHITE);
                    break;
                case PINK_J:
                    DrawTexture(blocks[3][4], left_box + block_size * j, bot_box - (i + 1) * block_size, WHITE);
                    break;
                case PURPLE_I:
                    DrawTexture(blocks[4][4], left_box + block_size * j, bot_box - (i + 1) * block_size, WHITE);
                    break;
                case RED_O:
                    DrawTexture(blocks[5][4], left_box + block_size * j, bot_box - (i + 1) * block_size, WHITE);
                    break;
                case YELLOW_S:
                    DrawTexture(blocks[6][4], left_box + block_size * j, bot_box - (i + 1) * block_size, WHITE);
                    break;
            }
        }
    }
}

void CheckDestroyRows() {
    bool populated = true;
    for (int i = 0; i < 20; ++i) {
        for (int j = 0; j < 10; ++j) {
            if (!spaces[i][j]) {
                populated = false;
                break;
            }
        }
        if (populated) {
            score++;
            PlaySound(clear_line);
            for (int k = 0; k < 10; ++k) {
                spaces[i][k] = NONE;
                BeginDrawing();
                    UpdateMusicStream(music);
                    ClearBackground(BLACK);
                    MakeGUI();
                    DrawExisting();
                EndDrawing();
                WaitTime(0.05);
            }
            for (int l = i; l < 20; ++l) {
                for (int m = 0; m < 10; ++m) {
                    spaces[l][m] = spaces[l + 1][m];
                }
                BeginDrawing();
                    UpdateMusicStream(music);
                    ClearBackground(BLACK);
                    MakeGUI();
                    DrawExisting();
                EndDrawing();
                WaitTime(0.05);
            }
            i--;
        }
        populated = true;
    }
}

void KeyPressCheck() {
    if (IsKeyPressed(KEY_LEFT)) {
        ConsiderMove(LEFT);
    }
    if (IsKeyPressed(KEY_RIGHT)) {
        ConsiderMove(RIGHT);
    }
    if (IsKeyPressed(KEY_DOWN)) {
        ConsiderMove(DOWN);
    }
    if (IsKeyPressed(KEY_UP)) {
        ConsiderMove(ROTATE);
    }
    if (IsKeyDown(KEY_SPACE)) {
        ConsiderMove(DOWN);
    }
}

void ConsiderMove(Move move) {

    switch (move) {
        case LEFT:
            if (CheckCollision(Vector2Add(curr_loc, vecL), rotation)) {
                curr_loc.x -= block_size;
            }
            break;
        case RIGHT:
            if (CheckCollision(Vector2Add(curr_loc, vecR), rotation)) {
                curr_loc.x += block_size;
            }
            break;
        case DOWN:
        if (CheckCollision(Vector2Add(curr_loc, vecD), rotation)) {
            curr_loc.y += block_size;
            frame_count = 0;
        }
        else {
            PixelsToCoords(curr_loc, block_locs[curr.type][rotation]);
            for (int i = 0; i < 4; ++i) {
                // Check if we're above top of play area
                if (coords[i][1] >= 20) {
                    game_over = true;
                    continue;
                }
                spaces[coords[i][1]][coords[i][0]] = curr.type + 1;
            }
            frame_count = 0;
            get_next = true;
            PlaySound(land_piece);
        }
        break;
        case ROTATE:
            if (CheckCollision(curr_loc, (rotation + 1) % 4)) {
                rotation = (rotation + 1) % 4;
            }
            break;
    }
}

bool CheckCollision(Vector2 proposed, int new_rotation) {
    // Length 4
    PixelsToCoords(proposed, block_locs[curr.type][new_rotation]);

    for (int i = 0; i < 4; ++i) {
        if (coords[i][0] < 0   ||
            coords[i][0] >= 10 ||
            coords[i][1] < 0   ||
            spaces[coords[i][1]][coords[i][0]] > 0
        ) {
            return false;
        }
    }
    return true;
}

void PixelsToCoords(Vector2 current, Vector2 *unit_offsets) {
    // Convert to coords relevant to "spaces"
    for (int i = 0; i < 4; ++i) {
        float px = current.x + unit_offsets[i].x * block_size;
        float py = current.y + unit_offsets[i].y * block_size;
        coords[i][0] = (int)(px - left_box) / block_size;
        coords[i][1] = (int)(py - 668) / -block_size;
    }
}

void GameOver() {
    if (!game_over) return;

    StopMusicStream(music);
    PlaySound(game_over_sfx);
    int game_over_ctr = 0;
    while (game_over && !WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);
            MakeGUI();
            DrawCurrent();
            DrawExisting();
            if (game_over_ctr < 15) {
                DrawText("GAME OVER", 250, 720, 30, WHITE);
                DrawText("PRESS SPACE TO PLAY AGAIN", 175, 760, 20, WHITE);
            }

        EndDrawing();
        game_over_ctr = (game_over_ctr + 1) % 30;

        if (IsKeyPressed(KEY_SPACE)) {
            score = 0;
            game_over = false;
            frame_count = 0;
            PickNextBlock();
            memset(spaces, 0, 25 * 10 * sizeof(Tetromino));
            PlayMusicStream(music);
        }
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

    // Debug: active block's 4 occupied grid cells, bottom-right of the play area
    // Vector2 *shape = block_locs[curr.type][rotation];
    // int cell_coords[4][2];
    // for (int i = 0; i < 4; i++) {
    //     float px = curr_loc.x + shape[i].x * block_size;
    //     float py = curr_loc.y + shape[i].y * block_size;
    //     cell_coords[i][0] = (int)(px - left_box) / block_size;
    //     cell_coords[i][1] = (int)(py - (bot_box - block_size)) / -block_size;
    // }
    // snprintf(debug_label, 48, "(%d,%d)(%d,%d)(%d,%d)(%d,%d)",
    //     cell_coords[0][0], cell_coords[0][1],
    //     cell_coords[1][0], cell_coords[1][1],
    //     cell_coords[2][0], cell_coords[2][1],
    //     cell_coords[3][0], cell_coords[3][1]);
    // DrawText(debug_label, right_box - MeasureText(debug_label, 16) - 4, bot_box - 20, 16, GRAY);
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

void UnloadBlockTextures() {
    for (int i = 0; i < 7; ++i) {
        for (int j = 0; j < 5; ++j) {
            UnloadTexture(blocks[i][j]);
        }
    }
}

void LoadSounds() {
    music = LoadMusicStream("music/Type B.mp3");
    land_piece = LoadSound("sound/drop.mp3");
    clear_line = LoadSound("sound/line_clear.mp3");
    game_over_sfx = LoadSound("sound/game_over.mp3");
}

void UnloadSounds() {
    UnloadMusicStream(music);
    UnloadSound(land_piece);
    UnloadSound(clear_line);
    UnloadSound(game_over_sfx);
}

void InitBlockLocs() {

    // Blue T
    // T1
    block_locs[0][0][0] = (Vector2){1, 0};
    block_locs[0][0][1] = (Vector2){0, 1};
    block_locs[0][0][2] = (Vector2){1, 1};
    block_locs[0][0][3] = (Vector2){2, 1};

    // T2
    block_locs[0][1][0] = (Vector2){1, 0};
    block_locs[0][1][1] = (Vector2){1, 1};
    block_locs[0][1][2] = (Vector2){2, 1};
    block_locs[0][1][3] = (Vector2){1, 2};

    // T3
    block_locs[0][2][0] = (Vector2){0, 1};
    block_locs[0][2][1] = (Vector2){1, 1};
    block_locs[0][2][2] = (Vector2){2, 1};
    block_locs[0][2][3] = (Vector2){1, 2};

    // T4
    block_locs[0][3][0] = (Vector2){1, 0};
    block_locs[0][3][1] = (Vector2){0, 1};
    block_locs[0][3][2] = (Vector2){1, 1};
    block_locs[0][3][3] = (Vector2){1, 2};

    // Cyan Z
    // Z1
    block_locs[1][0][0] = (Vector2){0, 0};
    block_locs[1][0][1] = (Vector2){1, 0};
    block_locs[1][0][2] = (Vector2){1, 1};
    block_locs[1][0][3] = (Vector2){2, 1};

    // Z2
    block_locs[1][1][0] = (Vector2){2, 0};
    block_locs[1][1][1] = (Vector2){1, 1};
    block_locs[1][1][2] = (Vector2){2, 1};
    block_locs[1][1][3] = (Vector2){1, 2};

    // Z3
    block_locs[1][2][0] = (Vector2){0, 1};
    block_locs[1][2][1] = (Vector2){1, 1};
    block_locs[1][2][2] = (Vector2){1, 2};
    block_locs[1][2][3] = (Vector2){2, 2};

    // Z4
    block_locs[1][3][0] = (Vector2){1, 0};
    block_locs[1][3][1] = (Vector2){0, 1};
    block_locs[1][3][2] = (Vector2){1, 1};
    block_locs[1][3][3] = (Vector2){0, 2};

    // Green L
    // L1
    block_locs[2][0][0] = (Vector2){2, 0};
    block_locs[2][0][1] = (Vector2){0, 1};
    block_locs[2][0][2] = (Vector2){1, 1};
    block_locs[2][0][3] = (Vector2){2, 1};

    // L2
    block_locs[2][1][0] = (Vector2){1, 0};
    block_locs[2][1][1] = (Vector2){1, 1};
    block_locs[2][1][2] = (Vector2){1, 2};
    block_locs[2][1][3] = (Vector2){2, 2};

    // L3
    block_locs[2][2][0] = (Vector2){0, 1};
    block_locs[2][2][1] = (Vector2){1, 1};
    block_locs[2][2][2] = (Vector2){2, 1};
    block_locs[2][2][3] = (Vector2){0, 2};

    // L4
    block_locs[2][3][0] = (Vector2){0, 0};
    block_locs[2][3][1] = (Vector2){1, 0};
    block_locs[2][3][2] = (Vector2){1, 1};
    block_locs[2][3][3] = (Vector2){1, 2};

    // Pink J
    // J1
    block_locs[3][0][0] = (Vector2){0, 0};
    block_locs[3][0][1] = (Vector2){0, 1};
    block_locs[3][0][2] = (Vector2){1, 1};
    block_locs[3][0][3] = (Vector2){2, 1};

    // J2
    block_locs[3][1][0] = (Vector2){1, 0};
    block_locs[3][1][1] = (Vector2){2, 0};
    block_locs[3][1][2] = (Vector2){1, 1};
    block_locs[3][1][3] = (Vector2){1, 2};

    // J3
    block_locs[3][2][0] = (Vector2){0, 1};
    block_locs[3][2][1] = (Vector2){1, 1};
    block_locs[3][2][2] = (Vector2){2, 1};
    block_locs[3][2][3] = (Vector2){2, 2};

    // J4
    block_locs[3][3][0] = (Vector2){1, 0};
    block_locs[3][3][1] = (Vector2){1, 1};
    block_locs[3][3][2] = (Vector2){0, 2};
    block_locs[3][3][3] = (Vector2){1, 2};

    // Purple I
    // I1
    block_locs[4][0][0] = (Vector2){0, 1};
    block_locs[4][0][1] = (Vector2){1, 1};
    block_locs[4][0][2] = (Vector2){2, 1};
    block_locs[4][0][3] = (Vector2){3, 1};

    // I2
    block_locs[4][1][0] = (Vector2){2, 0};
    block_locs[4][1][1] = (Vector2){2, 1};
    block_locs[4][1][2] = (Vector2){2, 2};
    block_locs[4][1][3] = (Vector2){2, 3};

    // I3
    block_locs[4][2][0] = (Vector2){0, 2};
    block_locs[4][2][1] = (Vector2){1, 2};
    block_locs[4][2][2] = (Vector2){2, 2};
    block_locs[4][2][3] = (Vector2){3, 2};

    // I4
    block_locs[4][3][0] = (Vector2){1, 0};
    block_locs[4][3][1] = (Vector2){1, 1};
    block_locs[4][3][2] = (Vector2){1, 2};
    block_locs[4][3][3] = (Vector2){1, 3};

    // Red O (same cells in every orientation)
    for (int r = 0; r < 4; r++) {
        block_locs[5][r][0] = (Vector2){1, 0};
        block_locs[5][r][1] = (Vector2){2, 0};
        block_locs[5][r][2] = (Vector2){1, 1};
        block_locs[5][r][3] = (Vector2){2, 1};
    }

    // Yellow S
    // S1
    block_locs[6][0][0] = (Vector2){1, 0};
    block_locs[6][0][1] = (Vector2){2, 0};
    block_locs[6][0][2] = (Vector2){0, 1};
    block_locs[6][0][3] = (Vector2){1, 1};

    // S2
    block_locs[6][1][0] = (Vector2){1, 0};
    block_locs[6][1][1] = (Vector2){1, 1};
    block_locs[6][1][2] = (Vector2){2, 1};
    block_locs[6][1][3] = (Vector2){2, 2};

    // S3
    block_locs[6][2][0] = (Vector2){1, 1};
    block_locs[6][2][1] = (Vector2){2, 1};
    block_locs[6][2][2] = (Vector2){0, 2};
    block_locs[6][2][3] = (Vector2){1, 2};

    // S4
    block_locs[6][3][0] = (Vector2){0, 0};
    block_locs[6][3][1] = (Vector2){0, 1};
    block_locs[6][3][2] = (Vector2){1, 1};
    block_locs[6][3][3] = (Vector2){1, 2};
}