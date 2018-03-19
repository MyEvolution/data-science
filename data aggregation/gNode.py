import copy;
class graph:


    def __init__(self,filename):#类变量和成员变量区分清楚！
        self.theMatrix = [];  # the Matrix we will read from a file
        self.theNodes = {};
        self.edgeCount = 0;
        self.communities = [];
        self.communitiesId = {};
        self.theEMatrix = [];
        self.nodeCount = 0;
        self.aList = [];
        self.Q = 0;
        with open(filename,"r") as f:

            readContent = f.readline().strip();
            theStack = [];  # a stack used to extract the content of the graph's node and edge
            sign = 0;  # this sign told us what we are reading,a node ? or a edge
            if  readContent.find("Creator")!=-1:
                f.readline();# skip the graph
                readContent = f.readline().strip() ;
                while readContent!="" :
                    if(readContent == "["):
                       theStack.append(readContent);
                    elif(readContent == "]"):
                        if theStack[-1] == "[":
                           theStack = theStack[:-1];#pop the symboy ,a cell has been read
                        else:
                            raise Exception("the backet is not a pair!");
                    elif(readContent == "node"):
                        sign = 0;
                    elif(readContent == "edge"):
                        sign = 1;
                    elif(sign == 0):
                        id = int(readContent.split(' ')[1]);

                        if not id in self.theNodes:
                            self.theNodes[id] = self.nodeCount;
                            self.communities.append([id]);
                            self.communitiesId[self.nodeCount] = self.nodeCount;
                            self.nodeCount+=1;

                            for i in self.theMatrix:#every time we expand the graph this Matrix will expand a circle
                                i.append(0);
                            if len(self.theMatrix ) == 0:
                                self.theMatrix.append([0]);
                            else:
                                self.theMatrix.append(copy.deepcopy(self.theMatrix[0]));
                        else:
                             raise Exception("the id is duplicate:"+id);
                    else:
                        start = int(readContent.split(" ")[1]);
                        readContent = f.readline().strip();
                        end = int(readContent.split(" ")[1]);

                        if start in self.theNodes and end in self.theNodes:
                            #print(self.theNodes[start],self.theNodes[end]);
                            self.theMatrix[self.theNodes[start]][self.theNodes[end]] = 1;
                            self.theMatrix[self.theNodes[end]][self.theNodes[start]] = 1;
                            self.edgeCount+=1;
                        else:
                            raise Exception("the edge' pokes has not been read !");
                    readContent = f.readline().strip();
                if len(theStack) != 0:
                    raise Exception("unknow error:something interrupt the data reading!");
            else:# newman 's  virtual datas

                while readContent!="":
                    ids = readContent.split("\t");
                    ids[0] = int(ids[0]);
                    ids[1] = int(ids[1]);
                    if not ids[0] in self.theNodes:
                        self.theNodes[ids[0]] = self.nodeCount;

                        if len(self.theMatrix) == 0:
                            self.theMatrix.append([0]);
                        else:
                            for i in self.theMatrix:
                                i.append(0);
                            self.theMatrix.append(copy.deepcopy(self.theMatrix[0]));
                        self.communities.append([ids[0]])
                        self.communitiesId[self.nodeCount] = self.nodeCount;
                        self.nodeCount += 1;

                    if not ids[1] in self.theNodes:
                        self.theNodes[ids[1]] = self.nodeCount;

                        for i in self.theMatrix:
                            i.append(0);
                        self.theMatrix.append(copy.deepcopy(self.theMatrix[0]));
                        self.communities.append([ids[1]]);
                        self.communitiesId[self.nodeCount] = self.nodeCount;
                        self.nodeCount += 1;
                    self.theMatrix[self.theNodes[ids[0]]][self.theNodes[ids[1]]] = 1;
                    self.theMatrix[self.theNodes[ids[1]]][self.theNodes[ids[0]]] = 1;
                    self.edgeCount+=1;
                    readContent = f.readline();

            self.theEMatrix = copy.deepcopy(self.theMatrix);
            self.Q = self.getQ(0, 0);

    def get_first_E(self,i,j):
        eij = 0;
        for node1 in self.communities[i]:
            for node2 in self.communities[j]:
                if self.theMatrix[self.theNodes[node1]][self.theNodes[node2]]==1:
                    eij+=1;
        #print("eij",i,j,eij);
        return eij;
    def getE(self,i,j,isSelf = False):
        big = self.communitiesId[max(i,j)];
        small = self.communitiesId[min(i,j)];
        if not isSelf:
            theEMatrix = copy.deepcopy(self.theEMatrix);
        else:
            theEMatrix = self.theEMatrix;

        for x,y in self.communitiesId.items():

            theEMatrix[small][y]+=theEMatrix[big][y];
            theEMatrix[y][small] += theEMatrix[y][big];
        theEMatrix[small][small]+=theEMatrix[big][big];

        return theEMatrix;
    def getA(self,i,x,y,theEM):
        aij =0;
        if x == y:
            p = self.communitiesId[i];
            for num,pos in self.communitiesId.items():
                #print(theEM);
                #print(p,pos);
                aij += theEM[p][pos];
            return aij;
        big = max(x,y);
        p = self.communitiesId[i];
        for num,pos in self.communitiesId.items():
            if num!=big:
                aij+= theEM[p][pos];
        return aij;
    def getQ(self,x=0,y=0):
        Q = 0;
        if x == 0 and y == 0:#first to compute
            for i in self.communitiesId:
                #print(self.theEMatrix[self.communitiesId[i]][self.communitiesId[i]], self.getA(i, x, y, self.theEMatrix));
                Q += self.theEMatrix[self.communitiesId[i]][self.communitiesId[i]] / (2 * self.edgeCount);
                # print(self.getA(i,communities));
                a = self.getA(i, x, y, self.theEMatrix) / (2 * self.edgeCount);
                self.aList.append(a);
                Q -= pow(a, 2);
            return Q;
        x = self.communitiesId[x];
        y = self.communitiesId[y];
        Q = self.Q+(self.theEMatrix[x][y]+self.theEMatrix[y][x])/(2*self.edgeCount) - 2* self.aList[x]*self.aList[y];
        return Q;
    def merge(self,i,j):
        #print("call merge!");
        self.aList[self.communitiesId[min(i, j)]] += self.aList[self.communitiesId[max(i, j)]];
        self.getE(i, j, True);
        if i>j:
            self.communities[self.communitiesId[j]].extend(self.communities[self.communitiesId[i]]);
        elif i<j:
            self.communities[self.communitiesId[i]].extend(self.communities[self.communitiesId[j]]);
        self.communities[self.communitiesId[max(i,j)]] = [];
        del self.communitiesId[max(i,j)];

        #if i!=j:
         #   del communities[max(i,j)];
    def printEM(self):
        for i,j in self.communitiesId.items():
            for p in range(10):
                if p in self.communitiesId:
                    print(self.theEMatrix[j][p],end=" ");
            print();
    def realmerge(self):
        ready = [];
        #print(self.aList);
        if self.communities == 1:
            print("only one community left!");
            return;
        cids = [v[0] for v in self.communitiesId.items()];
        oldQ = -0.5;
        for i in range(len(cids)):
            for j in range(i+1,len(cids)):
                newQ = self.getQ(cids[i],cids[j]);

                if newQ>oldQ:
                    ready = [newQ,cids[i],cids[j]];
                    oldQ = newQ;
        print("ready!");
        #ready.sort(reverse=True);
        print("this time we are going to merge community"+str(ready[1])+" and community"+str(ready[2])+",\n after this mergement,the Q changes to "+str(ready[0]));
        self.merge(ready[1],ready[2]);
        self.Q = ready[0];
    def getSocieties(self,cnum):
        while len(self.communitiesId)>cnum:
            self.realmerge();
        #print(self.getQ(0,0));
    def __str__(self):
        res = "  ";
        for i,j in self.theNodes:
            res+=str(j);
        res+="\n";
        for i in self.theMatrix:
            for j in i:
                res+=str(j)+" ";
            res+="\n";
        return res;
    def printCommunity(self):
        for i in self.communitiesId:
            print(self.communities[self.communitiesId[i]]);
