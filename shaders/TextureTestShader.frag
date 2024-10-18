	#version 330 core
	in vec2 fragUVs;
	out vec4 FragColor;
	uniform sampler2D uTexture;
	void main()
	{
	 	FragColor = texture(uTexture, fragUVs);
	}
