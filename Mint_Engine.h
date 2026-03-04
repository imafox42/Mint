#pragma once

#include "types.h"

TRI render(TRI t, CAM cam, bool rot);
int Mint_Initilize();
int Mint_tick(BUFFER* buffe);
void Mint_Draw_model(float x, float y, float z, bool rot, BUFFER* buffer, MODEL* model);
void Mint_Scale_model(float x, float y, float z, MODEL* in_model, MODEL* out_model);
void Mint_Rotate_model(float rotation, AXIS axis, MODEL* in_model, MODEL* out_model);
void Mint_Draw_hud_model(float x, float y, float z, BUFFER* buffer, MODEL* model);
int Mint_Create_Object(Vec3 position, MODEL* model, BBOX* bound, int health, OBJ_type type, bool DO_DRAW);
void render_buffer();
void Isle_Buffer_TRI(TRI tri, BUFFER* buffer);
void Rasterize_tri(TRI tri, BUFFER* buffer);
float edge(Vec3 a, Vec3 b, Vec3_int c);
float edge2(Vec3 a, Vec3 b, Vec3 c);
bool BBOX_DET_COL(BBOX a, BBOX b, Vec3 posA, Vec3 posB);
bool BBOX_RES_COL(PLAYER* player, OBJECT* obj);
bool BBOX_PHYS_COL(PLAYER* player, OBJECT* obj);
int Mint_PLAYER_OBJECT_COLLIDER(int location, bool* COLLIDING, BUFFER* buffer);
int Mint_OBJECT_OBJECT_COLLIDER(int location, int location2, bool* COLLIDING, BUFFER* buffer);

