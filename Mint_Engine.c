#include "Mint_Engine.h"
#include "types.h"

double Zbuff[500][500] = {0};
int f = 0, dis = 10, H = 500, W = 500, out_of_bounds = 0, Render = 0;


OBJECT Global_Object[1000] = {0};
bool   Global_Collider[1000] = {0};
PLAYER player;


int GO_open = 0;
Mat4 projcam = {0};
Mat4 proj = {0};
Mat4 cam1 = {0};
Mat4 cam2 = {0};


BUFFER RENDER_BUFFER;

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/__MATH-FUNCTIONS__/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\//

float edge(Vec3 a, Vec3 b, Vec3_int c){
    return (c.x - a.x) * (b.y - a.y)- (c.y - a.y) * (b.x - a.x);
}

float edge2(Vec3 a, Vec3 b, Vec3 c){
    return (c.x - a.x) * (b.y - a.y)- (c.y - a.y) * (b.x - a.x);
}

Mat4 Mat4_mul(Mat4 a, Mat4 b){
    Mat4 r;

    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            float s = 0.0f;
            for(int k = 0; k < 4; k++){
                s += a.m[i][k] * b.m[k][j];
            }
            r.m[i][j] = s;
        }
    }

    return r;
}

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/__UTILITY-FUNCTIONS__/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\//

int Mint_Initilize(){
    f = 1.0f / tanf((90 * (3.14159 / 180.0)) / 2);
    Mat4 proj_temp = { f * ((float)H / (float)W), 0, 0, 0, 0, f, 0, 0, 0, 0, (dis + 0.1f)/(0.1f - dis), (2.0f*dis*0.1f)/(0.1f - dis), 0, 0, -1, 0};
    memcpy(&proj, &proj_temp,sizeof(Mat4));
    for(int i = 0; i < 1000; i++){
        Global_Object[i] = (OBJECT){0};
        Global_Object[i].type = UNALLOC;
    }
    player.cam.z = 5.5;
    player.cam.x = -3.5;
    player.cam.y = -3.5;
    return 0;
}

int Set_view_size(int width, int height){
    H = height;
    W = width;
    realloc(RENDER_BUFFER.ren_buf, sizeof(unsigned long) * width * height);
    realloc(Zbuff, sizeof(double) * width * height);
}

int Mint_tick(BUFFER* buffer){
    for(int i = 0; i < 500; i++) for(int j = 0; j < 500; j++) Zbuff[i][j] = 1e9;
    Mat4 cam_rotY = {cosf(player.cam.a),0,sinf(player.cam.a),0,0,1,0,0,-sinf(player.cam.a),0,cosf(player.cam.a),0,0,0,0,1};
    Mat4 cam_rotX = {1,0,0,0,0,cosf(player.cam.p),-sinf(player.cam.p),0,0,sinf(player.cam.p),cosf(player.cam.p),0,0,0,0,1};
    Mat4 cam_trans = {1,0,0,-player.cam.x,0,1,0,-player.cam.y,0,0,1,-player.cam.z,0,0,0,1};
    cam1 = Mat4_mul(Mat4_mul(cam_rotX, cam_rotY), cam_trans);
    Mat4 cam2_temp = {1,0,0,-player.cam.x,0,1,0,-player.cam.y,0,0,1,-player.cam.z,0,0,0,1};
    cam2 = cam2_temp;
    for(int i = 0; i < 1000; i++){
        if(Global_Object[i].type != UNALLOC) Mint_PLAYER_OBJECT_COLLIDER(i, &Global_Collider[i], buffer);
        if(Global_Object[i].type == PHYSICS) for(int j = 0; j < 1000; j++) if(Global_Object[j].type == COLLISION) Mint_OBJECT_OBJECT_COLLIDER(j, i, &Global_Collider[i], buffer);
    }
    return 0;
}

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/__DRAWING-FUNCTIONS__/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\//

