#!/bin/bash

echo "Hola mundo..."
echo "La suma de 6+8 = $[6+8]"
echo "Otra forma de sumar de 6+8=$((6+8))"

#Declarando variables
var1=7
var2=4
echo "La variable 1 es $var1"
echo "La variable 2 es $var2"

#Concaternar cadenas
suma=$var1+$var2
echo "La suma es: $suma"

#Para que sea una expresión numérica (Formas diferentes de sumar)
let suma1=$var1+$var2
echo "La suma 1 es: $suma1"

suma2=[$var1+$var2]
echo "La suma 2 es: $suma2"

suma3=(($var1+$var2))
echo "La suma 3 es: $suma3"

echo "Dame el numero1"
read num1

echo "Dame el numero2"
read num2

suma4=[$num1+$num2]
echo "La suma 4 es: $suma4"