
#!/bin/bash

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

        let "suma=$num1+$num2"

        echo "La suma es: $suma"
        ;;
    2)
        echo "Dame el numero 1"
        read num1
        echo "Dame el numero 2"
        read num2

        let "resta=$num1-$num2"

        echo "La resta es: $resta"
        ;;
    3)
        echo "Dame el numero 1"
        read num1
        echo "Dame el numero 2"
        read num2

        let "mul=$num1*$num2"

        echo "La multiplicación es: $mul"
        ;;
    4)
        echo "Dame el numero 1"
        read num1
        echo "Dame el numero 2"
        read num2

        let "div=$num1/$num2"

        echo "La división es: $div"
        ;;
    *)
        echo "Opción incorrecta [1 | 2 | 3 | 4]"
        ;;
esac