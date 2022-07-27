
# Reads in a .OBJ file from the given filename.
# Returns a tuple (V, F, C) of:
#    V: a list of vertices, each as a 3-tuple
#    F: a list of faces, each as a 3-tuple of vertex indices
#    C: a list of colors, each as a 3-tuple of RGB
#    E: a list of light colors, each as a 3-tuple of RGB
def readObjFile(filename):
    f = open(filename, 'r', encoding='utf-8')
    V = []
    F = []
    C = []
    E = []
    vColors = []
    colors = {}
    currentColor = None
    currentPower = 0
    for line in f:
        words = line.split()
        if (len(words) == 0):
            continue
        if (words[0] == "v"):
            V.append((float(words[1]), float(words[2]), float(words[3])))
            # Were vertex colors specified?
            if (len(words) > 4):
                vColors.append((float(words[4]), float(words[5]), float(words[6])))
            else:
                vColors.append(None)
        elif (words[0] == "f"):
            # Might be using texture/normal indices; discard those, wrap around negatives
            for i in range(1, len(words)):
                words[i] = int(words[i].split("/")[0]) - 1
                if (words[i] < 0):
                    words[i] = words[i] + len(V)
            # Experimental: breaks polygons into triangles
            for i in range(2,(len(words)-1)):
                F.append((words[1], words[i], words[i+1]))
                if (currentColor == None):
                    C.append(meanColor([vColors[words[1]], vColors[words[2]], vColors[words[3]]]))
                else:
                    C.append(currentColor)
                E.append(multColor(C[-1], currentPower))
        elif (words[0] == "mtllib"):
            colors.update(readMtlFile(words[1]))
        elif (words[0] == "usemtl"):
            if (words[1] in colors):
                currentColor = colors[words[1]]
            else:
                currentColor = None
        elif (words[0] == "o"):
            currentPower = 0
        elif (words[0] == "#light"):
            currentPower = float(words[1])
    f.close()
    return (V, F, C, E)

# Writes out a .OBJ file with the given scene data.
# All input arguments expect the same format as readObjData returns.
# Input arguments:
#     filename: the filename to write to
#     V: A list of vertices in the scene
#     F: A list of triangles in the scene
#     C: A list of colors of those triangles
def writeObjFile(filename, V, F, C):
    f = open(filename, 'w', encoding='utf-8')
    vColors = computeVertexColors(len(V), F, C)
    for i in range(len(V)):
        f.write(f'v {V[i][0]} {V[i][1]} {V[i][2]} {vColors[i][0]} {vColors[i][1]} {vColors[i][2]}\n')
    for face in F:
        f.write(f'f {face[0] + 1} {face[1] + 1} {face[2] + 1}\n')

# Internal use
# Returns a dictionary of material names -> colors
def readMtlFile(filename):
    f = open(filename, 'r', encoding='utf-8')
    colors = {}
    currentMtl = ""
    Ka = (0, 0, 0)
    Kd = (0, 0, 0)
    for line in f:
        words = line.split()
        if (len(words) == 0):
            continue
        if (words[0] == "newmtl"):
            if (currentMtl != ""):
                colors[currentMtl] = (Ka[0] + Kd[0], Ka[1] + Kd[1], Ka[2] + Kd[2])
            currentMtl = words[1]
        elif (words[0] == "Ka"):
            Ka = (float(words[1]), float(words[2]), float(words[3]))
        elif (words[0] == "Kd"):
            Kd = (float(words[1]), float(words[2]), float(words[3]))
    if (currentMtl != ""):
        colors[currentMtl] = (Ka[0] + Kd[0], Ka[1] + Kd[1], Ka[2] + Kd[2])
    f.close()
    return colors

# Internal use: averages a list of colors
def meanColor(colors):
    totals = [0,0,0]
    for c in colors:
        for i in range(3):
            totals[i] += c[i]
    return (totals[0] / len(totals), totals[1] / len(totals), totals[2] / len(totals))

# Internal use: scales a color by some factor
def multColor(color, factor):
    return (color[0] * factor, color[1] * factor, color[2] * factor);

# Internal use: computes the vertex colors, averaged over each face they are part of
def computeVertexColors(numVertices, F, C):
    totals = [[0 for col in range(4)] for row in range(numVertices)]
    for i in range(len(F)):
        for j in range(3):
            totals[F[i][j]][0] += C[i][0]
            totals[F[i][j]][1] += C[i][1]
            totals[F[i][j]][2] += C[i][2]
            totals[F[i][j]][3] += 1
    vColors = []
    for i in range(numVertices):
        if (totals[i][3] > 0):
            for j in range(3):
                totals[i][j] /= totals[i][3]
        vColors.append((totals[i][0], totals[i][1], totals[i][2]))
    return vColors

# Internal use: for testing the pipeline
def copyObjFile(filename, copyTo):
    x = readObjFile(filename)
    writeObjFile(copyTo, x[0], x[1], x[2])