#version 330

// A VERY SIMPLE FRAGMENT SHADER...
uniform sampler2D basetex;
uniform sampler2D suptex;
uniform int visuMode;
uniform vec2 mouseCoord;

in vec2 owncoordinates;
void main()
{
	gl_FragColor = vec4(1.0,0.0,0.0,0.0);

	if (visuMode<3) {
		gl_FragColor.rgba = texture2D(basetex,owncoordinates.xy);
	}
	if (visuMode >= 3) {
		if (owncoordinates.x <= mouseCoord.x - 0.002) {
			gl_FragColor = texture2D(basetex,owncoordinates.xy);
		}
		else {
			if (owncoordinates.x >= mouseCoord.x + 0.002) {
				gl_FragColor = texture2D(suptex,owncoordinates.xy);
			}
			else {
				gl_FragColor = vec4(0.0,0.0,1.0,0.0);
			}
		}
	}
}
