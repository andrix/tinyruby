mipar = 20000


class Persona
	def initializer()
		puts "Initializer por defecto sin argumentos!!"
	end

	def nombre(nombre)
		puts "Entra a nombre con n=" + nombre
		nombre = "Mi nombre es " + nombre
		puts "Sale con " + nombre
		return nombre
	end
end

a=Persona.new()

puts a

puts a.nombre("Fernando")

b = "Carriquiy"

puts a.nombre(b)

puts "B: " + b


puts "THE END!"


