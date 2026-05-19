#include "space_game.h"

#include "../common/shared_data.h"

#include <math.h>

// =================================================
// ===================== SHIP ======================
// =================================================

static float ship_y = 120.0f;

static const int ship_x = 30;

struct Bullet
{
    float x;
    float y;

    bool active;
};


static Bullet bullets[5];
// =================================================
// ===================== INIT ======================
// =================================================

void initSpaceGame()
{
    ship_y = 120.0f;
    for(int i = 0; i < 5; i++)
    {
        bullets[i].active = false;
    }
}

// =================================================
// ==================== UPDATE =====================
// =================================================

void updateSpaceGame()
{
    // =============================================
    // ================= IMU =======================
    // =============================================

    float ay =
        g_imu_data.ay;

    // =============================================
    // ================= MOVE ======================
    // =============================================

    // ship_y += (ay * 20.0f);
    float move = ay;

// deadzone

    if(fabs(move) < 0.08f)
    {
        move = 0;
    }

    ship_y += (move * 20.0f);

    // =============================================
    // ================= LIMIT =====================
    // =============================================

    if(ship_y < 20)
    {
        ship_y = 20;
    }

    if(ship_y > 220)
    {
        ship_y = 220;
    }
    for(int i = 0; i < 5; i++)
    {
        if(bullets[i].active)
        {
            bullets[i].x += 10;

            if(bullets[i].x > 320)
            {
                bullets[i].active = false;
            }
        }
    }
}

// =================================================
// ===================== DRAW ======================
// =================================================

void drawSpaceGame(
    LGFX_Sprite* sprite
)
{
    // =============================================
    // ================= UPDATE ====================
    // =============================================

    updateSpaceGame();

    // =============================================
    // ================= TITLE =====================
    // =============================================

    sprite->setTextColor(TFT_CYAN);

    sprite->setTextSize(2);

    sprite->setCursor(80, 10);

    sprite->print("SPACE GAME");

    // =============================================
    // ================= STARS =====================
    // =============================================

    for(int i = 0; i < 40; i++)
    {
        int x = rand() % 320;

        int y = rand() % 240;

        sprite->drawPixel(
            x,
            y,
            TFT_WHITE
        );
    }

    // =============================================
    // ================= SHIP ======================
    // =============================================

    sprite->fillRect(ship_x, ship_y-6, 20, 12, 0x07FF);
    sprite->fillTriangle(ship_x+20, ship_y-6, ship_x+20, ship_y+6, ship_x+30, ship_y, 0x07FF);
    sprite->fillRect(ship_x+4, ship_y-12, 8, 8, 0x001F);
    sprite->fillRect(ship_x+4, ship_y+4, 8, 8, 0x001F);

    bool flame =
    (xTaskGetTickCount() / 10) % 2;
    if(flame)
    {
        sprite->fillTriangle(
            ship_x - 1,
            ship_y - 3,

            ship_x - 1,
            ship_y + 3,

            ship_x - 8,
            ship_y,

            TFT_ORANGE
        );
    }
    else
    {
        sprite->fillTriangle(
            ship_x - 1,
            ship_y - 2,

            ship_x - 1,
            ship_y + 2,

            ship_x - 5,
            ship_y,

            TFT_RED
        );
    }

    // sprite->fillTriangle(
    //     ship_x,
    //     ship_y,

    //     ship_x + 30,
    //     ship_y - 10,

    //     ship_x + 30,
    //     ship_y + 10,

    //     TFT_CYAN
    // );

    // sprite->fillRect(
    //     ship_x - 10,
    //     ship_y - 4,
    //     10,
    //     8,
    //     TFT_ORANGE
    // );

    for(int i = 0; i < 5; i++)
    {
        if(bullets[i].active)
        {
            sprite->fillRect(
                bullets[i].x,
                bullets[i].y - 1,
                6,
                2,
                TFT_YELLOW
            );
        }
    }
}

void fireBullet()
{
    for(int i = 0; i < 5; i++)
    {
        if(!bullets[i].active)
        {
            bullets[i].active = true;

            bullets[i].x =
                ship_x + 30;

            bullets[i].y =
                ship_y;

            break;
        }
    }
}