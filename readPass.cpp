#include <iostream>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <string>
#include <deque>
#include <cmath>
using namespace std;

void printMatrix(vector <string> matrix);
void removeNode(vector <int> node, vector <vector <int> > &nodes);
bool isNode(vector <int> loc, vector <vector<int> > nodes);
void locSort(vector < vector<int> > &locs);

class line
{
    public:
    line(vector <int> s, int d, int l);
    int getDirection() { return direction;}
    int getLength() { return length;}
    vector <int> getStart() { return start;}
    
    private:
    vector <int> start;
    int direction;
    int length;

};

line::line(vector <int> s, int d, int l)
{
    start = s;
    direction = d;
    length = l;
}

//checks if y, x is out of bounds or not
bool inBounds(int y,int x, vector <string> matrix)
{
    return ((y>=0) && (y<matrix.size()) && (x>=0) && (x<matrix[0].size()));
}

//See the comments above region::region for info on what this class does.
class region
{
    public:
    region(vector <int> loc, vector <string> matrix, char fill);
    int getWidth(){return width;}
    int getHeight(){return height;}
    vector <int> getFirst(){return first;}
    vector <string> getRegion(){return regSpace;}
    
    private:
    void expand(vector <int> loc, vector <string> &matrix, char fill);//fills region to find width/height 
    int width;
    int height;
    vector <int> first;
    vector <string> regSpace;
};

class Node
{
    public:
    Node(vector <int> loc, const vector <string> &matrix);
    ~Node();
    vector <int> getLocation(){return myLoc;}
    vector <Node*> getNext(){return next;}
    void addNext(Node *a){next.push_back(a);}
    void incrPos(){pos++;}
    void incrNeg(){neg++;}
    int getPos(){return pos;}
    int getNeg(){return neg;}
    bool operator==(Node &other);
    bool hasBeenChecked(){return checked;}
    void setChecked(bool a){checked = a;}
    int getNeighbors(){return neighbors;}
    
    private:
    vector<int> myLoc;
    vector<Node*> next;
    int pos;
    int neg;
    int neighbors;
    bool checked;
};

Node::~Node()
{

}
bool Node::operator==(Node &other)
{
    //check to make sure they have the same # neighbors
    int lhNeighbors = this->getNeighbors();
    int rhNeighbors = other.getNeighbors();
    if(lhNeighbors == rhNeighbors)
    {
        return ((this->getPos() == other.getPos()) && (this->getNeg() == other.getNeg()));
    }
    else
    {
        return false;
    }
}

Node::Node(vector <int> loc, const vector <string> &matrix)
{
    myLoc = loc;
    pos = 0;
    neg = 0;
    checked = false;
    neighbors = 0;
    int shift[3] = {-1, 0, 1};
    for(int i=0; i<3; i++)
    {
        for(int j=0; j<3; j++)
        {
            if(!(i==0 && j==0))
            {
                if(inBounds(loc[0]+shift[i], loc[1]+shift[j], matrix) && matrix[loc[0]+shift[i]][loc[1]+shift[j]] == '#')
                {
                    neighbors++;
                }
            }
        }
    }
}

//nodenet. The idea is to use flood-fill to make a tree of nodes
//each node has a link to it's neighboring nodes. a nodenet is just a list of all of those nodes (to make deleting them/memory allocation easier) and also has the first node encountered
class NodeNet
{
    public:
    NodeNet(vector <int> &node, vector <string> matrix, vector <vector <int> > &nodes, int diag = 0);
    void floodFindNodes(vector<int> loc, Node *a, vector <string> &matrix, vector <vector<int> > &nodes);
    Node * getFirst(){return startNode;}
    ~NodeNet();
    vector<Node*> getNodeList(){return nodeList;}
    
    private:
    vector<Node*> nodeList;
    Node *startNode;
    char letter;
};

NodeNet::NodeNet(vector <int> &node, vector <string> matrix, vector <vector <int> > &nodes, int diag)
{
    //make input node an actual node object
    startNode = new Node(node, matrix); 
    nodeList.push_back(startNode);
    //find all nodes on the same character
    floodFindNodes(startNode->getLocation(), startNode, matrix, nodes);
    if(diag)
    {
        for(int i=0; i<nodeList.size(); i++)
        {
            matrix[nodeList[i]->getLocation()[0]][nodeList[i]->getLocation()[1]] = '*';
        }
        printMatrix(matrix);
    }
    //remove all nodes in this character from the main matrix node list
    for(int i=0; i<nodeList.size(); i++)
    {
        removeNode(nodeList[i]->getLocation(), nodes);
        for(int j=0; j<nodeList[i]->getNext().size(); j++)
        {
            Node * next = nodeList[i]->getNext()[j];
            //vector <int> orig = node;
            double directionality = sqrt(static_cast<double>(pow((next->getLocation()[1]), 2) + pow((next->getLocation()[0]), 2)))-sqrt(static_cast<double>(pow((nodeList[i]->getLocation()[1]), 2) + pow((nodeList[i]->getLocation()[0]), 2)));
            if(directionality>0)
            {
                nodeList[i]->incrPos();
            }
            if(directionality<0)
            {
                nodeList[i]->incrNeg();
            } 
        }
    }
    //since letter hasn't been detected yet, set it to 255 as a default "unfound" value
    letter = 255;
}

void NodeNet::floodFindNodes(vector <int> loc, Node *a, vector <string> &matrix, vector <vector<int> > &nodes)
{
    matrix[loc[0]][loc[1]] = 'H';
    int shift[3] = {-1, 0, 1};
    for(int i=0; i<3; i++)
    {
       for(int j=0; j<3; j++)
       {
            if(inBounds(loc[0]+shift[i], loc[1]+shift[j], matrix) && matrix[loc[0]+shift[i]][loc[1]+shift[j]] =='#')
            {   
                vector <int> nLoc(2);
                nLoc[0] = loc[0]+shift[i];
                nLoc[1] = loc[1]+shift[j];
                if(isNode(nLoc, nodes))
                {
                    Node *b = new Node(nLoc, matrix);
                    a->addNext(b);
                    b->addNext(a);
                    nodeList.push_back(b);
                    floodFindNodes(nLoc, b, matrix, nodes);
                }
                else
                {
                    floodFindNodes(nLoc, a, matrix, nodes);
                }
            }
        }
     }
}

