# Why Java `.class` Files Preserve Symbol Names So Well

## Question

Why is it so easy to recover symbol names such as classes, fields, and methods from Java `.class` files?

At first glance, Java bytecode sounds like it should be similar to native assembly for a CPU such as the Motorola 68000, except aimed at a virtual machine. In practice, `.class` files often look much more readable than native binaries, and decompilers recover a surprising amount of structure.

## Short Answer

Java bytecode is real machine code for the JVM, but a `.class` file is not just a flat stream of opcodes.

It is better understood as:

- executable bytecode for a stack-based virtual machine
- plus a rich constant pool
- plus type descriptors
- plus optional debug metadata
- plus attributes used for linking, verification, reflection, and tooling

So the code is not "just tokenized source", but it is also not comparable to a stripped native executable.

The JVM depends on symbolic information much more heavily than a native CPU does, so `.class` files intentionally retain many names.

## The Native Binary Comparison

With a native target such as the 68000:

- the CPU only needs instructions and addresses
- labels and source-level names are assembler conveniences
- after assembly and linking, function names and variable names can disappear completely
- a stripped binary can still execute perfectly

With the JVM:

- classes are loaded dynamically
- methods and fields are resolved symbolically
- types are verified by the VM
- reflection can expose class and member information at runtime
- stack traces and annotations rely on metadata

That means a `.class` file must preserve more symbolic structure by design.

## What a `.class` File Actually Contains

A class file usually contains:

1. A header and version.
2. A constant pool.
3. Class, field, and method definitions.
4. Method bytecode.
5. Attributes such as `SourceFile`, `LineNumberTable`, annotations, and sometimes local variable tables.

The key part is the constant pool. It stores entries such as:

- class names
- method names
- field names
- type descriptors
- string literals
- source file names
- references used by bytecode instructions

So an instruction often looks like this:

```text
invokevirtual #35
```

That `#35` is not a raw address. It points into the constant pool, where the JVM can find the target symbolically, for example:

- owning class
- method name
- method descriptor

This is one of the main reasons Java remains readable under reverse engineering.

## What Bytecode Is, and What It Is Not

Java bytecode is genuine VM assembly. It is not source tokens packed into a file.

By the time Java source becomes bytecode:

- `if`, `for`, `while`, and `switch` become jumps and stack operations
- field and method access become bytecode instructions such as `getfield`, `putfield`, `invokevirtual`, and `invokestatic`
- generic types are mostly erased
- syntax sugar such as enhanced `for`, lambdas, and try-with-resources is lowered into more primitive forms

For example, this source:

```java
return sum * scale;
```

becomes bytecode in this shape:

```text
4: iload_3
5: aload_0
6: getfield      #7                  // Field scale:I
9: imul
10: ireturn
```

That is clearly not tokenized source code. It is a stack-machine instruction stream.

However, the instruction stream still refers to symbolic entries such as `scale:I`, and that is why the surrounding file remains much richer than stripped native code.

## Concrete Example From This Repository

I verified the examples below with `javap 17.0.15` against the original class files in `original/forward`.

### Example 1: `Mixable.class`

Command:

```powershell
javap -classpath original/forward -v muhmu.hifi.device.Mixable
```

Relevant output:

```text
Classfile /C:/works/projects/preservation-forward-by-komplex/original/forward/muhmu/hifi/device/Mixable.class
Compiled from "Mixable.java"
public interface muhmu.hifi.device.Mixable
Constant pool:
   #9 = Utf8               Mixable.java
  #10 = Utf8               SourceFile
  #11 = Utf8               java/lang/Object
  #12 = Utf8               mix
  #13 = Utf8               muhmu/hifi/device/Mixable
{
  public abstract boolean mix(muhmu.hifi.device.MAD, int[], int, int);
    descriptor: (Lmuhmu/hifi/device/MAD;[III)Z
}
SourceFile: "Mixable.java"
```

Even this tiny class already preserves:

- the source file name: `Mixable.java`
- the interface name: `muhmu.hifi.device.Mixable`
- the method name: `mix`
- the full method descriptor

So a decompiler does not have to guess those names. They are present in the class file.

### Example 2: `MAD.class`

Command:

```powershell
javap -classpath original/forward -v muhmu.hifi.device.MAD
```

Relevant constant-pool entries:

```text
  #26 = Fieldref           #23.#44       // muhmu/hifi/device/MAD.boost:I
  #27 = Fieldref           #23.#45       // muhmu/hifi/device/MAD.component:Ljava/awt/Component;
  #30 = Methodref          #23.#48       // muhmu/hifi/device/MAD.getBestAudioDevice:(Lmuhmu/hifi/device/Mixable;I)Lmuhmu/hifi/device/MAD;
  #35 = Methodref          #23.#53       // muhmu/hifi/device/MAD.init:(Lmuhmu/hifi/device/Mixable;IIILjava/awt/Component;)Z
  #36 = Fieldref           #23.#54       // muhmu/hifi/device/MAD.nameIE3:Ljava/lang/String;
  #37 = Fieldref           #23.#55       // muhmu/hifi/device/MAD.nameIE4:Ljava/lang/String;
  #38 = Fieldref           #23.#56       // muhmu/hifi/device/MAD.nameSun:Ljava/lang/String;
```

Relevant bytecode:

```text
14: aload_2
15: aload_0
16: sipush        22000
19: iconst_4
20: ldc           #6                  // int 44100
22: getstatic     #27                 // Field component:Ljava/awt/Component;
25: invokevirtual #35                 // Method init:(Lmuhmu/hifi/device/Mixable;IIILjava/awt/Component;)Z
28: pop
29: aload_2
30: areturn
```

This is a good illustration of how JVM bytecode works:

- the actual instruction stream is low-level and stack-based
- but symbolic references still exist through the constant pool
- `#27` and `#35` resolve to meaningful field and method names

This is why reverse engineering Java is usually much easier than reverse engineering a stripped native binary.

## Reproducible Mini Demo: With and Without Debug Metadata

I also compiled a small throwaway example with `javac 17.0.15` to show which names are always kept and which names are optional.

### Source

```java
public class SymbolDemo {
    private final int scale;

    public SymbolDemo(int scale) {
        this.scale = scale;
    }

    public int addScaled(int left, int right) {
        int sum = left + right;
        return sum * scale;
    }
}
```

### Build With Debug Info

Command:

```powershell
javac -g SymbolDemo.java
javap -v SymbolDemo
```

Relevant output:

```text
Constant pool:
   #7 = Fieldref           #8.#9          // SymbolDemo.scale:I
   #8 = Class              #10            // SymbolDemo
   #9 = NameAndType        #11:#12        // scale:I
  #10 = Utf8               SymbolDemo
  #11 = Utf8               scale
  #19 = Utf8               addScaled
  #21 = Utf8               left
  #22 = Utf8               right
  #23 = Utf8               sum

LocalVariableTable:
  Start  Length  Slot  Name   Signature
      0      11     0  this   LSymbolDemo;
      0      11     1  left   I
      0      11     2 right   I
      4       7     3   sum   I
```

This shows two different categories of names:

- required symbolic names such as `SymbolDemo`, `scale`, and `addScaled`
- optional debug names such as local variables `left`, `right`, and `sum`

### Build Without Debug Info

Command:

```powershell
javac -g:none SymbolDemo.java
javap -v SymbolDemo
```

Relevant output:

```text
Constant pool:
   #7 = Fieldref           #8.#9          // SymbolDemo.scale:I
   #8 = Class              #10            // SymbolDemo
   #9 = NameAndType        #11:#12        // scale:I
  #10 = Utf8               SymbolDemo
  #11 = Utf8               scale
  #15 = Utf8               addScaled
```

The local variable table is gone, but the class name, field name, and method name remain.

This is the crucial distinction:

- local variable names may disappear
- source file names may disappear
- line tables may disappear
- but many member and type names remain because the JVM needs them for linkage and type information

## Why Decompilers Work So Well on Java

A Java decompiler benefits from all of the following:

- explicit class names
- explicit field names
- explicit method names
- explicit descriptors
- structured exception tables
- source file metadata in many builds
- line numbers in many builds
- optional local variable names in debug builds

It still has to reconstruct higher-level syntax, but it starts from much richer input than a stripped native disassembly.

## What Still Gets Lost

A `.class` file does not preserve everything from the original source.

Common losses include:

- comments
- exact formatting
- many local variable names if debug metadata is removed
- some generic information after type erasure
- the exact original control-flow spelling
- some source-level intent hidden behind compiler lowering

So the result is not perfect source recovery. It is simply much better than people expect if they are thinking in terms of native machine code.

## Better Mental Model

The best mental model is:

- Java bytecode is indeed VM assembly
- but a `.class` file is more like a compact, typed, symbolic object file than a stripped CPU executable

So the answer is not:

- "the source is just tokenized"

and also not:

- "this is the same thing as 68000 assembly, just for a VM"

It is closer to:

- "real VM instructions wrapped in a metadata-rich format that preserves symbolic references on purpose"

## Practical Consequence

This is why obfuscation tools such as `ProGuard` and `R8` matter:

- they rename classes, fields, and methods
- they can remove or reduce debug metadata
- they make the surviving symbolic information much less useful to a reverse engineer

Without that extra step, Java class files are often very readable.
