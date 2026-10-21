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
// --- ESTE ENCABEZADO Y CONFIGURACIÓN INICIAL VIENEN DEL TEMPLATE DE RAYLIB ---
// LO DE ARRIBA VENIA EN EL TEMPLATE FECHA 19/10/2026 HORA 20:17

#include <raylib.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

#define FILAS 10
#define COLUMNAS 6
#define MAX 500 //  REPRESENTA LA CANTIDAD DE PARTICULAS MAXIMA PARA QUE NO EXPLOTE

Color coloresFilas[FILAS] = {             //COLORES SACADOS DE INTERNET PORQUE ERA MUY ABURRIDO UNO POR UNO :V
    CLITERAL(Color){ 255, 0, 128, 255 },    // Rosa Neón
    CLITERAL(Color){ 0, 255, 255, 255 },    // Cian Eléctrico
    CLITERAL(Color){ 255, 0, 255, 255 },    // Magenta Puro
    CLITERAL(Color){ 0, 255, 0, 255 },      // Verde Neón (puro)
    CLITERAL(Color){ 255, 255, 0, 255 },    // Amarillo Eléctrico
    CLITERAL(Color){ 255, 100, 0, 255 },    // Naranja Brillante
    CLITERAL(Color){ 128, 0, 255, 255 },    // Violeta Eléctrico
    CLITERAL(Color){ 0, 128, 255, 255 },    // Azul Neón
    CLITERAL(Color){ 255, 50, 50, 255 },    // Rojo Vivo
    CLITERAL(Color){ 50, 255, 150, 255 }    // Verde Menta Neón
};                                         //DEL TAMAÑO DE LAS FILAS EN ORDEN PARA IR CAMBIANDO PORQUE SINO EXPLOTA

typedef struct {
    Rectangle rect;
    bool activo;
} Ladrillo;

typedef struct {
    Rectangle EstructuraRectangulo;
} TipoBarra;

typedef struct{
    Color miColor;
    Vector2 velocidad;
    Vector2 posicion;
    float tiempoVida;
    bool activa;   //PA VER SI LA PARTICULA MURIO O NO
} TipoParticulas;

// --- VARIABLES GLOBALES PARA ACCESO EN EL BUCLE DE EMSCRIPTEN Y PC ---
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
static bool pausaPorMurete = false;
static bool ganador = false;
static float RadioPelota = 5.0f;
static bool PantallaInicial = true;
static bool MusicaFondo = true;

static TipoBarra barra;
static Ladrillo ladrillos[FILAS][COLUMNAS];
static TipoParticulas particulas[MAX]; //CREAR EL VECTOR Y DEJAMOS TODO INACTIVO PARA QUE NO EXPLOTE NUCLEAR

