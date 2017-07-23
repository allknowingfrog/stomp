#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>

#include "entity.h"

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

struct Player {
    struct Entity *ent;
    struct Player *prev;
    struct Player *next;
    bool left;
    bool up;
    bool right;
    bool down;
    enum spriteLabel sprite;
};

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

    struct SDL_Rect dest;

    struct Player *first;
    first = (struct Player *) malloc(sizeof(struct Player));
    first->prev = NULL;
    first->next = NULL;
    first->ent = (struct Entity *) malloc(sizeof(struct Entity));
    first->ent->w = SPRITE_WIDTH;
    first->ent->h = SPRITE_HEIGHT;
    setMidX(first->ent, WINDOW_WIDTH / 2);
    setMidY(first->ent, WINDOW_HEIGHT / 2);
    first->ent->vx = 0;
    first->ent->vy = 0;
    first->left = false;
    first->up = false;
    first->right = false;
    first->down = false;
    first->sprite = DEFAULT;

    struct Player *last;
    last = (struct Player *) malloc(sizeof(struct Player));
    last->prev = NULL;
    last->next = NULL;
    last->ent = (struct Entity *) malloc(sizeof(struct Entity));
    last->ent->w = SPRITE_WIDTH;
    last->ent->h = SPRITE_HEIGHT;
    setLeft(last->ent, 0);
    setTop(last->ent, 0);
    last->ent->vx = 0;
    last->ent->vy = 0;
    last->left = false;
    last->up = false;
    last->right = false;
    last->down = false;
    last->sprite = DEFAULT;

    first->next = last;
    last->prev = first;

    struct Player *current = NULL;

    int close_requested = 0;
    
    clock_t start;
    while (!close_requested) {
        start = clock();

        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
            case SDL_QUIT:
                close_requested = 1;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.scancode) {
                case SDL_SCANCODE_LEFT:
                    first->left = true;
                    break;
                case SDL_SCANCODE_UP:
                    first->up = true;
                    break;
                case SDL_SCANCODE_RIGHT:
                    first->right = true;
                    break;
                case SDL_SCANCODE_DOWN:
                    first->down = true;
                    break;
                case SDL_SCANCODE_A:
                    last->left = true;
                    break;
                case SDL_SCANCODE_W:
                    last->up = true;
                    break;
                case SDL_SCANCODE_D:
                    last->right = true;
                    break;
                case SDL_SCANCODE_S:
                    last->down = true;
                    break;
                }
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.scancode) {
                case SDL_SCANCODE_LEFT:
                    first->left = false;
                    break;
                case SDL_SCANCODE_UP:
                    first->up = false;
                    break;
                case SDL_SCANCODE_RIGHT:
                    first->right = false;
                    break;
                case SDL_SCANCODE_DOWN:
                    first->down = false;
                    break;
                case SDL_SCANCODE_A:
                    last->left = false;
                    break;
                case SDL_SCANCODE_W:
                    last->up = false;
                    break;
                case SDL_SCANCODE_D:
                    last->right = false;
                    break;
                case SDL_SCANCODE_S:
                    last->down = false;
                    break;
                }
                break;
            }
        }

        current = first;
        while(current != NULL) {
            if(current->up) {
                current->ent->vy -= ACC / TICKS_PER_SECOND;
            } else {
                current->ent->vy += ACC / TICKS_PER_SECOND;
            }

            current->sprite = DEFAULT;
            if(current->left) {
                current->ent->vx -= ACC / TICKS_PER_SECOND;
                current->sprite = LEFT;
            } else if(current->right) {
                current->ent->vx += ACC / TICKS_PER_SECOND;
                current->sprite = RIGHT;
            }

            current = current->next;
        }

        current = first;
        while(current != NULL) {
            move(current->ent);
            current = current->next;
        }

        if(collides(first->ent, last->ent)) {
            resolve(first->ent, last->ent);
        }

        current = first;
        while(current != NULL) {
            edges(current->ent);
            current = current->next;
        }
        
        SDL_RenderClear(rend);

        current = first;
        while(current != NULL) {
            dest.x = (int) current->ent->x;
            dest.y = (int) current->ent->y;
            dest.w = current->ent->w;
            dest.h = current->ent->h;
            SDL_RenderCopy(rend, tex, &spriteRects[current->sprite], &dest);
            current = current->next;
        }

        SDL_RenderPresent(rend);

        SDL_Delay(1000/TICKS_PER_SECOND - ((clock() - start) / CLOCKS_PER_SEC));
    }
    
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
}
