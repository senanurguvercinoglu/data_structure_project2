/*
Omercan Goktas 150119671
Senanur Guvercinoglu 150119740
Berkan Korkmaz 150119623
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define MAX_SIZE 256
#define CHAR_SIZE 10

/*---------------------------------------------------------------------------------------*/

/*those structures are for representing graph*/
/*this struct contains vertices and their weight*/
struct socialNetListNode
{
	int destination, weight;
	struct socialNetListNode *next;
};

/*this struct represents the upper struct*/
struct socialNetList
{
	struct socialNetListNode *header;
};

/*represents the graph and contains maxSize as how many vertices there are*/
struct graph
{
	int maxSize;
	struct socialNetList *arrayList;
};

struct vertex
{
	int source;
	struct socialNetListNode *destList;
	struct vertex *next;
};

/*---------------------------------------------------------------------------------------*/

/*those structures are for minheaps*/
/*this struct represents minHeap node*/
struct minHeapNode
{
	int maxSize, destination;
};

/*this structure represent min heap*/
struct minHeap
{
	int size; /*size represents how many nodes there are as vertices*/	
	int capacity; /*capacity represents minHeap's capacity*/
	int *position; /*position is going to be using at reduce() function as determinig position of minheap*/
	struct minHeapNode **arrayList;
};

/*---------------------------------------------------------------------------------------*/

struct vertex *vertexList = NULL;
struct vertex *undirectedVertex = NULL;

/*---------------------------------------------------------------------------------------*/

bool isFileRead = false;

/*---------------------------------------------------------------------------------------*/

struct socialNetListNode *newsocialNetListNode(int destination, int weight);
struct graph *newGraph(int size);
struct minHeap* newMinHeap(int capacity);
void addVertex(struct graph *graph, int source, int destination, int weight);
struct minHeapNode* newMinHeapNode(int maxSize, int destination);
void changeMinHeap(struct minHeapNode** firstHeapNode, struct minHeapNode** secondHeapNode);
void heapifyingMinHeap(struct minHeap* minHeap, int index);
int isInMinHeap(struct minHeap *minHeap, int vertice);
int isEmpty(struct minHeap* minHeap);
struct minHeapNode *getMin(struct minHeap* minHeap);
void reduce(struct minHeap* minHeap, int maxSize, int destination);
void algorithmDijkstra(struct graph *graph, int source);
void createAdjList(int source, int destination, int weight);
void unorganizedAdjList(int source, int destination, int weight);
void displayAdjList();

/*---------------------------------------------------------------------------------------*/

/*this function is using to add a new vertice*/
struct socialNetListNode *newsocialNetListNode(int destination, int weight)
{
	/*allocating a place from memory for new vertice*/
	struct socialNetListNode *newNode = (struct socialNetListNode*) malloc(sizeof(struct socialNetListNode));
	/*adding values to current vertice*/
	newNode->destination = destination;
	newNode->weight = weight;
	newNode->next = NULL;
	return newNode;
}

/*creating a new graph according to number of vertices*/
struct graph *newGraph(int size)
{
	/*allocating a place from memory for new graph*/
	struct graph *graph = (struct graph *) malloc(sizeof(struct graph));
	graph->maxSize = size;

	/*allocating a place according to size of vertices*/
	graph->arrayList = (struct socialNetList *) malloc(sizeof(struct socialNetList) * size);

	/*allocated places for arrayList will be initializing as value NULL*/
    int i;
	for (i = 0; i < size; ++i)
	{
		graph->arrayList[i].header = NULL;
	}

	return graph;
}

/*this function is using to create min heap*/
struct minHeap* newMinHeap(int capacity)
{
	/*allocating memory for min heap according to capacity*/
	struct minHeap* minHeap = (struct minHeap*) malloc(sizeof(struct minHeap));
	/*allocating one int place from memory for position which is gonna be using for reduce() function later*/
	minHeap->position = (int *)malloc(capacity * sizeof(int));
	/*signing size as zero becuase there is no any value yet*/
	minHeap->size = 0;
	minHeap->capacity = capacity;
	/*allocating memory for arrayList which is in min heap*/
	minHeap->arrayList = (struct minHeapNode**) malloc(capacity * sizeof(struct minHeapNode*));
	return minHeap;
}