// --- FUNCIÓN QUE EJECUTA CADA FRAME DEL JUEGO PARA WEB Y PC ---
static void UpdateDrawFrame(void)
{
    // --- CONTROL DE MÚSICA POR TECLADO ---
    if (IsKeyPressed(KEY_O)){
        MusicaFondo = false;
    }
    if (IsKeyPressed(KEY_I)){
        MusicaFondo = true;
    }

    // --- LÓGICA Y DIBUJO DE LA PANTALLA INICIAL CON SOPORTE TÁCTIL ---
    if (PantallaInicial) {
        if (MusicaFondo){
            UpdateMusicStream(musicaFondo);
            if (!IsMusicStreamPlaying(musicaFondo)) {
                PlayMusicStream(musicaFondo);
            }
        }
        
        // --- DEFINICIÓN DE RECTÁNGULOS TÁCTILES PARA EL MENÚ PRINCIPAL ---
        Rectangle rectBotonYouTube = { 30, 400, 580, 35 };
        Rectangle rectBotonIniciar = { 30, 440, 450, 35 };
        Rectangle rectBotonMusicaO = { 30, 840, 400, 30 };
        Rectangle rectBotonMusicaI = { 30, 880, 400, 30 };
        
        // --- DETECCIÓN DE CLIC O TOQUE EN PANTALLA TÁCTIL O MOUSE ---
        Vector2 posToqueMenu = { -1, -1 };
        bool tocandoPantalla = false;
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            posToqueMenu = GetMousePosition();
            tocandoPantalla = true;
        } else if (GetTouchPointCount() > 0) {
            posToqueMenu = GetTouchPosition(0);
            tocandoPantalla = true;
        }

        if (IsKeyPressed(KEY_Y) || (tocandoPantalla && CheckCollisionPointRec(posToqueMenu, rectBotonYouTube))) {
            OpenURL("https://www.youtube.com/@Morelloko");
        }
        if (IsKeyPressed(KEY_O) || (tocandoPantalla && CheckCollisionPointRec(posToqueMenu, rectBotonMusicaO))) {
            MusicaFondo = false;
        }
        if (IsKeyPressed(KEY_I) || (tocandoPantalla && CheckCollisionPointRec(posToqueMenu, rectBotonMusicaI))) {
            MusicaFondo = true;
        }
        if (IsKeyPressed(KEY_E) || (tocandoPantalla && CheckCollisionPointRec(posToqueMenu, rectBotonIniciar))) {
            PantallaInicial = false;
        }
        
        BeginDrawing();
            ClearBackground(BLACK);
            DrawText("Si estas en tactil presiona los textos .", 30, 40, 25, WHITE);
            DrawText("En la WEB 'ESC' sirve para marcar los", 30, 80, 25, WHITE);
            DrawText("limites en los que la pelota rebota .", 30, 120, 25, WHITE);
            DrawText("Presiona 'Y' para canal de YouTube :D", 30, 400, 30, GOLD);
            DrawText("Presiona 'E' para iniciar el juego", 30, 440, 30, SKYBLUE);
            DrawText("Presiona 'ESC' para salir del juego", 30, 480, 30, SKYBLUE);
            DrawText("CONTROLES AQUÍ", 30, 700, 30, WHITE);
            DrawText("Left Arrow - Right Arrow    <---", 30, 760, 25, WHITE);
            DrawText("Hold LeftMouseButton == Arrows    <---", 30, 800, 25, WHITE);
            DrawText("Apagar Musica con tecla 'O'", 30, 840, 25, WHITE);
            DrawText("Encender Musica con tecla 'I'", 30, 880, 25, WHITE);
        EndDrawing();
        return;
    }

    // --- ACTUALIZACIÓN DE MÚSICA DURANTE LA PARTIDA ---
    if (MusicaFondo && !pausaPorMurete && !ganador) {
        UpdateMusicStream(musicaFondo);
        if (!IsMusicStreamPlaying(musicaFondo)) {
            PlayMusicStream(musicaFondo);
        }
    }
    
    // --- CONTADOR DE TIEMPO INDEPENDIENTE ---
    frames++;
    if(frames >= 60){
        contador++;
        frames = 0;
    }
    
    // --- LÓGICA DE MOVIMIENTO Y COLISIONES DE LA PELOTA Y LA BARRA ---
    if (!pausaPorMurete && !ganador) {
        PosicionPelota.x += VelocidadPelota.x;
        PosicionPelota.y += VelocidadPelota.y;
       
        // --- MOVIMIENTO DE LA BARRA CON TECLADO Y SOPORTE TÁCTIL ---
        if (IsKeyDown(KEY_LEFT) && barra.EstructuraRectangulo.x >= 0.0f) barra.EstructuraRectangulo.x -= 7.0f;
        if (IsKeyDown(KEY_RIGHT) && barra.EstructuraRectangulo.x <= 640.0f - barra.EstructuraRectangulo.width) barra.EstructuraRectangulo.x += 7.0f;
        
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) || GetTouchPointCount() > 0) {
            Vector2 touchPos = GetMousePosition();
            if (GetTouchPointCount() > 0) {
                touchPos = GetTouchPosition(0);
            }
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
       
        // --- COMPROBACIÓN DE PÉRDIDA DE VIDA ---
        if (PosicionPelota.y >= 960.0f) {
            vidas -= 1;
            pausaPorMurete = true;
            if (MusicaFondo){
                PauseMusicStream(musicaFondo);
            }
            PlaySound(sonidoLose);
            if (vidas <= 0) {
                gameOver = true;
            }
        }
       
        // --- COMPROBACIÓN DE VICTORIA ---
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
        
        // --- PANTALLA DE VIDA PERDIDA CON SOPORTE TÁCTIL ---
        if (pausaPorMurete && !gameOver && !ganador) {
            Rectangle rectBotonReintentar = { 30, 520, 550, 40 };
            Vector2 posToquePausa = { -1, -1 };
            bool toquePausaActivo = false;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                posToquePausa = GetMousePosition();
                toquePausaActivo = true;
            } else if (GetTouchPointCount() > 0) {
                posToquePausa = GetTouchPosition(0);
                toquePausaActivo = true;
            }

            DrawText("Perdiste una vida", 140, 440, 40, RED);
            DrawText("Presiona 'R' para reintentar", 30, 520, 40, SKYBLUE);
            
            if (IsKeyPressed(KEY_R) || (toquePausaActivo && CheckCollisionPointRec(posToquePausa, rectBotonReintentar))) {
                if (MusicaFondo){
                    ResumeMusicStream(musicaFondo);
                }
                PosicionPelota.x = 320.0f;
                PosicionPelota.y = 760.0f; 
                VelocidadPelota.y = -6.0f;
                barra.EstructuraRectangulo.width += 15;
                pausaPorMurete = false;    
            }
        }
        
        // --- PANTALLA DE GAME OVER CON SOPORTE TÁCTIL ---
        if (gameOver && !ganador) {
            Rectangle rectBotonGameOver = { 30, 520, 550, 40 };
            Vector2 posToqueGO = { -1, -1 };
            bool toqueGOActivo = false;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                posToqueGO = GetMousePosition();
                toqueGOActivo = true;
            } else if (GetTouchPointCount() > 0) {
                posToqueGO = GetTouchPosition(0);
                toqueGOActivo = true;
            }

            DrawText("Te quedaste sin vidas", 70, 440, 40, RED);
            DrawText("Presiona 'R' para volver a jugar", 30, 520, 35, SKYBLUE);
            
            if (IsKeyPressed(KEY_R) || (toqueGOActivo && CheckCollisionPointRec(posToqueGO, rectBotonGameOver))) {
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
                pausaPorMurete = false;
                gameOver = false; 
            }
        }
        
        // --- DIBUJO DEL HUD (PUNTUACIÓN, VIDAS Y TIEMPO) ---
        DrawText("PUNTUACION :", 20, 830, 30, WHITE);
        DrawText(TextFormat(" %d", puntuacion), 250, 830, 30, YELLOW);
        
        DrawText("Vidas Restantes :", 20, 860, 30, WHITE);
        DrawText(TextFormat(" %d", vidas), 300, 860, 30, RED);
        
        DrawText("Tiempo Perdido en este juego :", 20, 900, 30, WHITE);
        DrawText(TextFormat(" %d", contador), 500, 900, 30, GOLD);
        
        // --- DIBUJO DE PELOTA Y BARRA ---
        DrawCircleV(PosicionPelota, RadioPelota, WHITE);
        DrawRectangleRec(barra.EstructuraRectangulo, CLITERAL(Color){ 0, 255, 0, 255 });
        
        // --- BUCLE Y COLISIONES DE LOS LADRILLOS ---
        for (int i = 0; i < FILAS; i++){
            for (int j = 0; j < COLUMNAS; j++){
                if (CheckCollisionCircleRec(PosicionPelota, RadioPelota, ladrillos[i][j].rect) && ladrillos[i][j].activo) {
                    puntuacion += 1000;
                    PlaySound(sonidoReboteLadrillo);
                    
                    //CREAR PARTÍCULAS AL ROMPER EL LADRILLO
                    int particulasCreadas = 0;
                    for (int p = 0; p < MAX && particulasCreadas < 15; p++) {
                        if (particulas[p].activa == false) {
                            particulas[p].posicion = (Vector2){ ladrillos[i][j].rect.x + (ladrillos[i][j].rect.width / 2.0f), ladrillos[i][j].rect.y + (ladrillos[i][j].rect.height / 2.0f)};
                            
                            // VELOCIDAD ALEATORIA PARA QUE SE VEA MAS REALISTA
                            particulas[p].velocidad = (Vector2){ (float)(GetRandomValue(-300, 300)) / 50.0f, (float)(GetRandomValue(-300, 300)) / 50.0f};
                            particulas[p].miColor = coloresFilas[i]; //EL VECTOR QUE ESTA ARRIBA DEL TODO DE COLORES
                            particulas[p].tiempoVida = 0.6f; // TIEMPO QUE EXISTE EN SEGUNDOS
                            particulas[p].activa = true;
                            particulasCreadas++;
                        }
                    }
                    // ---------------------------------------------
                    
                    if (PosicionPelota.x >= ladrillos[i][j].rect.x && PosicionPelota.x <= ladrillos[i][j].rect.x + ladrillos[i][j].rect.width) {
                        VelocidadPelota.y *= -1;
                    } 
                    else {
                        VelocidadPelota.x *= -1;
                    }
                    ladrillos[i][j].activo = false;
                }
                
                if(ladrillos[i][j].activo){
                    DrawRectangleRec(ladrillos[i][j].rect, coloresFilas[i]);
                }
            }
        }
        
        // --- ACTUALIZAR Y DIBUJAR PARTÍCULAS -----------------------
        for (int p = 0; p < MAX; p++) {
            if (particulas[p].activa == true) {
                // LA VIDA VA BAJANDO
                particulas[p].tiempoVida -= GetFrameTime();
                if (particulas[p].tiempoVida <= 0.0f) {
                    particulas[p].activa = false;
                } else {
                    // LA PARTICULA SE MUEVE
                    particulas[p].posicion.x += particulas[p].velocidad.x;
                    particulas[p].posicion.y += particulas[p].velocidad.y;
                    
                    // DIBUJAR CIRCULO PEQUEÑO Y EL FADE ES PARA QUE SE DESVANEZCAN (FUNCION EN BIBLIOTECA DE RAYLIB)
                    DrawCircleV(particulas[p].posicion, 4.0f, Fade(particulas[p].miColor, particulas[p].tiempoVida));
                }
            }
        }
        //--------------------------------------------------------------
        
        // --- PANTALLA DE VICTORIA CON SOPORTE TÁCTIL ---
        if (ganador){
            Rectangle rectBotonWin = { 30, 440, 550, 40 };
            Vector2 posToqueWin = { -1, -1 };
            bool toqueWinActivo = false;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                posToqueWin = GetMousePosition();
                toqueWinActivo = true;
            } else if (GetTouchPointCount() > 0) {
                posToqueWin = GetTouchPosition(0);
                toqueWinActivo = true;
            }

            DrawText("¡¡GANASTE FELICITACIONES!!", 30, 240, 40, GOLD);
            DrawText("Presiona 'R' para volver a jugar", 30, 440, 35, SKYBLUE);
            DrawText("Presiona 'ESC' para salirte", 30, 500, 35, SKYBLUE);
            
            if (IsKeyPressed(KEY_R) || (toqueWinActivo && CheckCollisionPointRec(posToqueWin, rectBotonWin))) {
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
    InitWindow(640, 960, "BrickBreaker Morelloko");
    InitAudioDevice();

    // --- CARGA DE RECURSOS CON RUTA DE RESOURCES PARA LA WEB ---
    musicaFondo = LoadMusicStream("resources/FondoBrick.mp3");
    sonidoWin = LoadSound("resources/WinBricks.mp3");
    sonidoLose = LoadSound("resources/LoseBricks.mp3");
    sonidoReboteLadrillo = LoadSound("resources/click.wav");
    sonidoReboteBarra = LoadSound("resources/hitHurt.wav");
    HideCursor();

    barra.EstructuraRectangulo.x = 275.0f;
    barra.EstructuraRectangulo.y = 800.0f;
    barra.EstructuraRectangulo.width = 60.0f;
    barra.EstructuraRectangulo.height = 15.0f;
    
    // INICIALIZAR PARTÍCULAS INACTIVAS
    for (int i = 0; i < MAX; i++){
        particulas[i].activa = false;
    }

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
