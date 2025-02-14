# ΛoremScriptum Compiler

LoremScriptum is a basic programming language with Latin-inspired syntax and mathematical symbols that replace conventional flow control and basic operations.

> [!WARNING] Do not use it in production
> This is just a student project for fun and education.

## Installation

To build and use the LoremScriptum compiler, follow these steps:

1. **Clone the repository:**

    ```bash
    git clone https://...
    cd ...
    ```

2. **Build the compiler:**

    ```bash
    mkdir build
    cd build
    cmake ..
    make
    ```

3. **Run the compiler:**

    ```bash
    ./lsc <input_file.ls>
    ```

## Language Syntax

Here are some examples of LoremScriptum code:

```loremscriptum
nihil main := λ():
    numerus x := X 
    si x > V:
        scribor("X est maior quam V")
    ;
    ni:
        scribor("X non est maior quam V")
    ;
;
```

```loremscriptum
numerus i := ∅
∑(∞):
    si i > X:
        finio
    ;
    i := i + I
;
```

## More examples

```loremscriptum
numerus foo := λ(numerus a, numerus b):
    a := a + b
    b := a * II
    retro b
;
numerus c = foo(I, V)
```

```loremscriptum
numerus cadabra := XVI

∑(∞):
    Numerus abra := II
    abra := abra + abra
    
    si abra > cadabra:
        finio
    ;    
;

scribor(abra)

literra test := 'c'

scribor(test)

si abra = cadabra:
    scribor("aaaa")
;
nisi abra < cadabra:
    scribor("bbbb");
ni:
    scribor("cccc");

scribor(abra ≠ cadabra)
scribor(abra = cadabra)
scribor(abra < cadabra)
scribor(abra > cadabra)

scribor(abra + cadabra)
scribor(abra - cadabra)
scribor(abra ^ cadabra)
scribor(abra / cadabra)
scribor(¬cadabra)
scribor(abra!)

numerus addition := λ(numerus α, numerus beta): retro α + β;

// beyond here is past minimal vialble product

∑(i) ∈ [0,5]:
    scribor(i)
;

literra[] magie := "abracadabra"

scribor(magie)

numerus[] trahtebedoh := [I, III, V, I]

∑(i ∈ trahtebedoh):
    scribor(i)
;

nihil function := \lamda (littera[] scripture): scribor(scripture);

```
