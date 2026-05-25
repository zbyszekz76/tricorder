#include "space_game.h"

#include "../audio/audio.h"
// #include "../audio/rttl_task.h"
#include "../common/shared_data.h"

#include <math.h>
#include <stdlib.h>

// =================================================
// ===================== SHIP ======================
// =================================================

enum GameState
{
    GAME_MENU,
    GAME_INTRO,
    GAME_PLAYING,
    GAME_OVER
};

static GameState game_state = GAME_MENU;

bool isGameMenu()
{
    return game_state == GAME_MENU;
}

bool isGamePlaying()
{
    return game_state == GAME_PLAYING;
}

// Aktualna pozycja statku gracza (oś Y)
static float ship_y = 120.0f;

// Stała pozycja X statku
static const int ship_x = 30;

// =================================================
// ==================== BULLETS ====================
// =================================================

// Struktura pojedynczego pocisku
struct Bullet
{
    float x;
    float y;

    bool active;
};

// Tablica pocisków
// Dzięki temu NIE używamy dynamicznej pamięci
static Bullet bullets[5];

// =================================================
// ==================== ENEMIES ====================
// =================================================

// Struktura przeciwnika
struct Enemy
{
    // Pozycja przeciwnika
    float x = 320;
    float y = 0;

    // Czy enemy istnieje
    bool active = false;

    // Czy aktualnie wybucha
    bool exploding = false;

    // Czas trwania eksplozji
    int explosion_timer = 0;
};

// Tablica przeciwników
static Enemy enemies[5];

// =================================================
// =================== INTRO =======================
// =================================================

static float intro_y = 260;

static int intro_timer = 0;

// =================================================
// ===================== INIT ======================
// =================================================

void initSpaceGame()
{
    // =============================================
    // ============== SHIP RESET ===================
    // =============================================

    ship_y = 120.0f;

    // =============================================
    // ============== BULLET RESET =================
    // =============================================

    for(int i = 0; i < 5; i++)
    {
        bullets[i].active = false;
    }

    // =============================================
    // ============== ENEMY RESET ==================
    // =============================================

    for(int i = 0; i < 5; i++)
    {
        enemies[i].active = false;

        enemies[i].exploding = false;

        enemies[i].explosion_timer = 0;
    }
}

// =================================================
// ================= FIRE BULLET ===================
// =================================================

// Funkcja wywoływana po kliknięciu przycisku
void fireBullet()
{
    // Szukamy wolnego pocisku
    for(int i = 0; i < 5; i++)
    {
        if(!bullets[i].active)
        {
            // Aktywacja pocisku
            bullets[i].active = true;

            // Start pocisku z przodu statku
            bullets[i].x = ship_x + 30;

            bullets[i].y = ship_y;

            playLaserSound();

            // Jeden klik = jeden pocisk
            break;
        }
    }
}

// =================================================
// ================= START GAME ====================
// =================================================

void startGame()
{
    // Reset intro

    intro_y = 260;

    intro_timer = 0;

    // Zmiana stanu

    game_state = GAME_INTRO;
}

// =============================================
// ============= INTRO MUSIC FLAG ==============
// =============================================

static bool intro_music_started = false;

// =================================================
// ==================== UPDATE =====================
// =================================================

// Tutaj znajduje się CAŁA logika gry
// WITHOUT rendering
// =================================================
// ==================== UPDATE =====================
// =================================================
//
// GŁÓWNA LOGIKA GRY
//
// Funkcja wykonywana cyklicznie:
// - menu
// - intro
// - gameplay
// - enemy
// - bullets
// - collision
//
// WITHOUT rendering.
//
// =================================================

