#version 330 core

uniform sampler2D texturedata;
varying float distToCamera;
in  vec2 ex_Color;
out vec4 out_Color;

void main(void) {
    vec4 t = texture(texturedata,ex_Color);
	float dist = distToCamera;
	vec4 fogColor = vec4(0.0,0.0,0.0,1.0);
	float alpha = exp(-(dist - 5) * 1.2);
	if(dist < 5){
        out_Color = t;
	} else {
		out_Color = mix(fogColor,t,alpha);
	}
}