void Mint_Draw_model(float x, float y, float z, bool rot, BUFFER* buffer, MODEL* model){
    MODEL temp = *model;
    for(int i = 0; i < temp.index; i++){
        for(int j = 0; j < 3; j++){
            temp.tris[i].p[j].x += x;
            temp.tris[i].p[j].y += y;
            temp.tris[i].p[j].z += z;
        }
        Rasterize_tri(render(temp.tris[i], player.cam, rot), buffer);
    }
}

void Mint_Scale_model(float x, float y, float z, MODEL* in_model, MODEL* out_model){
    for(int j = 0; j <= in_model->index; j++){
        out_model->tris[j].col = in_model->tris[j].col;
        for(int i = 0; i < 3; i++) out_model->tris[j].p[i].x = in_model->tris[j].p[i].x * x;
        for(int i = 0; i < 3; i++) out_model->tris[j].p[i].y = in_model->tris[j].p[i].y * y;
        for(int i = 0; i < 3; i++) out_model->tris[j].p[i].z = in_model->tris[j].p[i].z * z;
    }
}

void Mint_Rotate_model(float rotation, AXIS axis, MODEL* in_model, MODEL* out_model){
    Mat4 rot = {0};
        switch(axis){
            case X_AXIS:
                rot = (Mat4){1,0,0,0,0,cosf(rotation),-sinf(rotation),0,0,sinf(rotation),cosf(rotation),0,0,0,0,1};
            break;
            case Y_AXIS:
                rot = (Mat4){cosf(rotation),0,sinf(rotation),0,0,1,0,0,-sinf(rotation),0,cosf(rotation),0,0,0,0,1};
            break;
            case Z_AXIS:
                rot = (Mat4){cosf(rotation),-sinf(rotation),0,0,sinf(rotation),cosf(rotation),0,0,0,0,1,0,0,0,0,1};
            break;
        }

        for(int j = 0; j < in_model->index; j++){
            out_model->tris[j].col = in_model->tris[j].col;
            for(int i = 0; i < 3; i++){
                Vec4 v = {
                    in_model->tris[j].p[i].x,
                    in_model->tris[j].p[i].y,
                    in_model->tris[j].p[i].z,
                    1.0f
                };

                Vec4 r;

                r.x = rot.m[0][0]*v.x + rot.m[0][1]*v.y + rot.m[0][2]*v.z + rot.m[0][3]*v.w;
                r.y = rot.m[1][0]*v.x + rot.m[1][1]*v.y + rot.m[1][2]*v.z + rot.m[1][3]*v.w;
                r.z = rot.m[2][0]*v.x + rot.m[2][1]*v.y + rot.m[2][2]*v.z + rot.m[2][3]*v.w;

                out_model->tris[j].p[i].x = r.x;
                out_model->tris[j].p[i].y = r.y;
                out_model->tris[j].p[i].z = r.z;
            }
        }
}

void Mint_Draw_hud_model(float x, float y, float z, BUFFER* buffer, MODEL* model){
    Mint_Draw_model(-player.cam.x + x,-player.cam.y + y,-player.cam.z + z, false, buffer, model);
}

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/__OBJECT-FUNCTIONS__/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\//

int Mint_Create_Object(Vec3 position, MODEL* model, BBOX* bound, int health, OBJ_type type, bool DO_DRAW){
    for(int i = 0; i < 1000; i++){
        if(Global_Object[i].type == UNALLOC){
            Global_Object[i].model = *model;
            Global_Object[i].bound = *bound;
            Global_Object[i].health = health;
            Global_Object[i].type = type;
            Global_Object[i].pos = position;
            Global_Object[i].do_draw = DO_DRAW;
            return i;
        }
    }
    return -1;
}

