#include <bits/stdc++.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

using namespace std;

#define lim 100005
#define pub push_back
#define pob pop_back
#define en "\n"

const int SCREEN_WIDTH = 1056;
const int SCREEN_HEIGHT = 704;
const string WINDOW_TITLE = "Minesweeper";

pair<int, int> projector[1000];
int map_width = 30, map_height = 16, bombs = 99, bomb_left = 0;
int input_number = 0;
int navigate = 1;
bool multiplay = true, win_lose = true, running = true, main_logic = true, multicheck = true;
Mix_Music *bgm;

char minesmap[300][300], gamemap[300][300], mapflag[300][300];
int checkpoint[300][300];

//DRAW
void logSDLError(std::ostream& os, const std::string &msg, bool fatal = false);
void initSDL(SDL_Window* &window, SDL_Renderer* &renderer);
void quitSDL(SDL_Window* window, SDL_Renderer* renderer);
void loadmyimage_but_des(SDL_Texture* texture, SDL_Renderer* renderer, int xd, int yd, int width, int height);

//LOGIC
void start();
bool endgame(SDL_Renderer* renderer, bool result);
void endgame_output(SDL_Renderer* renderer);
bool multiplay_check(SDL_Window* window, SDL_Renderer* renderer);
void bomb_generate(int step1_x, int step1_y);
void complete_map();
void spreadmap(SDL_Renderer* renderer, int destix, int destiy, int xx, int yy);

//GAME MOVE
void board_output(SDL_Renderer* renderer, SDL_Event mymouse, int destix, int destiy, int mapx, int mapy);
void spread_output(SDL_Renderer* renderer, int destix, int destiy, int mapx, int mapy);
void board_input(SDL_Renderer* renderer, SDL_Window* window);
bool newgame(SDL_Event mousemotion, SDL_Renderer* renderer, string file1, string file2, int desx, int desy, int widthx, int heighty, SDL_Window* window);
bool chose_level(SDL_Event mymouse, SDL_Renderer* renderer, SDL_Window* window);
void count_bombs(SDL_Renderer* renderer);

int main(int argc, char* argv[])
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Event mousemotion;
    initSDL(window, renderer);

    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ) logSDLError(cout, "audio error", false);
    bgm = Mix_LoadMUS("game_resources/bgm.mp3");
    Mix_PlayMusic(bgm, -1);

    SDL_Texture* texture1 = IMG_LoadTexture(renderer, "game_resources/background.png");
    SDL_Texture* texture2 = nullptr;
    SDL_Texture* bruh = nullptr;
    loadmyimage_but_des(texture1, renderer, 0, 0, 1056, 704);
    SDL_RenderPresent(renderer);

    newgame(mousemotion, renderer, "game_resources/new_game_frame.png", "game_resources/new_game.png", 50, 50, 400, 200, window);
    SDL_RenderClear(renderer);
    texture1 = IMG_LoadTexture(renderer, "game_resources/background.png");
    loadmyimage_but_des(texture1, renderer, 0, 0, 1056, 704);

    while(main_logic == true)
    {
        if(chose_level(mousemotion, renderer, window) == false) break;
    	SDL_RenderClear(renderer);
    	start();
    	input_number = 0;

        texture1 = IMG_LoadTexture(renderer, "game_resources/background.png");
        loadmyimage_but_des(texture1, renderer, 0, 0, 1056, 704);
        bruh = IMG_LoadTexture(renderer, "game_resources/bruh.png");
        loadmyimage_but_des(bruh, renderer, SCREEN_WIDTH/2 - 55, 20, 110, 110);
        texture2 = IMG_LoadTexture(renderer, "game_resources/block.png");
        for(int i = 0; i < map_width; ++i)
            for(int j = 0; j < map_height; ++j)
                loadmyimage_but_des(texture2, renderer, 48 + i*32, 150 + j*32, 32, 32);

        count_bombs(renderer);
        SDL_RenderPresent(renderer);
        board_input(renderer, window);
        if(running == false) break;
        if(multiplay_check(window, renderer) == false) break;
        SDL_RenderClear(renderer);
        texture1 = IMG_LoadTexture(renderer, "game_resources/background.png");
        loadmyimage_but_des(texture1, renderer, 0, 0, 1056, 704);
        if(newgame(mousemotion, renderer, "game_resources/new_game_frame.png", "game_resources/new_game.png", 50, 50, 400, 200, window) == false) break;
    }

    Mix_FreeMusic(bgm);
    SDL_DestroyTexture(texture1);
	SDL_DestroyTexture(texture2);
    SDL_DestroyTexture(bruh);
    quitSDL(window, renderer);
    return 0;
}

