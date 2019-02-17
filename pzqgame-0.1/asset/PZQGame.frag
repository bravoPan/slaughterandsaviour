#version 330 core

uniform sampler2D texturedata;
in  vec2 ex_Color;
out vec4 out_Color;

void main(void) {
    vec4 t = texture(texturedata,ex_Color);
	float dist = gl_FragCoord.z;
	vec4 fogColor = vec4(0.0,0.0,0.0,1.0);
	float alpha = exp(-(dist - 0.92) * 100);
	if(dist < 0.92){
        out_Color = t;
	} else {
		out_Color = mix(fogColor,t,alpha);
	}
}