# CSE340_Project1
## Description
This project is a simple compiler for a polynomial-evaluation programming language. 

## Usage
This compiler takes programs written in a text file as input via redirection.

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;ex.) ```$ ./a.out < program.txt```

A program written in this language is composed of three sections (in order):

1. A polynomial declaration section
2. A START section
3. A user input section

### The Polynomial Declaration Section:
Polynomials are declared with the following 2 forms of syntax:

1.) ```POLY <name> = <polynomial>;```

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;note: polynomials declared in this form have only one default parameter, ```x```

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;ex.) ```POLY F = x^2 + x - 1;```

2.) ```POLY <name>(<parameter list>) = <polynomial>;```

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;ex.) ```POLY X(p, i) = p*(i + 22/7);```
      
### The START Section
The beginning of this section starts with a single line that reads ```START```.
The following lines can consist of the following statements:
  
1.) initializing a user-input variable

&nbsp;&nbsp;&nbsp;&nbsp;**form:** ```INPUT <variable name>;```

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;note: inputs are assigned to variables in the order in which they are provided.

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;ex.) "INPUT G;"
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;G will equal the int-value x if the input section consists of "```x w```" (where ```x``` is an integer, and ```w``` is a potentially empty list of integers delimited by spaces).

2.) polynomial function call 

&nbsp;&nbsp;&nbsp;&nbsp;**form:** ```<name>(<argument list>);```

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;ex.) ```F(1);```

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;ex.) ```F(G);```

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;ex.) ```X(G, i);```

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;ex.) ```F(X(5, G));```


### The User Input Section
This section is a line at the bottom of the program that consists of a non-empty list of integers.
There must be a sufficient number of integers specified in this line for all INPUT statements (There cannot be more INPUT statements than there are user inputs).
  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;ex.) ```-3 1 4 1 5 9```

### Simple Examples
Program_1.txt
```
POLY F = x + 9;
START
F(2);
1 2 3 4 5
```
Terminal output
```
$ ./a.out < Program_1.txt
11
```  
Program_2.txt
```
    POLY A(M, N) =
            M^6 + 3 N - N^2 + 71;
    POLY B(O, P, Q) = 1 O + 2 P + 3 Q - 4;
    POLY C(R, S, T) = S + 100;
    POLY D
            (U, V, W) = U^2 + 4 V^2 W^3 - 6 U^1 V^4;
    POLY E = x^5 - x^4 + 2;
    START
    INPUT X;
    INPUT Y;
    A(X, Y);

    INPUT X;
    INPUT Y;
    INPUT Z;
    B(X, Y, Z);

    INPUT X;
    INPUT Y;
    INPUT Z;
    C(X, Y, Z);
    D(X, Y, Z);

    INPUT X;
    INPUT Y;
    INPUT Z;
    D(X, Y, Z);
    E(Z);

    1 2 3 4 5 6 1 2 3 4 5 6 1 2 3 4 5 6 1 2 3 4 5 6
```
Terminal Output
```
    $ ./a.out < Program_1.txt
    74
    22
    101
    32
    3401
    2502
```