void logSDLError(std::ostream& os, const std::string &msg, bool fatal)
{
    os << msg << " Error: " << SDL_GetError() << std::endl;
    if (fatal) {
        SDL_Quit();
        exit(1);
    }
}

void initSDL(SDL_Window* &window, SDL_Renderer* &renderer)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        logSDLError(std::cout, "SDL_Init", true);
    window = SDL_CreateWindow(WINDOW_TITLE.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr)
        logSDLError(std::cout, "CreateWindow", true);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr)
        logSDLError(std::cout, "CreateRenderer", true);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

    SDL_Surface* iconimg = IMG_Load("game_resources/icon.png");
    SDL_SetWindowIcon(window, iconimg);
    SDL_FreeSurface(iconimg);
}

void quitSDL(SDL_Window* window, SDL_Renderer* renderer)
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void loadmyimage_but_des(SDL_Texture* texture, SDL_Renderer* renderer, int xd, int yd, int width, int height)
{
    SDL_Rect rect;
    rect.x = xd;
    rect.y = yd;
    rect.w = width;
    rect.h = height;
    SDL_RenderCopy(renderer, texture, NULL, &rect);
}

void start()
{
    input_number = 0;
    navigate = 1;
    multiplay = true; win_lose = true;
    for(int i = 0; i < map_height; ++i)
        for(int j = 0; j < map_width; ++j)
        {
            gamemap[i][j] = '#';
            minesmap[i][j] = '#';
            mapflag[i][j] = '#';
            checkpoint[i][j] = 0;
        }
}

bool endgame(SDL_Renderer* renderer, bool result)
{
    SDL_Delay(250);
    SDL_Texture* texture = nullptr;
    if(input_number == 0) return result;
    else
    {
        if(result == 1) texture = IMG_LoadTexture(renderer, "game_resources/you_win.png");   // win
        if(result == 0) texture = IMG_LoadTexture(renderer, "game_resources/you_lose.png");  // lose
        loadmyimage_but_des(texture, renderer, 50, 20, 350, 110);
        endgame_output(renderer);
        SDL_RenderPresent(renderer);
        return result;
	}
	SDL_DestroyTexture(texture);
}

void endgame_output(SDL_Renderer* renderer)
{
    SDL_Texture* texture2 = IMG_LoadTexture(renderer, "game_resources/block.png");
    string endfile;
    for(int i = 0; i < map_width; ++i)
        for(int j = 0; j < map_height; ++j)
        {
        	if(minesmap[j][i] == '0') endfile = "game_resources/pixil-0.png";
        	else if(minesmap[j][i] == '1') endfile = "game_resources/pixil-1.png";
        	else if(minesmap[j][i] == '2') endfile = "game_resources/pixil-2.png";
        	else if(minesmap[j][i] == '3') endfile = "game_resources/pixil-3.png";
        	else if(minesmap[j][i] == '4') endfile = "game_resources/pixil-4.png";
        	else if(minesmap[j][i] == '5') endfile = "game_resources/pixil-5.png";
        	else if(minesmap[j][i] == '6') endfile = "game_resources/pixil-6.png";
        	else if(minesmap[j][i] == '7') endfile = "game_resources/pixil-7.png";
        	else if(minesmap[j][i] == '8') endfile = "game_resources/pixil-8.png";
        	else if(minesmap[j][i] == '*') endfile = "game_resources/mine.png";
        	SDL_Texture* texture2 = IMG_LoadTexture(renderer, endfile.c_str());
            loadmyimage_but_des(texture2, renderer, 48 + i*32, 150 + j*32, 32, 32);
        }
	SDL_RenderPresent(renderer);
	SDL_DestroyTexture(texture2);
}

