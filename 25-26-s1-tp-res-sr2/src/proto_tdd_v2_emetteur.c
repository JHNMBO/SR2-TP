/*************************************************************
* proto_tdd_v2 - émetteur                                    *
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
/* Programme principal - émetteur  */
/* =============================== */
int main(int argc, char* argv[])
{
    unsigned char message[MAX_INFO]; /* message de l'application */
    int taille_msg; /* taille du message */
    int prochain_paquet = 0;
    paquet_t paquet, pack; /* paquet utilisé par le protocole */

    init_reseau(EMISSION);

    printf("[TRP] Initialisation reseau : OK.\n");
    printf("[TRP] Debut execution protocole transport.\n");

    /* lecture de donnees provenant de la couche application */
    de_application(message, &taille_msg);

    /* tant que l'émetteur a des données à envoyer */
    while ( taille_msg != 0 ) {

        /* construction paquet */
        for (int i=0; i<taille_msg; i++) {
            paquet.info[i] = message[i];
        }
        paquet.lg_info = taille_msg;
        paquet.type = DATA;
        paquet.somme_ctrl = generer_controle(&paquet);
        paquet.num_seq = prochain_paquet;

        /* remise à la couche reseau */
        vers_reseau(&paquet);
        depart_temporisateur_num(prochain_paquet, 20);
        int evt = attendre();
        
        /* retransmission paquet*/
        while(evt == prochain_paquet){ /* timeout */
            vers_reseau(&paquet);
            depart_temporisateur_num(prochain_paquet, 20);
            evt = attendre();
        }

        de_reseau(&pack);
        arret_temporisateur_num(prochain_paquet);
        prochain_paquet++;

        /* lecture des donnees suivantes de la couche application */
        de_application(message, &taille_msg);
    }

    printf("[TRP] Fin execution protocole transfert de donnees (TDD).\n");
    return 0;
}
