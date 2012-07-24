mipar = 20000


class Persona
	def initializer()
		global = "Mira que es otra je je"
		$global= "Initializer por defecto sin argumentos!!"
		global = "Nada"
	end

	def guardar(n,e)
		puts "--------------guardar"
		@nom=n
		@edad=e
		puts "--------------fin de guardar"
	end

	def miEdad()
		return @edad
	end

	def miNombre()
		return "Me llamo " + @nom
	end	

	def nombre(nombre)
		puts "Entra a nombre con n=" + nombre
		nombre = "Mi nombre es " + nombre
		puts "Sale con " + nombre
		return nombre
	end

	def procesar(p)
		$global = $global + "-" +p
		global =" Ipa!"
		puts "Procesar $global= " +$global 
	end
end

a=Persona.new()


puts "MAIN: $global=" + $global

a.procesar("'String enviado desde el MAIN'")

puts "MAIN: $global =" + $global


puts "-----FIN------"