bool multiplay_check(SDL_Window* window, SDL_Renderer* renderer)
{
	const Uint8 *keyState;
    keyState = SDL_GetKeyboardState(NULL);
	SDL_Event Nmouse;
    while(multicheck == true)
	{
		if(SDL_WaitEvent(&Nmouse) != 0 && (keyState[SDL_SCANCODE_ESCAPE] || Nmouse.type == SDL_QUIT))
        {
        	multicheck = false;
            quitSDL(window, renderer);
            return 0;
        }
        if(keyState[SDL_SCANCODE_O])
            if(!Mix_PlayingMusic()) Mix_PlayMusic(bgm, -1);
        if(keyState[SDL_SCANCODE_P]) Mix_HaltMusic();
    	else if(Nmouse.type == SDL_MOUSEBUTTONDOWN && Nmouse.button.button == SDL_BUTTON_LEFT && (Nmouse.button.x > 473 && Nmouse.button.x < 583 && Nmouse.button.y > 20 && Nmouse.button.y < 130))
        {multicheck = true; break;}
	}
	return multicheck;
}

void bomb_generate(int step1_x, int step1_y)
{
    srand(time(NULL));
    int cntb = 0, x, y;
    while(cntb < bombs)
    {
        x = rand() % map_height;
        y = rand() % map_width;
        if(minesmap[x][y] == '#')
        {
        	bool condition = 1;
        	for(int i = step1_x - 1; i <= step1_x + 1; ++i)
        		for(int j = step1_y - 1; j <= step1_y + 1; ++j)
        			if(x == i && y == j && i >= 0 && j >= 0 && i < map_height && j < map_width) condition = 0;
        	if(condition == 1)
        	{
        		minesmap[x][y] = '*';
           		cntb++;
        	}
        }
        if(cntb == bombs) break;
    }
    for(int i = 0; i < map_height; ++i)
        for(int j = 0; j < map_width; ++j)
            gamemap[i][j] = minesmap[i][j];
}

void complete_map()
{
    for(int i = 0; i < map_height; ++i)
        for(int j = 0; j < map_width; ++j)
            if(minesmap[i][j] == '#')
            {
                int count_mines = 0;
                for(int high = i - 1; high <= i + 1; ++high)
                    for(int col = j - 1; col <= j + 1; ++col)
                    {
                        if(minesmap[high][col] == '*' && high >= 0 && col >= 0 && high < map_height && col < map_width) count_mines++;
                    }
                minesmap[i][j] = count_mines + '0';
            }
}

void spreadmap(SDL_Renderer* renderer, int destix, int destiy, int xx, int yy)
{
	checkpoint[xx][yy] = 1;
	gamemap[xx][yy] = minesmap[xx][yy];
    for(int i = xx - 1; i <= xx + 1; ++i)
        for(int j = yy - 1; j <= yy + 1; ++j)
            if(i >= 0 && j >= 0 && i < map_height && j < map_width)
            {
                destix = j*32 + 48;
                destiy = i*32 + 150;
                if(minesmap[i][j] == '0' && i >= 0 && j >= 0 && checkpoint[i][j] == 0)
                {
                    spread_output(renderer, destix, destiy, i, j);
                    spreadmap(renderer, destix, destiy, i, j);
                    gamemap[i][j] = minesmap[i][j];
                    checkpoint[i][j] = 1;
                }
                if(i >= 0 && j >= 0 && checkpoint[i][j] == 0 && minesmap[i][j] != '0' && mapflag[i][j] != 'b')
                {
                    spread_output(renderer, destix, destiy, i, j);
                    gamemap[i][j] = minesmap[i][j];
                    checkpoint[i][j] = 1;
                }
            }
}

