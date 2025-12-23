# EAP Pseudocode Interpreter

A complete interpreter for the **EAP (Ελληνικό Ακαδημαϊκό Ψευδοκώδικα)** pseudocode language used in Greek academic institutions, particularly in the Hellenic Open University's computer science curriculum.

## Features

- ✅ **Full Greek & English syntax support** - Write algorithms in Greek or English keywords
- ✅ **Complete language implementation** - Variables, arrays, functions, procedures, loops, conditionals
- ✅ **Pass-by-reference semantics** - Proper handling of reference parameters with `%` notation
- ✅ **Multi-dimensional arrays** - Support for arrays with custom bounds (e.g., `ARRAY[1..10, 5..15]`)
- ✅ **Type system** - INTEGER, REAL, BOOLEAN, STRING, CHAR types
- ✅ **Built-in functions** - Mathematical operations, logical operators, I/O
- ✅ **Debug mode** - Step-by-step execution tracing with `--debug` flag
- ✅ **UTF-8 & Windows-1253** - Automatic encoding detection and conversion

## Installation

### Prerequisites
- GCC compiler
- Standard C library
- Math library (`-lm`)

### Build
```bash
git clone https://github.com/labrouss/C-Greek-Pseudocode-Interpreter
cd eap-interpreter
gcc -o eap_interpreter interpreter.c -lm
```

### Quick Test
```bash
./eap_interpreter examples/hello.eap
```

## Usage

```bash
./eap_interpreter <program.eap> [--debug]
```

### Options
- `--debug` - Enable detailed execution tracing

### Example

**hello.eap:**
```
ΑΛΓΟΡΙΘΜΟΣ HelloWorld
ΔΕΔΟΜΕΝΑ
    name: STRING;
ΑΡΧΗ
    ΤΥΠΩΣΕ("Πώς σε λένε; ");
    ΔΙΑΒΑΣΕ(name);
    ΤΥΠΩΣΕ("Γεια σου, ", name, "!", EOLN);
ΤΕΛΟΣ
```

**Run:**
```bash
./eap_interpreter hello.eap
```

**Output:**
```
Πώς σε λένε; Νίκος
Γεια σου, Νίκος!
```

## Language Overview

### Basic Structure

```
ΑΛΓΟΡΙΘΜΟΣ ProgramName
ΣΤΑΘΕΡΕΣ
    PI=3.14159;
    MAX=100;
ΔΕΔΟΜΕΝΑ
    x, y: INTEGER;
    result: REAL;
    numbers: ARRAY[1..10] OF INTEGER;
ΑΡΧΗ
    // Main program body
ΤΕΛΟΣ
```

### Control Structures

**IF Statement:**
```
ΕΑΝ (x > 0) ΤΟΤΕ
    ΤΥΠΩΣΕ("Positive");
ΑΛΛΙΩΣ
    ΤΥΠΩΣΕ("Non-positive");
ΕΑΝ-ΤΕΛΟΣ
```

**FOR Loop:**
```
ΓΙΑ i:=1 ΕΩΣ 10 ΕΠΑΝΑΛΑΒΕ
    ΤΥΠΩΣΕ(i);
ΓΙΑ-ΤΕΛΟΣ
```

**WHILE Loop:**
```
ΕΝΟΣΩ (x < 100) ΕΠΑΝΑΛΑΒΕ
    x:=x * 2;
ΕΝΟΣΩ-ΤΕΛΟΣ
```

**REPEAT-UNTIL Loop:**
```
ΕΠΑΝΑΛΑΒΕ
    ΔΙΑΒΑΣΕ(x);
ΜΕΧΡΙ (x >= 0);
```

### Functions and Procedures

**Function:**
```
ΣΥΝΑΡΤΗΣΗ factorial(n): INTEGER
ΔΙΕΠΑΦΗ
   ΕΙΣΟΔΟΣ
      n: INTEGER;
   ΕΞΟΔΟΣ
      factorial: INTEGER;
ΑΡΧΗ
    ΕΑΝ (n <= 1) ΤΟΤΕ
        factorial:=1;
    ΑΛΛΙΩΣ
        factorial:=n * factorial(n-1);
    ΕΑΝ-ΤΕΛΟΣ
ΤΕΛΟΣ-ΣΥΝΑΡΤΗΣΗΣ
```

