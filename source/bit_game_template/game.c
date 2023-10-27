//***************************************************************************
// 
// 파일: game.c
// 
// 설명: game
// 
// 작성자: bumpsgoodman
// 
// 작성일: 2023/10/28
// 
//***************************************************************************

#include <stdio.h>

#include "game.h"
#include "console_util.h"
#include "key_event.h"

// 전역 변수
extern HWND g_hwnd;

// 전역 함수
static void update(const float delta_time);
static void draw(void);

static void print_info(void);

bool init_game(void)
{
    visible_cursor(false);

    memset(gp_game, 0, sizeof(game_t));

    // DLL 로드
    {
    #if defined(_WIN64)
        gp_game->h_renderer_dll = LoadLibrary(L"safe99_soft_renderer_2d_x64.dll");
        gp_game->h_file_system_dll = LoadLibrary(L"safe99_file_system_x64.dll");
        gp_game->h_ecs_dll = LoadLibrary(L"safe99_ecs_x64.dll");
    #else
        gp_game->h_renderer_dll = LoadLibrary(L"safe99_soft_renderer_2d_x86.dll");
        gp_game->h_file_system_dll = LoadLibrary(L"safe99_file_system_x86.dll");
        gp_game->h_ecs_dll = LoadLibrary(L"safe99_ecs_x86.dll");
    #endif // _WIN64

        if (gp_game->h_renderer_dll == NULL
            || gp_game->h_file_system_dll == NULL
            || gp_game->h_ecs_dll == NULL)
        {
            ASSERT(false, "Failed load DLL");
            goto failed_init;
        }

        // 인스턴스 생성 함수
        create_instance_func pf_create_renderer = (create_instance_func)GetProcAddress(gp_game->h_renderer_dll, "create_instance");
        create_instance_func pf_create_file_system = (create_instance_func)GetProcAddress(gp_game->h_file_system_dll, "create_instance");
        create_instance_func pf_create_ecs = (create_instance_func)GetProcAddress(gp_game->h_ecs_dll, "create_instance");

        // 렌더러 초기화
        pf_create_renderer(&gp_game->p_renderer);
        if (!gp_game->p_renderer->vtbl->initialize(gp_game->p_renderer, g_hwnd))
        {
            ASSERT(false, "Failed init renderer");
            goto failed_init;
        }

        // 파일 시스템 초기화
        pf_create_file_system(&gp_game->p_file_system);
        if (!gp_game->p_file_system->vtbl->initialize(gp_game->p_file_system))
        {
            ASSERT(false, "Failed init file system");
            goto failed_init;
        }

    #if 0
        // ecs 월드 초기화
        pf_create_ecs(&gp_game->p_ecs);
        if (!gp_game->p_ecs->vtbl->initialize(gp_game->p_ecs, 10000, 100, 100))  // TODO: 매개 변수 수정하기
        {
            ASSERT(false, "Failed init ecs");
            goto failed_init;
        }
    #endif
    }

    // 게임 초기화
    {
        gp_game->b_running = true;
        gp_game->update_ticks_per_frame = 1.0f / 60.0f;
        gp_game->fps = 0;
        
        timer_init(&gp_game->frame_timer);
        timer_start(&gp_game->frame_timer);
    }

    // 초기화 코드 작성
    {
        
    }

    return true;

failed_init:
    shutdown_game();
    return false;
}

void shutdown_game(void)
{
    // DLL 객체 해제
    SAFE_RELEASE(gp_game->p_renderer);
    SAFE_RELEASE(gp_game->p_file_system);
    SAFE_RELEASE(gp_game->p_ecs);

    // DLL 핸들 해제
    FreeLibrary(gp_game->h_renderer_dll);
    FreeLibrary(gp_game->h_file_system_dll);
    FreeLibrary(gp_game->h_ecs_dll);
}

void tick_game(void)
{
    // 업데이트 시작
    {
        static float start_time;

        const float end_time = timer_get_time(&gp_game->frame_timer);
        const float elapsed_time = end_time - start_time;
        if (elapsed_time >= gp_game->update_ticks_per_frame)
        {
            update(elapsed_time);

            start_time = end_time;
        }
    }

    // 렌더링 시작
    {
        static float start_time;
        static size_t frame_count;

        draw();

        const float end_time = timer_get_time(&gp_game->frame_timer);
        const float elapsed_time = (end_time - start_time) * 1000.0f;
        if (elapsed_time >= 1000.0f)
        {
            gp_game->fps = frame_count;
            frame_count = 0;

            start_time = end_time;
        }

        ++frame_count;
    }
}

static void update(const float delta_time)
{

}

static void draw(void)
{
    // 화면 렌더링
    gp_game->p_renderer->vtbl->begin_draw(gp_game->p_renderer);
    {
        gp_game->p_renderer->vtbl->clear(gp_game->p_renderer, 0xff000000);  // 검은색으로 초기화
    }
    gp_game->p_renderer->vtbl->end_draw(gp_game->p_renderer);

    gp_game->p_renderer->vtbl->on_draw(gp_game->p_renderer);

    print_info();
}

static void print_info(void)
{
    const size_t window_width = gp_game->p_renderer->vtbl->get_width(gp_game->p_renderer);
    const size_t window_height = gp_game->p_renderer->vtbl->get_height(gp_game->p_renderer);

    gotoxy(0, 0);

    printf("window info\n");
    printf("- window width = %zd\t\t\t\n", window_width);
    printf("- window height = %zd\t\t\t\n\n", window_height);

    printf("frame info\n");
    printf("- FPS = %zd\t\t\t\t\n", gp_game->fps);
}