NodeNet::~NodeNet()
{
    //uses the sequence (',') operator to chain two commands.
    while(!nodeList.empty()) delete nodeList.back(), nodeList.pop_back();
}


//vector insert function. temporarily didn't have internet and couldn't look up if std already had this as a standard function. 
//Still not sure, but I don't feel like looking it up because this works just as well
void insert(string value, int pos, vector <string> &a)
{
    a.resize(a.size()+1);
    for(int i=pos; i<a.size()-1; i++)
    {
        a[i+1]=a[i];
    }
    a[pos] = value;
}

//region constructor!
//a region is a space of consecutive #'s. to build a region, find any # in the region and call region::expand() on it.
//this is done in the constructor for you. lots of useful things in the region class.
region::region(vector <int> loc, vector <string> matrix, char fill)
{
    first = loc;
    width = 0;
    height = 0;
    //expand to find the region
    expand(loc, matrix, fill);
    string line = "";
    for(int i=first[0]; i!=first[0]+height; i+=(height)/abs(height))
    {
        for(int j=first[1]; j!=first[1]+width; j+=(width)/abs(width))
        {
            //if it was found and marked as "found" by expand function, it will still have value 'H' (don't ask why I chose H)
            if(matrix[i][j] == 'H')
            {
                if(width>0)
                    line.push_back('#');
                if(width<=0)
                {
                    line.insert(0, "#");
                }
            }
            else
            {
                if(width>0)
                    line.push_back('.');
                if(width<=0)
                {
                    line.insert(0, ".");
                }
            }                    
        }
        if(height>0)
            regSpace.push_back(line);
        if(height<=0)
            insert(line, 0, regSpace);
        line = "";
    }
}

//a recursive floodfill algorithm
//http://en.wikipedia.org/wiki/Flood_fill for more info
//in adition, if it encounters a location which is above or to the left of private member first, move first so that it's still in the uppermost left corner of the character. also update new width and height
//to prevent retracing steps, set each encountered squre to 'H'. doesn't matter because matrix was passed by value to the region constructor so we're not changing the main matrix
//IF YOU MAKE THE FILL CHARACTER 'H' THEN THIS WILL INFINILOOP YOU. DON'T DO IT. I EVEN MADE AN IF STATMENT TO NICELY END THE PROGRAM INSTEAD
void region::expand(vector <int> loc, vector <string> &matrix, char fill)
{
    if(fill == 'H')
    {
        cout << "oh come on! I told you NOT to use 'H' as the fill character" << endl;
        cout << "you deserve this program crashing on you!" << endl;
        cout << "see comment right above the region::expand function" << endl;
        exit(1);
    }
    if(loc[0]<first[0])
    {
        height += first[0] - loc[0];
        first[0]=loc[0];
    }
    if(loc[1]<first[1])
    {
        width += first[1] - loc[1];
        first[1] = loc[1];
    }
    if(abs(loc[1]-first[1])+1 > abs(width))
    {
        width = loc[1]-first[1]+1;
    }
    if(abs(loc[0]-first[0])+1 > abs(height))
    {
        height = (loc[0]-first[0])+1;
    }
    //mark this location as found already so we don't retrace our steps and infiniloop
    matrix[loc[0]][loc[1]] = 'H';
    int shift[3] = {-1, 0, 1};
    for(int i=0; i<3; i++)
    {
        for(int j=0; j<3; j++)
        {   
            if(inBounds(loc[0]+shift[i], loc[1]+shift[j], matrix) && matrix[loc[0]+shift[i]][loc[1]+shift[j]] == fill)
            {
                vector <int> nloc(2);
                nloc[0] = loc[0]+shift[i];
                nloc[1] = loc[1]+shift[j];
                expand(nloc, matrix, fill);
            }
        }
    }
}

//sort everything position in locs horizontally (left first)
//this is a selection sort algorithm
void locSort(vector < vector<int> > &locs)
{
    int minIndex;
    vector <int> temp;
    for(int i=0; i<locs.size(); i++)
    {
        minIndex = i;
        for(int j=i; j<locs.size(); j++)
        {
            if(locs[j][1] == locs[minIndex][1])
            {
                if(locs[j][0]<locs[minIndex][0])
                {
                    minIndex = j;
                }
            }
            if(locs[j][1]<locs[minIndex][1])
            {
                minIndex = j;
            }
        }
        temp = locs[i];
        locs[i] = locs[minIndex];
        locs[minIndex] = temp;
    }
}

//returns true if vector <string> matrix is not empty
bool notEmpty(vector <string> matrix)
{
    for(int i=0; i<matrix.size(); i++)
    {
        for(int j=0; j<matrix[0].size(); j++)
        {
            if(matrix[i][j] == '#')
            {
                return true;
            }
        }
    }
    return false;
}

