class Persona
	def nombre(nombre)
		puts "Entra a nombre con n=" + nombre
		nombre = "Mi nombre es " + nombre
		puts "Sale con " + nombre
		return nombre
	end
end

a = Persona.new()

puts a

puts a.nombre("Fernando")

b = "Carriquiy"

puts a.nombre(b)


