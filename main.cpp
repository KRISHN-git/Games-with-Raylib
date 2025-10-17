#include <raylib.h>
#include <iostream>

// Theme with colors specified:
Color Blue         = Color{0, 229, 255, 255};   // Neon Cyan
Color Dark_Blue    = Color{10, 25, 47, 255};    // Dark background
Color Light_Blue   = Color{144, 224, 239, 255}; // Glow cyan
Color Orange       = Color{255, 111, 97, 255};  // Neon Orange
Color Glow_Orange  = Color{255, 111, 97, 80};   // Glow (ball & paddles)
Color Glow_Cyan    = Color{0, 229, 255, 60};    // Glow (center line & circle)

int player_score = 0;
int cpu_score = 0;

class Ball {
 public:
    float x, y;
    int speed_x, speed_y;
    int radius;

    void Draw() {
        // Glow layers behind the ball
        for (int i = 20; i > 0; i -= 5) {
            DrawCircle(x, y, radius + i, Color{Glow_Orange.r, Glow_Orange.g, Glow_Orange.b, (unsigned char)(i * 3)});
        }
        DrawCircle(x, y, radius, Orange);
    }

    void Update() {
        x += speed_x;
        y += speed_y;

        if (y + radius >= GetScreenHeight() || y - radius <= 0) {
            speed_y *= -1;
        }

        // Cpu wins
        if (x + radius >= GetScreenWidth()) {
            cpu_score++;
            ResetBall();
        }

        // Player wins
        if (x - radius <= 0) {
            player_score++;
            ResetBall();
        }
    }

    void ResetBall() {
        x = GetScreenWidth() / 2;
        y = GetScreenHeight() / 2;

        int speed_choices[2] = {-1, 1};
        speed_x *= speed_choices[GetRandomValue(0, 1)];
        speed_y *= speed_choices[GetRandomValue(0, 1)];
    }
};

class Paddle {
 protected:
    void LimitMovement() {
        if (y <= 0) y = 0;
        if (y + height >= GetScreenHeight()) y = GetScreenHeight() - height;
    }

 public:
    float x, y;
    float width, height;
    int speed;

    void Draw() {
        //  Glow behind paddle
        DrawRectangleRounded(Rectangle{x - 5, y - 5, width + 10, height + 10}, 0.8, 0, Glow_Orange);
        DrawRectangleRounded(Rectangle{x, y, width, height}, 0.8, 0, Orange);
    }

    void Update() {
        if (IsKeyDown(KEY_UP)) y -= speed;
        if (IsKeyDown(KEY_DOWN)) y += speed;
        LimitMovement();
    }
};

class CpuPaddle : public Paddle {
 public:
    void Update(int ball_y){
        if (y + height / 2 > ball_y) y -= speed;
        if (y + height / 2 <= ball_y) y += speed;
        LimitMovement();
    }
};

Ball ball;
Paddle player;
CpuPaddle cpu;

int main() {
    std::cout << "Starting the game" << std::endl;
    const int screen_width = 1280;
    const int screen_height = 800;
    InitWindow(screen_width, screen_height, " Ping Pong Game ");
    SetTargetFPS(60);

    ball.radius = 20;
    ball.x = screen_width / 2;
    ball.y = screen_height / 2;
    ball.speed_x = 7;
    ball.speed_y = 7;

    player.width = 25;
    player.height = 120;
    player.x = screen_width - player.width - 10;
    player.y = screen_height / 2 - player.height / 2;
    player.speed = 6;

    cpu.width = 25;
    cpu.height = 120;
    cpu.x = 10;
    cpu.y = screen_height / 2 - cpu.height / 2;
    cpu.speed = 6;

    while (!WindowShouldClose()) {
        // Updating
        ball.Update();
        player.Update();
        cpu.Update(ball.y);

        //  Collision detection
        if (CheckCollisionCircleRec({ball.x, ball.y}, ball.radius,
                                    {player.x, player.y, player.width, player.height})) {
            ball.speed_x *= -1;
        }

        if (CheckCollisionCircleRec({ball.x, ball.y}, ball.radius,
                                    {cpu.x, cpu.y, cpu.width, cpu.height})) {
            ball.speed_x *= -1;
        }

        // Drawing
        BeginDrawing();
        ClearBackground(Dark_Blue);

        // Glowing center circle
        for (int i = 80; i > 0; i -= 20) {
            DrawCircle(screen_width / 2, screen_height / 2, 150 + i, 
                       Color{Glow_Cyan.r, Glow_Cyan.g, Glow_Cyan.b, (unsigned char)(i)});
        }
        DrawCircle(screen_width / 2, screen_height / 2, 150, Light_Blue);

        // Glowing center line
        for (int i = 15; i > 0; i -= 5) {
            DrawLineEx({(float)screen_width / 2, 0},
                       {(float)screen_width / 2, (float)screen_height},
                       i, Color{Glow_Cyan.r, Glow_Cyan.g, Glow_Cyan.b, (unsigned char)(i * 4)});
        }
        DrawLine(screen_width / 2, 0, screen_width / 2, screen_height, Blue);

        // Ball & paddles
        ball.Draw();
        cpu.Draw();
        player.Draw();

        // Score
        DrawText(TextFormat("%i", cpu_score), screen_width / 4 - 20, 20, 80, Orange);
        DrawText(TextFormat("%i", player_score), 3 * screen_width / 4 - 20, 20, 80, Orange);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
