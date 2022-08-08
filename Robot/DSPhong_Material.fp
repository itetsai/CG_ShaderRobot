// ADS Point lighting Shader
// Fragment Shader
// Richard S. Wright Jr.
// OpenGL SuperBible
#version 430

#define _SnowflakeAmount 400   // 雪花数  
#define _BlizardFactor 0.25           // 风的大小  
  
#define fragCoord gl_FragCoord  
vec4 fragColor;   

struct MaterialInfo
{
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
};
uniform float ftime;
uniform float windowsizex;
uniform float windowsizey;

uniform int mode;

uniform bool uset=false;


uniform MaterialInfo Material;
out vec4 vFragColor;

//lighting color
vec4    ambientColor = vec4(0.1,0.1,0.1,1);
vec4    diffuseColor = vec4(1,1,1,1);   
vec4    specularColor = vec4(1,1,1,1);

in vec3 vVaryingNormal;
in vec3 vVaryingLightDir;
in vec2 UV;

in vec3 Position;
in vec3 Normal;




in vec2 tex_coord;
uniform sampler2D tex;
uniform sampler2D tex2;

float Shininess = 128.0;//for material specular

vec2 iResolution = vec2(windowsizex, windowsizey);  

vec4 orginal()
{
	vec4 endcolor;
    float diff = max(0.0, dot(normalize(vVaryingNormal),
					normalize(vVaryingLightDir)));

    endcolor = diff * diffuseColor*vec4(Material.Kd,1);

    endcolor += ambientColor;

    vec3 vReflection = normalize(reflect(-normalize(vVaryingLightDir),normalize(vVaryingNormal)));//反射角
    float spec = max(0.0, dot(normalize(vVaryingNormal), vReflection));
    if(diff != 0) 
	{
		spec = pow(spec, Shininess);
		endcolor += specularColor*vec4(Material.Ka,1)*spec;
    }	
    
    
	return endcolor*2;
}

vec4 NoLight()
{
	return vec4(Material.Kd,1);
}

const int levels = 3;
const float scaleFactor = 1.0 / levels;


vec4 toonShade()
{
 vec3 s = vVaryingLightDir;
 float cosine = max( 0.0, dot( s, Normal ) );
 vec3 diffuse = Material.Kd * floor( cosine * levels ) * scaleFactor; 
 return vec4(vec3(0.8,0.8,0.8) * (Material.Ka + diffuse),1.0);
}


vec4 quant(vec4 cl, float n)  
{  
   cl.x = floor(cl.x * 255./n)*n/255.;  
   cl.y = floor(cl.y * 255./n)*n/255.;  
   cl.z = floor(cl.z * 255./n)*n/255.;  
     
   return cl;  
} 
float rnd(float x)  
{  
   return fract(  
         sin(  
               dot(vec2(x + 47.49, 38.2467 / (x + 2.3)),  
                     vec2(12.9898, 78.233))) * (43758.5453));  
}
float drawCircle(vec2 uv, vec2 center, float radius)  
{  
   return 1.0 - smoothstep(0.0, radius, length(uv - center));  
}  
vec4 snow(vec4 orgin)
{
	vec2 uv = fragCoord.xy / windowsizex;
	vec4 color = orgin;
	fragColor = color;
	float j;
	// 生成若干个圆，当前uv依次与这些圆心计算距离，未落在圆域内则为黑色，落在圆域内则为白色  
	for (int i = 0; i < _SnowflakeAmount; i++)
	{
		j = float(i);
		// 控制了不同雪花的下落速度 和 雪花的大小  
		float speed = 0.3 + rnd(cos(j))  * (0.7 + 0.5  * cos(j / (float(_SnowflakeAmount)* 0.25)));

		vec2 center = vec2((-0.25 + uv.y) * _BlizardFactor + rnd(j) + 0.1 * cos(ftime + sin(j)),
			mod(rnd(j) - speed  * (ftime * 1.5  * (0.1 + _BlizardFactor)), 0.95)
		);
		fragColor += vec4(0.9 * drawCircle(uv, center, 0.001 + speed * 0.012)); // 输出是这些圆的颜色叠加  
	}
	return fragColor;
}


vec4 square()
{
	if((mod(gl_FragCoord.x,10)>5&&mod(gl_FragCoord.y,10)<5)||mod(gl_FragCoord.x,10)<5&&mod(gl_FragCoord.y,10)>5)
	return vec4(0,1.0,0,1);
}

