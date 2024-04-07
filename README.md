# ¿Qué es `hyde2`?

`hyde` es una utilidad que facilita la documentación de C++. La herramienta se diferencia de las utilidades de documentación existentes en los siguientes aspectos:

- **Basado en Clang**: para documentar adecuadamente su C++, `hyde` lo compila utilizando la excelente biblioteca [libTooling](https://clang.llvm.org/docs/LibTooling.html) de Clang. Por lo tanto, a medida que el lenguaje C++ evolucione, también lo hará `hyde`.
- **Fuera de línea**: muchas herramientas se basan en documentación colocada en línea dentro del código fuente como comentarios largos. Si bien parecen atractivos a primera vista, adolecen de dos grandes inconvenientes. En primer lugar, no hay nada que impida que los comentarios no estén sincronizados con los elementos que documentan. En segundo lugar (e irónicamente), los usuarios experimentados de estas bibliotecas eventualmente encuentran que la documentación en línea es más una distracción que una ayuda, saturando el código con comentarios que ya no leen.
- **Compatible con Jekyll**: `hyde` no produce una impresión bonita. Más bien, produce archivos Markdown bien estructurados que contienen información preliminar YAML. Estos archivos luego pueden ser consumidos por otras herramientas (como Jekyll) para personalizar la estructura y el diseño de la documentación final.
- **Aplicación del esquema**: debido a la naturaleza altamente estructurada de la salida, `hyde` puede comparar archivos de documentación preexistentes con el estado actual de sus fuentes C++. Los desarrolladores de bibliotecas pueden utilizar el modo _update_ de `hyde` para facilitar la actualización de la documentación con respecto al estado de las fuentes. Los ingenieros de compilación pueden usar el modo _validar_ de `hyde` para asegurarse de que los cambios en una base de código se reflejen con precisión en la documentación más reciente. Al final, la documentación se mantiene fiel al código con el mínimo esfuerzo.
- **Adaptable**: si bien el propósito principal de `hyde` en este punto es generar y hacer cumplir la documentación, la herramienta también se puede utilizar para generar información basada en AST sobre su código como un IR basado en JSON. Esto deja espacio para que se construyan herramientas adicionales sobre lo que "hyde" es capaz de producir, o se pueden agregar emisores adicionales de forma nativa a la herramienta.

# Salida de ejemplo

`hyde` produce archivos de documentación intermedios que luego el desarrollador completa con detalles adicionales según sea necesario. Luego, los archivos se alimentan a través de una herramienta de generación de sitios estáticos (como Jekyll) para producir [un resultado como este] (http://stlab.cc/libraries/stlab2Fcopy_on_write.hpp/copy_on_write3CT3E/).

# Requisitos

## Mac OS

- cerveza casera
     - `brew instalar cmake`
     - `brew install ninja` (opcional)

##Linux

(Nota: hasta ahora solo probado en ubuntu bionic)

- Apto
     - `sudo apt-get install libyaml-cpp-dev`

# Cómo construir

- clonar este repositorio
-`cd hyde`
- `actualización del submódulo git --init`
- `compilación mkdir`
- `compilación de CD`
- `cmake.. -GNinja` (o `-GXcode`, etc.)
- `ninja` (o lo que sea que haga tu IDE)

LLVM/Clang se declaran como una dependencia en el archivo `CMakeLists.txt` del proyecto y se descargarán y estarán disponibles para el proyecto automáticamente.

# Usando ventana acoplable

Es posible que necesite aumentar los recursos de la ventana acoplable para crear la imagen.

```sh
compilación acoplable --tag hyde.

ejecución de la ventana acoplable --platform linux/x86_64 --mount type=bind,source="$(pwd)",target=/mnt/host \
     --tty --interactivo \
     golpe de hyde
```

# Parámetros y banderas

Hay varios modos bajo los cuales se puede ejecutar la herramienta:

- `-hyde-json` - (predeterminado) Genera un volcado de análisis del archivo de entrada como JSON
- `-hyde-validate` - Valida la documentación YAML existente
- `-hyde-update` - Escribe documentación YAML actualizada

- `-hyde-src-root = <ruta>`: la ruta raíz al archivo de encabezado que se está analizando. Afecta los valores de salida de `defined_in_file` al eliminar la ruta raíz.
- `-hyde-yaml-dir = <ruta>` - Directorio raíz para la validación/actualización de YAML. Requerido para los modos hyde-validate o hyde-update.

- `--use-system-clang` - Detecta automáticamente y utiliza los directorios de recursos necesarios e incluye rutas

- `--fixup-hyde-subfield`: a partir de Hyde v0.1.5, todos los campos hyde están bajo un subcampo `hyde` de nivel superior en la salida YAML. Esta bandera actualizará la documentación anterior de Hyde que no tiene este subcampo creándolo y luego moviendo todos los campos de nivel superior excepto "título" y "diseño" debajo de él. Esta bandera está destinada a usarse solo una vez durante la migración de documentación anterior de la estructura que no es de subcampo a la estructura de subcampo.

Esta herramienta analiza el encabezado pasado usando Clang. Para pasar argumentos al compilador (por ejemplo, incluir directorios), agréguelos después del token `--` en la línea de comando. Por ejemplo:

     hyde input_file.hpp -hyde-json -use-system-clang -- -x c++ -I/ruta/a/incluye

Alternativamente, si tiene una base de datos de compilación y desea pasarla en lugar de los argumentos del compilador de la línea de comandos, puede pasarla con `-p`.

Mientras se compila el archivo fuente, la macro sin función `ADOBE_TOOL_HYDE` se define con el valor `1`. Esto puede resultar útil para omitir explícitamente código de la documentación.

# Ejemplos:

Para generar JSON:
```./hyde -use-system-clang ../test_files/classes.cpp --```

Para validar YAML preexistente:
```./hyde -use-system-clang -hyde-yaml-dir=/path/to/output -hyde-validate ../test_files/classes.cpp```