**Procedure with pass-by-reference:**
```
ΔΙΑΔΙΚΑΣΙΑ swap(%x, %y)
ΔΙΕΠΑΦΗ
   ΕΙΣΟΔΟΣ
      x, y: INTEGER;
   ΕΞΟΔΟΣ
      x, y: INTEGER;
ΔΕΔΟΜΕΝΑ
    temp: INTEGER;
ΑΡΧΗ
    temp:=x;
    x:=y;
    y:=temp;
ΤΕΛΟΣ-ΔΙΑΔΙΚΑΣΙΑΣ
```

### Arrays

**Declaration:**
```
numbers: ARRAY[1..10] OF INTEGER;
matrix: ARRAY[1..3, 1..3] OF REAL;
```

**Usage:**
```
numbers[5]:=42;
matrix[2,3]:=3.14;
```

**Arrays are always passed by reference:**
```
ΔΙΑΔΙΚΑΣΙΑ fill_array(%arr, n)
ΔΙΕΠΑΦΗ
   ΕΙΣΟΔΟΣ
      arr: ARRAY[1..100] OF INTEGER;
      n: INTEGER;
   ΕΞΟΔΟΣ
      arr: ARRAY[1..100] OF INTEGER;
ΑΡΧΗ
    ΓΙΑ i:=1 ΕΩΣ n ΕΠΑΝΑΛΑΒΕ
        arr[i]:=i * 10;
    ΓΙΑ-ΤΕΛΟΣ
ΤΕΛΟΣ-ΔΙΑΔΙΚΑΣΙΑΣ
```

## Examples

### Bubble Sort
```
ΑΛΓΟΡΙΘΜΟΣ BubbleSort
ΣΤΑΘΕΡΕΣ
    N=10;
ΔΕΔΟΜΕΝΑ
    arr: ARRAY[1..N] OF INTEGER;
    i, j, temp: INTEGER;

ΑΡΧΗ
    // Input
    ΓΙΑ i:=1 ΕΩΣ N ΕΠΑΝΑΛΑΒΕ
        ΔΙΑΒΑΣΕ(arr[i]);
    ΓΙΑ-ΤΕΛΟΣ
    
    // Bubble sort
    ΓΙΑ i:=1 ΕΩΣ N-1 ΕΠΑΝΑΛΑΒΕ
        ΓΙΑ j:=1 ΕΩΣ N-i ΕΠΑΝΑΛΑΒΕ
            ΕΑΝ (arr[j] > arr[j+1]) ΤΟΤΕ
                temp:=arr[j];
                arr[j]:=arr[j+1];
                arr[j+1]:=temp;
            ΕΑΝ-ΤΕΛΟΣ
        ΓΙΑ-ΤΕΛΟΣ
    ΓΙΑ-ΤΕΛΟΣ
    
    // Output
    ΓΙΑ i:=1 ΕΩΣ N ΕΠΑΝΑΛΑΒΕ
        ΤΥΠΩΣΕ(arr[i], " ");
    ΓΙΑ-ΤΕΛΟΣ
ΤΕΛΟΣ
```

See the `examples/` folder for more complete programs including:
- Sorting algorithms (selection, insertion, bubble)
- Search algorithms (linear, binary)
- Recursion examples
- Matrix operations
- String manipulation

## Language Reference

### Keywords (Greek/English)

