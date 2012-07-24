mipar = 20000


class Persona
	def initializer()
		puts "Initializer por defecto sin argumentos!!"
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

	def procrear()
		puts "-------------Entra a procrear"
		h= Persona.new()
		puts "--------------Creo el objeto h"
		h.guardar("Hijo de ", 5 )
		puts "--------------llamo a guardar"
		return h
	end
end

a=Persona.new()

puts a.to_s
puts "--------------IM a.to_s ------"
b= 1
puts "B vale " + b.to_s
puts "-----------Se imprimio b.to_s ----"


