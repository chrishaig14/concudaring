
/* Help message */
# define HELP "El juego sólo acepta dos parametros: nro de jugadores (debe ser par) y opcionalmente 'd' como segundo argumento para ejecutar en modo INFO."

/* Error constants */
#define TOO_MANY_ARGS "Se pasaron demasiados argumentos al programa."
#define INVALID_PLAYER_AMOUNT "Se pasó una cantidad inválida de jugadores."
#define GAME_EXIT_ERROR "Ocurrió un error durante la ejecución y no se finalizó el juego correctamente."

/* INFO messages */
#define DEBUG_MODE "Se ejecutó el programa en modo DEBUG."
#define STARTING_WITH "Se dará comienzo al juego con jugadores: "
#define PLAYER_WON "Ha ganado el jugador: "
#define GAME_EXIT_OK "El juego finalizó correctamente."

/* File path for shmem & sem. */
#define KEY_PATH "/bin/bash"

#define SEM_ACCIONES 102
#define SEM_TURNO_JUGADOR 1
#define SEM_JUGADOR 103
#define SEM_TURNO_TERMINADO 101

#define NUM_CARDS 48

#define SHM_PLAYER_NUM 220
#define SHM_CARDS 103
#define SHM_WINNER SHM_CARDS - 1
#define SHM_LOG 77

/* Log file name*/
#define LOG_PATH "log.txt"

/* Separator */
#define SEPARATOR "************************************************************"
