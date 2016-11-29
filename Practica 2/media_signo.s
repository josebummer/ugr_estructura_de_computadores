#suma con signo

.section .data
lista:		.int 9,8,8,8
longlista:	.int (.-lista)/4
cociente:	.int -1
resto: .int -1
formato: .ascii "Resultado: cociente:%ld ; resto:%ld\n\0"      #mostramos un mensaje por pantalla para comprobar el resultado

.section .text
main:	.global main																#cambiamos _start por main para poder usar printf

	mov    $lista, %ebx															#movemos la direccion de lista al registro ebx
	mov longlista, %ecx															#movemos el contenido de longlista al registro ecx
	call suma																				#llamamos a suma
	mov %eax, cociente															#movemos el contenido del registro eax a resultado_pmes (parte menos significativa)
	mov %edx , resto																#movemos el contenido del registro edx a resultado_pms (parte mas significativa)

	push resto																			#mandamos las variables al printf
	push cociente
	push $formato

	call printf																			#llamamos a printf para mostrar el mensaje

	mov $1, %eax
	mov $0, %ebx
	int $0x80																				#llamamos a exit para cerrar el programa

suma:
	push %edi																				#guardamos el contenido del registro esi en la pila para no perderlo
	push %ebp																				#guardamos el contenido del registro esi en la pila para no perderlo
	push %esi																				#guardamos el contenido del registro esi en la pila para no perderlo
	mov $0, %eax 																		#este registro cogera cada elemento de la lista aunque finalmente sera el acumulador
	mov $0 , %ebp																		#este registro sera el acumulador
	mov $0, %esi 																		#ponemos a 0 el indice
	mov $0, %edx 																		#ponemos a 0 el registro de la parte mas significativa
	mov $0 , %edi																		#este registro llevara la suma de de la parte mas significativa
bucle:
 	mov (%ebx,%esi,4), %eax													#movemos el elemento de la lista al registro
	cdq																							#extendemos el sigo de eax para dejarlo como edx:eax
	add %eax , %ebp																	#sumamos el elemento de la lista con el acumulador
	adc %edx , %edi																	#sumamos la parte mas significativa total, con la del elemento actual y el acarreo
	inc  %esi																				#incrementamos el indice
	cmp  %esi,%ecx																	#comparamos si el indice es igual al numero de elementos de la lista
	jne bucle																				#si no son iguales volvemos a realizar el bucle

	mov %ebp , %eax																	#movemos la parte menos significativa final al registro eax
	mov %edi , %edx																	#movemos la parte mas significativa final al registro edx
	mov longlista , %ebp														#movemos el divisor a un registro
	idiv %ebp																				#realizamos la division

	pop %esi																				#metemos el contenido anterior del registro en el registro
	pop %edi																				#metemos el contenido anterior del registro en el registro
	pop %ebp																				#metemos el contenido anterior del registro en el registro
	ret																							#volvemos a la llamada a suma