int Mint_PLAYER_OBJECT_COLLIDER(int location, bool* COLLIDING, BUFFER* buffer){
    Vec3 player_pos = {player.cam.x, player.cam.y, player.cam.z};
    bool colision;
    switch(Global_Object[location].type){
        case UNALLOC:
            return -1;
            break;
        case PHYSICS:
            colision = BBOX_PHYS_COL(&player, &Global_Object[location]);
            break;
        case DETECT:
            colision = BBOX_DET_COL(player.bound, Global_Object[location].bound, player_pos, Global_Object[location].pos);
            break;
        case COLLISION:
            colision = BBOX_RES_COL(&player, &Global_Object[location]);
            break;
    }
    *COLLIDING = colision;
    if(Global_Object[location].do_draw) Mint_Draw_model(Global_Object[location].pos.x, Global_Object[location].pos.y, Global_Object[location].pos.z, true, buffer, &Global_Object[location].model);
    return 0;
}

int Mint_OBJECT_OBJECT_COLLIDER(int location, int location2, bool* COLLIDING, BUFFER* buffer){
    Vec3 player_pos = Global_Object[location2].pos;
    PLAYER temp_player = { .cam = {Global_Object[location2].pos.x, Global_Object[location2].pos.y, Global_Object[location2].pos.z, 0, 0}, Global_Object[location2].health, Global_Object[location2].model, Global_Object[location2].bound};
    bool colision;
    switch(Global_Object[location].type){
        case UNALLOC:
            return -1;
            break;
        case PHYSICS:
            colision = BBOX_PHYS_COL(&temp_player, &Global_Object[location]);
            break;
        case DETECT:
            colision = BBOX_DET_COL(temp_player.bound, Global_Object[location].bound, player_pos, Global_Object[location].pos);
            break;
        case COLLISION:
            colision = BBOX_RES_COL(&temp_player, &Global_Object[location]);
            break;
    }
    if(colision){
        Global_Object[location2].pos.x = temp_player.cam.x;
        Global_Object[location2].pos.y = temp_player.cam.y;
        Global_Object[location2].pos.z = temp_player.cam.z;
    }
    *COLLIDING = colision;
    if(Global_Object[location].do_draw) Mint_Draw_model(Global_Object[location].pos.x, Global_Object[location].pos.y, Global_Object[location].pos.z, true, buffer, &Global_Object[location].model);
    return 0;
}

bool BBOX_DET_COL(BBOX a, BBOX b, Vec3 posA, Vec3 posB){
    if(a.MAXx + posA.x < b.MINx + posB.x) return false;
    if(a.MINx + posA.x > b.MAXx + posB.x) return false;

    if(a.MAXy + posA.y < b.MINy + posB.y) return false;
    if(a.MINy + posA.y > b.MAXy + posB.y) return false;

    if(a.MAXz + posA.z < b.MINz + posB.z) return false;
    if(a.MINz + posA.z > b.MAXz + posB.z) return false;
    return true;
}
bool BBOX_PHYS_COL(PLAYER* player, OBJECT* obj)
{
    Vec3 playerPos = {player->cam.x, player->cam.y, player->cam.z};
    Vec3 objPos = obj->pos;

    if(!BBOX_DET_COL(player->bound, obj->bound, playerPos, objPos))
        return false;

    float playerMinX = player->bound.MINx + playerPos.x;
    float playerMaxX = player->bound.MAXx + playerPos.x;
    float objMinX = obj->bound.MINx + objPos.x;
    float objMaxX = obj->bound.MAXx + objPos.x;

    float playerMinY = player->bound.MINy + playerPos.y;
    float playerMaxY = player->bound.MAXy + playerPos.y;
    float objMinY = obj->bound.MINy + objPos.y;
    float objMaxY = obj->bound.MAXy + objPos.y;

    float playerMinZ = player->bound.MINz + playerPos.z;
    float playerMaxZ = player->bound.MAXz + playerPos.z;
    float objMinZ = obj->bound.MINz + objPos.z;
    float objMaxZ = obj->bound.MAXz + objPos.z;

    float overlapX = fminf(playerMaxX, objMaxX) - fmaxf(playerMinX, objMinX);
    float overlapY = fminf(playerMaxY, objMaxY) - fmaxf(playerMinY, objMinY);
    float overlapZ = fminf(playerMaxZ, objMaxZ) - fmaxf(playerMinZ, objMinZ);

    if(overlapX <= 0 || overlapY <= 0 || overlapZ <= 0)
        return false;

    float minOverlap = overlapX;
    int axis = 0;

    if(overlapY < minOverlap) { minOverlap = overlapY; axis = 1; }
    if(overlapZ < minOverlap) { minOverlap = overlapZ; axis = 2; }

    float centerPlayerX = (playerMinX + playerMaxX) * 0.5f;
    float centerObjX    = (objMinX + objMaxX) * 0.5f;

    float centerPlayerY = (playerMinY + playerMaxY) * 0.5f;
    float centerObjY    = (objMinY + objMaxY) * 0.5f;

    float centerPlayerZ = (playerMinZ + playerMaxZ) * 0.5f;
    float centerObjZ    = (objMinZ + objMaxZ) * 0.5f;

    switch(axis)
    {
        case 0:
            obj->pos.x += (centerObjX < centerPlayerX) ? -minOverlap : minOverlap;
            break;
        case 1:
            obj->pos.y += (centerObjY < centerPlayerY) ? -minOverlap : minOverlap;
            break;
        case 2:
            obj->pos.z += (centerObjZ < centerPlayerZ) ? -minOverlap : minOverlap;
            break;
    }

    return true;
}

