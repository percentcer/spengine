#version 330 core

uniform float iTime;
uniform vec2 iResolution;
out vec4 FragColor;

#define FAR 30.
#define TAU 6.283
#define REPETITION_PERIOD 2.3
#define HALF_PERIOD REPETITION_PERIOD * .5
vec3 cell(vec3 p) { return floor((p + vec3(HALF_PERIOD)) / REPETITION_PERIOD); }

mat3 roty(float a)
{
    return mat3(cos(a), 0., sin(a), 0., 1., 0., -sin(a), 0., cos(a));
}

mat3 rotx(float a)
{
    return mat3(1., 0., 0., 0., cos(a), sin(a), 0., -sin(a), cos(a));
}

// https://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
float opSmoothSubtraction( float d1, float d2, float k ) {
    float h = clamp(0.5 - 0.5 * ( d2 + d1 ) / k, 0.0, 1.0 );
    return mix( d2, -d1, h ) + k * h * (1.0 - h); 
}

// shane's compact version of iq's 3d noise
float n3D(vec3 p){
	const vec3 s = vec3(7, 157, 113);
	vec3 ip = floor(p); p -= ip; 
    vec4 h = vec4(0., s.yz, s.y + s.z) + dot(ip, s);
    p = p*p*(3. - 2.*p); //p *= p*p*(p*(p * 6. - 15.) + 10.);
    h = mix(fract(sin(h)*43758.5453), fract(sin(h + s.x)*43758.5453), p.x);
    h.xy = mix(h.xz, h.yw, p.y);
    return mix(h.x, h.y, p.z); // Range: [0, 1].
}

// https://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
float sdCappedCone(vec3 p, vec3 a, vec3 b, float ra, float rb) {
  float rba = rb - ra;
  float baba = dot(b - a, b - a);
  float papa = dot(p - a, p - a);
  float paba = dot(p - a, b - a) / baba;
  float x = sqrt(papa - paba * paba * baba);
  float cax = max(0.0, x - ((paba < 0.5) ? ra : rb));
  float cay = abs(paba - 0.5) - 0.5;
  float k = rba * rba + baba;
  float f = clamp((rba * (x - ra) + paba * baba) / k, 0.0, 1.0);
  float cbx = x - ra - f * rba;
  float cby = paba - f;
  float s = (cbx < 0.0 && cay < 0.0) ? -1.0 : 1.0;
  return s *
         sqrt(min(cax * cax + cay * cay * baba, cbx * cbx + cby * cby * baba));
}

// https://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
float sdRoundCone( vec3 p, float r1, float r2, float h )
{
  vec2 q = vec2( length(p.xz), p.y );
    
  float b = (r1-r2)/h;
  float a = sqrt(1.0-b*b);
  float k = dot(q,vec2(-b,a));
    
  if( k < 0.0 ) return length(q) - r1;
  if( k > a*h ) return length(q-vec2(0.0,h)) - r2;
        
  return dot(q, vec2(a,b) ) - r1;
}

float blueberry(vec3 p) {
    vec3 bp = p;
    bp.z *= 1.2;
    float base = length(bp) - 1. + n3D(p.xyy*2.)*.1;
    
    vec3 sp = p;
    sp.xy *= (2.5 + sin(atan(sp.y,sp.x)*5.6)*.3);
    float starcap = sdCappedCone(sp, vec3(0.,0.,.6), vec3(0.,0.,1.0), .7, .1);
    
    vec3 tp = p;
    tp.z += .8;
    float butt = length(tp) - .05;
    
    float pass1 = opSmoothSubtraction(starcap, base, .1);
    return opSmoothSubtraction(butt, pass1, .05);
}

float rubus(vec3 p) {
    vec3 bp = p;
    bp.y += .5;
    float base = sdRoundCone( bp, .8, 1., .8 );
    
    vec3 cp = p;
    cp.y -= .3;
    float remove = sdRoundCone( cp, .4, .5, .7 );
    
    base = opSmoothSubtraction(remove, base, .1);
    return base - n3D(p)*.1;
    
    // how to map cone?
    //vec2 voruv = vec2(.5 + atan(p.x, p.z)/TAU, p.y);
    //float voroffset = 0.;//1.-vor(voruv*vec2(4.,1.)*.2);
    
    //return base - smoothstep(0.,1.,voroffset)*.1;
}

