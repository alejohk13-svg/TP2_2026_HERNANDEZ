Se cambio la configuracion del pin del LED a modo Open Drain.   
Al probar el circuito, el LED dejo de encender cuando se presionan las teclas, a diferencia de antes que funcionaba normalmente.
Esto pasa porque en este modo el pin ya no puede generar un nivel alto, solo puede llevar la señal a 0. Como el circuito no tiene una forma de “subir” la señal a 1, el LED nunca recibe la tension necesaria para prender.
En resumen, el LED no funciona porque el modo Open Drain necesita una conexión extra (pull-up) que en este caso no esta presente.