bool BBOX_RES_COL(PLAYER* player, OBJECT* obj){
    Vec3 playerPos = {player->cam.x, player->cam.y, player->cam.z};
    Vec3 objPos = obj->pos;
    bool is_collide = BBOX_DET_COL(player->bound, obj->bound, playerPos, objPos);
    if(is_collide){
        float overlapX = MIN(player->bound.MAXx + playerPos.x, obj->bound.MAXx + objPos.x) - MAX(player->bound.MINx + playerPos.x, obj->bound.MINx + objPos.x);
        float overlapY = MIN(player->bound.MAXy + playerPos.y, obj->bound.MAXy + objPos.y) - MAX(player->bound.MINy + playerPos.y, obj->bound.MINy + objPos.y);
        float overlapZ = MIN(player->bound.MAXz + playerPos.z, obj->bound.MAXz + objPos.z) - MAX(player->bound.MINz + playerPos.z, obj->bound.MINz + objPos.z);

        float minOverlap = overlapX;
        int axis = 0;
        if(overlapY < minOverlap){ minOverlap = overlapY; axis = 1; }
        if(overlapZ < minOverlap){ minOverlap = overlapZ; axis = 2; }

        switch(axis) {
            case 0:
                if(playerPos.x < objPos.x){
                    player->cam.x -= minOverlap;
                }else{
                    player->cam.x += minOverlap;
                }
                break;
            case 1:
                if(playerPos.y < objPos.y){
                    player->cam.y -= minOverlap;
                }else{
                    player->cam.y += minOverlap;
                }
                break;
            case 2:
                if(playerPos.z < objPos.z){
                    player->cam.z -= minOverlap;
                }else{
                    player->cam.z += minOverlap;
                }
                break;
        }
    }
    return is_collide;
}

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/__RENDER-FUNCTIONS__/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\//

TRI render(TRI t, CAM cam, bool rot){
    TRI t2;
    Vec4 r[3];
    t2.col = t.col;
    Mat4 camera;
    out_of_bounds = 0;

    if(rot) camera = cam1;
    else camera = cam2;

    projcam = Mat4_mul(proj, camera);

    for(int i = 0; i < 3; i++){
    t2.uv[i] = t.uv[i];
    Vec4 tri = { t.p[i].x, t.p[i].y, t.p[i].z, 1.0f };
    r[i].x = projcam.m[0][0]*tri.x + projcam.m[0][1]*tri.y + projcam.m[0][2]*tri.z + projcam.m[0][3]*tri.w;
    r[i].y = projcam.m[1][0]*tri.x + projcam.m[1][1]*tri.y + projcam.m[1][2]*tri.z + projcam.m[1][3]*tri.w;
    r[i].z = projcam.m[2][0]*tri.x + projcam.m[2][1]*tri.y + projcam.m[2][2]*tri.z + projcam.m[2][3]*tri.w;
    r[i].w = projcam.m[3][0]*tri.x + projcam.m[3][1]*tri.y + projcam.m[3][2]*tri.z + projcam.m[3][3]*tri.w;

    if(r[i].w < 0.1f){
        out_of_bounds++;

    }
    }

    switch(out_of_bounds){
        case 3:
            return t2;
        break;
        case 2:
            return t2;
        break;
        case 1:
            return t2;
        break;

    }

    for(int i = 0; i < 3; i++){
    float nx = r[i].x / r[i].w, ny = r[i].y / r[i].w, nz = r[i].z / r[i].w;
    t2.p[i].x = ((nx + 1.0f) * 0.5f * (float)W + 0.5f);
    t2.p[i].y = ((1.0f - (ny + 1.0f) * 0.5f) * (float)H + 0.5f);
    t2.p[i].z = r[i].w;
    }
    return t2;
}