float sdCircle( vec2 p, in float r )  
{     
    return length(p) - r;       
}  
  
vec4 blackcircle( vec4 orgin )  
{  
    vec2 uv = -1.0 + 2.0 * gl_FragCoord.xy/iResolution.xy;  
    uv.x *= iResolution.x/iResolution.y;  
          
    float d = sdCircle( uv, 0.5 )+ftime;  
	float e = sdCircle( uv, 0.5 ); 
	
    vec3 col = vec3(1.0) - sign(e)*vec3(0.1,0.1,0.3);
	
    col *= 1.0 - exp(-2.0*abs(e));
	
    col *= 0.8 + 0.2*cos(120.0*d);
	
    col = mix( col, vec3(1.0), 1.0-smoothstep(0.0,0.02,abs(e)) );  
  
    return orgin*vec4( col, 1.0 );  
}  


vec4 blackslide(vec4 orgin)
{
	float x=gl_FragCoord.x;
	float y=gl_FragCoord.y;
	float middlex;
	float middley;
	middlex=windowsizex/2;
	middley=windowsizey/2;
	
	float d=sqrt(pow(x-middlex,2)+pow(y-middley,2));	
	float scrd=sqrt(pow(middlex,2)+pow(middley,2));	
	float b= d/(scrd/2);
		
	return orgin*vec4(1-b,1-b,1-b,1);	
}


vec4 watercolor(vec4 orgin)
{
	vec4 fColor = orgin;                    
	vec4 color = quant(fColor, 255./pow(2., 3));
	return color;  
}










  
vec3 color = vec3(1.0, 1.0, 1.0); // red  
// 一元随机函数  
float rand(float x)  
{  
   return fract(sin(x) * 4358.5453123);  
}  
// 二元随机函数  
float rand(vec2 co)  
{  
   return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5357);  
}    
// 火星文算法 —— _(┐「ε:)_   
float invader(vec2 p, float n)  
{  
   p.x = abs(p.x);  
   p.y = floor(p.y - 5.0);  
     
   // 2^x  
   float tmp = exp2(floor(p.x - 3.0 * p.y));  
     
   return step(p.x, 2.0) * step(1.0, floor( mod(n / tmp, 2.0) ));  
}   
float ring(vec2 uv, float rnd)  
{  
   float t = 0.6 * (ftime + 0.2 * rnd);  
   float i = floor(t / 2.0); // 确保圆心在某时间范围内不变  
     
   // 随机圆心位置  
   vec2 pos =  2.0 * vec2(rand(i * 0.123), rand(i * 2.371)) - 1.0;  
      
   // 动态放大半径：  
   // length(uv - pos)表示圆，t在增大，需要更大的uv才使得 diff 为 0，  
   // t有一定的随机性，可以实现圆环的闪烁效果  
   float diff = length(uv - pos) - mod(t, 2.0);  
   return 1. - smoothstep(0.0, 0.2, abs(diff) );   
}
vec4 planet(vec4 orgin)
{
  
   vec2 p = gl_FragCoord.xy;  
   vec2 xy = gl_FragCoord.xy / iResolution.xy;  
   vec2 uv = p / iResolution.xy - 0.5;  
   p.y += 120.0*ftime;   // 下落的速度  
   float r = rand(floor(p/8.0));   // 控制火星文和光环的随机闪烁  
     
   // 用于生成火星文  
   vec2 ip = mod(p, 8.0) - 4.0;  
     
   // 背景中心到四周的颜色渐变  
   float a = -.2 * smoothstep(0.1, 0.8, length(uv));  
     
   // invader * 光环  
   float b = invader(ip, 809999.0*r) * (0.06 + 0.3*ring(uv,r));  
     
   // 火星文雨的闪烁高光  
   
   
   float c = color.x * invader(ip, 809999.0*r) * max(0.0, 0.2*sin(10.0*r*ftime));  
     
   a += b;  
   a += c;  
   
   return orgin*vec4( color + vec3(a), 1.0);  
}






vec2 hash( vec2 p )   
{   
   p=vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)));   
     
   return fract(sin(p)*18.5453);  
}  
  
