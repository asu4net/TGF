#ifndef ENGINE_H_STATE
#define ENGINE_H_STATE

typedef void(*CALLBACK_RUN)();
typedef void(*CALLBACK_PROCESS_EVENTS)(struct input_event* event);
typedef void(*CALLBACK_TICK)(f32 dt);
typedef void(*CALLBACK_END)();

struct engine_callbacks
{
  CALLBACK_RUN run;
  CALLBACK_PROCESS_EVENTS on_event;
  CALLBACK_TICK tick;
  CALLBACK_END end;
};

struct engine_params
{
  char** argv;
  s32 argc;
  struct engine_callbacks callbacks;
};

#define DEFAULT_ENGINE_PARAMS make_struct(engine_params,  \
  .argv = NULL,                                           \
  .argc = 0,                                              \
  .callbacks = make_struct(engine_callbacks,              \
    .run = NULL,                                          \
    .on_event = NULL,                                     \
    .tick = NULL,                                         \
    .end = NULL,                                          \
  )                                                       \
);

struct engine_state
{
  b8 quit;
  b8 vsync;
  union vec4 clear_color;
  struct window* window;
  struct engine_callbacks callbacks;
};

void engine_run(struct engine_params* params);

#endif // ENGINE_H_STATE

#ifdef ENGINE_IMPL_STATE

struct engine_state g_engine = nil;

void engine_run(struct engine_params* params)
{
#ifdef ENGINE_MODE_CODEGEN
  UNUSED(params);
  generate_code();
#else
  // Initialization.
  g_engine.clear_color = COLOR_CHILL_GREEN;
  g_engine.vsync = true;
  g_engine.window = create_window_default();
  create_gpu_context(g_engine.window);
  g_engine.callbacks = params->callbacks;

  if (g_engine.callbacks.run)
  {
    g_engine.callbacks.run();
  }

  // Main loop.
  while (!g_engine.quit)
  {
    poll_events();

    struct input_event_view view = events_this_frame();

    // Process events.
    for each_index(i, view.len)
    {
      struct input_event* ev = &view.data[i];

      if (ev->kind == INPUT_EVENT_QUIT)
      {
        g_engine.quit = true;
        break;
      }

      if (ev->kind == INPUT_EVENT_KEY && ev->key == KEY_ESCAPE && ev->key_state == KEY_STATE_PRESSED)
      {
        g_engine.quit = true;
        break;
      }

      if (g_engine.callbacks.on_event != NULL)
      {
        g_engine.callbacks.on_event(ev);
      }
    }

    // Draw frame.
    clear_screen(g_engine.clear_color);

    if (g_engine.callbacks.tick != NULL)
    {
      g_engine.callbacks.tick(0);
    }
    
    // Present.
    swap_buffers(g_engine.window, g_engine.vsync);
  }

  // Finish.
  if (g_engine.callbacks.end != NULL)
  {
    g_engine.callbacks.end();
  }

  destroy_window(g_engine.window);
#endif // ENGINE_MODE_CODEGEN;
}

#endif // ENGINE_IMPL_STATE