void Rasterize_tri(TRI tri, BUFFER* buffer)
{
    if(out_of_bounds > 0) return;

    int minX = MAX(0, (int)MIN(tri.p[0].x, MIN(tri.p[1].x, tri.p[2].x)));
    int minY = MAX(0, (int)MIN(tri.p[0].y, MIN(tri.p[1].y, tri.p[2].y)));
    int maxX = MIN(W - 1, (int)MAX(tri.p[0].x, MAX(tri.p[1].x, tri.p[2].x)));
    int maxY = MIN(H - 1, (int)MAX(tri.p[0].y, MAX(tri.p[1].y, tri.p[2].y)));

    float area = edge2(tri.p[0], tri.p[1], tri.p[2]);
    if(area <= 0.0f) return;
    float invArea = 1.0f / area;

    float dx12 = tri.p[2].x - tri.p[1].x;
    float dy12 = tri.p[2].y - tri.p[1].y;
    float dx20 = tri.p[0].x - tri.p[2].x;
    float dy20 = tri.p[0].y - tri.p[2].y;
    float dx01 = tri.p[1].x - tri.p[0].x;
    float dy01 = tri.p[1].y - tri.p[0].y;

    Vec3_int start = { minX, minY, 0 };

    float w0_row = edge(tri.p[1], tri.p[2], start);
    float w1_row = edge(tri.p[2], tri.p[0], start);
    float w2_row = edge(tri.p[0], tri.p[1], start);

    for(int y = minY; y <= maxY; y++)
    {
        float w0 = w0_row, w1 = w1_row, w2 = w2_row;
        for(int x = minX; x <= maxX; x++)
        {
            if(w0 >= 0 && w1 >= 0 && w2 >= 0)
            {
                float alpha = w0 * invArea, beta  = w1 * invArea, gamma = w2 * invArea;
                float Z = alpha * tri.p[0].z + beta  * tri.p[1].z + gamma * tri.p[2].z;

                if(Z < Zbuff[x][y])
                {
                    float invZ = alpha * (1.0f / tri.p[0].z) + beta  * (1.0f / tri.p[1].z) + gamma * (1.0f / tri.p[2].z);
                    float u = (alpha * (tri.uv[0].u / tri.p[0].z) + beta  * (tri.uv[1].u / tri.p[1].z) + gamma * (tri.uv[2].u / tri.p[2].z)) / invZ;
                    float v = (alpha * (tri.uv[0].v / tri.p[0].z) + beta  * (tri.uv[1].v / tri.p[1].z) + gamma * (tri.uv[2].v / tri.p[2].z)) / invZ;

                    int texX = (int)(u * 16.0f) & 15;
                    int texY = (int)(v * 16.0f) & 15;

                    unsigned long color = tri.col->data[texY * 16 + texX];

                    buffer->ren_buf[y][x] = color;
                    Zbuff[x][y] = Z;
                }
            }
            w0 += dy12;
            w1 += dy20;
            w2 += dy01;
        }
        w0_row -= dx12;
        w1_row -= dx20;
        w2_row -= dx01;
    }
}