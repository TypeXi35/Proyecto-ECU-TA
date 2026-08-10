# Arquitectura de Control: Gateway + Control ECU

## 1. Descripción general

El sistema está compuesto por **dos sistemas que trabajan secuencialmente**:

1. **Gateway** — recibe y valida las señales provenientes de los sensores.
2. **Control ECU** — consume las señales validadas y determina el estado operativo de la máquina.

El flujo general es:

```text
Sensores
   │
   ▼
┌──────────────┐
│   Gateway    │
│              │
│ Validación   │
│ de señales   │
└──────┬───────┘
       │
       │ Señales + estado de validez
       ▼
┌──────────────┐
│  Control ECU │
│              │
│ Lógica de    │
│ estados      │
└──────┬───────┘
       │
       ▼
 Estado del sistema
```

---

# 2. Gateway

El **Gateway** es responsable de recibir los datos crudos provenientes de los sensores y determinar si las señales son plausibles.

### Responsabilidades

* Recibir los valores crudos de los sensores.
* Validar que cada señal se encuentre dentro de los límites establecidos.
* Detectar valores físicamente imposibles.
* Detectar valores negativos cuando no sean válidos para la señal.
* Detectar saltos abruptos o físicamente imposibles entre mediciones consecutivas.
* Reportar al **Control ECU** tanto el valor de la señal como su estado de validez.

### Ejemplo de validaciones

Una señal puede considerarse inválida si:

```text
Valor < límite mínimo
Valor > límite máximo
        │
        ├── Valor físicamente imposible
        │
        └── Salto entre mediciones > umbral permitido
```

El Gateway **no es responsable de decidir el estado de la máquina**. Su función principal es determinar la integridad y plausibilidad de las señales.

---

# 3. Control ECU

El **Control ECU** es responsable de la lógica funcional del sistema y del manejo de los estados de la máquina.

Recibe del Gateway:

* Valor de cada señal.
* Indicador de validez de cada señal.

A partir de esta información, el Control ECU determina el estado actual del sistema.

## 3.1 Señales críticas

El sistema cuenta con dos señales críticas:

* Temperatura.
* Voltaje.

Si **cualquiera de las dos señales críticas es inválida**, el sistema debe pasar inmediatamente a `SAFE_STATE`.

```text
Señal crítica 1 inválida
        OR
Señal crítica 2 inválida
        │
        ▼
    SAFE_STATE
```

### Condiciones críticas

Además de una señal crítica inválida, una condición puede ser considerada crítica cuando el valor de la señal supera los límites funcionales definidos para el sistema.

Por ejemplo:

```text
Temperatura > CRITICAL_TEMPERATURE
              │
              ▼
         SAFE_STATE
```

```text
Voltaje < CRITICAL_VOLTAGE
              │
              ▼
         SAFE_STATE
```

---

## 3.2 Señales normales

Las señales normales son:

* RPM.
* Velocidad.
* Acelerador.

Si **dos o más señales normales son inválidas simultáneamente**, el sistema debe pasar a `SAFE_STATE`.

```text
2 señales normales inválidas
            │
            ▼
        SAFE_STATE
```

> La clasificación exacta de las señales como **críticas** o **normales** debe quedar definida durante la configuración del sistema.

---

# 4. Máquina de estados

El sistema cuenta con **seis estados principales**:

* `INIT`
* `SELF_TEST`
* `OPERATIONAL`
* `DEGRADED`
* `SAFE_STATE`
* `SHUTDOWN`

La máquina de estados permite tanto la permanencia en un estado como la recuperación hacia un estado anterior, siempre que las condiciones del sistema lo permitan.

## 4.1 Diagrama de estados

```text
                         ┌──────────┐
                         │   INIT   │
                         └────┬─────┘
                              │
                              │ Inicialización completa
                              ▼
                     ┌────────────────┐
                     │   SELF_TEST    │
                     └───┬────┬────┬──┘
                         │    │    │
                    normal│    │    │critical
                         │    │    │
                         │    │    └────────────────────┐
                         │    │                         │
                    degraded│                         ▼
                         │    │                  ┌─────────────┐
                         │    └─────────────────►│ SAFE_STATE  │
                         │                       └──────┬──────┘
                         │                              │
                         ▼                              │
                  ┌─────────────┐                       │
                  │ OPERATIONAL │                       │
                  └───┬─────┬───┘                       │
                      │     │                           │
                 normal│     │degraded                  │
                      │     │                           │
                      │     ▼                           │
                      │ ┌─────────────┐                 │
                      │ │  DEGRADED   │◄────────────────┘
                      │ └──┬──────┬───┘
                      │    │      │
                      │    │      │critical
                      │    │      └──────────────────► SAFE_STATE
                      │    │
                      │    │normal
                      │    └─────────────────────────► OPERATIONAL
                      │
                      └──────────────────────────────► OPERATIONAL

SAFE_STATE
     │
     │ Apagado seguro completado
     ▼
┌──────────┐
│ SHUTDOWN │
└──────────┘
```