// this function has a model of 0-9 and A-F. returns a model (vector <string>) of char a
// the idea is to generate nodenets for each of these and compare them
// to the nodenets gathered from the input.
// some numbers have alternate ways of writing them. like A. that's what the modelNo param is for. 1 is for A1. 2 is for A2. otherwise it's optional.
vector <string> getChar(char a, int modelNo = 1)
{
    vector <string> one;
    for(int i=0; i<6; i++)
    {
        one.push_back("#");
    }
    vector <string> two;
    two.push_back("......");
    two.push_back(".####.");
    two.push_back("....#.");
    two.push_back(".####.");
    two.push_back(".#....");
    two.push_back(".####.");
    two.push_back("......");
    
    vector <string> three;
    three.push_back(".......");
    three.push_back(".#####.");
    three.push_back(".....#.");
    three.push_back(".#####.");
    three.push_back(".....#.");
    three.push_back(".#####.");
    three.push_back(".......");

    vector <string> four1;
    four1.push_back("......");
    four1.push_back(".#..#.");
    four1.push_back(".#..#.");
    four1.push_back(".####.");
    four1.push_back("....#.");
    four1.push_back("....#.");
    four1.push_back("......");
    
    vector <string> four2;
    four2.push_back("......");
    four2.push_back("....#.");
    four2.push_back("...##.");
    four2.push_back("..#.#.");
    four2.push_back(".####.");
    four2.push_back("....#.");
    four2.push_back("....#.");
    four2.push_back("......");
    
    vector <string> five;
    five.push_back("#####");
    five.push_back("#....");
    five.push_back("#####");
    five.push_back("....#");
    five.push_back("#####");
    
    vector <string> six;
    six.push_back(".####.");
    six.push_back(".#....");
    six.push_back(".#....");
    six.push_back(".####.");
    six.push_back(".#..#.");
    six.push_back(".####.");
    
    vector <string> six2;
    six2.push_back(".####.");
    six2.push_back(".#....");
    six2.push_back(".####.");
    six2.push_back(".#..#.");
    six2.push_back(".####.");
    
    vector <string> seven;
    seven.push_back(".......");
    seven.push_back(".#####.");
    seven.push_back(".....#.");
    seven.push_back(".....#.");
    seven.push_back(".....#.");
    seven.push_back(".....#.");
    seven.push_back(".......");
    
    vector <string> eight;
    eight.push_back(".......");
    eight.push_back(".#####.");
    eight.push_back(".#...#.");
    eight.push_back(".#####.");
    eight.push_back(".#...#.");
    eight.push_back(".#####.");
    eight.push_back(".......");
    
    vector <string> nine;
    nine.push_back("......");
    nine.push_back(".####.");
    nine.push_back(".#..#.");
    nine.push_back(".####.");
    nine.push_back("....#.");
    nine.push_back("....#.");
    nine.push_back("......");
    
    vector <string> zero;
    zero.push_back("......");
    zero.push_back(".####.");
    zero.push_back(".#..#.");
    zero.push_back(".#..#.");
    zero.push_back(".#..#.");
    zero.push_back(".####.");
    zero.push_back("......");
    
    vector <string> a1;
    a1.push_back("...........");
    a1.push_back(".....#.....");
    a1.push_back("....#.#....");
    a1.push_back("...#####...");
    a1.push_back("..#.....#..");
    a1.push_back(".#.......#."); 
    a1.push_back("...........");
    
    
    vector <string> a2;
    a2.push_back(".........");
    a2.push_back("..#####..");
    a2.push_back(".#.....#.");
    a2.push_back(".#.....#.");
    a2.push_back(".#######.");
    a2.push_back(".#.....#.");
    a2.push_back(".#.....#.");
    a2.push_back(".........");

    
    vector <string> a3;
    a3.push_back(".........");
    a3.push_back(".#######.");
    a3.push_back(".#.....#.");
    a3.push_back(".#.....#.");
    a3.push_back(".#######.");
    a3.push_back(".#.....#.");
    a3.push_back(".#.....#.");
    a3.push_back(".........");

    
    vector <string> b;
    
    b.push_back(".######..");
    b.push_back(".#.....#.");
    b.push_back(".#.....#.");
    b.push_back(".######..");
    b.push_back(".#.....#.");
    b.push_back(".#.....#.");
    b.push_back(".######..");
    
    vector <string> c;
    c.push_back(".######.");
    c.push_back(".#......");
    c.push_back(".#......");
    c.push_back(".######.");
    
    vector <string> d;
    d.push_back(".#####..");
    d.push_back(".#....#.");
    d.push_back(".#....#.");
    d.push_back(".#....#.");
    d.push_back(".#....#.");
    d.push_back(".#####..");
    
    vector <string> e;
    e.push_back(".#####.");
    e.push_back(".#.....");
    e.push_back(".#####.");
    e.push_back(".#.....");
    e.push_back(".#####.");
    
    vector <string> f;
    f.push_back(".#####.");
    f.push_back(".#.....");
    f.push_back(".#####.");
    f.push_back(".#.....");
    f.push_back(".#.....");
    
    switch(a)
    {
        case '1':
            return one;
        case '2':
            return two;
        case '3':
            return three;
        case '4':
            if(modelNo == 1)
                return four1;
            if(modelNo == 2)
                return four2;
        case '5':
            return five;
        case '6':
            if(modelNo == 1)
                return six;
            if(modelNo == 2)
                return six2;
        case '7':
            return seven;            
        case '8':
            return eight;            
        case '9':
            return nine;            
        case '0':
            return zero;            
        case 'A':
            if(modelNo == 1)
                return a1;
            if(modelNo == 2)
                return a2;
            if(modelNo == 3)
                return a3;
        case 'B':
            return b;
        case 'C':
            return c;
        case 'D':
            return d;
        case 'E':
            return e;
        case 'F':
            return f;
    }
}

//print out the given vector <string>
void printMatrix(vector <string> matrix)
{
    for(int i=0; i<matrix.size(); i++)
    {
        for(int j=0; j<matrix[0].size(); j++)
        {
            cout << matrix[i][j];
        }
        cout <<endl;
    }
}

