#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_HEARTS 3
#define MAX_ENV_ITEMS 20
#define MAX_MUSHROOMS 10
#define MAX_COINS 10
#define MARIO_ANIMATION_SPEED 0.15f
#define MARIO_FRAMES 6
#define COUNTDOWN_DURATION 3

typedef struct AnimatedSprite {
    Texture2D texture;
    Texture2D texture2;
    Rectangle frameRec;
    int frame;
    float currentTime;
} AnimatedSprite;

typedef struct Player {
    Rectangle rect;
    float speed;
    int canJump;
    int isJumping;
    AnimatedSprite sprite;
} Player;

typedef struct EnvItem {
    Rectangle rect;
    Color color;
} EnvItem;

typedef struct Mushroom {
    Rectangle rect;
    Texture2D texture;
    float speed;
    Vector2 xBounds;
    int direction;
    int isActive;
} Mushroom;

typedef struct Coin {
    Rectangle rect;
    Texture2D texture;
    int isActive;
} Coin;

typedef struct Level {
    EnvItem envItems[MAX_ENV_ITEMS];
    Mushroom mushrooms[MAX_MUSHROOMS];
    Coin coins[MAX_COINS];
    int finishLine;
} Level;

typedef struct Heart {
    Vector2 position;
    Texture2D texture;
} Heart;

typedef enum {
    STATE_MENU,
    STATE_GAME,
    STATE_LOADING,
    STATE_HOWTOPLAY,
    STATE_INFO,
    STATE_DEATH,
    STATE_GAMEOVER,
    STATE_GAMEWON,
} GameState;

void UpdatePlayer(Player *player, EnvItem *envItems, int envItemsLength, float delta, Rectangle playerRect, Mushroom *mushrooms, int numMushrooms, Heart *hearts, int numHearts, GameState gameState, int *direction, Sound jumpsound, Sound deathsound, Sound kicksound, int *dead, int *lives);
void UpdateMushrooms(Mushroom *mushrooms, int numMushrooms, float delta, int currentLevel);
void updateCamera(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height);
void LoadNextLevel(Level *levels, int *currentLevel, Level *currentLevelData);

const int screenWidth = 800;
const int screenHeight = 450;
int G = 750;
float PLAYER_JUMP_SPD = 400.0;
float PLAYER_HOR_SPD = 350.0;

