#ifndef ENGINE_H_MATH
#define ENGINE_H_MATH

// ============================================
// @: Vec2.
// ============================================

union vec2
{
  struct
  {
    f32 x;
    f32 y;
  };

  f32 v[2];
};

// ============================================
// @: Vec3.
// ============================================

union vec3
{
  struct
  {
    f32 x;
    f32 y;
    f32 z;
  };

  struct
  {
    union vec2 xy;
    f32 _z;
  };

  struct
  {
    f32 _x;
    union vec2 yz;
  };
    
  f32 v[3];
};

// ============================================
// @: Vec4.
// ============================================

union vec4
{
  struct
  {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
  };
    
  struct
  {
    union vec2 xy;
    union vec2 zw;
  };
    
  struct
  {
    union vec3 xyz;
    f32 _w;
  };
    
  struct
  {
    f32 _x;
    union vec3 yzw;
  };

  f32 v[4];
};

#define vec4_lit(_x, _y, _z, _w) make_union(vec4, .x = _x, .y = _y, .z = _z, .w = _w)

// ============================================
// @: Colors. (Vec4)
// ============================================

#define COLOR_WHITE            vec4_lit(1.000f, 1.000f, 1.000f, 1.000f)
#define COLOR_WHITE_FADED      vec4_lit(1.000f, 1.000f, 1.000f, 0.000f)
#define COLOR_BLACK            vec4_lit(0.000f, 0.000f, 0.000f, 1.000f)
#define COLOR_CORN_FLOWER_BLUE vec4_lit(0.388f, 0.584f, 0.933f, 1.000f)
#define COLOR_BLUE             vec4_lit(0.000f, 0.000f, 1.000f, 1.000f)
#define COLOR_LIGHT_BLUE       vec4_lit(0.300f, 0.300f, 1.000f, 1.000f)
#define COLOR_CYAN             vec4_lit(0.000f, 1.000f, 1.000f, 1.000f)
#define COLOR_GRAY             vec4_lit(0.500f, 0.500f, 0.500f, 1.000f)
#define COLOR_DARK_GRAY        vec4_lit(0.200f, 0.200f, 0.200f, 1.000f)
#define COLOR_GREEN            vec4_lit(0.000f, 1.000f, 0.000f, 1.000f)
#define COLOR_LIGHT_GREEN      vec4_lit(0.300f, 1.000f, 0.300f, 1.000f)
#define COLOR_CHILL_GREEN      vec4_lit(0.047f, 0.651f, 0.408f, 1.000f)
#define COLOR_MAGENTA          vec4_lit(1.000f, 0.000f, 1.000f, 1.000f)
#define COLOR_RED              vec4_lit(1.000f, 0.000f, 0.000f, 1.000f)
#define COLOR_LIGHT_RED        vec4_lit(1.000f, 0.300f, 0.300f, 1.000f)
#define COLOR_YELLOW           vec4_lit(1.000f, 0.920f, 0.016f, 1.000f)
#define COLOR_ORANGE           vec4_lit(0.970f, 0.600f, 0.110f, 1.000f)

#endif // ENGINE_H_MATH

// -------------------------------------------------------------------------------------------------

#ifdef ENGINE_IMPL_MATH



#endif // ENGINE_IMPL_MATH
