////////////////////////////////////////////////////////////////////////////////
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License version 3, as published
// by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranties of
// MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
// PURPOSE.  See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program.  If not, see <http://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////

// Was looking for some nice lightning effects and stumbled on the shader:
//   https://www.shadertoy.com/view/3sXSD2

// Tinkered a bit with it and while I added nothing innovative I thought it 
// looked quite nice in the end and wanted to share. 
// NyanCat added just because why not?



// using a slightly adapted implementation of iq's simplex noise from
// https://www.shadertoy.com/view/Msf3WH with hash(), noise() and fbm()
vec2 hash (in vec2 p)
{
  p = vec2 (dot (p, vec2 (127.1, 311.7)),
            dot (p, vec2 (269.5, 183.3)));

  return -1. + 2.*fract (sin (p)*43758.5453123);
}

float noise (in vec2 p)
{
  const float K1 = .366025404;
  const float K2 = .211324865;

  vec2 i = floor (p + (p.x + p.y)*K1);
   
  vec2 a = p - i + (i.x + i.y)*K2;
  vec2 o = step (a.yx, a.xy);    
  vec2 b = a - o + K2;
  vec2 c = a - 1. + 2.*K2;

  vec3 h = max (.5 - vec3 (dot (a, a), dot (b, b), dot (c, c) ), .0);

  vec3 n = h*h*h*h*vec3 (dot (a, hash (i + .0)),
                         dot (b, hash (i + o)),
                         dot (c, hash (i + 1.)));

  return dot (n, vec3 (70.));
}

float fbm (in vec2 p, float time)
{
  float c =  cos(time/sqrt(3.0));
  float d =  noise (p                 );
  d += .5*   noise (p + vec2(+c  ,+0.0));
  d += .25*  noise (p + vec2(+0.0,+c  ));
  d += .125* noise (p + vec2(-c  ,+0.0));
  d += .0625*noise (p + vec2(+0.0,-c  ));
  d /= (1. + .5 + .25 + .125 + .0625);
  return .5 + .5*d;
}

vec2 toPolar (in vec2 p)
{
  float r = length (p);
  float a = atan (p.y, p.x);
  return vec2 (r, a); 
}

vec2 toRect (in vec2 p)
{
  float x = p.x*cos (p.y);
  float y = p.x*sin (p.y);
  return vec2 (x, y); 
}



vec3 electricNyanCat(in vec2 uv)
{
  const float thickness = 0.15;
  const float haze = 1.5;
  const float size = .075;
  const int count = 3;

  vec2 p = uv;

  vec2 pp = toPolar(p);
  pp.y += 0.2*p.x;
  p = toRect(pp);

  vec3 col = vec3(0.0);

  float a1 = smoothstep(0.05, 1.0, length(p - vec2(-0.0, -20.80)));
  float s1 = 1.0 / (a1 + 0.5)*1.1;
   float mytime = mod(iTime,1024.0);

  float e1 = 1.6 + 0.4*sin(mytime*sqrt(6.0));

  for (int i = 0; i < count; ++i)
  {
    float fi = float(i);
    float time = mytime + fi;
    float fe1 = (pow(fi + 1.0, 0.2))*e1;
    vec2 o1 = 1.5*time*vec2(0,-1);
    float d1 = abs ((p.y*haze)*thickness / (p.y - fe1*fbm (p + o1, time*0.11)*a1))*s1;
    col += d1*size*vec3 (.1, .8, 2.);
  }
  col /= float(count-1);
  return vec3(col.b);
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
  vec2 uv = fragCoord.xy/iResolution.xy;
  uv = 6.0*uv - 3.;
  uv.x *= 0.8* iResolution.x/iResolution.y;
  
  vec3 col = electricNyanCat(uv*2.0);

  fragColor = vec4 (col, 1.);
}
