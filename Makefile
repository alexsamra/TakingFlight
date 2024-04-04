all: Omni planes

Omni: Omni.c
	gcc -o Omni Omni.c -I.

planes: planes.c
	gcc -o planes planes.c -I.

clean:
	rm Omni planes
