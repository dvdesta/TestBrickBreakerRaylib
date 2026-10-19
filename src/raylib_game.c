/*******************************************************************************************
*
*   raylib game template
*
*
*   Code licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2021-2026 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include <raylib.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

#define FILAS 10
#define COLUMNAS 6

typedef struct {
    Rectangle rect;
    bool activo;
} Ladrillo;

typedef struct {
    Rectangle EstructuraRectangulo;
} TipoBarra;

// Variables globales para que el bucle web pueda acceder a ellas
static Music musicaFondo = { 0 };
static Sound sonidoWin = { 0 };
static Sound sonidoLose = { 0 };
static Sound sonidoReboteLadrillo = { 0 };
static Sound sonidoReboteBarra = { 0 };

static int anchoLadrillo = 60, altoLadrillo = 20;
static int espacio = 10, frames = 0, contador = 0;
static int vidas = 3, puntuacion = 0;
static Vector2 PosicionPelota = {320.0f, 760.0f};
static Vector2 VelocidadPelota = {6.0f, -6.0f};
static bool gameOver = false;
static bool pausaPorMuerte = false;
static bool ganador = false;
static float RadioPelota = 5.0f;
static bool PantallaInicial = true;
static bool MusicaFondo = true;

static TipoBarra barra;
static Ladrillo ladrillos[FILAS][COLUMNAS];

// Función que ejecuta un frame del juego (necesaria para Emscripten y PC)
static void UpdateDrawFrame(void)
{
    if (IsKeyPressed(KEY_O)){
        MusicaFondo = false;
    }
    if (IsKeyPressed(KEY_I)){
        MusicaFondo = true;
    }

    if (PantallaInicial) {
        if (MusicaFondo){
            UpdateMusicStream(musicaFondo);
            if (!IsMusicStreamPlaying(musicaFondo)) {
                PlayMusicStream(musicaFondo);
            }
        }
        
        BeginDrawing();
            ClearBackground(BLACK);
            DrawText("Presiona 'Y' para video de YouTube :D", 30, 400, 30, GOLD);
            DrawText("Presiona 'E' para iniciar el juego", 30, 440, 30, SKYBLUE);
            DrawText("Presiona 'ESC' para salir del juego", 30, 480, 30, SKYBLUE);
            DrawText("CONTROLES AQUÍ", 30, 700, 30, WHITE);
            DrawText("Left Arrow - Right Arrow    <---", 30, 760, 25, WHITE);
            DrawText("Hold LeftMouseButton == Arrows    <---", 30, 800, 25, WHITE);
            DrawText("Apagar Musica con tecla 'O'", 30, 840, 25, WHITE);
            DrawText("Encender Musica con tecla 'I'", 30, 880, 25, WHITE);
            
            if (IsKeyPressed(KEY_Y)){
                OpenURL("https://www.youtube.com/watch?v=yztF4NBuanI");
            }
            if (IsKeyPressed(KEY_E)){
                PantallaInicial = false;
            }
        EndDrawing();
        return;
    }

    // BUCLE PRINCIPAL DEL JUEGO
    if (MusicaFondo && !pausaPorMuerte && !ganador) {
        UpdateMusicStream(musicaFondo);
        if (!IsMusicStreamPlaying(musicaFondo)) {
            PlayMusicStream(musicaFondo);
        }
    }
    
    frames++;
    if(frames >= 60){
        contador++;
        frames = 0;
    }
    
    if (!pausaPorMuerte && !ganador) {
        PosicionPelota.x += VelocidadPelota.x;
        PosicionPelota.y += VelocidadPelota.y;
       
        if (IsKeyDown(KEY_LEFT) && barra.EstructuraRectangulo.x >= 0.0f) barra.EstructuraRectangulo.x -= 7.0f;
        if (IsKeyDown(KEY_RIGHT) && barra.EstructuraRectangulo.x <= 640.0f - barra.EstructuraRectangulo.width) barra.EstructuraRectangulo.x += 7.0f;
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 touchPos = GetMousePosition();
            barra.EstructuraRectangulo.x = touchPos.x - (barra.EstructuraRectangulo.width / 2.0f);
        }
       
        if (CheckCollisionCircleRec(PosicionPelota, RadioPelota, barra.EstructuraRectangulo)){
            PlaySound(sonidoReboteBarra);
            float centroBarra = barra.EstructuraRectangulo.x + (barra.EstructuraRectangulo.width / 2.0f);
            float distanciaX = PosicionPelota.x - centroBarra;
            VelocidadPelota.x = distanciaX * 0.3f; 
            VelocidadPelota.y *= -1;
        }
        if (PosicionPelota.x - RadioPelota <= 0) VelocidadPelota.x *= -1;
        if (PosicionPelota.x + RadioPelota >= 640) VelocidadPelota.x *= -1;
        if (PosicionPelota.y - RadioPelota <= 0) VelocidadPelota.y *= -1;
       
        if (PosicionPelota.y >= 960.0f) {
            vidas -= 1;
            pausaPorMuerte = true;
            if (MusicaFondo){
                PauseMusicStream(musicaFondo);
            }
            PlaySound(sonidoLose);
            if (vidas <= 0) {
                gameOver = true;
            }
        }
       
        if (puntuacion == 60000){
            ganador = true;
            if (MusicaFondo){
                PauseMusicStream(musicaFondo);
            }
            PlaySound(sonidoWin);
        }
    }
        
    BeginDrawing();
        ClearBackground(BLACK);
        
        if (pausaPorMuerte && !gameOver && !ganador) {
            DrawText("Perdiste una vida", 140, 440, 40, RED);
            DrawText("Presiona 'R' para reintentar", 30, 520, 40, SKYBLUE);
            if (IsKeyPressed(KEY_R)) {
                if (MusicaFondo){
                    ResumeMusicStream(musicaFondo);
                }
                PosicionPelota.x = 320.0f;
                PosicionPelota.y = 760.0f; 
                VelocidadPelota.y = -6.0f;
                barra.EstructuraRectangulo.width += 15;
                pausaPorMuerte = false;    
            }
        }
        
        if (gameOver && !ganador) {
            DrawText("Te quedaste sin vidas", 70, 440, 40, RED);
            DrawText("Presiona 'R' para volver a jugar", 30, 520, 35, SKYBLUE);
            
            if (IsKeyPressed(KEY_R)) {
                for (int i = 0; i < FILAS; i++) {
                    for (int j = 0; j < COLUMNAS; j++) {
                        ladrillos[i][j].activo = true;
                    }
                }
                if (MusicaFondo){
                    ResumeMusicStream(musicaFondo);
                }
                PosicionPelota.x = 320.0f;
                PosicionPelota.y = 760.0f;
                VelocidadPelota.y = -6.0f;
                puntuacion = 0;
                vidas = 3;
                barra.EstructuraRectangulo.width = 60.0f;
                pausaPorMuerte = false;
                gameOver = false; 
            }
        }
        
        DrawText("PUNTUACION :", 20, 830, 30, WHITE);
        DrawText(TextFormat(" %d", puntuacion), 250, 830, 30, YELLOW);
        
        DrawText("Vidas Restantes :", 20, 860, 30, WHITE);
        DrawText(TextFormat(" %d", vidas), 300, 860, 30, RED);
        
        DrawText("Tiempo Perdido en este juego :", 20, 900, 30, WHITE);
        DrawText(TextFormat(" %d", contador), 500, 900, 30, GOLD);
        
        DrawCircleV(PosicionPelota, RadioPelota, WHITE);
        DrawRectangleRec(barra.EstructuraRectangulo, GREEN);
        
        for (int i = 0; i < FILAS; i++){
            for (int j = 0; j < COLUMNAS; j++){
                if (CheckCollisionCircleRec(PosicionPelota, RadioPelota, ladrillos[i][j].rect) && ladrillos[i][j].activo) {
                    puntuacion += 1000;
                    PlaySound(sonidoReboteLadrillo);
                    
                    if (PosicionPelota.x >= ladrillos[i][j].rect.x && PosicionPelota.x <= ladrillos[i][j].rect.x + ladrillos[i][j].rect.width) {
                        VelocidadPelota.y *= -1;
                    } 
                    else {
                        VelocidadPelota.x *= -1;
                    }
                    ladrillos[i][j].activo = false;
                }
                
                if(ladrillos[i][j].activo){
                    DrawRectangleRec(ladrillos[i][j].rect, YELLOW);
                }
            }
        }
        
        if (ganador){
            DrawText("¡¡GANASTE FELICATIONES!!", 40, 240, 40, GOLD);
            DrawText("Presiona 'R' para volver a jugar", 30, 440, 35, SKYBLUE);
            DrawText("Presiona 'ESC' para salirte", 30, 500, 35, SKYBLUE);
            if (IsKeyPressed(KEY_R)) {
                if (MusicaFondo){
                    ResumeMusicStream(musicaFondo);
                }
                for (int i = 0; i < FILAS; i++) {
                    for (int j = 0; j < COLUMNAS; j++) {
                        ladrillos[i][j].activo = true;
                    }
                }
                PosicionPelota.x = 320.0f;
                PosicionPelota.y = 760.0f;
                VelocidadPelota.y = -6.0f;
                puntuacion = 0;
                vidas = 3;
                barra.EstructuraRectangulo.width = 60.0f;
                ganador = false;
            }
        }
    EndDrawing();
}

int main(void) {
    InitWindow(640, 960, "Mi Segundo Juego");
    InitAudioDevice();

    // Cargar recursos con la ruta de la carpeta resources/ para la web
    musicaFondo = LoadMusicStream("resources/FondoBrick.mp3");
    sonidoWin = LoadSound("resources/WinBricks.mp3");
    sonidoLose = LoadSound("resources/LoseBricks.mp3");
    sonidoReboteLadrillo = LoadSound("resources/click.wav");
    sonidoReboteBarra = LoadSound("resources/hitHurt.wav");

    barra.EstructuraRectangulo.x = 275.0f;
    barra.EstructuraRectangulo.y = 800.0f;
    barra.EstructuraRectangulo.width = 60.0f;
    barra.EstructuraRectangulo.height = 15.0f;
    
    for (int i = 0; i < FILAS; i++){
        for (int j = 0; j < COLUMNAS; j++){
            ladrillos[i][j].rect.x = 100 + j * (anchoLadrillo + espacio);
            ladrillos[i][j].rect.y = 50 + i * (altoLadrillo + espacio);
            ladrillos[i][j].rect.width = anchoLadrillo;
            ladrillos[i][j].rect.height = altoLadrillo;
            ladrillos[i][j].activo = true;
        }
    }
    
    PlayMusicStream(musicaFondo);

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        UpdateDrawFrame();
    }
#endif

    UnloadSound(sonidoReboteLadrillo);
    UnloadSound(sonidoReboteBarra);
    UnloadSound(sonidoWin);
    UnloadSound(sonidoLose);
    UnloadMusicStream(musicaFondo);
    CloseAudioDevice();
    CloseWindow();
    
    return 0;
}
    //----------------------------------------------------------------------------------
}