void updateSpaceGame()
{
    // =============================================
    // ================= MENU ======================
    // =============================================
    //
    // W menu NIE wykonujemy gameplay.
    // Czekamy na klik przycisku.
    //
    // =============================================

    if(game_state == GAME_MENU)
    {
        return;
    }

    // =============================================
    // ================= INTRO =====================
    // =============================================
    //
    // Intro:
    // - scrolling tekstu
    // - RTTL music
    // - brak gameplay
    //
    // =============================================

    if(game_state == GAME_INTRO)
    {
        // =========================================
        // ============ START MUSIC ================
        // =========================================

        if(!intro_music_started)
        {
            intro_music_started = true;

            playIntroMusic();
        }

        // =========================================
        // ============== INTRO SCROLL =============
        // =========================================

        intro_y -= 1.2f;

        intro_timer++;

        // =========================================
        // ============== END INTRO ================
        // =========================================

        if(intro_timer > 400)
        {
            // Reset flagi

            intro_music_started = false;

            // Start gameplay

            game_state = GAME_PLAYING;

            initSpaceGame();
        }

        return;
    }

    // =============================================
    // ================= IMU =======================
    // =============================================
    //
    // Odczyt osi Y akcelerometru.
    //
    // =============================================

    float ay = g_imu_data.ay;

    // =============================================
    // =============== DEADZONE ===================
    // =============================================
    //
    // Eliminacja mikrodrgań IMU.
    //
    // =============================================

    float move = ay;

    if(fabs(move) < 0.08f)
    {
        move = 0;
    }

    // =============================================
    // ================= SHIP ======================
    // =============================================
    //
    // Sterowanie statkiem:
    // ruch góra/dół.
    //
    // =============================================

    ship_y += (move * 20.0f);

    // =============================================
    // ================= LIMIT =====================
    // =============================================
    //
    // Ograniczenie ruchu statku do ekranu.
    //
    // =============================================

    if(ship_y < 10)
    {
        ship_y = 10;
    }

    if(ship_y > 230)
    {
        ship_y = 230;
    }

    // =============================================
    // ============== BULLET UPDATE ================
    // =============================================
    //
    // Aktualizacja pocisków:
    // - ruch
    // - usuwanie poza ekranem
    //
    // =============================================

    for(int i = 0; i < 5; i++)
    {
        if(bullets[i].active)
        {
            // Ruch pocisku

            bullets[i].x += 10;

            // Usunięcie poza ekranem

            if(bullets[i].x > 320)
            {
                bullets[i].active = false;
            }
        }
    }

    // =============================================
    // =============== ENEMY SPAWN =================
    // =============================================
    //
    // Losowy spawn przeciwników.
    //
    // =============================================

    for(int i = 0; i < 5; i++)
    {
        if((rand() % 120 == 0) && (!enemies[i].active))
        {
            enemies[i].active = true;

            enemies[i].exploding = false;

            // Spawn z prawej strony ekranu

            enemies[i].x = 320;

            // Losowa wysokość

            enemies[i].y = rand() % 200 + 20;
        }
    }

    // =============================================
    // ============== ENEMY UPDATE =================
    // =============================================
    //
    // Aktualizacja enemy:
    // - eksplozja
    // - ruch
    // - usuwanie
    //
    // =============================================

    for(int i = 0; i < 5; i++)
    {
        // =========================================
        // ============== EXPLOSION ================
        // =========================================
        //
        // Enemy w stanie eksplozji:
        // - odliczanie czasu
        // - usunięcie po wybuchu
        //
        // =========================================

        if(enemies[i].active && enemies[i].exploding)
        {
            enemies[i].explosion_timer--;

            if(enemies[i].explosion_timer <= 0)
            {
                enemies[i].active = false;

                enemies[i].exploding = false;
            }
        }

        // =========================================
        // ================= MOVE ===================
        // =========================================
        //
        // Ruch aktywnego enemy.
        //
        // =========================================

        else if(enemies[i].active && !enemies[i].exploding)
        {
            enemies[i].x -= 6;

            // Usunięcie poza ekranem

            if(enemies[i].x < -20)
            {
                enemies[i].active = false;
            }
        }
    }

    // =============================================
    // =============== COLLISION ===================
    // =============================================
    //
    // Detekcja trafień:
    // bullet vs enemy
    //
    // =============================================

    for(int i = 0; i < 5; i++)
    {
        for(int j = 0; j < 5; j++)
        {
            // Collision tylko dla:
            // - aktywnych enemy
            // - nieeksplodujących enemy
            // - aktywnych bullets

            if(enemies[i].active && !enemies[i].exploding && bullets[j].active)
            {
                // =================================
                // ========= HIT TEST ==============
                // =================================
                //
                // Bounding-box collision.
                //
                // =================================

                if((bullets[j].x > enemies[i].x) && (bullets[j].x < enemies[i].x + 12) &&
                   (bullets[j].y > enemies[i].y - 8) && (bullets[j].y < enemies[i].y + 8))
                {
                    // Usunięcie pocisku

                    bullets[j].active = false;

                    // Enemy nadal istnieje podczas eksplozji

                    enemies[i].active = true;

                    // Start eksplozji

                    enemies[i].exploding = true;

                    // Czas eksplozji

                    enemies[i].explosion_timer = 10;

                    // Dźwięk eksplozji

                    playExplosionSound();

                    break;
                }
            }
        }
    }
}

