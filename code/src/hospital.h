#ifndef HOSPITAL_H
#define HOSPITAL_H

#include <deque>
#include <vector>
#include <pcosynchro/pcomutex.h>

#include "iwindowinterface.h"
#include "seller.h"

/**
 * @brief The Hospital class
 * Gère un hôpital qui reçoit des patients malades des ambulances et des patients soignés des cliniques.
 * Hérite de la classe Seller, car les patients peuvent être "échangés" comme des ressources.
 */
class Hospital : public Seller 
{
public:
    /**
     * @brief Hospital Constructor
     * @param uniqueId L'identifiant unique de l'hôpital
     * @param fund L'argent initial de l'hôpital
     * @param maxBeds Le nombre maximum de lits disponibles à l'hôpital
     */
    Hospital(int uniqueId, int fund, int maxBeds);

    /**
     * @brief run
     * La boucle principale de l'hôpital, qui gère l'import de patients des ambulances et des cliniques.
     * Cette fonction est responsable de la gestion des transferts et de la logistique des patients.
     */
    void run();

    /**
    * @brief getItemsForSale
    * @return Retourne la map des patients présents à l'hôpital, avec la clé étant le type de patient (malade ou soigné) et la valeur la quantité.
    */
    std::map<ItemType, int> getItemsForSale() override;

    /**
     * @brief send
     * Transfère des patients hors de l'hôpital, par exemple vers une autre institution.
     * @param it Le type de patients échangés (malades ou soignés)
     * @param qty La quantité de patients à transférer
     * @param bill Le coût associé à la transaction
     * @return Le coût de la transaction, ou 0 si l'échange n'est pas possible (ex. manque de lits).
     */
    int send(ItemType it, int qty, int bill) override;

    /**
     * @brief request
     * Demande une certaine quantité de patients d'un type spécifique (malades ou soignés).
     * @param what Le type de patients demandés
     * @param qty La quantité de patients demandée
     * @return Le nombre de patients reçus ou 0 si la demande n'est pas satisfaite.
     */
    int request(ItemType what, int qty) override;

    /**
     * @brief setClinics
     * @param clinics Une liste de cliniques avec lesquelles l'hôpital va interagir
     * Cette fonction configure les cliniques avec lesquelles l'hôpital va échanger des patients soignés.
     */
    void setClinics(std::vector<Seller*> clinics);

    int getNumberPatients();

    /**
     * @brief getAmountPaidToWorkers
     * @return Le montant total payé aux travailleurs de l'ambulance.
     */
    int getAmountPaidToWorkers();

    /**
     * @brief setInterface
     * @param windowInterface Pointeur vers l'interface graphique utilisée pour l'affichage des logs et mises à jour
     * Configure l'interface pour afficher les actions de l'hôpital, comme les transferts de patients.
     */
    static void setInterface(IWindowInterface* windowInterface);

private:
    /**
     * @brief transferPatientsFromClinic
     * Transfère des patients d'une clinique vers l'hôpital.
     * Cette fonction est appelée dans le cadre de l'intégration avec les cliniques et gère l'arrivée de patients soignés.
     */
    void transferPatientsFromClinic();

    /**
     * @brief buyResources
     * Fonction pour acheter des ressources, ici des patients, soit malades (d'ambulances) soit soignés (de cliniques).
     * Cette méthode est utilisée pour gérer l'approvisionnement en "ressources" humaines.
     */
    void buyResources();

    void freeHealedPatient();

    std::vector<Seller*> ambulances;  // Liste des ambulances liées à l'hôpital, qui apportent des patients malades
    std::vector<Seller*> clinics;     // Liste des cliniques liées à l'hôpital, qui renvoient des patients soignés

    int maxBeds;        // Nombre maximum de lits disponibles à l'hôpital
    int currentBeds;    // Nombre actuel de lits occupés, représente le nombre de patients présents

    int nbHospitalised; //Nombre de transfert réussi vers l'hôpital (nombre de fois ou un(e) infirmier/infirmière est payé)

    int nbFree; // Nombre de personnes qui sont sorties soignées de l'hôpital.

    std::deque<int> healedPatientsDaysLeft;  // Chaque élément représente le nombre de jours restants pour un patient guéri


    static IWindowInterface* interface;  // Pointeur statique vers l'interface utilisateur pour les logs et mises à jour visuelles
};

#endif // HOSPITAL_H
