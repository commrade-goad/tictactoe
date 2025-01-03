#include <raylib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

#define DEBUG_KEY

#define WINDOW_W 1280
#define WINDOW_H 720
#define BGCOLOR GetColor(0x1e1e1eff)
#define GRID 100
#define SCALE 2.0f
#define PADDING_TOTAL PADDING * 2
#define PADDING 10
#define BOARD_SIZE 3

typedef enum {
    NONE,
    PLAYERO,
    PLAYERX,
} Player;

Player game_state[BOARD_SIZE][BOARD_SIZE] = {
    {NONE, NONE, NONE},
    {NONE, NONE, NONE},
    {NONE, NONE, NONE},
};
Rectangle game_rec[BOARD_SIZE][BOARD_SIZE] = {};
Vector2 sel_rec = {1, 1};
Vector2 sel_rec_shadow = {1, 1};

Vector2 turn_box = {PADDING * 2, PADDING * 2};
Vector2 turn_box_anim = {PADDING * 2, PADDING * 2 + 100 + PADDING};
bool turn = 0;

Vector2 Vec2Lerp(Vector2 a, Vector2 b, float t) {
    return (Vector2) {
        a.x + t * (b.x - a.x),
        a.y + t * (b.y - a.y)
    };
}

void move_sel(Vector2 move) {
    Vector2 tmp = {sel_rec.x + move.x, sel_rec.y + move.y};
    if (tmp.x < BOARD_SIZE && tmp.x >= 0 &&
        tmp.y < BOARD_SIZE && tmp.y >= 0) {
            sel_rec = tmp;
    }
}

void set_game_state() {
    int x = sel_rec.x;
    int y = sel_rec.y;
    if (turn == 0 && game_state[y][x] == NONE) {
        game_state[y][x] = PLAYERO;
        turn = !turn;
    } else if (game_state[y][x] == NONE) {
        game_state[y][x] = PLAYERX;
        turn = !turn;
    }
}

void init_window() {
    InitWindow(WINDOW_W, WINDOW_H, "tictactoe");
    SetExitKey(KEY_Q);
    SetTargetFPS(60);
}

void render_game(bool win) {
    static Rectangle inner = {
        .width = GRID * BOARD_SIZE * SCALE + PADDING_TOTAL,
        .height = GRID * BOARD_SIZE * SCALE + PADDING_TOTAL,
    };

    inner.x = (WINDOW_W - inner.width) / 2;
    inner.y = (WINDOW_H - inner.height) / 2;

    BeginDrawing();

    ClearBackground(BGCOLOR);

    // board
    DrawRectangleRec(inner, GetColor(0x323232ff));

    // turn indicator
    Color tc = RED;
    Color tc2 = BLUE;

    turn_box_anim = Vec2Lerp(turn_box_anim, 
                             (Vector2) {PADDING * 2, turn ? PADDING * 2 : PADDING * 2 + 100 + PADDING}, 
                             0.6f);
    turn_box = Vec2Lerp(turn_box, 
                        (Vector2) {PADDING * 2, turn ? PADDING * 2 + 100 + PADDING : PADDING * 2}, 
                        0.6f);
    DrawRectangle(turn_box.x, turn_box.y, 100, 100, tc);
    DrawRectangle(turn_box_anim.x, turn_box_anim.y, 100, 100, tc2);

    // the fade effect
    DrawRectangleGradientV(PADDING * 2, PADDING * 2 + 100, 100, 50, GetColor(0x1f1f1f00), BGCOLOR);
    DrawRectangle(PADDING * 2, PADDING * 2 + 150, 100, 100, BGCOLOR);
    // the border
    DrawRectangleLinesEx((Rectangle) {PADDING, PADDING, 100 + PADDING * 2, 100 + PADDING * 2}, 4.0f, WHITE);

    // the moving highlight
    DrawRectangle(
        inner.x + PADDING + (sel_rec_shadow.x * GRID * SCALE),
        inner.y + PADDING + (sel_rec_shadow.y * GRID * SCALE),
        GRID * SCALE,
        GRID * SCALE,
        WHITE
    );

    // grid and stuff
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            Color c = GetColor(0x292929ff);
            switch (game_state[y][x]) {
                case PLAYERO: {
                    c = RED;
                    break;
                }
                case PLAYERX: {
                    c = BLUE;
                    break;
                }
                default: {
                    break;
                }
            }
            float tolerance = 0.01f;
            if (y == sel_rec.y && x == sel_rec.x &&
                fabsf(sel_rec_shadow.x - sel_rec.x) < tolerance &&
                fabsf(sel_rec_shadow.y - sel_rec.y) < tolerance) {
                sel_rec_shadow = sel_rec;
                DrawRectangle(
                    inner.x + PADDING + (x * GRID * SCALE),
                    inner.y + PADDING + (y * GRID * SCALE),
                    GRID * SCALE,
                    GRID * SCALE,
                    WHITE
                );
            }
            game_rec[y][x] = (Rectangle) {
                inner.x + PADDING + ((float)PADDING/2) + (x * GRID * SCALE),
                inner.y + PADDING + ((float)PADDING/2) + (y * GRID * SCALE),
                GRID * SCALE - PADDING,
                GRID * SCALE - PADDING,
            };
            DrawRectangleRec(game_rec[y][x],c);
        }
    }

    // WINNER
    if (win) {
        char buf[512];
        Color tc = BLUE;
        if (!turn) {
            strncpy(buf, "BLUE", strlen("BLUE"));
            buf[strlen("BLUE")] = '\0';
        } else {
            strncpy(buf, "RED", strlen("RED"));
            buf[strlen("RED")] = '\0';
            tc = RED;
        }

        DrawRectangle(0, 0, WINDOW_W, WINDOW_H, GetColor(0x1f1f1faa));
        DrawText(buf, (WINDOW_W - MeasureText(buf, 64)) / 2, (WINDOW_H - 32)/ 2 - 60,  64, tc);
        DrawText("WIN!", (WINDOW_W - MeasureText("WIN!", 64)) / 2, (WINDOW_H - 32)/ 2,  64, WHITE);
        DrawText("Click to play again!", (WINDOW_W - MeasureText("Click to play again!", 32)) / 2, WINDOW_H - (WINDOW_H - 16)/ 4,  32, WHITE);
    }

    EndDrawing();
}

