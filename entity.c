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

enum edges resolve(struct Entity *a, struct Entity *b, bool block) {
    float dx = getMidX(a) - getMidX(b);
    float dy = getMidY(a) - getMidY(b);
    float w = a->w / 2 + b->w / 2;
    float h = a->h / 2 + b->h / 2;
    float x_move = w - abs(dx);
    float y_move = h - abs(dy);

    if(x_move < y_move) {
        a->vx = 0;
        b->vx = 0;
        if(dx > 0) {
            if(block) {
                b->x -= x_move;
            } else {
                a->x += x_move / 2;
                b->x -= x_move / 2;
            }
            //setRight(b, getLeft(a));
            return EDGE_RIGHT;
        } else {
            if(block) {
                b->x += x_move;
            } else {
                a->x -= x_move / 2;
                b->x += x_move / 2;
            }
            //setLeft(b, getRight(a));
            return EDGE_LEFT;
        }
    } else {
        a->vy = 0;
        b->vy = 0;
        if(dy > 0) {
            if(block) {
                b->y -= y_move;
            } else {
                a->y += y_move / 2;
                b->y -= y_move / 2;
            }
            //setBottom(b, getTop(a));
            return EDGE_BOTTOM;
        } else {
            if(block) {
                b->y -= y_move;
            } else {
                a->y -= y_move / 2;
                b->y += y_move / 2;
            }
            //setTop(b, getBottom(a));
            return EDGE_TOP;
        }
    }
}
