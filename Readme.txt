Kung Fu MXR (Mr. X's Revenge)
-----------------------------
v.1.0 Summer 2020
Designed and Coded by Fensox



Future Updates/Optimizations:
	Sprite.h/.cpp
		- Function loadDataFile() is quite lengthy and hard to follow. Create a helper function that handles the reading of the file from the filesystem and puts it into a std::vector of std::strings or something and returns that to our loadDataFile() function. That would clean up the complexity of that code substantially. Any other helper functions to pull apart each line of the metadata file and get that code outside of loadDataFile would help.