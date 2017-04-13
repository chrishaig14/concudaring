
/* Help message */
# define HELP "El juego sólo acepta dos parametros: nro de jugadores (debe ser par) y opcionalmente 'd' como segundo argumento para ejecutar en modo INFO."

/* Error constants */
#define TOO_MANY_ARGS "Se pasaron demasiados argumentos al programa."
#define INVALID_PLAYER_AMOUNT "Se pasó una cantidad inválida de jugadores."
#define GAME_EXIT_ERROR "Ocurrió un error durante la ejecución y no se finalizó el juego correctamente."

/* INFO messages */
#define INFO_MODE "Se ejecutó el programa en modo INFO."
#define STARTING_WITH "Se dará comienzo al juego con jugadores: "
#define PLAYER_WON "Ha ganado el jugador: "
#define GAME_EXIT_OK "El juego finalizó correctamente."

/* File path for shmem */
#define SHMEM_PATH "/bin/bash"

#define SEM_JUGAR 100
#define SEM_TURNO 101

#define SHM_PLAYER_NUM 102

#define SHM_CARDS 103
#define NUM_CARDS 48