Player check_win() {
    Vector2 direction[8] = {
        { 1,  0},
        {-1,  0},
        { 0, -1},
        { 0,  1},
        { 1,  1},
        {-1, -1},
        { 1, -1},
        {-1,  1}
    };

    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            Player p = game_state[y][x];
            if (p == NONE) continue;

            for (int d = 0; d < 8; d++) {
                int c = 1;
                for (int step = 1; step < BOARD_SIZE; step++) {
                    int nextY = y + step * direction[d].y;
                    int nextX = x + step * direction[d].x;

                    if (nextX >= 0 && nextX < BOARD_SIZE && nextY >= 0 &&
                        nextY < BOARD_SIZE && game_state[nextY][nextX] == p) {
                        c++;
                    } else break;

                    if (c == BOARD_SIZE) {
                        return p;
                    }
                }
            }
        }
    }
    return NONE;
}

void mechanic(Vector2 *last_mp, bool* win) {

    Player res = check_win();
    if (res != NONE) {
        if (!(*win)) (*win) = true;
        if (GetKeyPressed() != 0 || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            (*win) = false;
            turn = 0;
            for (int i = 0; i < BOARD_SIZE; i++) {
                for (int j = 0; j < BOARD_SIZE; j++) {
                    game_state[i][j] = NONE;
                }
            }
        }
        return;
    }

    sel_rec_shadow = Vec2Lerp(sel_rec_shadow, sel_rec, 0.65f);
    Vector2 mp = GetMousePosition();
    bool mouse_mode = false;
    if (last_mp->x != mp.x && last_mp->y != mp.y) {
        mouse_mode = true;
    }

#ifdef DEBUG_KEY
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || IsKeyPressed(KEY_SPACE)) {
        int x = sel_rec.x, y = sel_rec.y;
        if (game_state[y][x] != NONE) {
            game_state[y][x] = NONE;
            turn = !turn;
        }
    }
    if (IsKeyPressed(KEY_N)) turn = !turn;
#endif

    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            bool colliding = CheckCollisionPointRec(mp, game_rec[x][y]);
            if (colliding && mouse_mode) sel_rec = (Vector2) {y, x};
            if (colliding && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) set_game_state();
        }
    }
    if (!mouse_mode) {
        if (IsKeyPressed(KEY_W)) move_sel((Vector2) {0, -1});
        if (IsKeyPressed(KEY_S)) move_sel((Vector2) {0, 1});
        if (IsKeyPressed(KEY_D)) move_sel((Vector2) {1, 0});
        if (IsKeyPressed(KEY_A)) move_sel((Vector2) {-1, 0});
    }
    if (IsKeyPressed(KEY_ENTER)) set_game_state();
    *last_mp = mp;
}

int main(void) {
    init_window();
    Vector2 last_mp = GetMousePosition();
    bool win = false;
    while (!WindowShouldClose()) {
        const float ft = GetFrameTime();
        mechanic(&last_mp, &win);
        check_win();
        render_game(win);
    }
    return 0;
}