void board_output(SDL_Renderer* renderer, SDL_Event mymouse, int destix, int destiy, int mapx, int mapy)
{
    string filepath;
    if(mymouse.button.button == SDL_BUTTON_RIGHT && input_number != 0)
    {
        if(mapflag[mapx][mapy] == '#' && checkpoint[mapx][mapy] == 0)
        {
            filepath = "game_resources/flag.png";
            mapflag[mapx][mapy] = 'b';
            navigate = SDL_BUTTON_RIGHT;
            count_bombs(renderer);
        }
        else return;
    }
    else if(mymouse.button.button == SDL_BUTTON_MIDDLE && input_number != 0)
    {
        if(mapflag[mapx][mapy] == 'b')
        {
            filepath = "game_resources/block.png";
            mapflag[mapx][mapy] = '#';
            checkpoint[mapx][mapy] = 0;
            navigate = SDL_BUTTON_MIDDLE;
            count_bombs(renderer);
        }
        else if(mapflag[mapx][mapy] == '#') return;
    }
    else if(mymouse.button.button == SDL_BUTTON_LEFT or input_number == 0)
    {
        if(mapflag[mapx][mapy] == 'b') return;
    	else if(minesmap[mapx][mapy] == '0')
    	{
    		filepath = "game_resources/pixil-0.png";
    		spreadmap(renderer, destix, destiy, mapx, mapy);
    	}
        else if(minesmap[mapx][mapy] == '1') filepath = "game_resources/pixil-1.png";
        else if(minesmap[mapx][mapy] == '2') filepath = "game_resources/pixil-2.png";
        else if(minesmap[mapx][mapy] == '3') filepath = "game_resources/pixil-3.png";
        else if(minesmap[mapx][mapy] == '4') filepath = "game_resources/pixil-4.png";
        else if(minesmap[mapx][mapy] == '5') filepath = "game_resources/pixil-5.png";
        else if(minesmap[mapx][mapy] == '6') filepath = "game_resources/pixil-6.png";
        else if(minesmap[mapx][mapy] == '7') filepath = "game_resources/pixil-7.png";
        else if(minesmap[mapx][mapy] == '8') filepath = "game_resources/pixil-8.png";
        else if(minesmap[mapx][mapy] == '*') filepath = "game_resources/mine.png";
        gamemap[mapx][mapy] = minesmap[mapx][mapy];
        checkpoint[mapx][mapy] = 1;
        navigate = SDL_BUTTON_LEFT;
        int cntflag = 0;
        for(int i = mapx - 1; i <= mapx + 1; ++i)
            for(int j = mapy - 1; j <= mapy + 1; ++j)
                if(i >= 0 && j >= 0 && i < map_height && j < map_width)
                    if(mapflag[i][j] == 'b') cntflag++;
        if(minesmap[mapx][mapy] - '0' == cntflag && checkpoint[mapx][mapy] == 1) spreadmap(renderer, destix, destiy, mapx, mapy);
    }

    SDL_Texture* mycell = IMG_LoadTexture(renderer, filepath.c_str());
    loadmyimage_but_des(mycell, renderer, destix, destiy, 32, 32);
    SDL_RenderPresent(renderer);
    if(filepath == "game_resources/mine.png") {endgame(renderer, false); win_lose = false;}
    SDL_DestroyTexture(mycell);
}

void spread_output(SDL_Renderer* renderer, int destix, int destiy, int mapx, int mapy)
{
	string filepath;
	if(minesmap[mapx][mapy] == '0') filepath = "game_resources/pixil-0.png";
    else if(minesmap[mapx][mapy] == '1') filepath = "game_resources/pixil-1.png";
    else if(minesmap[mapx][mapy] == '2') filepath = "game_resources/pixil-2.png";
    else if(minesmap[mapx][mapy] == '3') filepath = "game_resources/pixil-3.png";
    else if(minesmap[mapx][mapy] == '4') filepath = "game_resources/pixil-4.png";
    else if(minesmap[mapx][mapy] == '5') filepath = "game_resources/pixil-5.png";
    else if(minesmap[mapx][mapy] == '6') filepath = "game_resources/pixil-6.png";
    else if(minesmap[mapx][mapy] == '7') filepath = "game_resources/pixil-7.png";
    else if(minesmap[mapx][mapy] == '8') filepath = "game_resources/pixil-8.png";
    else if(minesmap[mapx][mapy] == '*' && mapflag[mapx][mapy] != 'b') filepath = "game_resources/mine.png";
    else filepath = "game_resources/flag.png";
    navigate = SDL_BUTTON_LEFT;
    SDL_Texture* mycell = IMG_LoadTexture(renderer, filepath.c_str());
    loadmyimage_but_des(mycell, renderer, destix, destiy, 32, 32);
    SDL_RenderPresent(renderer);
    if(filepath == "game_resources/mine.png") {endgame(renderer, false); win_lose = false;}
    SDL_DestroyTexture(mycell);
}

