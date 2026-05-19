GCC     = gcc
CFLAGS  = -lm -O3 -Wall
POSIX   = -lpthread -lrt
BIN     = bin
ACT1    = Actividad_1
ACT2    = Actividad_2

PROGRAMAS = producer consumer posixSincro concurrenciaPosix

All: $(BIN) $(PROGRAMAS)

# Crea el directorio bin si no existe
$(BIN):
	mkdir -p $(BIN)

# ─── Actividad 1: Productor/Consumidor con procesos y semáforos POSIX ──────────
producer:
	$(GCC) $(CFLAGS) $(ACT1)/$@.c -o $(BIN)/$@ $(POSIX)

consumer:
	$(GCC) $(CFLAGS) $(ACT1)/$@.c -o $(BIN)/$@ $(POSIX)

# ─── Actividad 2: Productor/Consumidor con hilos, mutex y cond. variables ──────
posixSincro:
	$(GCC) $(CFLAGS) $(ACT2)/$@.c -o $(BIN)/$@ $(POSIX)

# ─── Bonus: Búsqueda del máximo con hilos concurrentes ──────────────────────────
concurrenciaPosix:
	$(GCC) $(CFLAGS) $@.c -o $(BIN)/$@ $(POSIX)

clean:
	$(RM) $(BIN)/*
