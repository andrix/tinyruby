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

puts a

puts a.nombre("Fernando")

b = "Carriquiy"

puts a.nombre(b)

#puts "B: " + b

#a.guardar("Fernando Carriquiry",29)

#puts a.miNombre()

#puts "Mi edad->"

#puts a.miEdad()

#puts "THE END!"

#hi=a.procrear() 

#puts hi

#puts hi.miNombre()

#puts hi.miEdad()