void board_input(SDL_Renderer* renderer, SDL_Window* window)
{
    SDL_Event mymouse;
    const Uint8 *keyState;
    keyState = SDL_GetKeyboardState(NULL);

    while(running == true)
    {
        if(keyState[SDL_SCANCODE_ESCAPE] or mymouse.type == SDL_QUIT)
        {
        	running = false;
        	quitSDL(window, renderer);
        }
        else if(SDL_WaitEvent(&mymouse) != 0 && (mymouse.type == SDL_MOUSEBUTTONDOWN))
        {
        	if(mymouse.button.x > 48 && mymouse.button.x <= (48 + 32*map_width) && mymouse.button.y > 150 && mymouse.button.y <= (150 + 32*map_height))
            {
                int mx = mymouse.button.x - (mymouse.button.x - 48) % 32;
                int my = mymouse.button.y - (mymouse.button.y - 150) % 32;
                projector[input_number].second = (mx - 48)/32;
                projector[input_number].first = (my - 150)/32;
                int xx = projector[input_number].first;        //descartes
                int yy = projector[input_number].second;
                gamemap[xx][yy] = minesmap[xx][yy];
                if(input_number == 0)
                {
                    start();
                    bomb_generate(xx, yy);
                    complete_map();
                }
                board_output(renderer, mymouse, mx, my, xx, yy);
                cout << projector[input_number].first + 1 << ' ' << projector[input_number].second + 1 << en;
                input_number++;
                
                int wincheck = 0;
                for(int i = 0; i < map_height; ++i)
                    for(int j = 0; j < map_width; ++j)
                        if(gamemap[i][j] != '*' && gamemap[i][j] == '#') wincheck++;
                if(wincheck == 0 && win_lose == true) {endgame(renderer, true); return;}
                
                int winflag = 0;
                for(int i = 0; i < map_height; ++i)
                    for(int j = 0; j < map_width; ++j)
                        if(mapflag[i][j] == 'b' && minesmap[i][j] == '*') winflag++;
                if(winflag == bombs && win_lose == true) {endgame(renderer, true); return;}
                
                else if(win_lose == false) break;
            }
            else if(mymouse.button.button == SDL_BUTTON_LEFT && mymouse.button.x > 473 && mymouse.button.x < 583 && mymouse.button.y > 20 && mymouse.button.y < 130)
            {
                endgame(renderer, false);
                return;
            }
        }

        if(keyState[SDL_SCANCODE_O])
            if(!Mix_PlayingMusic()) Mix_PlayMusic(bgm, -1);
        if(keyState[SDL_SCANCODE_P]) Mix_HaltMusic();
    }
    return;
}

