#version 330

uniform sampler2D basetex;
uniform int treatment;
uniform vec3 sepiaColor;

// une variable in a creer (coordonnee de texture)
in vec2 texCoord;

void main()
{
	// 0 - initialisation ou recopie de la texture
  gl_FragColor.rgba = vec4(1.0);

  vec4 pixelColor = texture2D(basetex, texCoord.xy);

  if (treatment == 1)
  {
    gl_FragColor.bgra = vec4(pixelColor.rbg, 1.0);
  }

  else if (treatment == 2)
  {
    mat3 toLuminance = mat3(vec3(0.3, 0.3, 0.3), vec3(0.59, 0.59, 0.59), vec3(0.11, 0.11, 0.11));
    gl_FragColor.rgb = toLuminance * pixelColor.rgb;
  }

  else if (treatment == 3)
  {
    mat3 toLuminance = mat3 (vec3(0.3, 0.3, 0.3), vec3(0.59, 0.59, 0.59), vec3(0.11, 0.11, 0.11));
    vec3 luminance = toLuminance * pixelColor.rgb;
    gl_FragColor.rgb = sepiaColor * luminance;
  }

}