//move in dir direction by 1 space
void incr(vector <int> &loc, const int &dir)
{
    if(dir == 0)
    {
        loc[0]--;
        loc[1]--;
    }
    if(dir == 1)
    {   
        loc[0]--;
    }
    if(dir == 2)
    {
        loc[0]--;
        loc[1]++;
    }
    if(dir == 3)
    {
        loc[1]++;
    }
    if(dir == 4)
    {
        loc[1]++;
        loc[0]++;
    }
    if(dir == 5)
    {
        loc[0]++;
    }
    if(dir == 6)
    {
        loc[0]++;
        loc[1]--;
    }
    if(dir == 7)
    {
        loc[1]--;
    }
}

//returns true if loc is on the line
bool isPartOf(const vector <int> &loc, line &l)
{
    vector <int> temp;
    temp = l.getStart();
    for(int k=0; k<l.getLength()-1; k++)
    {
        if(temp[0] == loc[0] && temp[1] == loc[1])
        {
            return true;
        }
        incr(temp, l.getDirection());
    }
    return false;
}

//returns true if loc is not already on a  line in lines
bool notAPartOf(const vector <int> &loc, vector <line> &lines, vector <string> matrix)
{
    for(int i=0; i<lines.size(); i++)
    {
        if(inBounds(loc[0], loc[1], matrix) && isPartOf(loc, lines[i]))
        {
            return false;
        }
    }
    return true;
}

//use line-fitting to find nodes
void findNodes(vector <string> matrix, vector <vector <int> > &nodes)
{
    vector <line> lines;
    //use a line-fitting algorithm
    for(int j=0; j<matrix[0].size(); j++)
    {
        for(int i=0; i<matrix.size(); i++)
        {
            vector <int> loc(2);
            vector <int> tempLoc(2);
            loc[0] = i;
            loc[1] = j;
            if(matrix[i][j] == '#' && notAPartOf(loc, lines, matrix))
            {
                int k=0;
                int y=i, x=j;
                tempLoc = loc;
                incr(tempLoc, 1);
                while(inBounds(y, x, matrix) && matrix[y][x] == '#')
                {
                    k++;
                    y++;
                }
                //if a line has been found (3 or more points in a line). if issues, try changing this to 2
                if(k>=3)
                {
                    while(inBounds(tempLoc[0], tempLoc[1], matrix) && matrix[tempLoc[0]][tempLoc[1]] == '#')
                    {
                        incr(tempLoc, 1);
                        k++;
                    }
                    incr(tempLoc, 5);
                    line temp(tempLoc, 5, k);
                    lines.push_back(temp);

                }
                k=0, y=i, x=j;
                tempLoc = loc;
                incr(tempLoc, 2);
                while(inBounds(y, x, matrix) && matrix[y][x] == '#')
                {
                    k++;
                    y++;
                    x--;
                }
                //if a line has been found (3 or more points in a line). if issues, try changing this to 2
                if(k>=3)
                {
                    while(inBounds(tempLoc[0], tempLoc[1], matrix) && matrix[tempLoc[0]][tempLoc[1]] == '#')
                    {
                        incr(tempLoc, 2);
                        k++;
                    }
                    incr(tempLoc, 6);
                    line temp(tempLoc, 6, k);
                    lines.push_back(temp);
                }
                
                k=0, y=i, x=j;
                tempLoc = loc;
                incr(tempLoc, 0);
                while(inBounds(y, x, matrix) && matrix[y][x] == '#')
                {
                    k++;
                    y++;
                    x++;
                }
                //if a line has been found (3 or more points in a line). if issues, try changing this to 2
                if(k>=3)
                {
                    while(inBounds(tempLoc[0], tempLoc[1], matrix) && matrix[tempLoc[0]][tempLoc[1]] == '#')
                    {
                        incr(tempLoc, 0);
                        k++;
                    }
                    incr(tempLoc, 4);
                    line temp(tempLoc, 4, k);
                    lines.push_back(temp);
                }
                k=0, y=i, x=j;
                tempLoc = loc;
                incr(tempLoc, 7);
                while(inBounds(y, x, matrix) && matrix[y][x] == '#')
                {
                    k++;
                    x++;
                }
                //if a line has been found (3 or more points in a line). if issues, try changing this to 2
                if(k>=3)
                {
                    while(inBounds(tempLoc[0], tempLoc[1], matrix) && matrix[tempLoc[0]][tempLoc[1]] == '#')
                    {
                        incr(tempLoc, 7);
                        k++;
                    }
                    incr(tempLoc, 3);
                    line temp(tempLoc, 3, k);
                    lines.push_back(temp);
                }
                
                k=0, y=i, x=j;
                tempLoc = loc;
                incr(tempLoc, 3);
                while(inBounds(y, x, matrix) && matrix[y][x] == '#')
                {
                    k++;
                    x--;
                }
                //if a line has been found (3 or more points in a line). if issues, try changing this to 2
                if(k>=3)
                {
                    while(inBounds(tempLoc[0], tempLoc[1], matrix) && matrix[tempLoc[0]][tempLoc[1]] == '#')
                    {
                        incr(tempLoc, 3);
                        k++;
                    }
                    incr(tempLoc, 7);
                    line temp(tempLoc, 7, k);
                    lines.push_back(temp);
                }
                
                k=0, y=i, x=j;
                tempLoc = loc;
                incr(tempLoc, 6);
                while(inBounds(y, x, matrix) && matrix[y][x] == '#')
                {
                    k++;
                    y--;
                    x++;
                }
                //if a line has been found (3 or more points in a line). if issues, try changing this to 2
                if(k>=3)
                {
                    while(inBounds(tempLoc[0], tempLoc[1], matrix) && matrix[tempLoc[0]][tempLoc[1]] == '#')
                    {
                        incr(tempLoc, 6);
                        k++;
                    }
                    incr(tempLoc, 2);
                    line temp(tempLoc, 2, k);
                    lines.push_back(temp);
                }                
                
                k=0, y=i, x=j;
                tempLoc = loc;
                incr(tempLoc, 5);
                while(inBounds(y, x, matrix) && matrix[y][x] == '#')
                {
                    k++;
                    y--;
                    
                }
                //if a line has been found (3 or more points in a line). if issues, try changing this to 2
                if(k>=3)
                {
                    while(inBounds(tempLoc[0], tempLoc[1], matrix) && matrix[tempLoc[0]][tempLoc[1]] == '#')
                    {
                        incr(tempLoc, 5);
                        k++;
                    }
                    incr(tempLoc, 1);
                    line temp(tempLoc, 1, k);
                    lines.push_back(temp);
                }
               
                k=0, y=i, x=j;
                tempLoc = loc;
                incr(tempLoc, 4);
                while(inBounds(y, x, matrix) && matrix[y][x] == '#')
                {
                    k++;
                    y--;
                    x--;
                }
                //if a line has been found (3 or more points in a line). if issues, try changing this to 2
                if(k>=3)
                {
                    while(inBounds(tempLoc[0], tempLoc[1], matrix) && matrix[tempLoc[0]][tempLoc[1]] == '#')
                    {
                        incr(tempLoc, 4);
                        k++;
                    }
                    incr(tempLoc, 0);
                    line temp(tempLoc, 0, k);
                    lines.push_back(temp);
                }
            } 
        }
    }
    int shift[3] = {-1, 0, 1};
    //add endpoints of each line as nodes and add neighborcount
    for(int i=0; i<lines.size(); i++)
    {
        vector <int> loc(4);
        loc[0] = lines[i].getStart()[0];
        loc[1] = lines[i].getStart()[1];
        int ncount = 0;
        for(int j=0; j<3; j++)
        {   
            for(int k=0; k<3; k++)
            {
                if(!(j==1 && k==1))
                {
                    if(inBounds(loc[0]+shift[j], loc[1]+shift[k], matrix) && matrix[loc[0]+shift[j]][loc[1]+shift[k]] == '#')
                        ncount++;
                }
            }
        }
        loc[3] = ncount;
        nodes.push_back(loc);
    }
}

