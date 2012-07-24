mipar = 20000


class Persona

	def initializer()
		puts "Se crea una clase"
	end

	def guardar(nombre)
		@nom= nombre
		return " TEST : "+@nom
	end

	def hijo()
		h = Persona.new()
		h.guardar("Soy hijo!!")	
		return Persona.new()
	end

	def darNombre()
		return @nom
	end
end

a=Persona.new()

puts a.guardar("Fernando")

puts "THE END!"


