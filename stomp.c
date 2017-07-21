#include <stdio.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>

#define WINDOW_WIDTH (640)
#define WINDOW_HEIGHT (480)
#define SPRITE_WIDTH (18)
#define SPRITE_HEIGHT (24)
#define TICKS_PER_SECOND (60)
#define ACC (300)
#define SPEED (60)

enum spriteLabel {
    LEFT, RIGHT, DEFAULT
};

struct Entity {
    float x;
    float y;
    float vx;
    float vy;
    struct SDL_Rect dest;
};

float getLeft(struct Entity *e) {
    return e->x;
}

void setLeft(struct Entity *e, float value) {
    e->x = value;
}

float getTop(struct Entity *e) {
    return e->y;
}

void setTop(struct Entity *e, float value) {
    e->y = value;
}

float getRight(struct Entity *e) {
    return e->x +  e->dest.w;
}

void setRight(struct Entity *e, float value) {
    e->x = value -  e->dest.w;
}

float getBottom(struct Entity *e) {
    return e->y +  e->dest.h;
}

void setBottom(struct Entity *e, float value) {
    e->y = value -  e->dest.h;
}

float getMidX(struct Entity *e) {
    return e->x + e->dest.w / 2;
}

void setMidX(struct Entity *e, float value) {
    e->x = value - e->dest.w / 2;
}

float getMidY(struct Entity *e) {
    return e->y + e->dest.h / 2;
}

void setMidY(struct Entity *e, float value) {
    e->y = value - e->dest.h / 2;
}

void move(struct Entity *e) {
    if(e->vx > SPEED) {
        e->vx = SPEED;
    } else if(e->vx < -SPEED) {
        e->vx = -SPEED;
    }
    if(e->vy > SPEED) {
        e->vy = SPEED;
    } else if(e->vy < -SPEED) {
        e->vy = -SPEED;
    }
    e->x += e->vx / TICKS_PER_SECOND;
    e->y += e->vy / TICKS_PER_SECOND;
}

void edges(struct Entity *e) {
    if(getLeft(e) <= 0) {
        setLeft(e, 0);
        e->vx = 0;
    } else if(getRight(e) >= WINDOW_WIDTH) {
        setRight(e, WINDOW_WIDTH);
        e->vx = 0;
    }
    if(getTop(e) <= 0) {
        setTop(e, 0);
        e->vy = 0;
    } else if(getBottom(e) >= WINDOW_HEIGHT) {
        setBottom(e, WINDOW_HEIGHT);
        e->vy = 0;
    }
}

int main(void) {
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* win = SDL_CreateWindow(
        "Stompy",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        0
    );
    if(!win) {
        printf("error creating window: %s\n", SDL_GetError());
        SDL_Quit();
	    return 1;
    }

    Uint32 render_flags = SDL_RENDERER_ACCELERATED;
    SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);
    if(!rend) {
        printf("error creating renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    SDL_Surface* surface = IMG_Load("sprites.png");
    if(!surface) {
        printf("error creating surface\n");
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, surface);
    SDL_FreeSurface(surface);
    if(!tex) {
        printf("error creating texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    SDL_Rect spriteRects [3];
    spriteRects[LEFT].x = 3 * SPRITE_WIDTH;
    spriteRects[LEFT].y = 1 * SPRITE_HEIGHT;
    spriteRects[RIGHT].x = 4 * SPRITE_WIDTH;
    spriteRects[RIGHT].y = 0 * SPRITE_HEIGHT;
    spriteRects[DEFAULT].x = 0 * SPRITE_WIDTH;
    spriteRects[DEFAULT].y = 0 * SPRITE_HEIGHT;
    for(int i=0; i<3; i++) {
        spriteRects[i].w = SPRITE_WIDTH;
        spriteRects[i].h = SPRITE_HEIGHT;
    }

    enum spriteLabel label;

    struct Entity player;
    player.dest.w = SPRITE_WIDTH;
    player.dest.h = SPRITE_HEIGHT;
    setMidX(&player, WINDOW_WIDTH / 2);
    setMidY(&player, WINDOW_HEIGHT / 2);
    player.vx = 0;
    player.vy = 0;

    int up = 0;
    int down = 0;
    int left = 0;
    int right = 0;

    int close_requested = 0;
    
    clock_t start;
    while (!close_requested) {
        start = clock();

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                close_requested = 1;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.scancode)
                {
                case SDL_SCANCODE_W:
                case SDL_SCANCODE_UP:
                    up = 1;
                    break;
                case SDL_SCANCODE_A:
                case SDL_SCANCODE_LEFT:
                    left = 1;
                    break;
                case SDL_SCANCODE_S:
                case SDL_SCANCODE_DOWN:
                    down = 1;
                    break;
                case SDL_SCANCODE_D:
                case SDL_SCANCODE_RIGHT:
                    right = 1;
                    break;
                }
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.scancode)
                {
                case SDL_SCANCODE_W:
                case SDL_SCANCODE_UP:
                    up = 0;
                    break;
                case SDL_SCANCODE_A:
                case SDL_SCANCODE_LEFT:
                    left = 0;
                    break;
                case SDL_SCANCODE_S:
                case SDL_SCANCODE_DOWN:
                    down = 0;
                    break;
                case SDL_SCANCODE_D:
                case SDL_SCANCODE_RIGHT:
                    right = 0;
                    break;
                }
                break;
            }
        }

        if(up) {
            player.vy -= ACC / TICKS_PER_SECOND;
        } else {
            player.vy += ACC / TICKS_PER_SECOND;
        }

        label = DEFAULT;
        if(left) {
            player.vx -= ACC / TICKS_PER_SECOND;
            label = LEFT;
        } else if(right) {
            player.vx += ACC / TICKS_PER_SECOND;
            label = RIGHT;
        }

        move(&player);
        edges(&player);

        player.dest.y = (int) player.y;
        player.dest.x = (int) player.x;
        
        SDL_RenderClear(rend);

        SDL_RenderCopy(rend, tex, &spriteRects[label], &player.dest);
        SDL_RenderPresent(rend);

        SDL_Delay(1000/TICKS_PER_SECOND - ((clock() - start) / CLOCKS_PER_SEC));
    }
    
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
}
