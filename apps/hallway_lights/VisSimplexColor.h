/* I forked this SimplexNoise implementation because I'm tired of not being
 * able to find implementations freely available on the internet.
 * Original source code is available at
 * https://github.com/FastLED/FastLED/blob/master/noise.h
 * 
 * This file is completely self contained with a main() function so that it can
 * easily be run from the web interfaced C++ compiler / tester located at
 * https://ideone.com/
 *
 * Adding this code into your solution:
 * Remove the main() function and paste in, simple as that. Hopefully in
 * the future I will clean up this solution into header / main.cpp.
 * To seperate out do a copy and paste into your own code. If you can't
 * figure out this step then I can't help you.
 * If you are reading this and you want to seperate the files then please
 * send a pull request!! Thank you so much!!! Send your cool videos / photos
 * to info@zackees.com
 *
 * Pull requests for improvements is greatly appreciated!
 *
 * 2015, All Rights Reserved, Zackees.
 * MIT License.
 */




#include <stdint.h>

#ifdef FASTLED_AVR
#include <avr/pgmspace.h>
#define USE_PROGMEM
#endif

#ifdef USE_PROGMEM
#define FL_PROGMEM PROGMEM
#define P(x) pgm_read_byte_near(p + x)
#else
#define FL_PROGMEM
#define P(x) p[(x)]
#endif


#define P(x) p[(x)]

typedef uint8_t   fract8;   // ANSI: unsigned short _Fract
typedef uint16_t  fract16;  // ANSI: unsigned       _Fract

// scale8: scale one byte by a second one, which is treated as
//         the numerator of a fraction whose denominator is 256
//         In other words, it computes i * (scale / 256)
//         4 clocks AVR, 2 clocks ARM
uint8_t scale8( uint8_t i, fract8 scale)
{
    return static_cast<uint8_t>(((int)i * (int)(scale) ) >> 8);
}

uint16_t scale16( uint16_t i, fract16 scale )
{
    uint16_t result;
    result = ((uint32_t)(i) * (uint32_t)(scale)) / 65536;
    return result;
}

int16_t lerp15by16( int16_t a, int16_t b, fract16 frac)
{
    int16_t result;
    if( b > a) {
        uint16_t delta = b - a;
        uint16_t scaled = scale16( delta, frac);
        result = a + scaled;
    } else {
        uint16_t delta = a - b;
        uint16_t scaled = scale16( delta, frac);
        result = a - scaled;
    }
    return result;
}

uint8_t scale8_LEAVING_R1_DIRTY( uint8_t i, fract8 scale)
{
    return ((int)i * (int)(scale) ) >> 8;
}

// 16 bit, fixed point implementation of perlin's Simplex Noise.  Coordinates are
// 16.16 fixed point values, 32 bit integers with integral coordinates in the high 16
// bits and fractional in the low 16 bits, and the function takes 1d, 2d, and 3d coordinate
// values.  These functions are scaled to return 0-65535
extern uint16_t inoise16(uint32_t x, uint32_t y, uint32_t z);
extern uint16_t inoise16(uint32_t x, uint32_t y);
extern uint16_t inoise16(uint32_t x);

// 16 bit raw versions of the noise functions.  These values are not scaled/altered and have
// output values roughly in the range (-18k,18k)
extern int16_t inoise16_raw(uint32_t x, uint32_t y, uint32_t z);
extern int16_t inoise16_raw(uint32_t x, uint32_t y);
extern int16_t inoise16_raw(uint32_t x);

// 8 bit, fixed point implementation of perlin's Simplex Noise.  Coordinates are
// 8.8 fixed point values, 16 bit integers with integral coordinates in the high 8
// bits and fractional in the low 8 bits, and the function takes 1d, 2d, and 3d coordinate
// values.  These functions are scaled to return 0-255
extern uint8_t inoise8(uint16_t x, uint16_t y, uint16_t z);
extern uint8_t inoise8(uint16_t x, uint16_t y);
extern uint8_t inoise8(uint16_t x);

// 8 bit raw versions of the noise functions.  These values are not scaled/altered and have
// output values roughly in the range (-70,70)
extern int8_t inoise8_raw(uint16_t x, uint16_t y, uint16_t z);
extern int8_t inoise8_raw(uint16_t x, uint16_t y);
extern int8_t inoise8_raw(uint16_t x);