bool newgame(SDL_Event mousemotion, SDL_Renderer* renderer, string file1, string file2, int desx, int desy, int widthx, int heighty, SDL_Window* window)
{
	SDL_RenderClear(renderer);
	SDL_Texture* background = IMG_LoadTexture(renderer, "game_resources/background.png");
	loadmyimage_but_des(background, renderer, 0, 0, 1056, 704);
	SDL_RenderPresent(renderer);
    const Uint8 *keyState;
    keyState = SDL_GetKeyboardState(NULL);
    SDL_Surface *surface1 = IMG_Load(file1.c_str());
    SDL_Surface *surface2 = IMG_Load(file2.c_str());
    SDL_Texture* current1 = SDL_CreateTextureFromSurface(renderer, surface1);
    SDL_Texture* current2 = SDL_CreateTextureFromSurface(renderer, surface2);

    loadmyimage_but_des(current1, renderer, 50, 50, widthx, heighty);
    SDL_RenderPresent(renderer);
    bool check = true;
    while(check == true)
	{
		if(SDL_WaitEvent(&mousemotion) != 0 && (keyState[SDL_SCANCODE_ESCAPE] || mousemotion.type == SDL_QUIT))
        {
            main_logic = false;
        	quitSDL(window, renderer);
            return false;
        }
        if(keyState[SDL_SCANCODE_O])
            if(!Mix_PlayingMusic()) Mix_PlayMusic(bgm, -1);
        if(keyState[SDL_SCANCODE_P]) Mix_HaltMusic();
		else if(mousemotion.type == SDL_MOUSEMOTION)
		{
		    if(mousemotion.button.x > desx && mousemotion.button.x < (widthx+desx) && mousemotion.button.y > desy && mousemotion.button.y < (heighty+desy))
            {
            	loadmyimage_but_des(current1, renderer, desx, desy, widthx, heighty);
                SDL_RenderPresent(renderer);
            }
        	else
        	{
        		loadmyimage_but_des(current2, renderer, desx, desy, widthx, heighty);
                SDL_RenderPresent(renderer);
        	}
    	}
    	else if(mousemotion.type == SDL_MOUSEBUTTONUP && (mousemotion.button.x > desx && mousemotion.button.x < (widthx+desx) && mousemotion.button.y > desy && mousemotion.button.y < (heighty+desy)))
        {
			return true;
            break;
        }
    }
    SDL_DestroyTexture(current1);
    SDL_DestroyTexture(current2);
    SDL_DestroyTexture(background);
    SDL_FreeSurface(surface1);
    SDL_FreeSurface(surface2);
}

bool chose_level(SDL_Event mymouse, SDL_Renderer* renderer, SDL_Window* window)
{
	SDL_RenderClear(renderer);
	SDL_Texture* background = IMG_LoadTexture(renderer, "game_resources/background.png");
	loadmyimage_but_des(background, renderer, 0, 0, 1056, 704);
    mymouse.type = NULL;
	const Uint8 *keyState;
    keyState = SDL_GetKeyboardState(NULL);

    SDL_Surface *easy1 = IMG_Load("game_resources/easy.png");
    SDL_Surface *easy2 = IMG_Load("game_resources/easyframe.png");
    SDL_Texture* easy1t = SDL_CreateTextureFromSurface(renderer, easy1);
    SDL_Texture* easy2t = SDL_CreateTextureFromSurface(renderer, easy2);

    SDL_Surface *medium1 = IMG_Load("game_resources/medium.png");
    SDL_Surface *medium2 = IMG_Load("game_resources/mediumframe.png");
    SDL_Texture* medium1t = SDL_CreateTextureFromSurface(renderer, medium1);
    SDL_Texture* medium2t = SDL_CreateTextureFromSurface(renderer, medium2);

    SDL_Surface *hard1 = IMG_Load("game_resources/hard.png");
    SDL_Surface *hard2 = IMG_Load("game_resources/hardframe.png");
    SDL_Texture* hard1t = SDL_CreateTextureFromSurface(renderer, hard1);
    SDL_Texture* hard2t = SDL_CreateTextureFromSurface(renderer, hard2);

    bool check = true;
    while(check == true)
	{
		if(SDL_WaitEvent(&mymouse) != 0 && (keyState[SDL_SCANCODE_ESCAPE] || mymouse.type == SDL_QUIT))
        {
        	quitSDL(window, renderer);
            return false;
        }
        if(keyState[SDL_SCANCODE_O])
            if(!Mix_PlayingMusic()) Mix_PlayMusic(bgm, -1);
        if(keyState[SDL_SCANCODE_P]) Mix_HaltMusic();
		else if(mymouse.type == SDL_MOUSEMOTION)
		{
			if(mymouse.button.x > 328 && mymouse.button.x < 728 && mymouse.button.y > 25 && mymouse.button.y < 225)
			{
				loadmyimage_but_des(easy1t, renderer, 328, 25, 400, 200);
				SDL_RenderPresent(renderer);
			}
			else
			{
				loadmyimage_but_des(easy2t, renderer, 328, 25, 400, 200);
				SDL_RenderPresent(renderer);
			}

			if(mymouse.button.x > 328 && mymouse.button.x < 728 && mymouse.button.y > 250 && mymouse.button.y < 450)
			{
				loadmyimage_but_des(medium1t, renderer, 328, 250, 400, 200);
				SDL_RenderPresent(renderer);
			}
			else
			{
				loadmyimage_but_des(medium2t, renderer, 328, 250, 400, 200);
				SDL_RenderPresent(renderer);
			}

			if(mymouse.button.x > 328 && mymouse.button.x < 728 && mymouse.button.y > 475 && mymouse.button.y < 675)
			{
				loadmyimage_but_des(hard1t, renderer, 328, 475, 400, 200);
				SDL_RenderPresent(renderer);
			}
			else
			{
				loadmyimage_but_des(hard2t, renderer, 328, 475, 400, 200);
				SDL_RenderPresent(renderer);
			}
		}
		else if(mymouse.type == SDL_MOUSEBUTTONUP && mymouse.button.button == SDL_BUTTON_LEFT)
		{
			if(mymouse.button.x > 328 && mymouse.button.x < 728 && mymouse.button.y > 25 && mymouse.button.y < 225)
				{map_width = 10; map_height = 10; bombs = 12; break;}
			else if(mymouse.button.x > 328 && mymouse.button.x < 728 && mymouse.button.y > 250 && mymouse.button.y < 450)
				{map_width = 16; map_height = 16; bombs = 40; break;}
			else if(mymouse.button.x > 328 && mymouse.button.x < 728 && mymouse.button.y > 475 && mymouse.button.y < 675)
				{map_width = 30; map_height = 16; bombs = 99; break;}
		}
	}

	SDL_FreeSurface(easy1); SDL_FreeSurface(easy2); SDL_FreeSurface(medium1); SDL_FreeSurface(medium2); SDL_FreeSurface(hard1); SDL_FreeSurface(hard2);
	SDL_DestroyTexture(easy1t); SDL_DestroyTexture(easy2t); SDL_DestroyTexture(medium1t); SDL_DestroyTexture(medium2t); SDL_DestroyTexture(hard1t); SDL_DestroyTexture(hard2t);
	SDL_DestroyTexture(background);
	return true;
}

