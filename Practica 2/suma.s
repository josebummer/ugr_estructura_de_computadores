#suma sin signo

.section .data
lista:		.int 5,5,5,5,5,5,5,5
longlista:	.int (.-lista)/4
resultado_pmes:	.int -1
resultado_pms: .int -1
formato: .ascii "Resultado: hex:%llx = d:%llu\n\0"      #mostramos un mensaje por pantalla para comprobar el resultado

.section .text
main:	.global main																#cambiamos _start por main para poder usar printf

	mov    $lista, %ebx															#movemos la direccion de lista al registro ebx
	mov longlista, %ecx															#movemos el contenido de longlista al registro ecx
	call suma																				#llamamos a suma
	mov %eax, resultado_pmes												#movemos el contenido del registro eax a resultado_pmes (parte menos significativa)
	mov %edx , resultado_pms												#movemos el contenido del registro edx a resultado_pms (parte mas significativa)

	push resultado_pms															#guardamos los valores para enviar a printf
	push resultado_pmes
	push resultado_pms
	push resultado_pmes
	push $formato

	call printf																			#llamamos a printf para mostrar el mensaje

	mov $1, %eax
	mov $0, %ebx
	int $0x80																				#llamamos a exit para cerrar el programa

suma:
	push %esi																				#guardamos el contenido del registro esi en la pila para no perderlo
	mov $0, %eax 																		#ponemos a 0 el acumulador
	mov $0, %esi 																		#ponemos a 0 el indice
	mov $0, %edx 																		#ponemos a 0 el acarreo
bucle:
	add (%ebx,%esi,4), %eax													#sumamos el elemento correspondiente de la lista al acumulador
	jnc cont 																				#no incrementamos si no hay acarreo
	inc %edx
cont:
	inc  %esi																				#incrementamos el indice
	cmp  %esi,%ecx																	#comparamos si el indice es igual al numero de elementos de la lista
	jne bucle																				#si no son iguales volvemos a realizar el bucle

	pop %esi																				#en el caso de que si sean iguales, volvemos a meter el contenido del registro esi
	ret																							#volvemos a la llamada a suma