FL_PROGMEM static uint8_t const p[] = { 151,160,137,91,90,15,
   131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
   190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
   88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
   77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
   102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
   135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
   5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
   223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
   129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
   251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
   49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
   138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,151
   };

#define FADE(x) scale16(x,x)
#define LERP(a,b,u) lerp15by16(a,b,u)

static int16_t grad16(uint8_t hash, int16_t x, int16_t y, int16_t z) {
  hash = hash&15;
  int16_t u = hash<8?x:y;
  int16_t v = hash<4?y:hash==12||hash==14?x:z;
  if(hash&1) { u = -u; }
  if(hash&2) { v = -v; }

  return (u+v)>>1;
}

static int16_t  grad16(uint8_t hash, int16_t x, int16_t y) {
  hash = hash & 7;
  int16_t u,v;
  if(hash < 4) { u = x; v = y; } else { u = y; v = x; }
  if(hash&1) { u = -u; }
  if(hash&2) { v = -v; }

  return (u+v)>>1;
}

static int16_t  grad16(uint8_t hash, int16_t x) {
  hash = hash & 15;
  int16_t u,v;
  if(hash > 8) { u=x;v=x; }
  else if(hash < 4) { u=x;v=1; }
  else { u=1;v=x; }
  if(hash&1) { u = -u; }
  if(hash&2) { v = -v; }

  return (u+v)>>1;
}

static int8_t grad8(uint8_t hash, int8_t x, int8_t y, int8_t z) {
  hash &= 0xF;
  int8_t u = (hash&8)?y:x;
  int8_t v = hash<4?y:hash==12||hash==14?x:z;
  if(hash&1) { u = -u; }
  if(hash&2) { v = -v; }

  return (u+v)>>1;
}

static int8_t  grad8(uint8_t hash, int8_t x, int8_t y) {
  hash = hash & 7;
  int8_t u,v;
  if(hash < 4) { u = x; v = y; } else { u = y; v = x; }
  if(hash&1) { u = -u; }
  if(hash&2) { v = -v; }

  return (u+v)>>1;
}

static int8_t  grad8(uint8_t hash, int8_t x) {
  hash = hash & 15;
  int8_t u,v;
  if(hash > 8) { u=x;v=x; }
  else if(hash < 4) { u=x;v=1; }
  else { u=1;v=x; }
  if(hash&1) { u = -u; }
  if(hash&2) { v = -v; }

  return (u+v)>>1;
}

static int8_t lerp7by8( int8_t a, int8_t b, fract8 frac)
{
    int8_t result;
    if( b > a) {
        uint8_t delta = b - a;
        uint8_t scaled = scale8( delta, frac);
        result = a + scaled;
    } else {
        uint8_t delta = a - b;
        uint8_t scaled = scale8( delta, frac);
        result = a - scaled;
    }
    return result;
}

int16_t inoise16_raw(uint32_t x, uint32_t y, uint32_t z)
{
  // Find the unit cube containing the point
  uint8_t X = (x>>16)&0xFF;
  uint8_t Y = (y>>16)&0xFF;
  uint8_t Z = (z>>16)&0xFF;

  // Hash cube corner coordinates
  uint8_t A = P(X)+Y;
  uint8_t AA = P(A)+Z;
  uint8_t AB = P(A+1)+Z;
  uint8_t B = P(X+1)+Y;
  uint8_t BA = P(B) + Z;
  uint8_t BB = P(B+1)+Z;

  // Get the relative position of the point in the cube
  uint16_t u = x & 0xFFFF;
  uint16_t v = y & 0xFFFF;
  uint16_t w = z & 0xFFFF;

  // Get a signed version of the above for the grad function
  int16_t xx = (u >> 1) & 0x7FFF;
  int16_t yy = (v >> 1) & 0x7FFF;
  int16_t zz = (w >> 1) & 0x7FFF;
  uint16_t N = 0x8000L;

  u = FADE(u); v = FADE(v); w = FADE(w);


  // skip the log fade adjustment for the moment, otherwise here we would
  // adjust fade values for u,v,w
  int16_t X1 = LERP(grad16(P(AA), xx, yy, zz), grad16(P(BA), xx - N, yy, zz), u);
  int16_t X2 = LERP(grad16(P(AB), xx, yy-N, zz), grad16(P(BB), xx - N, yy - N, zz), u);
  int16_t X3 = LERP(grad16(P(AA+1), xx, yy, zz-N), grad16(P(BA+1), xx - N, yy, zz-N), u);
  int16_t X4 = LERP(grad16(P(AB+1), xx, yy-N, zz-N), grad16(P(BB+1), xx - N, yy - N, zz - N), u);

  int16_t Y1 = LERP(X1,X2,v);
  int16_t Y2 = LERP(X3,X4,v);

  int16_t ans = LERP(Y1,Y2,w);

  return ans;
}

