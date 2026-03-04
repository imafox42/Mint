#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef struct {unsigned long data[16 * 16];}TEXTURE;
typedef struct{float MINy, MINx, MINz, MAXy, MAXx, MAXz;}BBOX;
typedef struct{float x, y, z;      }Vec3;
typedef struct{float u, v;         }Vec2;
typedef struct{Vec3 p[3]; Vec2 uv[3]; TEXTURE* col;}TRI ;
typedef struct{TRI tris[50]; int index, type; }MODEL;

typedef enum {UNALLOC, COLLISION, DETECT, PHYSICS}OBJ_type;
typedef enum {X_AXIS, Y_AXIS, Z_AXIS}AXIS;

typedef struct{float x, y, z, w;   }Vec4;
typedef struct{int x, y, z;      }Vec3_int;
typedef struct{float m[4][4];      }Mat4;
typedef struct{float m[3][3];      }Mat3;
typedef struct{float x, y, z, p, a;}CAM ;
typedef struct{unsigned long ren_buf[500][500]; }BUFFER;
typedef struct{Vec3 origin, direction; int type;}RAY;
typedef struct{CAM cam; int health; MODEL model; BBOX bound;}PLAYER;
typedef struct{MODEL model; BBOX bound; Vec3 pos; int health; OBJ_type type; bool do_draw;}OBJECT;
