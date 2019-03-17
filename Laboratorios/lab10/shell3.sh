#!/bin/bash

function oper_suma
{
    #1 es el primer parámetro que recibe la función, 2 es el segundo parámetro...
    let "suma=$1+$2" 
    echo "La suma es: $suma"
}
function oper_resta
{
    #1 es el primer parámetro que recibe la función, 2 es el segundo parámetro...
    let "resta=$1-$2"
    echo "La resta es: $resta"
}
function oper_mul
{
    #1 es el primer parámetro que recibe la función, 2 es el segundo parámetro...
    let "mul=$1*$2"
    echo "La multiplicación es: $mul"
}
function oper_div
{
    #1 es el primer parámetro que recibe la función, 2 es el segundo parámetro...
    let "div=$1/$2"
    echo "La división es: $div"
}

echo "1. Suma"
echo "2. Resta"
echo "3. Multiplicación"
echo "4. División"
echo "Dame una opción: "

read opcion

case $opcion in
    1)
        echo "Dame el numero 1"
        read num1
        echo "Dame el numero 2"
        read num2

        oper_suma(num1 num2)
        ;;
    2)
        echo "Dame el numero 1"
        read num1
        echo "Dame el numero 2"
        read num2

        oper_resta(num1 num2)
        ;;
    3)
        echo "Dame el numero 1"
        read num1
        echo "Dame el numero 2"
        read num2

        oper_mul(num1 num2)
        ;;
    4)
        echo "Dame el numero 1"
        read num1
        echo "Dame el numero 2"
        read num2

        oper_div(num1 num2)
        ;;
    *)
        echo "Opción incorrecta [1 | 2 | 3 | 4]"
        ;;
esac