uint16_t inoise16(uint32_t x, uint32_t y, uint32_t z) {
  int32_t ans = inoise16_raw(x,y,z);
  ans = ans + 19052L;
  uint32_t pan = ans;
  return (pan*220L)>>7;
}

int16_t inoise16_raw(uint32_t x, uint32_t y)
{
  // Find the unit cube containing the point
  uint8_t X = x>>16;
  uint8_t Y = y>>16;

  // Hash cube corner coordinates
  uint8_t A = P(X)+Y;
  uint8_t AA = P(A);
  uint8_t AB = P(A+1);
  uint8_t B = P(X+1)+Y;
  uint8_t BA = P(B);
  uint8_t BB = P(B+1);

  // Get the relative position of the point in the cube
  uint16_t u = x & 0xFFFF;
  uint16_t v = y & 0xFFFF;

  // Get a signed version of the above for the grad function
  int16_t xx = (u >> 1) & 0x7FFF;
  int16_t yy = (v >> 1) & 0x7FFF;
  uint16_t N = 0x8000L;

  u = FADE(u); v = FADE(v);

  int16_t X1 = LERP(grad16(P(AA), xx, yy), grad16(P(BA), xx - N, yy), u);
  int16_t X2 = LERP(grad16(P(AB), xx, yy-N), grad16(P(BB), xx - N, yy - N), u);

  int16_t ans = LERP(X1,X2,v);

  return ans;
}

uint16_t inoise16(uint32_t x, uint32_t y) {
  int32_t ans = inoise16_raw(x,y);
  ans = ans + 17308L;
  uint32_t pan = ans;
  return (pan*242L)>>7;
}

int16_t inoise16_raw(uint32_t x)
{
  // Find the unit cube containing the point
  uint8_t X = x>>16;

  // Hash cube corner coordinates
  uint8_t A = P(X);
  uint8_t AA = P(A);
  uint8_t B = P(X+1);
  uint8_t BA = P(B);

  // Get the relative position of the point in the cube
  uint16_t u = x & 0xFFFF;

  // Get a signed version of the above for the grad function
  int16_t xx = (u >> 1) & 0x7FFF;
  uint16_t N = 0x8000L;

  u = FADE(u);

  int16_t ans = LERP(grad16(P(AA), xx), grad16(P(BA), xx - N), u);

  return ans;
}

uint16_t inoise16(uint32_t x) {
  return ((uint32_t)((int32_t)inoise16_raw(x) + 17308L)) << 1;
}

