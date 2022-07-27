function writeObjFile(filename,vertexList,faceList,colorList,linearShading)
%function writeObjFile(filename,vertexList,faceList,colorList,linearShading)
%Writes an obj file.
%
%INPUT -
%filename - the name of the file to write.
%vertexList - list of vertices in the scene; N x 3.
%faceList - list of faces in the scene; M x 3.
%colorList - the color (r,g,b) of each face in the scene. M x 3.
%linearShading - an optional argument, with default value 1.  If true, then each vertice is assigned a color by averaging the
%                color of the surrounding faces.  A per-vertex color is written.  Renderers will tend to give smoother results with this turned on.
    [fid,mesg]=fopen(filename,'w');
    if fid<0
        fprintf(2,'Error opening %s for writing: %s\n',filename,mesg);
        return;
    end

    if ~exist('linearShading','var') || isempty(linearShading)
        linearShading=1;
    end

    if ~exist('colorList','var')
        colorList=repmat([249 246 238],size(faceList,1),1);
        linearShading=1;
    end


    if linearShading  %If doing linear shading then we need one color per vertex.  No need to repeat vertices or anything like that
        for i=1:size(vertexList,1)
            relevantTriangles=find(any(faceList==i,2));
            avgColor=mean(colorList(relevantTriangles,:),1);
            fprintf(fid,'v %f %f %f %f %f %f\n',vertexList(i,:),avgColor);
        end

        for i=1:size(faceList,1)
            fprintf(fid,'f %d %d %d\n',faceList(i,:));
        end
    else %in this case, each triangle gets its own copy of each vertex, and all three vertices for a given triangle will share the same color.
        for i=1:size(faceList,1)
            for j=1:3
                fprintf(fid,'v %f %f %f %f %f %f\n',vertexList(faceList(i,j),:),colorList(i,:));
            end
        end

        for i=1:size(faceList,1)
            fprintf(fid,'f %d %d %d\n',3*(i-1)+1,3*(i-1)+2, 3*(i-1)+3);
        end
    end
end




