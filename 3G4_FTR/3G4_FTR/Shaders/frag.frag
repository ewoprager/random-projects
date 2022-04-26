#version 120

uniform vec4 u_lightColour;
uniform vec4 u_colourMult;
uniform vec4 u_specular;
uniform float u_shininess;
uniform float u_specularFactor;

varying vec3 v_colour;
varying vec3 v_normal;
varying vec3 v_surfaceToLight;
varying vec3 v_surfaceToView;
varying vec3 v_lightProjection;

// I took this function from somewhere on the internet a long time ago and have adapted it since
vec2 lit(float nDotL, float nDotH, float shininess) {
	return vec2(abs(nDotL),	(nDotL > 0.0 && nDotH > 0.0) ? pow(nDotH, shininess) : 0.0);
}

void main() {
	vec4 diffuseColour = vec4(v_colour, 1.0f);
	vec3 a_normal = normalize(v_normal);
	vec3 surfaceToLight = normalize(v_surfaceToLight);
	vec3 halfVector = normalize(surfaceToLight + v_surfaceToView);
	vec2 litR = lit(dot(a_normal, surfaceToLight), dot(a_normal, halfVector), u_shininess);
	vec4 mainColour = vec4((u_lightColour * (diffuseColour * litR.x * u_colourMult + u_specular * litR.y * u_specularFactor)).rgb, diffuseColour.a);
	gl_FragColor = mainColour;
}