// https://iquilezles.org/www/articles/distfunctions/distfunctions.htm
vec3 opRep(in vec3 p, in vec3 c)
{
    vec3 cell = cell(p);
    vec3 localpos = mod(p + c / 2., c) - c / 2.;
    localpos *= roty(sin(iTime)*5. * p.y);
    //vec3 noise = texture(iChannel0, vec2(cell.x, cell.y * cell.z) * .1).rgb;
    //vec3 offs = noise - .5;
    
    return localpos;//*rotx(sin(iTime*noise.x)*noise.y) + offs*2.;
}

vec2 map(vec3 p) {
	vec3 local = opRep(p, vec3(REPETITION_PERIOD, REPETITION_PERIOD, 0.));
    vec3 c = cell(p);
    if (mod(c.x, 3.) == 0.)
        return vec2(blueberry(local), 1.);
    
    if (mod(c.x, 3.) == 1.)
        return vec2(rubus(local), 2.);
    
    if (mod(c.x, 3.) == 2.)
        return vec2(rubus(local), 3.);
}

vec3 norm(vec3 p) {
    vec2 eps = vec2(.001, .0);
    return normalize(map(p).x - vec3(map(p - eps.xyy).x,
                                   map(p - eps.yxy).x,
                                   map(p - eps.yyx).x));
}

vec3 bluebColor(vec3 p) {
    vec3 base = vec3(20.,45.,246.)/255.;
    vec3 mottling = (2.+vec3(n3D(p*6.)));
    return base * mottling * pow(length(p), 10.);
}

vec3 blakbColor(vec3 p) {
    vec3 base = vec3(2.,1.,5.)/255.;
    vec3 mottling = (2.+vec3(n3D(p*6.)));
    return base * mottling;
}

vec3 raspbColor(vec3 p) {
    vec3 base = vec3(154.,13.,10.)/255.;
    vec3 mottling = (2.+vec3(n3D(p*6.)));
    return base * mottling;
}

vec3 color(vec3 p, vec3 rd, float mat) {
    vec3 diff = vec3(0.);
    vec3 pLocal = opRep(p, vec3(REPETITION_PERIOD));
    
    if (mat == 1.) { diff = bluebColor(pLocal); }
    else if (mat == 2.) { diff =  raspbColor(pLocal); }
    else if (mat == 3.) { diff =  blakbColor(pLocal); }
    
    vec3 lp = vec3(1.,1.,4.);
    //vec3 lc = vec3(1.);
    
    vec3 n = norm(p);
    vec3 p2l = normalize(lp - p);
    
    vec3 diffLit = diff * max(0., dot(n, p2l));
    vec3 spec = vec3( pow(max(0.,dot(reflect(-rd, n), -p2l)), 100.) );
    
    return max(mat3(.6,.2,.2,2.,.6,.2,.2,.2,.6)*diff*.1, diffLit + spec);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = (2.*fragCoord-iResolution.xy)/iResolution.y;

    vec3 col = vec3(.5);
    
    vec3 ro = vec3(0., 0., 10.);
    vec3 rd = normalize(vec3(uv, -2.));
   
    #if 0
    vec2 mouserot = iMouse.xy*8./iResolution.xy - 4.;
    ro *= (roty(mouserot.x) * rotx(mouserot.y));
    rd *= (roty(mouserot.x) * rotx(mouserot.y));
    #endif
    
    for (float t = 0.; t < FAR;) {
        vec3 p = ro + rd * t;
        
        vec2 dmat = map(p);
        if (dmat.x < .01) { col = color(p, rd, dmat.y); break; }
        t+=dmat.x*.85;
    }
    col = pow(col, vec3(.4545));
    fragColor = vec4(col,1.0);
}

void main()
{
    mainImage(FragColor, gl_FragCoord.xy);
}