void removeNode(vector <int> node, vector <vector <int> > &nodes)
{
    bool failed = true;
    for(int i=0; i<nodes.size(); i++)
    {
        if((nodes[i][0] == node[0]) && (nodes[i][1] == node[1]))
        {
            nodes.erase(nodes.begin()+i);
            failed = false;
        }
    }
    if(failed)
    {
        cout << "removal of node failed" << endl;
    }
}

bool isNode(vector <int> loc, vector <vector<int> > nodes)
{
    for(int i=0; i<nodes.size(); i++)
    {
        if((loc[0] == nodes[i][0]) && (loc[1] == nodes[i][1]))
            return true;
    }
    return false;
}

//this cleans the nodes found in findNodes() up to make them more natural (prevents two neighboring nodes, nodes which shouldn't be there, etc)
void postProcess(vector <vector <int> > &nodes, vector <string> matrix)
{
    double y, x, d;
    char c;
    int shift[3] = {-1, 0, 1};
    //iterate for each node
    for(int i=0; i<nodes.size(); i++)
    {
        //first make a count of # neighbors left, right, above and below the node
        int nCount = 0;
        vector <int> neighbor(4);
        vector <int> oppneighbor(4);
        
        neighbor[0] = nodes[i][0]+1;
        neighbor[1] = nodes[i][1];
        if(isNode(neighbor, nodes))
        {
            nCount++;
        }
        neighbor[0] = nodes[i][0]-1;
        neighbor[1] = nodes[i][1];
        if(isNode(neighbor, nodes))
        {
            nCount++;
        }
        neighbor[0] = nodes[i][0];
        neighbor[1] = nodes[i][1]+1;
        if(isNode(neighbor, nodes))
        {
            nCount++;
        }
        neighbor[0] = nodes[i][0];
        neighbor[1] = nodes[i][1]-1;
        if(isNode(neighbor, nodes))
        {
            nCount++;
        }
        nodes[i][3] = nCount;
        
        //now redo the same count, but don't count linear cases (if the node has one to the left and one to the right, it doesn't count because they make a line so it isn't a node
        //however if there are only 3 or only 4 neighbors from the  previous count, then it's a T or + node and you should count the neighbors anyways
        neighbor[0] = nodes[i][0]+1;
        neighbor[1] = nodes[i][1];
        oppneighbor[0] = nodes[i][0]-1;
        oppneighbor[1] = nodes[i][1];
        if(isNode(neighbor, nodes) && ((!isNode(oppneighbor, nodes)) || (nodes[i][3] == 3) || (nodes[i][3] == 4)))
        {
            nCount++;
        }
        neighbor[0] = nodes[i][0]-1;
        neighbor[1] = nodes[i][1];
        oppneighbor[0] = nodes[i][0]+1;
        oppneighbor[1] = nodes[i][1];
        if(isNode(neighbor, nodes) && ((!isNode(oppneighbor, nodes)) || (nodes[i][3] == 3) || (nodes[i][3] == 4)))
        {
            nCount++;
        }
        neighbor[0] = nodes[i][0];
        neighbor[1] = nodes[i][1]+1;
        oppneighbor[0] = nodes[i][0];
        oppneighbor[1] = nodes[i][1]-1;
        if(isNode(neighbor, nodes) && ((!isNode(oppneighbor, nodes)) || (nodes[i][3] == 3) || (nodes[i][3] == 4)))
        {
            nCount++;
        }
        neighbor[0] = nodes[i][0];
        neighbor[1] = nodes[i][1]-1;
        oppneighbor[0] = nodes[i][0];
        oppneighbor[1] = nodes[i][1]+1;
        if(isNode(neighbor, nodes) && ((!isNode(oppneighbor, nodes)) || (nodes[i][3] == 3) || (nodes[i][3] == 4)))
        {
            nCount++;
        }
        nodes[i][3] = nCount;
    }
    
    for(int i=0; i<nodes.size(); i++)
    {
        y = nodes[i][0];
        x = nodes[i][1];
        c = nodes[i][2];
        vector <vector <int> > neighbors;
        //add all the nodes which are less than 2 units away from the current node to the neighbors vector. this includes the node itself, which is important
        for(int j=0; j<nodes.size(); j++)
        {
            d = sqrt(pow(double(y-nodes[j][0]), 2) + pow(double(x-nodes[j][1]),2));
            if(d<2)
            {
                neighbors.push_back(nodes[j]);
            }
        }
        //find the neighbor with the max number of neighbors, stored from the previous count (see the for loop above)
        int maxindex = 0;
        for(int j=0; j<neighbors.size(); j++)
        {
            if(neighbors[j][3]>neighbors[maxindex][3])
            {
                maxindex = j;
            }
        }
        //delete all neighboring nodes (including the node itself) which are not the neighbor with the max # of neighbors. this is my way of finding which node is "central"
        //in a cluster of nodes and removing everything except the central one
        for(int j=0; j<neighbors.size(); j++)
        {
            if(j!=maxindex)
            {
                removeNode(neighbors[j], nodes);
            }
        }
        
    }
    
    //not sure why, but occaisionally with weird diagonals, two nodes will still be neighboring after the previous cluster-removing step. this removes any remaining doubles (very lazy patch :D)
    for(int i=0; i<nodes.size(); i++)
    {
        y = nodes[i][0];
        x = nodes[i][1];
        c = nodes[i][2];
        vector <vector <int> > neighbors;
        for(int j=0; j<nodes.size(); j++)
        {
            d = sqrt(pow(double(y-nodes[j][0]), 2) + pow(double(x-nodes[j][1]),2));
            if((d<2))
            {
                neighbors.push_back(nodes[j]);
            }
        }
        for(int j=0; j<neighbors.size(); j++)
        {
            if(nodes[i][1] < neighbors[j][1])
            {
                removeNode(neighbors[j], nodes);
            }
        }
        
    }
    //for every node, compute the actual neighbor count (including diagonal #'s,  #'s left, #'s right, #'s above, and #'s below)
    for(int i=0; i<nodes.size(); i++)
    {
        int nCount = 0;
        vector <int> neighbor(4);
        for(int k=0; k<3; k++)
        {
            for(int l=0; l<3; l++)
            {
                if(!((k==1) && (l==1)))
                {
                    if(inBounds(nodes[i][0]+shift[k], nodes[i][1]+shift[l], matrix) && (matrix[nodes[i][0]+shift[k]][nodes[i][1]+shift[l]] != '.'))
                        nCount++;
                }
            }
        }
        nodes[i][3] = nCount;
        //assign the proper character to the node's character code
        switch(nCount)
        {
            case 0:
                nodes[i][2] = '0';
                break;
            case 1:
                nodes[i][2] = '1';
                break;
            case 2:
                nodes[i][2] = '2';
                break;
            case 3:
                nodes[i][2] = '3';
                break;
            case 4:
                nodes[i][2] = '4';
                break;
        }
    }
}

