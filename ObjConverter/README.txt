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