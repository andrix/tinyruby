#	En este test vamos a probar todos los operadores
#	La idea principal es que sirva de test de regresion
#	Entonces cuando ande el operador se imprime OP OK
#	Si no anda se imprime OP ERROR se esperaba X y dio Z por ejemplo

if 1==1
	puts "(==INT) OK"
else
	puts "(==) ERROR se esperaba true y retorno "+ (1==1).to_s
end


if "hola lola" == "hola lola"
	puts "(==STRING) OK"
else
	puts "(==STRING) ERROR se esperaba true y retorno "+ ("hola lola"=="hola lola").to_s
end

if 10 % 3 == 1  
	puts "(%INT) OK"
else
	puts "(%INT) ERROR se esperaba "+1.to_s+" y dio " +(10%3).to_s
end

if 10.2 + 9.8 == 20.0
	puts "(+FLOAT) OK"
else
	puts "(+FLOAT) ERROR se esperaba 20 y retorno "+ (10.2+9.8).to_s
end 

if 1-2 == -1
	puts "(-INT) OK"
else
	puts "(-INT) ERROR se esperaba -1 y retorno "+(1-2).to_s
end

if ( 3 >= 2) 
	puts "(>=INT) OK"
else
	puts "(>=INT) ERROR se esperaba true y retorno false " 
end

if (not (10 < 5))
	puts "(not) OK"
else
	puts "(not) ERROR se esperaba true y dio false"
end

if ( 1<=2 and (1==1))
	puts "(and) OK"
else
	puts "(and) Error se esperaba true y retorno false"
end

if ( true && true)
	puts "(&&) OK"
else
	puts "(&&) ERROR se esperaba true y dio false"
end


