#version 430 core

in vec2 vUV;

uniform vec3 Color;
uniform bool isTextureMap;
uniform bool isLava;
uniform sampler2D Texture;
uniform sampler2D Texture1;
uniform float Theta;
uniform float time;

//lighting color
vec4    ambientColor = vec4(0.1,0.1,0.1,1);
vec4    diffuseColor = vec4(0.8,0.8,0.8,1);   
vec4    specularColor = vec4(1,1,1,1);

in vec3 vVaryingNormal;
in vec3 vVaryingLightDir;

float Shininess = 128.0;//for material specular

out vec4 fColor;

void main()
{
	float diff = max(0.0, dot(normalize(vVaryingNormal),
						normalize(vVaryingLightDir)));
	vec3 vReflection = normalize(reflect(-normalize(vVaryingLightDir),
						  normalize(vVaryingNormal)
						  )
				);//反射角
	float spec = max(0.0, dot(normalize(vVaryingNormal), vReflection));
	if(isTextureMap){
		if(isLava){			
			vec2 position = - 1.0 + 2.0 * vUV;	
			vec4 noise = texture2D( Texture1, vUV );
			vec2 T1 = vUV + vec2( 1.5, - 1.5 ) * time * 0.02;
			vec2 T2 = vUV + vec2( - 0.5, 2.0 ) * time * 0.01;

			T1.x += noise.x * 2.0;
			T1.y += noise.y * 2.0;
			T2.x -= noise.y * 0.2;
			T2.y += noise.z * 0.2;

			float p = texture2D( Texture1, T1 * 2.0 ).a;

			vec4 color = texture2D( Texture, T2 * 2.0 );
			vec4 temp = color * ( vec4( p, p, p, p ) * 2.0 ) + ( color * color - 0.1 );

			if( temp.r > 1.0 ) { temp.bg += clamp( temp.r - 2.0, 0.0, 100.0 ); }
			if( temp.g > 1.0 ) { temp.rb += temp.g - 1.0; }
			if( temp.b > 1.0 ) { temp.rg += temp.b - 1.0; }
			

			fColor = temp;

			float depth = gl_FragCoord.z / gl_FragCoord.w;
			const float LOG2 = 1.442695;
			float fogFactor = exp2( - 0.45 * 0.45 * depth * depth * LOG2 );

			fColor = mix( fColor, vec4( vec3(0,0,0), fColor.w ), fogFactor );
		}else{
			float sinX = sin ( Theta );
			float cosX = cos ( Theta );
			float sinY = sin ( Theta );
			mat2 rotationMatrix = mat2( cosX, sinX, -sinY, cosX);
			vec2 newUV = vUV * rotationMatrix;
			vec4 c = texture2D(Texture,newUV);
			if(c.a < 0.1)
				discard;
			fColor = vec4(c.r,c.g,c.b,1.0);
		}		
	}
	else{
		
		
		fColor = vec4(Color,1.0);
		
	}
}