/*this function adds a new vertice to current graph*/
void addVertex(struct graph *graph, int source, int destination, int weight)
{
	source = source - 65;
	destination = destination - 65;
	/*new vertices are sending newsocialNetListNode to add nodes*/
	struct socialNetListNode *newNode = newsocialNetListNode(destination, weight);
	/*adding vertices to node*/
	newNode->next = graph->arrayList[source].header;
	graph->arrayList[source].header = newNode;

	/*because graph is not organized yet, edge is gonna be adding to source*/
	newNode = newsocialNetListNode(source, weight);
	newNode->next = graph->arrayList[destination].header;
	graph->arrayList[destination].header = newNode;
}

/*this function creates min heap node*/
struct minHeapNode* newMinHeapNode(int maxSize, int destination)
{
	/*allocating memory for minheapnode*/
	struct minHeapNode* minHeapNode = (struct minHeapNode*) malloc(sizeof(struct minHeapNode));

	/*assigning values to values of node*/
	minHeapNode->maxSize = maxSize;
	minHeapNode->destination = destination;
	return minHeapNode;
}

/*---------------------------------------------------------------------------------------*/

/*this function changes to values of min heap node for heapifying*/
void changeMinHeap(struct minHeapNode** firstHeapNode, struct minHeapNode** secondHeapNode)
{
	/*holding the value after changing first value, then it gets its value from this tempminheapnode*/
	struct minHeapNode* tempMinHeapNode = *firstHeapNode;
	*firstHeapNode = *secondHeapNode;
	*secondHeapNode = tempMinHeapNode;
}

/*unorganized min heap is organizing with this function (heapifying)*/
void heapifyingMinHeap(struct minHeap* minHeap, int index)
{
	int smallest, leftNode, rightNode;
	smallest = index;

	/*due to that we didn't start from index 1, 1 should be adding to left node after multiplication
	also positionition of right node is 2*index+1 but in this situation also 1 should be adding to here*/
	leftNode = 1 + (index * 2);
	rightNode = (index * 2) + 2;

	/*checking the condition if the positionition of current node smaller than smallest node destination*/
	if ((minHeap->arrayList[leftNode]->destination < minHeap->arrayList[smallest]->destination) && (leftNode < minHeap->size))
	{
		/*assigning smallest node as left node because left node's positionition is smaller*/
		smallest = leftNode;
	}

	/*checking if positionition of the current node is smaller than smallest node destination*/
	if ((minHeap->arrayList[rightNode]->destination < minHeap->arrayList[smallest]->destination) && (rightNode < minHeap->size))
	{
		/*assigning smallest node as right node because positionition of right node is smaller*/
		smallest = rightNode;
	}

	/*because min heap is not organized completly, it should be heapifying again*/
	if (smallest != index)
	{
		/*those nodes will be using to change*/
		struct minHeapNode *smallestNode = minHeap->arrayList[smallest];
		struct minHeapNode *indexNode = minHeap->arrayList[index];

		/*changing the positionition of node*/
		minHeap->position[smallestNode->maxSize] = index;
		minHeap->position[indexNode->maxSize] = smallest;
		
		/*with using function changeMinHeap(), positionitions change then heapifyingMinHeap is calling again until heapifying is not required*/
		changeMinHeap(&minHeap->arrayList[smallest], &minHeap->arrayList[index]);
		heapifyingMinHeap(minHeap, smallest);
	}
}

/*checking if vertice is in min heap*/
int isInMinHeap(struct minHeap *minHeap, int vertice)
{
	/*returning 1 because given vertice is in min heap*/
	if (minHeap->position[vertice] < minHeap->size)
		return 1;
	/*returning zero because given vertice is not in min heap*/
	return 0;
}

/*checking if given min heap is empty or not*/
int isEmpty(struct minHeap* minHeap)
{
	/*returning one(true value) because min heap size is empty, means there is nothing*/
	if(minHeap->size == 0)
	{
		return 1;
	}
	/*returning zero(false value) because min heap is not empty*/
	return 0;
}

