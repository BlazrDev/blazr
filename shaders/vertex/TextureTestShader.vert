
	#version 330 core
	layout (location = 0) in vec3 aPos;
	layout (location = 1) in vec2 aTexCoords;
	out vec2 fragUVs;
	uniform mat4 uProjection;
	void main()
	{
	   gl_Position = uProjection * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	 	fragUVs = aTexCoords;
	}
