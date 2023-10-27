//***************************************************************************
// 
// 파일: game.h
// 
// 설명: game
// 
// 작성자: bumpsgoodman
// 
// 작성일: 2023/10/27
// 
//***************************************************************************

#ifndef GAME_H
#define GAME_H

#include "safe99_common/defines.h"
#include "safe99_ecs/i_ecs.h"
#include "safe99_file_system/i_file_system.h"
#include "safe99_soft_renderer_2d/i_soft_renderer_2d.h"
#include "safe99_generic/util/timer.h"

#define NUM_MAX_ENEMY1 1000
#define NUM_MAX_ENEMY2 1000

typedef struct game
{
    // DLL 핸들
    HINSTANCE h_renderer_dll;
    HINSTANCE h_file_system_dll;
    HINSTANCE h_ecs_dll;

    // DLL 인스턴스
    i_soft_renderer_2d_t* p_renderer;
    i_file_system_t* p_file_system;
    i_ecs_t* p_ecs;

    // 게임 관련
    bool b_running;
    float update_ticks_per_frame;
    size_t fps;

    timer_t frame_timer;
} game_t;

game_t* gp_game;

START_EXTERN_C

bool init_game(void);
void shutdown_game(void);
void tick_game(void);

FORCEINLINE bool is_running_game(void)
{
    return gp_game->b_running;
}

FORCEINLINE void update_window_pos_game(void)
{
    gp_game->p_renderer->vtbl->update_window_pos(gp_game->p_renderer);
}

FORCEINLINE void update_window_size_game(void)
{
    gp_game->p_renderer->vtbl->update_window_size(gp_game->p_renderer);
}

END_EXTERN_C

#endif // GAME_H