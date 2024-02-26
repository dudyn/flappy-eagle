#include <iostream>
#include <time.h>
#include <cstdlib>
#include <string>
#include <fstream>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
using namespace std;

int width = 500;
int height = 600;
int grass_height = height - 90;
int topposx = 40;
int topposy = 160;

bool done = false, redraw = true;
int states = -1;
bool gotowy_zapis = false;
bool falling = false;
bool key[1] = { false };
const int ile_blokow = 3;
int sourceX = 0, sourceY = 0;
const int FPS = 60, birdFPS = 8;
float speed = 5;
float vely = 0;
int x = 100, y = 10;
const float gravity = 0.6;
bool live = true;
bool jump = false;
float jumpSpeed = 10;
int score = 0;
string name = "dudek";
string namechange = "";
bool stop_blocks = false;
bool isMenuOpen = true;
fstream file;
string names[1000];
int points[1000];
string topnames[10];
int toppoints[10];
int max_points;
string max_names;
bool nacisniety_login = 0;
bool nacisniete_haslo = 0;
string haslo = "123";
string haslochange = "";
string haslogwiazdka = "";
bool loginhasloready = 0;
bool gotowarejestracja = 0;
bool poczatek = 1;
bool blad = 0;
bool pokazane_haslo = 0;
int suma_punktow = 0, ilosc_meczow = 0, min_punkty = 0, max_punkty = 0;
float srednia_punktow=0;
int pozycja_x=0;
int pozycja_y=0;
int zapisana_x = 0;
int zapisana_y = 0;
bool gotoweprzesuniecie = 1, gotoweprzesuniecie2 = 1, gotoweprzesuniecie3 = 1, gotoweprzesuniecie4 = 1;
bool gotowyfalling = 1;

struct Block
{
    int x;
    int y;
    int x2;
    int y2;
    bool live = false;
    int speed = 3;
    int boundx = 80;
    int boundy;
    bool score = true;
};

struct Grass
{
    int x;
    int y = grass_height;
    bool live = false;
    int speed = 3;
};

void DrawBlocks(Block blocks[], int size, ALLEGRO_BITMAP* blok_dolny, ALLEGRO_BITMAP* blok_gorny);
void DrawFloor(Grass trawa[], ALLEGRO_BITMAP* floor, int size, bool& live);
void StartBlocks(Block blocks[], int size);
void UpdateBlocks(Block blocks[], int size);
void Collision(Block blocks[], int size, bool& live, int& x, int& y, bool& stop_blocks, bool& falling, bool& gotowy_zapis, ALLEGRO_SAMPLE* hit1);
void ScoreCounting(Block blocks[], int size, bool& live, int& x, int& y, int& score, ALLEGRO_SAMPLE* point1);
void Falling(int& y, bool& live, bool& stop_blocks, bool& falling, ALLEGRO_SAMPLE* hit1);
void RestartGame(Block blocks[], int size, int& x, int& y, bool& live, int& score, bool& stop_blocks, bool& isMenuOpen, float& vely, bool& falling, Grass trawa[], bool& gotowy_zapis);
void FileReading(string& max_names, int& max_points, string names[], int points[], fstream& plik);
void ChangeState(int& states, int new_state);
void FileReadingLogowanie();
void FileSavingAccount();
void GwiazdkowanieHasla();
void FileReadingStatystyki();
void Przesuniecie(ALLEGRO_SAMPLE* przesuniecie1);
void Przesuniecie2(ALLEGRO_SAMPLE* przesuniecie1);
void Przesuniecie3(ALLEGRO_SAMPLE* przesuniecie1);
void Przesuniecie4(ALLEGRO_SAMPLE* przesuniecie1);
void CzyGotowePrzesuniecie();
void CzyGotowePrzesuniecie2();
void CzyGotowePrzesuniecie3();
void CzyGotowePrzesuniecie4();

enum keys { UP };
enum states { MENU, STATS, PLAYING, LOST, WYBORY, ZMIANANICKU, LOGOWANIE, LOGOWANIEZHASLEM, LOGOWANIEREJESTRACJA, REJESTRACJA, MOJESTATY };

Block blocks[ile_blokow];
Grass trawa[3];