//returns true if deriv and deriv1 have different signs (one is >0, the other is <0. or one == 0 and the other doesn't.
//used for the comparePeak() function right below it.
bool signChange(double deriv, double deriv1)
{
    return ((deriv>0 && deriv1<=0) || (deriv<=0 && deriv1>0) || (deriv==0 && deriv1!=0) || (deriv!=0 && deriv1 == 0));
}

//a function which compares the peak data generated in generatePeak() and finds the peaks (local max values in the graph generated in generatePeak()
bool comparePeak(vector <double> d1, vector <double> d2)
{
    double deriv, deriv1;
    vector <int> peaks;
    vector <int> peaks2;
    //cycle through d1. whenever slope changes sign, you've found a local max
    for(int i=0; i<d1.size()-2; i++)
    {
        deriv = (d1[i+1]-d1[i]);
        deriv1 = (d1[i+2]-d1[i+1]);
        if(signChange(deriv, deriv1))
        {
            cout << "d1:" << i+1 << endl;
            peaks.push_back(i+1);
        }
    }
    //same thing for d2.
    for(int i=0; i<d2.size()-2; i++)
    {
        deriv = (d2[i+1]-d2[i]);
        deriv1 = (d2[i+2]-d2[i+1]);
        if(signChange(deriv, deriv1))
        {
            cout << "d2:" << i+1 << endl;
            peaks2.push_back(i+1);
        }
    }
    
    double d;
    
    //for any two similar characters, if they have a different # of peaks on the graph, they're probably not the same character
    if(peaks.size()!=peaks2.size())
    {
        return false;
    }
    else
    {
        //find the total distance between peaks in peaks and peaks2. if it's pretty small (0.1 in this case), return true; else return false
        for(int i=0; i<peaks.size(); i++)
        {
            d+= abs(peaks[i]-peaks2[i]);
        }
        cout << "d:" << d << endl;
        return d<=0.1;
    }
}