// return distance, and cell id  
vec2 voronoi( in vec2 x )  
{  
    vec2 n = floor( x );              // cell(n)  
    vec2 f = fract( x );              // 当前像素在cell space的坐标  
  
   vec3 m = vec3( 8. );               // 影响每个cell的大小，影响背景颜色  
     
   // 遍历相邻的9个cell  
    for( int j=-1; j<=1; j++ )  
    {  
       for( int i=-1; i<=1; i++ )  
       {  
           vec2  g = vec2( float(i), float(j) );  // 临近的 cell id offset  
           // n+g 临近的 cell(n+g) 的随机像素坐标 o (cell space)  
           vec2  o = hash( n + g );   // 影响cell的颜色  
             
           // ❤  
           vec2  r = g - f + (0.5+0.5*sin(ftime+6.2831*o));  
           //vec2  r = g - f + o;     // cell(n+g)[o] - cell(n)[f]   
            
           // ❤  
           float d = dot( r, r );  
   
           // 保存更小的d  
           if( d<m.x )  
           {  
              m = vec3( d, o );  
           }  
       }  
    }  
    return vec2( sqrt(m.x), m.y+m.z );  
}
vec4 Voronoi(vec4 orgin)
{ 
   vec2 p = gl_FragCoord.xy/max(iResolution.x,iResolution.y);  
      
   // computer voronoi patterm  
   vec2 c = voronoi( (14.0+6.0*sin(0.2*ftime))*p );  
     
   // colorize  
   vec3 col = 0.5 + 0.5*cos( c.y*6.2831 + vec3(0.0,1.0,2.0) ); // cell的随机颜色  
       
   col *= clamp(1.0 - 0.6*c.x*c.x, 0.0, 1.0);  
//   col -= (1.0-smoothstep( 0.05, 0.06, c.x));                  // 画Voronoi的site点集  
     
   return orgin*0.5+vec4( col*0.5, 0); 

}


vec4 Sephia(vec4 orgin)
{
    float grey = dot(orgin.xyz, vec3(0.299, 0.587, 0.114));   
    return vec4(grey * vec3(1.2, 1.0, 0.8), 1.0);   
}





vec3 color_bg = vec3(0.0);  
vec3 color_inner = vec3(1.0,0.9,0.16);  
  
vec3 color_outer = vec3(0.12,0.59,0.21);  
//vec3 color_outer = mix(color_bg, color_inner, 0.3); // also nice effect  
  
float timeScale = 1.0;  // 时间(速度)的缩放尺度  
float mapScale = .8;    // cell的缩放尺度  
  
#define cellCount 20.0  
  
// size in pixels inner/outer with mapscale 1.0  
// cell的内圈大小和外圈大小  
vec2 cellSize = vec2(45.0, 60.0);   
  
vec3 powerToColor(vec2 power)  
{  
    // 控制颜色平滑程度  
    float tMax = pow(1.12, mapScale*3.2);  
    float tMin = 1.0 / tMax;  
      
    vec3 color = mix(color_bg, color_outer, smoothstep(tMin,tMax,power.y));// 外圈颜色  
    color = mix(color, color_inner, smoothstep(tMin,tMax,power.x));        // 内圈颜色  
    return color;  
}  
  
  
vec2 getCellPower( vec2 coord, vec2 pos, vec2 size )  
{  
    vec2 power;  
    // 当前像素离cell越远，power越小，反之越大  
    power = (size*size) / dot(coord-pos,coord-pos);  
    power *= power * sqrt(power); // ^5  
      
    return power;  
}  
  
  
vec3 CellMerge()  
{  
    float T = ftime * .1 * timeScale / mapScale;  
      
    vec2 hRes = 0.5*iResolution.xy;  
      
    vec2 pos;  
    vec2 power = vec2(0.0,0.0);  
      
      
    for(float x = 1.0; x != cellCount + 1.0; ++x)  
    {  
        // 随机cell的位置  
        pos = hRes * vec2(sin(T*fract(0.246*x)+x*3.6)*cos(T*fract(0.374*x)-x*fract(0.6827*x))+1.,  
                          cos(T*fract(0.246*x)+x*3.6)*sin(T*fract(.374*x)-x*fract(0.6827*x))+1.);  
        // 根据当前像素的coord和cell的pos和size计算cell的power  
        power += getCellPower(gl_FragCoord.xy, pos, cellSize*(.75+fract(0.2834*x)*.25) / mapScale);  
    }  
  
    
    return powerToColor(power);  
}






