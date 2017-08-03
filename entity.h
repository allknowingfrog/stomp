#ifndef entity
#define entity

struct Entity {
    float x;
    float y;
    int w;
    int h;
    float vx;
    float vy;
};

enum edges {
    EDGE_LEFT, EDGE_TOP, EDGE_RIGHT, EDGE_BOTTOM
};

float getLeft(struct Entity *e);
void setLeft(struct Entity *e, float value);

float getTop(struct Entity *e);
void setTop(struct Entity *e, float value);

float getRight(struct Entity *e);
void setRight(struct Entity *e, float value);

float getBottom(struct Entity *e);
void setBottom(struct Entity *e, float value);

float getMidX(struct Entity *e);
void setMidX(struct Entity *e, float value);

float getMidY(struct Entity *e);
void setMidY(struct Entity *e, float value);

bool collides(struct Entity *a, struct Entity *b);

enum edges resolveStatic(struct Entity *a, struct Entity *b);

enum edges resolve(struct Entity *a, struct Entity *b);

#endif
