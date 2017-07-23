#include <stdlib.h>
#include <stdbool.h>

#include "entity.h"

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
    return e->x +  e->w;
}

void setRight(struct Entity *e, float value) {
    e->x = value -  e->w;
}

float getBottom(struct Entity *e) {
    return e->y +  e->h;
}

void setBottom(struct Entity *e, float value) {
    e->y = value -  e->h;
}

float getMidX(struct Entity *e) {
    return e->x + e->w / 2;
}

void setMidX(struct Entity *e, float value) {
    e->x = value - e->w / 2;
}

float getMidY(struct Entity *e) {
    return e->y + e->h / 2;
}

void setMidY(struct Entity *e, float value) {
    e->y = value - e->h / 2;
}

bool collides(struct Entity *a, struct Entity *b) {
    if(getRight(b) < getLeft(a)) return false;
    if(getBottom(b) < getTop(a)) return false;
    if(getLeft(b) > getRight(a)) return false;
    if(getTop(b) > getBottom(a)) return false;
    return true;
}

int resolve(struct Entity *a, struct Entity *b) {
    float dx = a->x - b->x;
    float dy = a->y - b->y;
    float w = a->w / 2 + b->w / 2;
    float h = a->h / 2 + b->h / 2;
    float dx_abs = abs(dx);
    float dy_abs = abs(dy);
    float move;

    if(dx_abs > dy_abs) {
        move = (w - dx_abs) / 2;
        if(dx > 0) {
            a->x += move;
            setRight(b, getLeft(a));
            return 0;
        } else {
            a->x -= move;
            setLeft(b, getRight(a));
            return 2;
        }
    } else {
        move = (h - dy_abs) / 2;
        if(dy > 0) {
            a->y += move;
            setBottom(b, getTop(a));
            return 1;
        } else {
            a->y -= move;
            setTop(b, getBottom(a));
            return 3;
        }
    }
}