### 4.2 Transiciones permitidas

| Estado actual | Condición                           | Estado siguiente |
| ------------- | ----------------------------------- | ---------------- |
| `INIT`        | Inicialización completada           | `SELF_TEST`      |
| `SELF_TEST`   | Condiciones normales                | `OPERATIONAL`    |
| `SELF_TEST`   | Condición degradada                 | `DEGRADED`       |
| `SELF_TEST`   | Condición crítica                   | `SAFE_STATE`     |
| `OPERATIONAL` | Condiciones normales                | `OPERATIONAL`    |
| `OPERATIONAL` | Condición degradada                 | `DEGRADED`       |
| `OPERATIONAL` | Condición crítica                   | `SAFE_STATE`     |
| `DEGRADED`    | Condiciones normales                | `OPERATIONAL`    |
| `DEGRADED`    | Condición degradada                 | `DEGRADED`       |
| `DEGRADED`    | Condición crítica                   | `SAFE_STATE`     |
| `SAFE_STATE`  | Procedimiento de apagado completado | `SHUTDOWN`       |
| `SHUTDOWN`    | —                                   | Fin de ejecución |

### 4.3 Reglas generales

Las transiciones siguen estas reglas:

```text
                    Condición actual
                          │
                          ▼
                  ¿Condición crítica?
                    /            \
                  SÍ              NO
                  │                │
                  ▼                ▼
             SAFE_STATE     ¿Condición degradada?
                              /             \
                            SÍ               NO
                            │                 │
                            ▼                 ▼
                        DEGRADED         OPERATIONAL
```

Las condiciones críticas tienen **mayor prioridad** que las condiciones degradadas.

---

# 5. INIT

El estado `INIT` se ejecuta al iniciar el sistema.

### 5.1 Inicialización

Se establecen los valores iniciales de los atributos:

* Velocidad.
* RPM.
* Temperatura.
* Acelerador.
* Voltaje.

### 5.2 Configuración

También se establecen:

* Rangos de aceptación utilizados por el Gateway.
* Rangos funcionales utilizados por el Control ECU.
* Umbrales de aceptación para cambios entre mediciones.
* Clasificación de señales críticas y normales.
* Reglas asociadas a señales faltantes o que no sean recibidas.

Una vez terminada la inicialización:

```text
INIT
  │
  ▼
SELF_TEST
```

`INIT` **solamente puede transicionar a `SELF_TEST`**.

---

# 6. SELF_TEST

El estado `SELF_TEST` permite comprobar que las señales recibidas sean válidas antes de entrar en operación normal.

A diferencia de una inicialización tradicional, `SELF_TEST` **no necesariamente debe terminar en `OPERATIONAL`**.

El resultado de la evaluación puede ser:

* `OPERATIONAL`
* `DEGRADED`
* `SAFE_STATE`

## 6.1 Flujo

```text
                  SELF_TEST
                      │
                      ▼
                  Gateway
                      │
             ┌────────┴────────┐
             │                 │
       Validación          Resultado
       de señales          de validez
             │                 │
             └────────┬────────┘
                      ▼
                 Control ECU
                      │
             Evaluación de estado
                      │
          ┌───────────┼───────────┐
          │           │           │
          ▼           ▼           ▼
     OPERATIONAL   DEGRADED   SAFE_STATE
```

---

## 6.2 SELF_TEST → OPERATIONAL

Se realiza esta transición cuando:

* Las señales críticas son válidas.
* No existen dos o más señales normales inválidas.
* Temperatura no está en estado `CRITICAL`.
* Voltaje no está en estado `CRITICAL`.
* No existen inconsistencias funcionales.
* Las demás condiciones de operación son normales.

```text
SELF_TEST
    │
    │ normal
    ▼
OPERATIONAL
```

---

## 6.3 SELF_TEST → DEGRADED

Esta transición ocurre cuando el sistema puede continuar operando, pero existe una condición anormal que reduce su confiabilidad o funcionalidad.

Ejemplos:

* Temperatura en rango `DEGRADED`.
* Voltaje en rango `DEGRADED`.
* Inconsistencia entre RPM y velocidad.
* Inconsistencia entre RPM y acelerador.

```text
SELF_TEST
    │
    │ degraded
    ▼
DEGRADED
```

---

## 6.4 SELF_TEST → SAFE_STATE

Esta transición ocurre cuando durante el `SELF_TEST` se detecta una condición que hace inseguro continuar con la operación.

Ejemplos:

* Una señal crítica es inválida.
* Dos o más señales normales son inválidas.
* Temperatura está en estado `CRITICAL`.
* Voltaje está en estado `CRITICAL`.
* Se detecta cualquier otra condición definida como insegura.

```text
SELF_TEST
    │
    │ critical
    ▼
SAFE_STATE
```

### Prioridad de evaluación

Las condiciones críticas deben evaluarse antes que las degradadas:

```text
SELF_TEST
    │
    ▼
¿Condición crítica?
    │
   SÍ ──────────────► SAFE_STATE
    │
   NO
    ▼
¿Condición degradada?
    │
   SÍ ──────────────► DEGRADED
    │
   NO
    ▼
OPERATIONAL
```

---

# 7. OPERATIONAL

El estado `OPERATIONAL` representa el funcionamiento normal del sistema.

El ciclo de procesamiento continúa de forma periódica:

```text
Sensores
   │
   ▼
Gateway
   │
   ├── Evalúa señales
   │
   ▼
Control ECU
   │
   └── Determina estado
```

## 7.1 OPERATIONAL → OPERATIONAL

Si todas las condiciones continúan siendo normales:

```text
        ┌─────────────────┐
        │                 │
        │     normal      │
        │                 ▼
   OPERATIONAL ───────────┘
```

El sistema permanece en `OPERATIONAL`.

---

## 7.2 OPERATIONAL → DEGRADED

Si se detecta una condición degradada:

```text
OPERATIONAL
     │
     │ degraded
     ▼
DEGRADED
```

---

## 7.3 OPERATIONAL → SAFE_STATE

Si se detecta una condición crítica:

```text
OPERATIONAL
     │
     │ critical
     ▼
SAFE_STATE
```

Una condición crítica siempre tiene prioridad sobre permanecer en `OPERATIONAL` o pasar a `DEGRADED`.

---

# 8. DEGRADED

El estado `DEGRADED` representa una condición en la que el sistema todavía puede operar, pero existe una condición anormal o una pérdida parcial de información o funcionalidad.

El sistema continúa recibiendo y evaluando señales.

```text
Gateway
   │
   ├── Evalúa señales
   │
   ▼
Control ECU
   │
   └── Evalúa condición
```

## 8.1 DEGRADED → OPERATIONAL

Si las condiciones vuelven a ser normales:

```text
DEGRADED
    │
    │ normal
    ▼
OPERATIONAL
```

---

## 8.2 DEGRADED → DEGRADED

Si la condición degradada persiste:

```text
        ┌─────────────────┐
        │                 │
        │    degraded     │
        │                 ▼
     DEGRADED ────────────┘
```

El sistema permanece en `DEGRADED`.

---

## 8.3 DEGRADED → SAFE_STATE

Si la condición empeora y alcanza un estado crítico:

```text
DEGRADED
    │
    │ critical
    ▼
SAFE_STATE
```

Ejemplos:

* Una señal crítica se vuelve inválida.
* Dos señales normales se vuelven inválidas.
* Una condición funcional pasa a `CRITICAL`.
* Se detecta una condición físicamente insegura.

---

# 9. SAFE_STATE

`SAFE_STATE` representa una condición en la que continuar operando el vehículo ya no es seguro.

Una vez alcanzado este estado, **no existe recuperación hacia `OPERATIONAL` o `DEGRADED`**.

La única transición permitida es:

```text
SAFE_STATE
    │
    ▼
SHUTDOWN
```

## 9.1 Acciones

Al entrar en `SAFE_STATE`:

1. Avisar al usuario que el vehículo será apagado.
2. Mandar las señales controladas a `0`.
3. Ejecutar las acciones necesarias para llevar el sistema a una condición segura.
4. Preparar el apagado.
5. Transicionar a `SHUTDOWN`.

```text
SAFE_STATE
    │
    ├── Avisar al usuario
    │
    ├── Señales → 0
    │
    ├── Llevar sistema a condición segura
    │
    └── Preparar apagado
             │
             ▼
         SHUTDOWN
```

---

# 10. SHUTDOWN

`SHUTDOWN` representa el estado final del sistema.

### Acciones

* Informar al usuario que el sistema se está apagando.
* Completar las acciones necesarias para el apagado.
* Detener la ejecución del programa.

```text
SHUTDOWN
   │
   └── Fin de ejecución
```

No existen transiciones desde `SHUTDOWN` hacia ningún otro estado.

---

# 11. Reglas de seguridad

Las reglas de seguridad deben tener prioridad sobre las transiciones normales de la máquina de estados.

## 11.1 Señales críticas

Si cualquiera de las señales críticas es inválida:

```text
Critical_1 == INVALID
        OR
Critical_2 == INVALID
        │
        ▼
   SAFE_STATE
```

## 11.2 Señales normales