//this is the 360 degree scan technique. if you don't understand what it's doing, ask me to explain
//finds the graph of distance from center to outer portion of letter with respect to degree and stores information in data and i(nner)data
//This function was not used in the end, but I left it for legacy (it might be useful for future coding projects)
void generatePeakData(vector <string> input, vector <double> &data, vector <double> &iData)
{
    //find center + some other initializations
    int y = input.size()/2;
    int x = input[0].size()/2;
    double i=1, j=1;
    bool first = true;
    /*  
     *  the following while loop is repeated 8 times. one for each possible direction
     *  understand one loop, and the rest are the same. So I figured I'd only comment on the first one
     *  each loop starts at the center (y, x) and analyzes each block between the center and the outermost character in one of 8 directions
     *  it saves both the distance to the first and last block encountered. the distance to the first is put in idata (profile of inner part of character)
     *  and the distance for the last is put into data. 
     *
     *  After these 8 loops, the distance profile for both data and idata is normalized, so that scaling doesn't matter
     *  I think this method would work much better if we programmed in more slopes besides 0deg, 45deg, 90deg, etc. but it's hard
     *  because in a grid of text, you can only head 8 directions from the center. also 0 looks more like 8 then a smaller zero (o) does,
     *  and the input characters could be any scale. I think normalizing cancels that out, but I'm not sure.
    /**/
    while((y-i >= 0) && (x-j >= 0))
    {
        if(input[y-i][x-j] == '#')
        {
            data[0] = sqrt(i*i + j*j);
            if(first)
            {
                iData[0] = sqrt(i*i + j*j);
                first = false;
            }
        }
        i++;
        j++;
    }
    i = 1;
    j = 1;
    first = true;
    while((y-i >= 0))
    {
        if(input[y-i][x] == '#')
        {
            data[1] = sqrt(i*i + j*j);
            if(first)
            {
                iData[1] = sqrt(i*i + j*j);
                first = false;
            }
        }
        i++;
    }
    i = 1;
    j = 1;
    first = true;
    while((y-i>=0) && (x+j<input[0].size()))
    {
        if(input[y-i][x+j] == '#')
        {
            data[2] = sqrt(i*i + j*j);
            if(first)
            {
                iData[2] = sqrt(i*i + j*j);
                first = false;
            }
        } 
        i++;
        j++;
    }
    i = 1;
    j = 1;
    first = true;
    while(x-j > 0)
    {
        if(input[y][x-j] == '#')
        {
            data[3] = sqrt(i*i + j*j);
            if(first)
            {
                iData[3] = sqrt(i*i + j*j);
                first = false;
            }
        }
        j++;
    }
    i = 1;
    j = 1;
    first = true;
    while(x+j < input[0].size())
    {
        if(input[y][x+j] == '#')
        {
            data[4] = sqrt(i*i + j*j);
            if(first)
            {
                iData[4] = sqrt(i*i + j*j);
                first = false;
            }
        }
        j++;
    }
    
    i = 1;
    j = 1;
    first = true;
    while((x-j >= 0) && (y+i < input.size()))
    {
        if(input[y+i][x-j] == '#')
        {
            data[5] = sqrt(i*i + j*j);
            if(first)
            {
                iData[5] = sqrt(i*i + j*j);
                first = false;
            }
        }
        j++;
        i++;
    }
    i = 1;
    j = 1;
    first = true;
    while((y+i < input.size()))
    {
        if(input[y+i][x] == '#')
        {
            data[6] = sqrt(i*i + j*j);
            if(first)
            {
                iData[6] = sqrt(i*i + j*j);
                first = false;
            }
        }
        i++;
    }
    i = 1;
    j = 1;
    first = true;
    while((y+i < input.size()) && (x+j < input[0].size()))
    {
        if(input[y+i][x+j] == '#')
        {
            data[7] = sqrt(i*i + j*j);
            if(first)
            {
                iData[7] = sqrt(i*i + j*j);
                first = false;
            }
        }
        i++;
        j++;
    }
    
    //normalize
    //normalizes the numbers in data and idata
    //normaize means divide each value by the maximum value, canceling out scale
    //and making everything on a scale from 0 to 1.
    double max = data[0];
    for(int i=0; i<8; i++)
    {
        if(data[i]>max)
        {
            max = data[i];
        }
    }
    for(int i=0; i<8; i++)
    {
        data[i] /= max;
    }
    
    max = iData[0];
    for(int i=0; i<8; i++)
    {
        if(iData[i]>max)
        {
            max = iData[i];
        }
    }
    for(int i=0; i<8; i++)
    {
        iData[i] /= max;
    }
    
    //yay done!
    
}

//remove the given character at given location from the matrix
void remove(vector<string> character, vector <int> loc, vector <string> &matrix)
{
    for(int i=0; i<character.size(); i++)
    {
        for(int j=0; j<character[0].size(); j++)
        {
            if(character[i][j] == '#')
                matrix[loc[0]+i][loc[1]+j] = '.';
        }
    }
}

//flood fill for comparing nodeNets
/*void floodCountMatching(Node * certain, Node * uncertain, int &count, vector <int> &missingColumns)
{
    certain->hasBeenChecked(true);
    if((*certain) == (*uncertain))
    {
        count++;
        //loop through neighboring nodes
        for(int i=0; i<certain->getNext().size(); i++)
        {
            if(!certain->getNext()[i]->hasBeenChecked())
            {
                floodCountMatching(certain->getNext()[i], uncertain->getNext()[i], count, missingColumns);
            }
        }
    }
    else
    {
        missingColumns.push_back(uncertain->getLocation()[1]);
    }
}/**/
int iabs(int a)
{
    //gcc was giving me issues with the built in abs, and I was tired so I just made my own abs(), integer abs (iabs)
    if(a<0)
    {
        return -a;
    }
    else
    {
        return a;
    }
}
//returns char which is closest to nodenet net
string detect(NodeNet *uncertain, int flag = 0)
{
    //loop through each possible character. repeats mean there's an alternate model. use modelNo=2 in call to getChar function
    //(see getChar function for info on modelNo = 2)
    string a = "AAABCDEF012344566789";
    int maxdex = -1;
    double maxVal = 0;
    double temp = 0;
    double tSize=0;
    double temp2 = 0;
    vector <string> character;
    

    
    for(int i=0; i<a.size(); i++)
    {
        int k = i;
        int modelNo = 1;
        while(k>0 && a[k] == a[k-1])
        {
            k--;
            modelNo++;
        }
        character = getChar(a[i], modelNo);
        
        
        //find nodenet info for character we're comparing NodeNet net[i] to
        vector< vector<int> > knownNodes;
        findNodes(character, knownNodes);
        postProcess(knownNodes, character);
        NodeNet certain(knownNodes[0], character, knownNodes);
        
        
        int count = 0;
        bool matched = false;

        for(int l=0; l<uncertain->getNodeList().size(); l++)
        {
            matched = false;
            for(int j=0; j<certain.getNodeList().size(); j++)
            {

                if(((*(uncertain->getNodeList()[l]))==(*(certain.getNodeList()[j]))) && (!(certain.getNodeList()[j]->hasBeenChecked())) && (!matched))
                {
                    count++;
                    certain.getNodeList()[j]->setChecked(true);
                    matched = true;
                }
                
            }
        }
        temp = static_cast<double>(count)/(uncertain->getNodeList().size());
        temp2 = static_cast<double>(count)/(certain.getNodeList().size());
        //cout << a[i] << " count: " << count << " and certain size:" << certain.getNodeList().size() << " and comp: " << temp << " and t2 " << temp2 << " maxt2 " << tSize << endl;

        if(temp>maxVal)
        {
            maxdex = i;
            tSize = temp2;
            maxVal = temp;
        }
        if((temp == maxVal) && ((temp2 > tSize)))
        {
            maxdex = i;
            tSize = temp2;
            maxVal = temp;
        }
        
    }
       string ans("");
       ans+=a[maxdex];
       return ans;

    /*else
    {
        
        return a[maxdex] + detect();
    }*/
}