/*getting the minimum node from min heap*/
struct minHeapNode *getMin(struct minHeap* minHeap)
{
	/*first of all, it must be checking if min heap is empty or not. if it is empty then value NULL must be returning*/
	if (isEmpty(minHeap))
		return NULL;

	/*assigning the root of node to minheap *root*/
	struct minHeapNode *root = minHeap->arrayList[0];

	/*and here, root and the last node change their positionition*/
	struct minHeapNode *lastNode = minHeap->arrayList[minHeap->size - 1];
	/*assigning root to last node*/
	minHeap->arrayList[0] = lastNode;

	/*updating last node's positionition*/
	minHeap->position[root->maxSize] = minHeap->size-1;
	minHeap->position[lastNode->maxSize] = 0;

	/*decreasing the size of min heap*/
	--minHeap->size;
	/*then min heap should be heapifiying*/
	heapifyingMinHeap(minHeap, 0);

	return root;
}

/*this function reduces destination of vertice*/
void reduce(struct minHeap* minHeap, int maxSize, int destination)
{
	/*getting the index of the min heap*/
	int size = minHeap->position[maxSize];

	/*updating min heap destination with the given destination*/
	minHeap->arrayList[size]->destination = destination;

	while (size && minHeap->arrayList[size]->destination < minHeap->arrayList[(size - 1) / 2]->destination)
	{
		// Swap this node with its parent
		minHeap->position[minHeap->arrayList[size]->maxSize] = (size-1)/2;
		minHeap->position[minHeap->arrayList[(size-1)/2]->maxSize] = size;
		changeMinHeap(&minHeap->arrayList[size], &minHeap->arrayList[(size - 1) / 2]);

		// move to parent index
		size = (size - 1) / 2;
	}
}

/*calculating distance between vertices with using dijsktra algorithm*/
void algorithmDijkstra(struct graph *graph, int source)
{
	source = source - 65;
	/*getting the number of vertex from the graph*/
	int maxSize = graph->maxSize;
	
	/*initializing int array (according to number of vertex (maxSize)) for weights*/
	int destination[maxSize];	
	struct minHeap *minHeap = newMinHeap(maxSize);
	
	/*assigning min heap from vertices*/
    int i;
	for (i = 0; i < maxSize; ++i)
	{
		destination[i] = MAX_SIZE;
		minHeap->arrayList[i] = newMinHeapNode(i, destination[i]);
		minHeap->position[i] = i;
	}

	minHeap->arrayList[source] = newMinHeapNode(source, destination[source]);
	minHeap->position[source] = source;
	destination[source] = 0;
	reduce(minHeap, source, destination[source]);

	/*assigning number of vertices in graph*/
	minHeap->size = maxSize;
	
	while (!isEmpty(minHeap))
	{
		/*getting shortest distance*/
		struct minHeapNode* minHeapNode = getMin(minHeap);
	
		/*getting number of vertices*/
		int tempMaxSize = minHeapNode->maxSize;

		struct socialNetListNode *moveOn = graph->arrayList[tempMaxSize].header;

		while (moveOn != NULL)
		{
			int tempDestination = moveOn->destination;
			if ((destination[tempMaxSize] != MAX_SIZE) && (moveOn->weight + destination[tempMaxSize] < destination[tempDestination]) && (isInMinHeap(minHeap, tempDestination)))
			{
				destination[tempDestination] = destination[tempMaxSize] + moveOn->weight;
				/*changing distance between vertices*/
				reduce(minHeap, tempDestination, destination[tempDestination]);
			}
			moveOn = moveOn->next;
		}
	}
	int dest;
	for(dest = 0; dest < maxSize; dest++)
	{
		if(source == dest) continue;
		if(destination[dest] == MAX_SIZE) break;
		createAdjList(source, dest, destination[dest]);
	}
}

