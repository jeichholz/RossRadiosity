# RossRadiosity
 A simple radiosity engine; some framework code and a description

## Use
Take a look at the file RadiosityInstructions.pdf, which has a pretty full description of what things are, what you need to do in order to complete the engine, and suggestions for community improvements. 

## Contents

    * **.m Files** -- basic MATLAB and Octave utility files.  
    * **.c Files** -- the only .c file is computeVisibility.c, which is a much faster version of visibility computing; suitable for those who know how to compile code on their platform. 
    * **MiscAssets** -- miscellaneous assets that you can use to create a custom scene using, for example, 3D-Builder.  These contain no lighting information; you'll have to add lights yourself.  See the informational pdf. 
    * **ScenesAndVisibility** -- these .obj files represent complete scenes; they include lighting already.  The file visTestScene is pretty small, around 1100 faces, and is a good first test.  The other scenes are larger and should be put off until visTestScene seems like it is doing something reasonable. I've included the visibility files to go along with these.  They are stored in .mat format suitable for Matlab and Octave; if anyone is interested in a text version of those files suitable for other languages please email me or open an issue.  The point of including the visibility data for you is to let you avoid computing it -- on the larger scenes anything other than the .c code is not practical to run. 