void count_bombs(SDL_Renderer* renderer)
{
    int temp, copytemp;
    bomb_left = 0;
    string filepath;
    for(int i = 0; i < map_height; ++i)
        for(int j = 0; j < map_width; ++j)
            if(mapflag[i][j] == 'b') bomb_left++;
    SDL_Texture* flag_screen1;
    SDL_Texture* flag_screen2;

    copytemp = bombs - bomb_left;
    temp = copytemp % 10;
    if(temp == 0) filepath = "game_resources/flag0.png";
    else if(temp == 1) filepath = "game_resources/flag1.png";
    else if(temp == 2) filepath = "game_resources/flag2.png";
    else if(temp == 3) filepath = "game_resources/flag3.png";
    else if(temp == 4) filepath = "game_resources/flag4.png";
    else if(temp == 5) filepath = "game_resources/flag5.png";
    else if(temp == 6) filepath = "game_resources/flag6.png";
    else if(temp == 7) filepath = "game_resources/flag7.png";
    else if(temp == 8) filepath = "game_resources/flag8.png";
    else if(temp == 9) filepath = "game_resources/flag9.png";

    flag_screen1 = IMG_LoadTexture(renderer, filepath.c_str());
    loadmyimage_but_des(flag_screen1, renderer, 898, 20, 110, 110);

    temp = copytemp / 10;
    if(temp == 0) filepath = "game_resources/flag0.png";
    else if(temp == 1) filepath = "game_resources/flag1.png";
    else if(temp == 2) filepath = "game_resources/flag2.png";
    else if(temp == 3) filepath = "game_resources/flag3.png";
    else if(temp == 4) filepath = "game_resources/flag4.png";
    else if(temp == 5) filepath = "game_resources/flag5.png";
    else if(temp == 6) filepath = "game_resources/flag6.png";
    else if(temp == 7) filepath = "game_resources/flag7.png";
    else if(temp == 8) filepath = "game_resources/flag8.png";
    else if(temp == 9) filepath = "game_resources/flag9.png";

    flag_screen2 = IMG_LoadTexture(renderer, filepath.c_str());
    loadmyimage_but_des(flag_screen2, renderer, 788, 20, 110, 110);
    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(flag_screen1);
    SDL_DestroyTexture(flag_screen2);
}