/*this function prints the content of the adjacency list*/
void createAdjList(int source, int destination, int weight)
{
	source += 65;
	destination += 65;
	if(vertexList == NULL)
	{
		vertexList = (struct vertex *) malloc(sizeof(struct vertex));
		vertexList->source = source;
		vertexList->next = NULL;
		vertexList->destList = (struct socialNetListNode *) malloc(sizeof(struct socialNetListNode));
		vertexList->destList->destination = destination;
		vertexList->destList->weight = weight;
		vertexList->destList->next = NULL;
		return;
	}

	struct socialNetListNode *iterSocial;

	if(vertexList->source == source)
	{
		iterSocial = vertexList->destList;
		while(iterSocial->next != NULL)
			iterSocial = iterSocial->next;
		iterSocial->next = (struct socialNetListNode *) malloc(sizeof(struct socialNetListNode));
		iterSocial = iterSocial->next;
		iterSocial->weight = weight;
		iterSocial->destination = destination;
		iterSocial->next = NULL;
		return;
	}

	struct vertex *iterVertex = vertexList;
	
	while(iterVertex->source != source && iterVertex->next != NULL)
		iterVertex = iterVertex->next;
	
	if(iterVertex->source == source)
	{
		iterSocial = iterVertex->destList;
		while(iterSocial->next != NULL)
			iterSocial = iterSocial->next;
		
		iterSocial->next = (struct socialNetListNode *) malloc(sizeof(struct socialNetListNode));
		iterSocial = iterSocial->next;
		iterSocial->destination = destination;
		iterSocial->weight = weight;
		iterSocial->next = NULL;
		return;
	}

	if(iterVertex->next == NULL)
	{
		iterVertex->next = (struct vertex *) malloc(sizeof(struct vertex));
		iterVertex = iterVertex->next;
		iterVertex->source = source;
		iterVertex->next = NULL;
		iterVertex->destList = (struct socialNetListNode *) malloc(sizeof(struct socialNetListNode));
		iterVertex->destList->destination = destination;
		iterVertex->destList->weight = weight;
		iterVertex->destList->next = NULL;
		return;
	}
}

/*prints all the vertices with their destination and their weights*/
void displayAdjList(struct vertex *root)
{
	struct vertex *iterVertex = root;
	struct socialNetListNode *iterSocial;
	while(iterVertex != NULL)
	{
		printf("%c : ", iterVertex->source);
		iterSocial = iterVertex->destList;
		while(iterSocial != NULL)
		{
			printf("%c,%d ", iterSocial->destination, iterSocial->weight);
			iterSocial = iterSocial->next;
		}
		iterVertex = iterVertex->next;
		printf("\n");
	}
}

/*prints shortest path between two vertices from created struct before*/
void findShortestPath()
{
	char charSource, charDest;
	int source, destination;
	printf("Enter the source vertex: ");
	scanf(" %c", &charSource);
	printf("Enter the destination vertex: ");
	scanf(" %c", &charDest);
	source = (int)charSource;
	destination = (int)charDest;

	if(source == destination)
	{
		printf("Shortest path between %c and %c is %d\n", source, destination, 0);
		return;
	}

	struct vertex *iterVertex = vertexList;
	while(iterVertex->source != source)
	{
		iterVertex = iterVertex->next;
		if(iterVertex == NULL)
		{
			printf("There is no such source in verteces. Please check and try again\n");
			return;
		}
	}
	struct socialNetListNode *iterSocial = iterVertex->destList;
	while(iterSocial->destination != destination)
	{
		iterSocial = iterSocial->next;
		if(iterSocial == NULL)
		{
			printf("There is no such destination in verteces. Please check and try again\n");
			return;
		}
	}
	printf("Shortest path between %c and %c is %d\n", iterVertex->source, iterSocial->destination, iterSocial->weight);
}

/*---------------------------------------------------------------------------------------*/