// 动画相关参数  
float dotSize = 0.01;  
float iteration = 100.;  
float xAmp = 0.3;  
float yAmp = 0.1;  
float speed = 0.05;  
float rotateCanvas = 0.;  
float rotateParticles = 1.;  
float rotateMultiplier = 10.;  
vec2 pos = vec2(.5, .5);  
float xFactor = 0.2;  
float yFactor = 0.2;  


 
  
//rotation  
vec2 rot(vec2 uv, float a)  
{  
    // [uv.x uv.y] * [cos(a),  sin(a),  
    //                -sin(a), cos(a)]  
    return vec2(uv.x * cos(a) - uv.y * sin(a), uv.y * cos(a) + uv.x * sin(a));  
}  
  
float circle(vec2 uv, float size)  
{  
    // 向量长度在范围内为白色，范围外为黑  
    return length(uv) > size ? 0.0 : 1.0;  
}

vec4 rotateParticle(vec4 orgin)
{

	int flag=0;
	vec2 uv = gl_FragCoord.xy / iResolution; // [0., 1.]  
    uv -= vec2(pos);// [-.5, .5]  
    uv.x *= iResolution.x / iResolution.y;    // ratio = X/Y  
  
    vec3 color = vec3(1);  
  
    //第一个粒子(i == 0)的 uv【其实第一个粒子是看不见的，因为size == 0】  
    uv = rot(uv, rotateCanvas);  
  
    // 99个粒子  
    for (float i = 0.0; i < 100.0; i++)  
    {  
        // set max number of iterations  
        if (iteration < i)  
            break;  
  
        // x:sin() y:cos() 的旋转动画  
        vec2 new_pos = vec2(cos(i * xFactor * (ftime*1.5 * speed)) * xAmp,  
                sin(i * yFactor * (ftime*1.5 * speed)) * yAmp);  
        // st:新位置到uv的向量  
        vec2 st = uv - new_pos;  
  
        // 计算st向量的长度，并设置粒子的尺寸（从小到大）  
        float dots = circle((st), dotSize * (i * 0.01));  
  
        //旋转当前粒子的uv得到下一个粒子的uv  
        uv = rot(uv, rotateParticles * rotateMultiplier);  
  
        // 更新纹素的颜色（只要当前纹素属于其中一个粒子，则置为白色，否则仍然是黑色）  
        // 严谨的话是应该clamp，但是超过1也是白色
		
		if(dots==1.0)
		{
			color= vec3(sin(i),cos(i),sin(i)*cos(i));
			color*=1.5;
			flag=1;
		}  
    }  
	if(flag==1)
		return vec4(vec3(color), 1.0);
	else
		return orgin;
}



vec2 sketchSize = vec2(windowsizex, windowsizey);  
  
const int lookupSize = 64; // 查找表的大小  
const float errorCarry = 0.5; // 误差传输率  
  
float getGrayscale(vec2 coords,vec4 orgin)  
{  
   vec2 uv = coords / sketchSize.xy;  
   // processing is already using inverted y coordinates  
   // uv.y = 1.0-uv.y;   
   vec3 sourcePixel = orgin.xyz;  
   return length(sourcePixel * vec3(0.2126,0.7152,0.0722));  
}
vec4 FloydSteinberg(vec4 orgin)
{
	      float xError = 0.0;  
      for(int xLook=0; xLook<lookupSize; xLook++)  
      {  
         float grayscale = getGrayscale(gl_FragCoord.xy + vec2(-lookupSize+xLook,0),orgin);//   
         grayscale += xError;   // 在X方向上的误差传播(从最左端到当前像素)  
         float bit = (grayscale >= 0.5) ? 1.0 : 0.0; // t = (w+b)/2.  
         xError = (grayscale - bit)*errorCarry;  
      }  
        
      float yError = 0.0;  
      for(int yLook=0; yLook<lookupSize; yLook++)  
      {  
         float grayscale = getGrayscale(gl_FragCoord.xy + vec2(0,-lookupSize+yLook),orgin);  
         grayscale += yError;   // 在Y方向上的误差传播 (从最上端到当前像素)  
         float bit = (grayscale >= 0.5) ? 1.0 : 0.0; // t = (w+b)/2.  
         yError = (grayscale - bit)*errorCarry;  
      }  
        
      float finalGrayscale = getGrayscale(gl_FragCoord.xy,orgin);  
      finalGrayscale += xError * .5; // X,Y方向的误差各占一半  
      finalGrayscale += yError * .5;  
      finalGrayscale = clamp(finalGrayscale, 0., 1.);  
        
      float finalBit = finalGrayscale >= 0.5 ? 1.0 : 0.0;  
        
      return vec4(finalBit, finalBit, finalBit, 1.); 
}


