int main()
{
    InitWindow(screenWidth, screenHeight, "The Adventures of Pookie");
    InitAudioDevice();
    GameState gameState = STATE_MENU;

    Texture2D marioTexture = LoadTexture("./images/scarfy2.png");
    Texture2D mushroomTexture = LoadTexture("./images/slime.png");
    Texture2D menubackground = LoadTexture("./images/BG.png");
    Texture2D background = LoadTexture("./images/mbg.png");
    Texture2D background2 = LoadTexture("./images/bgunderground (1).png");
    Texture2D background3 = LoadTexture("./images/bg3.png");
    Texture2D loadingscreenbackground = LoadTexture("./images/loadingscreen.png");
    Texture2D platform = LoadTexture("./images/platform.png");
    Texture2D platform2 = LoadTexture("./images/platform2.png");
    Texture2D lava = LoadTexture("./images/lava.png");
    Texture2D heartTexture = LoadTexture("./images/heart.png");
    Texture2D startbutton = LoadTexture("./images/Start_BTN.png");
    Texture2D exitbutton = LoadTexture("./images/Exit_BTN.png");
    Texture2D infobutton = LoadTexture("./images/Info_BTN.png");
    Texture2D faqbutton = LoadTexture("./images/FAQ_BTN.png");
    Texture2D returnbutton = LoadTexture("./images/Replay_BTN.png");
    Texture2D cointexture = LoadTexture("./images/coin.png");
    Texture2D spacebg = LoadTexture("./images/spacebg.png");

    Texture2D startbuttonact = LoadTexture("./activebtn/Start_BTN.png");
    Texture2D exitbuttonact = LoadTexture("./activebtn/Exit_BTN.png");
    Texture2D infobuttonact = LoadTexture("./activebtn/Info_BTN.png");
    Texture2D faqbuttonact = LoadTexture("./activebtn/FAQ_BTN.png");
    Texture2D returnbuttonact = LoadTexture("./activebtn/Replay_BTN.png");


    Rectangle startbuttonrect = {screenWidth / 2 - 100, screenHeight / 2 - 5, 200, 50};
    Rectangle exitbuttonrect = {screenWidth / 2 - 100, screenHeight / 2 + 50, 200, 50};
    Rectangle infobuttonrect = {screenWidth - 100, screenHeight - 75, 50, 50};
    Rectangle returnbuttonrect = {screenWidth - 100, screenHeight - 75, 50, 50};
    Rectangle faqbuttonrect = {50, screenHeight - 75, 50, 50};

    Sound coinsound = LoadSound("./sounds/coin.mp3");
    Sound gameoversound = LoadSound("./sounds/gameover.wav");
    Sound jumpsound = LoadSound("./sounds/jump.mp3");
    Sound levelcompletesound = LoadSound("./sounds/levelcomplete.mp3");
    Sound deathsound = LoadSound("./sounds/player_die.wav");
    Sound kicksound = LoadSound("./sounds/kick.wav");
    Sound winsound = LoadSound("./sounds/wingame.wav");

    

    AnimatedSprite marioSprite = {
        LoadTexture("./images/scarfy2.png"),
        LoadTexture("./images/scarfy3.png"),
        (Rectangle){0, 0, marioTexture.width / MARIO_FRAMES, marioTexture.height},
        0,
        0.0f
    };

    Player player = {
        {150, 280, 50, 50},
        0,
        0,
        0,
        .sprite = marioSprite    
    };

    int playerdirection = 1;
    int dead = 0;
    float countdownTimer = COUNTDOWN_DURATION;
    int countdownDisplayed = 0;

    mushroomTexture.height = 30;
    mushroomTexture.width = 30;
    cointexture.height = 30;
    cointexture.width = 30;
    background.height = screenHeight;
    background.width = screenWidth;
    loadingscreenbackground.height = screenHeight;
    loadingscreenbackground.width = screenWidth;
    background3.height = screenHeight;
    background3.width = screenWidth;
    menubackground.height = screenHeight;
    menubackground.width = screenWidth;
    spacebg.height = screenHeight;
    spacebg.width = screenWidth;
    lava.width = 200;
    startbutton.width = 200;
    startbutton.height = 50;
    exitbutton.width = 200;
    exitbutton.height = 50;
    infobutton.width = 50;
    infobutton.height = 50;
    faqbutton.width = 50;
    faqbutton.height = 50;
    returnbutton.width = 50;
    returnbutton.height = 50;
    startbuttonact.width = 200;
    startbuttonact.height = 50;
    exitbuttonact.width = 200;
    exitbuttonact.height = 50;
    infobuttonact.width = 50;
    infobuttonact.height = 50;
    faqbuttonact.width = 50;
    faqbuttonact.height = 50;
    returnbuttonact.width = 50;
    returnbuttonact.height = 50;
    heartTexture.height = 10;
    heartTexture.width = 10;

    Level levels[] = {
        // LEVEL 1
        {
            {
                {{0, 0, 2300, 520}, BLANK},
                {{0, 500, 800, 25}, BROWN},
                {{900, 400, 60, 25}, BROWN},
                {{1150, 500, 1000, 25}, BROWN},
                {{1250, 400, 100, 25}, BROWN},
                {{1350, 320, 200, 25}, BROWN},
                {{2250, 500, 100, 25}, BROWN},
            },
            {
                {{1610, 470, 30, 30}, mushroomTexture, 250.0f, (Vector2){1450, 1750}, 1, 1},
                {{1810, 470, 30, 30}, mushroomTexture, 250.0f, (Vector2){1750, 2050}, -1, 1},
            },
            {
                {{1260, 360, 30, 30}, LoadTexture("./images/coin.png"), 1},
                {{1400, 280, 30, 30}, LoadTexture("./images/coin.png"), 1},
                {{1500, 460, 30, 30}, LoadTexture("./images/coin.png"), 1},
            },
            2275,
        },
        // LEVEL 2
        {
            {
                {{0, 0, 2500, 520}, BLANK},

                {{0, 450, 1000, 100}, BROWN},
                {{1000, 525, 200, 25}, ORANGE},
                {{1200, 450, 500, 100}, BROWN},

                {{1700, 525, 600, 25}, ORANGE},
                {{1900, 380, 100, 25}, BROWN},
                {{2300, 450, 600, 100}, BROWN},
                {{2900, 525, 1700, 25}, ORANGE},
                {{3150, 380, 100, 25}, BROWN},
                {{3500, 380, 100, 25}, BROWN},
                {{3850, 380, 100, 25}, BROWN},
                {{4200, 380, 100, 25}, BROWN},
                {{4600, 450, 400, 100}, BROWN},

            },
            {
                {{1610, 420, 30, 30}, mushroomTexture, 250.0f, (Vector2){1200, 1670}, 1, 1},
                {{1210, 420, 30, 30}, mushroomTexture, 250.0f, (Vector2){1200, 1670}, -1, 1},
                {{2310, 420, 30, 30}, mushroomTexture, 250.0f, (Vector2){2300, 2870}, 1, 1},
                {{2870, 420, 30, 30}, mushroomTexture, 250.0f, (Vector2){2300, 2870}, -1, 1},
                {{2500, 420, 30, 30}, mushroomTexture, 250.0f, (Vector2){2300, 2870}, -1, 1},

            },
            {
                {{1200, 290, 30, 30}, LoadTexture("./images/coin.png"), 1},
                {{1530, 340, 30, 30}, LoadTexture("./images/coin.png"), 1},
                {{2250, 270, 30, 30}, LoadTexture("./images/coin.png"), 1},
                {{3200, 340, 30, 30}, LoadTexture("./images/coin.png"), 1},
                {{3550, 340, 30, 30}, LoadTexture("./images/coin.png"), 1},
                {{3900, 340, 30, 30}, LoadTexture("./images/coin.png"), 1},
                {{4250, 340, 30, 30}, LoadTexture("./images/coin.png"), 1},

            },
            4999,
        },


        
        // LEVEL 3
        {
            {
                {{0, 0, 2300, 520}, BLANK},
                {{0, 500, 750, 25}, BROWN},
                {{850, 420, 300, 25}, BROWN},
                {{1550, 500, 600, 25}, BROWN},
                {{2400, 420, 200, 25}, BROWN},
                {{2750, 340, 100, 25}, BROWN},
                {{2900, 280, 100, 25}, BROWN},
                {{3300, 420, 100, 25}, BROWN},
                {{3650, 500, 150, 25}, BROWN},
            },
            {
                {{1110, 390, 30, 30}, mushroomTexture, 250.0f, (Vector2){900, 1120}, 1, 1},
                {{1610, 470, 30, 30}, mushroomTexture, 250.0f, (Vector2){1600, 2000}, 1, 1},
                {{1810, 470, 30, 30}, mushroomTexture, 250.0f, (Vector2){1600, 2000}, -1, 1},

            },
            {
                {{1200, 360, 30, 30}, LoadTexture("./images/coin.png"), 1},
                {{1420, 300, 30, 30}, LoadTexture("./images/coin.png"), 1},
                {{1530, 440, 30, 30}, LoadTexture("./images/coin.png"), 1},
                {{2250, 370, 30, 30}, LoadTexture("./images/coin.png"), 1},
                {{2575, 300, 30, 30}, LoadTexture("./images/coin.png"), 1},
            },
            3750,

        },
        // LEVEL 4
        {
            {
                {{0, 0, 2300, 520}, BLANK},
                {{0, 500, 1500, 25},BROWN},
                {{950, 420, 150, 25}, BROWN},
                {{1600, 500, 400, 25}, BROWN},
                {{2150, 420, 150, 25}, BROWN},
                {{2550, 350, 50, 25}, BROWN},
                {{2700, 500, 200, 25}, BROWN},
            },
            {
                {{1110, 470, 30, 30}, mushroomTexture, 250.0f, (Vector2){1000, 1400}, 1, 1},

                {{1610, 470, 30, 30}, mushroomTexture, 250.0f, (Vector2){1600, 1970}, 1, 1},
                {{1810, 470, 30, 30}, mushroomTexture, 250.0f, (Vector2){1600, 1970}, -1, 1},
            },
            {
                {{1260, 380, 30, 30}, LoadTexture("./images/coin.png"), 1},
                {{1400, 320, 30, 30}, LoadTexture("./images/coin.png"), 1},
                {{1500, 460, 30, 30}, LoadTexture("./images/coin.png"), 1},
                {{2250, 370, 30, 30}, LoadTexture("./images/coin.png"), 1},
                {{2575, 300, 30, 30}, LoadTexture("./images/coin.png"), 1},
            },
            2850,
        },
        // LEVEL 5
        {
            {
                {{0, 0, 2500, 520}, BLANK},

                {{0, 480, 1000, 35}, BROWN},
                
                {{1200, 480, 500, 35}, BROWN},

                
                {{1900, 410, 100, 25}, BROWN},
                {{2300, 480, 600, 35}, BROWN},
                
                {{3150, 410, 100, 25}, BROWN},
                {{3500, 410, 100, 25}, BROWN},
                {{3850, 410, 100, 25}, BROWN},
                {{4200, 410, 100, 25}, BROWN},
                {{4600, 480, 400, 35}, BROWN},

            },
            {
                {{1610, 450, 30, 30}, mushroomTexture, 250.0f, (Vector2){1200, 1670}, 1, 1},
                {{1210, 450, 30, 30}, mushroomTexture, 250.0f, (Vector2){1200, 1670}, -1, 1},
                {{2310, 450, 30, 30}, mushroomTexture, 250.0f, (Vector2){2300, 2870}, 1, 1},
                {{2870, 450, 30, 30}, mushroomTexture, 250.0f, (Vector2){2300, 2870}, -1, 1},
                {{2500, 450, 30, 30}, mushroomTexture, 250.0f, (Vector2){2300, 2870}, -1, 1},
            },
            {
                {{1200, 290, 30, 30}, LoadTexture("./images/coin.png"), 1},
                {{1530, 340, 30, 30}, LoadTexture("./images/coin.png"), 1},
                {{2250, 270, 30, 30}, LoadTexture("./images/coin.png"), 1},
                {{3200, 340, 30, 30}, LoadTexture("./images/coin.png"), 1},
                {{3550, 340, 30, 30}, LoadTexture("./images/coin.png"), 1},
                {{3900, 340, 30, 30}, LoadTexture("./images/coin.png"), 1},
                {{4250, 340, 30, 30}, LoadTexture("./images/coin.png"), 1},
            },
            4999,
        }
    };

    int currentLevel = 0;
    int playerLives = 3;
    Level currentLevelData = levels[currentLevel];

    Camera2D camera = {0};
    camera.target = (Vector2){player.rect.x, player.rect.y};
    camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    SetTargetFPS(60);

    Heart hearts[MAX_HEARTS];

    for (int i = 0; i < MAX_HEARTS; i++) {
        hearts[i].texture = heartTexture;
        hearts[i].position = (Vector2){screenWidth/2 + 60 + i * 40, screenHeight / 2 - 100};  // Adjust the positions as needed
    }

    int collectedCoins = 0;

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        switch (gameState)
        {
        case STATE_MENU:
            if (CheckCollisionPointRec(GetMousePosition(), startbuttonrect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                PlaySound(levelcompletesound);
                gameState = STATE_LOADING;
            }
            if (CheckCollisionPointRec(GetMousePosition(), exitbuttonrect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                CloseAudioDevice();
                CloseWindow();
            }
            if (CheckCollisionPointRec(GetMousePosition(), faqbuttonrect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                gameState = STATE_HOWTOPLAY;
            if (CheckCollisionPointRec(GetMousePosition(), infobuttonrect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                gameState = STATE_INFO;
                    

            BeginDrawing();
            // ClearBackground(BLACK);
            DrawTexture(spacebg, 0, 0, WHITE);
            DrawText("The Adventures of Pookie", 125, screenHeight / 2 - 100, 40, WHITE);
            if (CheckCollisionPointRec(GetMousePosition(), startbuttonrect))
                DrawTexture(startbuttonact, screenWidth / 2 - 100, screenHeight / 2 - 5, WHITE);
            else
                DrawTexture(startbutton, screenWidth / 2 - 100, screenHeight / 2 - 5, WHITE);

            if (CheckCollisionPointRec(GetMousePosition(), exitbuttonrect))
                DrawTexture(exitbuttonact, screenWidth / 2 - 100, screenHeight / 2 + 50, WHITE);
            else
                DrawTexture(exitbutton, screenWidth / 2 - 100, screenHeight / 2 + 50, WHITE);
            
            if (CheckCollisionPointRec(GetMousePosition(), infobuttonrect))
                DrawTexture(infobuttonact, screenWidth - 100, screenHeight - 75, WHITE);
            else
                DrawTexture(infobutton, screenWidth - 100, screenHeight - 75, WHITE);

            if (CheckCollisionPointRec(GetMousePosition(), faqbuttonrect))
                DrawTexture(faqbuttonact, 50, screenHeight - 75, WHITE);
            else
                DrawTexture(faqbutton, 50, screenHeight - 75, WHITE);
            
            EndDrawing();
            break;

        case STATE_LOADING:
            countdownTimer -= deltaTime;

            BeginDrawing();
            DrawTexture(spacebg, 0, 0, WHITE);
            char str[100];
            strcpy(str, "Loading Level: ");
            sprintf(str + strlen(str), "%d", currentLevel + 1);
            DrawText(str, screenWidth / 3, screenHeight / 2 - 50, 30, WHITE);

            if (countdownTimer > 0) {
                char countdownStr[10];
                sprintf(countdownStr, "%d", (int)countdownTimer + 1);
                DrawText(countdownStr, screenWidth / 2 - 10, screenHeight / 2, 50, WHITE);
            } else if (!countdownDisplayed) {
                countdownDisplayed = 1;
                gameState = STATE_GAME;
                player.rect.x = 150;
                player.rect.y = 300;
            }

            EndDrawing();
            break;

        case STATE_HOWTOPLAY:
            if (CheckCollisionPointRec(GetMousePosition(), returnbuttonrect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                gameState = STATE_MENU;
            BeginDrawing();
            DrawTexture(spacebg, 0, 0, WHITE);
            DrawText("HOW TO PLAY:\n", screenWidth / 2 - 190, 40, 50, BLUE);
            DrawText("Welcome to The Adventures of Pookie!\n\n\n-> Your objective is to complete all the levels while\n\ncollecting all the coins and killing the enemies.\n\n-> Use the arrow keys to move left and right and \n\nspace key to jump.\n\n-> Jump on top of the enemies to kill them.\n\n-> You will die if you fall out of the\n\nmap or get hit by an enemy.", 10, 100, 30,WHITE);
            DrawText("Good Luck!", screenWidth / 2 - 120, screenHeight - 80, 40, BLUE);
            if (CheckCollisionPointRec(GetMousePosition(), returnbuttonrect))
                DrawTexture(returnbuttonact, screenWidth - 100, screenHeight - 75, WHITE);
            else
                DrawTexture(returnbutton, screenWidth - 100, screenHeight - 75, WHITE);

            EndDrawing();
            break;

        case STATE_INFO:
            if (CheckCollisionPointRec(GetMousePosition(), returnbuttonrect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                gameState = STATE_MENU;
            BeginDrawing();
            DrawTexture(spacebg, 0, 0, WHITE);
            DrawText("ABOUT THE GAME:\n", screenWidth / 2 - 240, 60, 50, BLUE);
            DrawText("Welcome to The Adventures of Pookie!\n\n\n This game was made by Muhammad Rafey from\n\nBSCS-13-B as a project for FOCP. Hope you enjoy\n\nyour time while playing the game! ", 10, 150, 30,WHITE);
            
            if (CheckCollisionPointRec(GetMousePosition(), returnbuttonrect))
                DrawTexture(returnbuttonact, screenWidth - 100, screenHeight - 75, WHITE);
            else
                DrawTexture(returnbutton, screenWidth - 100, screenHeight - 75, WHITE);

            EndDrawing();
            break;

        case STATE_DEATH:
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("You Died. ", screenWidth / 2 - 60, screenHeight / 2, 30, WHITE);
            EndDrawing();
            sleep(2);
            player.rect.x = 150;
            player.rect.y = 280;
            gameState = STATE_GAME;
            break;

        case STATE_GAMEOVER:
            BeginDrawing();
            DrawTexture(spacebg, 0, 0, WHITE);
            DrawText("Game Over! You lost all your lives. ", screenWidth / 2 - 200, screenHeight / 2, 25, WHITE);
            EndDrawing();
            sleep(2);
            CloseAudioDevice();
            CloseWindow();
            return main();

        case STATE_GAMEWON:
            const char *sstr = TextFormat("Coins: %d", collectedCoins);
            
            PlaySound(winsound);
            BeginDrawing();
            DrawTexture(spacebg, 0, 0, WHITE);
            DrawText("Congratulations!", screenWidth / 2 - 150, screenHeight / 2, 25, WHITE);
            DrawText("You Completed the Game!", screenWidth / 2 - 170, screenHeight / 2 + 35, 25, WHITE);
            DrawText(sstr, screenWidth / 2 - 50, screenHeight / 2 + 70, 30, YELLOW);
            EndDrawing();
            sleep(7);
            CloseAudioDevice();
            CloseWindow();
            return main();


        case STATE_GAME:
            UpdatePlayer(&player, currentLevelData.envItems, MAX_ENV_ITEMS, deltaTime, player.rect, currentLevelData.mushrooms, MAX_MUSHROOMS, hearts, MAX_HEARTS, gameState, &playerdirection, jumpsound, deathsound, kicksound, &dead, &playerLives);
            updateCamera(&camera, &player, currentLevelData.envItems, MAX_ENV_ITEMS, deltaTime, screenWidth, screenHeight);
            UpdateMushrooms(currentLevelData.mushrooms, MAX_MUSHROOMS, deltaTime, currentLevel);

            BeginDrawing();
            ClearBackground(RAYWHITE);

            //BACKGROUNDS______________________________________________
            
            if (currentLevel == 1)
                DrawTexture(background2, 0, 0, WHITE);
            else if (currentLevel == 4)
                DrawTexture(background3, 0, 0, WHITE);
            else
                DrawTexture(background, 0, 0, WHITE);

            DrawTexture(platform, 0, 500, WHITE);
            BeginMode2D(camera);

            //PLATFORMS________________________________________________

            for (int i = 1; i < MAX_ENV_ITEMS; i++)
            {
                EnvItem *ei = currentLevelData.envItems + i;
                platform.height = ei->rect.height;
                platform.width = ei->rect.width;
                platform2.height = ei->rect.height;
                platform2.width = ei->rect.width;
                DrawRectangleRec(ei->rect, ei->color);
                
                
                if (currentLevel == 4)
                    DrawTexture(platform2, ei->rect.x, ei->rect.y, WHITE);
                else if (currentLevel == 1)
                {
                    if (ei->rect.y < 460)
                        DrawTexture(platform2, ei->rect.x, ei->rect.y, WHITE);
                    if (player.rect.y >= 450)
                        player.rect.y += 50;
                }
                else
                    DrawTexture(platform, ei->rect.x, ei->rect.y, WHITE);
                    
            }

            // Check collisions with coins
            for (int i = 0; i < MAX_COINS; i++)
            {
                Coin *coin = &currentLevelData.coins[i];
                coin->texture.height = coin->rect.height;
                coin->texture.width = coin->rect.width;

                if (coin->isActive && CheckCollisionRecs(player.rect, coin->rect))
                {
                    coin->isActive = 0;
                    collectedCoins++;
                    UnloadTexture(coin->texture);
                    PlaySound(coinsound);
                }

                if (coin->isActive)
                    DrawTexture(coin->texture, coin->rect.x, coin->rect.y, WHITE);
            }

            for (int i = 0; i < MAX_MUSHROOMS; i++)
            {
                Mushroom *mushroom = &currentLevelData.mushrooms[i];
                DrawTexture(mushroom->texture, mushroom->rect.x, mushroom->rect.y, WHITE);
                UpdateMushrooms(currentLevelData.mushrooms, MAX_MUSHROOMS, deltaTime, currentLevel);

            }

            //HUD________________________________________________________

            // Draw hearts
            for (int i = 0; i < MAX_HEARTS; i++) {
                hearts[i].position = (Vector2){player.rect.x + 10 + i * 10, player.rect.y - 20};  // Adjust the positions as needed

                if (i < playerLives) {
                    DrawTexture(hearts[i].texture, hearts[i].position.x, hearts[i].position.y, WHITE);
                }
            }
            DrawTexture(cointexture, camera.target.x + 150, 100, WHITE);
            const char *coinsstr = TextFormat("Coins: %d", collectedCoins);
            DrawText(coinsstr, camera.target.x + 200, 100, 30, YELLOW);

            if (player.isJumping) {
                if (playerdirection)
                    DrawTextureRec(player.sprite.texture, player.sprite.frameRec, (Vector2){player.rect.x, player.rect.y}, WHITE);
                else
                    DrawTextureRec(player.sprite.texture2, player.sprite.frameRec, (Vector2){player.rect.x, player.rect.y}, WHITE);
                } 
            else {
                if (playerdirection)
                    DrawTextureRec(player.sprite.texture, player.sprite.frameRec, (Vector2){player.rect.x, player.rect.y}, WHITE);
                else
                    DrawTextureRec(player.sprite.texture2, player.sprite.frameRec, (Vector2){player.rect.x, player.rect.y}, WHITE);
            }
            if(IsKeyDown(KEY_SPACE)  && player.canJump)
                PlaySound(jumpsound);



            if (player.rect.y > screenHeight + 75)
            {
                // Player has fallen out of the map
                playerLives--;
                PlaySound(deathsound);
                if (playerLives > 0)
                    dead = 1;
                else
                    gameState = STATE_GAMEOVER;
            }
            if (currentLevel == 4 && player.rect.x >= currentLevelData.finishLine)
                gameState = STATE_GAMEWON;
            else if (player.rect.x >= currentLevelData.finishLine)
            {
                player.rect.x = 150;
                player.rect.y = 280;
                currentLevel++;
                currentLevelData = levels[currentLevel];
                if (currentLevel == 4)
                {
                    G = 450;
                    PLAYER_JUMP_SPD = 350;
                    PLAYER_HOR_SPD = 250;
                }
                
                countdownTimer = COUNTDOWN_DURATION;
                countdownDisplayed = 0;
                PlaySound(levelcompletesound);
                gameState = STATE_LOADING;
            }
            if (dead)
            {
                // sleep(1);
                dead = 0;
                player.rect.x = 150;
                player.rect.y = 280;
            }
            EndMode2D();
            EndDrawing();

            break;
        }
    }
    CloseAudioDevice();
    CloseWindow();
    printf("Collected Coins: %d\n", collectedCoins);
    return 0;
}

void updateCamera(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height)
{
    camera->target = (Vector2){player->rect.x, player->rect.y};
    camera->offset = (Vector2){width / 2.0f, height / 2.0f};
    float minX = 1000, minY = 1000, maxX = -1000, maxY = -1000;

    for (int i = 0; i < envItemsLength; i++)
    {
        EnvItem *ei = envItems + i;
        minX = fminf(ei->rect.x, minX);
        maxX = fmaxf(ei->rect.x + ei->rect.width, maxX);
        minY = fminf(ei->rect.y, minY);
        maxY = fmaxf(ei->rect.y + ei->rect.height, maxY);
    }

    Vector2 max = GetWorldToScreen2D((Vector2){maxX, maxY}, *camera);
    Vector2 min = GetWorldToScreen2D((Vector2){minX, minY}, *camera);

    if (max.x < width)
        camera->offset.x = width - (max.x - width / 2);
    if (max.y < height)
        camera->offset.y = height - (max.y - height / 2);
    if (min.x > 0)
        camera->offset.x = width / 2 - min.x;
    if (min.y > 0)
        camera->offset.y = height / 2 - min.y;
}

void UpdatePlayer(Player *player, EnvItem *envItems, int envItemsLength, float delta, Rectangle playerRect, Mushroom *mushrooms, int numMushrooms, Heart *hearts, int numHearts, GameState gameState, int *direction, Sound jumpsound, Sound deathsound, Sound kicksound, int *dead, int *lives) 
{    
    if (gameState == STATE_GAME)
    {
        if (IsKeyDown(KEY_LEFT))
            player->rect.x -= PLAYER_HOR_SPD * delta;
        if (IsKeyDown(KEY_RIGHT))
            player->rect.x += PLAYER_HOR_SPD * delta;
        if (IsKeyDown(KEY_SPACE) && player->canJump)
        {
            player->speed = -PLAYER_JUMP_SPD;
            player->canJump = false;
            player->isJumping = true;
        }
            
        if ((IsKeyPressed(KEY_LEFT)))
            *direction = 0;
        if ((IsKeyPressed(KEY_RIGHT)))
            *direction = 1;

        if ((IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT)) && !player->isJumping)
        {
            player->sprite.currentTime += delta * 2;
            if (player->sprite.currentTime >= MARIO_ANIMATION_SPEED)
            {
                player->sprite.frame++;
                player->sprite.currentTime = 0.0f;
                if (player->sprite.frame > MARIO_FRAMES - 1)
                {
                    player->sprite.frame = 0;
                }
                player->sprite.frameRec.x = player->sprite.frame * player->sprite.frameRec.width;
            }
        }
        else
        {
            player->sprite.frame = 0;
            player->sprite.currentTime = 0.0f;
            player->sprite.frameRec.x = 0;
        }
        
        if (player->isJumping)
        {
            
            if (*direction)
                DrawTextureRec(player->sprite.texture, player->sprite.frameRec, (Vector2){player->rect.x, player->rect.y}, WHITE);
            else
                DrawTextureRec(player->sprite.texture2, player->sprite.frameRec, (Vector2){player->rect.x, player->rect.y}, WHITE);
        }
        else
        {
            if (*direction)
                DrawTextureRec(player->sprite.texture, player->sprite.frameRec, (Vector2){player->rect.x, player->rect.y}, WHITE);
            else
                DrawTextureRec(player->sprite.texture2, player->sprite.frameRec, (Vector2){player->rect.x, player->rect.y}, WHITE);
        }

        int hitObstacle = 0;
        for (int i = 0; i < envItemsLength; i++)
        {
            EnvItem *ei = envItems + i;
            Vector2 *p = &((Vector2){player->rect.x, player->rect.y});
            if (
                ei->rect.x - player->rect.width<= p->x &&
                ei->rect.x + ei->rect.width + 50  - player->rect.width>= p->x &&
                ei->rect.y - player->rect.height >= p->y &&
                ei->rect.y - player->rect.height <= p->y + player->speed * delta)
            {
                hitObstacle = 1;
                player->speed = 0.0f;
                p->y = ei->rect.y;
                player->isJumping = false;
            }
        }

        if (!hitObstacle)
        {
            player->rect.y += player->speed * delta;
            player->speed += G * delta;
            player->canJump = false;
        }
        else
            player->canJump = true;

        // Check collisions with mushrooms
        for (int i = 0; i < numMushrooms; i++)
        {
            Mushroom *mushroom = &mushrooms[i];
            
            // Check if player is colliding with the mushroom
            if (CheckCollisionRecs(playerRect, mushroom->rect))
            {
                // Check if the collision is from the top
                if (player->rect.y <= mushroom->rect.y - mushroom->rect.height && mushroom->isActive)
                {
                    mushroom->direction = 0;
                    mushroom->speed = 0.0f;
                    mushroom->rect.height = 10;
                    
                    mushroom->rect.y += 20;
                    mushroom->rect.height = 10;
                    mushroom->texture.height = 10;

                    player->rect.y = mushroom->rect.y - playerRect.height;
                    player->speed = 0.0f;
                    player->canJump = true;
                    player->isJumping = false;
                    mushroom->isActive = 0;
                    PlaySound(kicksound);
                    
                }
                else if (mushroom->isActive)
                {
                    player->isJumping = false;
                    *dead = 1;
                    player->rect.y += 100;
                    PlaySound(deathsound);
                }
            }
            if (mushroom->rect.y > 500)
                mushroom->rect.y += 20;
        }

        
    }
}

void UpdateMushrooms(Mushroom *mushrooms, int numMushrooms, float delta, int currentLevel)
{
    for (int i = 0; i < numMushrooms; i++)
    {
        if (mushrooms[i].isActive)
        {
            // Update mushroom position
            if (currentLevel < 4)
                mushrooms[i].rect.x += mushrooms[i].speed * delta * mushrooms[i].direction * 0.1;
            else
                mushrooms[i].rect.x += mushrooms[i].speed * delta * mushrooms[i].direction * 0.07;

            // Check if mushroom is out of bounds and change direction if necessary
            if (mushrooms[i].rect.x < mushrooms[i].xBounds.x || mushrooms[i].rect.x > mushrooms[i].xBounds.y)
            {
                mushrooms[i].direction *= -1;
            }
            
        }
        
    }
}

void LoadNextLevel(Level *levels, int *currentLevel, Level *currentLevelData)
{
    (*currentLevel)++;
    
    if (*currentLevel < 6)
    {
        *currentLevelData = levels[*currentLevel];
        printf("Loading Level: %d\n", *currentLevel + 1);
    }
    else
    {
        printf("Congratulations! You completed all levels.\n");
        CloseWindow();
        exit(0);
    }
}