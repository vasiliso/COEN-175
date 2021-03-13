# Building a SimpleC compiler in C++

COEN 175 taught by Dr. Darren C. Atkinson at Santa Clara University, Winter 2021.
Vasilis Odysseos



## The Compiler was built in six phases.

### Phase 1: Lexical Analysis

### Phase 2: Syntax Analysis

### Phase 3: Symbol Table Construction

### Phase 4: Type Checking

### Phase 5: Storage Allocation

### Phase 6: Code Generation


Each phase is contained in its own folder, and for each new phase, correct code for the previous phase was provided by Dr. Atkinson to be built upon for the new phase.

### The folder structure is as follows:

```
Phase X
│   CHECKSUB.sh -- Provided tester tool for each phase
│   examples.tar -- Provided test cases    
│
└─── Docs -- Provided lab document and keynote
│   
│   
└─── phasex\ -- Implementation of phase x
```

### Build instructions
* #### Build any phase
    1. Open a terminal window and cd into the Phase 6/phase6 directory
    2. Run the make command to create an executable
    3. Run the executable using ./scc
* #### Build the final compiler and use it
    1. Follow the instructions above to build phase 6
    2. You can use the following command to generate assembly code for any SImple-C file
        ```console
        foo@bar:~$ ./scc < input-file.c > output-file.s 
        ```
    3. You can then use gcc with the -m32 flag to generate the output file


