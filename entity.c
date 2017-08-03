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
    return e->x + e->w;
}

void setRight(struct Entity *e, float value) {
    e->x = value - e->w;
}

float getBottom(struct Entity *e) {
    return e->y + e->h;
}

void setBottom(struct Entity *e, float value) {
    e->y = value - e->h;
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

enum edges resolveStatic(struct Entity *a, struct Entity *b) {
    float dx = getMidX(a) - getMidX(b);
    float dy = getMidY(a) - getMidY(b);

    if(abs(dx) > abs(dy)) {
        if(dx > 0) {
            setRight(b, getLeft(a));
            if(b->vx > 0) b->vx = 0;
            return EDGE_RIGHT;
        } else {
            setLeft(b, getRight(a));
            if(b->vx < 0) b->vx = 0;
            return EDGE_LEFT;
        }
    } else {
        if(dy > 0) {
            setBottom(b, getTop(a));
            if(b->vy > 0) b->vy = 0;
            return EDGE_BOTTOM;
        } else {
            setTop(b, getBottom(a));
            if(b->vy < 0) b->vy = 0;
            return EDGE_TOP;
        }
    }
}

float getRatio(float a, float b) {
    a = abs(a);
    b = abs(b);

    if(a < .1 && b < .1) return .5;

    return (a / (a + b)) / 2 + .5;
}

enum edges resolve(struct Entity *a, struct Entity *b) {
    float dx = getMidX(a) - getMidX(b);
    float dy = getMidY(a) - getMidY(b);
    float w = a->w / 2 + b->w / 2;
    float h = a->h / 2 + b->h / 2;
    float x_move = w - abs(dx);
    float y_move = h - abs(dy);
    float ratio;
    float v;

    if(x_move < y_move) {
        v = a->vx;
        a->vx = (a->vx / 2) + (b->vx / 2);
        b->vx = (b->vx / 2) + (v / 2);
        ratio = getRatio(a->vx, b->vx);
        if(dx > 0) {
            a->x += x_move * (1 - ratio);
            b->x -= x_move * ratio;
            return EDGE_RIGHT;
        } else {
            a->x -= x_move * (1 - ratio);
            b->x += x_move * ratio;
            return EDGE_LEFT;
        }
    } else {
        v = a->vy;
        a->vy = (a->vy / 2) + (b->vy / 2);
        b->vy = (b->vy / 2) + (v / 2);
        ratio = getRatio(a->vy, b->vy);
        if(dy > 0) {
            a->y += y_move * (1 - ratio);
            b->y -= y_move * ratio;
            return EDGE_BOTTOM;
        } else {
            a->y -= y_move * (1 - ratio);
            b->y += y_move * ratio;
            return EDGE_TOP;
        }
    }
}
