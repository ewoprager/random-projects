#version 120

// It was expressed that some drivers required this next line to function properly
//precision mediump float;

uniform vec4 u_lightColour;
uniform vec4 u_colourMult;
uniform vec4 u_specular;
uniform float u_shininess;
uniform float u_specularFactor;
uniform sampler2D u_depthTexture;
uniform int u_portWidth;
uniform int u_portHeight;

varying vec3 v_colour;
varying vec3 v_normal;
varying vec3 v_surfaceToLight;
varying vec3 v_surfaceToView;
varying vec3 v_lightProjection;

float near = 0.1;
float far  = 100.0;
  
float LinearizeDepth(float depth){
	float z = depth * 2.0 - 1.0; // back to NDC
	return (2.0 * near * far) / (far + near - z * (far - near));
}

vec4 lit(float l, float h, float m) {
	return vec4(1.0,
				abs(l),
				(l > 0.0) ? pow(max(0.0, h), m) : 0.0,
				1.0);
}

void main() {
	vec4 diffuseColour = vec4(v_colour, 1.0f);
	vec3 a_normal = normalize(v_normal);
	vec3 surfaceToLight = normalize(v_surfaceToLight);
	vec3 halfVector = normalize(surfaceToLight + v_surfaceToView);
	vec4 litR = lit(dot(a_normal, surfaceToLight), dot(a_normal, halfVector), u_shininess);
	vec4 mainColour = vec4((u_lightColour * (diffuseColour * litR.y * u_colourMult + u_specular * litR.z * u_specularFactor)).rgb, diffuseColour.a);
	//gl_FragColor = texture2D(u_depthTexture, vec2(gl_FragCoord.x / u_portWidth, gl_FragCoord.y / u_portHeight));
	//float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
	//gl_FragColor = vec4(vec3(depth), 1.0);
	
	vec2 p = vec2((v_lightProjection.x + 1.0) / 2.0, (v_lightProjection.y + 1.0) / 2.0);
	//gl_FragColor = vec4(vec3(texture2D(u_depthTexture, p).r), 1.0);
	//float z = length(v_surfaceToLight);
	//float depth = LinearizeDepth(z) / far; // divide by far for demonstration
	//if(LinearizeDepth((v_lightProjection.z + 1.0)/2.0) / far > texture2D(u_depthTexture, p).r + 0.01){
	if(LinearizeDepth(((v_lightProjection.z + 1.0)/2.0)*0.99999)/far > texture2D(u_depthTexture, p).r){
		gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	} else {
		gl_FragColor = mainColour;
	}
	//gl_FragColor = vec4(vec3(texture2D(u_depthTexture, p).r), 1.0);
	//gl_FragColor = vec4(vec3(LinearizeDepth((v_lightProjection.z + 1.0)/2.0) / far), 1.0);
}
