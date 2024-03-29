uniform vec3 lightDir;
in vec4 normal;

void main()
{
	float intensity;
	vec4 color;

	intensity = dot(lightDir, normal);

	if (intensity > 0.66)
		color = vec4(0.5,1.0,0.5,1.0);
	else
		color = vec4(0.5*0.2,1.0*0.2,0.5*0.2,1.0);
	gl_FragColor = color;
}