| Greek | English | Purpose |
|-------|---------|---------|
| ΑΛΓΟΡΙΘΜΟΣ | ALGORITHM | Program declaration |
| ΣΤΑΘΕΡΕΣ | CONSTANTS | Constants section |
| ΔΕΔΟΜΕΝΑ | DATA | Variables section |
| ΑΡΧΗ | BEGIN | Start of block |
| ΤΕΛΟΣ | END | End of block |
| ΣΥΝΑΡΤΗΣΗ | FUNCTION | Function declaration |
| ΔΙΑΔΙΚΑΣΙΑ | PROCEDURE | Procedure declaration |
| ΔΙΕΠΑΦΗ | INTERFACE | Parameter specification |
| ΕΙΣΟΔΟΣ | INPUT | Input parameters |
| ΕΞΟΔΟΣ | OUTPUT | Output parameters |
| ΕΑΝ | IF | Conditional |
| ΤΟΤΕ | THEN | Then clause |
| ΑΛΛΙΩΣ | ELSE | Else clause |
| ΓΙΑ | FOR | For loop |
| ΕΩΣ | TO | Loop upper bound |
| ΒΗΜΑ | STEP | Loop step |
| ΕΠΑΝΑΛΑΒΕ | REPEAT | Loop body start |
| ΕΝΟΣΩ | WHILE | While loop |
| ΜΕΧΡΙ | UNTIL | Until condition |
| ΤΥΠΩΣΕ | PRINT | Output statement |
| ΔΙΑΒΑΣΕ | READ | Input statement |
| ΑΚΕΡΑΙΟΣ | INTEGER | Integer type |
| ΠΡΑΓΜΑΤΙΚΟΣ | REAL | Real type |
| ΛΟΓΙΚΟΣ | BOOLEAN | Boolean type |
| ΧΑΡΑΚΤΗΡΑΣ | CHAR | Character type |
| ΣΥΜΒΟΛΟΣΕΙΡΑ | STRING | String type |

### Operators

| Operator | Description |
|----------|-------------|
| `:=` | Assignment |
| `+` `-` `*` `/` | Arithmetic |
| `DIV` | Integer division |
| `MOD` | Modulo |
| `=` `<>` | Equality/Inequality |
| `<` `>` `<=` `>=` | Comparisons |
| `ΚΑΙ` / `AND` | Logical AND |
| `Ή` / `OR` | Logical OR |
| `ΟΧΙ` / `NOT` | Logical NOT |

### Built-in Constants

- `EOLN` - End of line (newline)

## Technical Details

### Implementation

- **Parser:** Recursive descent parser with operator precedence
- **Execution:** Tree-walking interpreter with environment-based scoping
- **Memory:** Heap-allocated for arrays and strings with automatic management
- **Arrays:** HashMap-based storage for flexible bounds and dimensions
- **Encoding:** Automatic detection and conversion between UTF-8 and Windows-1253

### Architecture

```
Source Code (.eap)
    ↓
Tokenizer (Lexical Analysis)
    ↓
Tokens
    ↓
Parser (Syntax Analysis)
    ↓
Abstract Syntax Tree (AST)
    ↓
Interpreter (Execution)
    ↓
Output
```

### Key Features

**Pass-by-Reference Implementation:**
- Arrays always use pointer sharing (no deep copies)
- Simple types marked with `%` in OUTPUT section are copied back
- Environment chain for proper scoping

**Array Implementation:**
- Custom bounds (e.g., `ARRAY[5..15]` or `ARRAY[-10..10]`)
- Multi-dimensional with arbitrary dimensions
- Bounds checking at runtime
- HashMap-based storage for sparse arrays

## Troubleshooting

### Common Issues

**1. Greek characters showing as gibberish:**
- Ensure your terminal supports UTF-8
- On Windows, use `chcp 65001` before running
- The interpreter auto-detects encoding

**2. "Undefined variable" errors:**
- Check variable spelling (case-insensitive)
- Ensure variables are declared in ΔΕΔΟΜΕΝΑ section
- Check scope (local vs global)

**3. Array index out of bounds:**
- Verify array bounds match declaration
- Arrays use declared bounds, not 0-based indexing

**4. Compilation errors:**
- Ensure you have gcc installed: `gcc --version`
- Link math library: `-lm` flag is required

## Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

### Development

**Running tests:**
```bash
./test.sh
```

**Debug mode:**
```bash
./eap_interpreter program.eap --debug
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Based on the EAP pseudocode specification from the Hellenic Open University
- Inspired by the need for a free, open-source EAP interpreter for students
- Thanks to all contributors and testers

**Star ⭐ this repository if you find it useful!**
