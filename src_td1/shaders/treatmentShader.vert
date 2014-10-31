#version 330
layout(location=0) in vec3 vx_pos; // Indice 0
layout(location=2) in vec2 vx_uvs; // Indice 2

// une variable out a creer (coordonnees de texture)
out vec2 texCoord;

void main()
{
	// Recuperation et transmission des coordonnees de texture
  texCoord = vx_uvs;

	// Resize to screen coordinates [-1,1]x[-1,1]
	vec3 position;
	position.xy = 2*vx_pos.xy - vec2(1.0);
	position.z = 0.0;

	gl_Position = vec4(position,1.0);
}
