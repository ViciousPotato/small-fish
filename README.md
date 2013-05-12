small-fish
==========

Toy shell implementation for exercise.

How to Use
=====

```
$ git clone https://github.com/ViciousPotato/small-fish.git
$ cd small-fish
$ make
$ ./fish
```

Currently support command execution, output/input redirect, and piping.

examples:

```
ls
ls | wc -l | wc -l
ls >> a
echo < a
ls > a
```

Also some useless builtins :P


TODO
=====

Add more tests.
