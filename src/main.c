#include "raylib.h"

#define SCREEN_WIDTH (1280)
#define SCREEN_HEIGHT (720)

// Change this depending on the path of your executable relative to the assets folder
#define ASSET_PATH "../assets/"

typedef enum enemy_flags_enum
{
	ENEMY_ACTIVE = 0x01,
	ENEMY_PASTUS = 0x02,
	ENEMY_PASTMIDDLE = 0x04
} EnemyFlags;

typedef struct enemy_state_struct
{
	EnemyFlags flags;
	union {
		Rectangle collider;
		struct
		{
			float x;
			float y;
			float width;
			float height;
		};
	};
} Enemy;
#define ENEMY_COUNT (3)
Enemy enemies[ENEMY_COUNT];

#define GROUND_HEIGHT (30)
int GetHeighOnGround(int height)
{
	return SCREEN_HEIGHT - (GROUND_HEIGHT + height);
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Window title");
    SetTargetFPS(60);
	
	const Rectangle standDino = {
			.x = 30,
			.width = 50,
			.height = 80,
			.y = GetHeighOnGround(80)
	};
	const Rectangle crouchDino = {
			.x = 30,
			.width = 75,
			.height = 40,
			.y = GetHeighOnGround(50)
	};
	
	const Rectangle cactiCollider = {
			.x = SCREEN_WIDTH,
			.width = standDino.width - 8,
			.height = standDino.height - 20,
			.y = GetHeighOnGround(standDino.height - 20)
	};
	const Rectangle pteroCollider = {
			.x = SCREEN_WIDTH,
			.width = standDino.width + 70,
			.height = 40,
			.y = GetHeighOnGround(40 + 56)
	};
	
	Rectangle dinoRect = standDino;
	float dinoSpeed = 0.0f;
	
	for (int i = 0; i < ENEMY_COUNT; i++)
	{
		enemies[i].collider = cactiCollider;
	}
	enemies[0].flags |= ENEMY_ACTIVE;
	
	int score = 0;
	bool gameOver = false;

    // Main game loop
    while (!WindowShouldClose())
    {
    	if (!gameOver)
		{
			if (IsKeyPressed(KEY_SPACE) && dinoRect.y == GetHeighOnGround(dinoRect.height))
			{
				dinoSpeed = -4.8f;
			}
			
			if (IsKeyPressed(KEY_C))
			{
				//dinoRect.y = dinoRect.y + (standDino.y - crouchDino.y);
				dinoRect.y = dinoRect.y + (crouchDino.y - standDino.y);
				dinoRect.width = crouchDino.width;
				dinoRect.height = crouchDino.height;
			} else if (IsKeyReleased(KEY_C))
			{
				//dinoRect.y = dinoRect.y - (standDino.y - crouchDino.y);
				dinoRect.y = dinoRect.y - (crouchDino.y - standDino.y);
				dinoRect.width = standDino.width;
				dinoRect.height = standDino.height;
			}
		
			if (dinoSpeed < 5.0f)
			{
				dinoSpeed += 5 * GetFrameTime();
			} else
			{
				dinoSpeed = 5.0f;
			}
			dinoRect.y += dinoSpeed;
		
			if (dinoRect.y >= GetHeighOnGround(dinoRect.height))
			{
				dinoRect.y = GetHeighOnGround(dinoRect.height);
			}
			
			for (int i = 0; i < ENEMY_COUNT; i++)
			{
				if (enemies[i].flags & ENEMY_ACTIVE)
				{
					if (enemies[i].x + enemies[i].width < dinoRect.x && !(enemies[i].flags & ENEMY_PASTUS))
					{
						enemies[i].flags |= ENEMY_PASTUS;
						score++;
					}
					
					gameOver = gameOver | CheckCollisionRecs(dinoRect, enemies[i].collider);
					
					enemies[i].x -= 150 * GetFrameTime();
					
					if (enemies[i].x + enemies[i].width < 0)
						enemies[i].flags &= ~ENEMY_ACTIVE;
					
					if (enemies[i].x - enemies[i].width < SCREEN_WIDTH / 3 && !(enemies[i].flags & ENEMY_PASTMIDDLE))
					{
						enemies[i].flags |= ENEMY_PASTMIDDLE;
						for (int j = 0; j < ENEMY_COUNT; j++)
						{
							if (j != i && !(enemies[j].flags & ENEMY_ACTIVE))
							{
								int pteroORcacti = GetRandomValue(0, 1);
								
								enemies[j] = (Enemy){0};
								enemies[j].flags |= ENEMY_ACTIVE;
								
								if (pteroORcacti)
									enemies[j].collider = cactiCollider;
								else
									enemies[j].collider = pteroCollider;
									
								break;
							}
						}
					}
				}
			}
		}
    	
        BeginDrawing();

        ClearBackground(RAYWHITE);
        
        DrawRectangle(0, SCREEN_HEIGHT - GROUND_HEIGHT, SCREEN_WIDTH, GROUND_HEIGHT, BROWN);
        DrawRectangleRec(dinoRect, BLACK);
        for (int i = 0; i < ENEMY_COUNT; i++)
		{
        	if (enemies[i].flags & ENEMY_ACTIVE)
				DrawRectangleRec(enemies[i].collider, GRAY);
		}
        
        if (gameOver)
		{
			DrawText("Game Over!", 30, 30, 60, BLACK);
		} else
		{
			DrawText(FormatText("Score: %i", score), 30, 30, 60, BLACK);
		}

        EndDrawing();
    }

    CloseWindow();

    return 0;
}