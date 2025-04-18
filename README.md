<h1 align="center">LoremScriptum Compiler</h1>
<p align="center">
    <img src="https://img.shields.io/github/repo-size/vbielov/LoremScriptumCompiler" />
    <img src="https://img.shields.io/github/contributors/vbielov/LoremScriptumCompiler" />
    <img src="https://img.shields.io/github/stars/vbielov/LoremScriptumCompiler" />
</p>
<p align="center">
    <img src="./resources/icon.ico" width="100" />
</p>

LoremScriptum is a new compiled programming language featuring Latin-inspired syntax and mathematical symbols. It uses LLVM to generate executable code for any Linux or Windows system.

> [!WARNING]
> This is just a student project for fun and education.
> Do not use it in production!

**Compiler usage:**

   ```bash
   ./lsc <input_file.lorem>
   ```

> [!TIP]
> For a better programming experience we **strongly** recommend using VS Code with the [LoremScriptum Extension](https://marketplace.visualstudio.com/items?itemName=BackBencher.loremscriptum)  
> _For more information on how to use the extension, see [this](#how-to-special-keywords--operators) section below._

## Installation

You can find pre-built binaries for Windows/Linux in [Releases](https://github.com/vbielov/LoremScriptumCompiler/releases).

But they still require LLVM libraries of right versions to be installed.

Guide for building this project can be found in [INSTALL.md](./INSTALL.md)

## Our Compiler structure

The LoremScriptum compiler is divided into 4 main parts all of which are written in C++:

- **Lexer**: This part of the compiler reads the input file and converts it into a list of tokens.
- **Parser**: The parser reads the list of tokens and creates an Abstract Syntax Tree (AST).
- **Code Generator**: The code generator reads the AST and generates LLVM IR code.
- **LLVM**: Now LLVM takes over and generates the executable code. This will tremendously increase the speed and portability of the compiler.

## Custom Error Handling

The compiler uses a custom error handling system to provide detailed error messages to help the user understand what went wrong.
If the compiler encounters an error, it will try to regain control and continue parsing the code to find more errors.  
You can also click on the line number to jump directly to the error in your code.

![Error Handling](./resources/img/showcase_error_handling.png)

## Language Syntax

Here is a short overview of the capabilities of LoremScriptum

```lorem
apere './std.lorem'

numerus var = III

var += II

si(var ⇔ V):
    scriborNum(var)
;

numerus[IV] array = [O, I, II, III]

∑(numerus i = I, i < IV, i += I):
    numerus combinedValue = foo(array[i-I], array[i])
    scriborNum(combinedValue)
;

numerus foo = λ(numerus a, numerus b):
    retro a + b
;

rerum vector_2d = (numerus x, numerus y)
vector_2d point = [V, -VI]
point[x] = VI
```

```lorem
nihil printf = λ(litera str)

litera[] helloWorld = "Hello World!"
printf(helloWorld)
```

### How to: Types

There are a total of **4** types in LoremScriptum:

| LoremScriptum | Equivalent | Example                     |
| ------------- | ---------- | --------------------------- |
| numerus       | int        | O, XLII                     |
| asertio       | boolean    | veri, falso                 |
| litera        | char       | 'a', '\n'                   |
| rerum         | struct     | [see this](#how-to-structs) |
| nihil         | void       | /                           |

> [!NOTE]  
> _nihil_ can only be used in function declarations.  
> numerus `O` is the equivalent to an Arabic zero. The Roman number system does not actually include a symbol for zero.

You can also make an _array_ of any type by appending `[size]` to the type where `size` is a fixed numerus.

**Examples:**

```lorem
numerus[V] arr = [O, I, II, III, IV]
litera[V] word = ['H', 'e', 'l', 'l', 'o']
```

### How to: Declarations

A declaration is always of the structure: `type identifier = expression`  
An identifier can be any combination of letters, numbers and underscores. but it must start with a letter or underscore.

**Examples:**

```lorem
numerus var = II + (IX + I) * IV
asertio isTrue = veri
litera letter = 'a'
```

You can also declare functions using the following syntax: `type identifier = λ(parameters): ... ;`

```lorem
numerus add = λ(numerus a, numerus b):
    retro a + b
;

numerus result = add(II, III)
```

> [!NOTE]
> The `retro` keyword is used to return a value from a function.  
> Code blocks can be opened with `:` and closed with `;`. This is the equivalent of `{` and `}` in other languages.

### How to: Arrays

You can define an array using the following syntax: `type[size] identifier = [value1, value2, ...]`  
It is also possible to define strings. In LoremScriptum a string is an array of chars `litera[]`.

**Examples:**

```lorem
numerus[IV] a = [O, I, II, III, IV]
litera[VI] b = ['H', 'e', 'l', 'l', 'o', '!']

litera[] str = "Hello World!"

a[II] += I

numerus[] c = [O, I, II, III, IV]
```

> [!NOTE]
> You can access an element of an array using the `[x]` operator, where `x` is the index of the element.  
> You can also define an array without specifying the size. In this case the size will be calculated from the number of elements in the array.  
> If you define a string using the shorthand version `litera[] str = 'Hello World!'` the array will always end with `\0`. It is not possible to add two strings.

> [!WARNING]
> The compiler will not set default values for the elements of an array and without setting them manually their behaviour is undefined.

### How to: Structs

You can define a struct using the following syntax: `rerum structName = (type member1, type member2, ...)`  
After defining a struct you can create an instance of it using the following syntax: `structName instance = [value1, value2, ...]`

**Examples:**

```lorem
rerum vector_2d = (numerus x, numerus y)
vector_2d point
point[x] = VI
```

> [!NOTE]
> Creation of a struct type is done using `()` and instantiation is like an array with `[]`.

### How to: Flow Control

Flow control in LoremScriptum is done using the following keywords:

| LoremScriptum | Equivalent  |
| ------------- | ----------- |
| si            | if          |
| ni            | else        |
| nisi          | else if     |
| ∑             | while / for |

**Examples:**

```lorem
si a > b:
    ...
; nisi a < b:
    ...
; ni:
    ...
;
```

```lorem
∑():
    finio
;

∑(numerus i = O, i < V, i++):
    ...
;

∑(var < V, var++):
    ...
;

∑(var++):
    ...
;
```

> [!NOTE]
> If you want to escape the loop use the `finio` keyword. This is the equivalent of `break` in other languages.

### How to: Special Keywords & Operators

LoremScriptum uses multiple unique keywords and operators, most of which are difficult to type on a standard keyboard.

You can use the VS Code [LoremScriptum extension](https://marketplace.visualstudio.com/items?itemName=BackBencher.loremscriptum) and `CTRL + SHIFT + P` to open the command palette and search for  
`>LOREM toggle latex replace on save` to _automatically_ replace the keywords with their LoremScriptum equivalent.  
With this extension you can also have **syntax highlighting** for LoremScriptum.

The plugin will replace the following keywords:

| User Input | Plugin will replace with |
| ---------- | ------------------------ |
| `\lambda`  | `λ`                      |
| `\sum`     | `∑`                      |
| `==`       | `⇔`                      |
| `!=`       | `≠`                      |
| `>=`       | `≥`                      |
| `<=`       | `≤`                      |
| `*`        | `×`                      |
| `/`        | `÷`                      |
| `\neg`     | `¬`                      |
| `\and`     | `∧`                      |
| `\or`      | `∨`                      |

> [!NOTE]
> If you do not want to use the extension you can also copy the characters from the table above and paste them into your code.

### How to: Include other LoremScriptum files

You can include other LoremScriptum files using the `apere` keyword. This is the equivalent of `#include` in other languages.

The compiler will paste the content of the included file at the position of the `apere` keyword.

**Example:**

```lorem
apere './std.lorem'
apere './lib.a'
```

<p align="center">
    <img src="./resources/img/jeff.png" />
    <br/>
    say hi to Jeff
</p>