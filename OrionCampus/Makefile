CC     = gcc
CFLAGS = -Wall -Wextra -std=c99
SRC    = main.c drone.c operador.c missao.c evento.c historico.c \
         cadastro.c operacao.c consulta.c relatorios.c
TARGET = orion

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

test: teste.c drone.c operador.c missao.c evento.c historico.c \
      cadastro.c operacao.c consulta.c relatorios.c
	$(CC) $(CFLAGS) -o teste $^
	./teste

clean:
	rm -f $(TARGET) teste
