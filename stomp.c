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
#define SPRITE_WIDTH (40)
#define SPRITE_HEIGHT (40)
#define TICKS_PER_SECOND (60)
#define ACC (300)
#define DCC (2)
#define MAX_SPEED (100)
#define MIN_SPEED (.1)
#define JUMP_TIME (.2)
#define JUMP_SPEED (175)

enum sprites {
    SPRITE_FACE, SPRITE_LEFT, SPRITE_RIGHT, SPRITE_CRATE
};

struct Player {
    struct Entity *ent;
    struct Player *prev;
    struct Player *next;
    bool left;
    bool up;
    bool right;
    bool down;
    enum sprites sprite;
    bool falling;
    float jumpTimer;
};

void move(struct Entity *e) {
    if(e->vx > MAX_SPEED) {
        e->vx = MAX_SPEED;
    } else if(e->vx < -MAX_SPEED) {
        e->vx = -MAX_SPEED;
    }
    e->x += e->vx / TICKS_PER_SECOND;
    e->y += e->vy / TICKS_PER_SECOND;
}

enum edges bounds(struct Entity *e) {
    enum edges edge = EDGE_NONE;
    if(getLeft(e) <= 0) {
        setLeft(e, 0);
        e->vx = 0;
        edge = EDGE_LEFT;
    } else if(getRight(e) >= WINDOW_WIDTH) {
        setRight(e, WINDOW_WIDTH);
        e->vx = 0;
        edge = EDGE_RIGHT;
    }
    if(getTop(e) <= 0) {
        setTop(e, 0);
        e->vy = 0;
        edge = EDGE_TOP;
    } else if(getBottom(e) >= WINDOW_HEIGHT) {
        setBottom(e, WINDOW_HEIGHT);
        e->vy = 0;
        edge = EDGE_BOTTOM;
    }

    return edge;
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

    SDL_Rect spriteRects[4];
    spriteRects[SPRITE_FACE].x = 0 * SPRITE_WIDTH;
    spriteRects[SPRITE_FACE].y = 0 * SPRITE_HEIGHT;
    spriteRects[SPRITE_LEFT].x = 1 * SPRITE_WIDTH;
    spriteRects[SPRITE_LEFT].y = 0 * SPRITE_HEIGHT;
    spriteRects[SPRITE_RIGHT].x = 2 * SPRITE_WIDTH;
    spriteRects[SPRITE_RIGHT].y = 0 * SPRITE_HEIGHT;
    spriteRects[SPRITE_CRATE].x = 3 * SPRITE_WIDTH;
    spriteRects[SPRITE_CRATE].y = 0 * SPRITE_HEIGHT;
    for(int i=0; i<4; i++) {
        spriteRects[i].w = SPRITE_WIDTH;
        spriteRects[i].h = SPRITE_HEIGHT;
    }

    int BLOCK_COUNT = 5;
    struct Entity *blocks[BLOCK_COUNT];
    for(int i=0; i<BLOCK_COUNT; i++) {
        blocks[i] = (struct Entity *) malloc(sizeof(struct Entity));
        blocks[i]->w = SPRITE_WIDTH;
        blocks[i]->h = SPRITE_HEIGHT;
        setLeft(blocks[i], SPRITE_WIDTH * (i + 2));
        setBottom(blocks[i], WINDOW_HEIGHT - SPRITE_HEIGHT * (i * 2));
        blocks[i]->vx = 0;
        blocks[i]->vy = 0;
    }

    struct SDL_Rect dest;

    struct Player *first;
    first = (struct Player *) malloc(sizeof(struct Player));
    first->prev = NULL;
    first->next = NULL;
    first->ent = (struct Entity *) malloc(sizeof(struct Entity));
    first->ent->w = SPRITE_WIDTH;
    first->ent->h = SPRITE_HEIGHT;
    setRight(first->ent, WINDOW_WIDTH);
    setBottom(first->ent, WINDOW_HEIGHT);
    first->ent->vx = 0;
    first->ent->vy = 0;
    first->left = false;
    first->up = false;
    first->right = false;
    first->down = false;
    first->sprite = SPRITE_FACE;
    first->falling = true;
    first->jumpTimer = 0.0;

    struct Player *last;
    last = (struct Player *) malloc(sizeof(struct Player));
    last->prev = NULL;
    last->next = NULL;
    last->ent = (struct Entity *) malloc(sizeof(struct Entity));
    last->ent->w = SPRITE_WIDTH;
    last->ent->h = SPRITE_HEIGHT;
    setMidX(last->ent, WINDOW_WIDTH / 2);
    setBottom(last->ent, WINDOW_HEIGHT);
    last->ent->vx = 0;
    last->ent->vy = 0;
    last->left = false;
    last->up = false;
    last->right = false;
    last->down = false;
    last->sprite = SPRITE_FACE;
    last->falling = true;
    last->jumpTimer = 0.0;

    first->next = last;
    last->prev = first;

    struct Player *current = NULL;

    enum edges edge = EDGE_NONE;

    int close_requested = 0;
    
    clock_t start;
    while(!close_requested) {
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
            if(current->up && !current->falling) {
                current->ent->vy = -JUMP_SPEED;
                current->jumpTimer = JUMP_TIME;
            } else if(current->up && current->jumpTimer > 0) {
                current->jumpTimer -= 1.0 / TICKS_PER_SECOND;
            } else {
                current->jumpTimer = 0;
                if(!current->up && current->ent->vy < 0) {
                    current->ent->vy -= DCC * current->ent->vy / TICKS_PER_SECOND;
                }
                current->ent->vy += ACC / TICKS_PER_SECOND;
            }

            current->sprite = SPRITE_FACE;
            if(current->left) {
                current->ent->vx -= ACC / TICKS_PER_SECOND;
                current->sprite = SPRITE_LEFT;
            } else if(current->right) {
                current->ent->vx += ACC / TICKS_PER_SECOND;
                current->sprite = SPRITE_RIGHT;
            } else if(!current->falling) {
                current->ent->vx -= DCC * current->ent->vx / TICKS_PER_SECOND;
                if(abs(current->ent->vx) < MIN_SPEED) current->ent->vx = 0;
            }

            current = current->next;
        }

        current = first;
        while(current != NULL) {
            move(current->ent);
            current->falling = true;
            current = current->next;
        }

        if(collides(first->ent, last->ent)) {
            edge = resolve(last->ent, first->ent);
            if(edge == EDGE_BOTTOM) {
                first->falling = false;
            } else if(edge == EDGE_TOP) {
                last->falling = false;
            }
        }

        current = first;
        while(current != NULL) {
            edge = EDGE_NONE;
            for(int i=0; i<BLOCK_COUNT; i++) {
                if(collides(blocks[i], current->ent)) {
                    edge = resolveStatic(blocks[i], current->ent);
                    if(edge == EDGE_BOTTOM) {
                        current->falling = false;
                    }
                }
            }
            current = current->next;
        }

        current = first;
        while(current != NULL) {
            if(bounds(current->ent) == EDGE_BOTTOM) {
                current->falling = false;
            }
            current = current->next;
        }

        SDL_RenderClear(rend);

        for(int i=0; i<BLOCK_COUNT; i++) {
            dest.x = (int) blocks[i]->x;
            dest.y = (int) blocks[i]->y;
            dest.w = blocks[i]->w;
            dest.h = blocks[i]->h;
            SDL_RenderCopy(rend, tex, &spriteRects[SPRITE_CRATE], &dest);
        }

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