/*adds vertices into struct node to print them later for user choice '2'*/
void unorganizedAdjList(int source, int destination, int weight)
{
	if(undirectedVertex == NULL)
	{
		undirectedVertex = (struct vertex *) malloc(sizeof(struct vertex));
		undirectedVertex->next = NULL;
		undirectedVertex->source = source;
		undirectedVertex->destList = (struct socialNetListNode *) malloc(sizeof(struct socialNetListNode));
		undirectedVertex->destList->destination = destination;
		undirectedVertex->destList->weight = weight;
		undirectedVertex->destList->next = NULL;
		return;
	}

	struct socialNetListNode *iterSocial;

	if(undirectedVertex->source == source)
	{
		iterSocial = undirectedVertex->destList;
		while(iterSocial->next != NULL)
			iterSocial = iterSocial->next;
		
		iterSocial->next = (struct socialNetListNode *) malloc(sizeof(struct socialNetListNode));
		iterSocial->next->destination = destination;
		iterSocial->next->weight = weight;
		iterSocial->next->next = NULL;
		return;
	}

	struct vertex *iterVertex = undirectedVertex;
	while(iterVertex->source != source && iterVertex->next != NULL)
		iterVertex = iterVertex->next;

	if(iterVertex->source == source)
	{
		iterSocial = iterVertex->destList;
		while(iterSocial->next != NULL)
			iterSocial = iterSocial->next;
		
		iterSocial->next = (struct socialNetListNode *) malloc(sizeof(struct socialNetListNode));
		iterSocial->next->destination = destination;
		iterSocial->next->weight = weight;
		iterSocial->next->next = NULL;
		return;
	}

	if(iterVertex->next == NULL)
	{
		iterVertex->next = (struct vertex *) malloc(sizeof(struct vertex));
		iterVertex = iterVertex->next;
		iterVertex->source = source;
		iterVertex->next = NULL;
		iterVertex->destList = (struct socialNetListNode *) malloc(sizeof(struct socialNetListNode));
		iterVertex->destList->destination = destination;
		iterVertex->destList->weight = weight;
		iterVertex->destList->next = NULL;
		return;
	}
}

/*this function asks user to input file's name, checks if the input file exists.
then reads vertices and weights from input file to process*/
int inputFile(struct graph *graph)
{
	/*it will be using for input file name which is entering by user*/
	char fileName[MAX_SIZE];

	char source[CHAR_SIZE], destination[CHAR_SIZE];
	char charSource, charDestination;
	int intSource, intDestination;
	int weight;

	printf("Enter the name of the input file(i.e. input.txt): ");
	scanf("%s", fileName);
	FILE *inputText = fopen(fileName, "r");

	if(inputText == NULL)
	{
		/*printing input file's name is wrong or file doesn't exist*/
		printf("Please check the input file name.\n");
		/*returning zero(false value) because file doesn't exist*/
		return 0;
	}

	while(!feof(inputText))
	{
		/*first inputs are taken as a string (talking for source and destination not the weight)*/
		fscanf(inputText, "%[^,],%[^,],%d\n", source, destination, &weight);
		/*then source and destination should be converted to char type, to do that index 0 of strings is enough*/
		charSource = source[0];
		charDestination = destination[0];
		/*at the end, to not face any problem, char type should be converted into int type
		and to do that (int) is necessary*/
		intSource = (int)charSource;
		intDestination = (int)charDestination;
		/*source and destination (like A, B, C) are converted into into(ascii number) because
		they will be holding as number in minheap*/
		addVertex(graph, intSource, intDestination, weight);
		unorganizedAdjList(intSource, intDestination, weight);
		unorganizedAdjList(intDestination, intSource, weight);
	}
	isFileRead = true;
}

/*---------------------------------------------------------------------------------------*/

int main()
{
	int userChoice;
	struct graph *graph = newGraph(MAX_SIZE);
	struct graph *temp;
	int i, source, destination;
	char charSource, charDest;
	while(userChoice != 4)
	{
		printf("1- Read file\n2- Show adjacency list\n3- Find shortest path\n4- Exit\nYour choice: ");
		scanf("%d", &userChoice);
		temp = graph;
		for(i = 65; i < 73; i++)
		{
			algorithmDijkstra(temp, i);
		}
		switch (userChoice)
		{
		case 1:
			if(isFileRead == true)
			{
				printf("File has already read.\n");
				break;
			}
			inputFile(graph);
			break;
		
		case 2:
			if(isFileRead == false)
			{
				printf("Please, first of all try to read input file.\n");
				break;
			}
			displayAdjList(undirectedVertex);
			break;

		case 3:
			if(isFileRead == false)
			{
				printf("Please, first of all try to read input file.\n");
				break;
			}
			findShortestPath();
			
		default:
			break;
		}
	}
	return 0;
}