vec2 hash22( vec2 p )  
{  
    p = vec2( dot(p,vec2(127.1,311.7)),  
              dot(p,vec2(269.5,183.3)) );  
  
    return -1.0 + 2.0*fract(sin(p)*43758.5453123);  
}  
  
float hash21(vec2 p)  
{  
    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);  
    //vec3 p3  = fract(vec3(p.xyx) * .1931);  
    //p3 += dot(p3, p3.yzx + 19.19);  
    //return fract((p3.x + p3.y) * p3.z);  
}
  
float noise( in vec2 p )  
{  
    vec2 i = floor( p );  
    vec2 f = fract( p );  
      
    // Ease Curve  
    //vec2 u = f*f*(3.0-2.0*f);  
    vec2 u = f*f*f*(6.0*f*f - 15.0*f + 10.0);  
  
    return mix( mix( dot( hash22( i + vec2(0.0,0.0) ), f - vec2(0.0,0.0) ),   
                     dot( hash22( i + vec2(1.0,0.0) ), f - vec2(1.0,0.0) ), u.x),  
                mix( dot( hash22( i + vec2(0.0,1.0) ), f - vec2(0.0,1.0) ),   
                   dot( hash22( i + vec2(1.0,1.0) ), f - vec2(1.0,1.0) ), u.x), u.y);  
      
    //return dot(hash22(i+vec2(0.0, 0.0)), f-vec2(0.0, 0.0));  
    //return dot(hash22(i+vec2(1.0, 0.0)), f-vec2(1.0, 0.0));  
    //return mix(dot(hash22(i+vec2(0.0, 0.0)), f-vec2(0.0, 0.0)),  
    //           dot(hash22(i+vec2(1.0, 0.0)), f-vec2(1.0, 0.0)), u.x);  
      
    //return dot(hash22(i+vec2(0.0, 1.0)), f-vec2(0.0, 1.0));  
    //return dot(hash22(i+vec2(1.0, 1.0)), f-vec2(1.0, 1.0));  
    //return mix(dot(hash22(i+vec2(0.0, 1.0)), f-vec2(0.0, 1.0)),  
    //           dot(hash22(i+vec2(1.0, 1.0)), f-vec2(1.0, 1.0)), u.x);  
}  
  
float noise_fractal(in vec2 p)  
{  
    p *= 5.0;  
    mat2 m = mat2( 1.6,  1.2, -1.2,  1.6 );  
    float f  = 0.5000*noise(p); p = m*p;  
    f += 0.2500*noise(p); p = m*p;  
    f += 0.1250*noise(p); p = m*p;  
    f += 0.0625*noise(p); p = m*p;  
      
    return f;  
}  
  
float noise_sum_abs(vec2 p)  
{  
    float f = 0.0;  
    p = p * 7.0;  
    f += 1.0000 * abs(noise(p)); p = 2.0 * p;  
    f += 0.5000 * abs(noise(p)); p = 2.0 * p;  
    f += 0.2500 * abs(noise(p)); p = 2.0 * p;  
    f += 0.1250 * abs(noise(p)); p = 2.0 * p;  
    f += 0.0625 * abs(noise(p)); p = 2.0 * p;  
  
    return f;  
}  
  
float value_noise(vec2 p)  
{  
    p *= 56.0;  
    vec2 pi = floor(p);  
    //vec2 pf = p - pi;  
    vec2 pf = fract(p);  
  
    vec2 w = pf * pf * (3.0 - 2.0 * pf);  
  
    // 它把原来的梯度替换成了一个简单的伪随机值，我们也不需要进行点乘操作，  
    // 而直接把晶格顶点处的随机值按权重相加即可。  
    return mix(mix(hash21(pi + vec2(0.0, 0.0)), hash21(pi + vec2(1.0, 0.0)), w.x),  
              mix(hash21(pi + vec2(0.0, 1.0)), hash21(pi + vec2(1.0, 1.0)), w.x),  
              w.y);  
}  
  