Si dos o más señales normales son inválidas:

```text
Invalid_Normal_Signals >= 2
              │
              ▼
         SAFE_STATE
```

## 11.3 Prioridad general

La evaluación de condiciones debe seguir conceptualmente este orden:

```text
1. ¿Existe una condición crítica?
             │
            SÍ
             │
             ▼
        SAFE_STATE
             │
            NO
             ▼
2. ¿Existe una condición degradada?
             │
            SÍ
             │
             ▼
         DEGRADED
             │
            NO
             ▼
        OPERATIONAL
```

Esto garantiza que una condición crítica **nunca sea degradada por error a `DEGRADED`**.

---

# 12. Señales no recibidas

Se deben establecer casos especiales para situaciones en las que una señal esperada no sea recibida.

Una señal ausente no debe tratarse automáticamente como un valor válido.

Se debe distinguir entre:

```text
┌────────────────────────────┐
│ Estado de una señal        │
├────────────────────────────┤
│ VÁLIDA                     │
│ INVÁLIDA                   │
│ AUSENTE / TIMEOUT          │
└────────────────────────────┘
```

Para cada señal se deberá definir:

* Tiempo máximo permitido sin recibir una actualización.
* Si una ausencia temporal puede tolerarse.
* Cuánto tiempo puede permanecer el sistema en `DEGRADED`.
* Cuándo una señal ausente debe considerarse `INVALID`.
* Qué sucede si una señal crítica deja de recibirse.
* Qué sucede si una o varias señales normales dejan de recibirse.

### Ejemplo

```text
No se recibe señal
       │
       ▼
¿Timeout excedido?
   │          │
  NO         SÍ
   │          │
   ▼          ▼
Continuar   INVALID
            │
            ▼
       Evaluar reglas
       de seguridad
```

---

# 13. Ciclo de procesamiento

Durante `SELF_TEST`, `OPERATIONAL` y `DEGRADED`, el procesamiento de señales sigue el mismo principio general:

```text
┌─────────────────────────────────────┐
│                                     │
│  Sensores                            │
│     │                                │
│     ▼                                │
│  Gateway                             │
│     │                                │
│     ├── Validación                   │
│     ├── Rangos                       │
│     ├── Plausibilidad                │
│     └── Saltos entre mediciones      │
│     │                                │
│     ▼                                │
│  Control ECU                         │
│     │                                │
│     ├── Evaluar validez              │
│     ├── Evaluar criticidad           │
│     ├── Evaluar funcionalidad        │
│     └── Determinar siguiente estado  │
│                                     │
└─────────────────────────────────────┘
```

La diferencia principal entre los estados es **qué transiciones están permitidas como resultado de esa evaluación**.

---

# 14. Resumen de la máquina de estados

```text
INIT
 │
 │ inicialización completa
 ▼
SELF_TEST
 │
 ├──────── normal ──────────────► OPERATIONAL
 │
 ├──────── degraded ───────────► DEGRADED
 │
 └──────── critical ───────────► SAFE_STATE


OPERATIONAL
 │
 ├──────── normal ──────────────► OPERATIONAL
 │
 ├──────── degraded ───────────► DEGRADED
 │
 └──────── critical ───────────► SAFE_STATE


DEGRADED
 │
 ├──────── normal ──────────────► OPERATIONAL
 │
 ├──────── degraded ───────────► DEGRADED
 │
 └──────── critical ───────────► SAFE_STATE


SAFE_STATE
 │
 │ apagado seguro completado
 ▼
SHUTDOWN
 │
 └──────────────► Fin de ejecución
```

## Tabla final de transiciones

| Desde         | Normal        | Degraded   | Critical     |
| ------------- | ------------- | ---------- | ------------ |
| `INIT`        | `SELF_TEST`   | —          | —            |
| `SELF_TEST`   | `OPERATIONAL` | `DEGRADED` | `SAFE_STATE` |
| `OPERATIONAL` | `OPERATIONAL` | `DEGRADED` | `SAFE_STATE` |
| `DEGRADED`    | `OPERATIONAL` | `DEGRADED` | `SAFE_STATE` |
| `SAFE_STATE`  | —             | —          | `SHUTDOWN`   |
| `SHUTDOWN`    | —             | —          | —            |

> **Regla fundamental:** `SAFE_STATE` es unidireccional. Una vez que el sistema entra en `SAFE_STATE`, no puede regresar a `OPERATIONAL` ni a `DEGRADED`; únicamente puede avanzar hacia `SHUTDOWN`.

---

# 15. Principio fundamental

> **El Gateway determina si las señales son plausibles; el Control ECU determina qué significa esa información para el estado de la máquina.**

Esto mantiene separadas las responsabilidades de **validación de datos** y **lógica funcional/estado del sistema**.