int main()
{
    srand(time(NULL));

    ALLEGRO_DISPLAY* display = NULL;

    if (!al_init())
    {
        return -1;
    }

    display = al_create_display(width, height);


    if (!display)
    {
        return -1;
    }

    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();
    al_install_keyboard();
    al_install_mouse();
    al_init_image_addon();
    al_install_audio();
    al_init_acodec_addon();

    ALLEGRO_FONT* font1 = al_load_font("BebasNeue-Regular.ttf", 35, 0);
    ALLEGRO_FONT* font2 = al_load_font("BebasNeue-Regular.ttf", 20, 0);
    ALLEGRO_FONT* font3 = al_load_font("BebasNeue-Regular.ttf", 30, 0);
    ALLEGRO_FONT* font4 = al_load_font("BebasNeue-Regular.ttf", 14, 0);
    ALLEGRO_FONT* score_font = al_load_font("BebasNeue-Regular.ttf", 50, NULL);
    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);
    ALLEGRO_TIMER* birdtimer = al_create_timer(1.0 / birdFPS);
    ALLEGRO_BITMAP* kac = al_load_bitmap("kac.jpg");
    ALLEGRO_BITMAP* bird = al_load_bitmap("flappymove2.png");
    ALLEGRO_BITMAP* fallingbird = al_load_bitmap("flappyspadanie2.png");
    ALLEGRO_BITMAP* blok_dolny = al_load_bitmap("blokdolny.jpg");
    ALLEGRO_BITMAP* blok_gorny = al_load_bitmap("blokgorny.jpg");
    ALLEGRO_BITMAP* floor = al_load_bitmap("floor.jpg");
    ALLEGRO_BITMAP* background = al_load_bitmap("tlo.jpg");
    ALLEGRO_SAMPLE* wing = al_load_sample("wing.wav");
    ALLEGRO_SAMPLE* hit = al_load_sample("hit.wav");
    ALLEGRO_SAMPLE* point = al_load_sample("point.wav");
    ALLEGRO_SAMPLE* klik = al_load_sample("klik.wav");
    ALLEGRO_SAMPLE* przesuniecie = al_load_sample("przesuniecie.wav");
    ALLEGRO_COLOR white = al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR grey = al_map_rgb(125, 125, 125);
    ALLEGRO_COLOR button_color_start = grey;
    ALLEGRO_COLOR button_color_statystyki = grey;
    ALLEGRO_COLOR button_color_zmiana_nicku = grey;
    ALLEGRO_COLOR button_color_wyjdz = grey;
    ALLEGRO_COLOR button_color_back = grey;
    ALLEGRO_COLOR button_color_ok = grey;

    al_reserve_samples(5);

    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_timer_event_source(birdtimer));
    al_start_timer(timer);
    al_start_timer(birdtimer);

    FileReading(max_names, max_points, names, points, file);
    ChangeState(states, LOGOWANIEREJESTRACJA);

    while (!done)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            switch (ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_SPACE:
                if (states == PLAYING)
                {
                    if (!stop_blocks && y >= 0)
                    {
                        al_play_sample(wing, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
                        key[UP] = true;
                        break;
                    }
                }
                else if (states == LOST)
                {
                    ChangeState(states, PLAYING);
                }
                else if (states == STATS)
                {
                    ChangeState(states, MENU);
                }
                else if (states == MENU)
                {
                    ChangeState(states, PLAYING);
                }
                else if (states == WYBORY)
                {
                    ChangeState(states, MENU);
                }
            case ALLEGRO_KEY_ENTER:
                if (states == MENU)
                {
                    ChangeState(states, PLAYING);
                }
                if (states == LOST)
                {
                    ChangeState(states, PLAYING);
                }
            case ALLEGRO_KEY_BACKSPACE:
                if (states == LOST)
                {
                    ChangeState(states, WYBORY);
                }
                if ((states == ZMIANANICKU || states == LOGOWANIE) || (states==LOGOWANIEZHASLEM && nacisniety_login==1) || (states == REJESTRACJA && nacisniety_login == 1))
                {
                    if (namechange.length() > 0)
                    {
                        namechange.pop_back();
                    }
                }
                else if ((states == LOGOWANIEZHASLEM && nacisniete_haslo == 1) || (states == REJESTRACJA && nacisniete_haslo == 1))
                {
                    if (haslochange.length() > 0)
                    {
                        haslochange.pop_back();
                    }
                }
            }
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_UP)
        {
            switch (ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_SPACE:
                key[UP] = false;
                break;
            case ALLEGRO_KEY_ESCAPE:
                done = true;
                break;
            }
        }
        else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            done = true;
        }
        else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
        {
            if (states == WYBORY)
            {
                if (ev.mouse.button & 1)
                {
                    if (ev.mouse.x < 350 && ev.mouse.x>150 && ev.mouse.y > 165 && ev.mouse.y < 215)
                    {
                        al_play_sample(klik, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
                        button_color_start = grey;
                        states = MENU;
                    }
                }
                if (ev.mouse.button & 1)
                {
                    if (ev.mouse.x < 350 && ev.mouse.x>150 && ev.mouse.y > 245 && ev.mouse.y < 295)
                    {
                        al_play_sample(klik, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
                        button_color_statystyki = grey;
                        states = STATS;
                    }
                }
                if (ev.mouse.button & 1)
                {
                    if (ev.mouse.x < 350 && ev.mouse.x>150 && ev.mouse.y > 325 && ev.mouse.y < 375)
                    {
                        al_play_sample(klik, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
                        button_color_zmiana_nicku = grey;
                        zapisana_x = pozycja_x;
                        zapisana_y = pozycja_y;
                        states = LOGOWANIEREJESTRACJA;
                    }
                }
                if (ev.mouse.button & 1)
                {
                    if (ev.mouse.x < 350 && ev.mouse.x>150 && ev.mouse.y > 405 && ev.mouse.y < 455)
                    {
                        al_play_sample(klik, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
                        done = true;
                    }
                }
            }
            if (states == LOGOWANIEREJESTRACJA)
            {
                if (ev.mouse.button & 1)
                {
                    if (ev.mouse.x < 350 && ev.mouse.x>150 && ev.mouse.y > 190 && ev.mouse.y < 240)
                    {
                        al_play_sample(klik, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
                        button_color_statystyki = grey;
                        states = LOGOWANIEZHASLEM;
                    }
                }
                if (ev.mouse.button & 1)
                {
                    if (ev.mouse.x < 350 && ev.mouse.x>150 && ev.mouse.y > 270 && ev.mouse.y < 320)
                    {
                        al_play_sample(klik, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
                        button_color_zmiana_nicku = grey;
                        states = REJESTRACJA;
                    }
                }
                if (ev.mouse.button & 1)
                {
                    if (ev.mouse.x < 350 && ev.mouse.x>150 && ev.mouse.y > 350 && ev.mouse.y < 400 && ev.mouse.x!=zapisana_x && ev.mouse.y!=zapisana_y)
                    {
                        if (poczatek == 1)
                        {
                            al_play_sample(klik, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
                            done = true;
                        }
                        else
                        {
                            al_play_sample(klik, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
                            button_color_wyjdz = grey;
                            states = WYBORY;
                        }
                    }
                }
            }
            else if (states == STATS)
            {
                if (ev.mouse.button & 1)
                {
                    if (ev.mouse.x < 350 && ev.mouse.x>150 && ev.mouse.y > 495 && ev.mouse.y < 545)
                    {
                        al_play_sample(klik, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
                        button_color_back = grey;
                        states = WYBORY;
                    }
                    if (ev.mouse.x < 447 && ev.mouse.x>403 && ev.mouse.y > 498 && ev.mouse.y < 542)
                    {
                        al_play_sample(klik, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
                        button_color_wyjdz = grey;
                        button_color_statystyki = grey;
                        states = MOJESTATY;
                        FileReadingStatystyki();
                    }
                }
            }
            else if (states == MOJESTATY)
            {
                if (ev.mouse.button & 1)
                {
                    if (ev.mouse.x < 350 && ev.mouse.x>150 && ev.mouse.y > 495 && ev.mouse.y < 545)
                    {
                        al_play_sample(klik, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
                        button_color_back = grey;
                        states = WYBORY;
                    }
                }
            }
            else if (states == ZMIANANICKU)
            {
                if (ev.mouse.button & 1)
                {
                    if (ev.mouse.x < 350 && ev.mouse.x>150 && ev.mouse.y > 445 && ev.mouse.y < 495)
                    {
                        al_play_sample(klik, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
                        button_color_wyjdz = grey;
                        namechange = "";
                        states = WYBORY;
                    }
                }
                if (ev.mouse.button & 1)
                {
                    if (ev.mouse.x < 350 && ev.mouse.x>150 && ev.mouse.y > 375 && ev.mouse.y < 425)
                    {
                        al_play_sample(klik, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
                        button_color_ok = grey;
                        name = namechange;
                        namechange = "";
                        states = WYBORY;
                    }
                }
            }
            else if (states == LOGOWANIE)
            {
                if (ev.mouse.button & 1)
                {
                    if (ev.mouse.x < 350 && ev.mouse.x>150 && ev.mouse.y > 395 && ev.mouse.y < 445)
                    {
                        al_play_sample(klik, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
                        button_color_ok = grey;
                        name = namechange;
                        namechange = "";
                        states = WYBORY;
                    }
                }
            }
            else if (states == LOGOWANIEZHASLEM)
            {
                if (ev.mouse.button & 1)
                {
                    if (ev.mouse.x < 410 && ev.mouse.x>90 && ev.mouse.y > 140 && ev.mouse.y < 190)
                    {
                        nacisniete_haslo = 0;
                        nacisniety_login = 1;
                    }
                    if (ev.mouse.x < 410 && ev.mouse.x>90 && ev.mouse.y > 270 && ev.mouse.y < 320)
                    {
                        nacisniete_haslo = 1;
                        nacisniety_login = 0;
                    }
                    if (ev.mouse.x < 350 && ev.mouse.x>150 && ev.mouse.y > 400 && ev.mouse.y < 450)
                    {
                        FileReadingLogowanie();
                        if (loginhasloready == 1)
                        {
                            al_play_sample(klik, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
                            button_color_ok = grey;
                            name = namechange;
                            haslo = haslochange;
                            namechange = "";
                            haslochange = "";
                            haslogwiazdka = "";
                            loginhasloready = 0;
                            poczatek = 0;
                            if (poczatek == 1)
                            {
                                states = LOGOWANIEREJESTRACJA;
                            }
                            else
                            {
                                states = WYBORY;
                            }
                            blad = 0;
                        }
                        else
                        {
                            al_play_sample(klik, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
                            blad = 1;
                        }
                    }
                    if (ev.mouse.x < 410 && ev.mouse.x>90 && ev.mouse.y > 460 && ev.mouse.y < 510)
                    {
                        al_play_sample(klik, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
                        button_color_wyjdz = grey;
                        haslochange = "";
                        namechange = "";
                        haslogwiazdka = "";
                        if (poczatek == 1)
                        {
                            states = LOGOWANIEREJESTRACJA;
                        }
                        else
                        {
                            states = WYBORY;
                        }
                        blad = 0;
                    }
                    if (ev.mouse.x < 460 && ev.mouse.x>430 && ev.mouse.y > 280 && ev.mouse.y < 310)
                    {
                        al_play_sample(klik, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
                        if (pokazane_haslo == 1)
                        {
                            pokazane_haslo = 0;
                        }
                        else
                        {
                            pokazane_haslo = 1;
                        }
                    }
                }
            }
            else if (states == REJESTRACJA)
            {
            if (ev.mouse.button & 1)
            {
                if (ev.mouse.x < 410 && ev.mouse.x>90 && ev.mouse.y > 140 && ev.mouse.y < 190)
                {
                    nacisniete_haslo = 0;
                    nacisniety_login = 1;
                }
                if (ev.mouse.x < 410 && ev.mouse.x>90 && ev.mouse.y > 270 && ev.mouse.y < 320)
                {
                    nacisniete_haslo = 1;
                    nacisniety_login = 0;
                }
                if (ev.mouse.x < 350 && ev.mouse.x>150 && ev.mouse.y > 400 && ev.mouse.y < 450)
                {
                    al_play_sample(klik, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
                    FileSavingAccount();
                    if (gotowarejestracja == 1)
                    {
                        button_color_ok = grey;
                        name = namechange;
                        haslo = haslochange;
                        namechange = "";
                        haslochange = "";
                        haslogwiazdka = "";
                        gotowarejestracja = 0;
                        poczatek = 0;
                        states = WYBORY;
                        blad = 0;
                    }
                    else
                    {
                        blad = 1;
                    }
                }
                if (ev.mouse.x < 410 && ev.mouse.x>90 && ev.mouse.y > 460 && ev.mouse.y < 510)
                {
                    al_play_sample(klik, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
                    button_color_wyjdz = grey;
                    haslochange = "";
                    namechange = "";
                    haslogwiazdka = "";
                    if (poczatek == 1)
                    {
                        states = LOGOWANIEREJESTRACJA;
                    }
                    else
                    {
                        states = WYBORY;
                    }
                    blad = 0;
                }
                if (ev.mouse.x < 460 && ev.mouse.x>430 && ev.mouse.y > 280 && ev.mouse.y < 310)
                {
                    al_play_sample(klik, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
                    if (pokazane_haslo == 1)
                    {
                        pokazane_haslo = 0;
                    }
                    else
                    {
                        pokazane_haslo = 1;
                    }
                }
            }
            }
        }
        else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES)
        {
            if (states == WYBORY)
            {
                if (ev.mouse.x < 350 && ev.mouse.x>150 && ev.mouse.y > 165 && ev.mouse.y < 215)
                {
                    Przesuniecie(przesuniecie);
                    button_color_start = white;
                }
                else
                {
                    CzyGotowePrzesuniecie();
                    button_color_start = grey;
                }
                if (ev.mouse.x < 350 && ev.mouse.x>150 && ev.mouse.y > 245 && ev.mouse.y < 295)
                {
                    Przesuniecie2(przesuniecie);
                    button_color_statystyki = white;
                }
                else
                {
                    CzyGotowePrzesuniecie2();
                    button_color_statystyki = grey;
                }
                if (ev.mouse.x < 350 && ev.mouse.x>150 && ev.mouse.y > 325 && ev.mouse.y < 375)
                {
                    Przesuniecie3(przesuniecie);
                    button_color_zmiana_nicku = white;
                }
                else
                {
                    CzyGotowePrzesuniecie3();
                    button_color_zmiana_nicku = grey;
                }
                if (ev.mouse.x < 350 && ev.mouse.x>150 && ev.mouse.y > 405 && ev.mouse.y < 455)
                {
                    Przesuniecie4(przesuniecie);
                    button_color_wyjdz = white;
                }
                else
                {
                    CzyGotowePrzesuniecie4();
                    button_color_wyjdz = grey;
                }
            }
            if (states == LOGOWANIEREJESTRACJA)
            {
                if (ev.mouse.x < 350 && ev.mouse.x>150 && ev.mouse.y > 190 && ev.mouse.y < 240)
                {
                    Przesuniecie(przesuniecie);
                    button_color_statystyki = white;
                }
                else
                {
                    CzyGotowePrzesuniecie();
                    button_color_statystyki = grey;
                }
                if (ev.mouse.x < 350 && ev.mouse.x>150 && ev.mouse.y > 270 && ev.mouse.y < 320)
                {
                    Przesuniecie2(przesuniecie);
                    button_color_zmiana_nicku = white;
                }
                else
                {
                    CzyGotowePrzesuniecie2();
                    button_color_zmiana_nicku = grey;
                }
                if (ev.mouse.x < 350 && ev.mouse.x>150 && ev.mouse.y > 350 && ev.mouse.y < 400)
                {
                    Przesuniecie3(przesuniecie);
                    button_color_wyjdz = white;
                }
                else
                {
                    CzyGotowePrzesuniecie3();
                    button_color_wyjdz = grey;
                }
            }
            else if (states == STATS)
            {
                if (ev.mouse.x < 350 && ev.mouse.x>150 && ev.mouse.y > 495 && ev.mouse.y < 545)
                {
                    Przesuniecie(przesuniecie);
                    button_color_back = white;
                }
                else
                {
                    CzyGotowePrzesuniecie();
                    button_color_back = grey;
                }
                if (ev.mouse.x < 447 && ev.mouse.x>403 && ev.mouse.y > 498 && ev.mouse.y < 542)
                {
                    Przesuniecie2(przesuniecie);
                    button_color_statystyki = white;
                }
                else
                {
                    CzyGotowePrzesuniecie2();
                    button_color_statystyki = grey;
                }
            }
            else if (states == MOJESTATY)
            {
                if (ev.mouse.x < 350 && ev.mouse.x>150 && ev.mouse.y > 495 && ev.mouse.y < 545)
                {
                    Przesuniecie(przesuniecie);
                   button_color_back = white;
                }
                 else
                {
                    CzyGotowePrzesuniecie();
                   button_color_back = grey;
                }
                
            }
            else if (states == ZMIANANICKU)
            {
                if (ev.mouse.x < 350 && ev.mouse.x>150 && ev.mouse.y > 375 && ev.mouse.y < 425)
                {
                    Przesuniecie(przesuniecie);
                    button_color_ok = white;
                }
                else
                {
                    CzyGotowePrzesuniecie();
                    button_color_ok = grey;
                }
                if (ev.mouse.x < 350 && ev.mouse.x>150 && ev.mouse.y > 455 && ev.mouse.y < 495)
                {
                    Przesuniecie2(przesuniecie);
                    button_color_wyjdz = white;
                }
                else
                {
                    CzyGotowePrzesuniecie2();
                    button_color_wyjdz = grey;
                }
            }
            else if (states == LOGOWANIE)
            {
                if (ev.mouse.x < 350 && ev.mouse.x>150 && ev.mouse.y > 395 && ev.mouse.y < 445)
                {
                    Przesuniecie(przesuniecie);
                    button_color_ok = white;
                }
                else
                {
                    CzyGotowePrzesuniecie();
                    button_color_ok = grey;
                }
            }
            else if (states == LOGOWANIEZHASLEM)
            {
                if (ev.mouse.x < 350 && ev.mouse.x>150 && ev.mouse.y > 400 && ev.mouse.y < 450)
                {
                    Przesuniecie(przesuniecie);
                    button_color_ok = white;
                }
                else
                {
                    CzyGotowePrzesuniecie();
                    button_color_ok = grey;
                }
                if (ev.mouse.x < 350 && ev.mouse.x>150 && ev.mouse.y > 460 && ev.mouse.y < 510)
                {
                    Przesuniecie2(przesuniecie);
                    button_color_wyjdz = white;
                }
                else
                {
                    CzyGotowePrzesuniecie2();
                    button_color_wyjdz = grey;
                }
            }
            else if (states == REJESTRACJA)
            {
                if (ev.mouse.x < 350 && ev.mouse.x>150 && ev.mouse.y > 400 && ev.mouse.y < 450)
                {
                    Przesuniecie(przesuniecie);
                    button_color_ok = white;
                }
                else
                {
                    CzyGotowePrzesuniecie();
                    button_color_ok = grey;
                }
                if (ev.mouse.x < 350 && ev.mouse.x>150 && ev.mouse.y > 460 && ev.mouse.y < 510)
                {
                    Przesuniecie2(przesuniecie);
                    button_color_wyjdz = white;
                }
                else
                {
                    CzyGotowePrzesuniecie2();
                    button_color_wyjdz = grey;
                }
            }
            pozycja_x = ev.mouse.x;
            pozycja_y = ev.mouse.y;
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_CHAR)
        {
            if ((states == ZMIANANICKU || states == LOGOWANIE) || (states==LOGOWANIEZHASLEM && nacisniety_login==1) || (states == REJESTRACJA && nacisniety_login == 1))
            {
                if (namechange.length() < 20)
                {
                    if (ev.keyboard.unichar == 'q')
                    {
                        namechange.append("q");
                    }
                    if (ev.keyboard.unichar == 'w')
                    {
                        namechange.append("w");
                    }
                    if (ev.keyboard.unichar == 'e')
                    {
                        namechange.append("e");
                    }
                    if (ev.keyboard.unichar == 'r')
                    {
                        namechange.append("r");
                    }
                    if (ev.keyboard.unichar == 't')
                    {
                        namechange.append("t");
                    }
                    if (ev.keyboard.unichar == 'y')
                    {
                        namechange.append("y");
                    }
                    if (ev.keyboard.unichar == 'u')
                    {
                        namechange.append("u");
                    }
                    if (ev.keyboard.unichar == 'i')
                    {
                        namechange.append("i");
                    }
                    if (ev.keyboard.unichar == 'o')
                    {
                        namechange.append("o");
                    }
                    if (ev.keyboard.unichar == 'p')
                    {
                        namechange.append("p");
                    }
                    if (ev.keyboard.unichar == 'a')
                    {
                        namechange.append("a");
                    }
                    if (ev.keyboard.unichar == 's')
                    {
                        namechange.append("s");
                    }
                    if (ev.keyboard.unichar == 'd')
                    {
                        namechange.append("d");
                    }
                    if (ev.keyboard.unichar == 'f')
                    {
                        namechange.append("f");
                    }
                    if (ev.keyboard.unichar == 'g')
                    {
                        namechange.append("g");
                    }
                    if (ev.keyboard.unichar == 'h')
                    {
                        namechange.append("h");
                    }
                    if (ev.keyboard.unichar == 'j')
                    {
                        namechange.append("j");
                    }
                    if (ev.keyboard.unichar == 'k')
                    {
                        namechange.append("k");
                    }
                    if (ev.keyboard.unichar == 'l')
                    {
                        namechange.append("l");
                    }
                    if (ev.keyboard.unichar == 'z')
                    {
                        namechange.append("z");
                    }
                    if (ev.keyboard.unichar == 'x')
                    {
                        namechange.append("x");
                    }
                    if (ev.keyboard.unichar == 'c')
                    {
                        namechange.append("c");
                    }
                    if (ev.keyboard.unichar == 'v')
                    {
                        namechange.append("v");
                    }
                    if (ev.keyboard.unichar == 'b')
                    {
                        namechange.append("b");
                    }
                    if (ev.keyboard.unichar == 'n')
                    {
                        namechange.append("n");
                    }
                    if (ev.keyboard.unichar == 'm')
                    {
                        namechange.append("m");
                    }
                    if (ev.keyboard.unichar == '1')
                    {
                        namechange.append("1");
                    }
                    if (ev.keyboard.unichar == '2')
                    {
                        namechange.append("2");
                    }
                    if (ev.keyboard.unichar == '3')
                    {
                        namechange.append("3");
                    }
                    if (ev.keyboard.unichar == '4')
                    {
                        namechange.append("4");
                    }
                    if (ev.keyboard.unichar == '5')
                    {
                        namechange.append("5");
                    }
                    if (ev.keyboard.unichar == '6')
                    {
                        namechange.append("6");
                    }
                    if (ev.keyboard.unichar == '7')
                    {
                        namechange.append("7");
                    }
                    if (ev.keyboard.unichar == '8')
                    {
                        namechange.append("8");
                    }
                    if (ev.keyboard.unichar == '9')
                    {
                        namechange.append("9");
                    }
                    if (ev.keyboard.unichar == '0')
                    {
                        namechange.append("0");
                    }
                }
                GwiazdkowanieHasla();
            }
            else if ((states == LOGOWANIEZHASLEM && nacisniete_haslo == 1) || (states == REJESTRACJA && nacisniete_haslo == 1))
            {
            if (haslochange.length() < 20)
            {
                if (ev.keyboard.unichar == 'q')
                {
                    haslochange.append("q");
                }
                if (ev.keyboard.unichar == 'w')
                {
                    haslochange.append("w");
                }
                if (ev.keyboard.unichar == 'e')
                {
                    haslochange.append("e");
                }
                if (ev.keyboard.unichar == 'r')
                {
                    haslochange.append("r");
                }
                if (ev.keyboard.unichar == 't')
                {
                    haslochange.append("t");
                }
                if (ev.keyboard.unichar == 'y')
                {
                    haslochange.append("y");
                }
                if (ev.keyboard.unichar == 'u')
                {
                    haslochange.append("u");
                }
                if (ev.keyboard.unichar == 'i')
                {
                    haslochange.append("i");
                }
                if (ev.keyboard.unichar == 'o')
                {
                    haslochange.append("o");
                }
                if (ev.keyboard.unichar == 'p')
                {
                    haslochange.append("p");
                }
                if (ev.keyboard.unichar == 'a')
                {
                    haslochange.append("a");
                }
                if (ev.keyboard.unichar == 's')
                {
                    haslochange.append("s");
                }
                if (ev.keyboard.unichar == 'd')
                {
                    haslochange.append("d");
                }
                if (ev.keyboard.unichar == 'f')
                {
                    haslochange.append("f");
                }
                if (ev.keyboard.unichar == 'g')
                {
                    haslochange.append("g");
                }
                if (ev.keyboard.unichar == 'h')
                {
                    haslochange.append("h");
                }
                if (ev.keyboard.unichar == 'j')
                {
                    haslochange.append("j");
                }
                if (ev.keyboard.unichar == 'k')
                {
                    haslochange.append("k");
                }
                if (ev.keyboard.unichar == 'l')
                {
                    haslochange.append("l");
                }
                if (ev.keyboard.unichar == 'z')
                {
                    haslochange.append("z");
                }
                if (ev.keyboard.unichar == 'x')
                {
                    haslochange.append("x");
                }
                if (ev.keyboard.unichar == 'c')
                {
                    haslochange.append("c");
                }
                if (ev.keyboard.unichar == 'v')
                {
                    haslochange.append("v");
                }
                if (ev.keyboard.unichar == 'b')
                {
                    haslochange.append("b");
                }
                if (ev.keyboard.unichar == 'n')
                {
                    haslochange.append("n");
                }
                if (ev.keyboard.unichar == 'm')
                {
                    haslochange.append("m");
                }
                if (ev.keyboard.unichar == '1')
                {
                    haslochange.append("1");
                }
                if (ev.keyboard.unichar == '2')
                {
                    haslochange.append("2");
                }
                if (ev.keyboard.unichar == '3')
                {
                    haslochange.append("3");
                }
                if (ev.keyboard.unichar == '4')
                {
                    haslochange.append("4");
                }
                if (ev.keyboard.unichar == '5')
                {
                    haslochange.append("5");
                }
                if (ev.keyboard.unichar == '6')
                {
                    haslochange.append("6");
                }
                if (ev.keyboard.unichar == '7')
                {
                    haslochange.append("7");
                }
                if (ev.keyboard.unichar == '8')
                {
                    haslochange.append("8");
                }
                if (ev.keyboard.unichar == '9')
                {
                    haslochange.append("9");
                }
                if (ev.keyboard.unichar == '0')
                {
                    haslochange.append("0");
                }
                GwiazdkowanieHasla();
            }
            }
        }
        else if (ev.type == ALLEGRO_EVENT_TIMER)
        {
            if (ev.timer.source == timer)
            {
                if (states == MENU)
                {
                    al_clear_to_color(al_map_rgb(0, 0, 0));
                    al_draw_text(font2, al_map_rgb(255, 0, 255), 435, 10, ALLEGRO_ALIGN_CENTRE, "~ made by dudyn");
                    al_draw_text(font2, al_map_rgb(255, 255, 0), 10, 10, ALLEGRO_ALIGN_LEFT, "aktualna nazwa:");
                    al_draw_text(font2, al_map_rgb(255, 255, 0), 125, 10, ALLEGRO_ALIGN_LEFT, name.c_str());
                    al_draw_text(font1, al_map_rgb(0, 255, 0), width / 2, height / 2 - 75, ALLEGRO_ALIGN_CENTRE, "flappy eagle");
                    al_draw_text(font1, al_map_rgb(255, 255, 0), width / 2, height / 2 - 25, ALLEGRO_ALIGN_CENTRE, "kliknij spacje aby zagrac");
                    al_flip_display();
                }
                else if (states == LOGOWANIE)
                {
                    al_clear_to_color(al_map_rgb(0, 0, 0));
                    al_draw_text(font2, al_map_rgb(255, 0, 255), 435, 10, ALLEGRO_ALIGN_CENTRE, "~ made by dudyn");
                    al_draw_text(font3, al_map_rgb(255, 0, 0), width / 2, 150, ALLEGRO_ALIGN_CENTRE, "podaj nazwe");
                    al_draw_filled_rectangle(90, 190, 410, 240, al_map_rgb(255, 255, 255));
                    al_draw_text(font1, al_map_rgb(0, 100, 0), width / 2, 195, ALLEGRO_ALIGN_CENTRE, namechange.c_str());
                    al_draw_filled_rectangle(150, 395, 350, 445, button_color_ok);
                    al_draw_text(font1, al_map_rgb(0, 100, 0), width / 2, 400, ALLEGRO_ALIGN_CENTRE, "ok");
                    al_flip_display();
                }
                else if (states == LOGOWANIEREJESTRACJA)
                {
                    al_clear_to_color(al_map_rgb(0, 0, 0));
                    al_draw_text(font2, al_map_rgb(255, 0, 255), 435, 10, ALLEGRO_ALIGN_CENTRE, "~ made by dudyn");
                    al_draw_filled_rectangle(150, 190, 350, 240, button_color_statystyki);
                    al_draw_filled_rectangle(150, 270, 350, 320, button_color_zmiana_nicku);
                    al_draw_filled_rectangle(150, 350, 350, 400, button_color_wyjdz);
                    al_draw_text(font1, al_map_rgb(0, 0, 125), width / 2, 195, ALLEGRO_ALIGN_CENTRE, "LOGOWANIE");
                    al_draw_text(font1, al_map_rgb(0, 0, 125), width / 2, 275, ALLEGRO_ALIGN_CENTRE, "REJESTRACJA");
                    if (poczatek == 1)
                    {
                        al_draw_text(font1, al_map_rgb(255, 0, 0), width / 2, 355, ALLEGRO_ALIGN_CENTRE, "WYJDZ");
                    }
                    else
                    {
                        al_draw_text(font1, al_map_rgb(0, 100, 0), width / 2, 355, ALLEGRO_ALIGN_CENTRE, "WROC");
                    }
                    al_flip_display();
                }
                else if (states == LOGOWANIEZHASLEM)
                {
                    al_clear_to_color(al_map_rgb(0, 0, 0));
                    al_draw_text(font2, al_map_rgb(255, 0, 255), 435, 10, ALLEGRO_ALIGN_CENTRE, "~ made by dudyn");

                    al_draw_text(font3, al_map_rgb(255, 0, 0), width / 2, 100, ALLEGRO_ALIGN_CENTRE, "podaj nazwe");
                    al_draw_text(font2, al_map_rgb(255, 255, 0), 10, 10, ALLEGRO_ALIGN_LEFT, "logowanie");

                    if (blad == 1)
                    {
                        al_draw_line(75, 155, 55, 175, al_map_rgb(255, 0, 0), 3);
                        al_draw_line(55, 155, 75, 175, al_map_rgb(255, 0, 0), 3);
                        al_draw_line(75, 285, 55, 305, al_map_rgb(255, 0, 0), 3);
                        al_draw_line(55, 285, 75, 305, al_map_rgb(255, 0, 0), 3);
                    }

                    if (nacisniety_login == 1)
                    {
                        al_draw_filled_rectangle(90, 140, 410, 190, al_map_rgb(255,255,255));
                    }
                    else
                    {
                        al_draw_filled_rectangle(90, 140, 410, 190, al_map_rgb(250, 250, 100));
                    }

                    al_draw_text(font1, al_map_rgb(0, 100, 0), width / 2, 145, ALLEGRO_ALIGN_CENTRE, namechange.c_str());

                    al_draw_text(font3, al_map_rgb(255, 0, 0), width / 2, 230, ALLEGRO_ALIGN_CENTRE, "podaj haslo");
                    if (nacisniete_haslo == 1)
                    {
                        al_draw_filled_rectangle(90, 270, 410, 320, al_map_rgb(255, 255, 255));
                    }
                    else
                    {
                        al_draw_filled_rectangle(90, 270, 410, 320, al_map_rgb(250, 250, 100));
                    }

                    if (pokazane_haslo == 1)
                    {
                        al_draw_filled_rectangle(430, 280, 460, 310, al_map_rgb(180, 0, 0));
                        al_draw_text(font4, al_map_rgb(0, 0, 0), 445, 286, ALLEGRO_ALIGN_CENTRE, "ukryj");
                        al_draw_text(font1, al_map_rgb(0, 100, 0), width / 2, 275, ALLEGRO_ALIGN_CENTRE, haslochange.c_str());
                    }
                    else
                    {
                        al_draw_filled_rectangle(430, 280, 460, 310, al_map_rgb(0, 180, 0));
                        al_draw_text(font4, al_map_rgb(0, 0, 0), 445, 286, ALLEGRO_ALIGN_CENTRE, "pokaz");
                        al_draw_text(font1, al_map_rgb(0, 100, 0), width / 2, 275, ALLEGRO_ALIGN_CENTRE, haslogwiazdka.c_str());
                    }

                    al_draw_filled_rectangle(150, 400, 350, 450, button_color_ok);
                    al_draw_text(font1, al_map_rgb(0, 100, 0), width / 2, 405, ALLEGRO_ALIGN_CENTRE, "ok");

                    al_draw_filled_rectangle(150, 460, 350, 510, button_color_wyjdz);
                    al_draw_text(font1, al_map_rgb(0, 100, 0), width / 2, 465, ALLEGRO_ALIGN_CENTRE, "wroc");

                    al_flip_display();
                }
                else if (states == REJESTRACJA)
                {
                    al_clear_to_color(al_map_rgb(0, 0, 0));
                    al_draw_text(font2, al_map_rgb(255, 0, 255), 435, 10, ALLEGRO_ALIGN_CENTRE, "~ made by dudyn");
                    al_draw_text(font2, al_map_rgb(255, 255, 0), 10, 10, ALLEGRO_ALIGN_LEFT, "rejestracja");

                    if (blad == 1)
                    {
                        al_draw_line(75, 155, 55, 175, al_map_rgb(255, 0, 0), 3);
                        al_draw_line(55, 155, 75, 175, al_map_rgb(255, 0, 0), 3);
                        al_draw_line(75, 285, 55, 305, al_map_rgb(255, 0, 0), 3);
                        al_draw_line(55, 285, 75, 305, al_map_rgb(255, 0, 0), 3);
                    }

                    al_draw_text(font3, al_map_rgb(255, 0, 0), width / 2, 100, ALLEGRO_ALIGN_CENTRE, "podaj nazwe");
                    if (nacisniety_login == 1)
                    {
                        al_draw_filled_rectangle(90, 140, 410, 190, al_map_rgb(255, 255, 255));
                    }
                    else
                    {
                        al_draw_filled_rectangle(90, 140, 410, 190, al_map_rgb(250, 250, 100));
                    }
                    al_draw_text(font1, al_map_rgb(0, 100, 0), width / 2, 145, ALLEGRO_ALIGN_CENTRE, namechange.c_str());

                    al_draw_text(font3, al_map_rgb(255, 0, 0), width / 2, 230, ALLEGRO_ALIGN_CENTRE, "podaj haslo");
                    if (nacisniete_haslo == 1)
                    {
                        al_draw_filled_rectangle(90, 270, 410, 320, al_map_rgb(255, 255, 255));
                    }
                    else
                    {
                        al_draw_filled_rectangle(90, 270, 410, 320, al_map_rgb(250, 250, 100));
                    }
                    
                    if (pokazane_haslo == 1)
                    {
                        al_draw_filled_rectangle(430, 280, 460, 310, al_map_rgb(180, 0, 0));
                        al_draw_text(font4, al_map_rgb(0, 0, 0), 445, 286, ALLEGRO_ALIGN_CENTRE, "ukryj");
                        al_draw_text(font1, al_map_rgb(0, 100, 0), width / 2, 275, ALLEGRO_ALIGN_CENTRE, haslochange.c_str());
                    }
                    else
                    {
                        al_draw_filled_rectangle(430, 280, 460, 310, al_map_rgb(0, 180, 0));
                        al_draw_text(font4, al_map_rgb(0, 0, 0), 445, 286, ALLEGRO_ALIGN_CENTRE, "pokaz");
                        al_draw_text(font1, al_map_rgb(0, 100, 0), width / 2, 275, ALLEGRO_ALIGN_CENTRE, haslogwiazdka.c_str());
                    }

                    al_draw_filled_rectangle(150, 400, 350, 450, button_color_ok);
                    al_draw_text(font1, al_map_rgb(0, 100, 0), width / 2, 405, ALLEGRO_ALIGN_CENTRE, "ok");

                    al_draw_filled_rectangle(150, 460, 350, 510, button_color_wyjdz);
                    al_draw_text(font1, al_map_rgb(0, 100, 0), width / 2, 465, ALLEGRO_ALIGN_CENTRE, "wroc");

                    al_flip_display();
                }
                else if (states == ZMIANANICKU)
                {
                    al_clear_to_color(al_map_rgb(0, 0, 0));
                    al_draw_text(font2, al_map_rgb(255, 0, 255), 435, 10, ALLEGRO_ALIGN_CENTRE, "~ made by dudyn");
                    al_draw_text(font2, al_map_rgb(255, 255, 0), 10, 10, ALLEGRO_ALIGN_LEFT, "aktualna nazwa:");
                    al_draw_text(font2, al_map_rgb(255, 255, 0), 125, 10, ALLEGRO_ALIGN_LEFT, name.c_str());
                    al_draw_text(font3, al_map_rgb(255, 0, 0), width / 2, 150, ALLEGRO_ALIGN_CENTRE, "podaj nowa nazwe");
                    al_draw_filled_rectangle(90, 190, 410, 240, al_map_rgb(255, 255, 255));
                    al_draw_text(font1, al_map_rgb(0, 100, 0), width / 2, 195, ALLEGRO_ALIGN_CENTRE, namechange.c_str());
                    al_draw_filled_rectangle(150, 375, 350, 425, button_color_ok);
                    al_draw_filled_rectangle(150, 445, 350, 495, button_color_wyjdz);
                    al_draw_text(font1, al_map_rgb(0, 100, 0), width / 2, 380, ALLEGRO_ALIGN_CENTRE, "ok");
                    al_draw_text(font1, al_map_rgb(255, 0, 0), width / 2, 450, ALLEGRO_ALIGN_CENTRE, "powrot");
                    al_flip_display();
                }
                else if (states == WYBORY)
                {
                    al_clear_to_color(al_map_rgb(0, 0, 0));
                    al_draw_text(font2, al_map_rgb(255, 0, 255), 435, 10, ALLEGRO_ALIGN_CENTRE, "~ made by dudyn");
                    al_draw_text(font2, al_map_rgb(255, 255, 0), 10, 10, ALLEGRO_ALIGN_LEFT, "aktualna nazwa:");
                    al_draw_text(font2, al_map_rgb(255, 255, 0), 125, 10, ALLEGRO_ALIGN_LEFT, name.c_str());
                    al_draw_filled_rectangle(150, 165, 350, 215, button_color_start);
                    al_draw_filled_rectangle(150, 245, 350, 295, button_color_statystyki);
                    al_draw_filled_rectangle(150, 325, 350, 375, button_color_zmiana_nicku);
                    al_draw_filled_rectangle(150, 405, 350, 455, button_color_wyjdz);
                    al_draw_text(font1, al_map_rgb(0, 255, 0), width / 2, 170, ALLEGRO_ALIGN_CENTRE, "START");
                    al_draw_text(font1, al_map_rgb(0, 0, 125), width / 2, 250, ALLEGRO_ALIGN_CENTRE, "TOP 10");
                    al_draw_text(font1, al_map_rgb(0, 0, 125), width / 2, 330, ALLEGRO_ALIGN_CENTRE, "ZMIANA KONTA");
                    al_draw_text(font1, al_map_rgb(255, 0, 0), width / 2, 410, ALLEGRO_ALIGN_CENTRE, "WYJDZ");
                    al_flip_display();
                }
                else if (states == STATS)
                {
                    al_clear_to_color(al_map_rgb(0, 0, 0));
                    al_draw_text(font2, al_map_rgb(255, 0, 255), 435, 10, ALLEGRO_ALIGN_CENTRE, "~ made by dudyn");
                    al_draw_line(0, 45, 500, 45, al_map_rgb(255, 0, 255), 5);
                    al_draw_text(font1, al_map_rgb(255, 0, 255), width / 2, 50, ALLEGRO_ALIGN_CENTRE, "TOP 10");
                    al_draw_line(0, 95, 500, 95, al_map_rgb(255, 0, 255), 5);
                    al_draw_text(font2, al_map_rgb(255, 255, 0), topposx + 30, topposy - 30, ALLEGRO_ALIGN_LEFT, "NICK");
                    al_draw_text(font2, al_map_rgb(255, 255, 0), topposx + 200, topposy - 30, ALLEGRO_ALIGN_LEFT, "PUNKTY");

                    for (int i = 0; i < 10; i++)
                    {
                        if (topnames[i] != "" || (topnames[i]=="" && toppoints[i]>0))
                        {
                            al_draw_textf(font2, al_map_rgb(255, 255, 0), topposx, topposy + i * 30, ALLEGRO_ALIGN_LEFT, "%i. ", i + 1);
                            al_draw_text(font2, al_map_rgb(255, 255, 0), topposx + 30, topposy + i * 30, ALLEGRO_ALIGN_LEFT, topnames[i].c_str());
                            al_draw_textf(font2, al_map_rgb(255, 255, 0), topposx + 200, topposy + i * 30, ALLEGRO_ALIGN_LEFT, "%i", toppoints[i]);
                        }
                        else
                        {
                            al_draw_textf(font2, al_map_rgb(255, 255, 0), topposx, topposy + i * 30, ALLEGRO_ALIGN_LEFT, "%i. ", i + 1);
                            al_draw_text(font2, al_map_rgb(255, 255, 0), topposx + 30, topposy + i * 30, ALLEGRO_ALIGN_LEFT, "-");
                            al_draw_text(font2, al_map_rgb(255, 255, 0), topposx + 200, topposy + i * 30, ALLEGRO_ALIGN_LEFT, "-");
                        }
                    }

                    al_draw_filled_rectangle(150, 495, 350, 545, button_color_back);
                    al_draw_text(font1, al_map_rgb(0, 100, 0), width / 2, 500, ALLEGRO_ALIGN_CENTRE, "WROC");
                    al_draw_filled_rectangle(403, 498, 447, 542, button_color_statystyki);
                    al_draw_text(font4, al_map_rgb(0, 0, 0), 425, 504, ALLEGRO_ALIGN_CENTRE, "moje");
                    al_draw_text(font4, al_map_rgb(0, 0, 0), 425, 519, ALLEGRO_ALIGN_CENTRE, "staty");
                    al_flip_display();
                }
                else if (states == MOJESTATY)
                {
                al_clear_to_color(al_map_rgb(0, 0, 0));
                al_draw_text(font2, al_map_rgb(255, 0, 255), 435, 10, ALLEGRO_ALIGN_CENTRE, "~ made by dudyn");
                al_draw_line(0, 45, 500, 45, al_map_rgb(255, 0, 255), 5);
                al_draw_text(font1, al_map_rgb(255, 0, 255), width / 2, 50, ALLEGRO_ALIGN_CENTRE, "MOJE STATYSTYKI");
                al_draw_line(0, 95, 500, 95, al_map_rgb(255, 0, 255), 5);

                al_draw_text(font3, al_map_rgb(255, 255, 0), 20, 175, ALLEGRO_ALIGN_LEFT, "minimalna liczba punktow: ");
                al_draw_text(font3, al_map_rgb(255, 255, 0), 20, 225, ALLEGRO_ALIGN_LEFT, "maksymalna liczba punktow: ");
                al_draw_text(font3, al_map_rgb(255, 255, 0), 20, 275, ALLEGRO_ALIGN_LEFT, "srednia liczba punktow: ");
                al_draw_text(font3, al_map_rgb(255, 255, 0), 20, 325, ALLEGRO_ALIGN_LEFT, "suma punktow: ");
                al_draw_text(font3, al_map_rgb(255, 255, 0), 20, 375, ALLEGRO_ALIGN_LEFT, "ilosc rozegranych gier: ");
                al_draw_textf(font3, al_map_rgb(255, 255, 0), 350, 175, ALLEGRO_ALIGN_LEFT, "%i", min_punkty);
                al_draw_textf(font3, al_map_rgb(255, 255, 0), 350, 225, ALLEGRO_ALIGN_LEFT, "%i", max_punkty);
                al_draw_textf(font3, al_map_rgb(255, 255, 0), 350, 275, ALLEGRO_ALIGN_LEFT, "%.2f", srednia_punktow);
                al_draw_textf(font3, al_map_rgb(255, 255, 0), 350, 325, ALLEGRO_ALIGN_LEFT, "%i", suma_punktow);
                al_draw_textf(font3, al_map_rgb(255, 255, 0), 350, 375, ALLEGRO_ALIGN_LEFT, "%i", ilosc_meczow);
                
                al_draw_filled_rectangle(150, 495, 350, 545, button_color_back);
                al_draw_text(font1, al_map_rgb(0, 100, 0), width / 2, 500, ALLEGRO_ALIGN_CENTRE, "WROC");

                al_flip_display();
                }
                else if (states == PLAYING)
                {
                    if (live)
                    {
                        if (key[UP])
                        {
                            key[UP] = false;
                            vely = -jumpSpeed;
                        }

                        vely += gravity;

                        y += vely;

                    }
                }
            }
            else if (ev.timer.source == birdtimer)
            {
                if (states == PLAYING)
                {
                    sourceX += al_get_bitmap_width(bird) / 3;
                    if (sourceX >= al_get_bitmap_width(bird))
                    {
                        sourceX = 0;
                    }
                }
            }
        }
        redraw = true;
        if (redraw && al_is_event_queue_empty(event_queue))
        {
            redraw = false;
            if (live && states == PLAYING)
            {
                StartBlocks(blocks, ile_blokow);
                if (!stop_blocks)
                {
                    UpdateBlocks(blocks, ile_blokow);
                }
                Collision(blocks, ile_blokow, live, x, y, stop_blocks, falling, gotowy_zapis, hit);
                ScoreCounting(blocks, ile_blokow, live, x, y, score,point);
                if (!stop_blocks)
                {
                    al_draw_bitmap_region(bird, sourceX, sourceY, 55, 40, x, y, NULL);
                }
                else
                {
                    falling = true;
                    al_draw_rotated_bitmap(fallingbird, 27, 20, x, y, 90 * 3.14 / 180, NULL);
                    for (int i = 0; i < 3; i++)
                    {
                        trawa[i].speed = 0;
                    }
                }
                al_draw_textf(score_font, al_map_rgb(255, 0, 0), width / 2, 15, ALLEGRO_ALIGN_CENTRE, "%i", score);
                al_flip_display();
                al_draw_bitmap(background, 0, 0, NULL);
                DrawBlocks(blocks, ile_blokow, blok_dolny, blok_gorny);
                DrawFloor(trawa, floor, 3, live);
            }
            else if (!live && states == LOST)
            {
                if (!falling)
                {
                    al_draw_bitmap_region(bird, 0, 0, 55, 40, x, y, ALLEGRO_FLIP_VERTICAL);
                }
                else
                {
                    al_draw_rotated_bitmap(fallingbird, 27, 20, x, y, 90 * 3.14 / 180, NULL);
                }

                if (gotowy_zapis)
                {
                    file.open("zapis.txt", ios::out | ios::app);
                    file << name << endl;
                    file << score << endl;
                    file.close();
                    gotowy_zapis = false;
                    FileReading(max_names, max_points, names, points, file);
                }

                al_draw_text(score_font, al_map_rgb(255, 0, 0), width / 2, 200, ALLEGRO_ALIGN_CENTRE, "koniec gry");
                al_draw_textf(score_font, al_map_rgb(255, 0, 0), width / 2, 250, ALLEGRO_ALIGN_CENTRE, "twoje punkty: %i", score);
                al_draw_text(font3, al_map_rgb(0, 0, 0), width / 2, 420, ALLEGRO_ALIGN_CENTRE, "kliknij spacje aby zagrac ponownie");
                al_draw_text(font3, al_map_rgb(0, 0, 0), width / 2, 450, ALLEGRO_ALIGN_CENTRE, "kliknij backspace aby wrocic do menu");
                al_flip_display();
                al_draw_bitmap(background, 0, 0, NULL);
                DrawBlocks(blocks, ile_blokow, blok_dolny, blok_gorny);
                DrawFloor(trawa, floor, 3, live);
            }
        }
    }
    al_destroy_font(font1);
    al_destroy_font(font2);
    al_destroy_font(font3);
    al_destroy_font(font4);
    al_destroy_font(score_font);
    al_destroy_event_queue(event_queue);
    al_destroy_timer(timer);
    al_destroy_timer(birdtimer);
    al_destroy_bitmap(kac);
    al_destroy_bitmap(fallingbird);
    al_destroy_bitmap(bird);
    al_destroy_bitmap(blok_dolny);
    al_destroy_bitmap(blok_gorny);
    al_destroy_bitmap(floor);
    al_destroy_bitmap(background);
    al_destroy_sample(hit);
    al_destroy_sample(point);
    al_destroy_sample(wing);
    al_destroy_sample(klik);
    al_destroy_sample(przesuniecie);
    al_destroy_display(display);
    return EXIT_SUCCESS;
}

// rysowanie przesuwajacych sie klockow
void DrawBlocks(Block blocks[], int size, ALLEGRO_BITMAP* blok_dolny, ALLEGRO_BITMAP* blok_gorny)
{
    for (int i = 0; i < size; i++)
    {
        if (blocks[i].live)
        {
            al_draw_bitmap_region(blok_dolny, 0, 0, 80, blocks[i].y + blocks[i].boundy, blocks[i].x, blocks[i].y, NULL);
            al_draw_bitmap_region(blok_gorny, 0, 530 - (blocks[i].y - 160), 80, 530, blocks[i].x2, 0, NULL);
        }
    }
}

// przemieszczanie sie i rysowanie trawy
void DrawFloor(Grass trawa[], ALLEGRO_BITMAP* floor, int size, bool& live)
{
    for (int i = 0; i < size; i++)
    {
        if (!trawa[i].live)
        {
            if (i == 0)
            {
                if (!trawa[size - 1].live)
                {
                    trawa[i].x = 0;
                    trawa[i].live = true;
                }
                else
                {
                    if (trawa[size - 1].x + 600 <= width)
                    {
                        trawa[i].x = trawa[size - 1].x + 600;
                        trawa[i].live = true;
                    }
                }
            }
            else
            {
                if (trawa[i - 1].x + 600 <= width && trawa[i - 1].live)
                {
                    trawa[i].x = trawa[i - 1].x + 600;
                    trawa[i].live = true;
                }
            }
        }
        else
        {
            al_draw_bitmap(floor, trawa[i].x, trawa[i].y, NULL);
            trawa[i].x -= trawa[i].speed;
            if (trawa[i].x + 600 < 0)
            {
                trawa[i].live = false;
            }
        }
    }
}

// pojawianie sie nowych klockow w odpowiednim momencie
void StartBlocks(Block blocks[], int size)
{
    for (int i = 0; i < size; i++)
    {
        if (!blocks[i].live)
        {
            if (i == 0)
            {
                if (!blocks[size - 1].live)
                {
                    blocks[i].x = width;
                    blocks[i].y = (rand() % 250) + 201;
                    blocks[i].x2 = width;
                    blocks[i].y2 = 0;
                    blocks[i].boundy = grass_height - blocks[i].y;
                    blocks[i].live = true;
                    blocks[i].score = true;
                }
                else
                {
                    if (blocks[size - 1].x + blocks[size - 1].boundx <= width - 200)
                    {
                        blocks[i].x = width;
                        blocks[i].y = (rand() % 250) + 201;
                        blocks[i].x2 = width;
                        blocks[i].y2 = 0;
                        blocks[i].boundy = grass_height - blocks[i].y;
                        blocks[i].live = true;
                        blocks[i].score = true;
                    }
                }
            }
            else
            {
                if (blocks[i - 1].x + blocks[i - 1].boundx <= width - 200 && blocks[i - 1].live)
                {
                    blocks[i].x = width;
                    blocks[i].y = (rand() % 250) + 201;
                    blocks[i].x2 = width;
                    blocks[i].y2 = 0;
                    blocks[i].boundy = grass_height - blocks[i].y;
                    blocks[i].live = true;
                    blocks[i].score = true;
                }
            }
        }
    }
}

// przesuwanie sie klockow
void UpdateBlocks(Block blocks[], int size)
{
    for (int i = 0; i < size; i++)
    {
        if (blocks[i].live)
        {
            blocks[i].x -= blocks[i].speed;
            blocks[i].x2 -= blocks[i].speed;
            if (blocks[i].x + blocks[i].boundx <= 0)
            {
                blocks[i].live = false;
            }
        }
    }
}

// sprawdzanie czy ptok uderzyl w klocka lub w ziemie
void Collision(Block blocks[], int size, bool& live, int& x, int& y, bool& stop_blocks, bool& falling, bool& gotowy_zapis, ALLEGRO_SAMPLE* hit1)
{
    for (int i = 0; i < size; i++)
    {
        if (blocks[i].live && live)
        {
            if ((x<blocks[i].x + blocks[i].boundx &&
                x + 55>blocks[i].x &&
                y<blocks[i].y + blocks[i].boundy &&
                y + 40>blocks[i].y) ||
                (x<blocks[i].x2 + blocks[i].boundx &&
                    x + 55>blocks[i].x2 &&
                    y<blocks[i].y2 + blocks[i].y - 160 &&
                    y + 40>blocks[i].y2))
            {
                Falling(y, live, stop_blocks, falling, hit1);
                if (gotowyfalling == 1)
                {
                    gotowyfalling = 0; 
                    al_play_sample(hit1, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
                }
            }
            if (falling)
            {
                if (y + 40 > grass_height + 20)
                {
                    al_play_sample(hit1, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
                    live = false;
                    gotowy_zapis = true;
                    ChangeState(states, LOST);
                }
            }
            else
            {
                if (y + 40 > grass_height)
                {
                    al_play_sample(hit1, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
                    live = false;
                    gotowy_zapis = true;
                    ChangeState(states, LOST);
                }
            }
        }
    }
    if (live == false)
    {
        for (int i = 0; i < 3; i++)
        {
            trawa[i].speed = 0;
        }
    }
}

// liczenie punktow
void ScoreCounting(Block blocks[], int size, bool& live, int& x, int& y, int& score, ALLEGRO_SAMPLE* point1)
{
    for (int i = 0; i < size; i++)
    {
        if (live && blocks[i].live && blocks[i].score && x<blocks[i].x + blocks[i].boundx && x + 55>blocks[i].x + (blocks[i].boundx / 2))
        {
            blocks[i].score = false;
            al_play_sample(point1, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
            score++;
        }
    }
}

// spadanie ptoka po uderzeniu w klocka
void Falling(int& y, bool& live, bool& stop_blocks, bool& falling, ALLEGRO_SAMPLE* hit1)
{
    stop_blocks = true;

    if (y + 40 < grass_height)
    {
        y = y + 5;
    }
    if (falling)
    {
        if (y + 40 > grass_height + 20)
        {
            al_play_sample(hit1, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
            live = false;
            ChangeState(states, LOST);
        }
    }
    else
    {
        if (y + 40 > grass_height)
        {
            al_play_sample(hit1, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
            live = false;
            ChangeState(states, LOST);
        }
    }
    if (live == false)
    {
        for (int i = 0; i < 3; i++)
        {
            trawa[i].speed = 0;
        }
    }
}

// restart gry
void RestartGame(Block blocks[], int size, int& x, int& y, bool& live, int& score, bool& stop_blocks, bool& isMenuOpen, float& vely, bool& falling, Grass trawa[], bool& gotowy_zapis)
{
    y = 10;
    vely = 0;
    blocks[0].x = width;
    blocks[0].y = (rand() % 250) + 201;
    blocks[0].x2 = width;
    blocks[0].y2 = 0;
    blocks[0].boundy = grass_height - blocks[0].y;
    for (int i = 0; i < size; i++)
    {
        blocks[i].score = true;
        blocks[i].live = false;
    }
    for (int i = 0; i < 3; i++)
    {
        trawa[i].speed = 3;
    }
    blocks[0].live = true;
    score = 0;
    falling = false;
    stop_blocks = false;
    gotowyfalling = 1;
    live = true;
    gotowy_zapis = true;
}

// czytanie danych z pliku i wypelnianie tablicy top10
void FileReading(string& max_names, int& max_points, string names[], int points[], fstream& file)
{
    file.open("zapis.txt", ios::in);
    if (file.good() == false)
    {
        cout << "Cos poszlo nie tak!";
    }
    else
    {
        string liniaPunkty, liniaNazwy;
        int nr_linii = 0;
        while (!file.eof())
        {
            if (nr_linii % 2 == 0)
            {
                getline(file, liniaNazwy);
                names[nr_linii] = liniaNazwy;
                if (nr_linii == 0)
                {
                    max_names = names[0];
                }
            }
            else
            {
                getline(file, liniaPunkty);
                points[nr_linii - 1] = stoi(liniaPunkty.c_str());
                if (nr_linii == 0)
                {
                    max_points = points[0];
                }
                else
                {
                    if (max_points < points[nr_linii - 1])
                    {
                        max_points = points[nr_linii - 1];
                        max_names = names[nr_linii - 1];
                    }
                }
            }
            nr_linii++;
        }
        file.close();
        //cout << "Rekord: " << endl;
        //cout << "Imie: " << max_names << endl;
        //cout << "Punkty: " << max_points << endl;
        //cout << endl;
        /*
        for (int i = 0; i < 100; i++)
        {
            if (names[i] != "")
            {
                cout << "Imie: " << names[i] << endl;
                cout << "Punkty: " << points[i] << endl;
            }
        }*/

        int tymczasowa[1000];
        string stymczasowa[1000];
        int temp;
        string stemp;

        for (int i = 0; i < 1000; i++)
        {
            tymczasowa[i] = points[i];
            stymczasowa[i] = names[i];
        }
        for (int i = 0; i < 1000; i++)
        {
            for (int j = 0; j < 999; j++)
            {
                if (tymczasowa[j] < tymczasowa[j + 1])
                {
                    temp = tymczasowa[j];
                    stemp = stymczasowa[j];
                    tymczasowa[j] = tymczasowa[j + 1];
                    stymczasowa[j] = stymczasowa[j + 1];
                    tymczasowa[j + 1] = temp;
                    stymczasowa[j + 1] = stemp;
                }
                else if ((tymczasowa[j] == tymczasowa[j + 1]) && stymczasowa[j + 1] != "")
                {
                    temp = tymczasowa[j];
                    stemp = stymczasowa[j];
                    tymczasowa[j] = tymczasowa[j + 1];
                    stymczasowa[j] = stymczasowa[j + 1];
                    tymczasowa[j + 1] = temp;
                    stymczasowa[j + 1] = stemp;
                }
            }
        }
        //cout << endl;
        for (int i = 0; i < 10; i++)
        {
            topnames[i] = stymczasowa[i];
            toppoints[i] = tymczasowa[i];
            //cout << i + 1 << ". " << stymczasowa[i] << " = " << tymczasowa[i] << endl;
        }
    }
}

// zmiana stanu gry
void ChangeState(int& states, int new_state)
{
    states = new_state;

   if (states == PLAYING)
    {
        RestartGame(blocks, ile_blokow, x, y, live, score, stop_blocks, isMenuOpen, vely, falling, trawa, gotowy_zapis);
    }
}

// wczytywanie danych z pliku w celu logowania
void FileReadingLogowanie()
{
    string nazwa;
    string haslo;
    file.open("konta.txt", ios::in);
    if (file.good() == false)
    {
        cout << "Cos poszlo nie tak!";
    }
    else
    {
        string liniaLogin, liniaHaslo;
        int nr_linii = 0;
        while (!file.eof())
        {
            if (nr_linii % 2 == 0)
            {
                getline(file, liniaLogin);
                nazwa = liniaLogin;
            }
            else
            {
                getline(file, liniaHaslo);
                haslo = liniaHaslo;
            }
            if (nazwa == namechange && haslo == haslochange)
            {
                loginhasloready = 1;
            }
            nr_linii++;
        }
        file.close();
    }
}

// zapisywanie do pliku z rejestracji
void FileSavingAccount()
{
    int suma = 0;

    file.open("konta.txt", ios::in);
    if (file.good() == false)
    {
        cout << "Cos poszlo nie tak!";
    }
    else
    {
        string liniaLogin, liniaHaslo;
        int nr_linii = 0;
        while (!file.eof())
        {
            if (nr_linii % 2 == 0)
            {
                getline(file, liniaLogin);
            }
            else
            {
                getline(file, liniaHaslo);
            }

            if (namechange == liniaLogin)
            {
                suma++;
            }

            nr_linii++;
        }

        if (suma == 0)
        {
            gotowarejestracja = 1;
        }
        //cout << suma << endl;
        file.close();

        if (gotowarejestracja == 1)
        {
            file.open("konta.txt", ios::out | ios::app);
            file << namechange << endl;
            file << haslochange << endl;
            file.close();
        }
    }
}

// zagwiazdkowywanie hasla
void GwiazdkowanieHasla()
{
    haslogwiazdka = "";
    for (int i = 0; i < haslochange.length(); i++)
    {
        haslogwiazdka.append("*");
    }
}

// wczytywanie statystyk z pliku
void FileReadingStatystyki()
{
    file.open("zapis.txt", ios::in);
    if (file.good() == false)
    {
        cout << "Cos poszlo nie tak!";
    }
    else
    {
        string liniaNazwa, liniaPunkty;
        int suma = 0, liczba_meczow = 0, max = 0, min = 0;
        int czy_to_poczatek = 1;
        float srednia;
        int nr_linii = 0;
        while (!file.eof())
        {
            if (nr_linii % 2 == 0)
            {
                getline(file, liniaNazwa);
            }
            else
            {
                getline(file, liniaPunkty);
            }

            if (nr_linii % 2 != 0)
            {
                if (name == liniaNazwa)
                {
                    if (czy_to_poczatek == 1)
                    {
                        max = stoi(liniaPunkty.c_str());
                        min = stoi(liniaPunkty.c_str());
                        czy_to_poczatek = 0;
                    }
                    suma = suma + stoi(liniaPunkty.c_str());
                    liczba_meczow++;
                    if (max < stoi(liniaPunkty.c_str()))
                    {
                        max = stoi(liniaPunkty.c_str());
                    }
                    if (min > stoi(liniaPunkty.c_str()))
                    {
                        min = stoi(liniaPunkty.c_str());
                    }
                }
            }
            nr_linii++;
        }
        file.close();
        if (liczba_meczow == 0 || suma == 0)
        {
            srednia = 0;
        }
        else
        {
            srednia = float(suma) / float(liczba_meczow);
        }

        min_punkty = min;
        max_punkty = max;
        srednia_punktow = srednia;
        suma_punktow = suma;
        ilosc_meczow = liczba_meczow;
        //cout << "Min: " << min << endl;
        //cout << "Max: " << max << endl;
        //cout << "Srednia: " << srednia << endl;
        //cout << "Liczba meczow: " << liczba_meczow << endl;
        //cout << "Suma punktow: " << suma << endl;
    }
}

// odtwarzanie dzwieku na klawiszach
void Przesuniecie(ALLEGRO_SAMPLE* przesuniecie1)
{
    if (gotoweprzesuniecie == 1)
    {
        gotoweprzesuniecie = 0;
        al_play_sample(przesuniecie1, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
    }
}

void Przesuniecie2(ALLEGRO_SAMPLE* przesuniecie1)
{
    if (gotoweprzesuniecie2 == 1)
    {
        gotoweprzesuniecie2 = 0;
        al_play_sample(przesuniecie1, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
    }
}

void Przesuniecie3(ALLEGRO_SAMPLE* przesuniecie1)
{
    if (gotoweprzesuniecie3 == 1)
    {
        gotoweprzesuniecie3 = 0;
        al_play_sample(przesuniecie1, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
    }
}

void Przesuniecie4(ALLEGRO_SAMPLE* przesuniecie1)
{
    if (gotoweprzesuniecie4 == 1)
    {
        gotoweprzesuniecie4 = 0;
        al_play_sample(przesuniecie1, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
    }
}

// sprawdzenie czy mozna odtwarzac dzwiek na klawiszach
void CzyGotowePrzesuniecie()
{
    if (gotoweprzesuniecie == 0)
    {
        gotoweprzesuniecie = 1;
    }
}

void CzyGotowePrzesuniecie2()
{
    if (gotoweprzesuniecie2 == 0)
    {
        gotoweprzesuniecie2 = 1;
    }
}

void CzyGotowePrzesuniecie3()
{
    if (gotoweprzesuniecie3 == 0)
    {
        gotoweprzesuniecie3 = 1;
    }
}

void CzyGotowePrzesuniecie4()
{
    if (gotoweprzesuniecie4 == 0)
    {
        gotoweprzesuniecie4 = 1;
    }
}