float simplex_noise(vec2 p)  
{  
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;  
    const float K2 = 0.211324865; // (3-sqrt(3))/6;  
    // 变换到新网格的(0, 0)点  
    vec2 i = floor(p + (p.x + p.y) * K1);  
    // i - (i.x+i.y)*K2换算到旧网格点  
    // a:变形前输入点p到该网格点的距离  
    vec2 a = p - (i - (i.x + i.y) * K2);  
    vec2 o = (a.x < a.y) ? vec2(0.0, 1.0) : vec2(1.0, 0.0);  
    // 新网格(1.0, 0.0)或(0.0, 1.0)  
    // b = p - (i+o - (i.x + i.y + 1.0)*K2);  
    vec2 b = a - o + K2;  
    // 新网格(1.0, 1.0)  
    // c = p - (i+vec2(1.0, 1.0) - (i.x+1.0 + i.y+1.0)*K2);  
    vec2 c = a - 1.0 + 2.0 * K2;  
    // 计算每个顶点的权重向量，r^2 = 0.5  
    vec3 h = max(0.5 - vec3(dot(a, a), dot(b, b), dot(c, c)), 0.0);  
    // 每个顶点的梯度向量和距离向量的点乘，然后再乘上权重向量  
    vec3 n = h * h * h * h * vec3(dot(a, hash22(i)), dot(b, hash22(i + o)), dot(c, hash22(i + 1.0)));  
      
    // 之所以乘上70，是在计算了n每个分量的和的最大值以后得出的，这样才能保证将n各个分量相加以后的结果在[-1, 1]之间  
    return dot(vec3(70.0, 70.0, 70.0), n);  
}
vec4 noise(vec4 orgin)
{
	vec2 p = (gl_FragCoord.xy+ftime*1000) / iResolution.xy;  
  
    vec2 uv = p * vec2(iResolution.x/iResolution.y,1.0)*4;  
      
    float f = 0.0;  
      
    /*// 1: perlin noise    
    if( p.x<0.2 )  
    {  
        f = noise( 16.0 * uv );  
    }  
    // 2: fractal noise (4 octaves)  
    else if(p.x>=0.2 && p.x<0.4)    
    {  
        f = noise_fractal(uv);  
    }  
    // 3：fractal abs noise  
    else if(p.x>=0.4 && p.x<0.6)  
    {  
        f = noise_sum_abs(uv);  
    }  */
    // 4: value noise  
    //else if(p.x>=0.6 && p.x<0.8)  
   // {  
        f = value_noise(uv);  
   // }  
    // 5:simplex_noise  
    /*else  
    {  
        f = simplex_noise(16.0*uv);  
    } */ 
  
    f = 0.5 + 0.5*f;  
	
    // 分割线：注意如果第三个参数超过了限定范围就不进行插值  
 /*   f *= smoothstep(0.0, 0.005, abs(p.x-0.2));  
    f *= smoothstep(0.0, 0.005, abs(p.x-0.4));    
    f *= smoothstep(0.0, 0.005, abs(p.x-0.6));  
    f *= smoothstep(0.0, 0.005, abs(p.x-0.8)); */ 
    

	float gray = 0.299 * orgin.x + 0.587 * orgin.y + 0.114 * orgin.z;	
    return vec4(vec3(gray),0.05)*vec4( f, f, f, 1.0 )*vec4( f, f, f, 1.0 );
}





