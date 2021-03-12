# <center> 🌽🏃 MazeSolver </center>
**An Implementation of Dijkstra's Shortest Path Algorithm Using Btree**

  
![algorithm demo](Btree-Dijkstra.gif)

##  Table of Contents


* [About the Program](#about-the-program)

	* [Built With](#built-with)

	* [How to Use](#how_to_use)

* [Contact](#contact)

  

<!-- ABOUT THE Program -->

##  About The Program

This is a maze solver program which is implemented using the Dijkstra's shortest path algorithm and Btree data structure for improving the efficiency.
The Btree was implemented with a little help from [HERE](https://uxmankabir.wordpress.com/2017/05/08/cpp-program-to-perform-insertion-deletion-and-traversal-in-b-tree/).

###  Built With

* [C++](https://www.cplusplus.com/)



###  How to Use

#### Input:

 1. Number of rows and columns of your maze
 2. Your maze such that:
			
	⛔:  0 means a dead end.
			
	⛏️: Positive numbers indicate the difficulty of that path.

#### Output:

The solved maze such that the shortest path from the start to the finish is marked with -1s.

  
####  Test Cases

  You can directly copy and paste the following test cases to try out the program. 

 1. First test case:
 
	    4 4
		0 1 0 0
		0 1 1 1
		1 2 3 2
		0 0 1 0
		
2. Second test case:
		
		8 10
        0 0 0 1 0 0 0 0 0 0
	    0 3 1 1 0 1 51 0 1 0
	    0 2 0 1 0 0 1 41 1 0
	    0 10 0 1 1 111 1 0 0 0
	    0 34 1 1 0 0 1 23 1 0
	    0 1 0 5 0 1 1 0 0 0
	    0 1 0 166 1 0 54 1 1 0
	    0 0 0 0 0 0 0 7 0 0
  
4. Third test case:

	    7 11
		0 0 0 0 0 0 0 1 0 0 0
		0 0 3 2 5 3 2 5 6 4 2
		0 1 1 7 6 1 1 7 9 0 3
		1 1 4 0 1 9 4 8 6 0 2
		1 1 4 1 9 9 1 8 3 4 2
		0 3 1 1 2 1 1 2 1 0 0
		0 0 0 0 1 0 0 0 0 0 0

<!-- CONTACT -->

##  Contact

Parisa Rabbany - Parisa.Rabbany.pr@gmail.com
