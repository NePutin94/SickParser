# SickParser

Простая тулза, которая спасет меня от перекомпиляции файлов при изменении параметров(position, size etc.) у элементов интерфейса, например [тут](https://github.com/NePutin94/sfml_engine/blob/0978a1dffef9b232f3e48c9fd4bff307fb2ee455/SFMLEngine/SFMLEngine/MainMenu.cpp#L56)

## Синтаксис
[gui element type]: item parameter name{parameter value}, ... ;

* ***Gui element type***:
1. button
2. textbox
3. image
4. group


* ***Item parameter name***:
1. position
2. size
3. array
4. name
5. text
6. path


* ***Parametr value***:
1. int: {5,5}
2. string: {"string"}