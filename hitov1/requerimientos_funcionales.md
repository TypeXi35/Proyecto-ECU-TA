# Requerimientos Funcionales

1. El Sistema debera contar 6 estados INIT, SELF_TEST, OPERATIONAL, DEGRADED, SAFE_STATE Y SHUTDOWN.

2. El Sistema debera inicializar sus variables dentro del estado INIT.

3. El Sistema debera hacer un test preliminar con los valores de arranque en el estado de SELF_TEST.

4. El Sistema deberá mantenerse en estado operacional al no encontrar ningún problema.

5. El Sistema deberá definir los rangos válidos de los valores de los atributos.

6. El Sistema deberá definir los rangos válidos de los saltos de valores entre cada lectura del sensor. El Acelerador queda exento de esta validación de salto, ya que por diseño puede pasar de 0 a 100 casi instantáneamente.

7. El sistema se deberá de mover a Degradado cuando encuentre un estado inconsistente o una señal (básica) no llegue 3 veces.

8. El Sistema se deberá mover a SAFE_STATE cuando la ECU Gateway no logre validar uno de los atributos críticos (Voltaje y Temperatura en nuestro caso). Basta con una sola lectura inválida de un atributo crítico para mover el sistema a SAFE_STATE de inmediato, sin usar el contador de repeticiones que sí aplica a las señales básicas/normales.

9. El Sistema se deberá mover a SAFE_STATE cuando no encuentre una señal (básica) 5 veces.

10. El Sistema se deberá mover a SAFE_STATE cuando dos o más señales normales (RPM, Velocidad, Acelerador) sean inválidas de manera simultánea, independientemente del conteo de repeticiones de cada señal por separado.

11. El Sistema deberá moverse a SAFE_STATE cuando existan tanto una condición que lo movería a SAFE_STATE como a DEGRADED, siempre deberá moverse a SAFE_STATE de manera prioritaria.

12. El Sistema deberá evaluar que todas las entradas de datos sean del tipo que les corresponde, en caso contrario el sistema se mandará a SAFE_STATE.

13. El Sistema deberá recuperarse de DEGRADED a OPERATIONAL cuando ya no exista ninguna condición crítica ni degradada activa.

14. El Sistema no deberá permitir ninguna recuperación desde SAFE_STATE hacia OPERATIONAL o DEGRADED; una vez en SAFE_STATE, el sistema no regresa a un estado anterior.

15. El Sistema deberá moverse de manera automática e incondicional a SHUTDOWN al completar las acciones correspondientes al estado SAFE_STATE.