if __name__ == "__main__":
    g = graph("C:/Users/无聊时的自娱自乐/Desktop/Newman128/newmans/newman10.txt");
    g.getSocieties(4);
    g.printCommunity();
    '''ids = list(g.theNodes.items());
    ids.sort();
    with open("./karateEdges.txt","w") as f:
        for i,j in ids:
            for x,y in ids:
                if x>i and g.theMatrix[j][y] == 1:
                    f.write(str(i)+"\t"+str(x)+"\n");
    nodeFrom = {};
    keyId = {};
    id = 0;
    for key in g.communitiesId:
        keyId[key] = id;
        id += 1;
    for key, value in g.communitiesId.items():
        for node in g.communities[value]:
            nodeFrom[node] = keyId[key];
    # print(len(nodeFrom));
    abc = [[v[0], v[1]] for v in nodeFrom.items()];
    abc.sort();
    with open("./karateAttributes.txt", "w") as f:
        for key, value in abc:
            f.write(str(key) + "\t" + str(value) + "\n");'''

    '''g = [];
    for i in range(1,11):
        g .append( graph("C:/Users/无聊时的自娱自乐/Desktop/Newman128/newmans/newman"+str(i)+".txt"));
    for i in range(0,10):
    #g = graph("C:/Users/无聊时的自娱自乐/Desktop/Newman128/newmans/newman5.txt");
        print("edge:",g[i].edgeCount,"node:",g[i].nodeCount);

        print("Q:",g[i].Q);
    #print("EM:");
    #g.printEM();
        g[i].getSocieties(4);
        nodeFrom = {};
        keyId = {};
        id = 0;
        for key in g[i].communitiesId:
            keyId[key] = id;
            id+=1;
        for key,value in g[i].communitiesId.items():
            for node in g[i].communities[value]:
                nodeFrom[node] = keyId[key];
        #print(len(nodeFrom));
        abc = [[v[0],v[1]] for v in nodeFrom.items()];
        abc.sort();
        with open("./community"+str(i+1)+".txt","w") as f:
            for key,value in abc:
                f.write(str(key)+"\t"+str(value)+"\n");
    g[4].printCommunity();'''

