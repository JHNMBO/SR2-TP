/*************************************************************
* proto_tdd_v2 -  récepteur                                  *
* TRANSFERT DE DONNEES  v2                                   *
*                                                            *
* Protocole stop and wait ARQ                                *
*                                                            *
* Johan NZENGUE MOMBO                                        *
**************************************************************/

#include <stdio.h>
#include "application.h"
#include "couche_transport.h"
#include "services_reseau.h"

/* =============================== */
/* Programme principal - récepteur */
/* =============================== */
int main(int argc, char* argv[])
{
    unsigned char message[MAX_INFO]; /* message pour l'application */
    paquet_t paquet, pack; /* paquet utilisé par le protocole */
    int fin = 0; /* condition d'arrêt */
    int paquet_attendu = 0;

    init_reseau(RECEPTION);

    printf("[TRP] Initialisation reseau : OK.\n");
    printf("[TRP] Debut execution protocole transport.\n");

    /* tant que le récepteur reçoit des données */
    while ( !fin ) {

        // attendre(); /* optionnel ici car de_reseau() fct bloquante */
        de_reseau(&paquet);

        // test du controle
        if (verifier_controle(&paquet)) {
            printf("[TRP] CTRL Valide.\n");
            if(paquet.num_seq == paquet_attendu) {
                /* extraction des donnees du paquet recu */
                for (int i=0; i<paquet.lg_info; i++) {
                    message[i] = paquet.info[i];
                }
                /* remise des données à la couche application */
                fin = vers_application(message, paquet.lg_info);
                paquet_attendu++;
            }
        }
        pack.type = ACK;
        pack.lg_info = 0;
        vers_reseau(&pack);
    }

    printf("[TRP] Fin execution protocole transport.\n");
    return 0;
}
