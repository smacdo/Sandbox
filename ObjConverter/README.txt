NOTES
======================
This should (eventually) be an asset processor tool with a powershell front end.

Command line mode
===================
	./assettool.exe <action> [params] [outputfile]

actions:
	* convert
		* -type: asset source type
			* model
			* texture
		* -file: path/to/input/file.type
	* compile
		* -type: shader
		* -file: path/to/shader


Material Format (binary)
=============================================================================
file
{
	header
	material
}

header 
{
	char[6] magic    = "SMTL\n\0"
	byte version     = 1
}

material
{
	string name;
	vec3 ambient_color;
	vec3 diffuse_color;
	vec3 specular_color;
	vec3 specular_power;
	string ambient_texture;
	string diffuse_texture;
	string specular_texture;
	byte render_mode [shaded, solid, wireframe]
	byte filtering [point, linear, anisotropic]
	byte u_address_mode [wrap, mirror, mirror_once, clamp, border]
	byte v_address_mode [wrap, mirror, mirror_once, clamp, border]
	byte max_anisotropy
	byte compare_func [never, always, equal, not_equal, less, less_equal, greater, greater_equal]
	byte min_lod
	byte max_lod
}