float snoise(vec3 uv, float res)  
{  
   // ❤  
   const vec3 s = vec3(1e0, 1e2, 1e3);  
   //const vec3 s = vec3(1., 100., 1000.);  
     
   uv *= res;  
     
   vec3 uv0 = floor(mod(uv, res))*s;  
   vec3 uv1 = floor(mod(uv+vec3(1.), res))*s;  
     
   vec3 f = fract(uv);   
   // 缓和函数  
   f = f*f*(3.0-2.0*f);  
     
   //  ❤扭曲图像   
   vec4 v = vec4(uv0.x+uv0.y+uv0.z, uv1.x+uv0.y+uv0.z,  
               uv0.x+uv1.y+uv0.z, uv1.x+uv1.y+uv0.z);  
   //vec4 v = vec4(uv0.x, uv0.y, uv1.x, uv1.y);  
     
  
   // ❤ 影响形状和速度  
   vec4 r = fract(sin(v*1e-1)*1e3);  
   //vec4 r = fract(sin(v));  
   float r0 = mix(mix(r.x, r.y, f.x), mix(r.z, r.w, f.x), f.y);  
     
   // ❤ 影响形状和速度  
   r = fract(sin((v + uv1.z - uv0.z)*1e-1)*1e3);  
   //r = fract(sin(v));  
   float r1 = mix(mix(r.x, r.y, f.x), mix(r.z, r.w, f.x), f.y);  
     
   return mix(r0, r1, f.z)*2.-1.;  
}  
vec4 fire(vec4 orgin)
{
   // 换算到[(-.5, -.5), (.5, .5)]  
   vec2 p = -.5 + gl_FragCoord.xy / iResolution.xy;  
     
   // 换算到[(-1., -1.), (1., 1.)]  
   //vec2 p = (2.*gl_FragCoord.xy - iResolution.xy) / iResolution.xy;  
   //p *= 0.5; // 放大2倍  
     
   // 根据屏幕纵横比变换  
   p.x *= iResolution.x/iResolution.y;  
     
   // 屏幕中心到边界，亮度由高到低  
   // 定义火焰的基本形状  
   float color = 3.0 - (3.*length(2.*p));  
   //float color = 3.0 - (3.*length(2.*p - vec2(-.5, .5)));  
   //float color = 3.0 - (3.*length(2.*p - vec2(2*p.x, 0.0)));  
     
   // ❤ 控制火焰发散的形式  
   vec3 coord = vec3(atan(p.x,p.y)/6.2832+.5, length(p)*.4, 0.5);  
   //vec3 coord = vec3(p.y, 0, 0);  
   //vec3 coord = vec3(p.x, 0, 0);  
   //vec3 coord = vec3(atan(p.x,p.y), 0, 0);  
   //vec3 coord = vec3(length(p)*.4, 0, 0);  
     
   // 控制颜色的层次  
   for(int i = 1; i <= 7; i++)  
   {  
      float power = pow(2.0, float(i));  
      color += (1.5 / power) *   
      snoise(coord + vec3(0.,-ftime*.05, ftime*.01), power*16.);  
      //snoise(coord + vec3(0., 0.05, 0.01), power*16.);  
   }  
   return orgin*vec4( color, pow(max(color,0.),2.)*0.4, pow(max(color,0.),3.)*0.15 , 1.0)*vec4( color, pow(max(color,0.),2.)*0.4, pow(max(color,0.),3.)*0.15 , 1.0); 
}

vec4 gray(vec4 orgin)
{
	float gray = 0.2126 * orgin.r + 0.7152 * orgin.g + 0.0722 * orgin.b;
    return vec4(vec3(gray),1.0);
}


vec4 negative(vec4 orgin)
{
	return vec4(1-orgin.x,1-orgin.y,1-orgin.z,1.0);
}


void main(void)
{ 
	
    vFragColor=orginal();
	
	if(uset==true)
	{
	vec4 color = texture(tex,UV);		
    vFragColor = color; 
	}
	
	switch(mode)
	{
		case -1:
		vFragColor=NoLight();
		if(uset==true)
		{
		vec4 color = texture(tex,UV);		
		vFragColor = color; 
		}
		break;
		
		case 1:
		vFragColor=gray(vFragColor);
		break;
		
		case 2:
		vFragColor=blackslide(vFragColor);
		break;
		
		case 3:
		vFragColor=blackcircle(vFragColor);
		break;
		
		case 4:
		vFragColor=negative(vFragColor);
		break;
		
		case 5:
		vFragColor=toonShade()*3;
		if(uset==true)
		{
		vec4 color = texture(tex,UV);		
		vFragColor = color;
		}
		break;
		
		case 6:
		vFragColor=watercolor(vFragColor)*1.5;	
		break;
		
		case 7:
		vFragColor=planet(vFragColor);
		break;
		
		case 8:
		vFragColor=Sephia(vFragColor);
		break;
		
		case 9:
        vec4 oo=vFragColor;
        oo.rgb=CellMerge();
        
		vFragColor=vFragColor*oo;
		break;
		
		case 10:
		vFragColor=FloydSteinberg(vFragColor)*1.5;
		break;
		
		case 11:
		vFragColor=noise(vFragColor);
		break;
		
		case 12:
		vFragColor=Voronoi(vFragColor);
		break;
		
		case 13:
		vFragColor=rotateParticle(vFragColor);
		break;
		
		case 14:
		vFragColor=snow(vFragColor);
		break;
		
		case 15:
		vFragColor=fire(vFragColor)*vec4(0.6,0.6,0.6,1);
		break;
	}
	
    
    
    
    
}