void displayNodes(vector <vector <int> > nodes, vector <string> matrix)
{
    for(int i=0; i<nodes.size(); i++)
    {
        if(inBounds(nodes[i][0], nodes[i][1], matrix))
            matrix[nodes[i][0]][nodes[i][1]] = '0'+nodes[i][3];
    }
    for(int i=0; i<matrix.size(); i++)
    {
        for(int j=0; j<matrix[0].size(); j++)
        {
            cout << matrix[i][j];
        }
        cout << endl;
    }
}

void listNodes(const vector <vector <int> > &nodes)
{
    for(int i=0; i<nodes.size(); i++)
    {
        cout << "node: " << nodes[i][0] << " " << nodes[i][1] << endl;
    }
}

string getDoorPass(int row, int col, vector <string> matrix)
{
    //your logic here
    string pass("");
    vector< vector<int> > nodes;
    vector<region> charList;
    vector<NodeNet*> nodeNets;
    findNodes(matrix, nodes);
    postProcess(nodes, matrix);
    NodeNet *tempNet = NULL;
    
    while(nodes.size()>0)
    {
        tempNet = new NodeNet(nodes[0], matrix, nodes);
        nodeNets.push_back(tempNet);
    }
    /*while(notEmpty(matrix))
    {
        for(int j=0; j<matrix[0].size(); j++)
        {
            for(int i=0; i<matrix.size(); i++)
            {
                if(matrix[i][j] == '#')
                {
                    vector <int> loc(2);
                    loc[0]=i;
                    loc[1]=j;
                    region a(loc, matrix, '#');
                    printMatrix(matrix);
                    printMatrix(a.getRegion());
                    remove(a.getRegion(), a.getFirst(), matrix);
                    printMatrix(matrix);
                    charList.push_back(a);
                }
            }
        }
    }*/
    string tempChar;
    stringstream as;
    /*for(int i=0; i<charList.size(); i++)
    {
        vector <vector<int> > nodes;
        findNodes(charList[i].getRegion(), nodes);
        postProcess(nodes, charList[i].getRegion());
        NodeNet net(nodes[0], charList[i].getRegion(), nodes);
        //cout << "char " << i+1 << endl;
        tempChar = detect(&net);
        as << tempChar;
    }*/
    int flag = 0;
    for(int i=0; i<nodeNets.size(); i++)
    {
        if(i == nodeNets.size() - 2)
        {
            //flag = 1;
        }
        tempChar = detect(nodeNets[i], flag);
        as << tempChar;
        //cout << tempChar << endl;
        if(i == nodeNets.size() - 2)
        {
            flag = 0;
        }
    }

    pass = as.str();
    //cout << "done detecting" << endl;
    while(!nodeNets.empty()) delete nodeNets.back(), nodeNets.back() = 0, nodeNets.pop_back();
    return pass;
    //see main function. testing functions there. when everything works, it will be moved up here (and non password output will be deleted)
}

//finds all 2x2 blocks of #'s and removes them
void clean(vector <string> matrix)
{
    
    for(int i=0; i<matrix.size(); i++)
    {
        for(int j=0; j<matrix[0].size(); j++)
        {   
            bool square = true;
            for(int l=0; l<2; l++)
            {
                for(int k=0; k<2; k++)
                {
                    if(inBounds(i+l, j+k, matrix) && !(matrix[i+l][j+k] == '#'))
                    {
                        square = false;
                    }
                }
            }
            if(square)
            {
                for(int l=0; l<matrix.size(); l++)
                {
                    matrix[l].push_back('.');
                }
                for(int k=0; k<matrix.size(); k++)
                {
                    for(int l=matrix[0].size()-1; l>i+2; l--)
                    {
                        matrix[k][l] = matrix[k][l-1];
                    }
                }
                for(int l=0; l<matrix.size(); l++)
                {
                    matrix[i+1][l] = '.';
                }
            }
        }
    }
}

int main() 
{
    int row,col;
    vector <string> matrix;

    //Read the rows and colums of character matrix
    cin >> row;
    cin >> col;
   
    //Read the board now.
    for(int i=0; i<row; i++) {
        string s; cin >> s;
        matrix.push_back(s);
    }

    vector <vector <int> > nodes;
    findNodes(matrix, nodes);
    postProcess(nodes, matrix);
    //displayNodes(nodes, matrix);
    
    string answer = getDoorPass(row,col,matrix);
    cout << answer << endl;
    
    return 0;
}

/*

         /\_/\
        / O O \
        \_=O=_/   /\
         /    \  / /  
        | _   _|| |
         \o|_|o/_/  
  
*/