int8_t inoise8_raw(uint16_t x, uint16_t y, uint16_t z)
{
  // Find the unit cube containing the point
  uint8_t X = x>>8;
  uint8_t Y = y>>8;
  uint8_t Z = z>>8;

  // Hash cube corner coordinates
  uint8_t A = P(X)+Y;
  uint8_t AA = P(A)+Z;
  uint8_t AB = P(A+1)+Z;
  uint8_t B = P(X+1)+Y;
  uint8_t BA = P(B) + Z;
  uint8_t BB = P(B+1)+Z;

  // Get the relative position of the point in the cube
  uint8_t u = x;
  uint8_t v = y;
  uint8_t w = z;

  // Get a signed version of the above for the grad function
  int8_t xx = (x>>1) & 0x7F;
  int8_t yy = (y>>1) & 0x7F;
  int8_t zz = (z>>1) & 0x7F;
  uint8_t N = 0x80;

  u = scale8_LEAVING_R1_DIRTY(u,u); v = scale8_LEAVING_R1_DIRTY(v,v); w = scale8(w,w);

  int8_t X1 = lerp7by8(grad8(P(AA), xx, yy, zz), grad8(P(BA), xx - N, yy, zz), u);
  int8_t X2 = lerp7by8(grad8(P(AB), xx, yy-N, zz), grad8(P(BB), xx - N, yy - N, zz), u);
  int8_t X3 = lerp7by8(grad8(P(AA+1), xx, yy, zz-N), grad8(P(BA+1), xx - N, yy, zz-N), u);
  int8_t X4 = lerp7by8(grad8(P(AB+1), xx, yy-N, zz-N), grad8(P(BB+1), xx - N, yy - N, zz - N), u);

  int8_t Y1 = lerp7by8(X1,X2,v);
  int8_t Y2 = lerp7by8(X3,X4,v);

  int8_t ans = lerp7by8(Y1,Y2,w);

  return ans;
}

uint8_t inoise8(uint16_t x, uint16_t y, uint16_t z) {
  return scale8(76+(inoise8_raw(x,y,z)),215)<<1;
}

int8_t inoise8_raw(uint16_t x, uint16_t y)
{
  // Find the unit cube containing the point
  uint8_t X = x>>8;
  uint8_t Y = y>>8;

  // Hash cube corner coordinates
  uint8_t A = P(X)+Y;
  uint8_t AA = P(A);
  uint8_t AB = P(A+1);
  uint8_t B = P(X+1)+Y;
  uint8_t BA = P(B);
  uint8_t BB = P(B+1);

  // Get the relative position of the point in the cube
  uint8_t u = x;
  uint8_t v = y;

  // Get a signed version of the above for the grad function
  int8_t xx = (x>>1) & 0x7F;
  int8_t yy = (y>>1) & 0x7F;
  uint8_t N = 0x80;

  u = scale8_LEAVING_R1_DIRTY(u,u); v = scale8(v,v);

  int8_t X1 = lerp7by8(grad8(P(AA), xx, yy), grad8(P(BA), xx - N, yy), u);
  int8_t X2 = lerp7by8(grad8(P(AB), xx, yy-N), grad8(P(BB), xx - N, yy - N), u);

  int8_t ans = lerp7by8(X1,X2,v);

  return ans;
}

uint8_t inoise8(uint16_t x, uint16_t y) {
  return scale8(69+inoise8_raw(x,y),237)<<1;
}

int8_t inoise8_raw(uint16_t x)
{
  // Find the unit cube containing the point
  uint8_t X = x>>8;

  // Hash cube corner coordinates
  uint8_t A = P(X);
  uint8_t AA = P(A);
  uint8_t B = P(X+1);
  uint8_t BA = P(B);

  // Get the relative position of the point in the cube
  uint8_t u = x;

  // Get a signed version of the above for the grad function
  int8_t xx = (x>>1) & 0x7F;
  uint8_t N = 0x80;

  u = scale8(u,u);

  int8_t ans = lerp7by8(grad8(P(AA), xx), grad8(P(BA), xx - N), u);

  return ans;
}

uint8_t inoise8(uint16_t x) {
  return scale8(69+inoise8_raw(x), 255)<<1;
}




void VisSimplexColor(bool movement_detected, LedRopeTCL* rope) {

  // put your main code here, to run repeatedly:
  if (movement_detected) {
    Serial.println("YES");
   // rope->FillColor(Color3i::Red());
    
    unsigned long now = millis();
    for (int i = 0; i < rope->length(); ++i) {
      long r = inoise8(now/4 + i*10);
      r = r & ~(0x4);
      r = (r*r) / 255;
      r = max(0, r - 64);
      
      
      
      long b = inoise8(now/4 - i*10 + 100);
      b = b & ~(0x4);
      b = (b*b) / 255;
      b = max(0, b - 64);
      
      rope->Set(i, Color3i(r, 0, b));
    }
  } else {
    Serial.println("NO");
    rope->FillColor(Color3i::Black());
  }
  rope->Draw();  
  
}