// =================================================
// ===================== DRAW ======================
// =================================================

// Tutaj znajduje się TYLKO rendering
void drawSpaceGame(LGFX_Sprite *sprite)
{
    // =============================================
    // ================= UPDATE ====================
    // =============================================

    // Aktualizacja logiki gry
    updateSpaceGame();

    // =============================================
    // ================= MENU ======================
    // =============================================

    if(game_state == GAME_MENU)
    {
        sprite->setTextColor(TFT_YELLOW);

        sprite->setTextSize(3);

        sprite->setCursor(30, 70);

        sprite->print("SPACE IMPACT");

        // Miganie napisu

        bool blink = (xTaskGetTickCount() / 30) % 2;

        if(blink)
        {
            sprite->setTextSize(2);

            sprite->setCursor(70, 160);

            sprite->print("PRESS BUTTON");
        }

        return;
    }

    // =============================================
    // ================= INTRO =====================
    // =============================================

    if(game_state == GAME_INTRO)
    {
        // Gwiazdy

        for(int i = 0; i < 40; i++)
        {
            int x = rand() % 320;

            int y = rand() % 240;

            sprite->drawPixel(x, y, TFT_WHITE);
        }

        // STAR WARS STYLE TEXT

        sprite->setTextColor(TFT_YELLOW);

        sprite->setTextSize(2);

        sprite->setCursor(30, intro_y);

        sprite->print("A LONG TIME AGO");

        sprite->setCursor(20, intro_y + 40);

        sprite->print("IN A DISTANT");

        sprite->setCursor(40, intro_y + 80);

        sprite->print("GALAXY...");

        sprite->setCursor(10, intro_y + 150);

        sprite->print("SPACE IMPACT");

        return;
    }

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

        sprite->drawPixel(x, y, TFT_WHITE);
    }

    // =============================================
    // ================= SHIP ======================
    // =============================================

    sprite->fillRect(ship_x, ship_y - 6, 20, 12, 0x07FF);

    sprite->fillTriangle(ship_x + 20, ship_y - 6,

                         ship_x + 20, ship_y + 6,

                         ship_x + 30, ship_y,

                         0x07FF);

    sprite->fillRect(ship_x + 4, ship_y - 12, 8, 8, 0x001F);

    sprite->fillRect(ship_x + 4, ship_y + 4, 8, 8, 0x001F);

    // =============================================
    // ================= FLAME =====================
    // =============================================

    bool flame = (xTaskGetTickCount() / 10) % 2;

    if(flame)
    {
        sprite->fillTriangle(ship_x - 1, ship_y - 3,

                             ship_x - 1, ship_y + 3,

                             ship_x - 8, ship_y,

                             TFT_ORANGE);
    }
    else
    {
        sprite->fillTriangle(ship_x - 1, ship_y - 2,

                             ship_x - 1, ship_y + 2,

                             ship_x - 5, ship_y,

                             TFT_RED);
    }

    // =============================================
    // ================= BULLETS ===================
    // =============================================

    for(int i = 0; i < 5; i++)
    {
        if(bullets[i].active)
        {
            sprite->fillRect(bullets[i].x, bullets[i].y - 1, 6, 2, TFT_YELLOW);
        }
    }

    // =============================================
    // ================= ENEMIES ===================
    // =============================================

    for(int i = 0; i < 5; i++)
    {
        if(enemies[i].active)
        {
            // =====================================
            // ============== EXPLOSION ============
            // =====================================

            if(enemies[i].exploding)
            {
                sprite->fillCircle(enemies[i].x + 6, enemies[i].y, 12, TFT_ORANGE);

                sprite->fillCircle(enemies[i].x + 6, enemies[i].y, 8, TFT_YELLOW);

                sprite->fillCircle(enemies[i].x + 6, enemies[i].y, 4, TFT_RED);
            }

            // =====================================
            // ================ ENEMY ==============
            // =====================================

            else
            {
                sprite->fillTriangle(enemies[i].x, enemies[i].y,

                                     enemies[i].x + 12, enemies[i].y - 8,

                                     enemies[i].x + 12, enemies[i].y + 8,

                                     TFT_RED);

                sprite->fillCircle(enemies[i].x + 12, enemies[i].y, 3, TFT_YELLOW);
            }
        }
    }
}