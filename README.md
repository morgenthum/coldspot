# coldspot

A java virtual machine written from scratch

## Description

I wrote this JVM just for fun. It works - I had a webserver up running on it.

### Features

- Simple switch-case interpreter
- Mark and sweep garbage collection algorithm

### OpenJDK integration

It's painful to bind a nacked JVM to the OpenJDK standard library, because the OpenJDK is quite HotSpot VM specific.

I had to 
- load some core classes by hand before loading the JDK (see [src/jvm/VirtualMachine.cpp](src/jvm/VirtualMachine.cpp))
- implement [sun.misc.Unsafe](https://dzone.com/articles/understanding-sunmiscunsafe) by hand

## State

Inactive
