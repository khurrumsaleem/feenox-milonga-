# Why nothing happens when I double click on `feenox.exe`?

Because by design, FeenoX does not have a Graphical User Interface. Instead, it works like a transfer function between one or more input files and zero or more output files:

```include
transfer.md
```

Recall that FeenoX is designed as a cloud-first tool, and “the cloud” runs on Unix (essentially GNU/Linux) so FeenoX is based on the [Unix philosophy](./unix.md). In this world, programs act like filters (i.e. transfer functions). They are executed from a command-line terminal. So instead of “double clicking” the executable, one has to open a terminal and execute it there. Without any arguments it says how to use it:

```terminal
> feenox.exe
FeenoX v0.2.14-gbbf48c9
a free no-fee no-X uniX-like finite-element(ish) computational engineering tool

usage: feenox [options] inputfile [replacement arguments] [petsc options]

  -h, --help         display options and detailed explanations of commmand-line usage
  -v, --version      display brief version information and exit
  -V, --versions     display detailed version information

Run with --help for further explanations.
>
```

With `-h` it gives more information:

```terminal
> feenox.exe -h
usage: feenox [options] inputfile [replacement arguments] [petsc options]

  -h, --help         display options and detailed explanations of commmand-line usage
  -v, --version      display brief version information and exit
  -V, --versions     display detailed version information

  --progress         print ASCII progress bars when solving PDEs
  --mumps            ask PETSc to use the direct linear solver MUMPS
  --linear           force FeenoX to solve the PDE problem as linear
  --non-linear       force FeenoX to solve the PDE problem as non-linear

Instructions will be read from standard input if “-” is passed as
inputfile, i.e.

    $ echo 'PRINT 2+2' | feenox -
    4

The optional [replacement arguments] part of the command line mean that
each argument after the input file that does not start with an hyphen
will be expanded verbatim in the input file in each occurrence of $1,
$2, etc. For example

    $ echo 'PRINT $1+$2' | feenox - 3 4
    7

PETSc and SLEPc options can be passed in [petsc options] as well, with
the difference that two hyphens have to be used instead of only once.
For example, to pass the PETSc option -ksp_view the actual FeenoX
invocation should be

    $ feenox input.fee --ksp_view

See https://www.seamplex.com/feenox/examples for annotated examples.

Report bugs at https://github.com/seamplex/feenox/issues
Ask questions at https://github.com/seamplex/feenox/discussions
Feenox home page: https://www.seamplex.com/feenox/
>
```

It is explained there that the main input file has to be given as the first argument. So go to the tests or examples directory, find a test you like and run it:

```terminal
> cd tests
> feenox parallelepiped.fee
0.000295443
>
```

In any case, recall once again that FeenoX is a cloud-first tool, and Windows is not cloud-friendly, let alone cloud-first.
It is time to re-think what you expect from a finite-element(ish) tool.
If you still need a GUI, please check [CAEplex](https://www.caeplex.com).

Try to avoid Windows as much as you can. The binaries are provided as transitional packages for people that for some reason still use such an outdated, anachronous, awful and invasive operating system. They are compiled with [Cygwin](http://cygwin.com/) and have no support whatsoever. Really, really, **get rid of Windows ASAP**.

```include
windows.md
```
